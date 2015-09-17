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
 * \todo
 * Create utests to refactor contour/descriptor code
 * to use ocv_adaptor.
 *
 */
#include "src/adaptors.h"
#include "src/contour.h"
#include "src/vision.h"
#include "src/fourier.h"
#include "src/descriptors.h"
#include <iostream>
#include <fstream>
using namespace std;

#include <check.h>

const char shapes_path[] = "../data/shapes.jpg";
const int thres_value = 160;

CvSeq *find_contour_image(CvMemStorage *store, int *num_contours)
{
	CvSeq *result = NULL;
	IplImage *image, *gray, *thres;

	image = cvLoadImage(shapes_path, 1);
	fail_unless(image != NULL, "Failed to load image");
	gray = cvCreateImage(cvSize(image->width,image->height),
			     IPL_DEPTH_8U, 1);
	thres = cvCreateImage(cvSize(image->width,image->height),
			      IPL_DEPTH_8U, 1);

	cvCvtColor(image, gray, CV_BGR2GRAY);
	threshold(thres_value, gray, thres);
	result = contour_follow(thres, store, num_contours);

	cvReleaseImage(&image);
	cvReleaseImage(&gray);

	return result;

}

//Test for transform
START_TEST (t_ocv_adapt)
{

	CvSeq *sequence = NULL;
	int num_contours;
	CvMemStorage* storage = cvCreateMemStorage(0);
	ocv_adaptor<double> handler;

	sequence = find_contour_image(storage, &num_contours);
	handler.reset(sequence);
	/* How to handler invalid return objects implicitly? */
	//mcomplex<double> obj = NULL;

	fail_unless(num_contours == handler.contour_number(),
		    "Adaptor failed to count number of shapes!");
	fail_unless(handler.next() == 1, "Failed to advance to next shape \
		contour");


	if (handler.contour_length() > 10)
		fail_unless((handler[0][0] == handler[0][0]) &&
			    (handler[10][0] == handler[10][0]) &&
			    (handler[10][0] != handler[10][1]) &&
			    (handler[1][0] == handler[1][0]) &&
			    (handler[1][1] == handler[1][1]),
			    "Adaptor sequence access is faulty!");

}
END_TEST


START_TEST (t_adapt_access)
{
	CvSeq *sequence = NULL;
	int num_contours, length, i, counter = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	ocv_adaptor<double> handler;
	m_point *tmp_contour;

	sequence = find_contour_image(storage, &num_contours);
	handler.reset(sequence);


	do {
		tmp_contour =  points(sequence, &length);
		fail_unless(tmp_contour != NULL, "Failed to copy contour!");
		for (i = 0; i < length; ++i) {
			fail_unless(tmp_contour[i].x == handler[i][0],
				    "Error when comparing points: real part!");
			fail_unless(tmp_contour[i].y == handler[i][1],
				    "Error when comparing points: imag part!");
#ifdef F_DEBUG
			cout << "contour = " << counter <<
			  "\ti = " << i << "\treal = " << handler[i][0] <<
			  "\timag = " << handler[i][1] << endl;
#endif
		}

		++counter;
		delete [] tmp_contour;

		sequence = sequence->h_next;
		handler.next();
	} while (counter < num_contours);

}
END_TEST

START_TEST (t_adapt_curvature)
{

	CvSeq *sequence = NULL;
	int num_contours, i = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	ocv_adaptor<int> handler;
	double *curvature = NULL;
	double tau = 10.0, c_energy;
	const char *shape_names[] = { "square.txt", "circle.txt",
				     "elipse.txt", "star.txt" };
	ofstream fout;

	sequence = find_contour_image(storage, &num_contours);
	handler.reset(sequence);

	do {
		curvature = contour_curvature<ocv_adaptor<int>,
		  mcomplex<double> >(handler, handler.contour_length(), tau);
		fail_unless(curvature != NULL, "Failed to calculate curvature!");
		c_energy = energy(curvature, handler.contour_length());

		if ((0 < i) && (i < 4)) {
			fout.open(shape_names[i]);
			fout << "index\tcurvature" << endl;
			for (int z = 0; z < handler.contour_length(); ++z)
				fout << z << "\t" << curvature[z] << endl;
			fout.close();
		}
		delete [] curvature;

#ifdef F_DEBUG
		cout << "\ni = " << i <<  "\tenergy = " << c_energy <<
			"\tlength = " << handler.contour_length() << endl;
#endif
		++i;

	} while (handler.next() == 1);

}
END_TEST


Suite *test_suite(void);
Suite *test_suite(void)
{
	Suite *s = suite_create("test_adaptor");
	TCase *test_case = tcase_create("transf_test_case");
	tcase_set_timeout (test_case, 10);
	suite_add_tcase(s, test_case);
	tcase_add_test(test_case, t_ocv_adapt);
	tcase_add_test(test_case, t_adapt_curvature);
	tcase_add_test(test_case, t_adapt_access);

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

