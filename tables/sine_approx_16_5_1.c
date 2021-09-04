#include <stdint.h>

#ifndef TABLE_TYPE
	#define TABLE_TYPE int32_t
#endif

#define LONG_DOUBLE long double

#define LINE_SIZE 6

#ifdef Q1_ONLY
	#define LINE_NUM 4
#else
	#define LINE_NUM 16
#endif

#ifndef DC
	#define DC 0.5
#endif

#define AC_SHIFT 1

#ifndef AC0
	#define AC0 0x40000000LL
#endif // AC0
#define AC1 (AC0 << AC_SHIFT * 1)
#define AC2 (AC0 << AC_SHIFT * 2)
#define AC3 (AC0 << AC_SHIFT * 3)
#define AC4 (AC0 << AC_SHIFT * 4)
#define AC5 (AC0 << AC_SHIFT * 5)


TABLE_TYPE sine_approx_table_16_deg_5_acs_1 [LINE_SIZE * LINE_NUM] =
{
	// Q 1
	+0.00007622441345448825545350152 * AC5 + DC, +0.00000238093137108367626348031 * AC4 + DC, -0.01009456427768245247077942388 * AC3 + DC, +0.00000033955342769603918763400 * AC2 + DC, +0.39269905188118203140943199281 * AC1 + DC, +0.00000000041604712502454880583 * AC0 + DC,   // 0
	+0.00006461993748207724369217577 * AC5 + DC, +0.00038790238616762768955572667 * AC4 + DC, -0.00932952623388755318220734837 * AC3 + DC, -0.02950619614392419475017376521 * AC2 + DC, +0.36280654899147716103665131659 * AC1 + DC, +0.38268343368655481867655204775 * AC0 + DC,   // 1
	+0.00004317766180921660754476683 * AC5 + DC, +0.00071436921901402522260293804 * AC4 + DC, -0.00714415239334917479308186125 * AC3 + DC, -0.05452068095269773236079811386 * AC2 + DC, +0.27768003786737632259556431136 * AC1 + DC, +0.70710678321224942357269005466 * AC0 + DC,   // 2
	+0.00001516197853236040609762390 * AC5 + DC, +0.00093207981403868866687602342 * AC4 + DC, -0.00387114611282614694715742854 * AC3 + DC, -0.07123488631762658484532614936 * AC2 + DC, +0.15027925815377892281693959775 * AC1 + DC, +0.92387953493283076465431707947 * AC0 + DC,   // 3
	// Q 2
	#ifndef Q1_ONLY
	-0.00001516197853235969462882569 * AC5 + DC, +0.00100788970670048654212738769 * AC4 + DC, -0.00000879292865220663153960148 * AC3 + DC, -0.07710422598654929316836165523 * AC2 + DC, -0.00000017632886382028676119613 * AC1 + DC, +1.00000000244872800476362423305 * AC0 + DC,   // 4
	-0.00004317766180921727575119182 * AC5 + DC, +0.00093025752806010894464667553 * AC4 + DC, +0.00385489889920090464508509404 * AC3 + DC, -0.07123514620056893983598717696 * AC2 + DC, -0.15027958396703547422790670646 * AC1 + DC, +0.92387953461440212784292272397 * AC0 + DC,   // 5
	-0.00006461993748208836733250325 * AC5 + DC, +0.00071100207357804357162657906 * AC4 + DC, +0.00713171731439623924768116277 * AC3 + DC, -0.05452116115376030857858440613 * AC2 + DC, -0.27768028723404697741651905862 * AC1 + DC, +0.70710678262387002341237893233 * AC0 + DC,   // 6
	-0.00007622441345450182247286192 * AC5 + DC, +0.00038350299864356061015714365 * AC4 + DC, +0.00932279641765319948184522966 * AC3 + DC, -0.02950682355684826932803359670 * AC2 + DC, -0.36280668394774682659030540233 * AC1 + DC, +0.38268343291780008511297934970 * AC0 + DC,   // 7
	// Q 3
	-0.00007622441345446921095000391 * AC5 + DC, -0.00000238093137113042017372123 * AC4 + DC, +0.01009456427768249025014293731 * AC3 + DC, -0.00000033955342771584768176281 * AC2 + DC, -0.39269905188118203146364210143 * AC1 + DC, -0.00000000041604700234616493096 * AC0 + DC,   // 8
	-0.00006461993748205177170415105 * AC5 + DC, -0.00038790238616769308611084798 * AC4 + DC, +0.00932952623388761050092688907 * AC3 + DC, +0.02950619614392416775862186800 * AC2 + DC, -0.36280654899147717832967596774 * AC1 + DC, -0.38268343368655470537742502302 * AC0 + DC,   // 9
	-0.00004317766180915392521738950 * AC5 + DC, -0.00071436921901417849517350740 * AC4 + DC, +0.00714415239334930216947295288 * AC3 + DC, +0.05452068095269768735624356035 * AC2 + DC, -0.27768003786737635428137280225 * AC1 + DC, -0.70710678321224933678230614720 * AC0 + DC,   // 10
	-0.00001516197853234525362531932 * AC5 + DC, -0.00093207981403872539184233620 * AC4 + DC, +0.00387114611282617778085077449 * AC3 + DC, +0.07123488631762657258028907312 * AC2 + DC, -0.15027925815377896668647000195 * AC1 + DC, -0.92387953493283071776257311947 * AC0 + DC,   // 11
	// Q 4
	+0.00001516197853233036071898224 * AC5 + DC, -0.00100788970670041562874080180 * AC4 + DC, +0.00000879292865214931236676581 * AC3 + DC, +0.07710422598654931204703198364 * AC2 + DC, +0.00000017632886377017824232165 * AC1 + DC, -1.00000000244872800465520401580 * AC0 + DC,   // 12
	+0.00004317766180914459895450631 * AC5 + DC, -0.00093025752805992989088135103 * AC4 + DC, -0.00385489889920105805545733600 * AC3 + DC, +0.07123514620056899811185394805 * AC2 + DC, +0.15027958396703542298580152936 * AC1 + DC, -0.92387953461440217457203635809 * AC0 + DC,   // 13
	+0.00006461993748197793095184711 * AC5 + DC, -0.00071100207357776585424594852 * AC4 + DC, -0.00713171731439648300766975656 * AC3 + DC, +0.05452116115376040305325121109 * AC2 + DC, +0.27768028723404693258475922635 * AC1 + DC, -0.70710678262387010982329207942 * AC0 + DC,   // 14
	+0.00007622441345438400130824704 * AC5 + DC, -0.00038350299864326660840928347 * AC4 + DC, -0.00932279641765345569914737872 * AC3 + DC, +0.02950682355684836926945104323 * AC2 + DC, +0.36280668394774679704579620210 * AC1 + DC, -0.38268343291780019811395077700 * AC0 + DC    // 15
	#endif // !Q1_ONLY
};

// Bit's reserve for K at worst line:
// ^05 = 13.68 -  5 = +8.68  @ line 7
// ^04 =  9.95 -  4 = +5.95  @ line 4
// ^03 =  6.63 -  3 = +3.63  @ line 8
// ^02 =  3.70 -  2 = +1.70  @ line 12
// ^01 =  1.35 -  1 = +0.35  @ line 8
// ^00 = -0.00 -  0 = -0.00  @ line 4  ***  Warning: no bits reserve!  ***

// Total 1 warning(s)

// Worst reserve: -0.00 on degree ^00 @ line 4
