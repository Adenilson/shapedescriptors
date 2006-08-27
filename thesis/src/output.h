//------------------------------------------------------------------------------
#ifndef _OUTPUT_H_
#define _OUTPUT_H_
//------------------------------------------------------------------------------
#include "base.h"

//Write the scilab code into a text file
//vs 0.02: 21-08-2005
void print_contour(char *img_file_name, CvSeq *contours, bool mthreshold = false, float diam_thres = 6);

//------------------------------------------------------------------------------
#endif
