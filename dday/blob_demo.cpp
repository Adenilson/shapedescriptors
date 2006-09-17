/*******************************************************************************
Purpose: Given an image, it does blobs analysis, calculates area of
 isolated blobs in image. Steps
a) read original image
b) transform it to gray scale
c) morphological operators
d) blobanalysis (only identify blobs within a given area range).

Author: Adenilson Cavalcanti da Silva savagobr@yahoo.com
License: GPL for non comercial use (contact author for other licensing contracts)
Dependencies: OpenCV (0.96) and Dave Grossman's blob code
History:
	vs 0.01 22-07-2005 Start

        vs 0.02 26-06-2006: Code clean up

        vs 0.03 17-09-2006: put code on svn repository


TODO: move code into separated modules.

*******************************************************************************/


#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <set>
#include <complex>
using namespace std;

/* Blob library (ps: pay attention that blobs.h require previous
   include of std::iostream!) */
#include "blobs.h"


/** Blobs descriptors features
 *
 */
struct blob_features {
	/** Some feature descriptors of blobs */
	float area, perimeter;
	complex<float> centroid;
	/** minimum coordinate values */
	float min_x, min_y;
	/** maximum coordinate values */
	float max_x, max_y;

	/** Function to calculate centroid */
	void calc_centroid() {
		centroid.real() = (min_x + max_x) / 2;
		centroid.imag() = (min_y + max_y) / 2;
	}

	/** Overloaded operator */
	blob_features &operator=(const blob_features &obj) {
		area = obj.area;
		perimeter = obj.perimeter;
		min_x = obj.min_x;
		min_y = obj.min_y;
		max_x = obj.max_x;
		max_y = obj.max_y;
		centroid = obj.centroid;
	}
};

/** Structure to hold filtered blobs.
 *
 */
struct blob_result {
	/** holds blob descriptors vector */
	blob_features *blobs;

	/** number of blobs within vector */
	int blob_count;

	/** Default empty constructor */
	blob_result(void): blob_count(0), blobs(NULL) { }

	/** Allocate memory space for blobs */
	int allocate(int length) {
		blob_count = length;
		blobs = new blob_features[blob_count];
		if (!blobs) {
			blob_count = 0;
			blobs = NULL;
		}
		return blob_count;
	}

	/** Constructor to allocate blob object vector */
	blob_result(int num_blobs) {
		allocate(num_blobs);
	}

	blob_result & operator=(const blob_result &obj) {
		if (allocate(obj.blob_count)) {
			for (int i = 0; i < blob_count; ++i)
				blobs[i] = obj.blobs[i];
		}
		return *this;
	}


	/** Destructor, free up resources */
	~blob_result() {
		if (blobs)
			delete [] blobs;

	}

};

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
blob_result process_image(IplImage* sample_image, int threshold, int min_area, int max_area,
		   int open_count = 0, bool grayit = true, bool morpho_operator = true);

/** Show a given image in a window.
 *
 * @param name Window name.
 *
 * @param transformed Image to be show.
 *
 * TODO: make some widget resource management (I want to keep
 *       opened windows still active).
 */
void show_img(const char* name, IplImage *transformed);


int main(int argc, char** argv)
{

	IplImage* sample_image = 0;
	blob_result result;

	// Input the sample picture. The user must make sure it's in the right folder
	const char *filename = (argc >= 2 ? argv[1] : "data/Circles.jpg");
	sample_image = cvLoadImage(filename, -1);
	show_img("Original", sample_image);

	int threshold, minarea, maxarea, opencount;
	bool grayit = true, morpho_operator = true;
	threshold = 100;
	minarea = 500;
	maxarea = 2000;
	opencount = 1;
	result = process_image(sample_image, threshold, minarea, maxarea, opencount,
		      grayit, morpho_operator);

	cvReleaseImage(&sample_image);
	cout << "ALL DONE" << endl;

	cout << " blobs number: " << result.blob_count << endl; 

	return 0;

}

void show_img(const char* name, IplImage *transformed) {
	cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
	cvShowImage(name, transformed);
	cvWaitKey(0);
	cvDestroyWindow(name);
}

blob_result process_image(IplImage* sample_image, int threshold, int min_area, int max_area,
		   int open_count, bool grayit, bool morpho_operator)
{
	//Images
	IplImage* gray_image = 0;
	IplImage* thresholded_image = 0;

	//Holds blob related descriptors
	float (*region_data)[BLOBDATACOUNT];
	int* working_storage;
	int high_region_num;
	//Helper variables
	int iMeanx, iMeany;//, max_area = 1000;
	int length = 3;
	int counter = 0;
	CvPoint point1,point2;
	blob_result result;

	// Display Sample image
	//show_img("sample_image", sample_image);
	int cols = sample_image->width;
	int rows = sample_image->height;

	// Make the sample picture into a gray image
	if (grayit) {
		gray_image = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 1);
		cvCvtColor(sample_image, gray_image, CV_BGR2GRAY);
        } else
		gray_image = sample_image;

	// Display Gray image
	show_img("gray_image", gray_image);

	// Make it into a binary image
	thresholded_image = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 1);
	cvThreshold(gray_image, thresholded_image, threshold, 255, CV_THRESH_BINARY);

	// Make sure that there are no isolated spots in the image
	if (morpho_operator)
		if(open_count > 0)
		{
			cvDilate(thresholded_image, thresholded_image, NULL, open_count);
			cvErode(thresholded_image, thresholded_image, NULL, open_count);
		}
		else
		{
			cvErode(thresholded_image, thresholded_image, NULL, -open_count);
			cvDilate(thresholded_image, thresholded_image, NULL, -open_count);
		}

	// Display Thresholded image
	show_img("thresholded_image", thresholded_image);

	// Call Blob Analysis routine to analyze image
	// Working storage. Note +2 +2
	working_storage = new int[(BLOBROWCOUNT+2)*(BLOBCOLCOUNT+2)];

	// Blob result array
	region_data = new float[BLOBTOTALCOUNT][BLOBDATACOUNT];

	high_region_num = BlobAnalysis(thresholded_image, working_storage, region_data, cols, rows, (uchar)255, min_area);

	// Add bounding rectangles to Sample image
	for (int this_region = 1; this_region <= high_region_num; this_region++)
	{
		if (region_data[this_region][BLOBAREA] < max_area) {
			point1.x = cvRound(region_data[this_region][BLOBMINX]);
			point1.y = cvRound(region_data[this_region][BLOBMINY]);
			point2.x = cvRound(region_data[this_region][BLOBMAXX]);
			point2.y = cvRound(region_data[this_region][BLOBMAXY]);

			// find the average of the blob (i.e. estimate its centre)
			// and draw a retangle.
			iMeanx=(point1.x + point2.x) / 2;
			iMeany=(point1.y + point2.y) / 2;
			cvRectangle(sample_image,
				    cvPoint(iMeanx - length, iMeany - length),
				    cvPoint(iMeanx + length, iMeany + length),
				    CV_RGB(0, 0, 255), 1);
			cvRectangle(sample_image,
				    cvPoint(iMeanx - length + 1, iMeany - length + 1),
				    cvPoint(iMeanx + length + 1, iMeany + length + 1),
				    CV_RGB(0, 0, 255), 1);


			cvRectangle(sample_image, point1, point2, CV_RGB(255, 0, 0), 1);
			counter++;
		}


	}

	if (result.allocate(counter))
		/* XXX: move this code to a separated function */
		for (int this_region = 1, counter = 0; this_region <= high_region_num; this_region++)
		{
			if (region_data[this_region][BLOBAREA] < max_area) {

				result.blobs[counter].min_x = region_data[this_region][BLOBMINX];
				result.blobs[counter].min_y = region_data[this_region][BLOBMINY];

				result.blobs[counter].max_x = region_data[this_region][BLOBMAXX];
				result.blobs[counter].max_y = region_data[this_region][BLOBMAXY];

				result.blobs[counter].max_y = region_data[this_region][BLOBMAXY];
				result.blobs[counter].calc_centroid();

			}


		}


        cout << "number of blobs = " << counter << endl;
	// Display Sample image with bounding rectangles added
	show_img("processed_image", sample_image);
	cvSaveImage("sample_image.jpg", sample_image);

	// Print the results
	PrintRegionDataArray(region_data);

	cvReleaseImage(&thresholded_image);
	cvReleaseImage(&gray_image);
	delete [] region_data;
	delete [] working_storage;

	return result;
}
