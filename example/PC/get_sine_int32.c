/*

Calculate sine function with integer arithmetic operation


victoragarkov@gmail.com


*/

#include "get_sine_int32.h"
// #include <stdio.h>

//-------------------------------------------------------------------------------------------------------
int32_t get_sine_int32(const int32_t* table, int32_t pN, int pw, int ac_shift, uint32_t angle)
{
	// angle is 0 .. 0xffffffff is 0 to 359.9999 degrees
	// ((a[0] * x + a[1]) * x + a[2]) * x + a[3]

	#ifdef Q1_ONLY
		
		// printf("Q1_ONLY; pre_angle = 0x%08x, ", angle);
		int Q = angle >> 30;
		angle &= 0x3fffffff;
		if(Q & 1) // quadrant 2 ot 4
		{
			angle = 0x3fffffff - angle;
		}
		// printf("Q = %i, ", Q);
	#endif
	int idx = angle >> (32 - pN); // index of string in table
	uint32_t X = (angle << pN) >> ac_shift; // дробная часть угла (фикс. точка 0.32)
	const int32_t* A = table + idx * (pw + 1); // ptr to string in table

	// printf("angle = 0x%08x, idx = %i, X = 0x%08x\n", angle, idx, X);

	int64_t sum = A[0];
	for(int i = 1; i <= pw; i++)
	{
		sum *= X;
		sum >>= 32;
		sum += A[i];
	}
	
	#ifdef Q1_ONLY
		if(Q & 2) // quadrant 3 ot 4
		{
			sum = -sum; // negative result
		}
	#endif
	
	return sum;
}

//-------------------------------------------------------------------------------------------------------
