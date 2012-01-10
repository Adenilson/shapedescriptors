/**
 * @file   window.h
 * @author Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2005
 * @date   Sometime in 2005.
 *
 * @brief  OpenCV auxiliary image display code, there help
 *        in widget resource management.
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

#ifndef _MWINDOW_H_
#define _MWINDOW_H_


#include "base.h"


/** Create window resources to display images
 *
 *
 * @param num_win Number of windows to be allocated.
 * @param names Array of strings with window names.
 */
inline void win_alloc(int num_win, char** names)
{
	for (int i = 0; i < num_win; ++i)
		cvNamedWindow(names[i], 0);
};


/** Deallocate window resources
 *
 *
 * @param num_win Number of windows preallocated.
 * @param names Array of strings with window names.
 */
inline void win_free(int num_win, char** names)
{
	for (int i = 0; i < num_win; ++i)
		cvDestroyWindow(names[i]);
};


#endif
