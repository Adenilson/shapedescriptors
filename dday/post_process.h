#ifndef __POST_PROCESS__
#define __POST_PROCESS__

#include <opencv/cv.h>
#include "data_types.h"

/** FIXME: Add doxygen comments. */
void threshold(int threshold_value, void *gray_img, void *thres_img,
	       int block_size = 7, double std_desv = 5);


/** Experimental function to crop a ROI and upscale it.
 *
 * @param img A image pointer.
 *
 * @param blob_features A object with coordinates of ROI.
 *
 * TODO: rewrite the function
 */
void test(IplImage *img, blob_features &coord);

#endif
