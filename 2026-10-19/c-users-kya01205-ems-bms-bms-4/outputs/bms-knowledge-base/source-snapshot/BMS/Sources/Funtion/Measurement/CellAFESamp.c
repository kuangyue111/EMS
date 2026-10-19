/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CellAFESamp.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 单体电压采样模块
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "DEVGPIO.h"
#include "osif.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "DiagInterface.h"
#include "DiagCheck.h"
#include "BalanTactic.h"
#include "LTC6813-1.h"
#include "BF8915A.h"
#include "CellAFESamp.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGBmuGenPara_102[];                                      /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];                                   /*主控高级参数值*/
extern u8 gGHardPara_104[];                                         /*主控硬件参数值*/

static u8 sVSampFinFlag = 0;                                        /*电压采样完成标志*/
static u8 sTSampFinFlag = 0;                                        /*温度采样完成标志*/
static u8 sRSampFinFlag = 0;                                        /*内阻采样完成标志*/

#if(0 == BMU_BSU_COM_TYPE)                                          /*菊花链单体采样方式*/
#if(1 == AFE_TYPE_CHOICE)
static u16 sLTC6813CellVoltAD[MAX_LTC6813_NUM][18] = {0};           /*6813单体电压通道电压采样AD值*/
static u16 sLTC6813AUXGPIOAD[MAX_LTC6813_NUM][12] = {0};            /*6813辅助通道温度采样值,辅助通道与电压采样共用AD0模块*/
static u8 sLTC6813CfgData[6] = {0};                                 /*LTC67813配置寄存器的值*/
#else
extern BOOLEAN	isospi_reverse;									    /*BF8915A通信端选择(0:从A口读 1:从B口读)*/
static u16 sBydTempStandV = 3290;									/*BF8915A温度采样上拉电压基准值(3.3V)*/
CellVoltage_TypeDef BF8915ACellVoltage[MAX_LTC6813_NUM];            /*BF8915A CELL电池电压采集信息*/
GPIOReg_TypeDef BF8915AGpioVoltage[MAX_LTC6813_NUM];                /*BF8915A GPIO采集信息*/
#endif

static u8 sLTC6813PasBalanReqState = 0;                             /*6813被动均衡请求状态 0:无请求 1:有请求*/
static u8 sLTC6813PasBalanCtrlState = 0;                            /*6813被动均衡控制状态 0:未控制 1:已控制*/
static u32 sLTC6813PasBalanCmd[MAX_LTC6813_NUM] = {0};              /*6813被动均衡命令,按bit位表示,每bit对应1节电池实际通道(0关闭均衡 1:开启被动均衡)*/

//static u16 voltage_cell[MAX_LTC6813_NUM][MAXVOLTNUM] = {0};       /*单体电池电压值*/

/*根据项目选择单体温感对应温度表*/
#if(1 == PRJ_PACK_NTC_INFO)
/*亿纬电池项目*/
static const u32 sNTCTempTabR[] =                                   //亿纬电池NTC(10K)温度阻值取值表[CEL_T_TAB_LEN],[-41~125]
{
	221500,//-41℃
	204700, 193400, 182700, 172700, 163300, 154400, 146100, 138400, 131000, 124100,
	117600, 111500, 105800, 100400, 95250, 90440, 85900, 81610, 77570, 73750,
	70150, 66740, 63520, 60480,57600, 54870,52290, 49850, 47540, 45350,
	43270, 41300, 39440, 37670, 35990, 34390, 32880, 31440, 30070, 28770,
	27530, 26360, 25240, 24180, 23160, 22200, 21280, 20410, 19570, 18780,
	18020, 17300, 16610, 15950, 15320, 14720, 14150, 13600, 13080, 12580,
	12100, 11640, 11200, 10780, 10380, 10000, 9633, 9281, 8944, 8621,
	8311, 8015, 7730, 7457, 7195, 6944, 6703, 6471, 6249, 6035,
	5830, 5633, 5443, 5261, 5086, 4918, 4756, 4600, 4450, 4305,
	4166, 4033, 3904, 3780, 3660, 3545, 3434, 3327, 3224, 3124,
	3028, 2935, 2846, 2760, 2677, 2596, 2519, 2444, 2371, 2301,
	2234, 2169, 2106, 2045, 1986, 1929, 1874, 1820, 1769, 1719,
	1671, 1624, 1579, 1535, 1492, 1451, 1412, 1373, 1336, 1300,
	1265, 1231, 1198, 1166, 1135, 1105, 1076, 1048, 1020, 994,
	968, 943, 919, 895, 872, 850, 828, 808, 787, 768,
	748, 730, 712, 694, 677, 660, 644, 629, 613, 598,
	584, 570, 556, 543, 530, 518
};
#elif(2 == PRJ_PACK_NTC_INFO)
/*CATL-PACK14S-240AH项目/CATL液冷PACK52S-270AH项目/CATL-PACK14S-280AH项目*/
static const u32 sNTCTempTabR[] =                                   //CATL电池NTC(10K)温度阻值取值表[CEL_T_TAB_LEN],[-41~125]
{
	356000,//-41℃
	333562, 312315, 292550, 274155, 257027, 241071, 226202, 212340, 199410, 187344,
	176081, 165562, 155735, 146549, 137960, 129925, 122406, 115366, 108774, 102597,
	96807, 91378, 86286, 81507, 77021, 72808, 68851, 65131, 61635, 58346,
	55252, 52340, 49599, 47017, 44584, 42292, 40130, 38092, 36168, 34353,
	32639, 31021, 29492, 28047, 26682, 25390, 24168, 23012, 21918, 20882,
	19901, 18972, 18091, 17256, 16464, 15713, 15000, 14324, 13682, 13072,
	12493, 11943, 11419, 10922, 10449, 10000, 9572, 9164, 8776, 8407,
	8055, 7720, 7401, 7097, 6806, 6530, 6265, 6013, 5773, 5543,
	5324, 5115, 4915, 4723, 4541, 4366, 4100, 4039, 3886, 3740,
	3600, 3466, 3338, 3215, 3097, 2984, 2876, 2772, 2673, 2578,
	2486, 2398, 2314, 2233, 2156, 2081, 2010, 1941, 1875, 1812,
	1751, 1692, 1636, 1581, 1529, 1479, 1431, 1384, 1340, 1297,
	1255, 1215, 1177, 1140, 1104, 1070, 1036, 1004, 974, 944,
	915, 887, 861, 835, 810, 786, 763, 740, 719, 698,
	678, 658, 639, 621, 603, 586, 570, 554, 538, 523,
	509, 495, 482, 468, 456, 443, 432, 420, 409, 398,
	388, 377, 368, 358, 349, 340
};
#elif(3 == PRJ_PACK_NTC_INFO)
/*CATL240AH电池旧百城改造项目*/
static const u32 sNTCTempTabR[] =                                   //CATL电池NTC(100K)温度阻值取值表[CEL_T_TAB_LEN],[-41~125]
{
	4422000,//-41℃
    4123445, 3846898, 3590500, 3352675, 3131979, 2927087, 2736784, 2559953, 2395568, 2242688,
    2100446, 1968136, 1844753, 1729894, 1622847, 1523039, 1429942, 1343069, 1261972,1186235,
    1115477, 1049344, 987508, 929669, 875547, 824883, 777438, 732992, 691338, 652287,
    615662, 581301, 549051, 518771, 490332, 463611, 438497, 414883, 392673, 371776,
    352107, 333588, 316146, 299713, 284225, 269624, 255852, 242861, 230600, 219026,
    208096, 197771, 188016, 178794, 170076, 161829, 154028, 146645, 139655, 133037,
    126768, 120828, 115199, 109862, 104800, 100000, 95444, 91121, 87016, 83118,
    79415, 75897, 72553, 69375, 66352, 63477, 60741, 58138, 55660, 53300,
    51053, 48912, 46872, 44928, 43074, 41307, 39621, 38012, 36477, 35012,
    33614, 32278, 31002, 29783, 28618, 27505, 26441, 25423, 24449, 23518,
    22627, 21774, 20957, 20175, 19426, 18709, 18022, 17363, 16732, 16127,
    15546, 14990, 14456, 13943, 13452, 12980, 12527, 12092, 11674, 11272,
    10887, 10516, 10160, 9817, 9488, 9172, 8867, 8574, 8292, 8021,
    7760, 7508, 7266, 7033, 6808, 6592, 6384, 6183, 5989, 5803,
    5623, 5449, 5282, 5120, 4964, 4814
};
#else
/*其他默认标准PACK280AH电池*/
static const u32 sNTCTempTabR[] =                                   //自研电池NTC(10K)温度阻值取值表[CEL_T_TAB_LEN],[-41~125]
{
	204050,//-41℃
	193160, 182910, 173260, 164160, 155590, 147510, 139890, 132700, 125920, 119520,
	113480, 107780, 102400, 97312, 92507, 87964, 83670, 79609, 75767, 72132,
	68691, 65433, 62348, 59425, 56656, 54031, 51542, 49181, 46942, 44817,
	42800, 40885, 39066, 37339, 35697, 34136, 32652, 31241, 29899, 28621,
	27405, 26248, 25145, 24095, 23095, 22141, 21232, 20365, 19538, 18750,
	17997, 17279, 16593, 15938, 15312, 14714, 14143, 13597, 13075, 12575,
	12098, 11641, 11203, 10785, 10384, 10000, 9632, 9280, 8943, 8619,
	8309, 8012, 7727, 7453, 7191, 6939, 6697, 6465, 6242, 6028,
	5822, 5625, 5435, 5253, 5077, 4908, 4746, 4590, 4440, 4296,
	4157, 4023, 3894, 3770, 3650, 3535, 3424, 3317, 3214, 3115,
	3019, 2927, 2838, 2752, 2669, 2589, 2511, 2437, 2365, 2295,
	2228, 2163, 2101, 2040, 1981, 1925, 1870, 1817, 1766, 1716,
	1669, 1622, 1577, 1534, 1492, 1451, 1412, 1374, 1337, 1301,
	1266, 1233, 1200, 1169, 1138, 1109, 1080, 1052, 1025, 999,
	973, 949, 925, 902, 879, 857, 836, 816, 796, 776,
	758, 739, 722, 705, 688, 672, 656, 641, 626, 612,
	598, 585, 572, 559, 547, 535
};
#endif

#if(1 == AFE_TYPE_CHOICE)
/*均衡板温温度传感器,阻值47K电阻(ADI用3950)*/
static const u32 sNTCBalTTabR[] =                                   //板温NTC(47K)温度阻值取值表[BAL_T_TAB_LEN],[-41~125]
{
	2154500,//-41℃
	1888700, 1756900, 1635300, 1523000, 1419300, 1323400, 1234700, 1152700, 1076700, 1006300,
	941000, 880400, 824200, 771900, 723400, 678300, 636300, 597200, 560800, 526900,
	495310, 465810, 438290, 412580, 388570, 366120, 345130, 325490, 307100, 289890,
	273750, 258630, 244450, 231140, 218650, 206920, 195900, 185540, 175800, 166640,
	158020, 149900, 142250, 135040, 128250, 121850, 115800, 110100, 104710, 99630,
	94820, 90280, 85980, 81920, 78070, 74430, 70990, 67720, 64630, 61700,
	58920, 56280, 53770, 51400, 49140, 47000, 44964, 43029, 41189, 39439,
	37775, 36190, 34683, 33247, 31879, 30576, 29334, 28150, 27021, 25944,
	24917, 23936, 23000, 22106, 21252, 20436, 19657, 18911, 18198, 17517,
	16864, 16240, 15643, 15071, 14523, 13999, 13496, 13014, 12552, 12110,
	11685, 11278, 10887, 10512, 10152, 9806, 9474, 9155, 8849, 8554,
	8271, 7999, 7738, 7486, 7244, 7011, 6787, 6571, 6363, 6163,
	5971, 5785, 5606, 5434, 5268, 5107, 4953, 4804, 4660, 4522,
	4388, 4259, 4134, 4014, 3898, 3785, 3677, 3572, 3471, 3373,
	3278, 3187, 3099, 3013, 2930, 2850, 2773, 2698, 2625, 2555,
	2487, 2421, 2357, 2296, 2236, 2178, 2122, 2067, 2014, 1963,
	1913, 1865, 1819, 1773, 1729, 1687
};
#else
/*均衡板温温度传感器,阻值47K电阻(BYD用4150)*/
static const u32 sNTCBalTTabR[] =                                   //板温NTC(47K)温度阻值取值表[BAL_T_TAB_LEN],[-41~125]
{
	2643000,//-41℃
	2277000, 2110000, 1957000, 1816000, 1687000, 1567000, 1457000, 1355000, 1262000, 1175000,
	1095000, 1021000, 952800, 889500, 830800, 776500, 726000, 679300, 635900, 595500,
	558000, 523200, 490700, 460600, 432400, 406200, 381800, 359000, 337700, 317900,
	299300, 282000, 265700, 250600, 236400, 223000, 210600, 198900, 187900, 177700,
	168000, 159000, 150500, 142500, 134900, 127900, 121200, 114900, 109000, 103500,
	98250, 93310, 88650, 84260, 80110, 76190, 72490, 68990, 65680, 62550,
	59590, 56790, 54140, 51630, 49250, 47000, 44860, 42840, 40910, 39090,
	37360, 35710, 34150, 32670, 31260, 29920, 28640, 27430, 26270, 25180,
	24130, 23130, 22180, 21280, 20420, 19590, 18810, 18060, 17340, 16660,
	16010, 15390, 14800, 14230, 13680, 13170, 12670, 12190, 11740, 11310,
	10890, 10490, 10110, 9744, 9394, 9058, 8736, 8427, 8131, 7847,
	7575, 7313, 7062, 6821, 6589, 6367, 6153, 5948, 5751, 5561,
	5378, 5203, 5034, 4871, 4715, 4564, 4420, 4280, 4146, 4016,
	3891, 3771, 3655, 3544, 3436, 3332, 3232, 3135, 3042, 2952,
	2865, 2781, 2700, 2622, 2546, 2473, 2403, 2334, 2268, 2205,
	2143, 2084, 2026, 1970, 1916, 1864, 1813, 1765, 1717, 1671,
	1627, 1584, 1542, 1502, 1463, 1425
};
#endif

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u32 ChangeMcuTempVoltToR(u16 Vdata)
** @Input     : ADdata:温度采样电压值
** @Output    : 温度采样NTC阻值 1Ω
** @Function  : 将温度电压值转换为NTC阻值
** @The notes :
**===========================================================================================*/
static u32 ChangeMcuTempVoltToR(u16 Vdata);

/*=============================================================================================
** @Name      : static u8 SearchCellTempTabByR(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 应温度下标值
** @Function  : 通过NTC阻值查询单体温度表得到对应温度值
** @The notes :
**===========================================================================================*/
static u8 SearchCellTempTabByR(u32 data);

/*=============================================================================================
** @Name      : static s16 ChangeBalADtoTemp(u16 ADdata)
** @Input     : ADdata:均衡温度采样AD值
** @Output    : 均衡温度值 ℃
** @Function  : 将均衡温度AD值转换为实际温度值
** @The notes : AFE采到的AD值已经是电压值
**===========================================================================================*/
//static s8 ChangeBalADtoTemp(u16 ADdata);

/*=============================================================================================
** @Name      : static u32 ChangeBalTempVoltToR(u8 channel, u16 Vdata)
** @Input     : channel:均衡温度采样GPIO通道[0, 1] Vdata:均衡温度采样电压值 1mV
** @Output    : 均衡温度值 ℃
** @Function  : 将均衡温度AD值转换为实际温度值
** @The notes : AFE采到的AD值已经是电压值
**===========================================================================================*/
static u32 ChangeBalTempVoltToR(u8 channel, u16 Vdata);

/*=============================================================================================
** @Name      : static u8 SearchBalTempTabByR(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 应温度下标值
** @Function  : 通过NTC阻值查询均衡板温温度表得到对应温度值
** @The notes :
**===========================================================================================*/
static u8 SearchBalTempTabByR(u32 data);

/*=============================================================================================
** @Name      : static u8 CorrectBalTempByKB(u8 channel, u8 temp)
** @Input     : channel:均衡温度采样GPIO通道[0, 1] temp:查表均衡温度[0, 166] 1℃
** @Output    : 修正后的均衡温度值
** @Function  : 通过KB值修正均衡板温温度值
** @The notes : KB值由实际测定
**===========================================================================================*/
static u8 CorrectBalTempByKB(u8 channel, u8 temp);

/*=============================================================================================
** @Name      : static void UpdateCellBalanReqInfo(void)
** @Input     : void
** @Output    : void
** @Function  : 更新单体电压均衡需求信息
** @The notes :
**===========================================================================================*/
static void UpdateCellBalanReqInfo(void);

/*=============================================================================================
** @Name      : static u8 CheckNeedStopBalanSamp(void)
** @Input     : 是否需要关闭均衡采样:TRUE:需要 FALSE:不需要
** @Output    : void
** @Function  : 检测是否需要关闭均衡采样
** @The notes :
**===========================================================================================*/
static u8 CheckNeedStopBalanSamp(void);

/*=============================================================================================
** @Name      : static void ClrCellPasBalanCtrlCmd(void)
** @Input     : void
** @Output    : void
** @Function  : 清除单体电压被动均衡控制命令
** @The notes : 需要强制关闭所有被动均衡发送命令前调用
**===========================================================================================*/
static void ClrCellPasBalanCtrlCmd(void);

/*=============================================================================================
** @Name      : static void AFEPasEqualWrite(u32* commande, u8 total_ic)
** @Input     : commande:均衡命令   total_ic:从件个数
** @Output    : void
** @Function  : AFE的被动均衡寄存器设置执行函数
** @The notes : 6813支持18S均衡,BF8915A支持16S均衡
**===========================================================================================*/
static void AFEPasEqualWrite(u32* commande, u8 total_ic);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void CellLTC6813Init(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化6813配置寄存器所需的变量
** @The notes :
**===========================================================================================*/
void CellLTC6813Init(void)
{
	u16 bsuNum = 0;							/*组内从控并联串数 */
	u8 cell_ic = 0;
	u16 voltage_vov_threshold = 0;        	/*过压临界值*/
	u16 voltage_vuv_threshold = 0;      	/*欠压临界值*/
	#if(1 == AFE_TYPE_CHOICE)
    u8 data[MAX_LTC6813_NUM][6] = {0};
	#else
	ConfigA_TypeDef ConfigARegWrite[TOTAL_IC];
	ConfigB_TypeDef ConfigBRegWrite[TOTAL_IC];
	ConfigC_TypeDef ConfigCRegWrite[TOTAL_IC];
	ConfigD_TypeDef ConfigDRegWrite[TOTAL_IC];
	#endif

    /*使能模拟前端通信转换芯片*/
	(void)DEVGPIOSetPortState(ePOut_LTC6820EN1, ePSTATE_OFF);        /*LTC6820-1使能(低有效)*/
	(void)DEVGPIOSetPortState(ePOut_LTC6820EN2, ePSTATE_ON);         /*LTC6820-2使能(低有效)*/

	(void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_OFF);       /*LTC6820-1片选使能(低有效)*/
	(void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_ON);        /*LTC6820-2片选禁用(低有效)*/

	/*组内从控串联个数 */
	bsuNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum];

	#if(1 == AFE_TYPE_CHOICE)
	/*计算过压和欠压判断限值的配置值(AFE自测过欠压阈值,可用作电压断线判断)*/
    voltage_vov_threshold = (u16)(((u32)DEFAULT_LO_MAXV * 10) / 16);
    voltage_vuv_threshold = (u16)((((u32)DEFAULT_LO_MINV * 10) / 16) - 1);

    /*寄存器配置A*/
    sLTC6813CfgData[0] = 0x1C;	/*基准使能上电*/
    sLTC6813CfgData[1] = (u8)(voltage_vuv_threshold & 0x00ff);
    sLTC6813CfgData[2] = (u8)(((voltage_vov_threshold & 0x000f) << 4) | ((voltage_vuv_threshold & 0x0f00) >> 8));
    sLTC6813CfgData[3] = (u8)((voltage_vov_threshold & 0x0ff0) >> 4);
    sLTC6813CfgData[4] = 0x00;  /*关闭被动均衡Cell1-8*/
    sLTC6813CfgData[5] = 0x00;  /*关闭被动均衡Cell9-12和禁止软件定时器*/

	for(cell_ic = 0; cell_ic < bsuNum; cell_ic++)
	{
		data[cell_ic][0] = sLTC6813CfgData[0];
		data[cell_ic][1] = sLTC6813CfgData[1];
		data[cell_ic][2] = sLTC6813CfgData[2];
		data[cell_ic][3] = sLTC6813CfgData[3];
		data[cell_ic][4] = sLTC6813CfgData[4];
		data[cell_ic][5] = sLTC6813CfgData[5];
	}
	LTC6813_DaisyWriteCfgA(bsuNum, data);

	/*寄存器配置B*/
	for(cell_ic = 0; cell_ic < bsuNum; cell_ic++)
	{
		data[cell_ic][0] = LTC_GPIO_CT_AD_BIT;/*关闭被动均衡Cell3-16和设置单体温度对应GPIO采样通道*/
		data[cell_ic][1] = 0x00;
		data[cell_ic][2] = 0x00;
		data[cell_ic][3] = 0x00;
		data[cell_ic][4] = 0x00;
		data[cell_ic][5] = 0x00;
	}
	LTC6813_DaisyWriteCfgB(bsuNum, data);

	#else
	/*计算过压和欠压判断限值的配置值(AFE自测过欠压阈值,可用作电压断线判断)*/
	voltage_vuv_threshold = (u16)((u32)DEFAULT_LO_MAXV * 65535U / 5000U); //!< Under voltage Comparison Voltage
	voltage_vov_threshold = (u16)((u32)DEFAULT_LO_MINV * 65535U / 5000U); //!< Over voltage Comparison Voltage

	wakeup_idle(bsuNum);//保证AFE处于唤醒状态
	for(cell_ic = 0; cell_ic < bsuNum; cell_ic++)
	{
		ConfigARegWrite[cell_ic].gpio_pd_en = 0xff;			//GPIO下拉使能，每一位对应一个IO口
		ConfigARegWrite[cell_ic].gpio_analog_en = 0;		//GPIO0~7模式选择		0:模拟输入		1:普通GPIO
		ConfigARegWrite[cell_ic].rsvd2 = 0;				    //保留位，永远置0
		ConfigARegWrite[cell_ic].rsvd1 = 0;		        	//保留位，永远置0
		ConfigARegWrite[cell_ic].osr_sel = 0;				//ADC过采样率选择  1:ADC过采样率为1024、2048、4096、4096	 0:ADC过采样率为64、128、256、512
		ConfigARegWrite[cell_ic].refon = 1;					//VREF1(基准电压)配置 1 :VREF1保持上电直到WDT溢出  0 :ADC转换完之后VREF1关闭 (默认)
		ConfigARegWrite[cell_ic].uv_th = voltage_vuv_threshold;	//欠压阈值
		ConfigARegWrite[cell_ic].ov_th = voltage_vov_threshold;	//过压阈值
		ConfigARegWrite[cell_ic].rx_pec_match = 0;			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
	}
	BF8915A_wrcfga(bsuNum, ConfigARegWrite);

	wakeup_idle(bsuNum);//保证AFE处于唤醒状态
	for(cell_ic = 0; cell_ic < bsuNum; cell_ic++)
	{
		ConfigBRegWrite[cell_ic].dcc = 0;					//放电电池开关1:打开Cell x 的开关  0:关闭Cell x 的开关 (默认)
		ConfigBRegWrite[cell_ic].bl_duty_sel = 0;			//主动均衡占空比选择  占空比=(BL_DUTY_SEL+1)*10%，配置范围10%~100%，当大于9时是100%。
		ConfigBRegWrite[cell_ic].dcto = 0;					//放电定时溢出值配置
		ConfigBRegWrite[cell_ic].vuv_bl_th = 0;				//均衡欠压阈值
		ConfigBRegWrite[cell_ic].dtmen = 0;					//在自动均衡下，如果DTMEN=1，当欠压(小于VUV_BL)关闭放电开关，如果DTMEN=0，不进行欠压比较
		ConfigBRegWrite[cell_ic].rx_pec_match = 0; 			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
	}
	BF8915A_wrcfgb(bsuNum, ConfigBRegWrite);

	wakeup_idle(bsuNum);//保证AFE处于唤醒状态
	for(cell_ic = 0; cell_ic < bsuNum; cell_ic++)
	{
		ConfigCRegWrite[cell_ic].vuv_m_th = 0;				//监测模式下，欠压阈值
		ConfigCRegWrite[cell_ic].vov_m_th = 0;				//监测模式下，过压阈值
		ConfigCRegWrite[cell_ic].itov_m_th = 0;				//监测模式下，内部过温阈值
		ConfigCRegWrite[cell_ic].extov_m_th = 0; 			//监测模式下，外部过温阈值
		ConfigCRegWrite[cell_ic].rx_pec_match = 0; 			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
	}
	BF8915A_wrcfgc(bsuNum, ConfigCRegWrite);

	wakeup_idle(bsuNum);//保证AFE处于唤醒状态
	for(cell_ic = 0; cell_ic < bsuNum; cell_ic++)
	{
		ConfigDRegWrite[cell_ic].csel_m = 0;				//监测模式下，电池电压监测比较选择 1:相应的电池就会在监测模式下进行过欠压比较，置标志位 0:相应的电池就会在监测模式下不进行过欠压比较，不置标志位
		ConfigDRegWrite[cell_ic].gsel_m = 0;				//监测模式下，GPIO0~7比较选择
		ConfigDRegWrite[cell_ic].isospi_wake_t = 0;			//isospi(菊花链SPI)唤醒时间，0:10us		1:20us
		ConfigDRegWrite[cell_ic].adc_rst_sel = 0;			//测量第1个通道ADC复位选择，0:所有测量通道都复位ADC，1:只有第1个通道复位ADC，之后2~n通道不复位ADC
		ConfigDRegWrite[cell_ic].time_sel_m = 0;			//监测模式下，定时监测时间配置，定时时间=(TIME_SEL_M+1)*1s(2~64s)，最小配置值是1(2s)
		ConfigDRegWrite[cell_ic].adc_init_t2 = 0x02;		//ADC测量第2~n个通道ADC初始化时间，(ADC_INIT_T2+1)*20us(20~320us)
		ConfigDRegWrite[cell_ic].adc_init_t1 = 0x0F;		//ADC测量第1个通道ADC初始化时间(ADC_INIT_T1+1)*100us(100~1600us)
		ConfigDRegWrite[cell_ic].trim2_en = 1;				//修调2使能，1:使能，0:不使能
		ConfigDRegWrite[cell_ic].trim1_en = 1;				//修调1使能，1:使能，0:不使能
		ConfigDRegWrite[cell_ic].err_t_sel_m = 0;			//监测模式下发生异常，上报的间隔时间  00:200ms，01:400ms，10:600ms，11:800ms
		ConfigDRegWrite[cell_ic].vd33t_on_m = 0;			//监测模式下，VREF1和VD33T的启动时间配置(5+VD33T_ON_M*10)ms，范围是5~155ms
		ConfigDRegWrite[cell_ic].rx_pec_match = 0; 			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
	}
	BF8915A_wrcfgd(bsuNum, ConfigDRegWrite);
	#endif
}

/*=============================================================================================
** @Name      : void CellInfoSampleTask(void *p)
** @Input     : *p:形参,任务函数参数需要
** @Output    : void
** @Function  : 单体电压、温度采样任务
** @The notes : 该任务目前只支持单线采样(暂不实现双线)
**===========================================================================================*/
void CellInfoSampleTask(void *p)
{
    static u8 sState = 0;
    static BOOLEAN results_error = TRUE;
    static u8 noVSampCounter[MAX_LTC6813_NUM] = {0};     /*电压采样失效次数*/
    static u8 noTSampCounter[MAX_LTC6813_NUM] = {0};     /*温度采样失效次数*/
	//u16 TempGPIOBuff[9] = {0};          	     		 /*6813辅助寄存器值*/
    u8 cell_Ic = 0;  									 /* IC counter */
    u8 cell_Id = 0;  									 /* cell counter */
	u8 cell_num = 0; 									 /* cell Num  */
    u8 k = 0;
	s16 cellTemp = 0;
	s16 balTemp = 0;
	u32 tempR = 0;
	u16 bsuNum = 0;		/*组内从控并联串数 */
	//u16 bsuVNum = 0;	/*一个从控电压采样节数*/
	//u16 bsuTNum = 0;	/*一个从控温度采样节数*/
	//u16 balTNum = 0;	/*一个从控板温采样节数*/
	u16 anaVNum = 0;
	u8 rcvMsgNum = 0;
	//u8 regNum = 0;
	#if(0 == AFE_TYPE_CHOICE)
	u16 tempVolt = 0;
	#endif
    p = p;

	//bsuNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum];	/*组内从控并联串数 */
	//bsuVNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum];	/*一个从控电压采样节数*/
	//bsuTNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum];	/*一个从控温度采样节数*/
	//balTNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum];	/*一个从控板温采样节数*/

	for(;;)
	{
		bsuNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum];		/*组内从控并联串数 */
		//bsuVNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum];	/*一个从控电压采样节数*/
		//bsuTNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum];	/*一个从控温度采样节数*/
		//balTNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum];	/*一个从控板温采样节数*/

		switch(sState)
		{
			case 0:
				/*测试程序从B口读温度(电压切回A口)*/
				#if(0 == PRJ_PACK_CELL_INFO)
				if((1 == gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
					/*&& (18 == gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum])*/)
				{
					/*使能模拟前端A口*/
					(void)DEVGPIOSetPortState(ePOut_LTC6820EN1, ePSTATE_OFF);           /*LTC6820-1使能(低有效)*/
					(void)DEVGPIOSetPortState(ePOut_LTC6820EN2, ePSTATE_ON);            /*LTC6820-2禁用(低有效)*/

					(void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_OFF);          /*LTC6820-1片选使能(低有效)*/
					(void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_ON);           /*LTC6820-2片选禁用(低有效)*/

					/*选择A口发送SPI通信*/
					#if(1 == AFE_TYPE_CHOICE)
					ltcSpi_port_Select(0);
					#else
					isospi_reverse = FALSE;
					#endif
				}
				#endif
				UpdateCellBalanReqInfo();										/*更新均衡信息*/

				/*达到均衡采样周期*/
				if(TRUE == CheckNeedStopBalanSamp())
				{
					/*关闭均衡再采样*/
					ClrCellPasBalanCtrlCmd();                                   /*清除被动均衡命令*/
					AFEPasEqualWrite(sLTC6813PasBalanCmd, bsuNum);              /*控制均衡全部关闭*/
					sLTC6813PasBalanCtrlState = 0;                              /*记录未控制均衡*/
					sState = 1;
				}
				/*未达到均衡采样周期*/
				else
				{
					#if(1 == AFE_TYPE_CHOICE)
					/*直接启动采样*/
					LTC6813_Cell_ADC_Clear(LTC6813_BROADCAST);    				/*清除LTC6813内部AD值*/
					#else
					wakeup_idle(bsuNum);
					/*直接启动采样*/
					BF8915A_clrcell();    										/*清除BF8915A内部电池电压寄存器*/
					#endif
					sState = 2;
				}
				OSIF_TimeDelay(10);
				break;

			/*清除原AD值*/
			case 1:
				#if(1 == AFE_TYPE_CHOICE)
				LTC6813_Cell_ADC_Clear(LTC6813_BROADCAST);    					/*清除LTC6813内部AD值*/
				#else
				wakeup_idle(bsuNum);
				BF8915A_clrcell();    										/*清除BF8915A内部电池电压寄存器*/
				#endif
				OSIF_TimeDelay(10);
				sState = 2;
				break;

			/*启动电压采样AD转换*/
			case 2:
				#if(1 == AFE_TYPE_CHOICE)
				LTC6813_Cell_ADC_Start(LTC6813_BROADCAST, LTC6813_CONVERSION_7KHZ_MODE, LTC6813_CH_ALL, FALSE);
				OSIF_TimeDelay(20);     /*7KHZ标准模式下扫描18节电压时间不能低于3.9ms*/
				sState = 3;
				#else
				wakeup_idle(bsuNum);
				BF8915A_adcv(CLKSEL_2M, OSR_256, DISCP_DISABLE, CSEL_CH_ALL);
				OSIF_TimeDelay(40);     /*7KHZ标准模式下扫描18节电压时间不能低于3.9ms*/
				sState = 5;
				#endif
				
//				if(bsuNum < 7)
//				{
//					OSIF_TimeDelay(20);                /*至少等待时间*/
//				}
//				else
//				{
//					OSIF_TimeDelay(bsuNum*LTC_RD_AD_T);/*根据从控数目定义等待时间*/
//				}
				break;

			#if(1 == AFE_TYPE_CHOICE)
			/*读取电压AD值(分次读AB寄存器)*/
			case 3:
				//正在读取禁止中断(防止被其他任务打断)[禁止中断会导致只能读取11个从控,原因待查]
				//BSP_DISABLE_IRQ();
				//for(regNum = 0; regNum <= 4; )
				//{
					//results_error = LTC6813_Cell_ADC_Read(LTC6813_CH_ALL, sLTC6813CellVoltAD, bsuNum, regNum);
					results_error = LTC6813_Cell_ADC_ReadAB(LTC6813_CH_ALL, sLTC6813CellVoltAD, bsuNum);/*读取AD采样值(寄存器A/B)*/
					//regNum = regNum + 2;
				//}
				//BSP_ENABLE_IRQ();
				sState = 4;

				OSIF_TimeDelay(30);   /*读取电压寄存器分三次读取(first read: A/B寄存器  second read: C/D/E/F寄存器)*/
//				if(bsuNum < 7)
//				{
//					OSIF_TimeDelay(20);                /*至少等待时间*/
//				}
//				else
//				{
//					OSIF_TimeDelay(bsuNum*LTC_RD_AD_T);/*根据从控数目定义等待时间*/
//				}
				break;

			/*读取电压AD值(分次读CD寄存器)*/
			case 4:
				//正在读取禁止中断(防止被其他任务打断)[禁止中断会导致只能读取11个从控,原因待查]
				//BSP_DISABLE_IRQ();
				results_error &= LTC6813_Cell_ADC_ReadCD(LTC6813_CH_ALL, sLTC6813CellVoltAD, bsuNum);/*读取AD采样值(寄存器A/B)*/
				//BSP_ENABLE_IRQ();
				sState = 5;

				OSIF_TimeDelay(30);   /*读取电压寄存器分三次读取(first read: A/B寄存器  second read: C/D/E/F寄存器)*/
//				if(bsuNum < 7)
//				{
//					OSIF_TimeDelay(20);                /*至少等待时间*/
//				}
//				else
//				{
//					OSIF_TimeDelay(bsuNum*LTC_RD_AD_T);/*根据从控数目定义等待时间*/
//				}
				break;

			/*读取电压AD值(分次读EF寄存器)*/
			case 5:
				//正在读取禁止中断(防止被其他任务打断)[禁止中断会导致只能读取11个从控,原因待查]
				//BSP_DISABLE_IRQ();
				results_error &= LTC6813_Cell_ADC_ReadEF(LTC6813_CH_ALL, sLTC6813CellVoltAD, bsuNum);/*读取AD采样值(寄存器A/B)*/
				//BSP_ENABLE_IRQ();
				rcvMsgNum = 0;
				sState = 6;

				OSIF_TimeDelay(30);   /*读取电压寄存器分三次读取(first read: A/B寄存器  second read: C/D/E/F寄存器)*/
//				if(bsuNum < 7)
//				{
//					OSIF_TimeDelay(20);                /*至少等待时间*/
//				}
//				else
//				{
//					OSIF_TimeDelay(bsuNum*LTC_RD_AD_T);/*根据从控数目定义等待时间*/
//				}
				break;

			#else
			/*读取电压AD值(一次性读取所有寄存器)*/
			case 5:
				wakeup_idle(bsuNum);
				results_error &= BF8915A_rdcv(0, bsuNum, BF8915ACellVoltage);
				OSIF_TimeDelay(200);
				sState = 6;
				break;
			#endif

			/*校验单体电压AD值*/
			case 6:
				/*校验AD值接收正常(有回复)*/
				//if(results_error == TRUE)
				{
					/*逐个从控解析*/
					for(cell_Ic = 0; cell_Ic < bsuNum; cell_Ic++)
					{
						/*记录能接收到报文通信正常*/
						DiagRcvDevMsgAddAPI(cell_Ic);

						//兼容CATL和标准PACK电池14[7+7]串解析
						#if(3 == PRJ_PACK_CELL_INFO)
						{
							anaVNum = 15;
						}
						//兼容标准PACK电池16[8+8]串解析
						#elif(2 == PRJ_PACK_CELL_INFO)
						{
							anaVNum = 17;
						}
						//兼容CATL液冷PACK电池52[16+18+18]串解析
						#elif(4 == PRJ_PACK_CELL_INFO)
						{
							//每3个从控首个少2S
							if(0 == (cell_Ic % 3))
							{
								anaVNum = 16;
							}
							else
							{
								anaVNum = 18;
							}
						}
						//常规电池
						#else
						{
							anaVNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum];
						}
						#endif

						/*逐个单体解析*/
						//for(cell_Id = 0; cell_Id < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]; cell_Id++)
						for(cell_Id = 0; cell_Id < anaVNum; cell_Id++)
						{
							/*该从控单体采样回复数据正常*/
							#if(1 == AFE_TYPE_CHOICE)
							//if(sLTC6813CellVoltAD[cell_Ic][cell_Id] != LTC6813_ADC_CLEAR)
							if(sLTC6813CellVoltAD[cell_Ic][cell_Id] <= 60000)
							#else
							if(BF8915ACellVoltage[cell_Ic].cellVoltage[cell_Id] <= 60000)
							#endif
							{
								//兼容CATL和标准PACK电池14[7+7]串(中间留空1串)解析
								#if(3 == PRJ_PACK_CELL_INFO)
								{
									//14串电池第8串空闲不处理
									if(cell_Id < 7)
									{
										cell_num = cell_Id;
									}
									else if(cell_Id > 7)
									{
										cell_num = cell_Id - 1;
									}
									else
									{
										continue;
									}
								}
								//兼容标准PACK电池16[8+8]串(中间留空1串)解析
								#elif(2 == PRJ_PACK_CELL_INFO)
								{
									//16串电池第9串空闲不处理
									if(cell_Id < 8)
									{
										cell_num = cell_Id;
									}
									else if(cell_Id > 8)
									{
										cell_num = cell_Id - 1;
									}
									else
									{
										continue;
									}
								}
								//有效电压通道连续(中间不留空)解析
								#else
								{
									cell_num = cell_Id;
								}
								#endif

								/*清除采样异常标志*/
								DiagRecordAFESampErrFlagAPI(cell_Ic, 0);
								noVSampCounter[cell_Ic] = 0;

								//voltage_cell[cell_Ic][cell_num] = sLTC6813CellVoltAD[cell_Ic][cell_Id]/10;/*AD转化为电压值,分辨率1mv*/

								#if(1 == AFE_TYPE_CHOICE)
								/*采样电压值判断断线*/
								DiagCheckCellVOffLFlagAPI(cell_Ic, cell_num, sLTC6813CellVoltAD[cell_Ic][cell_Id] / 10);/*电压采样线断线检测*/
								#else
								/*采样电压值判断断线*/
								DiagCheckCellVOffLFlagAPI(cell_Ic, cell_num, (u16)((u32)BF8915ACellVoltage[cell_Ic].cellVoltage[cell_Id] * 5000 / 65535));/*电压采样线断线检测*/
								#endif

								/*调试模式或无均衡或非均衡电池*/
								if((eMODE_DEBUG == GetGWorkModeAPI())
									|| (0 == sLTC6813PasBalanCtrlState)
									|| (0 == (sLTC6813PasBalanCmd[cell_Ic] & (0X00001 << cell_Id))))
								{
									#if(1 == AFE_TYPE_CHOICE)
									/*电压1V以上开始补偿*/
									if(sLTC6813CellVoltAD[cell_Ic][cell_Id] >= 10000)
									{
										/*每个从控首节电池补偿+3.0mv*/
										if(0 == cell_num)
										{
											sLTC6813CellVoltAD[cell_Ic][cell_Id] += CELF_COMP_VOLT;
										}
										/*每个从控尾节电池补偿+2.5mv*/
										else if((gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] - 1) == cell_num)
										{
											sLTC6813CellVoltAD[cell_Ic][cell_Id] += CELE_COMP_VOLT;
										}
										/*其它单体补偿+2.3mv*/
										else
										{
											sLTC6813CellVoltAD[cell_Ic][cell_Id] += CELM_COMP_VOLT;
										}
									}

									/*输出采样电压值*/
									WriteSampCellVoltHook(cell_Ic, cell_num, sLTC6813CellVoltAD[cell_Ic][cell_Id] / 10);  /*输出电压值*/
									#else
									/*输出采样电压值*/
									WriteSampCellVoltHook(cell_Ic, cell_num, (u16)((u32)BF8915ACellVoltage[cell_Ic].cellVoltage[cell_Id] * 5000 / 65535));  /*输出电压值*/
									#endif
								}
							}
							/*该从控单体采样回复数据异常*/
							else
							{
								/*该从控采样异常计数*/
								noVSampCounter[cell_Ic]++;
								/*达到异常次数则记录故障并清除该从控数据*/
								if(noVSampCounter[cell_Ic] >= SAMPE_ERR_TIMES)
								{
									/*记录该从控采样异常*/
									DiagRecordAFESampErrFlagAPI(cell_Ic, 1);
									noVSampCounter[cell_Ic] = SAMPE_ERR_TIMES;

									/*采样失效清除单体电压信息(输出单体电压值为0)*/
									ClrSampCellVoltHook(cell_Ic);
								}
								else
								{
									//重新初始化
									#if(1 == AFE_TYPE_CHOICE)
									CellLTC6813Init();
									#endif
								}
								break;
							}
						}

						/*该从控采样正常或已确认采样异常*/
						if((0 == noVSampCounter[cell_Ic])
							|| (noVSampCounter[cell_Ic] >= SAMPE_ERR_TIMES))
						{
							/*累计已采样从控数目*/
							rcvMsgNum++;
						}
					}

					/*每个从控都有数据*/
					if(rcvMsgNum >= bsuNum)
					{
						/*记录完成一遍电压采样*/
						WriteCellVSampFinFlagHook(1);
					}

					/*测试程序从B口读温度(初始化B口)*/
					#if(0 == PRJ_PACK_CELL_INFO)
					if((1 == gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
						/*&& (18 == gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum])*/)
					{
						/*使能模拟前端B口*/
						(void)DEVGPIOSetPortState(ePOut_LTC6820EN1, ePSTATE_ON);        /*LTC6820-1禁用(低有效)*/
						(void)DEVGPIOSetPortState(ePOut_LTC6820EN2, ePSTATE_OFF);       /*LTC6820-2使能(低有效)*/

						(void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_ON);       /*LTC6820-1片选禁用(低有效)*/
						(void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_OFF);      /*LTC6820-2片选使能(低有效)*/

						/*选择B口发送SPI通信*/
						#if(1 == AFE_TYPE_CHOICE)
						ltcSpi_port_Select(1);
						#else
						isospi_reverse = TRUE;
						#endif
					}
					#endif

					/*有任一电压数据则继续温度采样*/
					sState = 7;                                 /*进行温度采集*/
					rcvMsgNum = 0;
					OSIF_TimeDelay(5);            				/*延时5ms*/
				}
//				/*校验AD值接收异常(无任何回复)*/
//				else                                            /*读取AD采样值失效*/
//				{
//					/*通信故障确认后再清零*/
//					if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_COFF))
//					{
//						/*逐个从控清除数据*/
//						for(cell_Ic = 0; cell_Ic < bsuNum; cell_Ic++)
//						{
//							/*采样失效清除单体信息(包括设置采样完成)*/
//							ClrAfeSampVoltTemp(cell_Ic);
//						}
//					}
//
//					//重新初始化
//					CellLTC6813Init();
//
//					/*返回重新采样*/
//					sState = 0;
//					OSIF_TimeDelay(1);
//				}
				break;

			/*LTC6813温度AD清零*/
			case 7:
				#if(1 == AFE_TYPE_CHOICE)
				LTC6813_GPIO_ADC_Clear(LTC6813_BROADCAST); /*清除LTC6813内部AD值*/
				#else
				wakeup_idle(bsuNum);
				BF8915A_clrgp();
				#endif
				sState = 8;
				OSIF_TimeDelay(10);          /*这个时间随着菊花链上的从件增多必须是相应倍数(time = N * 5ms)*/
				break;

			/*启动LTC6813温度采集AD转换*/
			case 8:
				#if(1 == AFE_TYPE_CHOICE)
				LTC6813_GPIO_ADC_Start(LTC6813_BROADCAST, LTC6813_CONVERSION_7KHZ_MODE, LTC6813_CHG_GPIO_ALL);
				#else
				wakeup_idle(bsuNum);
				BF8915A_adgp(CLKSEL_2M, OSR_256, GSEL_CH_ALL);
				#endif
				sState = 9;
				OSIF_TimeDelay(20);          /*这个时间随着菊花链上的从件增多必须是相应倍数(time = N * 5ms)*/
//				if(bsuNum < 7)
//				{
//					OSIF_TimeDelay(20);        /*至少等待时间*/
//				}
//				else
//				{
//					OSIF_TimeDelay(bsuNum*LTC_RD_AD_T);/*根据从控数目定义等待时间*/
//				}
				break;

			/*读LTC6813温度采集AD值*/
			case 9:
				#if(1 == AFE_TYPE_CHOICE)
				//正在读取禁止中断(防止被其他任务打断)[禁止中断会导致只能读取11个从控,原因待查]
				//BSP_DISABLE_IRQ();
				results_error = LTC6813_GPIO_ADC_Read(LTC6813_NUM_GPIO, LTC6813_CHG_GPIO_ALL, sLTC6813AUXGPIOAD, bsuNum);
				//BSP_ENABLE_IRQ();
				#else
				wakeup_idle(bsuNum);
				results_error = BF8915A_rdgv(0, bsuNum, BF8915AGpioVoltage);
				#endif
				sState = 10;

				OSIF_TimeDelay(50);          /*这个时间随着菊花链上的从件增多必须是相应倍数(time = N * 3ms)*/
//				if(bsuNum < 7)
//				{
//					OSIF_TimeDelay(20);        /*至少等待时间*/
//				}
//				else
//				{
//					OSIF_TimeDelay(bsuNum*LTC_RD_AD_T);/*根据从控数目定义等待时间*/
//				}
				break;

			/*计算LTC6813温度AD值*/
			case 10:
				//if(results_error == TRUE)
				{
					/*逐个从控解析*/
					for(cell_Ic = 0; cell_Ic < bsuNum; cell_Ic++)
					{
						/*ADI芯片*/
						#if(1 == AFE_TYPE_CHOICE)
						/*从控内逐个温度数据处理(12个数据温度只分布在前10个)*/
						//for(k = 0; k < 12; k++)
						for(k = 0; k < 10; k++)
						{
							/*采样数据有效*/
							//if(sLTC6813AUXGPIOAD[cell_Ic][k] != LTC6813_ADC_CLEAR)
							if(sLTC6813AUXGPIOAD[cell_Ic][k] <= 60000)
							{
								/*清除采样异常标志*/
								DiagRecordAFESampErrFlagAPI(cell_Ic, 2);
								noTSampCounter[cell_Ic] = 0;

								/*均衡电阻温度(对应芯片GPIO2-GPIO1)*/
								if(k < 2)
								{
									/*GPIO3~5没有使用*/
									//TempGPIOBuff[k] = sLTC6813AUXGPIOAD[cell_Ic][5 - k] / 10;
									/*计算实际温度值(A样板已是实际温度带负号)*/
									//balTemp = ChangeBalADtoTemp(sLTC6813AUXGPIOAD[cell_Ic][k] / 10) + 41;
									/*计算实际温度值*/
									tempR = ChangeBalTempVoltToR(k, sLTC6813AUXGPIOAD[cell_Ic][k] / 10);

									balTemp = SearchBalTempTabByR(tempR);
									balTemp = CorrectBalTempByKB(k, balTemp);

									/*均衡温度采样线断线检测*/
									DiagCheckBalanTOffLFlagAPI(cell_Ic, (1 - k), balTemp - 41);

									/*输出均衡电阻温度,按温度顺序排列[0,1]*/
									WriteSampBalanTempHook(cell_Ic, (1 - k), balTemp - 41);
								}
								/*电池温度(对应芯片GPIO10-GPIO7)*/
								else if((k > 5) && (k < 10))
								{
									//TempGPIOBuff[k] = sLTC6813AUXGPIOAD[cell_Ic][8 - k] / 10;
									tempR = ChangeMcuTempVoltToR(sLTC6813AUXGPIOAD[cell_Ic][k] / 10);

									cellTemp = SearchCellTempTabByR(tempR);
									//cellTemp = 67;

									/*单体温度采样线断线检测*/
									DiagCheckCellTOffLFlagAPI(cell_Ic, 9 - k, (cellTemp - 41));

									/*输出电池温度值,按温度顺序排列[0,3]*/
									WriteSampCellTempHook(cell_Ic, 9 - k, (cellTemp - 41));
								}
								else
								{
									;/*2、3、4、10、11:不使用，5：第二基准源采样电压值不需要解析*/
								}
							}
							/*该从控单体采样回复数据异常*/
							else
							{
								/*有效采样通道*/
								if((k < 2) || ((k > 5) && (k < 10)))
								{
									/*该从控采样异常计数*/
									noTSampCounter[cell_Ic]++;
									/*达到异常次数则记录故障并清除该从控数据*/
									if(noTSampCounter[cell_Ic] >= SAMPE_ERR_TIMES)
									{
										/*记录该从控采样异常*/
										DiagRecordAFESampErrFlagAPI(cell_Ic, 3);
										noTSampCounter[cell_Ic] = SAMPE_ERR_TIMES;

										/*采样失效清除单体温度信息*/
										ClrSampCellTempHook(cell_Ic);
									}
									break;
								}
							}
						}

						/*BYD芯片*/
						#else
						/*先取温度采样上拉电压3.3V实际值(在GPIO[4])*/
						if(BF8915AGpioVoltage[cell_Ic].gpioVoltage[4] < 60000)
						{
							tempVolt = BF8915AGpioVoltage[cell_Ic].gpioVoltage[4] * 5000 / 65535 - 17;
							if(tempVolt >= 3310)
							{
								sBydTempStandV = 3295;
							}
							else if(tempVolt >= 3295)
							{
								sBydTempStandV = 3290;
							}
							else if(tempVolt >= 2500)
							{
								sBydTempStandV = tempVolt;
							}
							else
							{
								sBydTempStandV = 2500;
							}
						}

						/*从控内逐个温度数据处理(9个数据温度只分布在后7个)*/
						//for(k = 0; k < 9; k++)
						for(k = 2; k < 9; k++)
						{
							/*采样数据有效*/
							if(BF8915AGpioVoltage[cell_Ic].gpioVoltage[k] <= 60000)
							{
								/*清除采样异常标志*/
								DiagRecordAFESampErrFlagAPI(cell_Ic, 2);
								noTSampCounter[cell_Ic] = 0;

								/*均衡电阻温度(对应芯片GPIO7-GPIO8)*/
								if(k > 6)
								{
									/*GPIO7~GPIO8计算实际温度值*/
									tempR = ChangeBalTempVoltToR(k, (u16)((u32)BF8915AGpioVoltage[cell_Ic].gpioVoltage[k] * 5000 / 65535));

									balTemp = SearchBalTempTabByR(tempR);
									balTemp = CorrectBalTempByKB(k, balTemp);

									/*均衡温度采样线断线检测*/
									DiagCheckBalanTOffLFlagAPI(cell_Ic, (k - 7), balTemp - 41);

									/*输出均衡电阻温度,按温度顺序排列[0,1]*/
									WriteSampBalanTempHook(cell_Ic, (k - 7), balTemp - 41);
								}
								/*电池温度(对应芯片GPIO5-GPIO6,为T2~T3)*/
								else if(k > 4)
								{
									tempR = ChangeMcuTempVoltToR((u16)((u32)BF8915AGpioVoltage[cell_Ic].gpioVoltage[k] * 5000 / 65535));

									cellTemp = SearchCellTempTabByR(tempR);
									//cellTemp = 67;

									/*单体温度采样线断线检测*/
									DiagCheckCellTOffLFlagAPI(cell_Ic, k - 3, (cellTemp - 41));

									/*输出电池温度值,按温度顺序排列[0,3]*/
									WriteSampCellTempHook(cell_Ic, k - 3, (cellTemp - 41));
								}
								/*电池温度(对应芯片GPIO2-GPIO3,为T0~T1)*/
								else if(k < 4)
								{
									tempR = ChangeMcuTempVoltToR((u16)((u32)BF8915AGpioVoltage[cell_Ic].gpioVoltage[k] * 5000 / 65535));

									cellTemp = SearchCellTempTabByR(tempR);
									//cellTemp = 67;

									/*单体温度采样线断线检测*/
									DiagCheckCellTOffLFlagAPI(cell_Ic, k - 2, (cellTemp - 41));

									/*输出电池温度值,按温度顺序排列[0,3]*/
									WriteSampCellTempHook(cell_Ic, k - 2, (cellTemp - 41));
								}
								else
								{
									;/*GPIO4为VDD基准源采样电压值不需要解析*/
								}
							}
							/*该从控温度采样回复数据异常*/
							else
							{
								/*有效采样通道*/
								if(4 != k)
								{
									/*该从控采样异常计数*/
									noTSampCounter[cell_Ic]++;
									/*达到异常次数则记录故障并清除该从控数据*/
									if(noTSampCounter[cell_Ic] >= SAMPE_ERR_TIMES)
									{
										/*记录该从控采样异常*/
										DiagRecordAFESampErrFlagAPI(cell_Ic, 3);
										noTSampCounter[cell_Ic] = SAMPE_ERR_TIMES;

										/*采样失效清除单体温度信息*/
										ClrSampCellTempHook(cell_Ic);
									}
									break;
								}
							}
						}
						#endif

						/*该从控采样正常或已确认采样异常*/
						if((0 == noTSampCounter[cell_Ic])
							|| (noTSampCounter[cell_Ic] >= SAMPE_ERR_TIMES))
						{
							/*累计已采样从控数目*/
							rcvMsgNum++;
						}
					}

					/*每个从控都有数据*/
					if(rcvMsgNum >= bsuNum)
					{
						/*记录温度采样一遍完成*/
						WriteCellTSampFinFlagHook(1);
					}

					/*电压、温度采样完成,开启均衡*/
					sState = 12;
					OSIF_TimeDelay(5);
				}
//				/*校验AD值接收异常(无任何回复)*/
//				else
//				{
//					/*马上返回重新电压采样*/
//					sState = 0;
//					OSIF_TimeDelay(1);
//				}
				break;

			case 12:
				UpdateCellBalanReqInfo();				            /*更新均衡信息(正常执行均衡)*/
				if(sLTC6813PasBalanReqState > 0)		            /*有开启均衡需求*/
				{
					#if(1 == AFE_TYPE_CHOICE)
					AFEPasEqualWrite(sLTC6813PasBalanCmd, bsuNum);  /*开启被动均衡*/
					sLTC6813PasBalanCtrlState = 1;                  /*记录已控制均衡*/
					sState = 0;								        /*进行下一个周期*/
					OSIF_TimeDelay(50);  					        /*均衡开启时长,用户可进行修改*/
					#else
					//BYD AFE开均衡前需要先全部关闭再重新开启
					ClrCellPasBalanCtrlCmd();                       /*清除被动均衡命令*/
					AFEPasEqualWrite(sLTC6813PasBalanCmd, bsuNum);  /*关闭被动均衡*/
					sLTC6813PasBalanCtrlState = 0;                  /*记录已控制均衡*/
					sState = 13;								    /*进行均衡重新开启*/
					OSIF_TimeDelay(3);  					        /*均衡开启时长,用户可进行修改*/
					#endif
				}
				else		                                        /*无开启均衡需求*/
				{
					if(sLTC6813PasBalanCtrlState > 0)               /*已控制均衡开启*/
					{
						/*关闭均衡*/
						//ClrCellPasBalanCtrlCmd();                  /*清除被动均衡命令*/
						AFEPasEqualWrite(sLTC6813PasBalanCmd, bsuNum);/*控制均衡全部关闭*/
					}
					sLTC6813PasBalanCtrlState = 0;                  /*记录未控制均衡*/
					sState = 0;								        /*进行下一个周期*/
					OSIF_TimeDelay(3);  					        /*立刻返回下一周期采样*/
				}
				break;

			#if(0 == AFE_TYPE_CHOICE)
			case 13:
				UpdateCellBalanReqInfo();				            /*更新均衡信息(正常执行均衡)*/
				if(sLTC6813PasBalanReqState > 0)		            /*有开启均衡需求*/
				{
					AFEPasEqualWrite(sLTC6813PasBalanCmd, bsuNum);  /*开启被动均衡*/
					sLTC6813PasBalanCtrlState = 1;                  /*记录已控制均衡*/
					sState = 0;								        /*进行下一个周期*/
					OSIF_TimeDelay(50);  					        /*均衡开启时长,用户可进行修改*/
				}
				else		                                        /*无开启均衡需求*/
				{
					if(sLTC6813PasBalanCtrlState > 0)               /*已控制均衡开启*/
					{
						/*关闭均衡*/
						//ClrCellPasBalanCtrlCmd();                  /*清除被动均衡命令*/
						AFEPasEqualWrite(sLTC6813PasBalanCmd, bsuNum);/*控制均衡全部关闭*/
					}
					sLTC6813PasBalanCtrlState = 0;                  /*记录未控制均衡*/
					sState = 0;								        /*进行下一个周期*/
					OSIF_TimeDelay(3);  					        /*立刻返回下一周期采样*/
				}
				break;
			#endif

			default:
				sState = 0;
				OSIF_TimeDelay(1);
				break;
		}
	}
}
#endif

/*=============================================================================================
** @Name      : void WriteSampCellVoltHook(u8 ic_Num, u8 num, u16 volt)
** @Input     : ic_Num:第几个从控[0,31]  num:单体电池号 volt:单节电压值 1mv
** @Output    : void
** @Function  : 输出单节电压采样值
** @The notes : 用户将输出值放到单体寄存器保存
**===========================================================================================*/
void WriteSampCellVoltHook(u8 ic_Num, u8 num, u16 volt)
{
    u32 voltage = 0;
    #if(0 == BMU_BSU_COM_TYPE)/*菊花链单体采样方式*/
    static u8 voltErrTime[MAX_LTC6813_NUM][MAXVOLTNUM] = {0};/*软硬写死最大支持22个LTC6813，如有增加软件需要变更*/
    #else/*CAN通信单体采样方式*/
    static u8 voltErrTime[MAX_BSU_NUM][MAX_BSU_CELL] = {0};  /*软硬写死最大支持22个LTC6813，如有增加软件需要变更*/
    #endif

    if(num >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum])
    {
        return;
    }

    /*检测程序模式不考虑异常*/
    if(eMODE_DEBUG == GetGWorkModeAPI())
    {
        if((sVSampFinFlag == 0)                             /*未完成一遍采样*/
            || (BSPGetOSMillSecTimeAPI() < 5000)            /*上电前5s*/
            || (ABS(GetBsuCellVolt(ic_Num, num), volt) > 100))/*出现电压突变100mv*/
        {
            InputBsuCellVolt(ic_Num, num, volt);			/*不滤波*/
        }
        else                                            	/*正常采样*/
        {
            if(ABS(GetBsuCellVolt(ic_Num, num), volt) > 2)
            {
                voltage = ((u32)GetBsuCellVolt(ic_Num, num) * 20 + (u32)volt * 12) / 32;
            }
            else
            {
                voltage = volt;
            }

            InputBsuCellVolt(ic_Num, num, voltage);
        }
        return;
    }

    /*出现采样电路异常但断线故障未反馈到BMU则不更新单体信息(保证BMU出现电压失效后不报高低压)*/
    if((DiagGetCellSampErrFlagAPI() > 0) && (eErr_A == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP)))
    {
        return;
    }

    /*出现电压断线但断线故障未反馈到BMU则不更新单体信息(保证BMU出现电压断线后不报高低压)*/
    if((DiagGetCellVOffCellBitAPI() > 0) && (eErr_A == DiagGetErrLastStateAPI(eERR_ID_SAMP_VOFF)))
    {
        return;
    }

    /*刚上电未完成一次采样或未接收全部单体信息不进行滤波*/
    if((sVSampFinFlag == 0) || (BSPGetOSMillSecTimeAPI() < 5000) || (2 != DiagGetRcvAllCellFlagAPI()))
    {
    	InputBsuCellVolt(ic_Num, num, volt);
    }
    else
    {
        if((BSPGetOSMillSecTimeAPI() > 30000)                /*系统已运行30s*/
            && (ABS(GetBsuCellVolt(ic_Num, num), volt) > 30))/*出现电压突变30mv*/
        {
            voltErrTime[ic_Num][num]++;                     /*异常次数*/
            if(voltErrTime[ic_Num][num] >= 3)               /*异常连续3次*/
            {
            	InputBsuCellVolt(ic_Num, num, volt);
                voltErrTime[ic_Num][num] = 0;
            }
        }
        else                                            	/*上电前期或电压正常*/
        {
            if(ABS(GetBsuCellVolt(ic_Num, num), volt) > 2)
            {
                voltage = ((u32)GetBsuCellVolt(ic_Num, num) * 20 + (u32)volt * 12) / 32;
            }
            else
            {
                voltage = volt;
            }

            InputBsuCellVolt(ic_Num, num, (u16)voltage);
            voltErrTime[ic_Num][num] = 0;
        }
    }
}

/*=============================================================================================
** @Name      : void WriteSampCellTempHook(u8 ic_Num, u8 num, s8 temp)
** @Input     : ic_Num:第几个从控[0,31] num:单体温度序号[0,3] temp:单节温度值 1℃
** @Output    : void
** @Function  : 输出单体电池温度采样值
** @The notes : 用户将输出值放到单体寄存器保存
**===========================================================================================*/
void WriteSampCellTempHook(u8 ic_Num, u8 num, s8 temp)
{
    s16 temperature = 0;

    if(num >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum])
    {
        return;
    }

	/*检测程序模式不滤波*/
	if(eMODE_DEBUG == GetGWorkModeAPI())
	{
		InputBsuCellTemp(ic_Num, num, temp);
		return;
	}

    /*出现采样电路异常但断线故障未反馈到BMU则不更新单体信息(保证BMU出现温度失效后不报高低温)*/
    if((DiagGetCellSampErrFlagAPI() > 0) && (eErr_A == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP)))
    {
        return;
    }

	/*出现温度断线但断线故障未反馈到BMU则不更新单体信息(保证BMU出现温度断线后不报高低温)*/
	if((DiagGetCellTOffCellBitAPI() > 0) && (eErr_A == DiagGetErrLastStateAPI(eERR_ID_SAMP_TOFF)))
	{
		return;
	}

	/*刚上电未完成一次采样或未接收全部单体信息不进行滤波*/
	if((sTSampFinFlag == 0) || (BSPGetOSMillSecTimeAPI() < 10000) || (2 != DiagGetRcvAllCellFlagAPI()))
	{
		InputBsuCellTemp(ic_Num, num, temp);
	}
	else
	{
		/*跳动超过2℃进行滤波*/
		if(ABS(GetBsuCellTemp(ic_Num, num), temp) > 2)
		{
			temperature = ((s16)GetBsuCellTemp(ic_Num, num) * 20 + (s16)temp * 12) / 32;
		}
		else
		{
			temperature = temp;
		}

		InputBsuCellTemp(ic_Num, num, (s8)temperature);
	}
}

/*=============================================================================================
** @Name      : void WriteSampBalanTempHook(u8 ic_Num, u8 num, s8 temp)
** @Input     : ic_Num:第几个从控[0,31] num:均衡板温序号[0,1] temp:均衡板温值 1℃
** @Output    : void
** @Function  : 输出均衡板温温度采样值
** @The notes : 用户将输出值放到单体寄存器保存
**===========================================================================================*/
void WriteSampBalanTempHook(u8 ic_Num, u8 num, s8 temp)
{
    s16 temperature = 0;

    if(num >= gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum])
    {
        return;
    }

	/*检测程序模式不滤波*/
	if(eMODE_DEBUG == GetGWorkModeAPI())
	{
		InputBsuBalanTemp(ic_Num, num, temp);
		return;
	}

	/*出现温度断线但断线故障未反馈到BMU则不更新单体信息(保证BMU出现温度断线后不报高低温)*/
	if((DiagGetBalTOffCellBitAPI() > 0) && (eErr_A == DiagGetErrLastStateAPI(eERR_ID_BAL_TEXP)))
	{
		//return;
	}

	/*刚上电未完成一次采样或未接收全部单体信息不进行滤波*/
	if((sTSampFinFlag == 0) || (BSPGetOSMillSecTimeAPI() < 10000) || (2 != DiagGetRcvAllCellFlagAPI()))
	{
		InputBsuBalanTemp(ic_Num, num, temp);
	}
	else
	{
		/*跳动超过2℃进行滤波*/
		if(ABS(GetBsuBalanTemp(ic_Num, num), temp) > 2)
		{
			temperature = ((s16)GetBsuBalanTemp(ic_Num, num) * 20 + (s16)temp * 12) / 32;
		}
		else
		{
			temperature = temp;
		}

		InputBsuBalanTemp(ic_Num, num, (s8)temperature);
	}
}

/*=============================================================================================
** @Name      : static void ClrSampCellVoltHook(u8 ic_Num)
** @Input     : ic_Num:第几个从控[0,31]
** @Output    : void
** @Function  : 清除指定从控AFE所有单体电压采样值为0
** @The notes : AFE单体电压采样失效或CAN通信中断超时时调用
**===========================================================================================*/
void ClrSampCellVoltHook(u8 ic_Num)
{
    u8 i = 0;

    /*所有电池单体电压*/
    for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]; i++)
    {
        /*输出单体电压值为0*/
        //WriteSampCellVoltHook(ic_Num, i, 0);
        /*不滤波不平滑*/
        InputBsuCellVolt(ic_Num, i, 0);
    }
}

/*=============================================================================================
** @Name      : void ClrSampCellTempHook(u8 ic_Num)
** @Input     : ic_Num:第几个从控[0,31]
** @Output    : void
** @Function  : 清除指定从控AFE所有单体温度和均衡板温温度采样值为0
** @The notes : AFE单体温度采样失效或CAN通信中断超时时调用
**===========================================================================================*/
void ClrSampCellTempHook(u8 ic_Num)
{
    u8 i = 0;

    /*所有电池单体温度*/
    for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]; i++)
    {
        /*输出单体温度值为-41*/
        //WriteSampCellTempHook(ic_Num, i, -41);
        /*不滤波不平滑*/
        InputBsuCellTemp(ic_Num, i, -41);
    }

    /*所有均衡板温温度*/
    for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum]; i++)
    {
        /*输出板温值为-41*/
        //WriteSampBalanTempHook(ic_Num, i, -41);
        /*不滤波不平滑*/
        InputBsuBalanTemp(ic_Num, i, -41);
    }
}

/*=============================================================================================
** @Name      : void ClrSampCellResisHook(u8 ic_Num)
** @Input     : ic_Num:第几个从控[0,31]
** @Output    : void
** @Function  : 清除指定从控AFE所有单体内阻采样值为0
** @The notes : CAN通信中断超时时调用
**===========================================================================================*/
void ClrSampCellResisHook(u8 ic_Num)
{
    //使能自研从控BSU CAN通信信息
    #if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
    u8 i = 0;

    /*所有电池单体内阻*/
    for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]; i++)
    {
        /*输出单体内阻值为0,不滤波不平滑*/
        InputBsuCellResis(ic_Num, i, 0);
    }
    #endif
}

/*=============================================================================================
** @Name      : void WriteCellVSampFinFlagHook(u8 flag)
** @Input     : flag:完成标志 0未完成 1完成
** @Output    : void
** @Function  : 输出单节电压采样第一遍完成标志
** @The notes : 菊花链或CAN通信接收完成
**===========================================================================================*/
void WriteCellVSampFinFlagHook(u8 flag)
{
    if(flag > 1)
    {
        return;
    }

    /*出现采样电路异常但断线故障未反馈到BMU则不更新单体信息(保证BMU出现电压失效后不报高低压)*/
    if((DiagGetCellSampErrFlagAPI() > 0) && (eErr_A == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP)))
    {
        return;
    }

    /*出现电压断线但断线故障未反馈到BMU则不更新单体信息(保证BMU出现电压断线后不报高低压)*/
    if((DiagGetCellVOffCellBitAPI() > 0) && (eErr_A == DiagGetErrLastStateAPI(eERR_ID_SAMP_VOFF)))
    {
        return;
    }

    sVSampFinFlag = flag;
}

/*=============================================================================================
** @Name      : void WriteCellTSampFinFlagHook(u8 flag)
** @Input     : flag:完成标志 0未完成 1完成
** @Output    : void
** @Function  : 输出单节温度采样第一遍完成标志
** @The notes : 菊花链或CAN通信接收完成
**===========================================================================================*/
void WriteCellTSampFinFlagHook(u8 flag)
{
    if(flag > 1)
    {
        return;
    }

    /*出现温度断线但断线故障未反馈到BMU则不更新单体信息(保证BMU出现温度断线后不报高低温)*/
    if((DiagGetCellTOffCellBitAPI() > 0) && (eErr_A == DiagGetErrLastStateAPI(eERR_ID_SAMP_TOFF)))
    {
        return;
    }

    sTSampFinFlag = flag;
}

/*=============================================================================================
** @Name      : void WriteCellRSampFinFlagHook(u8 flag)
** @Input     : flag:完成标志 0未完成 1完成
** @Output    : void
** @Function  : 输出单体内阻采样第一遍完成标志
** @The notes : CAN通信接收完成
**===========================================================================================*/
void WriteCellRSampFinFlagHook(u8 flag)
{
    if(flag > 1)
    {
        return;
    }

    sRSampFinFlag = flag;
}

/*=============================================================================================
** @Name      : u8 GetCellVSampFinFlagAPI(void)
** @Input     : void
** @Output    : 完成标志 0未完成 1完成
** @Function  : 获取单节电压采样第一遍完成标志
** @The notes : 菊花链或CAN通信接收完成
**===========================================================================================*/
u8 GetCellVSampFinFlagAPI(void)
{
    return(sVSampFinFlag);
}

/*=============================================================================================
** @Name      : u8 GetCellTSampFinFlagAPI(void)
** @Input     : void
** @Output    : 完成标志 0未完成 1完成
** @Function  : 获取单节温度采样第一遍完成标志
** @The notes : 菊花链或CAN通信接收完成
**===========================================================================================*/
u8 GetCellTSampFinFlagAPI(void)
{
    return(sTSampFinFlag);
}

/*=============================================================================================
** @Name      : u8 GetCellRSampFinFlagAPI(void)
** @Input     : void
** @Output    : 完成标志 0未完成 1完成
** @Function  : 获取单体内阻采样第一遍完成标志
** @The notes : CAN通信接收完成
**===========================================================================================*/
u8 GetCellRSampFinFlagAPI(void)
{
    return(sRSampFinFlag);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*菊花链单体采样方式*/
#if(0 == BMU_BSU_COM_TYPE)
/*=============================================================================================
** @Name      : static u32 ChangeMcuTempVoltToR(u16 Vdata)
** @Input     : ADdata:温度采样电压值
** @Output    : 温度采样NTC阻值 1Ω
** @Function  : 将温度电压值转换为NTC阻值
** @The notes :
**===========================================================================================*/
static u32 ChangeMcuTempVoltToR(u16 Vdata)
{
    u32 tempR = 0;

	/*ADI芯片*/
	#if(1 == AFE_TYPE_CHOICE)
    /*3.0V(实测2.9v)*/
    if(Vdata < 2980)
    {
        /*10K电阻分压 V=3.0*(R/(R+100000))*/
        tempR = ((u32)Vdata * 10000) / (3000 - Vdata);//按公式算

        /*100K电阻分压 V=3.0*(R/(R+100000))
        tempR = ((u32)Vdata * 100000) / (3000 - Vdata);//按公式算*/
    }
    /*采样电压超量程(当做最大电压)*/
    else
    {
        tempR = 0xffffffff;
    }

	/*BYD芯片*/
	#else
    /*3.3V(实测3.29v)*/
    if(Vdata < sBydTempStandV)
    {
        /*10K电阻分压 V=3.3*(R/(R+100000))*/
        //tempR = ((u32)Vdata * 10000) / (3290 - Vdata);//按公式算
        /*根据采集的温度上拉电压进行计算*/
		if(sBydTempStandV >= 2500)
		{
			tempR = ((u32)Vdata * 10000) / (sBydTempStandV - Vdata);
		}
		else
		{
			tempR = ((u32)Vdata * 10000) / (3290 - Vdata);
		}

        /*100K电阻分压 V=3.3*(R/(R+100000))
        tempR = ((u32)Vdata * 100000) / (3290 - Vdata);//按公式算*/
    }
    /*采样电压超量程(当做最大电压)*/
    else
    {
        tempR = 0xffffffff;
    }
	#endif
    
    return(tempR);
}

/*=============================================================================================
** @Name      : static u8 SearchCellTempTabByR(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 应温度下标值
** @Function  : 通过NTC阻值查询单体温度表得到对应温度值
** @The notes :
**===========================================================================================*/
static u8 SearchCellTempTabByR(u32 data)
{
    u8 i = 0;

	/*断线阻值很大(-41℃)*/
    if(data >= sNTCTempTabR[0])
    {
    	return(0);
    }
	/*非断线(-40℃以上)*/
    else
    {
    	/*从-40℃开始检测*/
		for(i = 1; i < CEL_T_TAB_LEN; i++)
		{
			/*达到当前阻值(阻值表降序)*/
			if(data >= sNTCTempTabR[i])
			{
				/*判断与前一个值的中间值*/
				if(data <= ((sNTCTempTabR[i] + sNTCTempTabR[i - 1]) / 2))
				{
					/*没有超过中间值取当前值*/
					return(i);
				}
				/*超过中间值说明偏大取前一个更大阻值*/
				else
				{
					return(i - 1);
				}
			}
		}
	}

	/*阻值很小低于表中阻值*/
	return(CEL_T_TAB_LEN);
}

/*=============================================================================================
** @Name      : static s16 ChangeBalADtoTemp(u16 ADdata)
** @Input     : ADdata:均衡温度采样AD值
** @Output    : 均衡温度值 ℃
** @Function  : 将均衡温度AD值转换为实际温度值
** @The notes : AFE采到的AD值已经是电压值
**===========================================================================================*/
//static s8 ChangeBalADtoTemp(u16 ADdata)
//{
//    s8 temp = 0;
//
//    //3.0V(实测3.0v),16位采样 V=AD*3.0/65535
//    //temp = (u16)(((u32)ADdata * 3000) / 65535);//按公式算
//
//    temp = (((s16)ADdata - 500) + 5) / 10;/*电压对应温度关系：Y = (X - 500) / 10 */
//
//    /*防止越界*/
//    if(temp > 127)
//    {
//        temp = 127;
//    }
//
//    return(temp);
//}

/*=============================================================================================
** @Name      : static u32 ChangeBalTempVoltToR(u8 channel, u16 Vdata)
** @Input     : channel:均衡温度采样GPIO通道[0, 1] Vdata:均衡温度采样电压值 1mV
** @Output    : 均衡温度值 ℃
** @Function  : 将均衡温度AD值转换为实际温度值
** @The notes : AFE采到的AD值已经是电压值
**===========================================================================================*/
static u32 ChangeBalTempVoltToR(u8 channel, u16 Vdata)
{
    u32 temp = 0;

	/*ADI芯片*/
	#if(1 == AFE_TYPE_CHOICE)
    s32 volt = 0;

    //47K电压对应阻值关系：Y=47*X/(3.0-X)
	if(Vdata >= 3000)
	{
		temp = 0xffffffff;
	}
	else
	{
		//电压修正
		//GPIO1
		if(0 == channel)
		{
			//-40~0℃(-5mv误差)
			if(Vdata > 2246)
			{
				volt = (1089 * (s32)Vdata - 134200) / 1000;
			}
			//1~25℃
			else if(Vdata > 1465)
			{
				volt = (1067 * (s32)Vdata - 77100) / 1000;
			}
			//26℃以上
			else
			{
				volt = (1012 * (s32)Vdata - 1500) / 1000;
			}
		}
		//GPIO2
		else
		{
			//-40~25℃(-5mv误差)
			if(Vdata > 1495)
			{
				volt = (1009 * (s32)Vdata - 10200) / 1000;
			}
			//26℃以上
			else
			{
				volt = (1005 * (s32)Vdata - 1000) / 1000;
			}
		}

		//电压不能超过基准3.0V
		if(volt >= 3000)
		{
			Vdata = 2999;
		}
		//电压不能是负值
		else if(Vdata < 0)
		{
			Vdata = 0;
		}
		else
		{
			Vdata = volt;
		}

		//电压转换成温感阻值(47K温感)
		temp = ((u32)Vdata * 47000) / (3000 - Vdata);
	}

	/*BYD芯片*/
	#else
    //47K电压对应阻值关系：Y=47*X/(3.3-X)
	//if(Vdata >= 3300)
	if(Vdata >= sBydTempStandV)
	{
		temp = 0xffffffff;
	}
	else
	{
//		//电压修正
//		//GPIO8
//		if(7 == channel)
//		{
//			//-40~0℃(-5mv误差)
//			if(Vdata > 2246)
//			{
//				volt = (1089 * (s32)Vdata - 134200) / 1000;
//			}
//			//1~25℃
//			else if(Vdata > 1465)
//			{
//				volt = (1067 * (s32)Vdata - 77100) / 1000;
//			}
//			//26℃以上
//			else
//			{
//				volt = (1012 * (s32)Vdata - 1500) / 1000;
//			}
//		}
//		//GPIO9
//		else
//		{
//			//-40~25℃(-5mv误差)
//			if(Vdata > 1495)
//			{
//				volt = (1009 * (s32)Vdata - 10200) / 1000;
//			}
//			//26℃以上
//			else
//			{
//				volt = (1005 * (s32)Vdata - 1000) / 1000;
//			}
//		}
//
//		//电压不能超过基准3.3V
//		if(volt >= 3300)
//		{
//			Vdata = 3299;
//		}
//		//电压不能是负值
//		else if(Vdata < 0)
//		{
//			Vdata = 0;
//		}
//		else
//		{
//			Vdata = volt;
//		}

		//电压转换成温感阻值(47K温感)
		//temp = ((u32)Vdata * 47000) / (3300 - Vdata);
		if(sBydTempStandV >= 2500)
		{
			temp = ((u32)Vdata * 47000) / (sBydTempStandV - Vdata);
		}
		else
		{
			temp = ((u32)Vdata * 47000) / (3300 - Vdata);
		}
	}
	#endif

    return(temp);
}

/*=============================================================================================
** @Name      : static u8 SearchBalTempTabByR(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 应温度下标值
** @Function  : 通过NTC阻值查询均衡板温温度表得到对应温度值
** @The notes :
**===========================================================================================*/
static u8 SearchBalTempTabByR(u32 data)
{
    u8 i = 0;

	/*断线阻值很大(-41℃)*/
    if(data >= sNTCBalTTabR[0])
    {
    	return(0);
    }
	/*非断线(-40℃以上)*/
    else
    {
    	/*从-40℃开始检测*/
		for(i = 1; i < BAL_T_TAB_LEN; i++)
		{
			/*达到当前阻值(阻值表降序)*/
			if(data >= sNTCBalTTabR[i])
			{
				/*判断与前一个值的中间值*/
				if(data <= ((sNTCBalTTabR[i] + sNTCBalTTabR[i - 1]) / 2))
				{
					/*没有超过中间值取当前值*/
					return(i);
				}
				/*超过中间值说明偏大取前一个更大阻值*/
				else
				{
					return(i - 1);
				}
			}
		}
    }

	/*阻值很小低于表中阻值*/
    return(BAL_T_TAB_LEN);
}

/*=============================================================================================
** @Name      : static u8 CorrectBalTempByKB(u8 channel, u8 temp)
** @Input     : channel:均衡温度采样GPIO通道[0, 1] temp:查表均衡温度[0, 166] 1℃
** @Output    : 修正后的均衡温度值
** @Function  : 通过KB值修正均衡板温温度值
** @The notes : KB值由实际测定
**===========================================================================================*/
static u8 CorrectBalTempByKB(u8 channel, u8 temp)
{
	s32 corrTemp = 0;

	/*ADI芯片*/
	#if(1 == AFE_TYPE_CHOICE)
	//GPIO1
	if(0 == channel)
	{
		//13℃及以上
		if(temp >= 54)
		{
			corrTemp = (9569 * (s32)temp - 1639) / 10000;
		}
		//-40~12℃
		else
		{
			corrTemp = temp;
		}
	}
	//GPIO2
	else
	{
		//10℃及以上
		if(temp >= 51)
		{
			corrTemp = (9529 * (s32)temp - 12747) / 10000;
		}
		//-40~9℃
		else
		{
			corrTemp = (10168 * (s32)temp - 17877) / 10000;
		}
	}

	/*BYD芯片*/
	#else
	//GPIO8
	if(7 == channel)
	{
		corrTemp = temp;
	}
	//GPIO9
	else
	{
		corrTemp = temp;
	}
	#endif

	//不能是负值
	if(corrTemp < 0)
	{
		corrTemp = 0;
	}
	//不能超过最大值
	else if(corrTemp > 167)
	{
		corrTemp = 167;
	}

	return((u8)corrTemp);
}

/*=============================================================================================
** @Name      : static void UpdateCellBalanReqInfo(void)
** @Input     : void
** @Output    : void
** @Function  : 更新单体电压均衡需求信息
** @The notes :
**===========================================================================================*/
static void UpdateCellBalanReqInfo(void)
{
	u8 i = 0;
	u8 flag = 0;

	/*采样失效和内部CAN断线故障*/
	if((eMODE_DEBUG != GetGWorkModeAPI())
		&& ((eErr_B == DiagGetErrFilStateAPI(eERR_ID_SAMP_EXP))
		|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_SAMP_VOFF))
		|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_CELL_MISS))
		/*|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF))*/
		|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_COFF))))
	{
		/*关闭所有均衡*/
		for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
		{
			sLTC6813PasBalanCmd[i] = 0;
		}
		/*无均衡请求*/
		sLTC6813PasBalanReqState = 0;
	}
	/*从控和单体电压正常*/
	else
	{
		/*检测均衡需求*/
		for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
		{
			/*取电池组均衡策略检测的被动均衡需求*/
			//sLTC6813PasBalanCmd[i] = GetGroupVoltPasBalanReqState(i);

			//兼容CATL和标准PACK电池14[7+7]串(中间留空1串)解析
			#if(3 == PRJ_PACK_CELL_INFO)
			{
				/*取电池组均衡策略检测的被动均衡需求*/
				sLTC6813PasBalanCmd[i] = GetGroupVoltPasBalanReqState(i) & 0x007f;
				sLTC6813PasBalanCmd[i] |= (GetGroupVoltPasBalanReqState(i) & 0x3f80) << 1;
			}
			//兼容标准PACK电池16[8+8]串(中间留空1串)解析
			#elif(2 == PRJ_PACK_CELL_INFO)
			{
				/*取电池组均衡策略检测的被动均衡需求*/
				sLTC6813PasBalanCmd[i] = GetGroupVoltPasBalanReqState(i) & 0x000ff;
				sLTC6813PasBalanCmd[i] |= (GetGroupVoltPasBalanReqState(i) & 0x0ff00) << 1;
			}
			//有效电压通道连续(中间不留空)解析
			#else
			{
				/*取电池组均衡策略检测的被动均衡需求*/
				sLTC6813PasBalanCmd[i] = GetGroupVoltPasBalanReqState(i);
			}
			#endif

			if(sLTC6813PasBalanCmd[i] > 0)
			{
				/*设置有均衡需求*/
				flag = 1;
			}
		}

		/*记录有无均衡需求*/
		sLTC6813PasBalanReqState = flag;
	}
}

/*=============================================================================================
** @Name      : static u8 CheckNeedStopBalanSamp(void)
** @Input     : 是否需要关闭均衡采样:TRUE:需要 FALSE:不需要
** @Output    : void
** @Function  : 检测是否需要关闭均衡采样
** @The notes :
**===========================================================================================*/
static u8 CheckNeedStopBalanSamp(void)
{
	static u32 sBalanTime = 0;

	/*需要关闭均衡而当前开启均衡*/
	if((0 == sLTC6813PasBalanReqState) && (sLTC6813PasBalanCtrlState > 0))
	{
		/*记录本次均衡采样时间*/
		sBalanTime = BSPGetOSMillSecTimeAPI();

		/*需要关闭均衡采样*/
		return(TRUE);
	}
	/*调试模式不需要关均衡*/
	else if(eMODE_DEBUG == GetGWorkModeAPI())
	{
		/*不需关闭均衡采样*/
		return(FALSE);
	}
	/*达到均衡采样周期*/
	else if(BSPGetOSMillSecTimeAPI() >= (sBalanTime + ((u32)gGHardPara_104[eGHardPara104_BalSamT] * 100)))
	{
		/*记录本次均衡采样时间*/
		sBalanTime = BSPGetOSMillSecTimeAPI();

		/*需要关闭均衡采样*/
		return(TRUE);
	}
	/*电压偏高(超过平台区)达到一半均衡采样周期*/
	else if((GetGCellMaxVoltAPI() >= gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV])
		&& (BSPGetOSMillSecTimeAPI() >= (sBalanTime + ((u32)gGHardPara_104[eGHardPara104_BalSamT] * 50))))
	{
		/*记录本次均衡采样时间*/
		sBalanTime = BSPGetOSMillSecTimeAPI();

		/*需要关闭均衡采样*/
		return(TRUE);
	}
	/*电压很高(接近告警)*/
	else if(GetGCellMaxVoltAPI() >= (gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV] - 30))
	{
		/*记录本次均衡采样时间*/
		sBalanTime = BSPGetOSMillSecTimeAPI();

		/*需要关闭均衡采样*/
		return(TRUE);
	}

	return(FALSE);
}

/*=============================================================================================
** @Name      : static void ClrCellPasBalanCtrlCmd(void)
** @Input     : void
** @Output    : void
** @Function  : 清除单体电压被动均衡控制命令
** @The notes : 需要强制关闭所有被动均衡发送命令前调用
**===========================================================================================*/
static void ClrCellPasBalanCtrlCmd(void)
{
	u8 i = 0;

	/*关闭所有均衡*/
	for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
	{
		sLTC6813PasBalanCmd[i] = 0;
	}
}

/*=============================================================================================
** @Name      : static void AFEPasEqualWrite(u32* commande, u8 total_ic)
** @Input     : commande:均衡命令   total_ic:从件个数
** @Output    : void
** @Function  : AFE的被动均衡寄存器设置执行函数
** @The notes : 6813支持18S均衡,BF8915A支持16S均衡
**===========================================================================================*/
static void AFEPasEqualWrite(u32* commande, u8 total_ic)
{
	u8 cell_ic = 0;
	#if(1 == AFE_TYPE_CHOICE)
	u8 data[MAX_LTC6813_NUM][6] = {0};
	#else
	static u8 sFlag = 0;
	ConfigB_TypeDef ConfigBRegWrite[TOTAL_IC];
	#endif

	#if(1 == AFE_TYPE_CHOICE)
	/*配置寄存器A*/
	for(cell_ic = 0; cell_ic < total_ic; cell_ic++)
	{
		data[cell_ic][0] = sLTC6813CfgData[0];
		data[cell_ic][1] = sLTC6813CfgData[1];
		data[cell_ic][2] = sLTC6813CfgData[2];
		data[cell_ic][3] = sLTC6813CfgData[3];
		data[cell_ic][4] = (u8)(commande[cell_ic] & 0x00ff);		/* 8S - 1S*/
		data[cell_ic][5] = (u8)((commande[cell_ic] & 0x0f00) >> 8);	/* 12S - 9S*/
	}
	LTC6813_DaisyWriteCfgA(total_ic, data);

	/*配置寄存器B*/
	for(cell_ic = 0; cell_ic < total_ic; cell_ic++)
	{
		//data[cell_ic][0] = (u8)(((commande[cell_ic] & 0xf000) >> 12) << 4) | 0x0F;	/* 16S - 13S 和 单体温度对应GPIO采样通道*/
		data[cell_ic][0] = (u8)((commande[cell_ic] & 0xf000) >> 8) | LTC_GPIO_CT_AD_BIT;/* 16S - 13S 和 单体温度对应GPIO采样通道*/
		data[cell_ic][1] = (u8)(commande[cell_ic] >> 16) & 0x03;						/* 18S - 17S*/
		data[cell_ic][2] = 0x00;
		data[cell_ic][3] = 0x00;
		data[cell_ic][4] = 0x00;
		data[cell_ic][5] = 0x00;
	}
	LTC6813_DaisyWriteCfgB(total_ic, data);
	
	#else
	wakeup_idle(total_ic);//保证AFE处于唤醒状态
	for(cell_ic = 0; cell_ic < total_ic; cell_ic++)
	{
		ConfigBRegWrite[cell_ic].dcc = (u16)(commande[cell_ic] & 0xffff);//放电电池开关1:打开Cell x 的开关  0:关闭Cell x 的开关 (默认)
		ConfigBRegWrite[cell_ic].bl_duty_sel = 0;			//主动均衡占空比选择  占空比=(BL_DUTY_SEL+1)*10%，配置范围10%~100%，当大于9时是100%。
		ConfigBRegWrite[cell_ic].dcto = 0;					//放电定时溢出值配置
		ConfigBRegWrite[cell_ic].vuv_bl_th = 0;				//均衡欠压阈值
		ConfigBRegWrite[cell_ic].dtmen = 0;					//在自动均衡下，如果DTMEN=1，当欠压(小于VUV_BL)关闭放电开关，如果DTMEN=0，不进行欠压比较
		ConfigBRegWrite[cell_ic].rx_pec_match = 0; 			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
	}
	BF8915A_wrcfgb(total_ic, ConfigBRegWrite);

	/*控制均衡总开关(奇数和偶数节不能同时开启)*/
	wakeup_idle(total_ic);//保证AFE处于唤醒状态

	//只有1个Pack时为测试程序只开奇数或偶数节
	if((1 == gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (1 == total_ic))
	{
		//关闭全部均衡
		if(0 == commande[0])
		{
			if(1 == sFlag)
			{
				BF8915A_strbl(CLKSEL_2M, OSR_256, OESEL_ODD, BLEN_OFF);
			}
			else
			{
				BF8915A_strbl(CLKSEL_2M, OSR_256, OESEL_EVEN, BLEN_OFF);
			}
		}
		//开启奇数节
		else if((commande[0] & 0x5555) > 0)
		{
			BF8915A_strbl(CLKSEL_2M, OSR_256, OESEL_ODD, BLEN_ON);
			sFlag = 1;
		}
		//开启偶数节
		else
		{
			BF8915A_strbl(CLKSEL_2M, OSR_256, OESEL_EVEN, BLEN_ON);
			sFlag = 0;
		}
	}
	else
	{
		//需要关闭全部均衡
		if(0 == sLTC6813PasBalanReqState)
		{
			if(1 == sFlag)
			{
				BF8915A_strbl(CLKSEL_2M, OSR_256, OESEL_ODD, BLEN_OFF);
			}
			else
			{
				BF8915A_strbl(CLKSEL_2M, OSR_256, OESEL_EVEN, BLEN_OFF);
			}
		}
		//开启奇数节
		else if(0 == sFlag)
		{
			BF8915A_strbl(CLKSEL_2M, OSR_256, OESEL_ODD, BLEN_ON);
			sFlag = 1;
		}
		//开启偶数节
		else
		{
			BF8915A_strbl(CLKSEL_2M, OSR_256, OESEL_EVEN, BLEN_ON);
			sFlag = 0;
		}
	}
	#endif
}
#endif
