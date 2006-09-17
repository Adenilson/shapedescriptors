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


TODO: add copyright and license in source code files.

*******************************************************************************/


#include <stdlib.h>

#include <opencv/highgui.h>
#include <set>
#include <iostream>
using namespace std;

#include "blob_filter.h"
#include "window.h"

/** Square of number
 *
 * @param i A number to be squared.
 *
 * @return Squared number.
 *
 * TODO: make it template based.
 */
inline float square(float i)
{
	return i * i;
}



int main(int argc, char** argv)
{
	IplImage* sample_image = 0;
	blob_result result;
	float distance;

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
	result = process_image(sample_image, threshold, minarea,
			       maxarea, opencount, grayit, morpho_operator);

	cvReleaseImage(&sample_image);
	cout << "ALL DONE" << endl;

	cout << " blobs number: " << result.blob_count << endl;
	for (int i = 0; i < result.blob_count; ++i) {
		/* XXX: Move this code to a proper function */
		distance = square(result.blobs[i].min_x -
				  result.blobs[i].max_x) +
			square(result.blobs[i].min_y -
			       result.blobs[i].max_y);
		distance = sqrt(distance);
		distance = result.blobs[i].area / distance;

		cout << "distance: " << distance <<
			" area: " << result.blobs[i].area <<
			" x_max: " << result.blobs[i].max_x <<
			" y_max: " << result.blobs[i].max_y <<
			" x_min: " << result.blobs[i].min_x <<
			" y_min: " << result.blobs[i].min_y <<
			endl;
	}

	return 0;
}
