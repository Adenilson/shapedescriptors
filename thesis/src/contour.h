/**
 * @file   contour.h
 * @author  Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2005
 * @date   Sometime in 2005.
 *
 * @brief  Contour following code module.
 *
 *
 */

/*  Copyright (C) 2005  Adenilson Cavalcanti <cavalcantii@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; by version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef _CONTOUR_H_
#define _CONTOUR_H_


#include "base.h"

/** Contour following function. Given a image, it will find the contours
 * coordinates of objects/shapes in this image. We use OpenCV to this
 * task, 'cvFindContours'.
 *
 *
 * @param thres Image with contours, must have 1 layer and be 8bit based.
 *              A thresholded image in most of cases.
 * @param storage OpenCV auxiliary storage variable, create one with
 *                CvMemStorage* storage = cvCreateMemStorage(0);
 * @param ncontour Pointer to variable that will hold number of found contours.
 *
 * @return A OpenCV sequence with contours.
 */
CvSeq *contour_follow(IplImage *thres, CvMemStorage* storage, int *ncontour);


/** After you have already got the contours, this function will make it easy
 * to create an image with this contours.
 *
 * @param target A OpenCV image (3 channels, 8bit) structure in which we draw
 *               the contours.
 * @param contours Sequence with shape contours.
 * @param thickness Line thickness when drawing contours.
 * @param levels I quite don't remember what this is for...
 *
 */
void plot_contour(IplImage *target, CvSeq *contours,
		  int thickness = 1, int levels = 1);


/** Use this function to draw found shape centroids in an image (normally
 * one that already has the contours painted with 'plot_contour').
 *
 * @param contour A OpenCV sequence with several contours, most of case you will
 *                create this using 'contour_follow'.
 * @param img A OpenCV image (3 channels, 8bit) structure in which we draw
 *               the contours.
 * @param contour_diameters A vector with contours diameters, it can be used to set
 *                          a 'minimal diameter' threshold to draw the centroids.
 * @param diam_thres Minimal diameter threshold, shapes with diameter bigger than
 *                   diam_thres will be draw.
 */
void mark_centroid(CvSeq *contour, IplImage *img, float *contour_diameters = NULL,
		   float diam_thres = 30.0);


#endif
