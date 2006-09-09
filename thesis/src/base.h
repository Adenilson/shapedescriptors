//------------------------------------------------------------------------------
#ifndef _BASE_H_
#define _BASE_H_
//------------------------------------------------------------------------------
/*
Author: Adenilson Cavalcanti da Silva savagobr@yahoo.com
License: GPL for non comercial use (contact author for other licensing contracts)
History:
    vs 0.01 09-08-2005 Wrote basic stuff: 2d point struct, 'norm' and 'distance' functions

    vs 0.02 18-10-2005 Added 3d point struct (mainly to support 'ratio_dist')

*/
//math stuff
#include <math.h>

//OpenCv Stuff
#include <opencv/cv.h>
#include <opencv/highgui.h>

//Level of message information
typedef enum { DEBUG, VERBOSE, QUIET } OPMODE;

//My point type (if we are using OpenCV, defaults to CvPoint)
#define M2DPOINT
//#undef M2DPOINT

//typedef float NUM;
typedef double NUM;


//Simple struct to support a record with 3 fields
struct d3point {
  NUM x, y, z;

  d3point(void): x(0), y(0), z(0)
  {}

};


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
  cout << t2[0] << "\t" << t2[1] << endl;
*/
struct t_point{

  NUM data[2];
  NUM &x;
  NUM &y;

  //ANSI C 99 complex type compatibility layer
  NUM &operator[](int i) {
    return data[i];
  }

  //Simple math operators
  t_point &operator=(t_point &a) {
    x = a.x;
    y = a.y;
    return *this;
  }

  template<class TYPE>
  t_point &operator=(TYPE &a) {
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

  //Constructors (we should initialize the field x, y references
  //to array of data)
  template <class TYPE>
  t_point(TYPE &xi, TYPE &yi): x(data[0]), y(data[1])  {
    x = xi;
    y = yi;
  }

  t_point(t_point &p): x(data[0]), y(data[1]) {
    x = p.x;
    y = p.y;
  }

  t_point(int xi, int yi): x(data[0]), y(data[1]) {
    x = xi;
    y = yi;
  }

  t_point(void): x(data[0]), y(data[1])
  {}

};
#ifdef M2DPOINT
typedef t_point m_point;
#else
typedef CvPoint2D32f m_point;
#endif

//Norm or magnitude of a vector
template <class T1, class T2>
inline float norm(T1 x, T2 y) {
  return float(sqrt( (x * x) + (y * y) ));
}

//Distance of points
template <class T1, class T2>
inline float distance(T1 &a, T2 &b) {
  return norm(float(a.x - b.x), float(a.y - b.y));
}


//------------------------------------------------------------------------------
#endif