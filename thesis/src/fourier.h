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
 * FIXME: make it work with vector's length = 3.
 */
template <class TYPE>
TYPE *shift(TYPE *signal, int length)
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
		cutoff = length/2 + is_odd;

		for (i = 0; i < cutoff - 1; ++i, ++counter)
			transf[i] = signal[cutoff + i];

		/* XXX: do we need this arithmetic? */
		for (i = cutoff - 1; i < length; ++i, ++counter)
			transf[i] = signal[i - cutoff - 2];

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
 * @param diff_level Level of derivative (first = 1, second = 2, etc).
 * @param length Length of filter function, must be equal to filtered
 *               signal.
 *
 * @return A vector with filter, NULL otherwise.
 *
 * TODO: use type mcomplex.
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


/** Calculates constant for Gaussian fourier transform.
 *
 *
 * @param tau Analysing scale (equal 1/a in original Gaussian), where
 *            a = standard deviation.
 *
 * @return The constant (-(2*pi)^2)/(2 * (tau ^ 2))
 */
inline double calc_cnst(double tau)
{
	double cnst = 0.0;

	cnst = pow(-(2.0 * PI), 2);
	cnst /= 2 * pow(tau, 2);

	return cnst;
}


/** Calculates constant for Gaussian fourier transform (simplified version).
 *
 *
 * @param tau Analysing scale (equal 1/a in original Gaussian), where
 *            a = standard deviation.
 *
 * @return The simplified constant (2 * pi^2)/(tau ^ 2)
 */
inline double calc_scnst(double tau)
{
	double cnst = 0.0;

	cnst = PI * PI;
	cnst *= 2.0;
	cnst /= tau * tau;

	return cnst;
}


/** Calculates gaussian fourier transformed function
 * G(f) = exp(cnst* f^2), where cnst is calculated by 'calc_scnst'
 * and cnst = (-(2*pi)^2)/(2 * (tau ^ 2)).
 *
 *
 * @param length Vector length.
 *
 * @param tau Analysing scale.
 *
 * @param upper Gauss distribution coverage (6 covers whole distro).
 *
 * @return Fourier Transformed Gaussian or NULL in error.
 * TODO: we need to test it to determine if behaviour is correct since
 *       this function tends to infinite (inf). I test it against Scilab
 *       with: tau = 10; f= 7; exp((2*%pi^2/(tau^2))*f^2)
 */
double *gaussian_fourier(int length, double tau = 2.0, double upper = 6.0)
{
	double cnst, sampling, lower;
	double *G = NULL;
	sampling = cnst = 0;

	G = new double[length];
	if (!G)
		goto exit;

	lower = -upper;
	sampling = upper/(length/2.0);


	cnst = calc_scnst(tau);
	for (int i = 0; i < length; ++i) {
		G[i] = lower * lower;
		G[i] *= cnst;
		G[i] = exp(G[i]);
		lower += sampling;
	}

exit:

	return G;
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
 * @param tau Gaussian inverse variance (1/a) to smooth signal.
 *
 * @param mutex A mutex object to lock when doing fourier transform.
 *
 * @return  Complex object vector that holds filtered signal or NULL
 * FIXME:
 *       normalization issues in differentiated signal
 *
 * TODO:
 *       add sigmoid filter to derivative;
 *       should I use auto pointers?;
 *       must use mcomplex type (due to operator [] we achieve
 *        binary compatibility with fftw_complex);
 */
template <class TYPE1>
std::complex<double> *differentiate(TYPE1 signal, int length,
				    double diff_level = 1.0,
				    double tau = 2.0,
				    pthread_mutex_t *mutex = NULL)
{

	double *f_gaussian = NULL;
	std::complex<double> *transformed, *diff_filter, *tmp, *tmp2, *res;
	transformed = diff_filter = tmp = tmp2 = res = NULL;
	transformed = new std::complex<double> [length];
	if (!transformed)
		goto error;

	if (mutex)
		transform(signal, length, transformed, mutex);
	else
		transform(signal, length, transformed);

	tmp = shift(transformed, length);
	if (!tmp)
		goto error;

	diff_filter = create_filter(diff_level, length);
	if (!diff_filter)
		goto error;

	if (tau) {
		f_gaussian = gaussian_fourier(length, tau);
		if (!f_gaussian)
			goto error;
	}

	/* Apply diff filter and gaussian  to shifted signal */
	if (tau)
		for (int i = 0; i < length; ++i) {
			tmp[i] *= diff_filter[i];
			/* FIXME: Should I multiply real part too?
			   tmp[i].real() *= f_gaussian[i];
			*/
			tmp[i].imag() *= f_gaussian[i];
		}
	else /* dont do gaussian filter */
		for (int i = 0; i < length; ++i)
			tmp[i] *= diff_filter[i];


	tmp2 = unshift(tmp, length);
	if (!tmp2)
		goto error;

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
	/* FIXME: should I throw an exception? */
	printf("\nWe got a problem\n!");

dealloc:
	if (transformed)
		delete [] transformed;
	if (tmp2)
		delete [] tmp2;
	if (diff_filter)
		delete [] diff_filter;
	if (f_gaussian)
		delete [] f_gaussian;

	return res;
}

/** Curvature calculus
 * Given first and second derivatives of a given contour (that needs
 * to have x:y coordinates), calculate curvature of shape.
 *
 * @param x First derivative of x coordinate of contour.
 * @param xx Second derivative of x coordinate of contour.
 * @param y First derivative of y coordinate of contour.
 * @param yy Second derivative of y coordinate of contour.
 * @param length Vector elements.
 *
 * @return Curvature of given shape.
 *
 * TODO: Gaussian filter (actually in derivative code).
 */
template <typename TYPE>
double *curvature(TYPE x, TYPE xx, TYPE y, TYPE yy, int length)
{

	double *k = NULL;
	k = new double[length];
	double tmp;
	double raiser = 1.0/3.0;
	if (!k)
		goto exit;

	for (int i = 0; i < length; ++i) {
		k[i] = (x[i][0] * yy[i][0]) + (y[i][0] * xx[i][0]);
		tmp = (x[i][0] * x[i][0]) + (y[i][0] * y[i][0]);
		k[i] /= pow(tmp, raiser);
	}



exit:
	return k;

}


#endif

