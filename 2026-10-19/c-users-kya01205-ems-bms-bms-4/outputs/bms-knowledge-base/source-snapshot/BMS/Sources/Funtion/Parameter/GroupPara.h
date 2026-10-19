/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : GroupPara.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组参数存储模块
** @Instructions : 
**===============================================================================================*/
#ifndef _GROUPPARA_H
#define _GROUPPARA_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define GINDEX102_LEN			(eBmuGenPara102_CfgFin)		/*主控通用参数值数组长度*/
#define GINDEX103_LEN			(eBmuHigLevPara103_CfgFin)	/*主控高级参数值数组长度*/
#define GINDEX104_LEN			(eGHardPara104_Fin)			/*主控硬件参数值数组长度*/

#define GINDEX107_LEN			(eBmuGenPara102_CfgFin)		/*主控通用参数值数组长度*/
#define GINDEX108_LEN			(eBmuHigLevPara103_CfgFin)	/*主控高级参数值数组长度*/
#define GINDEX109_LEN			(eGHardPara104_Fin)			/*主控硬件参数值数组长度*/

/*********************************参数配置值范围**************************************************/
#define MIN_BMU_ADDR_CFG                (50)                /*主控设备地址最小配置值*/
#define MAX_BMU_ADDR_CFG                (65)        		/*主控设备地址最大配置值*/

#define MAX_CURR_CORR_10MA              (5000)              /*电流零飘修正最大值10mA*/

#define MAX_ALL_CAP_CFG                 (500000)            /*总容量最大配置值mAH*/
#define MAX_ALL_ENER_CFG                (500000)            /*总能量最大配置值WH*/

#define MIN_BSU_NUM_CFG                 (1)                 /*从控个数最小配置值*/
#define MAX_BSU_NUM_CFG                 (SLAVE_MAX_NUM)     /*从控个数最大配置值*/

#define MIN_BSU_CONNUM_CFG              (1)                 /*从机包含电池箱(从控)数最小配置值*/
#define MAX_BSU_CONNUM_CFG              (30)                /*从机包含电池箱(从控)数最大配置值*/
#define MIN_BSU_CELVNUM_CFG             (1)                 /*从控单体电压采样节数最小配置值*/
#define MAX_BSU_CELVNUM_CFG             (SLAVE_MAX_CELVNUM) /*从控单体电压采样节数最大配置值*/

#define MIN_BSU_CELTNUM_CFG             (1)                 /*从控单体温度采样节数最小配置值*/
#define MAX_BSU_CELTNUM_CFG             (SLAVE_MAX_CELTNUM) /*从控单体温度采样节数最大配置值*/
#define MIN_BSU_BALTNUM_CFG             (0)                 /*从控均衡板温采样节数最小配置值*/
#define MAX_BSU_BALTNUM_CFG             (SLAVE_MAX_BALTNUM) /*从控均衡板温采样节数最大配置值*/

#define MIN_BMU_CELVNUM_CFG             (1)                 /*主控单体电压采样节数最小配置值*/
#define MAX_BMU_CELVNUM_CFG             (GROUP_MAX_CELVNUM) /*主控单体电压采样节数最大配置值*/

#define MIN_BMU_CELTNUM_CFG             (1)                 /*主控单体温度采样总节数最小配置值*/
#define MAX_BMU_CELTNUM_CFG             (GROUP_MAX_CELTNUM) /*主控单体温度采样总节数最大配置值*/
#define MAX_BMU_BALTNUM_CFG             (GROUP_MAX_BALTNUM) /*主控均衡板温采样总节数最大配置值*/

#define MIN_VOLT_H_LIM                  (2000)              /*单体高压最小配置值*/
#define MAX_VOLT_H_LIM                  (5000)              /*单体高压最大配置值*/
#define MIN_VOLT_L_LIM                  (1000)              /*单体低压最小配置值*/
#define MAX_VOLT_L_LIM                  (4000)              /*单体低压最大配置值*/

#define MIN_SUMV_H_LIM                  (100)               /*总压高压最小配置值*/
#define MAX_SUMV_H_LIM                  (10000)             /*总压高压最大配置值*/
#define MIN_SUMV_L_LIM                  (100)               /*总压低压最小配置值*/
#define MAX_SUMV_L_LIM                  (10000)             /*总压低压最大配置值*/

#define MIN_CURR_H_LIM                  (10)                /*过流最小配置值*/
#define MAX_CURR_H_LIM                  (10000)             /*过流最大配置值*/

#define MIN_TEMP_H_LIM                  (20)                /*高温最小配置值*/
#define MAX_TEMP_H_LIM                  (80)                /*高温最大配置值*/
#define MIN_TEMP_L_LIM                  (-40)               /*低温最小配置值*/
#define MAX_TEMP_L_LIM                  (30)                /*低温最大配置值*/

#define MIN_PERT_H_LIM                  (10)                /*外围高温最小配置值*/
#define MAX_PERT_H_LIM                  (200)               /*外围高温最大配置值*/

#define MIN_POWER_H_LIM                 (1)                 /*请求功率最小配置值*/
#define MAX_POWER_H_LIM                 (5000)              /*请求功率最大配置值*/

#define MIN_ALLOW_CLIM_V                (2000)              /*释放充电电流限值电压最小配置值*/
#define MAX_ALLOW_CLIM_V                (5000)              /*释放充电电流限值电压最大配置值*/
#define MIN_ALLOW_DLIM_V                (1000)              /*释放放电电流限值电压最小配置值*/
#define MAX_ALLOW_DLIM_V                (4000)              /*释放放电电流限值电压最大配置值*/

#define MIN_CURR_OLIM_PER               (50)                /*电流超限告警比率最小配置值*/
#define MAX_CURR_OLIM_PER               (500)               /*电流超限告警比率最大配置值*/

#define MAX_ALM_CLR_SUB_CV              (1000)              /*单体电压告警释放回差取值最大配置值*/
#define MAX_ALM_CLR_SUB_SV              (1000)              /*总压电压告警释放回差取值最大配置值*/
#define MAX_ALM_CLR_SUB_C               (1000)              /*过流告警释放回差取值最大配置值*/
#define MAX_ALM_CLR_SUB_T               (60)                /*温度告警释放回差取值最大配置值*/

#define MAX_PRO_TIME_1S                 (6000)              /*故障保护时间最大配置值*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
/*协议102子索引列表*/
typedef enum
{
	eBmuGenPara102_CfgFlag = 0,								/*系统通用参数配置标志	23130*/
	eBmuGenPara102_BmuDev = 1,								/*设备地址[50,66]*/
	eBmuGenPara102_NowSoc = 2,								/*当前SOC	0.10%	[0,1000]*/
	eBmuGenPara102_NowSoe = 3,								/*当前SOE	0.10%	[0,1000]*/
	eBmuGenPara102_ReaAllCap = 4,							/*实际总容量0.1AH	[1,5000]*/
	eBmuGenPara102_ReaAllEner = 5,							/*实际总电能0.1kWh	[1,5000]*/
	eBmuGenPara102_RateCap = 6,								/*出厂额定容量0.1AH	[1,5000]*/
	eBmuGenPara102_DampNum = 7,								/*出厂衰减次数[1,15000]*/

	eBmuGenPara102_VH2Lim = 8,								/*单体高压二级告警限值[2000,5000]	*/
	eBmuGenPara102_VH1Lim = 9,								/*单体高压一级保护限值1mv [2000,5000]*/

	eBmuGenPara102_VL2Lim = 10,								/*单体低压二级告警限值1mv	 [1000,4000]*/
	eBmuGenPara102_VL1Lim = 11,								/*单体低压一级保护限值1mv	 [1000,4000]*/

	eBmuGenPara102_ChgTH2Lim = 12,							/*充电高温二级告警限值1℃	 [20,80]*/
	eBmuGenPara102_ChgTH1Lim = 13,							/*充电高温一级保护限值1℃	 [20,80]*/
	eBmuGenPara102_DhgTH2Lim = 14,							/*放电高温二级告警限值1℃	 [20,80]*/
	eBmuGenPara102_DhgTH1Lim = 15,							/*放电高温一级保护限值1℃	 [20,80]*/
	eBmuGenPara102_ChgTL2Lim = 16,							/*充电低温二级告警限值1℃	 [-40,30]*/
	eBmuGenPara102_ChgTL1Lim = 17,							/*充电低温一级保护限值1℃	 [-40,30]*/
	eBmuGenPara102_DhgTL2Lim = 18,							/*放电低温二级告警限值1℃	 [-40,30]*/
	eBmuGenPara102_DhgTL1Lim = 19,							/*放电低温一级保护限值1℃	 [-40,30]*/

	eBmuGenPara102_SumVH2Lim = 20,							/*总压高压二级告警限值0.1V [100,10000]*/
	eBmuGenPara102_SumVH1Lim = 21,							/*总压高压一级保护限值0.1V [100,10000]*/
	eBmuGenPara102_SumVL2Lim = 22,							/*总压低压二级告警限值0.1V [100,10000]*/
	eBmuGenPara102_SumVL1Lim = 23,							/*总压低压一级保护限值0.1V [100,10000]*/

	eBmuGenPara102_ChgCH2Lim = 24,							/*充电过流二级告警限值0.1A [10,10000]*/
	eBmuGenPara102_ChgCH1Lim = 25,							/*充电过流一级保护限值0.1A [10,10000]*/
	eBmuGenPara102_DhgCH2Lim = 26,							/*放电过流二级告警限值0.1A [10,10000]*/
	eBmuGenPara102_DhgCH1Lim = 27,							/*放电过流一级保护限值0.1A [10,10000]*/

	eBmuGenPara102_ChgCMaxLim = 28,							/*充电最大请求电流值0.1A	[10,10000]*/
	eBmuGenPara102_DhgCMaxLim = 29,							/*放电最大请求电流值0.1A	[10,10000]*/
	eBmuGenPara102_ChgCFinLim = 30,							/*充电截止电流值0.1A	[10,10000]*/
	eBmuGenPara102_DhgCFinLim = 31,							/*放电截止电流值0.1A	[10,10000]*/
	eBmuGenPara102_ChgPMaxLim = 32,							/*充电最大请求功率值0.1kW	 [1,5000*/
	eBmuGenPara102_DhgPMaxLim = 33,							/*放电最大请求功率值0.1kW	 [1,5000]*/
	eBmuGenPara102_ChgPRatLim = 34,							/*充电输入额定功率值0.1kW	 [1,5000]*/
	eBmuGenPara102_DhgPRatLim = 35,							/*放电输出额定功率值0.1kW	 [1,5000]*/

	eBmuGenPara102_Leak2Lim = 36,							/*绝缘二级告警限值1Ω/V	[1,1000]*/
	eBmuGenPara102_Leak1Lim = 37,							/*绝缘一级保护限值1Ω/V	[1,1000]*/

	eBmuGenPara102_Envir2Lim = 38,							/*环境温度高温二级告警限值1℃ [10,200]*/
	eBmuGenPara102_Envir1Lim = 39,							/*环境温度高温一级保护限值1℃ [10,200]*/
	eBmuGenPara102_Perip2Lim = 40,							/*外设温度高温二级告警限值1℃ [10,200]*/
	eBmuGenPara102_Perip1Lim = 41,							/*外设温度高温一级保护限值1℃ [10,200]*/

	eBmuGenPara102_VDiff2SubV = 42,							/*单体压差过大二级限值1mv	 [20,5000]*/
	eBmuGenPara102_VDiff1SubV = 43,							/*单体压差过大一级限值1mv	 [20,5000]*/
	eBmuGenPara102_TDiff2SubT = 44,							/*单体温差过大二级限值1℃	 [3,60]*/
	eBmuGenPara102_TDiff1SubT = 45,							/*单体温差过大一级限值1℃	 [3,60]*/

	eBmuGenPara102_CapHig2Soc = 46,							/*电池充电容量高二级限值0.10% [500,1000]*/
	eBmuGenPara102_CapHig1Soc = 47,							/*电池充电容量高一级限值0.10%	 [500,1000]*/
	eBmuGenPara102_CapLow2Soc = 48,							/*电池剩余容量低二级限值0.10%	 [0,500]*/
	eBmuGenPara102_CapLow1Soc = 49,							/*电池剩余容量低一级限值0.10%	 [0,500]*/

	eBmuGenPara102_CapHig1Volt = 50,						/*电池容量高一级预警对应电压值1mv	[2000,5000]*/
	eBmuGenPara102_CapLow1Volt = 51,						/*电池容量低一级预警对应电压值1mv	[1000,4000]*/

	eBmuGenPara102_CVH2Clr = 52,							/*单体高压二级消除回差值1mv [1,1000]*/
	eBmuGenPara102_CVH1Clr = 53,							/*单体高压一级消除回差值1mv [1,1000]*/
	eBmuGenPara102_CVL2Clr = 54,							/*单体低压二级消除回差值1mv [1,1000]*/
	eBmuGenPara102_CVL1Clr = 55,							/*单体低压一级消除回差值1mv [1,1000]*/

	eBmuGenPara102_CTH2Clr = 56,							/*充电高温二级消除回差值1℃ [1,80]*/
	eBmuGenPara102_CTH1Clr = 57,							/*充电高温一级消除回差值1℃ [1,80]*/
	eBmuGenPara102_DTH2Clr = 58,							/*放电高温二级消除回差值1℃ [1,80]*/
	eBmuGenPara102_DTH1Clr = 59,							/*放电高温一级消除回差值1℃ [1,80]*/

	eBmuGenPara102_CTL2Clr = 60,							/*充电低温二级消除回差值1℃ [-40,30]*/
	eBmuGenPara102_CTL1Clr = 61,							/*充电低温一级消除回差值1℃ [-40,30]*/
	eBmuGenPara102_DTL2Clr = 62,							/*放电低温二级消除回差值1℃ [-40,30]*/
	eBmuGenPara102_DTL1Clr = 63,							/*放电低温一级消除回差值1℃ [-40,30]*/

	eBmuGenPara102_SVH2Clr = 64,							/*总压高压二级消除回差值0.1v [10,1000]*/
	eBmuGenPara102_SVH1Clr = 65,							/*总压高压一级消除回差值0.1v [10,1000]*/
	eBmuGenPara102_SVL2Clr = 66,							/*总压低压二级消除回差值0.1v [10,1000]*/
	eBmuGenPara102_SVL1Clr = 67,							/*总压低压一级消除回差值0.1v [10,1000]*/

	eBmuGenPara102_ChgCH2Clr = 68,							/*充电过流二级消除回差值0.1A [10,10000]*/
	eBmuGenPara102_ChgCH1Clr = 69,							/*充电过流一级消除回差值0.1A [10,10000]*/
	eBmuGenPara102_DhgCH2Clr = 70,							/*放电过流二级消除回差值0.1A [10,10000]*/
	eBmuGenPara102_DhgCH1Clr = 71,							/*放电过流一级消除回差值0.1A [10,10000]*/

	eBmuGenPara102_Envir2Clr = 72,							/*环境温度高温二级消除回差值1℃ [1,100]*/
	eBmuGenPara102_Envir1Clr = 73,							/*环境温度高温一级消除回差值1℃ [1,100]*/
	eBmuGenPara102_Perip2Clr = 74,							/*外设温度高温二级消除回差值1℃ [1,100]*/
	eBmuGenPara102_Perip1Clr = 75,							/*外设温度高温一级消除回差值1℃ [1,100]*/

	eBmuGenPara102_ChgLimSubSoc = 76,						/*释放充电限制SOC回差值0.10%	[1,500]*/
	eBmuGenPara102_DhgLimSubSoc = 77,						/*释放放电限制SOC回差值0.10%	[1,500]*/
	eBmuGenPara102_ChgLimClrV = 78,							/*强制允许充电最大电压值1mv [2000,5000]*/
	eBmuGenPara102_DhgLimClrV = 79,							/*强制允许放电最小电压值1mv [1000,4000*/
	eBmuGenPara102_CellPlaUpV = 80,							/*单体压差过大检测区间上限1mv	[2000,5000]*/
	eBmuGenPara102_CellPlaDnV = 81,							/*单体压差过大检测区间下限1mv	[1000,4000]*/

	eBmuGenPara102_CfgFin           						/*设备通用参数配置结束*/
}e_GGenPara102;

/*协议103子索引列表*/
typedef enum
{
	eBmuHigLevPara103_CfgFlag = 0,							/*设备高级参数配置标志*/
	eBmuHigLevPara103_BsuNum = 1,							/*单组管理电池箱(从控)数目 [1,30]*/
	eBmuHigLevPara103_ConNum = 2,							/*一个从机包含电池箱(从控)数目 [1,30]*/
	eBmuHigLevPara103_BsuVNum = 3,							/*一个从控电压采样节数 [1,16]*/
	eBmuHigLevPara103_BsuTNum = 4,							/*一个从控温度采样节数 [1,16]*/
	eBmuHigLevPara103_BalTNum = 5,							/*一个从控板温采样节数 [0,2]*/
	eBmuHigLevPara103_VSumNum = 6,							/*单体电压采样总节数 [1,400]*/
	eBmuHigLevPara103_TSumNum = 7,							/*单体温度采样总节数 [1,200]*/
	eBmuHigLevPara103_BTSumNum = 8,							/*均衡板温采样总节数 [0,64]*/

	eBmuHigLevPara103_ChgLimOffT = 9,						/*充电限制请求保护时间1s	[0,6000]*/
	eBmuHigLevPara103_DhgLimOffT = 10,						/*放电限制请求保护时间1s	[0,6000]*/
	eBmuHigLevPara103_ChgProOffT = 11,						/*充电保护延迟保护时间1s	[0,6000]*/
	eBmuHigLevPara103_DhgProOffT = 12,						/*放电保护延迟保护时间1s	[0,6000]*/
	eBmuHigLevPara103_ErrProOffT = 13,						/*系统故障延迟保护时间1s	[0,6000]*/
	eBmuHigLevPara103_CommProOffT = 14,						/*外部通讯故障延迟保护时间1s	[0,6000]*/
	eBmuHigLevPara103_VL2PwrOffT = 15,						/*低压二级延迟关机时间1s	[0,6000]*/
	eBmuHigLevPara103_VL1PwrOffT = 16,						/*欠压一级延迟关机时间1s	[0,6000]*/
	eBmuHigLevPara103_ErrPWrOffT = 17,						/*系统故障延迟关机时间1s	[0,6000]*/
	eBmuHigLevPara103_CommPwrOffT = 18,						/*外部通讯故障延迟关机时间1s	[0,6000]*/

	eBmuHigLevPara103_FanOpenTemp = 19,						/*散热风扇开启温度值1℃	[-20,60]*/
	eBmuHigLevPara103_FanCloseTemp = 20,					/*散热风扇关闭温度值1℃	[-20,60]*/
	eBmuHigLevPara103_HotOpenTemp = 21,						/*加热膜开启温度值1℃	[-40,40]*/
	eBmuHigLevPara103_HotCloseTemp = 22,					/*加热膜关闭温度值1℃	[-40,40]*/

	eBmuHigLevPara103_ChgFinCVEn = 23,						/*充电末端限流恒压使能状态[0,1]	0:禁止 1:使能*/
	eBmuHigLevPara103_DhgFinCVEn = 24,						/*放电末端限流恒压使能状态[0,1]	0:禁止 1:使能*/
	eBmuHigLevPara103_ChgTHLimEn = 25,						/*充电高温降功率使能状态[0,1] 	0:禁止 1:使能*/
	eBmuHigLevPara103_DhgTHLimEn = 26,						/*放电高温降功率使能状态[0,1]	 	0:禁止 1:使能*/
	eBmuHigLevPara103_ChgTLLimEn = 27,						/*充电低温降功率使能状态[0,1]		0:禁止 1:使能*/
	eBmuHigLevPara103_DhgTLLimEn = 28,						/*放电低温降功率使能状态[0,1]		0:禁止 1:使能*/
	eBmuHigLevPara103_TSloVLimEn = 29,						/*温度调节高低压限值使能状态[0,1]	0:禁止 1:使能*/

	eBmuHigLevPara103_PwrPor = 30,							/*开机按钮信号检测端口	按位表示，每bit表示一个IO*/
	eBmuHigLevPara103_IsoPort = 31,							/*断路器信号检测端口		按位表示，每bit表示一个IO口*/
	eBmuHigLevPara103_FusPort = 32,							/*熔断器信号检测端口		按位表示，每bit表示一个IO口*/
	eBmuHigLevPara103_SmogPort = 33,						/*烟感故障信号检测端口	按位表示，每bit表示一个IO口*/
	eBmuHigLevPara103_EmergPort = 34,						/*急停按钮信号检测端口	按位表示，每bit表示一个IO口*/
	eBmuHigLevPara103_ThundPort = 35,						/*防雷器故障信号检测端口	按位表示，每bit表示一个IO口*/
	eBmuHigLevPara103_FirePort = 36,						/*消防告警信号检测端口	按位表示，每bit表示一个IO口*/
	eBmuHigLevPara103_GasPort = 37,							/*气体浓度信号检测端口	按位表示，每bit表示一个IO口*/
	eBmuHigLevPara103_FanPort = 38,							/*风扇状态信号检测端口	按位表示，每bit表示一个IO口*/

	eBmuHigLevPara103_LeakMonitType = 39,					/*绝缘检测方式[0,8] 0:不检测 1:非运行检测不报故障 2:非运行检测上报故障 3:全程检测不报故障 4:全程检测非运行上报故障 5:全程检测全程上报故障 6:运行检测不上报 7:运行检测上报 8:请求时检测上报*/
	eBmuHigLevPara103_AdhMonitType = 40,					/*粘连检测方式 [0,3],0:不检测 1:Link电路 2:外端电压 3:Link电路和外端电压,默认1*/

	eBmuHigLevPara103_MainSwhC = 41,						/*主接触器安全电流值0.1A	[0,5000]*/
	eBmuHigLevPara103_PChgSwhC = 42,						/*预充接触器安全电流值0.1A	[0,500]*/
	eBmuHigLevPara103_PChgResis = 43,						/*预充电阻阻值1Ω	[1,1000]*/
	eBmuHigLevPara103_PChgPower = 44,						/*预充电阻额定功率值1W	[1,1000]*/
	eBmuHigLevPara103_SafeSubV = 45,						/*允许启动内外安全压差0.1V [0,10000]*/
	eBmuHigLevPara103_PChgFinV = 46,						/*预充完成判定内外压差0.1V [0,10000]*/

	eBmuHigLevPara103_PChgWaitT = 47,						/*电流至少预充时间0.1s	[0,600]*/
	eBmuHigLevPara103_PChgOffT = 48,						/*电流预充超时时间0.1s	[0,600]*/
	eBmuHigLevPara103_SelfWaitT = 49,						/*自启动前等待时间0.1s	[0,600]*/
	eBmuHigLevPara103_MSwhOffV = 50,						/*断开接触器外端残存电压值0.1V	[0,10000]*/
	eBmuHigLevPara103_HigVOffV = 51,						/*高压采样正常至少电压值0.1V	[0,1000]*/
	eBmuHigLevPara103_SumDiffV = 52,						/*总电压一致允许差值0.1V	[0,1000]*/

	eBmuHigLevPara103_LedkRlyState = 53,					/*绝缘检测后PE继电器状态 [0,1](0:断开 1:闭合)*/
	eBmuHigLevPara103_LeakMonitCycl = 54,					/*绝缘检测周期1s [1,64800]默认6H(21600s)*/
	eBmuHigLevPara103_LedkSampDelyT = 55,					/*绝缘采样(闭合继电器后)延迟时间1ms [10,10000](等待采样口稳定时间[默认1500ms])*/
	eBmuHigLevPara103_PgndVSampGapT = 56,					/*负母线电压采样(无绝缘检测时)周期1ms [10,60000](更新负母线电压时间[默认1000ms])*/
	eBmuHigLevPara103_AdhRlyState = 57,						/*粘连检测后Link继电器状态 [0,1](0:断开 1:闭合)*/
	eBmuHigLevPara103_AdhMonitCycl = 58,					/*粘连检测周期1s [1,64800]默认6H(21600s)*/
	eBmuHigLevPara103_Lk0SampDelyT = 59,					/*负载电压采样(闭合继电器后)延迟时间1ms [10,10000](等待采样口稳定时间[默认700ms])*/
	eBmuHigLevPara103_Lk1SampDelyT = 60,					/*Link+电压采样(闭合继电器后)延迟时间1ms [10,10000](等待采样口稳定时间[默认600ms])*/
	eBmuHigLevPara103_Lk2SampDelyT = 61,					/*Link-电压采样(闭合继电器后)延迟时间1ms [10,10000](等待采样口稳定时间[默认2000ms])*/
	eBmuHigLevPara103_RlyCtrlGapT = 62,						/*板上继电器切换至少间隔时间1ms [10,10000](防止继电器动作太快[默认500ms])*/

	eBmuHigLevPara103_ChgHTDnFstT = 63,						/*充电高温降功率起始温度1℃ [1,80]*/
	eBmuHigLevPara103_ChgHTDnDifT = 64,						/*充电高温降功率起始温差1℃ [1,80]*/
	eBmuHigLevPara103_ChgHTDnRate = 65,						/*充电高温降功率下降比率0.1%/℃ [1,500]*/
	eBmuHigLevPara103_ChgHTDnMinC = 66,						/*充电高温降功率最低电流0.1A [10,5000]*/

	eBmuHigLevPara103_DhgHTDnFstT = 67,						/*放电高温降功率起始温度1℃ [1,80]*/
	eBmuHigLevPara103_DhgHTDnDifT = 68,						/*放电高温降功率起始温差1℃ [1,80]*/
	eBmuHigLevPara103_DhgHTDnRate = 69,						/*放电高温降功率下降比率0.1%/℃ [1,500]*/
	eBmuHigLevPara103_DhgHTDnMinC = 70,						/*放电高温降功率最低电流0.1A [10,5000]*/

	eBmuHigLevPara103_CfgFin								/*主控高级参数配置结束*/
}e_BmuHigLevPara103;

/*协议104子索引列表*/
typedef enum
{
    eGHardPara104_CfgFlag = 0,          					/*硬件参数配置标志*/
    eGHardPara104_BalTact,             	 					/*均衡策略算法 [0禁止均衡 1压差积分均衡]*/
    eGHardPara104_BalCirc,              					/*均衡电路     [0被动放电均衡 1主动充电+主动放电均衡 2被动放电+主动充放电均衡 ]*/
    eGHardPara104_BalCycT,              					/*均衡检测周期     0.1Min*/
    eGHardPara104_BalSamT,              					/*均衡采样周期     0.1s*/
    eGHardPara104_BalChgV,              					/*充电均衡压差 1mv*/
    eGHardPara104_BalDhgV,              					/*放电均衡压差 1mv*/

	eGHardPara104_CSampChan,                                /*电流采集通道[0:外部高压模块,1霍尔,2分流器,3霍尔和分流器]*/
    eGHardPara104_ShuntRate,            					/*分流器单向电压范围[1,200]1mV*/
    eGHardPara104_ShuntRang,            					/*分流器单向电流量程[1,200]10A*/
    eGHardPara104_HallSign,            						/*霍尔信号零点电压[1,200]0.1V*/
    eGHardPara104_HallRate,            						/*霍尔单向电压范围[1,200]0.1V*/
    eGHardPara104_HallRang,            						/*霍尔单向电流量程[1,200]10A*/
    eGHardPara104_CFilType,             					/*电流滤波方式[0不滤波 1瞬时平均值 2瞬时有效值 3积分平均值]*/
    eGHardPara104_CFilTime,             					/*电流滤波次数[1,200]1次*/

    eGHardPara104_ChgIntPoint,          					/*充电积分零点电流值[1,100]10mA*/
    eGHardPara104_DhgIntPoint,          					/*放电积分零点电流值[1,100]10mA*/
    eGHardPara104_ChgShowPoint,         					/*充电显示零点电流值[1,100]0.1A*/
    eGHardPara104_DhgShowPoint,         					/*放电显示零点电流值[1,100]0.1A*/
    eGHardPara104_ChgIdlePoint,         					/*待机时充电零点电流值[1,100]0.1A*/
    eGHardPara104_DhgIdlePoint,         					/*待机时放电零点电流值[1,100]0.1A*/

    eGHardPara104_BmuSWVerLL,           					/*BMU软件版本号最低位(支线变更版本)*/
    eGHardPara104_BmuSWVerML,           					/*BMU软件版本号次低位(支线归档版本)*/
    eGHardPara104_BmuSWVerMH,           					/*BMU软件版本号次高位(主线变更版本)*/
    eGHardPara104_BmuSWVerHH,           					/*BMU软件版本号最高位(主线归档版本)*/

    eGHardPara104_AirColdOn_CT,           					/*空调开启制冷电池温度[10,60]1℃(默认34℃)*/
    eGHardPara104_AirColdOff_CT,           					/*空调停止制冷电池温度[10,60]1℃(默认30℃)*/
    eGHardPara104_AirHeatOn_CT,           					/*空调开启制热电池温度[-30,20]1℃(默认5℃)*/
    eGHardPara104_AirHeatOff_CT,           					/*空调停止制热电池温度[-30,20]1℃(默认10℃)*/

    eGHardPara104_AirColdOn_ET,           					/*空调开启制冷环境温度[16,48]1℃,(触发启动制冷温度,默认28℃)*/
    eGHardPara104_AirColdOff_ET,           					/*空调停止制冷环境温度[16,38]1℃,(制冷回差温度=启动温度-停止温度,默认23℃)*/
    eGHardPara104_AirHeatOn_ET,           					/*空调开启制热环境温度[5,26]1℃,(触发开启制热温度,默认8℃)*/
    eGHardPara104_AirHeatOff_ET,           					/*空调制热偏移环境温度[5,36]1℃,(制热回差温度=停止温度-启动温度,默认15℃)*/
    eGHardPara104_AirDeHumOn_T,           					/*空调开启除湿温度[20,40]1℃,(即除湿加热开启温度,建议低于制冷设定温度,默认20℃)*/
    eGHardPara104_AirDeHumOff_T,           					/*空调停止除湿温度[20,45]1℃,(即除湿加热停止温度,建议低于制冷设定温度,默认25℃)*/
    eGHardPara104_AirDeHumOn_H,           					/*空调开启除湿湿度[50,99]1%,(默认80%)*/
    eGHardPara104_AirDeHumOff_H,           					/*空调停止除湿湿度[45,95]1%,(默认70%)*/

    eGHardPara104_PackFanPwm0,           					/*1#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm1,           					/*2#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm2,           					/*3#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm3,           					/*4#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm4,           					/*5#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm5,           					/*6#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm6,           					/*7#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm7,           					/*8#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm8,           					/*9#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm9,           					/*10#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm10,           					/*11#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm11,           					/*12#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm12,           					/*13#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm13,           					/*14#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm14,           					/*15#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/
    eGHardPara104_PackFanPwm15,           					/*16#电池箱调速风扇对应PWM通道号(电池柜插框位号)[1,16](默认与电池箱序号对应)*/

    eGHardPara104_Fin                   					/*硬件参数配置结束*/
}e_GHardPara104;

//从控常规配置参数
typedef enum
{
	eGBsuPara102_CfgFlag = 0,           //常规参数配置标志
	eGBsuPara102_EcuId,                 //从控设备地址

	eGBsuPara102_SlaveNum,              //组内从控数目(用于从控根据主控地址偏移)
	eGBsuPara102_DaisyType,             //菊花链类型
	eGBsuPara102_DaisyNum,              //菊花链从件个数(PACK数目)

	eGBsuPara102_SampVnum,              //PACK电压采样节数
	eGBsuPara102_SampTnum,              //PACK温度采样节数

	eGBsuPara102_TotalVnum,             //电压采样总数
	eGBsuPara102_TotalTnum,             //温度采样总数

	eGBsuPara102_BChgVolt,              //充电均衡开启压差值 1mv
	eGBsuPara102_BDhgVolt,              //放电均衡开启压差值 1mv

    eGBsuPara102_Fin                    //从控参数102子索引数
}e_GBsuParaCfg102;                      //从控参数102

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ParaCheckGGenParaEffect(u16 addr, u16 para)
** @Input     : addr:参数地址 para:参数值
** @Output    : TRUE:有效合法 FALSE:非法无效
** @Function  : 检测电池组通用参数值是否有效
** @The notes :
**===============================================================================================*/
u8 ParaCheckGGenParaEffect(u16 addr, u16 para);

/*=================================================================================================
** @Name      : u8 ParaCheckGHigParaEffect(u32 addr, u16 para)
** @Input     : addr:参数地址 para:参数值
** @Output    : TRUE:有效合法 FALSE:非法无效
** @Function  : 检测电池组高级参数值是否有效
** @The notes :
**===============================================================================================*/
u8 ParaCheckGHigParaEffect(u16 addr, u16 para);

/*=================================================================================================
** @Name      : u8 ParaCheckGHWParaEffect(u16 addr, u8 para)
** @Input     : addr:参数地址 para:参数值 len:参数长度
** @Output    : TRUE:有效合法 FALSE:非法无效
** @Function  : 检测电池组硬件参数值是否有效
** @The notes : 用户必须补充完善
**===============================================================================================*/
u8 ParaCheckGHWParaEffect(u16 addr, u8 para);

#endif
