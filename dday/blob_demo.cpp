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

*******************************************************************************/


#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <set>
using namespace std;

/* Blob library (ps: pay attention that blobs.h require previous
   include of std::iostream!) */
#include "blobs.h"


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
void process_image(IplImage* sample_image, int threshold, int min_area, int max_area,
		   int open_count = 0, bool grayit = true, bool morpho_operator = true);

/** Show a given image in a window.
 *
 * @param name Window name.
 *
 * @param transformed Image to be show.
 *
 */
void show_img(const char* name, IplImage *transformed);


int main(int argc, char** argv)
{

	IplImage* sample_image = 0;

	// Input the sample picture. The user must make sure it's in the right folder
	const char *filename = (argc >= 2 ? argv[1] : "data/Circles.jpg");
	sample_image = cvLoadImage(filename, -1);
	show_img("Original", sample_image);

	int threshold, minarea, maxarea, opencount;
	bool grayit = true, morpho_operator = true;
	threshold = 100;
	minarea = 500;
	maxarea = 5000;
	opencount = 1;
	process_image(sample_image, threshold, minarea, maxarea, opencount,
		      grayit, morpho_operator);

	cvReleaseImage(&sample_image);
	cout << "ALL DONE" << endl;
	return 0;

}

void show_img(const char* name, IplImage *transformed) {
	cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
	cvShowImage(name, transformed);
	cvWaitKey(0);
}

void process_image(IplImage* sample_image, int threshold, int min_area, int max_area,
		   int open_count, bool grayit, bool morpho_operator)
{
	//Images
	IplImage* GrayImage = 0;
	IplImage* ThresholdedImage = 0;

	//Helper variables
	int iMeanx, iMeany;//, max_area = 1000;

	// Display Sample image
	//show_img("sample_image", sample_image);
	int Cols = sample_image->width;
	int Rows = sample_image->height;

	// Make the sample picture into a gray image
	if(grayit) {
		GrayImage = cvCreateImage(cvSize(Cols, Rows), IPL_DEPTH_8U, 1);
		cvCvtColor(sample_image, GrayImage, CV_BGR2GRAY);
        }
        else
		GrayImage = sample_image;

	// Display Gray image
	show_img("GrayImage", GrayImage);

	// Make it into a binary image
	ThresholdedImage = cvCreateImage(cvSize(Cols, Rows), IPL_DEPTH_8U, 1);
	cvThreshold(GrayImage, ThresholdedImage, threshold, 255, CV_THRESH_BINARY);

	// Make sure that there are no isolated spots in the image
	if (morpho_operator)
		if(open_count > 0)
		{
			cvDilate(ThresholdedImage, ThresholdedImage, NULL, open_count);
			cvErode(ThresholdedImage, ThresholdedImage, NULL, open_count);
		}
		else
		{
			cvErode(ThresholdedImage, ThresholdedImage, NULL, -open_count);
			cvDilate(ThresholdedImage, ThresholdedImage, NULL, -open_count);
		}

	// Display Thresholded image
	show_img("ThresholdedImage", ThresholdedImage);

	// Call Blob Analysis routine to analyze image
	// Working storage. Note +2 +2
	int* WorkingStorage = new int[(BLOBROWCOUNT+2)*(BLOBCOLCOUNT+2)];

	// Blob result array
	float (*RegionData)[BLOBDATACOUNT] = new float[BLOBTOTALCOUNT][BLOBDATACOUNT];

	int HighRegionNum = BlobAnalysis(ThresholdedImage, WorkingStorage, RegionData, Cols, Rows, (uchar)255, min_area);

	// Add bounding rectangles to Sample image
	CvPoint point1,point2;
	int counter = 0;
	for(int ThisRegion = 1; ThisRegion <= HighRegionNum; ThisRegion++)
	{
		if(RegionData[ThisRegion][BLOBAREA] < max_area) {
			point1.x = cvRound(RegionData[ThisRegion][BLOBMINX]);
			point1.y = cvRound(RegionData[ThisRegion][BLOBMINY]);
			point2.x = cvRound(RegionData[ThisRegion][BLOBMAXX]);
			point2.y = cvRound(RegionData[ThisRegion][BLOBMAXY]);

			//************************** MOD SAVAGO*************************
			// find the average of the blob (i.e. estimate its centre)
			iMeanx=(point1.x + point2.x)/2; //(iMinx+iMaxx)/2;
			iMeany=(point1.y + point2.y)/2; //(iMiny+iMaxy)/2;
			cvRectangle(sample_image, cvPoint(iMeanx-1, iMeany-1), cvPoint(iMeanx+1, iMeany+1),
				    CV_RGB(0, 0, 255), 1);
			//************************** MOD SAVAGO*************************

			cvRectangle(sample_image, point1, point2, CV_RGB(255, 0, 0), 1);
			counter++;
		}


	}

        cout << "number of blobs = " << counter << endl;
	// Display Sample image with bounding rectangles added
	show_img("processed_image", sample_image);
	cvSaveImage("sample_image.jpg", sample_image);

	// Print the results
	PrintRegionDataArray(RegionData);

	cvReleaseImage(&ThresholdedImage);
	cvReleaseImage(&GrayImage);

}
