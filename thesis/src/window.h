/**
 * @file   window.h
 * @author Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2005
 * @date   Sometime in 2005.
 *
 * @brief  OpenCV auxiliary image display code, there help
 *        in widget resource management.
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
