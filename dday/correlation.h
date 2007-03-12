/**
 * @file   correlation.h
 * @author Adenilson Cavalcanti <savagobr@yahoo.com> Copyright 2005
 * @date   ending year, 2005
 *
 * @brief  Functions to calculate statistics (sample mean, variance,
 * standard error, correlation coefficient).
 * Results are compared against an OpenOffice.org 2.0 to garantee that
 * everything is fine.
 * Functions were tested using simple vector pointers (float *) as also
 * STL container class std::vector<float>::iterator.
 * vs 0.01 14-12-2005
 * - Wrote mean, square, variance, stderror, correlaton;
 * - Added comments LaTeX with formulas of functions;
 *
 * \todo
 * - Is there a way so Doxygen translate LaTeX embedded formulas?
 */

#ifndef __MCORRELATION__
#define __MCORRELATION__


/** Square of number
 *
 * @param i A number to be squared.
 *
 * @return Squared number.
  *
 */
/* FIXME: how to insert more information in doxygen docs?
	Formula LaTeX
	\begin{equation}
	X^2 = X * X
	\end{equation}
*/
template <typename T>
inline T square(T num)
{
	return num * num;
}


/** Calculates sample mean of a given data vector.
 *
 * @param data Pointer/iterator to data vector, should allow access to elements
 * using data[i].
 *
 * @param size Vector size.
 *
 * @return The mean.
 */
/*
	Formula LaTeX
	\begin{equation}
	\mu = \overline{x} = \frac{\sum_{i = 0}^{n - 1} (x _i)}{n}
	\end{equation}

*/
template <class T>
inline float mean(T *data, int size)
{
	float result = 0;
	for (int i = 0; i < size; ++i)
		result += data[i];

	result /= float(size);

	return result;
}


/** Calculates variance of a data vector.
 *
 * @param data Pointer/iterator to data vector, should allow access to elements
 * using data[i].
 *
 * @param size Vector size.
 *
 * @param mean_d The sample mean, should be calculated before using \ref mean.
 *
 * @return The variance.
 */
/*
  Formula LaTeX
	\begin{equation}
	\sigma ^2 = \frac{\sum _{0}^{n-1} (x_i - \overline{x})^2}{n - 1}
	\end{equation}

*/
template <class T>
inline float variance(T *data, int size, float mean_d)
{
	float result = 0;
	for (int i = 0; i < size; ++i)
		result += square(float(data[i]) - mean_d);

	result /= float(size - 1);

	return result;
}

/** Standard error, root square of variance.
 *
 * @param data Pointer/iterator to data vector, should allow access to elements
 * using data[i].
 *
 * @param size Vector size.
 *
 * @param mean_d Sample mean.
 *
 * @return The standard error.
 */

/*
   Formula LaTeX
   \begin{equation}
   \sigma = \sqrt{\sigma ^ 2} = \sqrt{\frac{\sum _{0}^{n-1} (x_i - \overline{x})^2}{n - 1}}
   \end{equation}

*/
template <class T>
inline float stderror(T *data, int size, float mean_d)
{
	float result = 0;

	result = variance(data, size, mean_d);
	result = float(sqrt(double(result)));

	return result;

}

/** Statistical correlation.
 *
 * The strength of linear relationship between 2 variables.
 *
 * @param X The first data set vector, pointer/iterator should allow access in form
 * X[i].
 *
 * @param Y Second data set vector.
 *
 * @param size Vectors length (both needs to have same length).
 *
 * @return The correlation.
 */

/*
  Formula LaTeX
   \begin{equation}
   r _{xy} = \frac{\sum (x_i - \overline{x}) (y_i - \overline{y})}{(n -1) \sigma _x \sigma _y}
   \end{equation}
*/
template <class T1, class T2>
inline float correlation(T1 *X, T2* Y, int size)
{
	float x_mean = 0, y_mean= 0;
	float x_desv = 0, y_desv = 0;

	float corr = 0, ratio = 0;


	x_mean = mean(X, size);
	y_mean = mean(Y, size);

	x_desv = stderror(X, size, x_mean);
	y_desv = stderror(Y, size, y_mean);

	ratio = (size - 1) * (x_desv * y_desv);
	for (int i = 0; i < size; ++i)
		corr += (X[i] - x_mean) * (Y[i] - y_mean);

	corr /= ratio;

	return corr;
}

/** Helper structure to hold position statistics, is used to speed up
 * iterative calculus (think about video) when 1 of data sets *doesn't*
 * change in time.
 */
struct moments
{
	float s_mean;
	float s_var;
	float s_error;

	moments(void): s_mean(0), s_var(0), s_error(0)
	{}

	moments(float &sample_mean, float &s_variance, float &std_error):
		s_mean(sample_mean), s_var(s_variance), s_error(std_error)
	{}

	void operator() (float &sample_mean, float &s_variance, float &std_error)
	{
		s_mean = sample_mean;
		s_var = s_variance;
		s_error = std_error;
	}

};

/** Statisticial correlation with some speed up, see \ref correlation and
 * \ref moments.
 *
 * @param The first data set vector, pointer/iterator should allow access in form
 * X[i].
 *
 * @param mdata Statitical moments of first data set (the one that doesn't change
 * in time).
 *
 * @param Y Second data set vector.
 *
 * @param size Data set vector's length.
 *
 * @return The correlation.
 */
template <class T1, class T2>
float quick_corr(T1* X, moments &mdata, T2* Y, int size)
{
	float &x_mean = mdata.s_mean, y_mean= 0;
	float &x_desv = mdata.s_error, y_desv = 0;

	float corr = 0, ratio = 0;

	y_mean = mean(Y, size);
	y_desv = stderror(Y, size, y_mean);

	ratio = (size - 1) * (x_desv * y_desv);
	for(int i = 0; i < size; ++i)
		corr += (X[i] - x_mean) * (Y[i] - y_mean);

	corr /= ratio;

	return corr;
}

#endif
