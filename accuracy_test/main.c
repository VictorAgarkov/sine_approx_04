#include <stdio.h>


/*
	Проверяем, насколько точно можем вычислить синус

	В опциях компилятора (Project -> Build options... -> Compiler settings -> #defines) можно задать:


TABLE_TYPE=int32_t        - тип таблиц, с которым они будут созданы. Например, int32_t, float, double, LONG_DOUBLE

TABLE_TYPE_NAME=\"INT32\" - строка, которая будет добавлена к именам файлов - лога и сводной таблицы.

AC0=0x40000000LL          - множитель для коэффициента при члене полинома степени 0. Актуально для целочисленного
                            типа таблиц. Для плавающей точки на точность не влияет, можно задать 1.

DC=0.5                    - сколько будет добавлено после умножения коэфф. талицы после умножения на множитель и перед
                            огрублением в целое число. Актуально для целых чисел. Для float лучше задать 0.0

После изменения TABLE_TYPE, AC0 и DC надо сделать Rebuld (F11), ибо таблицы сами пересобираться не хотят.


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
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../tables/approx_table_def.h"

#ifdef __linux__
	#include <time.h>
	#define min(a, b) (a < b) ? a : b
	#define max(a, b) (a > b) ? a : b
#else
	#include <windows.h>
#endif

//-----------------------------------------------------------------------------------------------------------------

#define NUMOFARRAY(a)    (sizeof(a) / sizeof(a[0]))
#ifndef AC0
	#define AC0 0x40000000LL
#endif
//-----------------------------------------------------------------------------------------------------------------

#ifndef TABLE_TYPE
	#define TABLE_TYPE int32_t
#endif

#ifndef TABLE_TYPE_NAME
	#define TABLE_TYPE_NAME "int32_t"
#endif

#define TBL_QTT NUMOFARRAY(approx_tables)

//#define RINT  // разрешение "правильного" округления в get_sine_int32: делает только хуже
#ifdef RINT
	#define SUFF3 "_rint"
#else
	#define SUFF3
#endif // RINT


struct t_MinMax
{
	long double min, max;
	int64_t imin, imax;
};

//-----------------------------------------------------------------------------------------------------------------
const char roller[] = "|/-\\";
char  pivot_table_name[256];
char  log_file_name[256];
char *log_tag = "";
int table_indexes[TBL_QTT];
int table_count = 0;

//-----------------------------------------------------------------------------------------------------------------
void log_out(char *s)
{
	FILE *fo = fopen(log_file_name, "a");
	if(fo)
	{
		fprintf(fo, "%s", s);
		fclose(fo);
	}
}
//-----------------------------------------------------------------------------------------------------------------

int create_pivot_table(int force)
{
	// создаём сводную таблицу, куда будем писать битовую точность
	FILE *f_pivot;
	int exists = 0;
	char s[256];

	f_pivot = fopen(pivot_table_name, "r");
	if(f_pivot)
	{
		exists = 1;
		fclose(f_pivot);
	}

	if(!force && exists)
	{
		sprintf(s, "Pivot table '%s' already exists.\n\n", pivot_table_name);
		log_out(s);
		return 0;
	}

	f_pivot = fopen(pivot_table_name, "wb");
	if(!f_pivot)
	{
		printf("Can not create '%s' for output.\n\n", pivot_table_name);
		return 4;
	}
	else
	{
		// размечаем файл
		// на каждую ячейку отводим 12 символов
		// ячейки разделяем символами табуляции \t
		// в каждой строке 6 + 1 ячейка
		// строки - символом новой строки \n
		int c_num = 7;
		int l_num = 16 - 2 + 1;
		char cell_delimiter = '\t';

		// первая строка - шапка таблицы
		for(int c = 1; c <= c_num; c++)
		{
			char ss[16];
			if(c == 1) strncpy(ss, "\"Tbl size\"", sizeof(ss) - 1);
			else sprintf(ss, "%i", c - 1);
			fprintf(f_pivot, "%11s%c", ss, (c == c_num) ? '\n' : cell_delimiter);
		}

		// следующие строки - записи
		for(int l = 0; l < l_num; l++)
		{
			for(int c = 1; c <= c_num; c++)
			{
				char ss[16];
				if   (c == 1) sprintf(ss, "\"%i\"",1 << (l + 2));
				else ss[0] = 0;
				fprintf(f_pivot, "%11s%c", ss, (c == c_num) ? '\n' : cell_delimiter);
			}
		}
		fclose(f_pivot);

		sprintf(s, "Pivot table '%s' created\n\n", pivot_table_name);
		log_out(s);
	}
	return 0;

}
//-----------------------------------------------------------------------------------------------------------------
int add_table_index(int idx)
{
	if(idx >= TBL_QTT)
	{
		printf("Wrong table index: %i\nValid index from 0 to %i\n", idx, (int)TBL_QTT - 1);
		return 1;
	}

	// проверка на уже существующий индекс
	for(int i = 0; i < table_count; i++)
	{
		if(table_indexes[i] == idx) continue;
	}

	// добавляем индекс в список
	if(table_count < TBL_QTT)
	{
		table_indexes[table_count++] = idx;
	}
	return 0;
}
//-----------------------------------------------------------------------------------------------------------------
int32_t get_sine_int32(const int32_t* table, int32_t pN, int pw, int ac_shift, uint32_t angle)
{
	// вычисляем синус угла angle, используя таблицу table, состоящей из
	// 2^pN строк и полином степени pw
	// angle делится на целую часть (pN бит) и дробную часть - смещение внутри отрезка
	// Вычисляем значение степенным рядом типа:
	// ((a[0] * x + a[1]) * x + a[2]) * x + a[3]

	int idx = angle >> (32 - pN); // индекс строки в таблице (целая часть)
	uint32_t X = (angle << pN) >> ac_shift; // дробная часть угла (фикс. точка 0.32)
	const int32_t* A = table + idx * (pw + 1); // указатель на нужную строку в таблице

	int64_t sum = A[0];
	for(int i = 1; i <= pw; i++)
	{
		sum *= X;

		#ifdef RINT
			sum += 0x80000000L;  // add 0.5 before round
		#endif // RINT

		sum >>= 32;
		sum += A[i];
	}
	return sum;
}
//-----------------------------------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------------------------
uint32_t get_ms(void)
{

	#ifdef __linux__
		// linux
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return  ts.tv_nsec / 1000000 + ts.tv_sec * 1000;
	#else
		// windows
		return GetTickCount();
	#endif
}
//-----------------------------------------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
	uint32_t start_time = get_ms();
	int ret = 0;

	char s[1024];

	sprintf(pivot_table_name, "./result/bit_accuracy_" TABLE_TYPE_NAME "_0x%08x" SUFF3 ".csv", (uint32_t)AC0);
	sprintf(log_file_name,    "./result/bit_accuracy_" TABLE_TYPE_NAME "_0x%08x" SUFF3 ".log", (uint32_t)AC0);

	// шерстим аргументы
	for(int i = 1; i < argc; i++)
	{
		char *ca = argv[i];
		char c = ca[0];
		if(c == 't' || c == 'T')
		{
			// создаём сводную таблицу (t - если не существует, T - принудительн)
			int ret = create_pivot_table(c == 'T');
			if(ret) return ret;
		}
		else if(c == 'a')
		{
			// заполняем список всеми индексами
			if(ca[1] == '-') for(int i = TBL_QTT; i > 0; i--) add_table_index(i - 1); // в обратном порядке
			else             for(int i = 0; i < TBL_QTT; i++) add_table_index(i);     // в прямом порядке
		}
		else if(c == 'l')
		{
			// тег (строка) для логирования
			log_tag = ca + 1;
		}
		else if(c >= '0' && c <= '9')
		{
			// цифры - используем их в качестве индексов таблиц, которые будем проверять
			char *minus = strchr(ca, '-');
			if(minus)
			{
				// диапазон индексов
				char s[8];
				strncpy(s, ca, min(7, minus - ca));
				int strt = atoi(s);
				int stop = atoi(minus + 1);
				if(strt < 0 || stop < 0 || strt >= TBL_QTT || stop >= TBL_QTT)
				{
					printf("Index out of range 0 - %i.\n",  (int)TBL_QTT - 1);
					ret = 5;
					goto wrong_argument;
				}
				if(strt < stop) for(int i = strt; i <= stop; i++) add_table_index(i);
				else            for(int i = strt; i >= stop; i--) add_table_index(i);
			}
			else
			{
				// один индекс
				ret = add_table_index(atoi(ca));
				if(ret) goto wrong_argument;
			}
		}
		else
		{
			wrong_argument:
			printf("Wrong argument: %s\n\n", ca);
			return ret;
		}
	}

	if(!table_count)
	{
		printf("No indices specified\n\n");
		return 0;
	}

	// начинаем проверку
	sprintf(s, "%s start check %i table(s): ", log_tag, table_count);
	log_out(s);
	s[0] = 0;
	for(int i = 0; i < table_count; i++)
	{
		sprintf(s + strlen(s), "%i ", table_indexes[i]);
	}
	log_out(s);
	log_out("\n\n");

	long double kk = M_PI * 2 / 0x100000000LL;

	const struct t_ApproxTableDef* t89 = approx_tables + 89; // "эталонная" таблица для проверки

	// перебираем заданные таблицы
	for(int ti = 0; ti < table_count; ti++)
	{
		const struct t_ApproxTableDef* tbl = approx_tables + table_indexes[ti];
		struct t_MinMax mm;
		mm.max = mm.min = 0.0;
		mm.imax = mm.imin = -1;
		uint32_t start_curr = get_ms();

		// проходимся по всем возможным аргументам (углам) - всего 2^32 значений
		for(uint32_t angle = 0;;)
		{
			// иногда выводим прогресс на экран
			if((angle & 0x003fffff) == 0)
			{
				uint32_t aa = angle >> 22;
				char ch = roller[aa & 3];
				aa *= 100;
				printf("%c %5.2f%%\b\b\b\b\b\b\b\b", ch, (double)aa / (1 << 10));
			}

			// вычисляем значение синуса аппроксимацией - нужно выбрать: float или int32_t
			TABLE_TYPE vi = get_sine_int32(tbl->p, tbl->pN, tbl->pw, tbl->S, angle);  // 32bit integer
			//TABLE_TYPE vi = get_sine_float(tbl->p, tbl->pN, tbl->pw, tbl->S, angle);  // any float

			// эталон: вычисляем синус стандартными методами
			long double vf = sinl(angle * kk) * AC0;
			// либо используем "самую-самую" таблицу (нужно выбрать: float или int32_t)
			//long double vf = get_sine_float(t89->p, t89->pN, t89->pw, t89->S, angle);
			//long double vf = get_sine_int32(t89->p, t89->pN, t89->pw, t89->S, angle);

			// получаем величину ошибки
			long double err = vf - vi;
			err /= AC0; // приводим ошибку к диапазону -1 ... +1

			// обновляем минимальную и максимальную ошибки
			if(mm.min > err)
			{
				mm.min = err;
				mm.imin = angle;
			}

			if(mm.max < err)
			{
				mm.max = err;
				mm.imax = angle;
			}

			// следующий угол
			if(!(angle += 1 << 0)) break;  // 0 - for full test, 10 - for fast (debug)
		}
		uint32_t stop_curr = get_ms();
		float dt = stop_curr - start_curr;
		// выводим результат
		char table_name[32];
		sprintf(table_name, "%5i_p^%i", 1 << tbl->pN, tbl->pw); // like a " 2048_p^2"
		long double max_err = max(mm.max, -mm.min);
		float actual_bits = (max_err == 0) ? 0 : -logl(max_err) / logl(2.0);

		sprintf
		(
			s, "%s %s: err = {%+Le @ 0x%08x ... %+Le @ 0x%08x}, actual bits = %8f, dt = %.3f\n",
			log_tag, table_name, mm.min, (int)mm.imin, mm.max, (int)mm.imax, actual_bits, dt / 1000.0
		);
		printf("\r%2i: %s", ti + 1, s);
		log_out(s);

		// пишем в ячейку сводной таблицы
		FILE *f_pivot = fopen(pivot_table_name, "r+b");
		if(f_pivot)
		{
			int offs = ((tbl->pN - 1) * 7 + tbl->pw + 0) * 12;
			fseek(f_pivot, offs, SEEK_SET);
			char ss[16];
			sprintf(ss, "%11f", actual_bits);
			// заменяем точку на запятую - для openoffice
			char *cp = strchr(ss, '.');
			if(cp) *cp = ',';
			fwrite(ss, 1, strlen(ss), f_pivot);
			fclose(f_pivot);
		}
	};

	// окончили проверки - считаем время выполнения
	uint32_t stop_time = get_ms();
	sprintf(s, "\n%s --------------------     complete in %.3f sec  -------------------------------\n\n\n", log_tag, (float)(stop_time - start_time) / 1000);
	log_out(s);
	return ret;
}
