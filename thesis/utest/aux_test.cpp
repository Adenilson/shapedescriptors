/**
 * @file   aux_test.cpp
 * @author Adenilson Cavalcanti
 * @date   Fri Feb  2 13:53:13 2007
 *
 * @brief  Auxiliary unit test.
 *
 * This file will hold class/types convertion tests,
 * and other stuff not directly related with
 * fourier + computer vision.
 *
 */
#include "src/adaptors.h"
#include <iostream>
#include <fstream>
using namespace std;

#include <check.h>


//Test for transform
START_TEST (t_ocv_adapt)
{

	CvSeq *tmp = NULL;
	ocv_adaptor<double> handler(tmp);

	handler.reset(NULL, DESTROY);
	/* How to handler invalid return objects implicitly? */
	//mcomplex<double> obj = NULL;

	cout << "\n\n" <<
		"handler[0][0] = " << handler[0][0] << "\n" <<
		"handler[0][1] = " << handler[0][1] << endl;

}
END_TEST



Suite *test_suite(void);
Suite *test_suite(void)
{
	Suite *s = suite_create("test_dft");
	TCase *test_case = tcase_create("transf_test_case");

	suite_add_tcase(s, test_case);
	tcase_add_test(test_case, t_ocv_adapt);
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

