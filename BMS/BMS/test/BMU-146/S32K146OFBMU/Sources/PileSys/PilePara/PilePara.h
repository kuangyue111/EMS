/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PilePara.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控参数存储模块
** @Instructions : 
**===============================================================================================*/
#ifndef _PILEPARA_H
#define _PILEPARA_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define PINDEX102_LEN			(eSmuGenPara102_CfgFin)		/*堆控通用参数值数组长度*/

#define PINDEX107_LEN			(eSmuGenPara102_CfgFin)		/*堆控通用参数值数组长度*/

/*********************************参数配置值范围**************************************************/
#define MIN_SMU_ADDR_CFG                (70)                /*堆控设备地址最小配置值*/
#define MAX_SMU_ADDR_CFG                (79)        		/*堆控设备地址最大配置值*/

#define MIN_SMU_CELVNUM_CFG             (1)                 /*主控单体电压采样节数最小配置值*/
#define MAX_SMU_CELVNUM_CFG             (400)               /*主控单体电压采样节数最大配置值*/

#define MIN_SMU_CELTNUM_CFG             (1)                 /*主控单体温度采样总节数最小配置值*/
#define MAX_SMU_CELTNUM_CFG             (200)               /*主控单体温度采样总节数最大配置值*/

#define MAX_PRO_TIME_1S                 (6000)              /*故障保护时间最大配置值*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
/*协议102子索引列表*/
typedef enum
{
	eSmuGenPara102_CfgFlag = 0,								/*系统通用参数配置标志	23130*/
	eSmuGenPara102_SmuDev = 1,								/*堆控设备地址[50,66]*/
	eSmuGenPara102_VSumNum,                            		/*电压采样总节数[1,400]*/
	eSmuGenPara102_TSumNum,                             	/*温度采样总节数[1,400]*/

	eSmuGenPara102_ChgLimT,									/*充电限制请求保护时间1s	[0,6000]*/
	eSmuGenPara102_DhgLimT,									/*放电限制请求保护时间1s	[0,6000]*/
	eSmuGenPara102_ChgProT,									/*充电保护延迟保护时间1s	[0,6000]*/
	eSmuGenPara102_DhgProT,									/*放电保护延迟保护时间1s	[0,6000]*/
	eSmuGenPara102_ErrProT,									/*系统故障延迟保护时间1s	[0,6000]*/
	eSmuGenPara102_ComProT,									/*外部通讯故障延迟保护时间1s	[0,6000]*/
	eSmuGenPara102_VL2OffT,									/*低压二级延迟关机时间1s	[0,6000]*/
	eSmuGenPara102_VL1OffT,									/*低压一级延迟关机时间1s	[0,6000]*/
	eSmuGenPara102_ErrOffT,									/*系统故障延迟关机时间1s	[0,6000]*/
	eSmuGenPara102_ComOffT,									/*外部通讯故障延迟关机时间1s	[0,6000]*/

	eSmuGenPara102_IdleChgC,                             	//堆控空闲零点充电电流值[0,100]0.1A
	eSmuGenPara102_IdleDhgC,                             	//堆控空闲零点放电电流值[0,100]0.1A
	eSmuGenPara102_MaiSafeC,                           		//主接触器安全电流值[0,5000]0.1A
	eSmuGenPara102_PreSafeC,                             	//预充接触器安全电流值[0,500]0.1A
	eSmuGenPara102_AutSafeC,                           		//电池组动态投切安全电流[0,5000]0.1A
	eSmuGenPara102_UpSafeV,                             	//启动电池串间安全电压差[0,500]0.1V
	eSmuGenPara102_WaitSafeT,                           	//逐个接触器吸合前等待电流超时时间[1,3000]10ms
	eSmuGenPara102_WaitPreT,                              	//所有预充接触器吸合后等待电流预充时间[0,60000]10ms
	eSmuGenPara102_PreMorTEn,                               //所有预充接触器吸合后组数少时预充时间延长使能[0,1]
	eSmuGenPara102_UpSwhExpT,                           	//启动接触器操作失败检测时间[1,3000]10ms
	eSmuGenPara102_DnSwhExpT,                            	//停机接触器操作失败检测时间[1,300]100ms
	eSmuGenPara102_StopFailT,                            	//停机失败检测时间[1,300]100ms
	eSmuGenPara102_InitWaitT,                             	//自检前等待时间[1,300]100ms
	eSmuGenPara102_UpWaitT,                             	//自启动前等待时间[1,60]s
	eSmuGenPara102_CutLimDnEn,                              //切降限值使能[0,1]
	eSmuGenPara102_CutTHWaitT,                            	//高温隔离后再并网需等待时间[1,1000]s
	eSmuGenPara102_CutUpWaitT,                             	//隔离后再并网至少等待时间[1,1000]s
	eSmuGenPara102_AutUpMaxV,                           	//不能动态并网最大电压值[2000,5000]mv
	eSmuGenPara102_AutUpMinV,                           	//不能动态并网最小电压值[1000,4000]mv
	eSmuGenPara102_ClrCLimMaxV,                           	//释放充电电流限值最大电压值[2000,5000]mv
	eSmuGenPara102_ClrDLimMinV,                           	//释放放电电流限值最小电压值[1000,4000]mv
	eSmuGenPara102_ChgRateP,                             	//充电输入额定功率
	eSmuGenPara102_DhgRateP,                             	//放电输出额定功率
	eSmuGenPara102_ChgMaxP,                             	//充电最大请求功率
	eSmuGenPara102_DhgMaxP,                              	//放电最大请求功率

	eSmuGenPara102_CfgFin           						/*设备通用参数配置结束*/
}e_PGenPara102;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/

#endif
#endif
