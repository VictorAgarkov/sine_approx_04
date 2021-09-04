/*

	Создаём таблицы для кусочной апроксимации функции синуса
	функцией N-порядка типа:

	y = a[0] + a[1] * x + a[2] * x^2 + a[3] * x^3 + .... {1}
	где a[i] - значения из строки таблицы,
	    x - дробная часть угла в диапазоне от 0.0 до 0.999

	Например, есть таблица, в которой содержится K=128 строк. Таким образом, весть интервал от
	0 до 2*пи разбит на 128 одинаковых отрезков. И если мы хотм вычислить синус для угла 33 градуса,
	то:
		1) приводим угол к основанию 128:  33 / 360 * 128 = 11.7333333333333
		2) коэффициенты a[i] берём строку с индексом 11 (целая часть угла) из таблицы (индекс - в диапазоне 0..127)
		3) x = 0.733333 (дробная часть угла)
		4) производим вычисления по формуле {1}


	Коэффициенты таблицы получаются приближением функции синуса полиномами Чебышева первого рода.

	Запуск программы:
	sin_approx_03 <256> <3> [0x40000000LL] [2] [0.5]> table1.c
	256 - на сколько интервалов разбит полный круг
	3 - степень апроксимации (кубическая функция, 4 коэфф. в строке)
	0x40000000LL - коэфф. (AC0) для члена при степени 0.
	2 - на сколько бит будет "весомее" коэфф. для следующего члена полинома
	0.5 - значение DC (постоянной состовляющей, прибавляемой для корректного округления в целое число)

	compile it with -D__USE_MINGW_ANSI_STDIO for valid print

---------------------------------------------------------------------------------------------------------------------------	
                                               The BSD License
                         Copyright (c) 2021, Victor Agarkov <victoragarkov@gmail.com>. 
                                              All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
   disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
   disclaimer in the documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software must display the following acknowledgement: 
   This product includes software developed by the Victor Agarkov.
4. Neither the name of the Victor Agarkov nor the names of its contributors may be used to endorse or promote products 
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Victor Agarkov AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Victor 
Agarkov BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
	
---------------------------------------------------------------------------------------------------------------------------	

*/

//-----------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "gaussian.h"

//-----------------------------------------------------------------------------------------------------------------

#define min(a, b) ((a) < (b)) ? (a) : (b)
#define max(a, b) ((a) > (b)) ? (a) : (b)
#define abs(a) ((a) > 0) ? (a) : (-(a))

#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795
#endif // M_PI

#define OUT_EN
//-----------------------------------------------------------------------------------------------------------------
const char help[] =
{
	"\n"
	"Making table (C language) for SINE aproximation with power series, like\n"
	"y = (((a0 * x) + a1) * x + a2) * x + a3\n"
	"where:\n"
	"a0..a3 - values of row of table with integer index = angle / N\n"
	"x - fraction of angle\n"
	"degree PW = 3\n"
	"\n"
	"Usage\n"
	"make_table.exe <N> <PW> [AC0] [AC_shift] [DC]\n"
	"where:\n"
	"N  - more than 1\n"
	"PW - more than 0\n"
	"PW - more than 0\n"
	"AC0 - multiply factor for degree 0 member, default 0x40000000LL\n"
	"AC_shift - multiply by 2^AC_shift for next degree members (AC1, AC2 etc), default 0\n"
	"DC - add to each table member for correct floor float to integer, default 0.5\n"
	"Example:\n"
	"sin_approx.exe 256 4 0x7f000000LL 2 > sine_approx_table_256_i_deg_4_acs_2.c\n"
	"\n"

};

const char out_header1[] =
{
	"#include <stdint.h>\n"
	"\n"
	"#ifndef TABLE_TYPE\n"
	"	#define TABLE_TYPE int32_t\n"
	"#endif\n"
	"\n"
//    "\n"
//    "//define Q1_ONLY\n\n"
	"#define LONG_DOUBLE long double\n"
	"\n"
	"#define LINE_SIZE %i\n"
	"\n"
	"#ifdef Q1_ONLY\n"
	"	#define LINE_NUM %i\n"
	"#else\n"
	"	#define LINE_NUM %i\n"
	"#endif\n"
	"\n"
	"#ifndef DC\n"
	"	#define DC %s\n"
	"#endif\n"
	"\n"
	"#define AC_SHIFT %i\n"
	"\n"
	"#ifndef AC0\n"
	"\t#define AC0 %s\n"
	"#endif // AC0\n"
};

const char out_header2[] =
{
	"\n"
	"\n"
	"TABLE_TYPE sine_approx_table_%i_deg_%i_acs_%i [LINE_SIZE * LINE_NUM] =\n"
	"{"
};


const char out_tail[] =
{
	"\n\t#endif // !Q1_ONLY"
	"\n};\n\n"
};

//-----------------------------------------------------------------------------------------------------------------
void get_Chebyshev_solutions(T* dst, int n,  T scale, T offset)
{
	// Получаем корни полинома Чебышева порядка n
	// решаем уравнение cos(n * arccos(x)) = 0

	if((n & 1) != 0) dst[n / 2] = offset;  // центральный корень при нечётном порядке

	for(int i = 0; i < n/2; i++)
	{
		T val = cosl(M_PI * (0.5 + i) / n);
		dst[i]         = offset - val * scale;
		dst[n - i - 1] = offset + val * scale;
	}
}
//-----------------------------------------------------------------------------------------------------------------

int main(int argc, char ** argv)
{

	int N, pw;
	int ret = 0;
	int ac_shift = 0;
	char *ac0_str = "0x40000000LL";
	char *dc_str = "0.5";
	if(argc < 3)
	{
		printf(help);
		return 1;
	}

	// проверки
	N = atoi(argv[1]);   // кол-во отрезков на 1 период
	pw = atoi(argv[2]);  // степень полинома
	if(N < 1 || pw < 1)
	{
		printf(help);
		return 2;
	}
	pw++;

	if(argc > 3)
	{
		ac0_str = argv[3];
		if(argc > 4)
		{
			ac_shift = atoi(argv[4]);
			if(argc > 5)
			{
				dc_str = argv[5];
			}
		}
	}

	T cheb[pw];            // корни полинома Чебышева
	T sol [pw];            // результат решения системы уравнений
	T buff[pw * (pw + 1)]; // коэфф. системы уравнений
	T abs_max_v[pw];       // абсолютные максимумы коэфф., для вычисления запаса ACx в битах
	int abs_max_i[pw];

	memset(abs_max_v, 0,    sizeof(abs_max_v));
	memset(abs_max_i, 0xff, sizeof(abs_max_i));

	// подготавливаем некоторые значения
	get_Chebyshev_solutions(cheb, pw, 0.5, 0.5); // находим корни полинома Чебышева заданного порядка
	T xpN = M_PI * 2.0 / N; // размер одного участка по Х, в радианах
	int quadrant_mask = N / 4 - 1;

	// выодим заголовок
	#ifdef OUT_EN
		printf(out_header1, pw, N/4, N, dc_str, ac_shift, ac0_str);

		for(int i = 1; i < pw; i++)
		{
			printf("#define AC%i (AC0 << AC_SHIFT * %i)\n", i, i);
		}

		printf(out_header2, N, pw - 1, ac_shift);
	#endif // OUT_EN

	//считаем таблицы
	int cnt = N * pw;
	for(int n = 0; n < N; n++) // текущий отрезок апроксимации
	{
		// находим коэффициенты апроксимации полиномом Чебышева
		for(int i = 0; i < pw; i++) // row scan
		{
			T x = cheb[i];
			T xx = 1.0;
			T *pRow = buff + i * (pw + 1);
			for(int j = 0; j < pw; j++) // collumn scan
			{
				// заполняем коэффициенты уравнения
				pRow[j] = xx;
				xx *= x;
			}
			// вычисляем значение уравнения (справа от '=')
			pRow[pw] = sinl((x + (T)n) * xpN);
		}
		// находим решение системы уравнений
		matrix_gaussian(buff, sol, pw);

		// печатаем результат
		#ifdef OUT_EN
			if(N > 3 && (n & quadrant_mask) == 0)
			{
				// номер квадранта, если необходимо
				int q = n * 4 / N;  //
				printf("\n\t// Q %i", q + 1);
				if(q == 1) printf("\n\t#ifndef Q1_ONLY");
			}
			printf("\n\t");
		#endif
		for(int i = 0; i < pw; i++) // row scan
		{
		#ifdef OUT_EN
			int ii = pw - 1 - i;
			//printf("%+1.59Lf * AC%i + DC", sol[ii], ii);
			printf("%+1.29Lf * AC%i + DC", sol[ii], ii);
			printf((--cnt) ? ", " :"  ");
		#endif
			T abs_sol = abs(sol[i]);
			if(abs_sol > abs_max_v[i])
			{
				abs_max_v[i] = abs_sol;
				abs_max_i[i] = n;
			}
		}
		#ifdef OUT_EN
			printf("  // %i", n);
		#endif // OUT_EN
	}
	#ifdef OUT_EN
		printf(out_tail);
		// печатаем запас по битам для каждого коэфф.
		printf("// Bit's reserve for K at worst line:\n");
	#endif
	int warn_cnt = 0;
	int worst_deg = 0;
	int worst_line = 0;
	T   worst_val = 111;

	for(int i = 0; i < pw; i++)
	{
		int idx = pw - 1 - i;
		int ssi = ac_shift * idx;
		T reserve_bits = -logl(abs_max_v[idx]) / logl(2.0);
		T reserve_bits_ssi = reserve_bits - ssi;
		char *warn = "";
		int line_max_i = abs_max_i[idx];
		if(reserve_bits_ssi < 0)
		{
			warn = "  ***  Warning: no bits reserve!  ***";
			warn_cnt++;
		}
		// get worst case
		if(reserve_bits_ssi < worst_val)
		{
			worst_val = reserve_bits_ssi;
			worst_line = line_max_i;
			worst_deg = idx;
		}
		#ifdef OUT_EN
			printf("// ^%02i = %5.2f - %2i = %+.2f  @ line %i%s\n", idx, (float)reserve_bits, ssi, (float)reserve_bits_ssi,  line_max_i, warn);
		#endif // OUT_EN
	}
	#ifdef OUT_EN
		if(warn_cnt)
		{
			printf("\n// Total %i warning(s)\n", warn_cnt);
		}
		printf("\n// Worst reserve: %+.2f on degree ^%02i @ line %i\n", (float)worst_val, worst_deg, worst_line);
	#else
		printf("%i\t%i\t%i\t%.6f\n", N, pw - 1, ac_shift, (float)worst_val);
	#endif

	return ret;

}
