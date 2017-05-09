/*
 * tracking.cpp
 *
 *  Created on: 27-Mar-2014
 *      Author: ubuntu
 */

#include <stdio.h>

#include "utils.h"
#include "Object.h"

/* namespace
 */
using namespace cv;
using namespace std;


/*
 * Method declarations
 */
void filterContours(vector< vector<Point> > &contours,
                    vector< vector<Point> > &filterd_contours
                   );
void processFilteredContours(Mat &frame, vector<Mat> &roi,
                             vector< vector<Point> > &filtered_contours,
                             deque<Object> &object_list
                            );
void removeOldObjects(deque<Object> &object_list);
void detectLoitering(Mat &frame, deque<Object> &object_list);
void detectUnauthAccess(Mat &frame, deque<Object> &object_list);


/**********************************************************
 * Function: main
 *
 ***********************************************************/
int main(int argc, char** argv)
{
    printf("Usage: tracking.o <video>\n\n");

    if( argc < 2 )
    {
        printf("Arguments mismatch!\n");
        return -1;
    }

    /* Video file */
    VideoCapture cap(argv[1]);

    if( !cap.isOpened() )
    {
       printf("Video file cannot be opened\n");
       return -1;
    }

    /* get the number of frames in the video
     */
    double frameCount = cap.get(CV_CAP_PROP_FRAME_COUNT);
    printf("frame count = %.0lf\n", frameCount);

    /* get the fps
     */
    int fps = cap.get(CV_CAP_PROP_FPS);
    printf("fps = %d\n", fps);

    /* get the frame dimensions
     */
    double width  = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    printf("dimensions (width x height) : %.1lf x %.1lf\n",
            width,
            height);

    /* Input image */
    Mat frame; //, frameCopy;

    /* foreground mask */
    Mat mask;

    /* Background model */
    BackgroundSubtractorMOG bgsubtractor(
                                    1000,  //history
                                    4,    //nMixtures
                                    0.4   //background ratio
                            );
    
    //initialize the VideoWriter object 
    VideoWriter oVideoWriter ("./loiter_video.avi",
                             CV_FOURCC('M','J','P','G'),  //('P','I','M','1'),
                             15, // fps
                             Size(width, height), // size
                             true);

    if ( !oVideoWriter.isOpened() ) //if not intialize the VideoWriter successfully
    {
      cout << "ERROR: Failed to write the video" << endl;
      return -1;
    }

/*    BackgroundSubtractorMOG2 bgsubtractor(
                                    500,   //history
                                    16,    //varThreshold
                                    true   //shadow detection
                            );
*/

    /* Set bgsubtractor parameters
     */
    //bgsubtractor.set("noiseSigma",10);
    //bgsubtractor.set("history",500);
    //bgsubtractor.set("nShadowDetection",50);

    /* Region of interest (ROI) */
    vector<Mat> roi(MAX_PERSON_COUNT);

    /* Detected Objects */
    deque<Object> object_list;

    /* clean garbage values
     */
    roi.clear();
    object_list.clear();


    /* Windows to be displayed
     */
    namedWindow("absdiff", 0);
    namedWindow("Video", 0);
    namedWindow("roi", 1);
    //namedWindow("EnhancedVideo",0);

    /* Start the processing
     */
    for(size_t itr_frames = 0; itr_frames < frameCount ; itr_frames++)
    {
      /* Process every (fps/5)th frame
       */
      int curr_pos = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
      cap.set(CV_CAP_PROP_POS_FRAMES, curr_pos + (int) fps/5);

      cap >> frame;

      if(!frame.data)
      {
        printf("Cannot read from source\n");
        break;
      }

      /* backup the original frame */
      //frame.copyTo(frameCopy);

      /* Enhance the image.
       */
      //frame = equalizeHistogram(frame);

      /* calculate the foreground mask
       */
      bgsubtractor(
        frame,    //source image
        mask,     //output mask
        -1        //learning rate
      );

      /* remove noise from the mask */
      filterNoise(mask, mask);

      imshow("absdiff", mask);

      /* Find contours -
       * 'contours' contains all the contours
       * 'filtered_contours' contains only required contours
       */
      vector< vector<Point> > contours, filtered_contours;

      /* clean garbage values
       */
      contours.clear();
      filtered_contours.clear();

      /* find all the contours
       */
      findContours(
         mask,                  //input image
         contours,              //output image
         CV_RETR_EXTERNAL,      //mode
         CV_CHAIN_APPROX_SIMPLE //method
      );


      /* Filter out unwanted contours
       */
      filterContours(contours, filtered_contours);

      /* Process the filtered contours
       */
      processFilteredContours(frame,
                                      roi,
                                      filtered_contours,
                                      object_list
                                     );

      /* Remove older objects from object_list
       */
      removeOldObjects(object_list);


      /* Check for loitering
       */
      detectLoitering(frame, object_list);

      /* Check for Unauthorized access
       */
      //detectUnauthAccess(frame, object_list);

      /*
      for(size_t itr_ob = 0; itr_ob < object_list.size(); itr_ob++)
      {
          object_list[itr_ob].calcSpeed();

          Point loc;
          object_list[itr_ob].getCenter(loc);

          char s[5];
          memset(s,'\0',5);
          sprintf(s, "%.02lf", object_list[itr_ob].getSpeed());

          putText(
                frame,                  //image
              s,                      //text
                loc,                    //location
                CV_FONT_HERSHEY_PLAIN,  //font
                0.8,                    //font scale
                cv::Scalar(0,0,255)     //color
          );
      }
      */


      //cout<<"hist size = "<<b_hist.size()<<endl;
      //cout<<"prev hist size = "<<prev_b_hist.size()<<endl;

      imshow("Video", frame);
      //imshow("EnhancedVideo",frameCopy);
      
      oVideoWriter.write(frame);
      oVideoWriter.write(frame);
      oVideoWriter.write(frame);

      int key = waitKey(WAIT_TIME) & 255;

      /* press <Esc> key to exit */
      if(key == 27)
        break;

      /* press <Space> key to fast forward video */
      while(key == 32)
      {
        cap.set(CV_CAP_PROP_POS_FRAMES, curr_pos + (int) fps/5);
        key = waitKey(WAIT_TIME) & 255;
        imshow("Video", frame);
      }

    } //end for itr_frames

    for(size_t it=0; it<object_list.size(); it++)
    {
        cout<<"ob "<<it<<": "<<object_list[it].getId()<<endl;
    }

    return 0;
}

/**********************************************************
 * Function: detectLoitering
 * Detects loitering activity in a frame for all the detected
 * objects. If an object is found to be loitering, then it is
 * marked with a 'Loiter' tag.
 *
 * Parameters:
 * frame:       the frame
 * object_list: list of detected objects
 *
 ***********************************************************/
void detectLoitering(Mat &frame, deque<Object> &object_list)
{
  for(size_t itr_ob = 0; itr_ob < object_list.size(); itr_ob++)
  {
      double t = object_list[itr_ob].getTElapsed();

      //cout<<"ob : "<< itr_ob <<" t_elapsed = "<<t<<endl;

      if(t > LOITERING_THRESH)
      {
          //cout<<"loitering ob "<<itr_ob;

          Point loc;
          object_list[itr_ob].getCenter(loc);
          putText(
              frame,                 //image
              "Loiter",              //text
              loc,                   //location
              CV_FONT_HERSHEY_PLAIN, //font
              0.8,                   //font scale
              cv::Scalar(0,0,255)    //color
         );
      }
  }

}


/**********************************************************
 * Function: detectUnauthAccess
 * Detects unauthorized access in a frame for all the
 * detected objects. If an object is found in an
 * unauthorized area, then it is marked with an
 * 'Unauthorized Access' tag.
 *
 * Parameters:
 * frame:       the frame
 * object_list: list of detected objects
 *
 ***********************************************************/
void detectUnauthAccess(Mat &frame, deque<Object> &object_list)
{
  for(size_t itr_ob = 0; itr_ob < object_list.size(); itr_ob++)
  {
      Rect bRect;
      object_list[itr_ob].getBoundingRect(bRect);
      Point2f br = bRect.br();
      if(br.x < 550.0 && br.y < 560.0 && br.y > 440)
      {
          Point loc;
          object_list[itr_ob].getCenter(loc);
          putText(
              frame,                  //image
              "Unauthorized Access", //text
              loc,                    //location
              CV_FONT_HERSHEY_PLAIN,  //font
              0.8,                    //font scale
              cv::Scalar(0,0,255)     //color
         );
      }
  }
}


/**********************************************************
 * Function: removeOldObjects
 * Removes older objects from object_list. Also updates t_elpased
 * of current objects. Older objects need to be removed in order
 * to keep the list up-to-date.
 *
 * Parameters:
 * object_list: list of detected objects
 *
 ***********************************************************/
void removeOldObjects(deque<Object> &object_list)
{
  for(size_t itr_ob = 0; itr_ob < object_list.size(); itr_ob++)
  {
      /* increment unused indicator */
      Point c, prev_c;
      object_list[itr_ob].getCenter(c);
      object_list[itr_ob].getPrevCenter(prev_c);

      if((c.x == prev_c.x) &&
         (c.y == prev_c.y)
        )
      {
          int usage = object_list[itr_ob].getTimesUnused();
          usage++;
          object_list[itr_ob].setTimesUnused(usage);
          //cout<<"incr: "<<usage<<endl;
      }

      /* Increment the t_elapsed of object by 1/5, i.e. 0.2
       * This value is selected here as we are processing
       * every 5th frame. The fps of the input video is 25.
       * Hence, when we process 5 frames, we have moved 1 sec
       * ahead in the video.
       */
      double t = object_list[itr_ob].getTElapsed();
      t = t + 0.2;
      object_list[itr_ob].setTElapsed(t);

      int unused = object_list[itr_ob].getTimesUnused();
      //cout<<"unused: "<<unused<<endl;
      if(unused > 5)
      {
          //cout<<"object removed : "<<itr_ob<<endl;
          object_list.erase(object_list.begin() + itr_ob);
          itr_ob--;
      }
      else
      {
          object_list[itr_ob].setPrevCenter(c);
      }

  }
}


/**********************************************************
 * Function: filterContours
 * Filters out unwanted contours. Small contours are
 * filtered out.
 *
 * Parameters:
 * contours:          input list of detected contours
 * filtered_contours: output list of filtered contours
 *
 ***********************************************************/
void filterContours(vector< vector<Point> > &contours,
                    vector< vector<Point> > &filtered_contours
                   )
{
    vector<Point> approx;

    for( size_t itr_filter = 0; itr_filter < contours.size(); itr_filter++ )
    {
       /* approximate contour with accuracy proportional
        * to the contour perimeter
        * ##Refer sample "squares.cpp"##
        */
       approxPolyDP(
           Mat(contours[itr_filter]), //input matrix
           approx,                    //output array
           arcLength(                /* epsilon – Parameter specifying   */
              Mat(                   /* the approximation accuracy. This */
                contours[itr_filter] /* is the maximum distance between  */
              ),                     /* the original curve and its       */
              true                   /* approximation.                   */
           ) * 0.02,
           true       // true means, the approximated curve is closed
       );

       /* only large contours will be considered
        * Note: absolute value of an area is used because area
        * may be positive or negative - in accordance with the
        * contour orientation
        */
       if( fabs(contourArea(Mat(approx))) > MIN_COUNTOUR_AREA )
       {
         filtered_contours.push_back(approx);
       }
    } //end for itr_filter
}


/**********************************************************
 * Function: processFilteredContours
 * Filters out unwanted contours. Small contours are
 * filtered out.
 *
 * Parameters:
 * frame:             the frame
 * roi:               list of regions of interest from the frame
 * filtered_contours: list of filtered contours
 * object_list:       output list of objects found in the frame
 *
 ***********************************************************/
void processFilteredContours(Mat &frame, vector<Mat> &roi,
                             vector< vector<Point> > &filtered_contours,
                             deque<Object> &object_list
                            )
{
    /* Process all filtered contours
     */
    for( size_t itr_filtc = 0;
         itr_filtc < filtered_contours.size()
             && itr_filtc < MAX_PERSON_COUNT;
         itr_filtc++
       )
    {
      /* get bounding rectangle for the contour */
      Rect boundRect = boundingRect(
              Mat(filtered_contours[itr_filtc])
              );

      /* draw the bounding rectangle in displayed video */
      rectangle(
        frame,               //image on which rectangle needs to be drawn
        boundRect,           //rectangle
        cv::Scalar(0,255,0), //color of rectangle
        1.5                  //line thickness
      );

      /* Calculate the ROI
       * Make the ROI a bit smaller than the bounding box,
       * so as to minimize the background in ROI
       */
      Rect rectCopy = boundRect;
      rectCopy.width  *= 0.5;
      rectCopy.height *= 0.5;
      rectCopy.x = rectCopy.x + rectCopy.width * 0.5;
      rectCopy.y = rectCopy.y + rectCopy.height * 0.2;

      /* Find center of the contour
       */
      Point center(boundRect.x + boundRect.width/2,
                  boundRect.y + boundRect.height/2);

      /* Save ROI as image.
       * Resize it.
       * ROI will be saved only if object has crossed
       * a baseline.
       */
    //  if((center.y > TOP_BASELINE) &&
    //     (center.y < BOT_BASELINE)
    //    )
    //  {
            roi[itr_filtc] = frame(rectCopy);
            resize(
                roi[itr_filtc],  //src
                roi[itr_filtc],  //dst
                Size(50,50)      //size
            );
            imshow("roi", roi[itr_filtc]);
    //  }


      /* Find color histogram of ROI
       */
      Mat temp_b_hist, temp_g_hist, temp_r_hist;
      int histStatus = calcColorHist(
                          roi[itr_filtc], //source image
                          temp_b_hist,    // destination blue histograms
                          temp_g_hist,    // destination green histograms
                          temp_r_hist     // destination red histograms
                       );

      if(histStatus != 1)
      {
        cout<<"Unable to calculate color histogram!"<<endl;
        continue;
      }

      /* Display the histogram
       */
      //drawColorHist(temp_b_hist, temp_g_hist, temp_r_hist);


      /* Compare each current histogram with all previous
       * histograms.
       */
      double max_hist_total = 0;
      int matching_index = -1;

      for(size_t itr_hist=0; itr_hist<object_list.size(); itr_hist++)
      {
        /*----------
        cout<<"\nobject list index = "<<itr_hist<<"\t";
        cout<<"curr index = "<<itr_filtc<<"\n";
        ----------*/

        /* increment unused indicator */
        int usage = object_list[itr_hist].getTimesUnused();
        usage++;
        object_list[itr_hist].setTimesUnused(usage);

        /* Get correlation between current and previous
         * BGR histograms.
         */
        Mat b, g, r;
        object_list[itr_hist].getBHist(b);
        object_list[itr_hist].getGHist(g);
        object_list[itr_hist].getRHist(r);

        double b_correlation = getCorrelation(b, temp_b_hist);
        double g_correlation = getCorrelation(g, temp_g_hist);
        double r_correlation = getCorrelation(r, temp_r_hist);

        /*----------
        cout<<"Correlation : "<<endl;
        cout<<"\tblue: "<<b_correlation;
        cout<<"\tgreen: "<<g_correlation;
        cout<<"\tred: "<<r_correlation<<endl;
        ----------*/

        double hist_total = b_correlation
                                  + g_correlation
                                  + r_correlation;

        if(hist_total > max_hist_total)
        {
            max_hist_total = hist_total;
            matching_index = itr_hist;
            //cout<<"max_hist_total = "<<max_hist_total<<endl;
        }

      }//end for itr_hist

      //cout<<"-----------------------"<<endl;



      /* Store detected object in object_list.
       * New objects will be appended to the list.
       * Old objects will be updated.
       */

      /* The detected object */
      Object ob;

      /* Object ID in int and string */
      int ob_id;
      char str_ob_id[STR_LEN];

      memset( str_ob_id, '\0', STR_LEN );

      /* Get top left corner of bounding rectangle.
       * Object ID will be displayed at this location.
       */
      Point text_location = boundRect.tl();
      text_location.y -= 5;

      /* assign the histograms */
      ob.setBHist(temp_b_hist);
      ob.setGHist(temp_g_hist);
      ob.setRHist(temp_r_hist);

      /* assign the bounding rectangle and center point */
      ob.setCenter(center);
      ob.setBoundingRect(boundRect);


      /* new object - append it to the list*/
      if(matching_index == -1)
      {
        int size = object_list.size();
        ob.setId(size);
        object_list.push_back(ob);

        ob_id = size;
      }

      /* A new object as it did not match
       * with any previous objects.
       */
      else if(max_hist_total < MATCHING_THRESH)
      {
        int size = object_list.size();
        ob.setId(size);
        object_list.push_back(ob);

        ob_id = size;
      }

      /* old object - update it*/
      else
      {
        ob.setId(matching_index);
        double t = object_list[matching_index].getTElapsed();
        ob.setTElapsed(t+0.2);
        object_list[matching_index] = ob;

        ob_id = matching_index;
      }

      /* Display object ID */
     /* sprintf(str_ob_id, "%02d", ob_id);
      putText(
         frame,                 //image
         str_ob_id,             //text
         text_location,         //location
         CV_FONT_HERSHEY_PLAIN, //font
         0.8,                   //font scale
         cv::Scalar(50,50,200)  //color
      );
     */


    } //end for itr_filtc
}
