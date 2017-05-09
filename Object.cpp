/*
 * Object.cpp
 *
 *  Created on: 16-Apr-2014
 *      Author: ubuntu
 */

#include "Object.h"
#include "utils.h"

Object::Object()
{
	id = 0;
	times_unused = 0;
	t_elapsed = 0;
	speed = 0;
}

Object::~Object()
{
	b_hist.release();
	g_hist.release();
	r_hist.release();
}

/**********************************************************
 * Function: setId
 * Assigns value to the ID of the object.
 *
 * Parameters:
 *   i: The ID to be assigned
 ***********************************************************/
void Object::setId(int i)
{
	id = i;
}

/**********************************************************
 * Function: setTimesUnused
 * Sets the times unused indicator.
 *
 * Parameters:
 *   u: times unused indicator to be assigned.
 ***********************************************************/
void Object::setTimesUnused(int u)
{
	times_unused = u;
}

/**********************************************************
 * Function: setTElapsed
 * Sets the time elapsed.
 *
 * Parameters:
 *   u: Time elapsed to be assigned.
 ***********************************************************/
void Object::setTElapsed(double t)
{
    t_elapsed = t;
}

/**********************************************************
 * Function: setSpeed
 * Sets the speed of the object.
 *
 * Parameters:
 *   s: speed to be assigned.
 ***********************************************************/
void Object::setSpeed(double s)
{
    speed = s;
}

/**********************************************************
 * Function: calcSpeed
 * Calculates the speed of the object.
 *
 ***********************************************************/
void Object::calcSpeed()
{
	double dist = abs(eulicdeanDist(prev_center, center)) / 100;
	if(dist == 0)
		speed = 0;
	else
		speed = dist / 0.2;
}

/**********************************************************
 * Function: setBHist
 * Assigns value to blue histogram of the object.
 *
 * Parameters:
 *   h: Assigned blue histogram
 ***********************************************************/
void Object::setBHist(Mat h)
{
	h.copyTo(b_hist);
}

/**********************************************************
 * Function: setGHist
 * Assigns value to green histogram of the object.
 *
 * Parameters:
 *   h: Assigned green histogram
 ***********************************************************/
void Object::setGHist(Mat h)
{
	h.copyTo(g_hist);
}

/**********************************************************
 * Function: setRHist
 * Assigns value to red histogram of the object.
 *
 * Parameters:
 *   h: Assigned red histogram
 ***********************************************************/
void Object::setRHist(Mat h)
{
	h.copyTo(r_hist);
}

/**********************************************************
 * Function: setCenter
 * Assigns value to the center point of the object.
 *
 * Parameters:
 *   p: The center point to be assigned
 ***********************************************************/
void Object::setCenter(Point p)
{
	center = p;
	//cout<<"Object Center = ("<<p.x<<", "<<p.y<<")"<<endl;
}

/**********************************************************
 * Function: setPrevCenter
 * Assigns value to the (previous) center point of the object.
 *
 * Parameters:
 *   p: The center point to be assigned
 ***********************************************************/
void Object::setPrevCenter(Point p)
{
	prev_center = p;
	//cout<<"Object Center = ("<<p.x<<", "<<p.y<<")"<<endl;
}

/**********************************************************
 * Function: setBoundingRect
 * Assigns value to the bounding rectangle of the object.
 *
 * Parameters:
 *   r: The rectangle to be assigned
 ***********************************************************/
void Object::setBoundingRect(Rect r)
{
	bounding_rect = r;
}

/**********************************************************
 * Function: getId
 * Returns ID of the object
 *
 * Returns:
 *   id
 ***********************************************************/
int Object::getId()
{
	return id;
}

/**********************************************************
 * Function: getTimesUnused
 * Returns the times unused indicator.
 *
 * Returns:
 *   times unused
 ***********************************************************/
int Object::getTimesUnused()
{
	return times_unused;
}

/**********************************************************
 * Function: getTElapsed
 * Returns the time elapsed indicator.
 *
 * Returns:
 *   times elapsed
 ***********************************************************/
double Object::getTElapsed()
{
	return t_elapsed;
}

/**********************************************************
 * Function: getSpeed
 * Returns the speed of the object.
 *
 * Returns:
 *   speed
 ***********************************************************/
double Object::getSpeed()
{
	return speed;
}

/**********************************************************
 * Function: getBHist
 * Sets blue histogram of the object to 'h'
 *
 * Parameters:
 *   h: Output blue histogram
 ***********************************************************/
void Object::getBHist(Mat& h)
{
	b_hist.copyTo(h);
}

/**********************************************************
 * Function: getGHist
 * Sets green histogram of the object to 'h'
 *
 * Parameters:
 *   h: Output green histogram
 ***********************************************************/
void Object::getGHist(Mat& h)
{
	g_hist.copyTo(h);
}

/**********************************************************
 * Function: getRHist
 * Sets red histogram of the object to 'h'
 *
 * Parameters:
 *   h: Output red histogram
 ***********************************************************/
void Object::getRHist(Mat& h)
{
	r_hist.copyTo(h);
}

/**********************************************************
 * Function: getCenter
 * Returns center point of the object.
 *
 * Parameters:
 *   p: Output center point
 ***********************************************************/
void Object::getCenter(Point& p)
{
	p = center;
}

/**********************************************************
 * Function: getPrevCenter
 * Returns (previous) center point of the object.
 *
 * Parameters:
 *   p: Output center point
 ***********************************************************/
void Object::getPrevCenter(Point& p)
{
	p = prev_center;
}

/**********************************************************
 * Function: getBoundingRect
 * Returns the bounding rectangle of the object.
 *
 * Parameters:
 *   r: output rectangle
 ***********************************************************/
void Object::getBoundingRect(Rect& r)
{
	 r = bounding_rect;
}

