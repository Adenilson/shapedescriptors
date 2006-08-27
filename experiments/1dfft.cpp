/* Copyright 2005 Adenilson Cavalcanti a.k.a. Savago <savagobr@yahoo.com>
   This file is part of the FOX PROJECT.

   FOX PROJECT is free software; you can
   redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   FOX PROJECT is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with the FOX PROJECT.
*/

#include <iostream>
#include <fftw3.h>
#include "ccomplex.h"

/* Use it to define memory allocation */
#define CPP_MALLOC

using namespace std;

template <class type>
void printc(type* v, int size, const char* label = "") {
	cout << label << endl;
	for(int i = 0 ; i < size; ++i) {
		cout << "Real " << *(*(v + i))
		     << "\tImg "  <<*(*(v + i)+1) << endl;
	}
}


void fourier_complex(float *data, int count) {
	fftw_complex *in, *out, *inver;
	fftw_plan fwd_plan, inv_plan;

#ifdef CPP_MALLOC
	in = new fftw_complex[count];
	out = new fftw_complex[count];
	inver = new fftw_complex[count];
#else
	/* Doesnt compile at all! */
	in = (*fftw_complex) fftw_malloc(sizeof(fftw_complex) * count);
	out = (*fftw_complex) fftw_malloc(sizeof(fftw_complex) * count);
	inver = (*fftw_complex) fftw_malloc(sizeof(fftw_complex) * count);
#endif

	//Copies original real data, set to zero imaginary part
	for(int i = 0; i < count; ++i) {
		//real number
		in[i][0] = *(data +i);
		//i - number
		in[i][1] = 0;

	}

	fwd_plan = fftw_plan_dft_1d(count, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fwd_plan);
	printc(out, count, "\nComplex fourier transf.");

	inv_plan = fftw_plan_dft_1d(count, out, inver, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(inv_plan);
	//See fftw manual "4.7 What FFTW Really Computes" p.44
	printc(inver, count, "\nInverse fourier transf. (not normalized!)");

#ifdef CPP_MALLOC
	delete [] in;
	delete [] out;
	delete [] inver;
#else
	fftw_free(in);
	fftw_free(out);
	fftw_free(inver);
#endif

	fftw_destroy_plan(fwd_plan);
	fftw_destroy_plan(inv_plan);
}

void test_compat(void)
{
	mcomplex<double> obj;
	fftw_complex *ptr;

	obj(10, 0.333);
	ptr = reinterpret_cast<fftw_complex *>(&obj);

	cout << "real: " << (*ptr)[0] << "\tcomplex: " << (*ptr)[1] << endl;
	cout << "class output: " << obj << endl;

	(*ptr)[0] = 13.333;
	(*ptr)[1] = 15.152313;
	/* Valid alternate syntax */
	ptr[0][0] = 13.333;
	ptr[0][1] = 15.152313;

	cout << "real: " << *ptr[0] << "\tcomplex: " << (*ptr)[1] << endl;
	cout << "class output: " << obj << endl;


}

void test_compat2(float *v, int count)
{
	fftw_plan fwd_plan, inv_plan;
	fftw_complex *in, *out, *inver;
	mcomplex<double> *vobj;

	vobj = new mcomplex<double> [count];
	out = new fftw_complex[count];

	for (int i = 0; i < count; ++i) {
		vobj[i](v[i], 0);
		cout << "vobj[" << i << "]=" << vobj[i] << endl;
	}

	in = reinterpret_cast<fftw_complex *>(vobj);
	fwd_plan = fftw_plan_dft_1d(count, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fwd_plan);
	printc(out, count, "\nComplex fourier transf.");


	inver = reinterpret_cast<fftw_complex *>(vobj);
	inv_plan = fftw_plan_dft_1d(count, out, inver, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(inv_plan);
	printc(inver, count, "\nInverse fourier transf. (not normalized!)");

	delete [] out;
	delete [] vobj;
	fftw_destroy_plan(fwd_plan);
	fftw_destroy_plan(inv_plan);
}



int main (void){

	//Some vector samples
	int count = 4;
	float v[] = { 1, 1, 1, 1 };
	//float v[] = { 1, 2, 3, 4, 5};
	//float v[] = { 11, 222, 3333, 4444, 5555, 6666};

	cout << "normal way..." << endl;
	fourier_complex(v, count);

	cout << "fftw_complex size is: " << sizeof(fftw_complex)
	     << endl;
	test_compat();
	cout << "casting way!" << endl;
	test_compat2(v, count);
	return 0;
}
