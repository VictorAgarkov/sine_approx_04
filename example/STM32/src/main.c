/******************************************************************************************************************

											Пример вычисления фугкции синуса при помощи кусочной
           аппроксимации функцией 3-го порядка на ядре ARM Cortex-M3 (STM32F103C8T)

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

Результаты измерения быстродействия в зависимости от:
1. Степень полинома;
2. Размещения таблиц в RAM или FLASH (тип таблиц TABLE_TYPE="const "uint32_t" / "const uint32_t" в свойствах проекта);
3. Очистки R2 перед MAC командами (один из двух макросов CLR_R2, слегка влияет на точность).

Время, необходимое для вычисления, призводилось посредством DWT Cortex-M3 (макросы BENCHMARK_START и BENCHMARK_STOP)

  +------------+-----------+-----------+
  | Размещение |    RAM    |   FLASH   |
  +------------+-----+-----+-----+-----+
  |  CLR_R2    |  N  |  Y  |  N  |  Y  |
  +------------+-----+-----+-----+-----+
  |  Степень   |        Кол-во         |
  |  полинома  |        тактов         |
  +------------+-----+-----+-----+-----+
  |     1      |  17 |  18 |  27 |  28 |
  |     2      |  25 |  24 |  34 |  35 |
  |     3      |  25 |  30 |  37 |  43 |
  |     4      |  30 |  34 |  43 |  45 |
  |     5      |  37 |  41 |  52 |  54 |
  |     6      |  40 |  44 |  55 |  63 |
  +------------+-----+-----+-----+-----+

******************************************************************************************************************/

#include "stm32f10x.h"

#define LED_PIN 12

#ifndef TABLE_TYPE
	#define TABLE_TYPE int32_t
#endif

#define NUMOFARRAY(a)    (sizeof(a) / sizeof(a[0]))

typedef struct
{
	uint32_t a, v;
}
tst_angle;

//	Это софт можно запустить как для проверки правильности вычисления
//	по углам из test_angles, так и для определения быстродействия.
//	В зависимости от преследуемой цели, следует раскомментировать
//	один из дефайнов FOR_ANGLE_1000 ниже:

//#define FOR_ANGLE_1000  for(angle = 1000; angle; angle--)  // для проверки быстродействия в цикле
#define FOR_ANGLE_1000                                     // для проверки правильности

#define BENCHMARK_START *DWT_CYCCNT = -2                  // clear DWT cycle counter
//#define BENCHMARK_START
#define BENCHMARK_STOP(i) bench_result[i] = *DWT_CYCCNT
//#define BENCHMARK_STOP(i)

// если можно пожетвовать точностью (1-2 младших бита) в угоду быстродействию,
// то регистр R2 перед умножением-с-накоплением можно не чистить
//#define CLR_R2  "movs  r2,     0                \n"  // r2 = 0 for best accuracy
#define CLR_R2                                       // not clear R2 for best performance

extern TABLE_TYPE  sine_approx_table_1024_deg_1_acs_5 [2 * 1024];
extern TABLE_TYPE   sine_approx_table_256_deg_2_acs_5 [3 * 256];
extern TABLE_TYPE    sine_approx_table_64_deg_3_acs_3 [4 * 64];
extern TABLE_TYPE    sine_approx_table_32_deg_4_acs_2 [5 * 32];
extern TABLE_TYPE    sine_approx_table_16_deg_5_acs_1 [6 * 16];
extern TABLE_TYPE    sine_approx_table_16_deg_6_acs_1 [7 * 16];

int main(void)
{
	// prepare to benchmark
	volatile uint32_t *DWT_CONTROL = (uint32_t *) 0xE0001000;
	volatile uint32_t *DWT_CYCCNT  = (uint32_t *) 0xE0001004;
	volatile uint32_t *DEMCR       = (uint32_t *) 0xE000EDFC;
	volatile uint32_t *LAR         = (uint32_t *) 0xE0001FB0;   // <-- added lock access register

	*DEMCR = *DEMCR | 0x01000000;     // enable trace
	*LAR = 0xC5ACCE55;                // <-- added unlock access to DWT (ITM, etc.)registers
	*DWT_CONTROL = *DWT_CONTROL | 1;  // enable DWT cycle counter
	//*DWT_CYCCNT = 0;                  // clear DWT cycle counter

	// init LED pin on GPIOB[12]
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->BSRR = 1 << LED_PIN;  // LED off
	GPIOB->CRH = (GPIOB->CRH & 0xfff0ffff) | (0x3 << ((LED_PIN-8)*4));


	tst_angle test_angles[] =
	{
		//   проверочные углы
		//   angle value                // sine value

		{  0.0 / 360 * 0x100000000ll,   0x00000000},
		{ 30.0 / 360 * 0x100000000ll,   0x20000000},
		{ 45.0 / 360 * 0x100000000ll,   0x2D413CCC},
		{ 60.0 / 360 * 0x100000000ll,   0x376CF5D0},
		{ 90.0 / 360 * 0x100000000ll,   0x40000000},
		{120.0 / 360 * 0x100000000ll,   0x376CF5D0},
		{135.0 / 360 * 0x100000000ll,   0x2D413CCC},
		{150.0 / 360 * 0x100000000ll,   0x20000000},
		{180.0 / 360 * 0x100000000ll,   0x00000000},
		{210.0 / 360 * 0x100000000ll,   0xe0000000},
		{225.0 / 360 * 0x100000000ll,   0xD2BEC334},
		{240.0 / 360 * 0x100000000ll,   0xC8930A30},
		{270.0 / 360 * 0x100000000ll,   0xc0000000},
		{300.0 / 360 * 0x100000000ll,   0xC8930A30},
		{315.0 / 360 * 0x100000000ll,   0xD2BEC334},
		{330.0 / 360 * 0x100000000ll,   0xe0000000},
	};

  while(1)
  {
		volatile int32_t  sine_val[6], sine_err[6];
		volatile int bench_result[6];
		register uint32_t angle;

		for(int cnt = 0; cnt < NUMOFARRAY(test_angles); cnt++)
		{
			angle = test_angles[cnt].a;

			/*********************************************************************************************

			       intervals = 1024
			          degree = 1
			        ac_shift = 5
			        accuracy = 18.7 bit
			       non const : 17 clocks
			           const : 27 clocks

			*********************************************************************************************/
			BENCHMARK_START;
			FOR_ANGLE_1000
			{
				asm volatile
				(
					".equ ipw,  10                  \n"  // intervals = 2 ^ ipw
					".equ acs,  5                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 5 MSB of angle
					"lsl  r0, r0, #3                \n"  // get offset in table (mul by 8 = 2 *4)
					"add  r0, %[table]              \n"  // r0 = ptr to table line (add table start address)
					"mov  r1, %[in], lsl ipw        \n"  // X (32-10 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get T0 * X + T1
					"ldm   r0,     {r3, %[out]}     \n"  // r3 = T0, out = T1
					CLR_R2
					"smlal r2, %[out], r1, r3       \n"  // T1:0 += T0 * X

					: [out]  "=r" (sine_val[0])
					: [in]    "r" (angle),
					  [table] "r" (sine_approx_table_1024_deg_1_acs_5)
					: "r0", "r1", "r2", "r3"  // clobber
				);
			}
			BENCHMARK_STOP(0);

//			GPIOB->BSRR = 1 << LED_PIN;  // LED off

			/*********************************************************************************************

			       intervals = 256
			          degree = 2
			        ac_shift = 5
			        accuracy = 23.6 bit
			       non const : 25 clocks
			           const : 34 clocks

			*********************************************************************************************/
			BENCHMARK_START;
			FOR_ANGLE_1000
			{
				asm volatile
				(
					".equ ipw,  8                   \n"  // intervals = 2 ^ ipw
					".equ acs,  5                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 8 MSB of angle
					"movs r1, 3 * 4                 \n"  // 3 cells per table string

					"mla  r0, r0, r1, %[table]      \n"  // add offset (idx * 7 *4) to table

					"mov  r1, %[in], lsl ipw        \n"  // X (32-8 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get (T0 * X + T1) * X + T2
					"ldm   r0,     {r3-r4, %[out]}  \n"  // r3  = T0, r4 = T1, [out] = T2
					CLR_R2
					"smlal r2, r4, r1, r3           \n"  // T1:0 += T0 * X
					CLR_R2
					"smlal r2, %[out], r1, r4       \n"  // T2:0 += ((T0 * X) + T1) * X

					: [out]  "=r" (sine_val[1])
					: [in]    "r" (angle),
						[table] "r" (sine_approx_table_256_deg_2_acs_5)
					: "r0", "r1", "r2", "r3",  "r4"  // clobber
				);
			}
			BENCHMARK_STOP(1);

//			GPIOB->BRR = 1 << LED_PIN;  // LED on

			/*********************************************************************************************

			       intervals = 64
			          degree = 3
			        ac_shift = 3
			        accuracy = 24.9 bit
			       non const : 25 clocks
			           const : 37 clocks

			*********************************************************************************************/

			BENCHMARK_START;
			FOR_ANGLE_1000
			{
				asm volatile
				(
					".equ ipw,  6                   \n"  // intervals = 2 ^ ipw
					".equ acs,  3                   \n"  // AC shift
					// prepare
					"mov r0, %[in], lsr (32-ipw)    \n"  // index in table - 6 MSB of angle
					"mov r0, r0, lsl 4              \n"  // get offset in table (mul by 16 = 4 *4)
					"add r0, %[table]               \n"  // add table start address
					"mov r1, %[in], lsl ipw         \n"  // X (32-6 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get ((T0 * X + T1) * X + T2) * X + T3
					"ldm   r0,     {r3-r5, %[out]}  \n"  // r3  = T0, r4 = T1, r5 = T2, [out] = T3
					CLR_R2
					"smlal r2, r4, r1, r3           \n"  // T1:0 += T0 * X
					CLR_R2
					"smlal r2, r5, r1, r4           \n"  //
					CLR_R2
					"smlal r2, %[out], r1, r5       \n"  //

					: [out]  "=r" (sine_val[2])
					: [in]    "r" (angle),
						[table] "r" (sine_approx_table_64_deg_3_acs_3)
					: "r0", "r1", "r2", "r3", "r4", "r5"  // clobber
				);
			}
			BENCHMARK_STOP(2);

//			GPIOB->BSRR = 1 << LED_PIN;  //LED off

			/*********************************************************************************************

			       intervals = 32
			          degree = 4
			        ac_shift = 2
			        accuracy = 27.3 bit
			       non const : 30 clocks
			           const : 43 clocks

			*********************************************************************************************/

			BENCHMARK_START;
			FOR_ANGLE_1000
			{
				asm volatile
				(
					".equ ipw,  5                   \n"  // intervals = 2 ^ ipw
					".equ acs,  2                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 5 MSB of angle
					"movs r1, 5 * 4                 \n"  // 5 cells per table string

					"mla  r0, r0, r1, %[table]      \n"  // add offset (28 = 7 *4) to table

					"mov  r1, %[in], lsl ipw        \n"  // X (32-5 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get (((T0 * X + T1) * X + T2) * X + T3) * X + T4
					"ldm   r0,     {r3-r6, %[out]}  \n"  // r3  = T0, r4 = T1, r5 = T2, r6 = T3, [out] = T4
					CLR_R2
					"smlal r2, r4, r1, r3           \n"  // T1:0 += T0 * X
					CLR_R2
					"smlal r2, r5, r1, r4           \n"  //
					CLR_R2
					"smlal r2, r6, r1, r5           \n"  //
					CLR_R2
					"smlal r2, %[out], r1, r6       \n"  //

					: [out]  "=r" (sine_val[3])
					: [in]    "r" (angle),
					  [table] "r" (sine_approx_table_32_deg_4_acs_2)
					: "r0", "r1", "r2", "r3", "r4", "r5", "r6"  // clobber
				);
			}
			BENCHMARK_STOP(3);

//			GPIOB->BRR = 1 << LED_PIN;  //LED on

			/*********************************************************************************************

			       intervals = 16
			          degree = 5
			        ac_shift = 1
			        accuracy = 28.0 bit
			       non const : 37 clocks
			           const : 52 clocks

			*********************************************************************************************/

			BENCHMARK_START;
			FOR_ANGLE_1000
			{
				asm volatile
				(
					".equ ipw,  4                   \n"  // intervals = 2 ^ ipw
					".equ acs,  1                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 4 MSB of angle
					"movs r1, 6 * 4                 \n"  // 6 cells per table string

					"mla  r0, r0, r1, %[table]      \n"  // add offset (28 = 7 *4) to table

					"mov  r1, %[in], lsl ipw        \n"  // X (32-4 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get ((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X + T5
					"ldm   r0,     {r3-r6, r8, %[out]}  \n"  // r3  = T0, r4 = T1, r5 = T2, r6 = T3, r8 = T4, [out] = T5
					CLR_R2
					"smlal r2, r4, r1, r3           \n"  // T1:0 += T0 * X
					CLR_R2
					"smlal r2, r5, r1, r4           \n"  //
					CLR_R2
					"smlal r2, r6, r1, r5           \n"  //
					CLR_R2
					"smlal r2, r8, r1, r6           \n"  //
					CLR_R2
					"smlal r2, %[out], r1, r8       \n"  //

					: [out]  "=r" (sine_val[4])
					: [in]    "r" (angle),
					  [table] "r" (sine_approx_table_16_deg_5_acs_1)
					: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r8"  // clobber
				);
			}
			BENCHMARK_STOP(4);

//			GPIOB->BSRR = 1 << LED_PIN;  //LED off

			/*********************************************************************************************

			       intervals = 16
			          degree = 6
			        ac_shift = 1
			        accuracy = 28.8 bit
			       non const : 40 clocks
			           const : 55 clocks

			*********************************************************************************************/

			BENCHMARK_START;
			FOR_ANGLE_1000
			{
				asm volatile
				(
					".equ ipw,  4                   \n"  // intervals = 2 ^ ipw
					".equ acs,  1                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 4 MSB of angle
					"movs r1, 7 * 4                 \n"  // 7 cells per table string

					"mla  r0, r0, r1, %[table]      \n"  // add offset (28 = 7 *4) to table

					"mov  r1, %[in], lsl ipw        \n"  // X (32-4 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get (((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X + T5) * X + T6
					"ldm   r0,     {r3-r6, r8-r9, %[out]}  \n"  // r3  = T0, r4 = T1, r5 = T2, r6 = T3, r8 = T4, r9 = T5, [out] = T6
					CLR_R2
					"smlal r2, r4, r1, r3           \n"  // T1:0 += T0 * X
					CLR_R2
					"smlal r2, r5, r1, r4           \n"  //
					CLR_R2
					"smlal r2, r6, r1, r5           \n"  //
					CLR_R2
					"smlal r2, r8, r1, r6           \n"  //
					CLR_R2
					"smlal r2, r9, r1, r8           \n"  //
					CLR_R2
					"smlal r2, %[out], r1, r9       \n"  //

					: [out]  "=r" (sine_val[5])
					: [in]    "r" (angle),
					  [table] "r" (sine_approx_table_16_deg_6_acs_1)
					: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r8", "r9"  // clobber
				);
			}
			BENCHMARK_STOP(5);
//			GPIOB->BRR = 1 << LED_PIN;  // LED on



			// вычисляем ошибку
			for(int i = 0; i < NUMOFARRAY(sine_err); i++)
			{
				sine_err[i] = sine_val[i] - test_angles[cnt].v;
			}
		}

  }
}
