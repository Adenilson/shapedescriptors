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

	if (handler.contour_length() > 10)
		fail_unless((handler[0][0] == handler[0][0]) &&
			    (handler[10][0] == handler[10][0]) &&
			    (handler[10][0] != handler[10][1]) &&
			    (handler[1][0] == handler[1][0]) &&
			    (handler[1][1] == handler[1][1]),
			    "Adaptor sequence access is faulty!");

}
END_TEST



Suite *test_suite(void);
Suite *test_suite(void)
{
	Suite *s = suite_create("test_adaptor");
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

