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
	fftw_plan fwdPlan, invPlan;
		
	in = new fftw_complex[count];
	out = new fftw_complex[count];
	inver = new fftw_complex[count];
	
	/* Doesnt compile at all!
	in = (*fftw_complex) fftw_malloc(sizeof(fftw_complex) * count);
  out = (*fftw_complex) fftw_malloc(sizeof(fftw_complex) * count);
  inver = (*fftw_complex) fftw_malloc(sizeof(fftw_complex) * count);
	*/
	
	//Copies original real data, set to zero imaginary part
	for(int i = 0; i < count; ++i) {
		//real number
		in[i][0] = *(data +i);
		//i - number
		in[i][1] = 0;
	 
	}

	fwdPlan = fftw_plan_dft_1d(count, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fwdPlan);		
	printc(out, count, "\nComplex fourier transf.");	

	invPlan = fftw_plan_dft_1d(count, out, inver, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(invPlan);
	//See fftw manual "4.7 What FFTW Really Computes" p.44
	printc(inver, count, "\nInverse fourier transf. (not normalized!)");
	
	//fftw_free(in); fftw_free(out); fftw_free(inver);
	delete [] in; delete [] out; delete [] inver;
	fftw_destroy_plan(fwdPlan); fftw_destroy_plan(invPlan);
}

/* I also tested 'mffm' but don't see any significant
 * advantage for easy of coding.
void inverse_mffm(float *data, int count) {
	realFFTData vector(count);		
	realFFT fmaker(&vector);

	for(int i = 0; i < count; ++i)
		vector.in[i] = *(data + i);
	//Do transformation, representation is 'halfcomplex' see fftw manual
	//"4.3.6 Real-to-Real Transform Kinds" p. 35
	//r0, r1, . . ., rn/2, i(n+1)/2âˆ’1, . . ., i2, i1
	fmaker.fwdTransform();	
	print(vector.in, count, "v = original input");
	print(vector.out, count, "\nV = fwd transform output");

	fftw_real *swap = vector.in;
	vector.in = vector.out;
	vector.out = swap;	

	//Inverse
	fmaker.invTransform();
	print(vector.in, count, "\nV = swap out fwd input");
	print(vector.out, count, "\nG = inv transform output  (should be same as v)");
}
*/
int main (void){
	
	//Some vector samples
  int count = 4; 
  float v[] = { 1, 1, 1, 1 };
  //float v[] = { 1, 2, 3, 4, 5}; 
  //float v[] = { 11, 222, 3333, 4444, 5555, 6666};
  
  //inverse_mffm(v, count);  
  fourier_complex(v, count);
  
  return 0;
}

/*
void fourier_real(float *data, int count) {
	fftw_real *in, *out;
	in = new fftw_real[count];
	out = new fftw_real[count];
	
	for(int i = 0; i < count; ++i)
		in[i] = *(data +i);
	 
	
	fftw_plan fwdPlan;
	fwdPlan=fftw_plan_r2r_1d(count, in, out, FFTW_DHT, FFTW_ESTIMATE);
	fftw_execute(fwdPlan);
	fftw_destroy_plan(fwdPlan);
	
	print(out, count, "calculado real Hartley");
	
	delete [] in;
	delete [] out;
}
*/
