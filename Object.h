/*
 * Object.h
 *
 *  Created on: 16-Apr-2014
 *      Author: ubuntu
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "utils.h"

class Object
{
	private:
		/* ID */
		int id;

		/* Number of times the object stays unused */
		int times_unused;

		/* Time, in seconds, the object stays in the frame */
		double t_elapsed;

		/* Speed of the object */
		double speed;

		/* RGB Histograms */
		Mat b_hist, g_hist, r_hist;

		/* Center point of the object */
		Point center, prev_center;

		/* Bounding rectangle */
		Rect bounding_rect;

	public:
		/* constructor & destructor */
		Object();
		virtual ~Object();

		/* setter functions */
		void setId(int);
		void setTimesUnused(int);
		void setTElapsed(double);
		void setSpeed(double);
		void calcSpeed();
		void setBHist(Mat);
		void setGHist(Mat);
		void setRHist(Mat);
		void setCenter(Point);
		void setPrevCenter(Point);
		void setBoundingRect(Rect);

		/* getter functions */
		int getId();
		int getTimesUnused();
		double getTElapsed();
		double getSpeed();
		void getBHist(Mat&);
		void getGHist(Mat&);
		void getRHist(Mat&);
		void getCenter(Point&);
		void getPrevCenter(Point&);
		void getBoundingRect(Rect&);
};

#endif /* OBJECT_H_ */
