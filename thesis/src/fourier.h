/**
 * @file   fourier.h
 * @author Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2006
 * @date   Middle year, 2006
 *
 * @brief  Fourier transform module, derivative and curvature
 * code goes here.
 *
 * Implemented multiscale curvature calculus (as described in Costa &
 * Cesar 'Shape Analysis and Classification', 2000). We use as fourier
 * transform fftw (Fast Fourier Transform in the West).
 *
 * \todo
 * - Fix normalization issue in curvature.
 * - Consider use integer when inputing data in fourier transform, when
 * possible.
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

#ifndef _FOURIER_H
#define _FOURIER_H

#include <fftw3.h>
#include <pthread.h>
#include "mcomplex.h"

/** PI value */
#define PI 3.14159265359

/** Extra filtering when calculating derivatives
 * \todo
 * - Get each function formula
 *
 */
typedef enum { /** Beta function */
	       FBETA,
	       /** Sigmoid function */
	       FSIGMOID,
	       /** Box function */
	       FBOX } FILTER_TYPE;

/** Bending energy error */
const double energy_error = -100.1111111111;
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
			/* XXX: This has changed since 2006 + gcc + linux */
			/*
			res[i].imag() = 1;
			res[i].imag() *= i - length/2;
			*/
			res[i].imag(1);
			res[i].imag(res[i].imag() * i - length/2);
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
			/* XXX: This has changed since 2006 + gcc + linux */
			//tmp[i].imag() *= f_gaussian[i];
			tmp[i].imag(tmp[i].imag() * f_gaussian[i]);
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
		/* XXX: This has changed since 2006 + gcc + linux */
		/*
		res[i].imag() /= length;
		res[i].real() /= length;
		*/
		res[i].imag(res[i].imag() / length);
		res[i].real(res[i].real() / length);
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
 * @return Curvature of given shape, NULL in error case.
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


/** Calculates energy.
 *
 * Given the curvature of a parametric curve, will  calculate the
 * energy of curve, e = sum(k^2(t))/n (where k = curvature).
 *
 * @param contour_curvature Curvature of contour.
 *
 * @param length Contour length.
 *
 * @return The energy or \ref energy_error otherwise.
 */
template <typename TYPE>
double energy(TYPE contour_curvature, int length)
{
	double result = energy_error;
	int i;
	if (!contour_curvature)
		goto exit;

	result = 0;
	for (i = 0; i < length; ++i)
		result += contour_curvature[i] * contour_curvature[i];

	result /= length;

exit:
	return result;
}



/** Calculates multiscale bending energy.
 *
 * This function implements bending energy, e(t) = sum(k(t)^2)/n
 * like in Cesar, R. M.; Costa, L. F. "Shape Characterization in Natural
 * scales by using multiscale bending energy" (1996). See also function
 * \ref curvature.
 *
 * @param signal The signal to be filtered, we expect a complex number
 * c(x, y) vector which can be represented as both integer/float/double.
 *
 * @param length The signal vector length.
 *
 * @param tau Gaussian inverse variance (1/a) to smooth signal.
 *
 * @param normalize Decide if we will normalize the energy to solve
 * energy shrinkage with perimeter normalization.
 *
 * @param extra_filter Use an extra filter (e.g. beta function) to control
 * high curvature spikes.
 *
 * @return A vector with contour curvature or NULL on error.
 */
template <typename TYPE1, typename TYPE2>
double *contour_curvature(TYPE1 signal, int length, double tau = 8,
		      bool normalize = false, FILTER_TYPE extra_filter = FBETA)
{
	double *result = NULL;
	TYPE2 *x, *y;
	mcomplex<double>  *x_diff, *xx_diff, *y_diff, *yy_diff;
	x_diff = xx_diff = y_diff = yy_diff = NULL;
	int i, diff_level;

	if (!(x = new TYPE2[length]))
		goto exit;

	if (!(y = new TYPE2[length]))
		goto cleanup;

	for (i = 0; i < length; ++i) {
		x[i][0] = signal[i][0];
		y[i][0] = signal[i][1];
	}

	x_diff = (mcomplex<double>*) differentiate(x, length, diff_level = 1,
						   tau);
	xx_diff = (mcomplex<double>*) differentiate(x, length,diff_level = 2,
						    tau);
	y_diff = (mcomplex<double>*) differentiate(y, length, diff_level = 1,
						   tau);
	yy_diff = (mcomplex<double>*) differentiate(y, length, diff_level = 2,
						    tau);
	if ((!x_diff) || (!xx_diff) || (!y_diff) || (!yy_diff))
		goto cleanup;


	result = curvature(x_diff, xx_diff, y_diff, yy_diff, length);
	if (!result)
		goto cleanup;

cleanup:
	if (x)
		delete [] x;
	if (y)
		delete [] y;
	if (x_diff)
		delete [] x_diff;
	if (xx_diff)
		delete [] xx_diff;
	if (y_diff)
		delete [] y_diff;
	if (yy_diff)
		delete [] yy_diff;
exit:

	return result;

}

/** A template wrapper to contour_curvature.
 *
 * Use this one with \ref mcomplex and with normal vectors.
 *
 * @param signal see \ref contour_curvature
 * @param length see \ref contour_curvature
 * @param tau see \ref contour_curvature
 * @param normalize see \ref contour_curvature
 * @param extra_filter see \ref contour_curvature
 *
 * @return see \ref contour_curvature
 */
template <typename TYPE>
double *contour_curvature(TYPE *signal, int length, double tau = 8,
		      bool normalize = false, FILTER_TYPE extra_filter = FBETA)
{
	return contour_curvature<TYPE *, TYPE>(signal, length, tau, normalize,
					     extra_filter);

}
/** Calculates multiscale bending energy.
 *
 * This function implements bending energy, e(t) = sum(k(t)^2)/n
 * like in Cesar, R. M.; Costa, L. F. "Shape Characterization in Natural
 * scales by using multiscale bending energy" (1996). See also function
 * \ref curvature.
 *
 * @param signal The signal to be filtered, we expect a complex number
 * c(x, y) vector which can be represented as both integer/float/double.
 *
 * @param length The signal vector length.
 *
 * @param tau Gaussian inverse variance (1/a) to smooth signal.
 *
 * @param normalize Decide if we will normalize the energy to solve
 * energy shrinkage with perimeter normalization.
 *
 * @param extra_filter Use an extra filter (e.g. beta function) to control
 * high curvature spikes.
 *
 * @return A scalar, representing bending energy or constant \ref energy_error.
 *
 * \todo
 * - Implement perimeter normalization.
 * - Implement extra filtering functions (maybe Gnu Scientific Library could
 * do the trick)?
 *
 */
template <typename COMPLEX_NUMBER>
double bending_energy(COMPLEX_NUMBER *signal, int length, double tau = 8,
		      bool normalize = false, FILTER_TYPE extra_filter = FBETA)
{
	double result = energy_error;
	double *shape_curvature = NULL;

	shape_curvature = contour_curvature(signal, length, tau, normalize,
					    extra_filter);
	if (!shape_curvature)
		goto exit;

	result = energy(shape_curvature, length);
	delete [] shape_curvature;

exit:
	return result;
}


#endif

