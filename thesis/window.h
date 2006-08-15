//------------------------------------------------------------------------------
#ifndef _MWINDOW_H_
#define _MWINDOW_H_
//------------------------------------------------------------------------------
#include "base.h"
//------------------------------------------------------------------------------

//Create window resources to display images
inline void win_alloc(int num_win, char** names) {
  for(int i = 0; i < num_win; ++i)
    cvNamedWindow(names[i], 0);
};
//------------------------------------------------------------------------------
//Deallocate window resources
inline void win_free(int num_win, char** names) {
  for(int i = 0; i < num_win; ++i)
    cvDestroyWindow(names[i]);
};



//------------------------------------------------------------------------------
#endif
