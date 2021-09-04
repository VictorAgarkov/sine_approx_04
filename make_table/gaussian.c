#include <string.h>
#include <stdlib.h>
#include "gaussian.h"

int matrix_gaussian(T *src, T *dst, int N)
{
	// решаем систему линейных арифметических уравнений (СЛАУ) методом Гаусса
	// пример системы с N = 3:
	// a11*x1 + a12*x2 + a13*x3 = b1
	// a21*x1 + a22*x2 + a23*x3 = b2
	// a31*x1 + a32*x2 + a33*x3 = b3
	// src - матрица N строк х (N+1) столбцов, записанная построчно:
	//     a11, a12, a13, b1, a21, a22 .... a32, a33, b3
	// dst - набор решений: x1, x2, x3
	// возврат:
	// 0 - успешное решение
	// 1 - система не имеет решения
	// 2 - разные проблемы

	T *pi, *pj;
	int i, j, k;
	int ret = 0;
	T buff[N + 1]; // буфер для обмена строк


	// прямой ход - приводим матрицу к верхнетреугольному виду
	for(i = 0; i < N; i++)
	{
		pi = src + i * (N + 1);  // начало очередной строки

		// приводим первый коэфф в строке к 1.
		if(pi[i] == 0.0)
		{
			// если коэфф. нулевой - пытаемся поменять строки
			for(j = i + 1; j < N; j++)
			{
				T *ff = src + j * (N + 1);
				if(ff[j + i] != 0)
				{
					void *p1 =  pi + i;
					void *p2 = ff + j + i;
					int sz = (N+1 - i) * sizeof(T);
					// меняем строки местами
					memcpy(buff, p2,   sz);
					memcpy(p2,   p1,   sz);
					memcpy(p1,   buff, sz);
					break;
				}
			}
			if(j == N)
			{ // матрица не имеет единственного решения
				return  1;
			}
		}
		T ka = 1.0 / pi[i];
		pi[i] = 1.0;
		for(j = i + 1; j <= N; j++)
		{
			pi[j] *= ka;
		}

		// вычитаем текущую строку из следующей, приводя ту к виду 0, .....  0, 1, a, ....
		for(j = i + 1; j < N; j++)
		{
			pj = src + j * (N + 1);
			ka = pj[i];

			for(k = i; k <= N; k++)
			{
				pj[k] -= ka * pi[k];
			}
		}
	}

	// обратный ход - находим искомые коэффициенты
	for(i = N - 1; i >= 0; i--)
	{
		pi = src + i * (N + 1);
		dst[i]  = pi[N];
		for(j = i + 1; j < N; j++)
		{
			dst[i] -= pi[j] * dst[j];
		}
	}

    return ret;
}
//--------------------------------------------------------------------------------------------------------