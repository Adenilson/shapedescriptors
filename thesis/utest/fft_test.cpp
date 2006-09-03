#include "src/fourier.h"
#include "src/ccomplex.h"
using namespace std;

#include <check.h>

START_TEST (transf)
{
	int res = 0;
	int size = 4;
	float v[] = { 1, 1, 1, 1 };
	float V[] = { 4, 0, 0, 0 };

	mcomplex<double> *vobj;
	vobj = new mcomplex<double> [size];

	transform(v, size, vobj);
	for (int i = 0; i < size; ++i)
		if (V[i] != vobj[i].real())
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

