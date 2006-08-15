/*******************************************************************************
Purpose: 
Contour extractor, this program retrieves a contour from a threholdable
image. Algorithm steps:
a) Threhold the image
b) Contour following and centroid calculus
c) Wrote text files with coordinates of contours (x, y)
d) Create a Scilab script to display each retrieved contour
e) Write external file with centroid of each contour

Author: Adenilson Cavalcanti da Silva savagobr@yahoo.com
License: GPL for non comercial use (contact author for other licensing contracts)
History: 
    vs 0.01 20-05-2005 Donne threshold
         
    vs 0.02 29-05-2005 Donne contour following
    
    vs 0.03 04-06-2005 Succesfully access to CvSeq* points (thanks 
                      Sentillay, you guy rock!)
                      
    vs 0.04 06-06-2005 Donne text file coordinates export, dinamically 
                      creation of Scilab script.
                      
    vs 0.05 16-06-2005 Cleaned source code, commented it too.

    vs 0.06 05-07-2005 Added support to receive threshold at calling in CLI 
                           (e.g. prog.out figure.bmp 160)
                            Changed the scilab code, the display of contour images
                           are optional.
                           
    vs 0.07 19-07-2005 Threshold controled by bar in threholded image window
                               
    vs 0.08 22-07-2005 Clean up the code (unified window name management/creation/deallocation)
             Exports contour found image to external file
             
    vs 0.09 02-08-2005 Calculate and mark the centroid.
    
    vs 0.10 03-08-2005 Calculate area, and write to std:io centroid
    
    vs 0.11 05-08-2005 Now writes centroid in a external text file
                        Started big rewrite to break code in serveral files
    
    vs 0.11 09-08-2005 Finished 'breaking code' transition (all seems to work)
                       Wrote a makefile (neat!)
                       
    vs 0.12 21-08-2005 Added diameter calculus
    
    
To-do: Voronoi tesselation, User manual, integrate with other programs, maybe Laplace edge detection?

*********************************************************************************/
//Standard libraries
#include <iostream>
#include <fstream>
#include <stdlib.h>
//OpenCV Stuff
#include <opencv/cv.h>
#include <opencv/highgui.h>
//Class stringstream
#include <sstream>
#include <string.h>

//My own aux functions
#include "base.h"
#include "contour.h"
#include "window.h"
#include "vision.h"
#include "output.h"
#include "descriptors.h"


using namespace std;

//Param threshold and external filename of contours centroid
int edge_thresh = 250;
char file_centroid[] = "centroid.txt";
char file_area[] = "area.txt";
char file_ratio[] = "ratio_centroid.txt";
char file_diam[] = "diameter.txt";

//Stores the found contour
CvSeq* contours = 0;
CvMemStorage* storage = cvCreateMemStorage(0);
//Our images
IplImage *image = 0, *gray = 0, *thres = 0, *cnt_img = 0; 
//Interface widgets names
char tbarname[] = "threshold";
int num_cell = 0;
const int n_windows = 3;
char *win_names[] = { "original", "threshold", "contour+centroid" };
typedef enum { ORIGINAL, THRESH, CONTOUR } wnames;

//Write centroid of each contour in external file
//ps: the last one is the external contour
bool write_centroid(CvSeq* contours, char *filename);

//Write area of each contour in external file
//ps: idem
bool write_area(CvSeq* contours, char *filename);

//Write diameter of each contour in external file
//ps: idem
bool write_diam(CvSeq* contours, char *filename);

//Show the contour stored in a sequence
void show_contour(void);

// define a trackbar callback
void on_trackbar(int h);

//Main function (duh!)
int main(int argc, char* argv[]) {

  char *filename = (argc >= 2 ? argv[1] : (char*)"escamas.bmp");
  if( (image = cvLoadImage( filename, 1)) == 0 ) {
    cout << "Can't find image \"escamas.bmp\". Please supply the image." << endl;
    return -1;
  }
  
  bool interactive = true;
  int thres_value = 160;
  string temp;
  int pos = 0;
  
  for(int i = 2; i < argc; ++i) {
    temp = argv[i];
    if(temp == "batch")
      interactive = false;
    else if(i == 2) {
      thres_value = atoi(argv[2]);
        //do_thres = true;        
    }  
  }
  
  //Allocate image structure resource
  gray = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
  thres = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
  cnt_img = cvCreateImage(cvSize(thres->width, thres->height), IPL_DEPTH_8U, 3);
  
  //Convert to grayscale,
  cvCvtColor(image, gray, CV_BGR2GRAY);
  /*
  cvNamedWindow("gray", 1);
  cvShowImage("gray", gray);
  */
  //Non interactive mode
  if(!interactive) {
    //Do the threshold    
    threshold(thres_value, gray, thres);
    //Try to do contour following    
    contours = contour_follow(thres, storage, &num_cell);              
    //Plots contours in a IplImage
    plot_contour(cnt_img, contours);
  }  
  else {
    //Alocates window resources and reloads original image
    win_alloc(n_windows, win_names);
    image = cvLoadImage(filename, 1);    
    //Show original image
    cvShowImage(win_names[ORIGINAL], image);   
    
    //Create a window and toolbar
    cvNamedWindow(win_names[THRESH], 1);
    cvCreateTrackbar(tbarname, win_names[THRESH], &thres_value, edge_thresh, on_trackbar);
    //Activates callback (which shows the image)
    on_trackbar(thres_value);
     
    //Now show the contour
    show_contour();
    
    // Wait for a key stroke; the same function arranges events processing
    cvWaitKey(0);              
    
    //Frees allocated resources
    win_free(n_windows, win_names);        
  }

  //Save the image with contours
  temp = filename;
  pos = temp.size();
  pos -= 4;
  temp = temp.substr(0, pos);  
  temp += "contour";
  temp += ".png";
  cvSaveImage(temp.c_str(), cnt_img);  
  
  //Free allocated resources
  cvReleaseImage(&image);
  cvReleaseImage(&gray);
  cvReleaseImage(&thres);
  cvReleaseImage(&cnt_img);   
  
  //Prints files with contour coordinates
  print_contour(filename, contours);
  
  //Write external file with each contour centroid
  write_centroid(contours, file_centroid);  
  
  //Write external file with each contour area
  write_area(contours, file_area);  

  //Write external file with each contour diameter
  write_diam(contours, file_diam);
   
  return 0;
}

void show_contour(void) {
  CvSeq *_contours = contours;  
  cvZero(cnt_img);    
  
  //Plot a contour in a image and marks its centroid
  plot_contour(cnt_img, _contours);
  mark_centroid(_contours, cnt_img);
  cvShowImage(win_names[CONTOUR], cnt_img);    
    
}


void on_trackbar(int h)
{
  threshold(h, gray, thres);
  cvShowImage(win_names[THRESH], thres);
  //Try to found the contour in thresholded image and display it    
  contours = contour_follow(thres, storage, &num_cell);              
  show_contour();  
}

bool write_centroid(CvSeq* contours, char *filename) {
  m_point *dumbo = NULL;
  bool result = true;
  int thasize = 0;
  dumbo = calc_centroid(contours, &thasize);  
  ratio_dist(contours, dumbo, thasize, file_ratio);      
  
  try {
    ofstream fout(filename);
    for(int k = 0; k < thasize; ++k)
      fout << dumbo[k].x << "     " << dumbo[k].y << endl;  
    //cout << "Contour " << k << " centroid= " << dumbo[k].x << " " << dumbo[k].y << endl;    
  }
  catch(...) {
    delete [] dumbo;
    return false;       
  }
  delete [] dumbo;
  return result;  
}

bool write_area(CvSeq* contours, char *filename) {
  float *dumbo = NULL;
  bool result = true;
  int thasize = 0;
  dumbo = calc_area(contours, &thasize);
      
  try {
    ofstream fout(filename);
    for(int k = 0; k < thasize; ++k)
      fout << dumbo[k] << endl;  
    //cout << "Contour " << k << " centroid= " << dumbo[k].x << " " << dumbo[k].y << endl;    
  }
  catch(...) {
    delete [] dumbo;
    return false;       
  }
  delete [] dumbo;
  return result;  
}

bool write_diam(CvSeq* contours, char *filename) {
  float *dumbo = NULL;
  bool result = true;
  int thasize = 0;
  dumbo = calc_diam(contours, &thasize);
      
  try {
    ofstream fout(filename);
    for(int k = 0; k < thasize; ++k)
      fout << dumbo[k] << endl;  
    //cout << "Contour " << k << " centroid= " << dumbo[k].x << " " << dumbo[k].y << endl;    
  }
  catch(...) {
    delete [] dumbo;
    return false;       
  }
  delete [] dumbo;
  return result;  
}

