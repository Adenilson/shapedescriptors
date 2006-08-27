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






//------------------------------------------------------------------------------
#endif
