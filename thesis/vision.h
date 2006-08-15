//------------------------------------------------------------------------------
#ifndef _VISION_H_
#define _VISION_H_
//------------------------------------------------------------------------------
#include "base.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Do threshold in original image
void threshold(int threshold, IplImage *gray, IplImage *thres) {

  //cvZero(gray);
  //cvAdaptiveThreshold(gray, image, max, method, type, param);  
  //cvThreshold( IplImage* src, IplImage* dst, float thresh, float maxvalue,
	//CvThreshType type);
  cvThreshold(gray, thres, threshold, 256, CV_THRESH_BINARY_INV);		
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#endif

