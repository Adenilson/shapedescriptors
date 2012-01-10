/**
 * @file   mcomplex.h
 * @author  Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2006
 * @date   Middle year, 2006
 *
 * @brief  Complex number class.

 * This file has a derived class from std::complex with some helper
 * overloaded operators. It was tested to be binary compatible with
 * fftw_complex
 *
 * \todo
 * - Add unit tests (fftw_complex compatibility).
 * - It seems that doxygen has some issues with template based class.
 *
 */

/*  Copyright (C) 2006  Adenilson Cavalcanti <cavalcantii@gmail.com>
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

#ifndef _MCOMPLEX_
#define _MCOMPLEX_

#include <complex>

/** \brief Complex type, we derive it from C++ STL type complex.
 *
 * Added 2 more operators to access number parts: set with
 * 'number(real, imag)'
 * and get with 'number[0] || number[1]'.
 *
 * At present, we use complex<T>::real() to access number parts (at least
 * it is inline...).
 *
 * Dependes on STL header complex, and its binary compatible with its
 * ancestor as also with FFTW (Fast Fourier Transformation in the West)
 * type 'fftw_complex'.
 */
template <class NUMBER>
class mcomplex: public std::complex<NUMBER> {
public:

	/** Extra operator, make it easier to set values
	 * into a complex number.
	 *
	 * @param _real A real/integer number
	 * @param _img  A real/integer number for imaginary part
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
