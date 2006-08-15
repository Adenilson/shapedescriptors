//------------------------------------------------------------------------------
#ifndef _OUTPUT_H_
#define _OUTPUT_H_
//------------------------------------------------------------------------------
#include "base.h"

//Create a scilab program to handle exported text files with coordinates of
//contours
//void sci_prog(int n_contour, string img_file_name);

//Write the scilab code into a text file
void print_contour(char *img_file_name, CvSeq *contours);

//------------------------------------------------------------------------------
#endif