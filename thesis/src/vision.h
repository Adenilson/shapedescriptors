//------------------------------------------------------------------------------
#ifndef _VISION_H_
#define _VISION_H_
//------------------------------------------------------------------------------
#include "base.h"

//------------------------------------------------------------------------------

const int element_shape = CV_SHAPE_RECT;
//Implements erosion
//22-08-2005
inline void erosion(IplImage *src, IplImage *dest, int times = 1, int pos = 1) {
  IplConvKernel* element = NULL;
  element = cvCreateStructuringElementEx(pos*2+1, pos*2+1, pos, pos,
  	element_shape, 0);
  cvErode(src, dest, element, times);
  cvReleaseStructuringElement(&element);

}

//Implements dilation
//22-08-2005
inline void dilation(IplImage *src, IplImage *dest, int times = 1, int pos = 1) {

  IplConvKernel* element = NULL;
  element = cvCreateStructuringElementEx(pos*2+1, pos*2+1, pos, pos,
  	element_shape, 0 );
  cvDilate(src, dest, element, times);
  cvReleaseStructuringElement(&element);

}


//------------------------------------------------------------------------------
//Do threshold in original image
inline void threshold(int threshold, IplImage *gray, IplImage *thres) {

  //cvZero(gray);
  //cvAdaptiveThreshold(gray, image, max, method, type, param);
  //cvThreshold( IplImage* src, IplImage* dst, float thresh, float maxvalue,
	//CvThreshType type);
  cvThreshold(gray, thres, threshold, 256, CV_THRESH_BINARY_INV);


  /*
	cvAdaptiveThreshold(gray, thres, threshold,
			    CV_ADAPTIVE_THRESH_MEAN_C,//_THRESH_GAUSSIAN_C
			    CV_THRESH_BINARY_INV, 7, 5);
  */

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#endif

