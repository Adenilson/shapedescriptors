/** @file
 *
 * Fourier transform module
 *
 * Fourier related code goes here.
 *
 *
 * Copyright 2006
 * @author Adenilson Cavalcanti <savagobr@yahoo.com>
 *
 * @version
 */
#ifndef _FOURIER_H
#define _FOURIER_H

#include <fftw3.h>

/** It does fourier transform in a given vector.
 *
 * @param g A vector with signal to be transformed, we
 *          wait for a object/vector where sinal is accessable
 *          with g[0][0] for real part and g[0][1] for
 *          imaginary part.
 *
 * @param length Signal length.
 *
 * @param G Transformed signal, we expect a pre-allocated
 *          vector compatible with type 'double o[2]'.
 *
 */
template <class TYPE1, class TYPE2>
void transform(TYPE1 g, int length, TYPE2 G)
{

	fftw_plan fwd_plan;
	fftw_complex *in, *out;

	in = reinterpret_cast<fftw_complex *>(g);
	out = reinterpret_cast<fftw_complex *>(G);

	fwd_plan = fftw_plan_dft_1d(length, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fwd_plan);

	fftw_destroy_plan(fwd_plan);
}

/** It does fourier transform in a given vector.
 *
 * @param G Transformed signal, we expect a pre-allocated
 *          vector compatible with type 'double o[2]'.
 *
 * @param length Signal length.
 *
 * @param g A vector with inverted transformed signal, we receive
 *          a pre-allocated vector (not normalized).
 *
 */
template <class TYPE>
void invert(TYPE G, int length, TYPE g)
{
	fftw_plan inv_plan;
	fftw_complex *in, *out;

	in = reinterpret_cast<fftw_complex *>(G);
	out = reinterpret_cast<fftw_complex *>(g);

	inv_plan = fftw_plan_dft_1d(length, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(inv_plan);

	fftw_destroy_plan(inv_plan);
}

#endif
