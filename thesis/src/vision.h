/**
 * @file   vision.h
 * @author Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2005
 * @date   Sometime in 2005
 *
 * @brief  Morphology code, functions to close/open contours.
 *
 * At moment, these are only wrappers to OpenCV, but there are some
 * morphological operations which I should implement here in future.
 *
 */

#ifndef _VISION_H_
#define _VISION_H_


#include "base.h"


/** Type of erosion structuring element, we use a rectangle as
 * default.
 */
const int element_shape = CV_SHAPE_RECT;

/** Provides erosion operation (just a wrapper to OpenCV).
 * First version date back to 22-08-2005.
 *
 * @param src Source image, the one that in which to perform operation.
 * @param dest Destination image, hold result of operation.
 * @param times How many times to apply operation (most of time 1 time is
 *              enough).
 * @param pos I quite dont remember what this one is for...
 *
 * FIXME: for what we need pos?
 */
inline void erosion(IplImage *src, IplImage *dest, int times = 1, int pos = 1)
{
	IplConvKernel* element = NULL;
	element = cvCreateStructuringElementEx(pos*2+1, pos*2+1, pos, pos,
					       element_shape, 0);
	cvErode(src, dest, element, times);
	cvReleaseStructuringElement(&element);

}

//Implements dilation
//
/** Provides erosion operation wrapper to OpenCV functions. Most of times you
 * will be supplying only the images and **maybe** number of times to repeat
 * operation.
 * First version date back to 22-08-2005.
 *
 * @param src Source image, the one that in which to perform operation.
 * @param dest Destination image, hold result of operation.
 * @param times How many times to apply operation (most of time 1 time is
 *              enough).
 * @param pos I quite dont remember what this one is for...
 *
 * FIXME: for what we need pos?
 */
inline void dilation(IplImage *src, IplImage *dest, int times = 1, int pos = 1)
{

	IplConvKernel* element = NULL;
	element = cvCreateStructuringElementEx(pos*2+1, pos*2+1, pos, pos,
					       element_shape, 0 );
	cvDilate(src, dest, element, times);
	cvReleaseStructuringElement(&element);

}


//Do threshold in original image
/** Performs threshold in a image, we default to make foreground to be
 * white (the object) and background to be black.
 *
 * @param threshold Threshold value (the one which pixels with value above
 *                  will be turned into background.
 * @param gray Input image, should be in grayscale (1 color channel, 8 bits).
 * @param thres Output image, same time of input image.
 *
 * FIXME: add a new function with adaptative threshold, currently the code
 *        is commented (shame on me!) ;-)
 */
inline void threshold(int threshold, IplImage *gray, IplImage *thres)
{

	//cvZero(gray);
	//cvAdaptiveThreshold(gray, image, max, method, type, param);
	//cvThreshold( IplImage* src, IplImage* dst, float thresh, float maxvalue,
	//CvThreshType type);
	cvThreshold(gray, thres, threshold, 256, CV_THRESH_BINARY_INV);


	/* XXX: move this to other function.
	  cvAdaptiveThreshold(gray, thres, threshold,
	  CV_ADAPTIVE_THRESH_MEAN_C,//_THRESH_GAUSSIAN_C
	  CV_THRESH_BINARY_INV, 7, 5);
	*/

}


#endif
