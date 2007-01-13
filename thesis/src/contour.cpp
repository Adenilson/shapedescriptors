/** @file
 *
 * Contour code module.
 *
 *
 * Copyright 2005
 * @author Adenilson Cavalcanti <savagobr@yahoo.com>
 *
 * @version
 */

#include "contour.h"
#include <iostream>
using namespace std;


void plot_contour(IplImage *target, CvSeq *contours, int thickness, int levels)
{

	cvDrawContours(target, contours, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0),
		       levels, thickness, CV_AA);
}

//Try to found the contour in thresholded image
//int contour_follow(IplImage *thres, CvMemStorage* storage, CvSeq **contours)
CvSeq *contour_follow(IplImage *thres, CvMemStorage* storage, int *ncontour)
{
	cout << "doing contourfollow" << endl;
	CvSeq *contours = NULL;
	/*The function returns total number of retrieved contours
	  int cvFindContours(IplImage *img, CvMemStorage *storage, CvSeq **firstcontour, int headersize, CvContourRetrivalMode mode,CvChainApproxMethod method)

	  Function parameters:
	  img = single channel image IPL_DEPTH_8U

	  storage = contour storage location

	  firstcontour = pointer to the first contour on the highest level

	  headersize = size of sequence header. When used:
	  a) CV_CHAIN_CODE: >= sizeof(CvChain)
	  b) otherwise    : >= sizeof(CvContour)

	  mode = Retrieval mode, can be:
	  a) CV_RETR_EXTERNAL : only the extreme outer contours (list)
	  b) CV_RETR_LIST     : all contours (list)
	  c) CV_RETR_CCOMP    : two level hierarchy (list of connected
	  components) (list)
	  d) CV_RETR_TREE     : complete hierarchy (tree)

	  method = Approximation method:
	  a) CV_CHAIN_CODE    : outputs contours in Freeman code
	  b) CV_CHAIN_APPROX_NONE  : translates all points from chain
	  code into points (?)
	  c) CV_CHAIN_APPROX_SIMPLE: compresses horizontal, vertical,
	  and diagonal segments (leaves only
	  their ending points)
	  d) CV_CHAIN_APPROX_TC89_L1 and _TC89_KCOS: are two versions of
	  Teh-Chin approximation algorithm.
	*/
	*ncontour = cvFindContours(thres, storage, &contours, sizeof(CvContour),
				   CV_RETR_LIST/*CV_RETR_TREE*/,
				   CV_CHAIN_APPROX_NONE,
				   cvPoint(0,0));

	cout << "Number of contours found: " << *ncontour << endl;

	return contours;

}

//This one marks the centroid of each found contour
void mark_centroid(CvSeq *contour, IplImage *img)
{
	CvPoint p;
	CvSeqReader reader;
	float meanx, meany;

	for (; contour != NULL; contour = contour->h_next) {
		cvStartReadSeq(contour, &reader);

		meanx = meany = 0;

		for (int i = 0; i < contour->total; i++) {
			CV_READ_SEQ_ELEM(p, reader);
			meanx += p.x;
			meany += p.y;
		}

		meanx /= contour->total;
		meany /= contour->total;

		cvRectangle(img, cvPoint((int) meanx-1, (int) meany-1),
			    cvPoint((int) meanx+1, (int) meany+1),
			    CV_RGB(0, 0, 255), 1);

	}

}

