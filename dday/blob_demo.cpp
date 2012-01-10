/**********************************************************************
Purpose: Given an image, it does blobs analysis, calculates area of
 isolated blobs in image. Steps
a) read original image
b) transform it to gray scale
c) morphological operators
d) blobanalysis (only identify blobs within a given area range)
e) crop and resize a ROI from original image


Author: Adenilson Cavalcanti da Silva savagobr@yahoo.com
License: GPL for non comercial use (contact author for other licensing
         contracts)
Dependencies: OpenCV (0.96) and Dave Grossman's blob code
History:
	vs 0.01 22-07-2005 Start

        vs 0.02 26-06-2006: Code clean up

        vs 0.03 17-09-2006: put code on svn repository


TODO: add copyright and license in source code files.
      add doxygen generation into build.

*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <opencv/highgui.h>
#include <set>
#include <iostream>
using namespace std;

#include "blob_filter.h"
#include "window.h"
#include "post_process.h"


int main(int argc, char** argv)
{
	IplImage* sample_image = 0;
	blob_result result;
	float distance;
	char plate_name[100], *file_name = "plate_", *tmp;

	// Input the sample picture.
	const char *filename = (argc >= 2 ? argv[1] : "data/Circles.jpg");
	sample_image = cvLoadImage(filename, -1);
	show_img("Original", sample_image);

	int threshold, minarea, maxarea, opencount;
	bool grayit = true, morpho_operator = true;
	threshold = 100;
	minarea = 500;
	maxarea = 2000;
	opencount = 1;
	result = process_image(sample_image, threshold, minarea,
			       maxarea, opencount, grayit, morpho_operator);

	cout << " blobs number: " << result.blob_count << endl;
	for (int i = 0; i < result.blob_count; ++i) {


		cout << "rect.: " << result.blobs[i].rectangularity <<
			" area: " << result.blobs[i].area <<
			" x_max: " << result.blobs[i].max_x <<
			" y_max: " << result.blobs[i].max_y <<
			" x_min: " << result.blobs[i].min_x <<
			" y_min: " << result.blobs[i].min_y <<
			endl;

		strcpy(plate_name, file_name);
		tmp = plate_name;
		tmp += sizeof(file_name) + 2;
		sprintf(tmp, "%d", i);
		strcat(tmp, ".bmp");
		cout << plate_name << endl;
		test(sample_image, result.blobs[i], plate_name, 0);

	}

	cvReleaseImage(&sample_image);
	return 0;
}
