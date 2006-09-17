#ifndef __BLOB_FILTER__
#define __BLOB_FILTER__

#include <opencv/cv.h>

#include "data_types.h"

/** The worker, filters image and search for blobs.
 *
 * @param sample_image analysed image.
 *
 * @param threshold param to threshold image (we use OCV cvThreshold).
 *
 * @param min_area minimal area to consider a blob.
 *
 * @param max_area maximum area to consider a blob.
 *
 * @param open_count dilation factor (morphology).
 *
 * @param morpho_operator if we should run morphological operations on image.
 *
 */
blob_result process_image(IplImage* sample_image, int threshold,
			  int min_area, int max_area,
			  int open_count = 0, bool grayit = true,
			  bool morpho_operator = true);


#endif
