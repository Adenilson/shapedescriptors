/**
 * @file   base.h
 * @author Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2005
 * @date   Sometime in 2005.
 *
 * @brief  Base math auxiliary code.
 *
 * vs 0.01 09-08-2005 Wrote basic stuff:
 *                    2d point struct, 'norm' and 'distance' functions
 *
 * vs 0.02 18-10-2005 Added 3d point struct (mainly to support
 *                    'ratio_dist')
 *
 */

#ifndef _BASE_H_
#define _BASE_H_

//math stuff
#include <math.h>

//OpenCv Stuff
#include <opencv/cv.h>
#include <opencv/highgui.h>

//Complex type
#include "mcomplex.h"

//My point type (if we are using OpenCV, defaults to CvPoint)
#define M2DPOINT
#undef M2DPOINT

/** The type used as basic number. */
typedef double NUM;


/** \brief Simple struct to support a record with 3 fields. */
struct d3point {
	/// Point coordinate x.
	NUM x,
	  /// Point coordinate y.
	  y,
	  /// Point coordinate z.
	  z;

	/// Default constructor, zero's struct fields;
	d3point(void): x(0), y(0), z(0)
		{}

};

#ifdef M2DPOINT
/** \brief Aux struct to support a coordinate point.
 *
 * I know that I could use CvPoint2D32f or ANSI C 99 "complex" but
 * I need some more general struct. Besides, at least gcc complex type
 * is broken (tested in august 2006).
 *
 * I put the data in a vector to keep compatibility with ANSI C and
 * also supply data fields (x, y) to keep compatibility with CvPoint2D32f.
 *
 * Example of use
 * int x = 1, y = 2;
 * float x1 = 10.1, y1 = 22.24;
 *
 * t_point t1(x, y);
 * t_point t2(x1, y1);
 * t_point t3 = t1 + t2;
 *
 * cout << t3.x << "\t" << t3.y << endl;
 * t2 = t3;
 * t3 = (t3 - t1);
 * cout << t3.x << "\t" << t3.y << endl;
 * t2 = t2 * t1;
 * cout << t2.x << "\t" << t2.y << endl;
 * cout << t2[0] << "\t" << t2[1] << endl;
 *
 * FIXME: remove all warnings on this struct!
 */
struct t_point {

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

/** Basic point type to hold contour coordinates. */
typedef t_point m_point;
#else
/** Basic point type to hold contour coordinates. */
typedef CvPoint2D32f m_point;
#endif

/** Calculates norm or magnitude of a vector.
 *
 *
 * @param x A coordinate of point.
 * @param y Other coordinate of point.
 *
 * @return The norm.
 */
template <class T1, class T2>
inline float norm(T1 x, T2 y) {
	return float(sqrt( (x * x) + (y * y) ));
}


/** Distance between 2 points.
 *
 *
 * @param a First point coordinates (x and y).
 * @param b Second point coordinates (x and y).
 *
 * @return The distance between this 2 points.
 */
template <class T1, class T2>
inline float distance(T1 &a, T2 &b) {
	return norm(float(a.x - b.x), float(a.y - b.y));
}


/** Resource ownership type */
typedef enum {
	/** The one who hold a reference must destroy/free resource */
	DESTROY,
	/** The one who holds a reference/pointer must *not* free resource*/
	IGNORE } OWNERSHIP;

/**
 * \brief An adaptor, makes CvSeq accessible like a vector of complex type.
 *
 * This adaptor insures a common access method to coordinate points
 * between OpenCV CvSeq and a vector of mcomplex<double> (and by
 * extension fftw_complex).
 * My ideia is have common algorithms (derivate/energy/etc) for any
 * kind of complex number vector.
 *
 * \todo Move this class to a separated file (any time I change
 * this guy, the whole app needs to be rebuilt).
 */
template <typename NUMBER>
class ocv_adaptor {
protected:
	/** An OpenCV contour sequence */
	CvSeq *sequence;
	/** Number of points in current contour */
	int current_contour_points;
	/** Contour sequence resource management behaviour, see
	 * \ref OWNERSHIP.
	 */
	OWNERSHIP purge_seq;

public:

	/** Object constructor.
	 *
	 * When constructing the object, its possible to ask him to
	 * clean up the pointed contour sequence memory in its destructor.
	 * To do this, just set behaviour properly.
	 *
	 * @param aseq An OpenCV contour sequence.
	 *
	 * @param behaviour If current object instance should or not
	 * to clean up contour sequence memory. See \ref OWNERSHIP.
	 *
	 */
	ocv_adaptor(CvSeq *aseq, OWNERSHIP behaviour = IGNORE):sequence(NULL),
				 current_contour_points(0),
				 purge_seq(IGNORE) {


	}

	/** Copy constructor.
	 *
	 * Since this class has protected pointer members, we must provide
	 * a copy constructor. Concerning contour sequence ownership, its
	 * default is to ignore clean up of pointer/reference.
	 *
	 * @param obj An ocv_adaptor object reference.
	 *
	 * @param behaviour If current object instance should or not
	 * to clean up contour sequence memory.
	 *
	 */
	ocv_adaptor(ocv_adaptor &obj, OWNERSHIP behaviour = IGNORE) {
		sequence = NULL;
	}


	/** Overloaded operator.
	 *
	 * This one ensures correct copy behaviour for objects. Ownership
	 * behaviour is as follow:
	 * \note if copied object has purge_seq field set to DESTROY,
	 * current object will *not* receive contour sequence ownership.
	 *
	 * @param obj A ocv_adaptor object reference.
	 *
	 * @return A reference to current object.
	 */
	ocv_adaptor &operator=(ocv_adaptor &obj) { return *this; }

	/** Overloaded operator, access coordinate point.
	 *
	 * Enables access to OpenCV sequence similar to a vector
	 * of complex numbers.
	 * How to handler invalid return objects implicitly?
	 * Should I throw an exception?
	 *
	 * @param point Contour point position, ranging from 0 to
	 * (length - 1).
	 *
	 * @return A object of \ref mcomplex type.
	 */
	mcomplex<NUMBER> operator[] (int point) {
		mcomplex<NUMBER> obj;
		obj[0] = 10;
		obj[1] = 22;
		return obj;
	}

	/** Change object contour sequence reference.
	 *
	 * It enables an user to change contour sequence pointed by adaptor.
	 *
	 * @param aseq A pointer to an OpenCV contour sequence.
	 *
	 * @param behaviour Destructor behaviour, will free up OpenCV
	 * contour sequence if set to \ref DESTROY.
	 *
	 */
	void reset(CvSeq *aseq, OWNERSHIP behaviour = IGNORE) { }


	/** Advance to next object coordinate set.
	 *
	 * An OpenCV contour sequence has point coordinates of several
	 * different shape objects. Call this function to advance to next
	 * object in sequence.
	 *
	 * @return 0 to end of contour sequence, 1 in sucess, -1 in error case.
	 */
	int next(void) { return 0; }


	/** Default destructor.
	 *
	 * If OpenCV contour sequence ownership is set to \ref DESTROY,
	 * it will free up sequence data.
	 *
	 */
	~ocv_adaptor(void) { }
};

#endif
