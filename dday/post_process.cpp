
#include <opencv/cv.h>
#include "post_process.h"


void threshold(int threshold_value, void *gray_img, void *thres_img,
	       int block_size, double std_desv)
{

	IplImage *gray = (IplImage *) gray_img;
	IplImage *thres = (IplImage *) thres_img;

	//cvThreshold(gray, thres, threshold, 256, CV_THRESH_BINARY_INV);
	cvAdaptiveThreshold(gray, thres, threshold_value,
			    CV_ADAPTIVE_THRESH_MEAN_C,//_THRESH_GAUSSIAN_C
			    CV_THRESH_BINARY_INV, block_size, std_desv);
}

