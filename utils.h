/*
 * utils.h
 *
 *  Created on: 27-Mar-2014
 *      Author: ubuntu
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"

#include <cv.h>

/* Minimum contour area for it to be a person */
#define MIN_COUNTOUR_AREA 500

/* Maximum number of people in a frame */
#define MAX_PERSON_COUNT 50

/* Number of histogram bins */
#define HIST_BINS 64

/* Time to wait (in ms) after processing a frame */
#define WAIT_TIME 180

/* Minimum hist_total required to satisfy matching
 * criterion
 */
#define MATCHING_THRESH 2.0

/* Baselines */
#define TOP_BASELINE 10
#define BOT_BASELINE 165

/* String length for displaying object number
 */
#define STR_LEN 4

/* Loitering threshold in seconds
 */
#define LOITERING_THRESH 20


using namespace cv;
using namespace std;

void filterNoise( Mat src, Mat dst );
//double sumOfMatrix(Mat matrix);
double eulicdeanDist(Point2f p1, Point2f p2);
int calcColorHist(Mat src, Mat& b_hist, Mat& g_hist, Mat& r_hist);
void drawColorHist(Mat b_hist, Mat g_hist, Mat r_hist);
double getCorrelation(Mat hist1, Mat hist2);
Mat equalizeHistogram(Mat src);


#endif /* UTILS_H_ */
