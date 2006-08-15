//------------------------------------------------------------------------------
#include "descriptors.h"

#include <fstream>
#include <iostream>
#include <string>
//Class stringstream
//#include <sstream>

using namespace std;
//------------------------------------------------------------------------------

void ratio_dist(CvSeq *contour, m_point* centroid, int size, d3point *distances) {

  bool result;
  float max, min, idist;
  int n_point, n_contours;
  m_point jcoord;
  CvSeqReader reader;
  CvPoint p;


  //Test if the size of vector with centroids is the same as the number of
  //contours inside the sequence
  CvSeq *temp = contour;
  n_contours = 0;
  for(; temp != NULL; temp = temp->h_next)
    ++n_contours;
  if(n_contours != size)
    return;

  //Calculate each distance in n-ith contour

    for(int i = 0; i < size; ++i) {
      cvStartReadSeq(contour, &reader);
      n_point = contour->total;

      //Initializes the distances with first coordinate
      CV_READ_SEQ_ELEM(p, reader)
      max = min = distance(p, centroid[i]);

      //Run over all contour and calculate distance to centroid
      for (int j = 0; j < n_point; ++j) {
        CV_READ_SEQ_ELEM(p, reader);
        jcoord = centroid[i];
        idist = distance(p, jcoord);
        //idist = norm(p.x - jcoord.x, p.y - jcoord.y);
        if(max < idist)
          max = idist;

        if(min > idist)
          min = idist;

      }

      distances[i].x = max/min;
      distances[i].y = max;
      distances[i].z = min;
      contour = contour->h_next;
    }

}
//------------------------------------------------------------------------------

//Write a file with max_dist, min_dist and max/min distances from centroid
bool ratio_dist(CvSeq *contour, m_point* centroid, int size, char *filename) {

  bool result;
  float max, min, idist;
  int n_point, n_contours;
  m_point jcoord;
  CvSeqReader reader;
  CvPoint p;
  ofstream fout(filename);

  //Test if the size of vector with centroids is the same as the number of
  //contours inside the sequence
  CvSeq *temp = contour;
  n_contours = 0;
  for(; temp != NULL; temp = temp->h_next)
    ++n_contours;
  if(n_contours != size)
    return result = false;

  //Calculate each distance in n-ith contour
  if(fout.is_open())
    for(int i = 0; i < size; ++i) {
      cvStartReadSeq(contour, &reader);
      n_point = contour->total;

      //Initializes the distances with first coordinate
      CV_READ_SEQ_ELEM(p, reader)
      max = min = distance(p, centroid[i]);

      //Run over all contour and calculate distance to centroid
      for (int j = 0; j < n_point; ++j) {
        CV_READ_SEQ_ELEM(p, reader);
        jcoord = centroid[i];
        idist = distance(p, jcoord);
        //idist = norm(p.x - jcoord.x, p.y - jcoord.y);
        if(max < idist)
          max = idist;

        if(min > idist)
          min = idist;

      }

      fout << max/min << "    " << max << "    " << min << endl;
      contour = contour->h_next;
    }
    return result = true;
}
//------------------------------------------------------------------------------

//Calculate centroid
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

    contour = contour->h_next;
  }

  return answer;

}

//------------------------------------------------------------------------------

//Calculate area and prints in std:io
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
//------------------------------------------------------------------------------

//Calculate area, returns vector with area of each contour
float *calc_area(CvSeq *contour, int *size) {
  CvSeq *temp = NULL;
  CvSeqReader reader;
  float area, *result = NULL;
  int counter = 0;

  for(temp = contour; temp != NULL; temp = temp->h_next)
    counter++;

  *size = counter;
  result = new float[counter];

  for(int i = 0; i < counter; ++i) {

    area = cvContourArea(contour);
    if(area < 0)
      area = -area;

    *(result + i) = area;

    contour = contour->h_next;
  }

  return result;

}
//------------------------------------------------------------------------------
float* calc_diam(CvSeq *contour, int *size) {
  CvSeq *temp = NULL;
  float *result = NULL;
  m_point *coord = NULL;
  int csize = 0;
  int counter = 0;

  for(temp = contour; temp != NULL; temp = temp->h_next)
    counter++;

  *size = counter;
  result = new float[counter];

  for(int i = 0; i < counter; ++i) {
    coord = points(contour, &csize);
    result[i] = diameter(coord, &csize);
    delete [] coord;
    contour = contour->h_next;
  }

  return result;

}
//------------------------------------------------------------------------------
float diameter(m_point *points, int *size) {
  float result = 0, temp = 0;

  for(int i = 0; i < (*size-1); ++i)
    for(int j = i+1; j < *size; ++j) {
      //temp = distance(points[i], points[j]);
      temp = norm(points[i].x - points[j].x, points[i].y - points[j].y);
      if(temp > result) {
        result = temp;

      }

    }

  return result;

}
//------------------------------------------------------------------------------
m_point *points(CvSeq *obj, int *size) {
  m_point *result = NULL;
  CvSeqReader reader;
  CvPoint p1;
  cvStartReadSeq(obj, &reader);

  *size = obj->total;
  result = new m_point[*size];

  for(int i = 0; i < *size; ++i) {
    CV_READ_SEQ_ELEM(p1, reader);
    result[i] = p1;
  }

  return result;

}

//------------------------------------------------------------------------------








