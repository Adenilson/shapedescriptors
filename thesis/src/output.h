/** @file
 *
 * Scilab code generation module.
 *
 *
 * Copyright 2005
 * @author Adenilson Cavalcanti <savagobr@yahoo.com>
 *
 * @version
 */


#ifndef _OUTPUT_H_
#define _OUTPUT_H_


#include "base.h"


/** The function writes out a scilab program script to display contours found
 * with function 'contour_follow'. This program depends on SIP (Scilab
 * Image Package) which is no longer mantained (maybe I should drop this
 * function altogether).
 * Second version of this function dates back to 21-08-2005.
 *
 *
 * @param img_file_name Original contours image filename (used to append in
 *                      name of output contour files).
 * @param contours Vector with sequence of contours.
 * @param mthreshold Set flag to true if you want to filter contours based
 *                   its diameter.
 * @param diam_thres Minimal allowed diameter of contour.
 */
void print_contour(char *img_file_name, CvSeq *contours, bool mthreshold = false, float diam_thres = 6);


#endif
