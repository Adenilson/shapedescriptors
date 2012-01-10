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

/*  Copyright (C) 2007  Adenilson Cavalcanti <cavalcantii@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; by version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
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
	/** OpenCV contour sequence point reader */
	CvSeqReader cv_reader;
	/** Number of shapes inside sequence */
	int sequence_length;


	/** Count the number of shapes inside sequence.
	 *
	 * This function will run over sequence to calculate
	 * its length or the number of shapes inside it.
	 *
	 */
	void count_sequence(void) {

		if (sequence) {
			sequence_length = 0;
			CvSeq *tmp = sequence;
			while (tmp) {
				++sequence_length;
				tmp = tmp->h_next;
			}
		}
	}

	/** CvSeq seems a bit hard to be cleaned up, requiring
	 * 2 stage destruction.
	 * \todo Discover how to effectively clean memory on CvSeq.
	 */
	void clean_sequence(void) {

		if ((purge_seq == DESTROY) && (sequence != NULL)) {
			cvClearSeq(sequence);
			/* FIXME: OpenCV header files says that we must
			 * call this to *really* free memory, but
			 * those are different types.
			 *
			 cvClearMemStorage(sequence);
			*/
		}

	}

public:

	/** Returns current shape contour length.
	 *
	 * This function will return the number of points present
	 * in current pointed contour.
	 *
	 * @return Contour length or -1 in error.
	 */
	int contour_length(void) {

		return current_contour_length;
	}

	/** Returns sequence length, or number of contours.
	 *
	 * @return Sequence length or -1 in error.
	 */
	int contour_number(void) {

		return sequence_length;
	}


	/** Reset contour sequence reader.
	 *
	 * If you are going to read a given contour sequence
	 * *after* transversing it, call this function to
	 * restart sequence reader.
	 *
	 * @param behaviour Destructor behaviour, will free up OpenCV
	 * contour sequence if set to \ref DESTROY.
	 *
	 */
	void reset(OWNERSHIP behaviour = IGNORE) {
		if (sequence)
			cvStartReadSeq(sequence, &cv_reader);
		purge_seq = behaviour;
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
	void reset(CvSeq *aseq, OWNERSHIP behaviour = IGNORE) {

		if (aseq) {

			if (aseq != sequence) {
				current_contour_length = aseq->total;
				clean_sequence();
				sequence = aseq;
				count_sequence();
			}

			cvStartReadSeq(sequence, &cv_reader);
			purge_seq = behaviour;
		}
	}


	/** Object default constructor.
	 *
	 * An empty constructor is necessary in some cases.
	 */
	ocv_adaptor(void): sequence(NULL), current_contour_length(-1),
		purge_seq(IGNORE), sequence_length(-1) {

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
		sequence(NULL), current_contour_length(0),
		purge_seq(IGNORE) {

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
	ocv_adaptor(ocv_adaptor &obj, OWNERSHIP behaviour = IGNORE):
		sequence(NULL), current_contour_length(0),
		purge_seq(IGNORE) {
		reset(obj.sequence, behaviour);
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
	ocv_adaptor &operator=(ocv_adaptor &obj) {
		reset(obj.sequence, obj.behaviour);
		return *this;
	}

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
	 * \todo
	 * - Create an exception class to throw a meaningful exception.
	 */
	mcomplex<NUMBER> operator[] (int point) {
		mcomplex<NUMBER> obj;
		CvPoint cv_point;

		/* KISS: keep it simple, stupid!
		 */
		cvSetSeqReaderPos(&cv_reader, point);

		if (!sequence)
			//XXX: We need an Exception class.
			throw int(10);
		else {
			CV_READ_SEQ_ELEM(cv_point, cv_reader);
			obj[0] = (NUMBER) cv_point.x;
			obj[1] = (NUMBER) cv_point.y;
		}

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
	int next(void) {
		int result = -1;
		if (sequence) {
			CvSeq *tmp = sequence;
			sequence = sequence->h_next;
			if (sequence) {
				result = 1;
				current_contour_length = sequence->total;
				reset(purge_seq);
			} else {
				result = 0;
				sequence = tmp;
			}
		}

		return result;
	}


	/** Default destructor.
	 *
	 * If OpenCV contour sequence ownership is set to \ref DESTROY,
	 * it will free up sequence data.
	 *
	 */
	~ocv_adaptor(void) {
		clean_sequence();
	}
};

#endif
