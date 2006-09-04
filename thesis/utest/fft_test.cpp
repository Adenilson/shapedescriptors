/** @file
 *
 * Fourier unit test module
 *
 *
 * Copyright 2006
 * @author Adenilson Cavalcanti <savagobr@yahoo.com>
 *
 * @version
 */
#include "src/fourier.h"
#include "src/ccomplex.h"
using namespace std;

#include <check.h>

START_TEST (invert)
{
	int res = 0;

	//Original data
	int size = 5;
	float v[] = { 1, 1, 1, 1, 1 };
	float V[] = { 5, 0, 0, 0, 0 };

	//Complex number to hold results
	mcomplex<double> *t_obj, *T_obj;
	t_obj = new mcomplex<double> [size];
	T_obj = new mcomplex<double> [size];
	for (int i = 0; i < size; ++i)
		T_obj[i](V[i], 0);

	//Transform and check for expected results
	invert(T_obj, size, t_obj);
	for (int i = 0; i < size; ++i)
		if (v[i] != (t_obj[i].real()/size))
			res = 1;

	fail_unless(res == 0, "failed invert transform");

}
END_TEST


START_TEST (transf)
{
	int res = 0;

	//Original data
	int size = 4;
	float v[] = { 1, 1, 1, 1 };
	float V[] = { 4, 0, 0, 0 };

	//Complex number to hold results
	mcomplex<double> *t_obj, *T_obj;
	t_obj = new mcomplex<double> [size];
	T_obj = new mcomplex<double> [size];
	for (int i = 0; i < size; ++i)
		t_obj[i](v[i], 0);

	//Transform and check for expected results
	transform(t_obj, size, T_obj);
	for (int i = 0; i < size; ++i)
		if (V[i] != T_obj[i].real())
		    res = 1;

	fail_unless(res == 0, "failed transform");

}
END_TEST


Suite *test_suite(void);
Suite *test_suite(void)
{
	Suite *s = suite_create("test_dft");
	TCase *test_case = tcase_create("transf_test_case");

	suite_add_tcase(s, test_case);
	tcase_add_test(test_case, transf);
	tcase_add_test(test_case, invert);
	return s;
}


int main(void)
{
	int nf;
	Suite *s = test_suite();
	SRunner *sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	nf = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (nf == 0) ? 0 : -1;
}

