/**
 * @file   descriptors.h
 * @author Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2005
 * @date  Sometime in 2005
 *
 * @brief  Shape descriptors code module, here we calculate contour based
 * shape descriptors like: centroid distance, area, etc.
 *
 */

#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_


#include "base.h"

/** Calculates diameter (maximum distance of 2 points).
 * First version was done in 21-08-2005.
 *
 * @param points Vector with coordinates of object.
 * @param size Size of vector, its length.
 *
 * @return Calculated diameter of given shape.
 */
float diameter(m_point *points, int *size);


/** It returns a vector with coordinates of a given contour (in OpenCV
 * representation).
 * First version was done in 21-08-2005.
 *
 * @param obj Sequence with coordinates of object.
 * @param size Pointer to variable that will hold the size of returned vector.
 *
 * @return A vector with coordinates of one contour (it does the memory
 *         allocation, so remember to free up this memory later...).
 *
 * TODO: change this name, its not descriptive.
 */
m_point *points(CvSeq *obj, int *size);


/** Calculates centroid (center of gravity) of each contour existent in a
 * shape contour sequence. Its the mean point in the shape.
 *
 * @param contour Pointer to sequence that holds several contours.
 * @param size Pointer to a variable that will hold the allocated size
 *             of answer vector.
 *
 * @return A vector with coordinates of centroid of each contour contained
 *         in sequence vector (it does the memory allocation, so remember
 *         to free up this memory later...).
 *
 * TODO: it should be changed to work with m_point vectors, just like
 *       function 'diameter'.
 */
m_point* calc_centroid(CvSeq *contour, int *size);


/** Calculates the area of each contour existent in a shape contour sequence.
 * It later prints the results in std::out.
 *
 * @param contour Pointer to sequence that holds several contours.
 *
 * FIXME: if this one is not been used, we should purge it from file.
 */
void calc_area(CvSeq *contour);

/** Calculates the area of each contour existent in a shape contour sequence.
 *
 * @param contour Pointer to sequence that holds several contours.
 * @param size Pointer to variable that will hold contour's areas vector size.
 *
 * @return Vector with calculated area of all contours.
 */
float *calc_area(CvSeq *contour, int *size);


/** It calculates diameter of each contour present in a OpenCV contour
 * sequence. First version in 21-08-2005.
 *
 * @param contour Pointer to sequence that holds several contours.
 * @param size Pointer to variable that will hold contour's areas vector size.
 *
 * @return Vector with calculated diameter of all contours.
 *
 * TODO: it should be changed to work with m_point vectors, just like
 *       function 'diameter'.
 */
float* calc_diam(CvSeq *contour, int *size);


/** Write out a file with centroid related data (maximum distance from
 * centroid, minimal distance from centroid, ratio of maximum/minimum distances
 * of centroid) of a OpenCV sequence of contours.
 *
 *
 * @param contour Pointer to sequence that holds several contours.
 * @param centroid Vector with centroid coordinates of each contour
 *                 (use 'calc_centroid' to calculate this one).
 * @param size Size of vector, or number of contours.
 * @param filename Output file name.
 *
 * @return True in success, false otherwise.
 */
bool ratio_dist(CvSeq *contour, m_point* centroid, int size, char *filename);

/** Calculates max/min, max and min distances of centroids ('centroid') in
 * each scale of 'contour' sequence.
 *
 * @param contour Pointer to sequence that holds contours.
 * @param centroid Vector with centroid coordinates of contours (use
 *                 'calc_centroid').
 * @param size The size of vector.
 * @param distances Pre-allocated vector of structure with 3 fields (x, y, z).
 *
 * TODO: I'm tired... must check if this comments make sense. Also maybe
 *       its not a good idea to use a d3point to hold this information.
 */
void ratio_dist(CvSeq *contour, m_point* centroid, int size, d3point *distances);



#endif
