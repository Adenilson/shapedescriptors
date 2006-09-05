#include <math.h>
#include <complex>
#include <iostream>
using namespace std;

#define PI 3.141593

int main(void)
{
	complex<double> *data;
	int length = 256;
	double step = 2 * PI /(length - 1);
	double pos = 0;
	data = new complex<double> [length];

	for (int i = 0; i < length; ++i) {
		data[i].real() = pos;
		pos += step;
	}

	for (int i = 0; i < length; ++i) {
		cout << data[i] << "\t";
		data[i] = sin(data[i]);
		cout << i << "\t" << data[i].real() << endl;
	}

	delete [] data;
}
