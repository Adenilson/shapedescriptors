
#ifndef __MCORRELATION__
#define __MCORRELATION__

/*******************************************************************************
Descricao: funcoes (e teste) de calculo das estatisticas (media, variancia,
desvio padrao e coeficiente de correlacao) e auxiliares (quadrado)

Ultima alteracao: Adenilson Cavalcanti

Observacoes: os resultados foram comparados com as funcoes de uma planilha do
OpenOffice.org 2.0 para certificar do correto funcionamento das funcoes de
calculo.
As funcoes foram testadas com sucesso utilizando vetores simples (float *) e
classe container std::vector<TYPE>::interator da STL.

	vs 0.01 14-12-2005
		-- Escritas funcoes 'mean', 'square', 'variance', 'stderror',
'correlation'

	vs 0.02 15-12-2005
		-- Adicionados alguns comentarios com o codigo LaTeX para as
formulas utilizadas

TODO: translate all portuguese text to english.

*******************************************************************************/

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


//Calcula a media simples de um conjunto de dados.
/* 	Parametros
	data = ponteiro para os dados, deve permitir acesso ao conteudo na forma
	data[i]

	size = tamanho do vetor
	vs 0.01 14-12-2005
	-- Escrita versao inicial

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


//Calcula a variancia de um vetor de dados
/* 	Parametros
	data = vetor de dados
	size = tamanho do vetor
	mean_d = a media simples dos dados
	vs 0.01 14-12-2005
	-- Escrita versao inicial


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

//Calcula o desvio padrao, depende de 'variance'
/* Parametros
   data = vetor de dados
   size = tamanho do vetor
   mean_d = a media simples dos dados
   vs 0.01 14-12-2005
   -- Escrita versao inicial

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

//Correlacao (medida da forca de correlacao linear entre 2 variaveis)
/* Parametros
   X = primeiro conjunto de dados
   Y = segundo conjunto de dados
   size = tamanho do vetor
   mean_d = a media simples dos dados

   vs 0.01 14-12-2005
   -- Escrita versao inicial

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

//Mesma coisa, mas exige que jah tenha sido calculadas as medidas
//de posicao (media e desvio) de 1 das variaveis
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
