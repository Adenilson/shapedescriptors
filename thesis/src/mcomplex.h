/** @file
 *
 * Complex number module
 *
 *
 * Copyright 2006
 * @author Adenilson Cavalcanti <savagobr@yahoo.com>
 *
 * @version
 */

#ifndef _MCOMPLEX_
#define _MCOMPLEX_

#include <complex>

/** Complex type, we derive it from C++ STL type complex, added
 * 2 more operators to access number parts: set with 'number(real, imag)'
 * and get with 'number[0] || number[1]'.
 * At present, we use complex<T>::real() to access number parts (at least
 * it is inline...).
 * Dependes on STL header <complex>, and its binary compatible with its
 * ancestor as also with FFTW (Fast Fourier Transformation in the West)
 * type 'fftw_complex'.
 */
template <class NUMBER>
class mcomplex: public std::complex<NUMBER> {
public:

	/** Extra operator, make it easier to set values
	 * into a complex number.
	 *
	 * @param real A real/integer number
	 * @param img  A real/integer number for imaginary part
	 *             of complex number
	 *
	 */
	template <class TYPE1, class TYPE2>
	mcomplex<NUMBER> &operator()(TYPE1 _real, TYPE2 _img) {
		std::complex<NUMBER>::real() = _real;
		std::complex<NUMBER>::imag() = _img;
		return *this;
	}

	/** Defines a simple structure based complex type compatibility layer.
	 *
	 * @param i Access to number index (0 == real, 1 == imaginary).
	 *
	 * @return number or real part on invalid index.
	 */
	NUMBER &operator[](int i) {
		if (i == 0)
			return std::complex<NUMBER>::real();
		else if (i == 1)
			return std::complex<NUMBER>::imag();

		return std::complex<NUMBER>::real();
	}


	/** Overloaded operator, to do implicit convertion.
	 *
	 * @param c A complex number, should have methods
	 *          c.real() and c.imag()
	 *
	 * @return A reference for this object
	 */
	template <class TYPE>
	mcomplex<NUMBER> &operator=(const std::complex<TYPE> &c) {
		std::complex<NUMBER>::real() = c.real();
		std::complex<NUMBER>::imag() = c.imag();
		return *this;
	}

};

#endif
