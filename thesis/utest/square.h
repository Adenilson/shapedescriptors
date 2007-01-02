#ifndef __T_SQUARE__
#define __T_SQUARE__


#include "src/mcomplex.h"
#include <iostream>

/** Creates contour of a square.
 *
 * @param length Pointer to vector length.
 *
 *
 * @return Vector of complex objects with coordinates of square.
 *
 * TODO: make it parameterized (given size and p1, calculate square).
 */
mcomplex<double> *create_square(int *length)
{

	int i, t = 0;
	*length = 120;
	mcomplex<double> *signal = new mcomplex<double> [*length];
	if (!signal)
		return NULL;

	//XXX: hardcoded, this is not good.
	int p1 = 0, p2 = 40, p3 = 60, p4 = 100;
	signal[p1][0] = 10;
	signal[p1][1] = 10;

	signal[p2][0] = 50;
	signal[p2][1] = 10;

	signal[p3][0] = 50;
	signal[p3][1] = 30;

	signal[p4][0] = 10;
	signal[p4][1] = 30;

	//P1 --> P2
	t = (int) (signal[p2][0] - signal[p1][0]);
	for (i = 1; i < t; ++i) {
		signal[i][0] = signal[p1][0] + i;
		signal[i][1] = signal[p1][1];
	}
	t = i;


	//P2 --> P3
	for (i = 1; i < (signal[p3][1] - signal[p2][1]); ++i) {
		signal[i+t][0] = signal[p2][0];
		signal[i+t][1] = signal[p2][1] + i;
	}
	t += i;


	//P3 --> P4
	for (i = 1; i < (signal[p3][0] - signal[p4][0]); ++i) {
		signal[i+t][0] = signal[p3][0] - i;
		signal[i+t][1] = signal[p3][1];
	}
	t += i;


	//P4 --> P1
	for (i = 1; i < (signal[p4][1] - signal[p1][1]); ++i) {
		signal[i+t][0] = signal[p4][0];
		signal[i+t][1] = signal[p4][1] - i;
	}


	for (int i = 0; i < *length; ++i)
		std::cout << "i = " << i << "\t" << signal[i] << std::endl;

	return signal;

}
#endif
