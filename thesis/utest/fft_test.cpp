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
#include <iostream>
using namespace std;

#include <check.h>


//Original data
int size = 4;
float v[] = { 1, 1, 1, 1 };
float V[] = { 4, 0, 0, 0 };

//Aux struct to hold parameters to thread functions
struct function_param
{
	mcomplex<double> *signal;
	mcomplex<double> *transf;
	int size;
	pthread_mutex_t *mutex;
};

//Do fft transform
void *thread_transform(void *param)
{
	function_param *obj = (function_param *) param;
	transform(obj->signal, obj->size, obj->transf, obj->mutex);
	return NULL;

}

//Do inverse fft transform
void *thread_inverse(void *param)
{
	function_param *obj = (function_param *) param;
	inverse(obj->signal, obj->size, obj->transf, obj->mutex);
	return NULL;

}

//Tests for shift operation
START_TEST (tshift)
{

	double v1[] = { 0, 1, 2, 3 };
	double sv1[] = { 2, 3, 0, 1 };
	double v2[] = { 0, 1, 2, 3, 4 };
	double sv2[] = { 3, 4, 0, 1, 2 };
	double *tmp;
	int length;
	int res = 0;

	length = sizeof(v1);
	tmp = shift(v1, length);
	fail_unless(tmp != NULL, "failed function call");
	for (int i = 0; i < length; ++i)
		if (tmp[i] != sv1[i])
			res = 1;
	fail_unless(res == 0, "failed shift tmp != sv1");

	delete [] tmp;

	length = sizeof(v2);
	tmp = shift(v2, length);
	fail_unless(tmp != NULL, "failed function call");
	for (int i = 0; i < length; ++i)
		if (tmp[i] != sv2[i])
			res = 1;
	fail_unless(res == 0, "failed shift tmp != sv2");

	delete [] tmp;
}
END_TEST


//Tests diferentiate calculus with fourier
START_TEST (diff)
{

	complex<double> *time;
	mcomplex<double> *g_sin, *g_cos, *g_transf;
	int length = 256;
	int diff_level = 1;
	double tolerance = 0.05;
	double step = 2 * PI /(length - 1);
	double pos = 0;
	int res = 1;

	time = new complex<double> [length];
	g_sin = new mcomplex<double> [length];
	g_cos = new mcomplex<double> [length];
	g_transf = new mcomplex<double> [length];

	for (int i = 0; i < length; ++i) {
		time[i].real() = pos;
		//mcomplex operators at rescue!
		g_sin[i][0] = sin(pos);
		g_cos[i][0] = cos(pos);
		pos += step;
	}

	differentiate(g_sin, length, g_transf, diff_level);
	for (int i = 0; i < length; ++i)
		if (((tolerance * g_cos[i][0]) < g_transf[i][0]) &&
		    (((tolerance + 1) * g_cos[i][0]) > g_transf[i][0]))
			res = 0;

	fail_unless(res == 0, "differentiate out of acceptable values");

	delete [] time;
	delete [] g_sin;
	delete [] g_cos;
	delete [] g_transf;

}
END_TEST

//Tests for thread safe transform.
START_TEST (thread_transf)
{
	int res = 0, tid;
	pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
	pthread_t thread1;
	function_param parameters;

	//Complex number to hold results
	mcomplex<double> *t_obj, *T_obj;
	t_obj = new mcomplex<double> [size];
	T_obj = new mcomplex<double> [size];
	for (int i = 0; i < size; ++i)
		t_obj[i](v[i], 0);

	//Create thread and pass parameters
	parameters.signal = t_obj;
	parameters.transf = T_obj;
	parameters.mutex = &mutex1;
	parameters.size = size;
	tid = pthread_create(&thread1, NULL, thread_transform, &parameters);
	if (tid) {
		fail_unless(true, "transform(thread): failed thread creation!");

	} else {
		//Wait for thread and check for expected results
		pthread_join(thread1, NULL);
		for (int i = 0; i < size; ++i)
			if (V[i] != T_obj[i].real())
				res = 1;

		fail_unless(res == 0, "failed transform");
	}

}
END_TEST

//Tests for thread safe backward transform.
START_TEST (thread_inver)
{
	int res = 0, tid;
	pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
	pthread_t thread1;
	function_param parameters;

	//Complex number to hold results
	mcomplex<double> *t_obj, *T_obj;
	t_obj = new mcomplex<double> [size];
	T_obj = new mcomplex<double> [size];
	for (int i = 0; i < size; ++i)
		T_obj[i](V[i], 0);

	//Create thread and pass parameters
	parameters.signal = T_obj;
	parameters.transf = t_obj;
	parameters.mutex = &mutex1;
	parameters.size = size;
	tid = pthread_create(&thread1, NULL, thread_inverse, &parameters);
	if (tid) {
		fail_unless(true, "transform(thread): failed thread creation!");

	} else {
		//Wait for thread and check for expected results
		pthread_join(thread1, NULL);
		for (int i = 0; i < size; ++i)
			if (v[i] != t_obj[i].real()/size)
				res = 1;

		fail_unless(res == 0, "failed transform");
	}

}
END_TEST

//Test for backward transform
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
	inverse(T_obj, size, t_obj);
	for (int i = 0; i < size; ++i)
		if (v[i] != (t_obj[i].real()/size))
			res = 1;

	fail_unless(res == 0, "failed inverse transform");

}
END_TEST

//Test for transform
START_TEST (transf)
{
	int res = 0;

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
	tcase_add_test(test_case, thread_transf);
	tcase_add_test(test_case, thread_inver);
	tcase_add_test(test_case, diff);
	tcase_add_test(test_case, tshift);
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
