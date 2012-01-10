/**
 * @file   output.h
 * @author Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2005
 * @date Sometime in 2005
 *
 * @brief Scilab code generation module (deprecated).
 *
 * Since we are targeting to do all calculus in C++, probably
 * this module is going to be removed from project.
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
