/*
 * utils.cpp
 *
 *  Created on: 27-Mar-2014
 *      Author: ubuntu
 */

#include <math.h>

#include "utils.h"


/**********************************************************
 * Function: filterNoise
 * Eliminates noise present in the source image.
 * Applies threshold, and performs morphological
 * open and close operations to eliminate noise.
 *
 * Parameters:
 *  src: Source image
 *  dst: Destination image
 *
 **********************************************************/
void filterNoise( Mat src, Mat dst )
{
    /* take threshold to highlight large differences
     */
    threshold(
      src,
      dst,
      128,
      255,
      THRESH_BINARY
     );

    /* open and close
     * operation open to shrink areas of small noise to 0
     * followed by the morphological operation close to
     * rebuild the area of surviving components that was
     * lost in opening
     */
    Mat close_element = getStructuringElement(
                 MORPH_ELLIPSE,      //element shape
                 Size(10, 30),         //size of structuring element
                 Point(0, 0)         //anchor point
                );

    Mat open_element = getStructuringElement(
                 //MORPH_RECT,         //element shape
    		     MORPH_ELLIPSE,
                 Size(2, 2),         //size of structuring element
                 Point(-1, -1)         //anchor point
                );

    morphologyEx(
       dst,        //source image
       dst,        //destination image
       CV_MOP_OPEN,    //type of operation
       open_element    //structuring element
    );
    morphologyEx(
       dst,        //source image
       dst,        //destination image
       CV_MOP_CLOSE,   //type of operation
       close_element   //structuring element
    );


   // erode(dst, dst, close_element);
}


/**********************************************************
 * Function: sumOfMatrix
 * Calculates the sum of all elements of a matrix
 *
 * Parameters:
 *   matrix: the matrix
 *
 * Returns:
 *   sum of all the elements of matrix
 ***********************************************************/
/*double sumOfMatrix(Mat matrix)
{
    double sum = 0.0;
    for(int i=0; i<matrix.rows; i++)
    {
        double row_sum = 0.0;
        for(int j=0; j<matrix.cols; j++)
        {
            row_sum = row_sum + matrix.at<int>(i,j);
        }
        sum = sum + row_sum;
    }
    return sum;
}
*/

/**********************************************************
 * Function: eulicdeanDist
 * Finds the Euclidean distance between two points
 *
 * Parameters:
 *   p1: 1st point
 *   p2: 2nd point
 *
 * Returns:
 *   The Euclidean distance
 ***********************************************************/
double eulicdeanDist(Point2f p1, Point2f p2)
{
    double euc_dist = sqrt( pow( (p1.x - p2.x), 2 )
                          + pow( (p1.y - p2.y), 2 )
                      );
    return euc_dist;
}


/**********************************************************
 * Function: calcColorHist
 * Calculates color histogram (BGR) of an image
 *
 * Parameters:
 *   src:    source image
 *   b_hist: destination blue histogram
 *   g_hist: destination green histogram
 *   r_hist: destination red histogram
 *
 * Returns:
 *   1 for success
 *   -1 for failure
 ***********************************************************/
int calcColorHist(Mat src, Mat& b_hist, Mat& g_hist, Mat& r_hist)
{
    /* Mat vector to store BGR values of source image */
    vector<Mat> bgr_planes;

    /* Establish the number of bins */
    int hist_size = HIST_BINS;

    /* Set the ranges ( for B,G,R) ) */
    float range[] = { 0, HIST_BINS } ;
    const float* hist_range = { range };

    bool uniform = true;
    bool accumulate = false;

    /* Separate the image in 3 places ( B, G and R )
     */
    if( src.channels() != 3 )
    {
      printf("Source image channels != 3\n");
      return -1;
    }
    split( src, bgr_planes );

    /* Compute the histograms
     */
    calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &hist_size, &hist_range, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &hist_size, &hist_range, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &hist_size, &hist_range, uniform, accumulate );

    //Normalize the result to [ 0, histImage.rows ]
    normalize(b_hist, b_hist, 0, src.rows, NORM_MINMAX, -1, Mat() );
    normalize(g_hist, g_hist, 0, src.rows, NORM_MINMAX, -1, Mat() );
    normalize(r_hist, r_hist, 0, src.rows, NORM_MINMAX, -1, Mat() );

    return 1;
}


/**********************************************************
 * Function: drawColorHist
 * Draws color histogram (BGR) of an image
 *
 * Parameters:
 *   b_hist: blue histogram
 *   g_hist: green histogram
 *   r_hist: red histogram
 *
 ***********************************************************/
void drawColorHist(Mat b_hist, Mat g_hist, Mat r_hist)
{
    // Draw the histograms for B, G and R
    int hist_w = 256/*512*/; int hist_h = 200/*400*/;

    /* Establish the number of bins */
    int hist_size = HIST_BINS;

    int bin_w = cvRound( (double) hist_w/hist_size );

    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Draw for each channel
    for( int i = 1; i < hist_size; i++ )
    {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0 );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                       Scalar( 0, 255, 0), 2, 8, 0 );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                       Scalar( 0, 0, 255), 2, 8, 0 );
    }

    /// Display
    namedWindow("Histogram", WINDOW_AUTOSIZE );
    imshow("Histogram", histImage );
}


/**********************************************************
 * Function: getCorrelation
 * Calculates the correlation between two histograms
 *
 * Parameters:
 *   hist1: first histogram
 *   hist2: second histogram
 *
 * Returns:
 *   correlation of hist1 and hist2
 *
 ***********************************************************/
 double getCorrelation(Mat hist1, Mat hist2)
 {
    double corr = compareHist(
                    hist1,         //input array h1
                    hist2,         //input array h2
                    CV_COMP_CORREL //method
                  );

    return corr;
 }


 /**********************************************************
  * Function: equalizeHistogram
  * Performs histogram equalization on a grayscale/color image.
  *
  * Parameters:
  *   src: Source image
  *
  * Returns:
  *   Histogram equalized image
  *
  ***********************************************************/
  Mat equalizeHistogram(Mat src)
  {
	vector<Mat> channels;
	Mat img_hist_equalized;

	if(src.channels() != 3)
	{
		cvtColor(src, src, COLOR_BGR2GRAY );
		equalizeHist( src, img_hist_equalized );
	}
	else
	{
		//change the color image from BGR to YCrCb format
		cvtColor(src, img_hist_equalized, CV_BGR2YCrCb);

		//split the image into channels
		split(img_hist_equalized,channels);

		//equalize histogram on the 1st channel (Y)
		equalizeHist(channels[0], channels[0]);

		//merge 3 channels including the modified 1st channel into one image
		merge(channels,img_hist_equalized);

		//change the color image from YCrCb to BGR format (to display image properly)
		cvtColor(img_hist_equalized, img_hist_equalized, CV_YCrCb2BGR);
	}

	return img_hist_equalized;

  }

