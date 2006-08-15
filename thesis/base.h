//------------------------------------------------------------------------------
#ifndef _BASE_H_
#define _BASE_H_
//------------------------------------------------------------------------------
//math stuff
#include <math.h>

//OpenCv Stuff
#include <opencv/cv.h>
#include <opencv/highgui.h>

//Level of message information
typedef enum { DEBUG, VERBOSE, QUIET } OPMODE;

//My point type (if we are using OpenCV, defaults to CvPoint)
#undef M2DPOINT
//#define M2DPOINT

//Aux struct to support a coordinate point (yes, I know that I could use
//CvPoint2D32f or ANSI C 99 "complex" but I need some more general struct).
//Besides, I put the data in a vector to keep compatibility with ANSI C and
//also supply data fields (x, y) to keep compatibility with CvPoint2D32f.
/* Example of use
  int x = 1, y = 2;
  float x1 = 10.1, y1 = 22.24;
  
  t_point t1(x, y);
  t_point t2(x1, y1);
  t_point t3 = t1 + t2;
  
  cout << t3.x << "\t" << t3.y << endl;
  t2 = t3;
  t3 = (t3 - t1);  
  cout << t3.x << "\t" << t3.y << endl;
  t2 = t2 * t1;  
  cout << t2.x << "\t" << t2.y << endl;
*/
struct t_point{
  //typedef double NUM;
  typedef float NUM;
  /*
  NUM data[2];
  NUM &x = data[0];
  NUM &y = data[1];
  */
  NUM x, y;
  
  /*
  t_point &operator[](int i) {
    return data[i];  
  }
  */
  
  t_point &operator=(t_point &a) {
    x = a.x;
    y = a.y;
    return *this;    
  }

  t_point &operator-(t_point &a) {
    t_point temp;
    temp.x = x - a.x;
    temp.y = y - a.y;
    return temp;    
  }

  t_point &operator+(t_point &a) {
    t_point temp;
    temp.x = x + a.x;
    temp.y = y + a.y;
    return temp;    
  }

  t_point &operator*(t_point &a) {
    t_point temp;
    temp.x = x * a.x;
    temp.y = y * a.y;
    return temp;    
  }

  t_point &operator/(t_point &a) {
    t_point temp;
    temp.x = x / a.x;
    temp.y = y / a.y;
    return temp;    
  }
  
  template <class TYPE>
  t_point(TYPE &xi, TYPE &yi): x(xi), y(yi)
  {}
  
  t_point(t_point &p) {
    x = p.x;  
    y = p.y;
  } 
  
  t_point(void): x(0), y(0)
  {}
    
};
#ifdef M2DPOINT
typedef t_point m_point;
#else
typedef CvPoint2D32f m_point;
#endif

//Norm or magnitude of a vector
inline float norm(float x, float y) {
  return sqrt( (x * x) + (y * y) );
}

inline float norm(m_point a) {
  //float result = 0;
  //result = sqrt( (a.x * a.x) + (a.y * a.y) );
  //return result;
  return sqrt( (a.x * a.x) + (a.y * a.y) );
}

//Distance of points
inline float distance(m_point a, m_point b) {
  //float result = 0;
  //result = norm((a.x - b.y), (a.y - b.y));
  return norm((a.x - b.y), (a.y - b.y));
}



//------------------------------------------------------------------------------
#endif
