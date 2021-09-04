/*

Calculate sine function with float arithmetic operation


victoragarkov@gmail.com


*/

#include "get_sine_float.h"
// #include <stdio.h>

//-------------------------------------------------------------------------------------------------------
TABLE_TYPE get_sine_float(const TABLE_TYPE* table, int32_t pN, int pw, int ac_shift, uint32_t angle)
{
	// вычисляем синус угла angle, используя таблицу table, состоящей из
	// 2^pN строк и полином степени pw
	// angle делится на целую часть (pN бит) и дробную часть - смещение внутри отрезка
	// Вычисляем значение степенным рядом типа:
	// ((a[0] * x + a[1]) * x + a[2]) * x + a[3]

	int idx = angle >> (32 - pN); // индекс строки в таблице (целая часть)
	TABLE_TYPE X = (angle << pN) >> ac_shift; // дробная часть угла (фикс. точка 0.32)
	X /= 0x100000000LL;
	const TABLE_TYPE* A = table + idx * (pw + 1); // указатель на нужную строку в таблице

	TABLE_TYPE sum = A[0];
	for(int i = 1; i <= pw; i++)
	{
		sum *= X;
		sum += A[i];
	}
	return sum;
}

//-------------------------------------------------------------------------------------------------------
