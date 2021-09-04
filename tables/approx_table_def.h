#ifndef APPROX_TYPLE_DEF_H
#define APPROX_TYPLE_DEF_H

#include <stdint.h>

#ifndef TABLE_TYPE
	#define TABLE_TYPE int32_t
#endif

#define LONG_DOUBLE long double


struct t_ApproxTableDef
{
	TABLE_TYPE *p;  // сама таблица
	int pN;         // 2^pN строк в таблице
	int pw;         // степень полинома (pw+1 коэффициентов в строке)
	int S;          // AC shift
};

extern const struct t_ApproxTableDef approx_tables[90];


#endif // APPROX_TYPLE_DEF_H
