#include <opencv/highgui.h>
#include <iostream>
using namespace std;
/* Blob library (ps: pay attention that blobs.h require previous
   include of std::iostream!) */
#include "blobs.h"
#include "blob_filter.h"
#include "window.h"

/* TODO: remove gratuitous comments
 *       break this enormous function to smaller functions.
 */
blob_result process_image(IplImage* sample_image, int threshold,
			  int min_area, int max_area,
			  int open_count, bool grayit,
			  bool morpho_operator)
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

				result.blobs[counter].area = region_data[this_region][BLOBAREA];

				result.blobs[counter].perimeter = region_data[this_region][BLOBPERIMETER];



				counter++;
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
