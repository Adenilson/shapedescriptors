//#include <math.h>
#include <iostream>
#include <complex>
#include <valarray>
#include <complex.h>
void check4math(void);
void test4ccomplex(void);

/** Complex type, we derive it from C++ STL type complex, added
 * 2 more operators to access number parts: set with 'number(real, imag)'
 * and get with 'number[0] || number[1]'.
 * At present, we use complex<T>::real() to access number parts (at least
 * it is inline...).
 * Dependes on STL header <complex>.
 */
template <class NUMBER>
class mcomplex: public complex<NUMBER> {
public:

	/** Extra operator, make it easier to set values
	 * into a complex number.
	 *
	 * @param real A real/integer number
	 * @param img  A real/integer number for imaginary part
	 *             of complex number
	 *
	 */
	template <class TYPE>
	mcomplex<NUMBER> &operator()(TYPE _real, TYPE _img)
	{
		complex<NUMBER>::real() = _real;
		complex<NUMBER>::imag() = _img;
		return *this;
	}

	/** Defines a simple structure based complex type compatibility layer.
	 *
	 * @param i Access to number index (0 == real, 1 == imaginary).
	 *
	 * @return number or NULL on invalid index.
	 */
	NUMBER operator[](int i) {
		if (i == 0)
			return complex<NUMBER>::real();
		else if (i == 1)
			return complex<NUMBER>::imag();

		return NULL;
	}


	/** Overloaded operator, to do implicit convertion.
	 *
	 * @param c A complex number, should have methods
	 *          c.real() and c.imag()
	 *
	 * @return A reference for this object
	 */
	template <class TYPE>
	mcomplex<NUMBER> &operator=(std::complex<TYPE> &c) {
		complex<NUMBER>::real() = c.real();
		complex<NUMBER>::imag() = c.imag();
		return *this;
	}

};


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
