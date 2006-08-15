//------------------------------------------------------------------------------
#ifndef _CONTOUR_H_
#define _CONTOUR_H_
//------------------------------------------------------------------------------
#include "base.h"

//Try to found the contour in thresholded image
CvSeq *contour_follow(IplImage *thres, CvMemStorage* storage, int *ncontour);

//Plot a contour in a image
void plot_contour(IplImage *target, CvSeq *contours, int thickness = 1, int levels = 1);

//This one marks the centroid of each found contour
void mark_centroid(CvSeq *contour, IplImage *img);


//Calculates centroid, parameters:
//a) contour: pointer to sequence that holds contours
//b) a vector with coordinates of centroid of each contour (it does the
// memory allocation!)
//c) size: pointer to a variable that will hold the allocated size of 
//answer vector
m_point* calc_centroid(CvSeq *contour, int *size);



//Calculate each contour area, parameters:
//a) contour: pointer to sequence that holds contours
void calc_area(CvSeq *contour);

void calc_area(CvSeq *contour, float *v, int *size);










//------------------------------------------------------------------------------
#endif
