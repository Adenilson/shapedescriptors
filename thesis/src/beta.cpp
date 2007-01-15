/*******************************************************************************
Purpose:
Contour extractor, this program retrieves a contour from a threholdable
image. Algorithm steps:
a) Threhold the image
b) Contour following and centroid calculus
c) Wrote text files with coordinates of contours (x, y)
d) Create a Scilab script to display each retrieved contour
e) Write external file with centroid of each contour

Author: Adenilson Cavalcanti da Silva savagobr@yahoo.com
License: GPL for non comercial use (contact author for other licensing contracts)
History:
    vs 0.01 20-05-2005 Donne threshold

    vs 0.02 29-05-2005 Donne contour following

    vs 0.03 04-06-2005 Succesfully access to CvSeq* points (thanks
                      Sentillay, you guy rock!)

    vs 0.04 06-06-2005 Donne text file coordinates export, dynamically
                      creation of Scilab script.

    vs 0.05 16-06-2005 Cleaned source code, commented it too.

    vs 0.06 05-07-2005 Added support to receive threshold at calling in CLI
                           (e.g. prog.out figure.bmp 160)
                            Changed the scilab code, the display of contour images
                           are optional.

    vs 0.07 19-07-2005 Threshold controled by bar in threholded image window

    vs 0.08 22-07-2005 Clean up the code (unified window name management/creation/deallocation)
             Exports contour found image to external file

    vs 0.09 02-08-2005 Calculate and mark the centroid.

    vs 0.10 03-08-2005 Calculate area, and write to std:io centroid

    vs 0.11 05-08-2005 Now writes centroid in a external text file
                        Started big rewrite to break code in serveral files

    vs 0.11 09-08-2005 Finished 'breaking code' transition (all seems to work)
                       Wrote a makefile (neat!)

    vs 0.12 21-08-2005 Added diameter calculus
    	    22-08-2005 Added dilation/erosion for closing of contours

    vs 0.13 18-10-2005 Only prints descriptors of scales which diameter is bigger than a give threshold


To-do: Voronoi tesselation, User manual, integrate with other programs, maybe Laplace edge detection?

*********************************************************************************/
//Standard libraries
#include <iostream>
#include <fstream>
#include <stdlib.h>
//OpenCV Stuff
#include <opencv/cv.h>
#include <opencv/highgui.h>
//Class stringstream
#include <sstream>
#include <string.h>

//My own aux functions
#include "base.h"
#include "contour.h"
#include "window.h"
#include "vision.h"
#include "output.h"
#include "descriptors.h"


using namespace std;

//Param threshold and external filename of contours centroid
int edge_thresh = 250;
int diameter_thresh = 500;
char file_centroid[] = "centroid.txt";
char file_area[] = "area.txt";
char file_ratio[] = "ratio_centroid.txt";
char file_diam[] = "diameter.txt";

//Minimum diameter
float diam_thres = 7;


/* Define this if you need contours coordinates written
 * in external files (1 file for each contour).
 */
#undef NEED_CONTOUR_COORDINATES

//Stores the found contour
CvSeq* contours = 0;
CvMemStorage* storage = cvCreateMemStorage(0);
//Our images
IplImage *image = 0, *gray = 0, *thres = 0, *cnt_img = 0;
//Interface widgets names
char tbarname[] = "threshold";
char tbardiameter[] = "diameter";
int num_cell = 0;
const int n_windows = 3;
char *win_names[] = { "original", "threshold", "contour+centroid" };
typedef enum { ORIGINAL, THRESH, CONTOUR } wnames;

//Write centroid of each contour in external file
//ps: the last one is the external contour
bool write_centroid(CvSeq* contours, char *filename, float diam,
		    float *diameters);


//Write area of each contour in external file
//ps: idem
bool write_area(CvSeq* contours, char *filename, float diam,
		float *diameters);

//Write diameter of each contour in external file
//ps: idem
//bool write_diam(CvSeq* contours, char *filename, float diam);
bool write_diam(CvSeq* contours, char *filename, float diam,
		float *diameters, int thasize);

//Show the contour stored in a sequence
void show_contour(void);

// define a trackbar callback
void on_trackbar(int h);
void on_track_diameter(int h);

//Main function (duh!)
int main(int argc, char* argv[])
{

	char *filename = (argc >= 2 ? argv[1] : (char*)"escamas.bmp");
	if ((image = cvLoadImage( filename, 1)) == 0) {
		cout << "Can't find image \"escamas.bmp\". Please supply an image." <<
			"\n\n" << "$program image_file_name <mode> <threshold_value>" <<
			" <minimal_diameter>" <<
			"\nwhere:" <<
			"\tmode = batch (non visual execution)\n" <<
			"\tthreshold_value = value which pixels above will be regarded" <<
			"\n\t\tas background\n" <<
			"\tminimal_diameter = shape diameter of valid objects" <<
			endl;
		return -1;
	}

	bool interactive = true;
	/* FIXME: 2 different groups of variables (diam_thres + thres_min_diameter)
	 * to handle 2 parameters is not good. Find a way to use only 2 vars.
	 * I think that casting can solve the issue.
	 */
	int thres_value = 160;
	int thres_min_diameter = 30;
	string temp;
	int pos = 0;
	float *diameters = NULL;
	int d_size = 0;

	for (int i = 2; i < argc; ++i) {
		temp = argv[i];
		if (temp == "batch")
			interactive = false;
		else if (i == 3)
			thres_value = atoi(argv[i]);
		else if (i == 4)
			thres_min_diameter = atoi(argv[i]);
	}

	//Allocate image structure resource
	gray = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
	thres = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
	cnt_img = cvCreateImage(cvSize(thres->width, thres->height), IPL_DEPTH_8U, 3);

	//Convert to grayscale,
	cvCvtColor(image, gray, CV_BGR2GRAY);

	//Non interactive mode
	if (!interactive) {
		//Do the threshold
		threshold(thres_value, gray, thres);
		//Morphology operations to ensure closing of contour
		dilation(thres, thres);
		erosion(thres, thres);
		//Try to do contour following
		contours = contour_follow(thres, storage, &num_cell);
		//Plots contours in a IplImage
		plot_contour(cnt_img, contours);
		//FIXME: 2 variables, 1 parameters... its evil.
		diam_thres = thres_min_diameter;
	} else {
		//Alocates window resources and reloads original image
		win_alloc(n_windows, win_names);
		image = cvLoadImage(filename, 1);
		//Show original image
		cvShowImage(win_names[ORIGINAL], image);

		//Create a window and toolbar
		cvNamedWindow(win_names[THRESH], 1);
		cvCreateTrackbar(tbarname, win_names[THRESH], &thres_value,
				 edge_thresh, on_trackbar);
		cvCreateTrackbar(tbardiameter, win_names[CONTOUR], &thres_min_diameter,
				 diameter_thresh, on_track_diameter);
		//Activates callback (which shows the image)
		on_trackbar(thres_value);
		on_track_diameter(thres_min_diameter);

		// Wait for a key stroke; the same function arranges events processing
		cvWaitKey(0);

		//Frees allocated resources
		win_free(n_windows, win_names);
	}

	//Save the image with contours
	temp = filename;
	pos = temp.size();
	pos -= 4;
	temp = temp.substr(0, pos);
	temp += "contour";
	temp += ".png";
	cvSaveImage(temp.c_str(), cnt_img);

	//Free allocated resources
	cvReleaseImage(&image);
	cvReleaseImage(&gray);
	cvReleaseImage(&thres);
	cvReleaseImage(&cnt_img);

#ifdef NEED_CONTOUR_COORDINATES
	/* Prints files with contour coordinates, we want diameter threshold
	 * (only contours with diameter greater than 7 pixels).
	 */
	print_contour(filename, contours, true, diam_thres);
#endif

	//Calculates all diameters. Its the threshold descriptor for others!
	diameters = calc_diam(contours, &d_size);

	//Write external file with each contour centroid
	write_centroid(contours, file_centroid, diam_thres, diameters);
	//Write external file with each contour area
	write_area(contours, file_area, diam_thres, diameters);
	//Write external file with each contour diameter
	write_diam(contours, file_diam, diam_thres, diameters, d_size);

	return 0;
}

void show_contour(void)
{
	CvSeq *_contours = contours;
	cvZero(cnt_img);
	int size;
	float *diameters;
	diameters = calc_diam(contours, &size);

	//Plot a contour in an image and marks its centroid
	plot_contour(cnt_img, _contours);
	mark_centroid(_contours, cnt_img, diameters, diam_thres);
	cvShowImage(win_names[CONTOUR], cnt_img);

	delete [] diameters;
	diameters = NULL;
}


void on_trackbar(int h)
{
	threshold(h, gray, thres);
	dilation(thres, thres);//, 1, 2);
	erosion(thres, thres);//, 1, 2);
	cvShowImage(win_names[THRESH], thres);
	//Try to found the contour in thresholded image and display it
	contours = contour_follow(thres, storage, &num_cell);
	show_contour();
}


void on_track_diameter(int h)
{
	diam_thres = (float) h;
	show_contour();
}

//Write max/min, max & min distances from centroid of each contour in external file
//ps: the last one is the external contour
bool write_dist(CvSeq* contours, char *filename, float diam,
		float *diameters, m_point *centroid, int size);

bool write_centroid(CvSeq* contours, char *filename, float diam,
		    float *diameters)
{
	m_point *dumbo = NULL;
	bool result = true;
	int thasize = 0;

	dumbo = calc_centroid(contours, &thasize);
	write_dist(contours, file_ratio, diam, diameters, dumbo, thasize);
	//ratio_dist(contours, dumbo, thasize, file_ratio);


	try {
		ofstream fout(filename);
		for (int k = 0; k < thasize; ++k)
			if (diameters[k] >= diam)
				fout << dumbo[k].x << "     " << dumbo[k].y << endl;

	}
	catch(...) {
		delete [] dumbo;
		return false;
	}
	delete [] dumbo;
	return result;
}

bool write_area(CvSeq* contours, char *filename, float diam,
		float *diameters)
{
	float *dumbo = NULL;
	bool result = true;
	int thasize = 0;
	dumbo = calc_area(contours, &thasize);

	try {
		ofstream fout(filename);
		for (int k = 0; k < thasize; ++k)
			if (diameters[k] >= diam)
				fout << dumbo[k] << endl;

	}
	catch(...) {
		delete [] dumbo;
		return false;
	}
	delete [] dumbo;
	return result;
}

bool write_diam(CvSeq* contours, char *filename, float diam,
		float *diameters, int thasize)
{
	float *dumbo = NULL;
	bool result = true;
	//int thasize = 0;
	//dumbo = calc_diam(contours, &thasize);

	try {
		ofstream fout(filename);
		for (int k = 0; k < thasize; ++k)
			if (diameters[k] >= diam)
				fout << diameters[k] << endl;
	}
	catch(...) {
		delete [] dumbo;
		return false;
	}
	delete [] dumbo;
	return result;
}


//Write centroid of each contour in external file
//ps: the last one is the external contour
bool write_dist(CvSeq* contours, char *filename, float diam,
		float *diameters, m_point *centroid, int size)
{

	d3point *distances = NULL;
	bool result = true;

	distances = new d3point[size];
	ratio_dist(contours, centroid, size, distances);
	//calc_centroid(contours, &thasize);
	//ratio_dist(contours, dumbo, thasize, file_ratio);

	try {
		ofstream fout(filename);
		for (int k = 0; k < size; ++k)
			if (diameters[k] >= diam)
				fout << distances[k].x << "     "
				     << distances[k].y << "     "
				     << distances[k].z << endl;
	}
	catch(...) {
		delete [] distances;
		return false;
	}
	delete [] distances;
	return result;

}
