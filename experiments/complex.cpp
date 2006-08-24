//#include <math.h>
#include <iostream>
#include <complex>
#include <valarray>

using namespace std;

void check4math(void);

int main(int argc, char* argv[])
{
	complex<double> c1(4.0, 3.0);
	complex<float> c2(polar(5.0, 0.75));
	complex<float> c3(0, 1);


	//Field access??
/*
	c3.real(0);
	c3.complex(1);
*/
	//Utility functions
	cout << "c1: " << c1 << endl;
	cout << "c2: " << c2 << endl;
	cout << "c3: " << c3 << endl;

	//ps: magnitude = sqrt(a^2 + b^2)
	//    norma = a^2 + b^2
	cout << "c1 magnitude: " << abs(c1) << "\n" <<
		"squared magnitude: " << norm(c1) << "\n" <<
		"phase angle: " << arg(c1) << endl;

	cout << "c1 conjugated: " << conj(c1) << endl;

	//Operations
	cout << "4.4 + c1 * 1.8: " << 4.4 + c1 * 1.8 << endl;
	cout << "c1^2: " << c3 * c3 << endl;
	cout << "sqrt(c1^2): " << sqrt(c3 * c3) << endl;
	if (c3 == sqrt(c3 * c3))
		cout << "Fine.\n";
	else
		cout << "damned!\n";

	check4math();

	return 0;
}

void check4math(void)
{
	float i = sqrt(25.333);
	cout << i << endl;
}
