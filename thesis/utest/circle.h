/**
 * @file   circle.h
 * @author Adenilson Cavalcanti
 * @date   Fri Jan 26 08:58:41 2007
 *
 * @brief  Helper test module, it includes a function to
 *        create a circle contour.
 *
 */
#ifndef __T_CIRCLE__
#define __T_CIRCLE__


#include "src/mcomplex.h"


/** Creates a circle contour.
 *
 *
 * @param length Circle perimeter
 *
 * @return Vector fo complex objects with coordinates of circle
 *        (equivalent to parametric curve p(t) = {sin(t), cos(t)} )
 *        or NULL in error case.
 */
mcomplex<double> *create_circle(int length)
{


	double step = 2 * PI/(length - 1);
	double position = 0;
	mcomplex<double> *circle_curve = NULL;
	int i;

	circle_curve = new mcomplex<double> [length];
	if (!circle_curve)
		goto exit;

	for (i = 0; i < length; ++i) {
		position += step;
		circle_curve[i][0] = sin(position);
		circle_curve[i][1] = cos(position);
	}

exit:
	return circle_curve;
}

#endif
