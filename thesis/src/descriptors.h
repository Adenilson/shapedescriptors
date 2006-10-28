/** @file
 *
 * Shape descriptors code module.
 *
 * Copyright 2005
 * @author Adenilson Cavalcanti <savagobr@yahoo.com>
 *
 * @version
 */

#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_


#include "base.h"

//Calculates diameter, parameters
//a) points: vector with coordinates of object
//b) size: of vector
//Returns the diameter
//21-08-2005
float diameter(m_point *points, int *size);

//Returns coordinates of one contour, parameters
//a) obj: sequence with coordinates of object
//b) size: pointer to variable that will hold the size of returned vector
//Returns a vector with coordinates of one contour (it does the
// memory allocation!)
//21-08-2005
m_point *points(CvSeq *obj, int *size);

//Calculates centroid, parameters:
//a) contour: pointer to sequence that holds contours
//b) size: pointer to a variable that will hold the allocated size of
//answer vector
//Returns a vector with coordinates of centroid of each contour (it does the
// memory allocation!)
m_point* calc_centroid(CvSeq *contour, int *size);



//Calculate each contour area and prints in std:io, parameters:
//a) contour: pointer to sequence that holds contours
void calc_area(CvSeq *contour);

//Calculate each contour area, parameters:
//a) contour: pointer to sequence that holds contours
//b) size: pointer to variable that will hold the size of vector
//Returns: vector with area of each contour.
float *calc_area(CvSeq *contour, int *size);


//Calculate diameter, parameters:
//a) contour: pointer to sequence that holds contours
//b) size: pointer to variable that will hold the size of vector
//Returns: vector with diameter of each contour.
//21-08-2005
float* calc_diam(CvSeq *contour, int *size);

//Write a file with max_dist, min_dist and max/min distances from centroid,
//parameters:
//a) contour: pointer to sequence that holds contours
//b) centroid: vector with centroid coordinates of contours (use 'calc_centroid')
//c) size: the size of vector
//d) filename: of file that will hold the descriptors
bool ratio_dist(CvSeq *contour, m_point* centroid, int size, char *filename);

//Calculate max/min, max and min distances of centroids ('centroid') in each scale of
//'contour' sequence. Parameters:
//a) contour: pointer to sequence that holds contours
//b) centroid: vector with centroid coordinates of contours (use 'calc_centroid')
//c) size: the size of vector
//d) distances: pre-allocated vector of structure with 3 fields (x, y, z)
void ratio_dist(CvSeq *contour, m_point* centroid, int size, d3point *distances);



#endif
