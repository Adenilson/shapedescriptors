//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include "output.h"
#include "descriptors.h"
#include <fstream>
#include <iostream>
#include <string>
//Class stringstream
#include <sstream>

using namespace std;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Create a scilab program to handle exported text files with coordinates of
//contours
void sci_prog(int n_contour, string img_file_name) {

  string filename, buffer;
  typedef enum { IMG_NAME, STR_CLOS, LST_NXT, LST_END, CONTOUR_LIST, PRG_BULK } prog_step;
  const char* prog[] = {"img_name ='", "'", ",", ");", "contour_name= list(", 
                        "stacksize(13000000); \n \
                         function D = showcontour() \n \
                         Img = imread(img_name); \n \
                         n = size(contour_name); \n \
                         for i = 1:n, \n \
                           A = read(contour_name(i), -1, 2); \n \
                           [row, col] = size(A); \n \
                           x = A(1:row, 1:1); \n \
                           y = A(1:row, 2:2); \n \
                           xset(\"window\", i); \n \
                           imshow(unfollow(x, y, size(Img))) \n \
                         end; \n \
                         D = 1 \n \
                         endfunction;" };

  ofstream fout;
  fout.open("plotter.sci");

  fout << prog[IMG_NAME] << img_file_name << prog[STR_CLOS] << endl;

  for(int i = 1; i <= n_contour; ++i) {
       
    stringstream i2s;
    i2s << "_contour_" << i << ".txt";
    filename = img_file_name; filename += i2s.str(); 
    buffer += prog[STR_CLOS];
    buffer += filename;
    buffer += prog[STR_CLOS];
    
    if(i < n_contour)
      buffer += prog[LST_NXT];
    else
      buffer += prog[LST_END];
    
  }
  fout << prog[CONTOUR_LIST] << buffer << endl;
  fout << prog[PRG_BULK] << endl;
}

//------------------------------------------------------------------------------
//Write the scilab code into a text file
//void print_contour4(string img_file_name, CvSeq *contours) {
void print_contour(char *img_file_name, CvSeq *contours, bool mthreshold, float diam_thres) {

  CvSeq* contourPtr = contours;
  CvSeqReader reader;
  int n_point = 0;
  int c_contour = 0;
  CvPoint p;  
  ofstream f_contour;
  string filename; 
  float *diameters = NULL;
  int d_size = 0;
  
  if(mthreshold) {
    diameters = calc_diam(contours, &d_size);  

    for (int i = 0; i < d_size; ++i)    
      if(diameters[i] >= diam_thres) {
    //if(true) {
  
        cvStartReadSeq(contourPtr, &reader);
        n_point = contourPtr->total;
  
        cout << n_point << endl;
        ++c_contour;
    
        stringstream i2s;
        i2s << "_contour_" << c_contour << ".txt";
        filename = img_file_name; filename += i2s.str();    
        cout << filename << endl;
        f_contour.open(filename.c_str());
 
        for (int i = 0; i < n_point; ++i) {
          CV_READ_SEQ_ELEM(p, reader);  	
          //CV_REV_READ_SEQ_ELEM(p, reader);
          f_contour << p.x << " " << p.y << endl;  		
        }

        f_contour.close();
        contourPtr = contourPtr->h_next;
      
      }
      delete [] diameters;
  }
  else {
    
    for(; contourPtr != NULL; contourPtr = contourPtr->h_next) {
      cvStartReadSeq(contourPtr, &reader);
      n_point = contourPtr->total;

      cout << n_point << endl;
      ++c_contour;
    
      stringstream i2s;
      i2s << "_contour_" << c_contour << ".txt";
      filename = img_file_name; filename += i2s.str();    
      cout << filename << endl;
      f_contour.open(filename.c_str());

      for (int i = 0; i < n_point; ++i) {
        CV_READ_SEQ_ELEM(p, reader);  	
        //CV_REV_READ_SEQ_ELEM(p, reader);
        f_contour << p.x << " " << p.y << endl;  		
      }

      f_contour.close();

    }  
  
  
  }    
  
  sci_prog(c_contour, img_file_name);
  
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
