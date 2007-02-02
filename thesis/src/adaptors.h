/**
 * @file   adaptors.h
 * @author Adenilson Cavalcanti
 * @date   Fri Feb  2 15:55:49 2007
 *
 * @brief  Adaptor classes.
 *
 * Provides class to enable access to contour objects
 * in similar way as a vector of complex type.
 */
#ifndef __ADAPTORS__
#define  __ADAPTORS__

//Complex type
#include "mcomplex.h"

//OpenCv Stuff
#include <opencv/cv.h>

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
 * \todo move class method definition to a file (e.g. adaptors.cpp),
 * I stumbled onto 'undefined reference' when exported the constructor
 * to a separated file.
 *
 */
template <typename NUMBER>
class ocv_adaptor {
protected:
	/** An OpenCV contour sequence */
	CvSeq *sequence;
	/** Number of points in current contour */
	int current_contour_length;
	/** Contour sequence resource management behaviour, see
	 * \ref OWNERSHIP.
	 */
	OWNERSHIP purge_seq;

	/** CvSeq seems a bit hard to be cleaned up, requiring
	 * 2 stage destruction.
	 * \todo Discover how to effectively clean memory on CvSeq.
	 */
	void clean_sequence(void) {
		if (purge_seq == DESTROY && (sequence != NULL)) {
			cvClearSeq(sequence);
			/* OpenCV header files says that we must
			 * call this to *really* free memory, but
			 * those are different types.
			 *
			cvClearMemStorage(sequence);
			*/
		}
	}

public:

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
	void reset(CvSeq *aseq, OWNERSHIP behaviour = IGNORE) {

		if (aseq) {
			current_contour_length = aseq->total;
			clean_sequence();
			sequence = aseq;
			purge_seq = behaviour;
		}

	}

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
	ocv_adaptor(CvSeq *aseq, OWNERSHIP behaviour = IGNORE):
		sequence(NULL), current_contour_length(0), purge_seq(IGNORE) {

		reset(aseq, behaviour);
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