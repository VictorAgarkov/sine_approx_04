#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "get_sine_int32.h"
#include "get_sine_float.h"

extern const TABLE_TYPE sine_approx_table_64_deg_3_acs_3[64*4];

#ifndef AC0
	#define AC0 0x40000000LL
#endif // AC0


const char *msg_help = 
{
	"\n"
	"Sine integet interpolation example.\n"
	"Used 64 interpolation segments with 3th degree Chebyshev polinomial.\n"
	"Usage:\n"
	"sine_approx_example {d|h} <angle>\n"
	"where:\n"
	"d - <angle> in degrees, 0 to 360\n"
	"h - <angle> in hexadecimal, 0 to ffffffff\n"
	"\n"
};
//-------------------------------------------------------------------------------------------------------
int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		printf(msg_help);
		return 1;
	}
	
	uint32_t angle32 = 0;
	
	switch(argv[1][0])
	{
		case 'd':
		{
			double dd = atof(argv[2]);
			angle32 = dd / 360.0 * 0x100000000LL;
		}
		break;
		case 'h':
		{
			angle32 = strtoul(argv[2], 0, 16);
		}
		break;
		default:
			printf(msg_help);
			return 1;
	}
	// далее надо раскомментировать один из двух вариантов аппроксимирующей функции:
	
	// вызываем целочисленные вычисления, если TABLE_TYPE был задан int32_t
	int32_t sine32 = get_sine_int32(sine_approx_table_64_deg_3_acs_3, 6, 3, 3, angle32);

	// вызываем вычисления с плавающей точкой, если TABLE_TYPE был задан float, double, LONG_DOUBLE
	//int32_t sine32 = get_sine_float(sine_approx_table_64_deg_3_acs_3, 6, 3, 3, angle32);
	
	printf("0x%08x * sine32(0x%08x) = %i 0x%08x\n", (uint32_t)AC0, angle32, sine32, sine32);
	
	return 0;
}
