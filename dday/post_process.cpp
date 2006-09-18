#include <opencv/highgui.h>
#include "post_process.h"
#include "window.h"


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

void test(IplImage *img, blob_features &coord)
{
	CvRect comp_rect;
	comp_rect = cvRect((int) coord.min_x, (int) coord.min_y,
			   (int) coord.max_y, (int) coord.max_x);

	//Cropping the image
	cvSetImageROI(img, comp_rect);
	uchar *data = NULL;
	cvGetImageRawData(img, &data, NULL, NULL);
	//XXX: CvSize != cvSize
	CvSize size = cvSize((int) (coord.max_x - coord.min_x),
			     (int) (coord.max_y - coord.min_y));
	IplImage *minor = cvCreateImage(size, IPL_DEPTH_8U, 3);//IPL_DEPTH_32F
	cvSetImageData(minor, data, img->widthStep);
	show_img("ROI", minor);

	//Upscaling
	size = cvSize(minor->width, minor->height);
	IplImage *major = cvCreateImage(cvSize(size.width * 2, size.height * 2), IPL_DEPTH_8U, 3);
	cvPyrUp(minor, major);
	show_img("upscale", major);

	cvSaveImage("license_plate.bmp", major);

	//Resources release
	//XXX: it seems that it doesn't copies data to new image
	//cvReleaseImage(&minor);
	cvReleaseImage(&major);
	//XXX: cvGetImageRawData dont seems to copy data
	//delete [] data;
}

