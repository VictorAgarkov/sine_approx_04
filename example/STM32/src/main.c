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

******************************************************************************************************************/

#include "stm32f10x.h"

#define LED_PIN 12

#ifndef TABLE_TYPE
	#define TABLE_TYPE int32_t
#endif

#define NUMOFARRAY(a)    (sizeof(a) / sizeof(a[0]))

//	Это софт можно запустить как для проверки правильности вычисления
//	по углам из test_angles, так и для определения быстродействия.
//	В зависимости от преследуемой цели, следует раскомментировать
//	один из дефайнов FOR_ANGLE_1000 ниже:

#define FOR_ANGLE_1000  for(angle = 1000; angle; angle--)  // для проверки быстродействия в цикле
//#define FOR_ANGLE_1000                                     // для проверки правильности

extern TABLE_TYPE  sine_approx_table_1024_deg_1_acs_5 [2 * 1024];
extern TABLE_TYPE   sine_approx_table_256_deg_2_acs_5 [3 * 256];
extern TABLE_TYPE    sine_approx_table_64_deg_3_acs_3 [4 * 64];
extern TABLE_TYPE    sine_approx_table_32_deg_4_acs_2 [5 * 32];
extern TABLE_TYPE    sine_approx_table_16_deg_5_acs_1 [6 * 16];
extern TABLE_TYPE    sine_approx_table_16_deg_6_acs_1 [7 * 16];

int main(void)
{
	// init LED pin on GPIOB[12]
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->BSRR = 1 << LED_PIN;  // LED off
	GPIOB->CRH = (GPIOB->CRH & 0xfff0ffff) | (0x3 << ((LED_PIN-8)*4));


	uint32_t test_angles[] =
	{
		//   проверочные углы
		//   angle value              // sine value
		  0.0 / 360 * 0x100000000ll,  // 0x00000000
		 30.0 / 360 * 0x100000000ll,  // 0x20000000
		 45.0 / 360 * 0x100000000ll,  // 0x2D413CCC
		 60.0 / 360 * 0x100000000ll,  // 0x376CF5D0
		 90.0 / 360 * 0x100000000ll,  // 0x40000000
		120.0 / 360 * 0x100000000ll,  // 0x376CF5D0
		135.0 / 360 * 0x100000000ll,  // 0x2D413CCC
		150.0 / 360 * 0x100000000ll,  // 0x20000000
		180.0 / 360 * 0x100000000ll,  // 0x00000000
		210.0 / 360 * 0x100000000ll,  // 0xe0000000
		225.0 / 360 * 0x100000000ll,  // 0xD2BEC334
		240.0 / 360 * 0x100000000ll,  // 0xC8930A30
		270.0 / 360 * 0x100000000ll,  // 0xc0000000
		300.0 / 360 * 0x100000000ll,  // 0xC8930A30
		315.0 / 360 * 0x100000000ll,  // 0xD2BEC334
		330.0 / 360 * 0x100000000ll,  // 0xe0000000
	};

  while(1)
  {
		volatile int32_t  sine_val[6];
		register uint32_t angle;

  	for(int cnt = 0; cnt < NUMOFARRAY(test_angles); cnt++)
		{
			angle = test_angles[cnt];

			/*********************************************************************************************

			       intervals = 1024
			          degree = 1
			        ac_shift = 5
			        accuracy = 18.7 bit
			       non const : 24 clocks/cycle
			           const : 35 clocks/cycle

			*********************************************************************************************/

			FOR_ANGLE_1000
			{
				asm
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
					"ldr   r3, [r0], #4             \n"  // r3      = T0

					"smull r3, r2, r3, r1           \n"  // {r2:r3} = T0 * X
					"ldr   r3, [r0]                 \n"  // r3 = T1
					"add   %[out], r3, r2           \n"  // out     = T0 * X + T1

					: [out]  "=r" (sine_val[0])
					: [in]    "r" (angle),
						[table] "r" (sine_approx_table_1024_deg_1_acs_5)
					: "r0", "r1", "r2", "r3", "memory"  // clobber
				);
			}

			GPIOB->BSRR = 1 << LED_PIN;  // LED off

			/*********************************************************************************************

			       intervals = 256
			          degree = 2
			        ac_shift = 5
			        accuracy = 23.6 bit
			       non const : 32 clocks/cycle
			           const : 43 clocks/cycle

			*********************************************************************************************/

			FOR_ANGLE_1000
			{
				asm
				(
					".equ ipw,  8                   \n"  // intervals = 2 ^ ipw
					".equ acs,  5                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 5 MSB of angle
					"movs r1, 3 * 4                 \n"  // 3 cells per table string
					"mul  r0, r0, r1                \n"  // get offset in table (mul by 12 = 3 *4)
					"add  r0, %[table]              \n"  // add table start address
					"mov  r1, %[in], lsl ipw        \n"  // X (32-5 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get (T0 * X + T1) * X + T2
					"ldr   r3, [r0], #4             \n"  // r3      =  T0

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =  T0 * X
					"ldr   r3, [r0], #4             \n"  // r3 = T1
					"add   r3, r2                   \n"  // r3      =  T0 * X + T1

					"smull r3, r2, r3, r1           \n"  // {r2:r3} = (T0 * X + T1) * X
					"ldr   r3, [r0]                 \n"  // r3 = T2
					"add   %[out], r3, r2           \n"  // out     = (T0 * X + T1) * X + T2


					: [out]  "=r" (sine_val[1])
					: [in]    "r" (angle),
						[table] "r" (sine_approx_table_256_deg_2_acs_5)
					: "r0", "r1", "r2", "r3", "memory"  // clobber
				);
			}

			GPIOB->BRR = 1 << LED_PIN;  // LED on

			/*********************************************************************************************

			       intervals = 64
			          degree = 3
			        ac_shift = 3
			        accuracy = 24.9 bit
			       non const : 37 clocks/cycle
			           const : 53 clocks/cycle

			      Время вычисления функций синуса в циклах:

			      +-----------+----------------+----------------+
			      |    QTT    |      const     |    no const    |
			      |           |     (flash)    |     (RAM)      |
			      +-----------+----------------+----------------+
			      |    60M    |     ~ 41s      |       ~ 31 s   |
			      |   1000    |     680 us     |       500 us   |
			      |     10    |     6.8 us     |      5.06 us   |
			      |      1    |    0.71 us     |     0.556 us   |
			      |      0    |     ~28 ns     |       ~28 ns   |
			      +-----------+----------------+----------------+
			      |  кол-во   |                |                |
			      | тактов на |       53       |       37       |
			      |  1 цикл   |                |                |
			      +-----------+----------------+----------------+


			*********************************************************************************************/

			FOR_ANGLE_1000
			{
				asm
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
					"ldr   r3, [r0], #4             \n"  // r3      =   T0

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =   T0 * X
					"ldr   r3, [r0], #4             \n"  // r3 = T1
					"add   r3, r2                   \n"  // r3      =   T0 * X + T1

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =  (T0 * X + T1) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T2
					"add   r3, r2                   \n"  // r3      =  (T0 * X + T1) * X + T2

					"smull r3, r2, r3, r1           \n"  // {r2:r3} = ((T0 * X + T1) * X + T2) * X
					"ldr   r3, [r0]                 \n"  // r3 = T3
					"add   %[out], r3, r2           \n"  // out     = ((T0 * X + T1) * X + T2) * X + T3

					: [out]  "=r" (sine_val[2])
					: [in]    "r" (angle),
						[table] "r" (sine_approx_table_64_deg_3_acs_3)
					: "r0", "r1", "r2", "r3", "memory"  // clobber
				);
			}

			GPIOB->BSRR = 1 << LED_PIN;  //LED off

			/*********************************************************************************************

			       intervals = 32
			          degree = 4
			        ac_shift = 2
			        accuracy = 27.3 bit
			       non const : 45 clocks/cycle
			           const : 65 clocks/cycle

			*********************************************************************************************/

			FOR_ANGLE_1000
			{
				asm
				(
					".equ ipw,  5                   \n"  // intervals = 2 ^ ipw
					".equ acs,  2                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 5 MSB of angle
					"movs r1, 5 * 4                 \n"  // 5 cells per table string
					"mul  r0, r0, r1                \n"  // get offset in table (mul by 20 = 5 *4)
					"add  r0, %[table]              \n"  // add table start address
					"mov  r1, %[in], lsl ipw        \n"  // X (32-5 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get (((T0 * X + T1) * X + T2) * X + T3) * X + T4
					"ldr   r3, [r0], #4             \n"  // r3 =         T0

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =    T0 * X
					"ldr   r3, [r0], #4             \n"  // r3 = T1
					"add   r3, r2                   \n"  // r3      =    T0 * X + T1

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =   (T0 * X + T1) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T2
					"add   r3, r2                   \n"  // r3      =   (T0 * X + T1) * X + T2

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =  ((T0 * X + T1) * X + T2) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T3
					"add   r3, r2                   \n"  // r3      =  ((T0 * X + T1) * X + T2) * X + T3

					"smull r3, r2, r3, r1           \n"  // {r2:r3} = (((T0 * X + T1) * X + T2) * X + T3) * X
					"ldr   r3, [r0]                 \n"  // r3 = T4
					"add   %[out], r3, r2           \n"  // out     = (((T0 * X + T1) * X + T2) * X + T3) * X + T4

					: [out]  "=r" (sine_val[3])
					: [in]    "r" (angle),
						[table] "r" (sine_approx_table_32_deg_4_acs_2)
					: "r0", "r1", "r2", "r3", "memory"  // clobber
				);
			}

			GPIOB->BRR = 1 << LED_PIN;  //LED on

			/*********************************************************************************************

			       intervals = 16
			          degree = 5
			        ac_shift = 1
			        accuracy = 28.0 bit
			       non const : 51 clocks/cycle
			           const : 74 clocks/cycle

			*********************************************************************************************/

			FOR_ANGLE_1000
			{
				asm
				(
					".equ ipw,  4                   \n"  // intervals = 2 ^ ipw
					".equ acs,  1                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 5 MSB of angle
					"movs r1, 6 * 4                 \n"  // 6 cells per table string
					"mul  r0, r0, r1                \n"  // get offset in table (mul by 24 = 6 *4)
					"add  r0, %[table]              \n"  // add table start address
					"mov  r1, %[in], lsl ipw        \n"  // X (32-5 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get ((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X + T5
					"ldr   r3, [r0], #4             \n"  // r3 =          T0

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =     T0 * X
					"ldr   r3, [r0], #4             \n"  // r3 = T1
					"add   r3, r2                   \n"  // r3      =     T0 * X + T1

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =    (T0 * X + T1) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T2
					"add   r3, r2                   \n"  // r3      =    (T0 * X + T1) * X + T2

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =   ((T0 * X + T1) * X + T2) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T3
					"add   r3, r2                   \n"  // r3      =   ((T0 * X + T1) * X + T2) * X + T3

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =  (((T0 * X + T1) * X + T2) * X + T3) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T4
					"add   r3, r2                   \n"  // r3      =  (((T0 * X + T1) * X + T2) * X + T3) * X + T4

					"smull r3, r2, r3, r1           \n"  // {r2:r3} = ((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X
					"ldr   r3, [r0]                 \n"  // r3 = T5
					"add   %[out], r3, r2           \n"  // out     = ((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X + T5

					: [out]  "=r" (sine_val[4])
					: [in]    "r" (angle),
						[table] "r" (sine_approx_table_16_deg_5_acs_1)
					: "r0", "r1", "r2", "r3", "memory"  // clobber
				);
			}

			GPIOB->BSRR = 1 << LED_PIN;  //LED off

			/*********************************************************************************************

			       intervals = 16
			          degree = 6
			        ac_shift = 1
			        accuracy = 28.8 bit
			       non const : 58 clocks/cycle
			           const : 85 clocks/cycle

			*********************************************************************************************/

			FOR_ANGLE_1000
			{
				asm
				(
					".equ ipw,  4                   \n"  // intervals = 2 ^ ipw
					".equ acs,  1                   \n"  // AC shift
					// prepare
					"mov  r0, %[in], lsr (32-ipw)   \n"  // index in table - 5 MSB of angle
					"movs r1, 7 * 4                 \n"  // 7 cells per table string
					"mul  r0, r0, r1                \n"  // get offset in table (mul by 28 = 7 *4)
					"add  r0, %[table]              \n"  // add table start address
					"mov  r1, %[in], lsl ipw        \n"  // X (32-5 LSB of angle)
					"lsr  r1, r1, acs               \n"  // r1 = X (apply AC shift value)

					// get (((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X + T5) * X + T6
					"ldr   r3, [r0], #4             \n"  // r3 = T0

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =      T0 * X
					"ldr   r3, [r0], #4             \n"  // r3 = T1
					"add   r3, r2                   \n"  // r3      =      T0 * X + T1

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =     (T0 * X + T1) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T2
					"add   r3, r2                   \n"  // r3      =     (T0 * X + T1) * X + T2

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =    ((T0 * X + T1) * X + T2) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T3
					"add   r3, r2                   \n"  // r3      =    ((T0 * X + T1) * X + T2) * X + T3

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =   (((T0 * X + T1) * X + T2) * X + T3) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T4
					"add   r3, r2                   \n"  // r3      =   (((T0 * X + T1) * X + T2) * X + T3) * X + T4

					"smull r3, r2, r3, r1           \n"  // {r2:r3} =  ((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X
					"ldr   r3, [r0], #4             \n"  // r3 = T5
					"add   r3, r2                   \n"  // r3      =  ((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X + T5

					"smull r3, r2, r3, r1           \n"  // {r2:r3} = (((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X + T5) * X
					"ldr   r3, [r0]                 \n"  // r3 = T5
					"add   %[out], r3, r2           \n"  // out     = (((((T0 * X + T1) * X + T2) * X + T3) * X + T4) * X + T5) * X + T6

					: [out]  "=r" (sine_val[5])
					: [in]    "r" (angle),
						[table] "r" (sine_approx_table_16_deg_6_acs_1)
					: "r0", "r1", "r2", "r3", "memory"  // clobber
				);
			}
			GPIOB->BRR = 1 << LED_PIN;  // LED on
		}
  }
}
