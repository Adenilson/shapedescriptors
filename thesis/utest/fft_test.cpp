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

struct function_param
{
	mcomplex<double> *signal;
	mcomplex<double> *transf;
	int size;
	pthread_mutex_t *mutex;
};

void *thread_transform(void *param)
{
	function_param *obj = (function_param *) param;
	transform(obj->signal, obj->size, obj->transf, obj->mutex);
	return NULL;

}

void *thread_inverse(void *param)
{
	function_param *obj = (function_param *) param;
	invert(obj->signal, obj->size, obj->transf, obj->mutex);
	return NULL;

}

START_TEST (thread_transf)
{
	int res = 0, tid;
	pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
	pthread_t thread1;
	function_param parameters;

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


START_TEST (thread_inver)
{
	int res = 0, tid;
	pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
	pthread_t thread1;
	function_param parameters;

	//Original data
	int size = 4;
	float v[] = { 1, 1, 1, 1 };
	float V[] = { 4, 0, 0, 0 };

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
	tcase_add_test(test_case, thread_transf);
	tcase_add_test(test_case, thread_inver);
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

