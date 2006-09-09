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
#include <pthread.h>

/** PI value */
#define PI 3.14159265359

/** It does fourier transform in a given vector. Pay attention that its not
 *  thread safe since fftw_plan_dft functions does share some global data.
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
/** Thread safe version of transform.
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
 * @param mutex A mutex pointer variable for locking fftw_plan creation.
 */
template <class TYPE1, class TYPE2, class TYPE3>
void transform(TYPE1 g, int length, TYPE2 G, TYPE3 *mutex)
{

	fftw_plan fwd_plan;
	fftw_complex *in, *out;

	in = reinterpret_cast<fftw_complex *>(g);
	out = reinterpret_cast<fftw_complex *>(G);

	pthread_mutex_lock(mutex);
	fwd_plan = fftw_plan_dft_1d(length, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	pthread_mutex_unlock(mutex);

	fftw_execute(fwd_plan);
	fftw_destroy_plan(fwd_plan);
}

/** It does fourier transform in a given vector. Pay attention that its not
 *  thread safe since fftw_plan_dft functions does share some global data.
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
template <class TYPE1, class TYPE2>
void inverse(TYPE1 G, int length, TYPE2 g)
{
	fftw_plan inv_plan;
	fftw_complex *in, *out;

	in = reinterpret_cast<fftw_complex *>(G);
	out = reinterpret_cast<fftw_complex *>(g);

	inv_plan = fftw_plan_dft_1d(length, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(inv_plan);

	fftw_destroy_plan(inv_plan);
}

/** It does fourier transform in a given vector. Pay attention that its not
 *  thread safe since fftw_plan_dft functions does share some global data.
 *
 * @param G Transformed signal, we expect a pre-allocated
 *          vector compatible with type 'double o[2]'.
 *
 * @param length Signal length.
 *
 * @param g A vector with inverted transformed signal, we receive
 *          a pre-allocated vector (not normalized).
 *
 * @param mutex A mutex pointer variable for locking fftw_plan creation.
 */
template <class TYPE1, class TYPE2, class TYPE3>
void inverse(TYPE1 G, int length, TYPE2 g, TYPE3 *mutex)
{
	fftw_plan inv_plan;
	fftw_complex *in, *out;

	in = reinterpret_cast<fftw_complex *>(G);
	out = reinterpret_cast<fftw_complex *>(g);

	pthread_mutex_lock(mutex);
	inv_plan = fftw_plan_dft_1d(length, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
	pthread_mutex_unlock(mutex);

	fftw_execute(inv_plan);
	fftw_destroy_plan(inv_plan);
}


/** Do shift operation. It allocates and returns a new vector
 * with shifted signal.
 *
 * @param signal Vector pointer to signal to be shifted.
 *
 * @param length Length of signal vector
 *
 * @return A new vector with shifted signal or NULL on error.
 */
template <class TYPE>
TYPE *shift(TYPE *signal, int length)
{

	return NULL;
}

/** Calculate derivate using Fourier derivative property.
 *
 * @param signal A given real or complex signal vector.
 *
 * @param length Signal vector length.
 *
 * @param transf Preallocated complex object vector, will hold filtered
 *               signal.
 * @param diff_level Which derivate we want i.e. use 'diff_level = 1' for
 *                   first derivate of signal function.
 *
 * TODO: implement the code!
 *       add gaussian filter to derivative
 *       add sigmoid filter to derivative
 */
template <class TYPE1, class TYPE2>
void differentiate(TYPE1 signal, int length, TYPE2 transf, double diff_level)
{





}

#endif
