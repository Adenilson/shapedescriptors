//#include <math.h>
#include <iostream>
#include <complex>
#include <valarray>
#include <complex.h>
#include "ccomplex.h"

void check4math(void);
void test4ccomplex(void);

int main(int argc, char* argv[])
{
	std::complex<double> c1(4.0, 3.0);
	std::complex<float> c2(std::polar(5.0, 0.75));
	std::complex<float> c3(0, 1);
	std::complex<double> *ptr;


	//Field access
	mcomplex<double> a;
	a = c3;
	std::cout << "a: " << a << std::endl;
	std::cout << "'a' real: " << a[0] << "\timag: " << a[1] << std::endl;

	a(10, 300);
	std::cout << "a: " << a << std::endl;
	a = c1;
	std::cout << "a: " << a << std::endl;

	ptr = &a;
	if (c1 == *ptr)
		std::cout << "Nice\n";
	if (c1 == a)
		std::cout << "Perfect!\n";

	a = c3;
	if ((a * a).real() == -1)
		std::cout << "We are done!\n";

	std::cout << "std::complex size is: "
		  << sizeof(mcomplex<double>) << std::endl;


	//Utility functions
	std::cout << "c1: " << c1 << std::endl;
	std::cout << "c2: " << c2 << std::endl;
	std::cout << "c3: " << c3 << std::endl;


	//ps: magnitude = sqrt(a^2 + b^2)
	//    norma = a^2 + b^2
	std::cout << "c1 magnitude: " << abs(c1) << "\n" <<
		"squared magnitude: " << norm(c1) << "\n" <<
		"phase angle: " << arg(c1) << std::endl;

	std::cout << "c1 conjugated: " << conj(c1) << std::endl;

	//Operations
	std::cout << "4.4 + c1 * 1.8: " << 4.4 + c1 * 1.8 << std::endl;
	std::cout << "c3^2: " << c3 * c3 << std::endl;
	std::cout << "sqrt(c3^2): " << sqrt(c3 * c3) << std::endl;
	if (c3 == sqrt(c3 * c3))
		std::cout << "Fine.\n";
	else
		std::cout << "damned!\n";

	check4math();
	test4ccomplex();
	return 0;
}

void check4math(void)
{
	float i = sqrt(25.333);
	std::cout << i << std::endl;
}

void test4ccomplex(void)
{
	std::cout << "gcc 3.3 && 4.0 support for complex is broken!\n";

	_Complex float c_complex;
/*
	c_complex = cexp(_Complex_I * 1);


	std::cout << "real: " << creal(c_complex)
		  << "imaginary: " << cimag(c_complex) << std::endl;
*/
}
