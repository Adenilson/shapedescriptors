/*******************************************************************************
Purpose: 
Contour extractor, this program retrieves a contour from a threholdable
image. Algorithm steps:
a) Threhold the image
b) Contour following
c) Wrote text files with coordinates of contours (x, y)
d) Create a Scilab script to display each retrieved contour

Author: Adenilson Cavalcanti da Silva savagobr@yahoo.com
License: GPL for non comercial use (contact author for other licensing contracts)
History: vs 0.01 20-05-2005 Donne threshold
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
	                            
	 vs 0.08 22-07-2005 Clean up the code (unified window name 			            management/creation/deallocation)
	 		    Exports contour found image to external file
	 		    
	 vs 0.09 02-08-2005 Calculate and mark the centroid.
	 
	 vs 0.10 03-08-2005 Calculate area, and write to std:io centroid
	 
	 
To-do: User manual, integrate with other programs, maybe Laplace edge detection?

*********************************************************************************/
//Standard libraries
#include <iostream>
#include <fstream>
#include <stdlib.h>
//Class stringstream
#include <sstream>
#include <string.h>
using namespace std;

//OpenCv Stuff
#include <opencv/cv.h>
#include <opencv/highgui.h>

//Stores the found contour
CvSeq* contours = 0;
CvMemStorage* storage = cvCreateMemStorage(0);
//Our images
IplImage *image = 0, *gray = 0, *thres = 0, *cnt_img = 0; 
//image = gray = thres = cnt_img = NULL;

//Interface widgets names
char tbarname[] = "threshold";
int edge_thresh = 250;
const int n_windows = 3, levels = 1, thickness = 1;
char *win_names[] = { "original", "threshold", "contour+centroid" };
typedef enum { ORIGINAL, THRESH, CONTOUR } wnames;


//My point type (if we are using OpenCV, defaults to CvPoint)
#undef M2DPOINT
//#define M2DPOINT
//Aux struct to support a coordinate point (yes, I know that I could use
//CvPoint2D32f but I need some more general struct)
#ifdef M2DPOINT
struct t_point{
  float x;
  float y;
};
typedef t_point m_point;
#else
typedef CvPoint2D32f m_point;
#endif

//Calculates centroid, must receive:
//a) pointer to sequence that holds contours
//b) pointer to a variable that will hold the allocated size of answer vector
//
//Returns a vector with coordinates of centroid of each contour
m_point* calc_centroid(CvSeq *contour, int *size);


//Do threshold in original image
void threshold(int thresh) {

  cvZero(image);
  //cvAdaptiveThreshold(gray, image, max, method, type, param);  
  //cvThreshold( IplImage* src, IplImage* dst, float thresh, float maxvalue,
	//CvThreshType type);
  cvThreshold(gray, thres, thresh, 256, CV_THRESH_BINARY_INV);		
}

//Try to found the contour in thresholded image
int contour_follow() {
  cout << "doing contourfollow" << endl;
  /*The function returns total number of retrieved contours
     int cvFindContours(IplImage *img, CvMemStorage *storage, CvSeq **firstcontour, int headersize, CvContourRetrivalMode mode,CvChainApproxMethod method)	        
    Function parameters:
    img = single channel image IPL_DEPTH_8U

    storage = contour storage location

    firstcontour = pointer to the first contour on the highest level

    headersize = size of sequence header. When used:
                 a) CV_CHAIN_CODE: >= sizeof(CvChain)
		 b) otherwise    : >= sizeof(CvContour) 

    mode = Retrieval mode, can be:
                 a) CV_RETR_EXTERNAL : only the extreme outer contours (list)
		 b) CV_RETR_LIST     : all contours (list)
		 c) CV_RETR_CCOMP    : two level hierarchy (list of connected
		                      components) (list)
		 d) CV_RETR_TREE     : complete hierarchy (tree)

    method = Approximation method:
                 a) CV_CHAIN_CODE    : outputs contours in Freeman code
		 b) CV_CHAIN_APPROX_NONE  : translates all points from chain
		                           code into points (?)
		 c) CV_CHAIN_APPROX_SIMPLE: compresses horizontal, vertical,
		                           and diagonal segments (leaves only
					   their ending points)
		 d) CV_CHAIN_APPROX_TC89_L1 and _TC89_KCOS: are two versions of
		                           Teh-Chin approximation algorithm.
  */
  int ncontours = cvFindContours(thres, storage, &contours, sizeof(CvContour),
				 CV_RETR_LIST/*CV_RETR_TREE*/, CV_CHAIN_APPROX_NONE,
				 cvPoint(0,0));

  cout << "Number of contours found: " << ncontours << endl;
  
  return ncontours;
  
}

//Calculate each contour area
void calc_area(CvSeq *contour) {
  CvSeqReader reader;
  double area;
  int counter = 0;
   
  for(; contour != NULL; contour = contour->h_next) {
    counter++;
    area = cvContourArea(contour);
    if(area < 0)
      area = -area;
      
    cout << "Contour " << counter << " area= " << area << endl;
   
  }  
   
}


//This one marks the centroid of each found contour
void mark_centroid(CvSeq *contour, IplImage *img) {
  CvPoint p;
  CvSeqReader reader;
  float meanx, meany;
  
  for (; contour != NULL; contour = contour->h_next) {
    cvStartReadSeq(contour, &reader);
    
    meanx = meany = 0;
    
    for(int i = 0; i < contour->total; i++) {
      CV_READ_SEQ_ELEM(p, reader);
      meanx += p.x;
      meany += p.y;      
    }         
    
    meanx /= contour->total;
    meany /= contour->total;
    
    //cout << "Centroid contour " << " x= " << meanx << "\t y= " << meany << endl;
    
    cvRectangle(img, cvPoint(meanx-1, meany-1), cvPoint(meanx+1, meany+1), 
			CV_RGB(0, 0, 255), 1);
   
  }
      
}

//Show the contour stored in a sequence
void show_contour(void) {
  CvSeq *_contours = contours;  
  cvZero(cnt_img);    
  cvDrawContours(cnt_img, _contours, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0),
		 levels, thickness, CV_AA);

  mark_centroid(_contours, cnt_img);
  cvShowImage(win_names[CONTOUR], cnt_img);	  
    
}

// define a trackbar callback
void on_trackbar(int h)
{
  threshold(h);
  cvShowImage(win_names[THRESH], thres);
  contour_follow();            
  show_contour();  
}

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

//Write the scilab code into a text file
void print_contour4(string img_file_name) {
  CvSeq* contourPtr = contours;
  CvSeqReader reader;
  int n_point = 0;
  int c_contour = 0;
  CvPoint p;  
  ofstream f_contour;
  string filename; 

  for (; contourPtr != NULL; contourPtr = contourPtr->h_next) {
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
      //CV_READ_SEQ_ELEM(p, reader);  	
      CV_REV_READ_SEQ_ELEM(p, reader);
      f_contour << p.x << " " << p.y << endl;  		
    }

    f_contour.close();

  }

  sci_prog(c_contour, img_file_name);
}

//Create window resources to display images
void win_alloc(int num_win, char** names) {
  for(int i = 0; i < num_win; ++i)
    cvNamedWindow(names[i], 0);
}

//Deallocate window resources
void win_free(int num_win, char** names) {
  for(int i = 0; i < num_win; ++i)
    cvDestroyWindow(names[i]);
}

//Main function (duh!)
int main(int argc, char* argv[]) {

  char *filename = (argc >= 2 ? argv[1] : (char*)"escamas.bmp");
  if( (image = cvLoadImage( filename, 1)) == 0 ) {
    cout << "Can't find image \"escamas.bmp\". Please supply the image." << endl;
    return -1;
  }
  
  bool interactive = true;
  int thres_value = 140;
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
  
  //Non interactive mode
  if(!interactive) {
    //Do the threshold
    threshold(thres_value);
    //Try to do contour following
    contour_follow();        
    //Plots contours in a IplImage
    cvDrawContours(cnt_img, contours, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0),
		 levels, thickness, CV_AA);
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

  //Save the image
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
  print_contour4(filename);
  
  //Calculate each contour area and centroid 
  calc_area(contours);
  
  m_point *dumbo = NULL;
  int thasize = 0;
  dumbo = calc_centroid(contours, &thasize);
  ofstream fout("centroid.txt");
  for(int k = 0; k < thasize-1; ++k)
    //cout << "Contour " << k << " centroid= " << dumbo[k].x << " " << dumbo[k].y << endl;
    fout << dumbo[k].x << "     " << dumbo[k].y << endl;
    
  delete [] dumbo;

  return 0;
}

//Calculate centroid
//void calc_centroid(CvSeq *contour, m_point *answer, int *size) {
m_point* calc_centroid(CvSeq *contour, int *size) {

  m_point *answer = NULL;
  CvPoint p;
  CvSeqReader reader;
  CvSeq *temp = NULL;
  float meanx, meany;
  int counter = 0;
    
  //Discover how many contours we do have and allocate memory  
  for(temp = contour; temp != NULL; temp = temp->h_next)
    counter++;
  
  *size = counter;
  answer = new m_point[counter];
  
  //Calculate each centroid
  for (counter = 0; counter < *size; ++counter) {

    cvStartReadSeq(contour, &reader);    
    meanx = meany = 0;
    
    for(int i = 0; i < contour->total; i++) {
      CV_READ_SEQ_ELEM(p, reader);
      meanx += p.x;
      meany += p.y;      
    }         
    
    meanx /= contour->total;
    meany /= contour->total;
    answer[counter].x = meanx;
    answer[counter].y = meany;    
    //*(answer + counter)    
    contour = contour->h_next;
  }
  
  return answer;
  
}

