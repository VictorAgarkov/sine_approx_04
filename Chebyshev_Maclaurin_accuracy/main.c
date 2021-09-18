/*

��������� �������� ������������� ������� ������
���������� �������� � ������ ���������
�� ������� 0 .. pi/2


*/




#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

//----------------------------------------------------------------------------------------------------------------------------
#define NUMOFARRAY(a)    (sizeof(a) / sizeof(a[0]))
#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795
#endif // M_PI
#define min(a, b) ((a) < (b)) ? (a) : (b)
#define max(a, b) ((a) > (b)) ? (a) : (b)
#define abs(a) (((a) > 0) ? (a) : -(a))

#define D_PI (M_PI * 2.0)     //  2 * pi
#define D_PI2 (D_PI * D_PI)   // (2 * pi)^2


#ifndef DC
	#define DC 0.0
#endif

#define AC_SHIFT 0

#ifndef AC0
	#define AC0 0x00000001LL
#endif // AC0
#define  AC1 (AC0 << AC_SHIFT * 1)
#define  AC2 (AC0 << AC_SHIFT * 2)
#define  AC3 (AC0 << AC_SHIFT * 3)
#define  AC4 (AC0 << AC_SHIFT * 4)
#define  AC5 (AC0 << AC_SHIFT * 5)
#define  AC6 (AC0 << AC_SHIFT * 6)
#define  AC7 (AC0 << AC_SHIFT * 7)
#define  AC8 (AC0 << AC_SHIFT * 8)
#define  AC9 (AC0 << AC_SHIFT * 9)
#define AC10 (AC0 << AC_SHIFT * 10)
#define AC11 (AC0 << AC_SHIFT * 11)
#define AC12 (AC0 << AC_SHIFT * 12)
#define AC13 (AC0 << AC_SHIFT * 13)
#define AC14 (AC0 << AC_SHIFT * 14)
#define AC15 (AC0 << AC_SHIFT * 15)
#define AC16 (AC0 << AC_SHIFT * 16)
#define AC17 (AC0 << AC_SHIFT * 17)
#define AC18 (AC0 << AC_SHIFT * 18)
#define AC19 (AC0 << AC_SHIFT * 19)
#define AC20 (AC0 << AC_SHIFT * 20)
#define AC21 (AC0 << AC_SHIFT * 21)
#define AC22 (AC0 << AC_SHIFT * 22)
#define AC23 (AC0 << AC_SHIFT * 23)
#define AC24 (AC0 << AC_SHIFT * 24)
#define AC25 (AC0 << AC_SHIFT * 25)


#define TABLE_TYPE long double



//----------------------------------------------------------------------------------------------------------------------------
TABLE_TYPE  Maclaurin[25];  // ������������ ���� ��������� ��� ���������� ������

// ������������ ��� ���������� ������ ���������� �������� �� ������� �� 0 �� 90 ��������
// ��������� �� 1 �� 25 (������� � 1 ����)
const TABLE_TYPE Chebyshev[25][26] =
{
	{+1.05449908456459720020297748944 *  AC1 + DC, +0.07358650863471593943886117639 *  AC0 + DC },  // 1
	{-0.83924026907334719956607985147 *  AC2 + DC, +1.86627686902085696538967918290 *  AC1 + DC, -0.01622158605554418011055123946 *  AC0 + DC},  // 2
	{-0.44285751448209377434026522147 *  AC3 + DC, -0.16402811173808974634581898944 *  AC2 + DC, +1.60645815599918215048867720096 *  AC1 + DC, -0.00113088106620447911469735979 *  AC0 + DC	},
	{+0.17481166526513451993536923251 *  AC4 + DC, -0.78903574154176099380783582227 *  AC3 + DC, +0.04926228799785016007975208679 *  AC2 + DC, +1.56474884424658220415105258638 *  AC1 + DC, +0.00012052567744307737828391816 *  AC0 + DC},
	{+0.05512165614418793012160174317 *  AC5 + DC, +0.03610310296658369622096107152 *  AC4 + DC, -0.66666098661154214667819151163 *  AC3 + DC, +0.00508719061986175730871156801 *  AC2 + DC, +1.57035061903752665938421850811 *  AC1 + DC, +0.00000621628624360570427593397 *  AC0 + DC},
	{-0.01447075100922765647188468398 *  AC6 + DC, +0.09833052098959506470026749325 *  AC5 + DC, -0.01222471099524447252888340737 *  AC4 + DC, -0.64178705715966677505844931173 *  AC3 + DC, -0.00068609726908784164505000015 *  AC2 + DC, +1.57083888481974834772786353554 *  AC1 + DC, -0.00000043362029924799510201056 *  AC0 + DC},
	{-0.00325421091641342980995344673 *  AC7 + DC, -0.00304100898377128965241049689 *  AC6 + DC, +0.08255005648763232685107731415 *  AC5 + DC, -0.00141291530337946740048419105 *  AC4 + DC, -0.64559335722887260686176855162 *  AC3 + DC, -0.00004715985226719489351376722 *  AC2 + DC, +1.57079859239268709869292300896 *  AC1 + DC, -0.00000001773045087725463945557 *  AC0 + DC},
	{+0.00064006108814949173738603140 *  AC8 + DC, -0.00580746347213186180575346818 *  AC7 + DC, +0.00109484807795063718336927737 *  AC6 + DC, +0.07906357407785568076067283724 *  AC5 + DC, +0.00021406995857212525177893431 *  AC4 + DC, -0.64600524876547688269074268264 *  AC3 + DC, +0.00000397918429667638797921150 *  AC2 + DC, +1.57079617815098535537569196530 *  AC1 + DC, +0.00000000091685398598535538279 *  AC0 + DC},
	{+0.00011186875007165273238328332 *  AC9 + DC, +0.00013555215808598263268787991 *  AC8 + DC, -0.00485918249765970809684288712 *  AC7 + DC, +0.00013335515979514501688403414 *  AC6 + DC, +0.07963197677547514977895292548 *  AC5 + DC, +0.00001658982703032532516689755 *  AC4 + DC, -0.64596668457816320646448435272 *  AC3 + DC, +0.00000020381879762289270814737 *  AC2 + DC, +1.57079632059135404368102606476 *  AC1 + DC, +0.00000000003104458686184938464 *  AC0 + DC},
	{-0.00001759287586083328401719563 * AC10 + DC, +0.00019967595782481252420457262 *  AC9 + DC, -0.00005066358900848379064031117 *  AC8 + DC, -0.00464060282242459062939247461 *  AC7 + DC, -0.00002163857913984104150352771 *  AC6 + DC, +0.07969997048236244780171659216 *  AC5 + DC, -0.00000155950405298121732828606 *  AC4 + DC, -0.64596390380540452853808108813 *  AC3 + DC, -0.00000001236524197291320488954 *  AC2 + DC, +1.57079632710333710378120680717 *  AC1 + DC, -0.00000000000127398224892666418 *  AC0 + DC},
	{-0.00000251474481161454894722594 * AC11 + DC, -0.00000374118823997868533788131 * AC10 + DC, +0.00016672430355434953089948704 *  AC9 + DC, -0.00000635888935137204089105347 *  AC8 + DC, -0.00467759774449774193783173934 *  AC7 + DC, -0.00000179083570648268381216548 *  AC6 + DC, +0.07969313053749236268775025388 *  AC5 + DC, -0.00000008959297347340045923592 *  AC4 + DC, -0.64596408814723235122844460787 *  AC3 + DC, -0.00000000050372920094933807339 *  AC2 + DC, +1.57079632680549076124802698740 *  AC1 + DC, -0.00000000000003680364214122183 *  AC0 + DC},
	{+0.00000032945858324902412904711 * AC12 + DC, -0.00000448900647375499426097151 * AC11 + DC, +0.00000143407286485279332230641 * AC10 + DC, +0.00015893227296693143753608446 *  AC9 + DC, +0.00000107958881835660154641090 *  AC8 + DC, -0.00468228682361055591591318453 *  AC7 + DC, +0.00000018044659578123320483969 *  AC6 + DC, +0.07969258519798665660517243345 *  AC5 + DC, +0.00000000600357430227555018094 *  AC4 + DC, -0.64596409802950449733629709193 *  AC3 + DC, +0.00000000002372277536062463382 *  AC2 + DC, +1.57079632679447353364472067527 *  AC1 + DC, +0.00000000000000125120010640726 *  AC0 + DC},
	{+0.00000003983777521100612004271 * AC13 + DC, +0.00000007023347310073535647504 * AC12 + DC, -0.00000374286246525779417689803 * AC11 + DC, +0.00000018350436390165373585478 * AC10 + DC, +0.00016028419163244796027844778 *  AC9 + DC, +0.00000009312262678953170369464 *  AC8 + DC, -0.00468179276594301742411467033 *  AC7 + DC, +0.00000001111456497400108308585 *  AC6 + DC, +0.07969262408290527100590810300 *  AC5 + DC, +0.00000000027208641800800461446 *  AC4 + DC, -0.64596409752670553579929554888 *  AC3 + DC, +0.00000000000080149260043298782 *  AC2 + DC, +1.57079632679488420806887538639 *  AC1 + DC, +0.00000000000000003151445543379 *  AC0 + DC},
	{-0.00000000447346085017040642413 * AC14 + DC, +0.00000007112228327288815358862 * AC13 + DC, -0.00000002743006639238719035926 * AC12 + DC, -0.00000356336248428917423601025 * AC11 + DC, -0.00000003219311559721932023963 * AC10 + DC, +0.00016046213849739474301008221 *  AC9 + DC, -0.00000000985421099351857727881 *  AC8 + DC, -0.00468175080275422226763708104 *  AC7 + DC, -0.00000000079723661415397061541 *  AC6 + DC, +0.07969262637705754355054062199 *  AC5 + DC, -0.00000000001403643598317091688 *  AC4 + DC, -0.64596409750533905993639602339 *  AC3 + DC, -0.00000000000003072376411803670 *  AC2 + DC, +1.57079632679489696923686575802 *  AC1 + DC, -0.00000000000000000091380935279 *  AC0 + DC},
	{-0.00000000047166165746966796394 * AC15 + DC, -0.00000000093132315918185983183 * AC14 + DC, +0.00000005915179942874594225338 * AC13 + DC, -0.00000000338379629611308623875 * AC12 + DC, -0.00000359531113159075196956187 * AC11 + DC, -0.00000000263539116734915456652 * AC10 + DC, +0.00016044261477039648862687602 *  AC9 + DC, -0.00000000056646810923725146939 *  AC8 + DC, -0.00468175397261620626715520601 *  AC7 + DC, -0.00000000003329260220994833060 *  AC6 + DC, +0.07969262625087467909358726267 *  AC5 + DC, -0.00000000000043818292658458270 *  AC4 + DC, -0.64596409750622135346933422317 *  AC3 + DC, -0.00000000000000074626426409712 *  AC2 + DC, +1.57079632679489656688943954865 *  AC1 + DC, -0.00000000000000000001741958853 *  AC0 + DC},
	{+0.00000000007477288173425291734 * AC16 + DC, -0.00000000105769600322144233168 * AC15 + DC, +0.00000000114565591511084252452 * AC14 + DC, +0.00000005475007008299030722357 * AC13 + DC, +0.00000000283209629566611747207 * AC12 + DC, -0.00000360148078526601588259387 * AC11 + DC, +0.00000000178951454595629944456 * AC10 + DC, +0.00016044029304720964758448633 *  AC9 + DC, +0.00000000032595024536332989767 *  AC8 + DC, -0.00468175422183829649146600996 *  AC7 + DC, +0.00000000001635501522036232635 *  AC6 + DC, +0.07969262624403316364805850355 *  AC5 + DC, +0.00000000000018314575375765799 *  AC4 + DC, -0.64596409750625575558373794860 *  AC3 + DC, +0.00000000000000026508087366215 *  AC2 + DC, +1.57079632679489655528847630306 *  AC1 + DC, +0.00000000000000000000477485756 *  AC0 + DC},
	{+0.00000000011932126280800041766 * AC17 + DC, -0.00000000094682573523100006403 * AC16 + DC, +0.00000000292938778561535739693 * AC15 + DC, -0.00000000824190382557522774378 * AC14 + DC, +0.00000006962263788223192565627 * AC13 + DC, -0.00000001391016167238134883772 * AC12 + DC, -0.00000358769586820035388162329 * AC11 + DC, -0.00000000662868569190856740883 * AC10 + DC, +0.00016044411891144376569808354 *  AC9 + DC, -0.00000000096094996944780824179 *  AC8 + DC, -0.00468175390600939208341949678 *  AC7 + DC, -0.00000000003883628152110122715 *  AC6 + DC, +0.07969262625064980243061495482 *  AC5 + DC, -0.00000000000033186938831685192 *  AC4 + DC, -0.64596409750623177915742484378 *  AC3 + DC, -0.00000000000000032059479095587 *  AC2 + DC, +1.57079632679489656092632759998 *  AC1 + DC, -0.00000000000000000000453927013 *  AC0 + DC},
	{-0.00000000019481711293138416997 * AC18 + DC, +0.00000000171795110324019821248 * AC17 + DC, -0.00000000688724027464580433094 * AC16 + DC, +0.00000001612430147594627527855 * AC15 + DC, -0.00000002770106405786375366454 * AC14 + DC, +0.00000008960244840757366258336 * AC13 + DC, -0.00000002842656686868291865675 * AC12 + DC, -0.00000358032134721115941186686 * AC11 + DC, -0.00000000909320960753505474952 * AC10 + DC, +0.00016044454091495620893123565 *  AC9 + DC, -0.00000000092198549429669127663 *  AC8 + DC, -0.00468175395022067661361320147 *  AC7 + DC, -0.00000000002638968049803888394 *  AC6 + DC, +0.07969262624872797365918825410 *  AC5 + DC, -0.00000000000015897225088612639 *  AC4 + DC, -0.64596409750624041114144130438 *  AC3 + DC, -0.00000000000000010805860128721 *  AC2 + DC, +1.57079632679489655875792325501 *  AC1 + DC, -0.00000000000000000000101829976 *  AC0 + DC},
	{+0.00000000017473793458384501860 * AC19 + DC, -0.00000000169978266592869619696 * AC18 + DC, +0.00000000763239491137759436833 * AC17 + DC, -0.00000002092926508499904765953 * AC16 + DC, +0.00000003859975900248935813993 * AC15 + DC, -0.00000005332666440982409964629 * AC14 + DC, +0.00000011103963836892453764279 * AC13 + DC, -0.00000004177509379100759792937 * AC12 + DC, -0.00000357411618356157127519630 * AC11 + DC, -0.00000001122854220913364205650 * AC10 + DC, +0.00016044507181056893077688241 *  AC9 + DC, -0.00000000101212595357209560112 *  AC8 + DC, -0.00468175394130437237446965872 *  AC7 + DC, -0.00000000002652718316021311651 *  AC6 + DC, +0.07969262624864077729577730227 *  AC5 + DC, -0.00000000000014780006373245922 *  AC4 + DC, -0.64596409750624099861638846565 *  AC3 + DC, -0.00000000000000009197207633790 *  AC2 + DC, +1.57079632679489655854108282051 *  AC1 + DC, -0.00000000000000000000074796764 *  AC0 + DC},
	{-0.00000000004261754562737639183 * AC20 + DC, +0.00000000031938795137978521978 * AC19 + DC, -0.00000000117739255759118632876 * AC18 + DC, +0.00000000304493072448604391315 * AC17 + DC, -0.00000000638778043976876109353 * AC16 + DC, +0.00000001051250892549331372904 * AC15 + DC, -0.00000001569714978871014654706 * AC14 + DC, +0.00000007401882664181522422287 * AC13 + DC, -0.00000001422982537039243645631 * AC12 + DC, -0.00000358984495760581877864055 * AC11 + DC, -0.00000000430401328277330861030 * AC10 + DC, +0.00016044272991887796656475755 *  AC9 + DC, -0.00000000041053236215493677950 *  AC8 + DC, -0.00468175405644130480492301331 *  AC7 + DC, -0.00000000001056079946519004157 *  AC6 + DC, +0.07969262624709422521124659380 *  AC5 + DC, -0.00000000000004778550331820881 *  AC4 + DC, -0.64596409750624505927878507561 *  AC3 + DC, -0.00000000000000000231904080536 *  AC2 + DC, +1.57079632679489655789056151702 *  AC1 + DC, +0.00000000000000000000024553361 *  AC0 + DC},
	{-0.00000002187345822979634743148 * AC21 + DC, +0.00000022325675027879657135506 * AC20 + DC, -0.00000105630252394443656748717 * AC19 + DC, +0.00000307467163747123618837077 * AC18 + DC, -0.00000616240282244239404157785 * AC17 + DC, +0.00000901914718174113918989177 * AC16 + DC, -0.00000997645404429080038554744 * AC15 + DC, +0.00000851378915715573518387647 * AC14 + DC, -0.00000561822243201949233643767 * AC13 + DC, +0.00000297237895873077107589784 * AC12 + DC, -0.00000482326757743457236642436 * AC11 + DC, +0.00000039529668655565062355014 * AC10 + DC, +0.00016034197798080139801440058 *  AC9 + DC, +0.00000001909764108019223706732 *  AC8 + DC, -0.00468175689963094094706640516 *  AC7 + DC, +0.00000000029230623353885051321 *  AC6 + DC, +0.07969262622451322542420750275 *  AC5 + DC, +0.00000000000105602425961032692 *  AC4 + DC, -0.64596409750627701564992810823 *  AC3 + DC, +0.00000000000000046477760857866 *  AC2 + DC, +1.57079632679489655528847630306 *  AC1 + DC, +0.00000000000000000000274205821 *  AC0 + DC},
	{+0.00000004018358854991159075774 * AC22 + DC, -0.00000044915856290391096205685 * AC21 + DC, +0.00000234025060742530644682389 * AC20 + DC, -0.00000754627461834927124731863 * AC19 + DC, +0.00001686248604239610094411233 * AC18 + DC, -0.00002770677941954439780196254 * AC17 + DC, +0.00003466666694914496919618950 * AC16 + DC, -0.00003375028690725447273460666 * AC15 + DC, +0.00002590348179238967696323729 * AC14 + DC, -0.00001572978608666287383779476 * AC13 + DC, +0.00000765854987539795311265337 * AC12 + DC, -0.00000655157649182834810421330 * AC11 + DC, +0.00000089971218297413682238349 * AC10 + DC, +0.00016022663816556855582745356 *  AC9 + DC, +0.00000003945177208718625229352 *  AC8 + DC, -0.00468175961337670474439719972 *  AC7 + DC, +0.00000000055779331458242311846 *  AC6 + DC, +0.07969262620619815684625117033 *  AC5 + DC, +0.00000000000189936151851408901 *  AC4 + DC, -0.64596409750630089558224786184 *  AC3 + DC, +0.00000000000000082373746458449 *  AC2 + DC, +1.57079632679489655301165174084 *  AC1 + DC, +0.00000000000000000000483848276 *  AC0 + DC},
	{+0.00000037171340410102751142534 * AC23 + DC, -0.00000420373676051911899583652 * AC22 + DC, +0.00002215887596961840489931411 * AC21 + DC, -0.00007230010100829897014722956 * AC20 + DC, +0.00016354249944980287110054882 * AC19 + DC, -0.00027221083033884032801747588 * AC18 + DC, +0.00034537858499734306588574578 * AC17 + DC, -0.00034146448979903293627923992 * AC16 + DC, +0.00026667047191363069692634191 * AC15 + DC, -0.00016579903718720474543736233 * AC14 + DC, +0.00008239950274843556633273440 * AC13 + DC, -0.00003265453049931594749138846 * AC12 + DC, +0.00000670262834842545482052033 * AC11 + DC, -0.00000256679133388382274620939 * AC10 + DC, +0.00016094095021701325465999447 *  AC9 + DC, -0.00000007492472612280999374505 *  AC8 + DC, -0.00468174565396175255088770818 *  AC7 + DC, -0.00000000070643441412506309020 *  AC6 + DC, +0.07969262628798323844231269245 *  AC5 + DC, -0.00000000000167391978540438693 *  AC4 + DC, -0.64596409750620429793916896233 *  AC3 + DC, -0.00000000000000056680976538435 *  AC2 + DC, +1.57079632679489656114316803448 *  AC1 + DC, -0.00000000000000000000267477422 *  AC0 + DC},
	{-0.00000034495388071081951251698 * AC24 + DC, +0.00000403138647945341216028071 * AC23 + DC, -0.00002213877016490978878376638 * AC22 + DC, +0.00007594742080624565110458161 * AC21 + DC, -0.00018249064416866073861808854 * AC20 + DC, +0.00032637828957874477352212359 * AC19 + DC, -0.00045060512410573782746591861 * AC18 + DC, +0.00049149004189894815261890410 * AC17 + DC, -0.00042982115966363580427815151 * AC16 + DC, +0.00030407401211914269223297610 * AC15 + DC, -0.00017478320762649065547009534 * AC14 + DC, +0.00008173645164189330939910371 * AC13 + DC, -0.00003094558110665678784019130 * AC12 + DC, +0.00000584781222030383405203621 * AC11 + DC, -0.00000230118119455654227328595 * AC10 + DC, +0.00016088247123036317978993530 *  AC9 + DC, -0.00000006541699105161830216125 *  AC8 + DC, -0.00468174681611540599987443968 *  AC7 + DC, -0.00000000059883086399724230296 *  AC6 + DC, +0.07969262628049005581356905789 *  AC5 + DC, -0.00000000000129572840242082030 *  AC4 + DC, -0.64596409750621697128678336264 *  AC3 + DC, -0.00000000000000032995368288926 *  AC2 + DC, +1.57079632679489655930002434125 *  AC1 + DC, -0.00000000000000000000103224754 *  AC0 + DC},
	{+0.00000651649348900681956720917 * AC25 + DC, -0.00008044231915292852939522232 * AC24 + DC, +0.00046646750551232914746418244 * AC23 + DC, -0.00168880739396312284663245989 * AC22 + DC, +0.00427952683257056248714315840 * AC21 + DC, -0.00806539333979751716132680552 * AC20 + DC, +0.01172599287630781158828441801 * AC19 + DC, -0.01346320572970278919580037081 * AC18 + DC, +0.01239605420751650631373363791 * AC17 + DC, -0.00924312338823115276418803440 * AC16 + DC, +0.00561357719047274525793407877 * AC15 + DC, -0.00278356186013154180741321594 * AC14 + DC, +0.00112650199099137333965079961 * AC13 + DC, -0.00037085217670617565131272485 * AC12 + DC, +0.00009514604057650112029901545 * AC11 + DC, -0.00002107630917783439222599269 * AC10 + DC, +0.00016400306316455996082409888 *  AC9 + DC, -0.00000046877017304698537848273 *  AC8 + DC, -0.00468170714159059385861305939 *  AC7 + DC, -0.00000000348388388411141371621 *  AC6 + DC, +0.07969262642959705683143919325 *  AC5 + DC, -0.00000000000647737379800225067 *  AC4 + DC, -0.64596409750610542802517696770 *  AC3 + DC, -0.00000000000000162912529423224 *  AC2 + DC, +1.57079632679489656547997672442 *  AC1 + DC, -0.00000000000000000000550359474 *  AC0 + DC},

};
const char roller[] = "|/-\\";


//----------------------------------------------------------------------------------------------------------------------------
void init_Maclaurin(void)
{
	// ��������� ������������ ���� ���������
	Maclaurin[0] = D_PI;
	for(int i = 1; i < NUMOFARRAY(Maclaurin); i++)
	{
		int n2 = i * 2;
		Maclaurin[i] = -D_PI2 / (n2) / (n2+1);
	}
}
//----------------------------------------------------------------------------------------------------------------------------
TABLE_TYPE get_sine_Maclaurin(uint32_t angle, int degree)
{
	// angle - ���� �� 0 �� 2pi (0x00000000 .. 0xffffffff)
	TABLE_TYPE a = (long double)angle / 0x100000000LL;
	TABLE_TYPE aa = a * a;
	TABLE_TYPE res = 1.0;
	for(int i = degree; i; i--)
	{
		res = res * Maclaurin[i] * aa + 1;
	}
	return res * Maclaurin[0] * a;
}
//----------------------------------------------------------------------------------------------------------------------------

TABLE_TYPE get_sine_Chebyshev(const TABLE_TYPE* table, int32_t pN, int pw, int ac_shift, uint32_t angle)
{
	// ��������� ����� ���� angle, ��������� ������� table, ��������� ��
	// 2^pN ����� � ������� ������� pw
	// angle ������� �� ����� ����� (pN ���) � ������� ����� - �������� ������ �������
	// ��������� �������� ��������� ����� ����:
	// ((a[0] * x + a[1]) * x + a[2]) * x + a[3]

	int idx = angle >> (32 - pN); // ������ ������ � ������� (����� �����)
	TABLE_TYPE X = (angle << pN) >> ac_shift; // ������� ����� ���� (����. ����� 0.32)
	X /= 0x100000000LL;
	const TABLE_TYPE* A = table + idx * (pw + 1); // ��������� �� ������ ������ � �������

	TABLE_TYPE sum = A[0];
	for(int i = 1; i <= pw; i++)
	{
		sum *= X;
		sum += A[i];
	}
	return sum;
}


//----------------------------------------------------------------------------------------------------------------------------
void log_out(char *str)
{
	FILE *f = fopen("bits.log", "a");
	if(f)
	{
		fwrite(str, 1, strlen(str), f);
		fclose(f);
	}
}
//----------------------------------------------------------------------------------------------------------------------------

int main()
{
	init_Maclaurin();

//	// ���������, ��� ����� ����������� � �������� ��������
//	for(int i = 0; i <= 18; i++)  // 5 degree step
//	{
//		uint32_t angle = 5.0 / 360.0 * i * 0x100000000LL;
//		long double res = get_sine_Maclaurin(angle, 8);
//		printf("%2i  %lg\n",  i * 5, (double)res);
//	}
//

	long double ln2 = logl(2.0);
	log_out("---------------------------------------------------------------------------\n");
	// ���������� ������ ���� ��� ��������
	for(int deg = 0; deg < 24; deg++)
	{
		TABLE_TYPE err_cheb = 0.0;
		TABLE_TYPE err_macl = 0.0;
		TABLE_TYPE sin_macl;
		long double sin_e;
		char str[1024];

		// ���������� �� ���� ��������� ���� �� 0 �� 89.999 �������� ��� ���������� ���������� ��������
		for(int a = 0; a < 0x40000000; a += 1 << 0)
		{
			TABLE_TYPE sin_cheb = get_sine_Chebyshev(Chebyshev[deg], 2, deg + 1, AC_SHIFT, a);
			sin_e = sinl(D_PI * a / 0x100000000LL);
			err_cheb = max(err_cheb, abs(sin_e - sin_cheb));

			// ������ ������� �������� �� �����
			if((a & 0x003fffff) == 0)
			{
				uint32_t aa = a >> 22;
				char ch = roller[aa & 3];
				aa *= 100;
				printf("%c %5.2f%%\b\b\b\b\b\b\b\b", ch, (double)aa / (1 << 10));
			}
		}
		// ������� ����� ����� ��������� � ����� �����, ��� ������ �����������
		sin_macl = get_sine_Maclaurin(0x3fffffff, deg + 1);
		err_macl = abs(sin_e - sin_macl);

		float bits_cheb = err_cheb ? -logl(err_cheb) / ln2 : 0;
		float bits_macl = err_macl ? -logl(err_macl) / ln2 : 0;

		sprintf(str, "deg = %2i, cheb = %8.5f,   macl = %8.5f\n", deg, bits_cheb, bits_macl);
		printf(str);
		log_out(str);
	}

	return 0;
}
//----------------------------------------------------------------------------------------------------------------------------