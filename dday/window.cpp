#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "window.h"



void show_img(const char* name, void *img) {
	IplImage *transformed = (IplImage *) img;
	cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
	cvShowImage(name, transformed);
	cvWaitKey(0);
	cvDestroyWindow(name);
}

