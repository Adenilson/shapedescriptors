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
#include <complex>

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
 * TODO: make it work with vector's length = 3.
 */
template <class TYPE>
TYPE *shift(TYPE *signal, int length)
{
	TYPE *transf = NULL;
	int is_odd = 0;
	int cutoff, counter;
	int i;

	if (!signal)
		goto exit;

	transf = new TYPE[length];
	if (!transf)
		goto exit;

	is_odd = length % 2;
	if (is_odd) {
		cutoff = length/2 + is_odd;

		for (i = 0; i < cutoff - 1; ++i, ++counter)
			transf[i] = signal[cutoff + i];

		/* XXX: do we need this arithmetic? */
		for (i = cutoff - 1; i < length; ++i, ++counter)
			transf[i] = signal[i - cutoff - 2];

	} else {

		cutoff = length/2;
		counter = 0;
		for (i = 0; i < cutoff; ++i, ++counter)
			transf[i] = signal[cutoff + i];

		for (i = cutoff; i < length; ++i, ++counter)
			transf[i] = signal[i - cutoff];

	}
	/* FIXME: should I throw an exception? */
	if (counter > length)
		printf("\nbuffer overflow!\n");

 exit:
	return transf;
}

/** Do unshift operation. It allocates and returns a new vector
 * with original signal.
 *
 * @param signal Vector pointer to signal to be unshifted.
 *
 * @param length Length of signal vector
 *
 * @return A new vector with original signal or NULL on error.
 */
template <class TYPE>
TYPE *unshift(TYPE *signal, int length)
{
	TYPE *transf = NULL;
	int is_odd = 0;
	int cutoff, counter = 0;
	int i;

	if (!signal)
		goto exit;

	transf = new TYPE[length];
	if (!transf)
		goto exit;

	is_odd = length % 2;
	if (is_odd) {
		cutoff = length/2;

		for (i = cutoff; i < length; ++i, ++counter)
			transf[i - cutoff] = signal[i];

		for (i = cutoff + 1; i < length; ++i, ++counter)
			transf[i] = signal[i - cutoff - 1];

	} else {
		cutoff = length/2;

		for (i = 0; i < cutoff; ++i, ++counter)
			transf[i] = signal[cutoff + i];

		for (i = cutoff; i < length; ++i, ++counter)
			transf[i] = signal[i - cutoff];

	}
	/* FIXME: should I throw an exception? */
	if (counter > length)
		printf("\nbuffer overflow!\n");

 exit:
	return transf;
}


/** Create filter function
 *
 * @param j Level of derivative
 *
 * @return A vector with filter, NULL otherwise.
 */
std::complex<double> *create_filter(double diff_level, int length)
{
	std::complex<double> *res;
	res = new std::complex<double>[length];

	if (res)
		for (int i = 0; i < length; ++i) {
			res[i].imag() = 1;
			res[i].imag() *= i - length/2;
			res[i] = pow(res[i] * (2 * PI), diff_level);
		}
	else
		res = NULL;

	return res;
}


/** Calculate derivate using Fourier derivative property.
 *
 * @param signal A given real or complex signal vector.
 *
 * @param length Signal vector length.
 *
 * @param diff_level Which derivate we want i.e. use 'diff_level = 1' for
 *                   first derivate of signal function.
 *
 * @return  Complex object vector that holds filtered signal or NULL
 * TODO:
 *       add gaussian filter to derivative
 *       add sigmoid filter to derivative
 *       use thread safe version of Fourier transform
 */
template <class TYPE1>
std::complex<double> *differentiate(TYPE1 signal, int length, double diff_level)
{

	std::complex<double> *transformed, *diff_filter, *tmp, *tmp2, *res;
	transformed = diff_filter = tmp = tmp2 = res = NULL;
	transformed = new std::complex<double> [length];
	if (!transformed)
		goto error;

	transform(signal, length, transformed);
	tmp = shift(transformed, length);
	if (!tmp) {
		/* TODO: unify deallocation of resources. */
		delete [] transformed;
		goto error;
	}

	diff_filter = create_filter(diff_level, length);
	if (!diff_filter)
		goto dealloc;
	/* Apply diff filter to shifted signal */
	for (int i = 0; i < length; ++i)
		tmp[i] *= diff_filter[i];

	tmp2 = unshift(tmp, length);
	if (!tmp2)
		goto dealloc;

	inverse(tmp2, length, tmp);
	res = tmp;

	/* Pay attention that Fourier inverse is not
	 * normalized!
	 */
	for (int i = 0; i < length; ++i) {
		res[i].imag() /= length;
		res[i].real() /= length;
	}

	goto dealloc;

error:
	/* FIXME: How to handle errors? */
	printf("\nWe got a problem\n!");
	goto exit;

dealloc:
	delete [] transformed;
	delete [] tmp2;
	delete [] diff_filter;
	transformed = tmp = NULL;
exit:
	return res;
}

/*
template <class TYPE1, class TYPE2>
void differentiate(TYPE1 signal, int length, TYPE2 transf, double diff_level)
{


}
*/


#endif
