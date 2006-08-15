
#include <iostream>
#include <fstream>
#include <stdlib.h>
//Class stringstream
#include <sstream>

//OpenCv Stuff
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;


//Contour following requirements?
#define w 500
int levels = 3;
CvSeq* contours = 0;
CvMemStorage* storage = cvCreateMemStorage(0);

//Our images
IplImage *image = 0, *gray = 0, *thres = 0; 


//Interface widgets names
char wndname[] = "Edge";  
char tbarname[] = "Threshold";
int edge_thresh = 1;


void threshold() {
  cout << "doing threshold" << endl;
  cvZero(image);
  //cvAdaptiveThreshold(gray, image, max, method, type, param);
  cvThreshold(gray, thres, 10, 200, CV_THRESH_BINARY_INV);		
}

void contour_follow() {
	
  cout << "doing contourfollow" << endl;
  /*The function returns total number of retrieved contours
     int cvFindContours(IplImage *img, CvMemStorage *storage, CvSeq **firstcontour, int headersize, 
     	CvContourRetrivalMode mode,CvChainApproxMethod method)
	    
    
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

  // comment this out if you do not want approximation
  //contours = cvApproxPoly(contours, sizeof(CvContour), storage, 
  //		  CV_POLY_APPROX_DP, 3, 1 );
}

void show_contour(void) {
  CvSeq *_contours = contours;  
  IplImage *cnt_img = cvCreateImage(cvSize(thres->width, thres->height), IPL_DEPTH_8U, 3);
  cvZero(cnt_img);
  
  int levels = 1;
  int thickness = 1;
  cvDrawContours(cnt_img, _contours, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0),
		 levels, thickness, CV_AA);

  //Create a window
  cvNamedWindow("contour", 1);
  cvShowImage("contour", cnt_img);	  
  
  //Clean memory
  cvReleaseImage(&cnt_img);  
    
}

void sci_prog(int n_contour, string img_file_name) {

  string filename, buffer;
  typedef enum { IMG_NAME, STR_CLOS, LST_NXT, LST_END, CONTOUR_LIST, PRG_BULK } prog_step;
  const char* prog[] = {"img_name ='", "'", ",", ");", "contour_name= list(", 
                        "Img = imread(img_name); \n \
                         n = size(contour_name); \n \
                         for i = 1:n, \n \
                           A = read(contour_name(i), -1, 2); \n \
                           [row, col] = size(A); \n \
                           x = A(1:row, 1:1); \n \
                           y = A(1:row, 2:2); \n \
                           xset(\"window\", i); \n \
                           imshow(unfollow(x, y, size(Img))) \n \
                         end" };

  ofstream fout;
  fout.open("plotter.sci");

  fout << prog[IMG_NAME] << img_file_name << prog[STR_CLOS] << endl;
  //buffer = prog[CONTOUR_LIST];

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


void print_contour4(string img_file_name) {
  CvSeq* contourPtr = contours;
  CvSeqReader reader;
  int n_point = 0;
  int c_contour = 0;
  CvPoint p;  
  ofstream f_contour;
  //stringstream i2s;
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
      CV_READ_SEQ_ELEM(p, reader);  	
      f_contour << p.x << " " << p.y << endl;  		
    }

    f_contour.close();

  }

  sci_prog(c_contour, img_file_name);
}

// define a trackbar callback
void on_trackbar(int h)
{
  //Espera...
  cout << "Starting on_track" << endl;
  cvShowImage(wndname, thres);
}

int main(int argc, char* argv[]) {

  char *filename = (argc == 2 ? argv[1] : (char*)"escamas.bmp");

  if( (image = cvLoadImage( filename, 1)) == 0 ) {
    cout << "Can't find image \"escamas.bmp\". Please supply the image." << endl;
    return -1;
  }
	

  //Convert to grayscale
  gray = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
  thres = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
  cvCvtColor(image, gray, CV_BGR2GRAY);

  //Create a window
  cvNamedWindow(wndname, 1);

  //Create a toolbar 
  cvCreateTrackbar(tbarname, wndname, &edge_thresh, 100, on_trackbar);

  //Do the threshold
  threshold();	

  //Show the image
  on_trackbar(0);

  //Try to do contour following
  contour_follow();
  //Now show the contour
  show_contour();
  

  // Wait for a key stroke; the same function arranges events processing
  cvWaitKey(0);
  cvReleaseImage(&image);
  cvReleaseImage(&gray);
  cvReleaseImage(&thres);
  cvDestroyWindow(wndname);

  print_contour4(filename);

  return 0;

}


