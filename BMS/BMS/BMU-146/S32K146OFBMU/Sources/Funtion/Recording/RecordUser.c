/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : RecordUser.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 事件/数据记录模块用户处理
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "osif.h"
#include "PublicInfo.h"
#include "RecordInterface.h"
#include "RecordUser.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
t_RcdPara gRcdPara = {0};               /*存储模块参数*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void RcdStoreParaInitHook(void)
** @Input     : void
** @Output    : void
** @Function  : 记录存储模块用户参数初始化
** @The notes : 模块初始化已调用
**===============================================================================================*/
void RcdStoreParaInitHook(void)
{
	/*参数初始化*/
	gRcdPara.errRcdEn = RCD_HISERR_EN;
	gRcdPara.errStoEn = RCD_ERRWR_EN;
	gRcdPara.errStoDNum = RCD_ERRWD_DAYNUM;

	gRcdPara.evnRcdEn = RCD_EVENT_EN;
	gRcdPara.evnStoEn = RCD_EVNWR_EN;
	gRcdPara.evnStoDNum = RCD_EVNWD_DAYNUM;

	gRcdPara.wavRcdEn = RCD_FATWAVE_EN;
	gRcdPara.wavStoEn = RCD_WAVWR_EN;
	gRcdPara.wavStoDNum = RCD_WAVWD_DAYNUM;

	gRcdPara.dataStoEn = RCD_RUNDATA_EN;

	gRcdPara.dodStoEn = RCD_HISDOD_EN;
}

/*=================================================================================================
** @Name      : u8 RcdGetErrRcdEnStateAPI(void)
** @Input     : void
** @Output    : 故障记录存储功能使能状态 0:禁用(未初始化) 1:使能(初始化完成)
** @Function  : 获取故障记录存储功能使能状态
** @The notes : 配置使能时反映是否初始化
**===============================================================================================*/
u8 RcdGetErrRcdEnStateAPI(void)
{
	//return(gRcdPara.errRcdEn & gRcdPara.errStoEn);
	return(gRcdPara.errRcdEn);
}

/*=================================================================================================
** @Name      : u8 RcdGetEventRcdEnStateAPI(void)
** @Input     : void
** @Output    : 事件记录存储功能使能状态 0:禁用(未初始化) 1:使能(初始化完成)
** @Function  : 获取事件记录存储功能使能状态
** @The notes : 配置使能时反映是否初始化
**===============================================================================================*/
u8 RcdGetEventRcdEnStateAPI(void)
{
	//return(gRcdPara.evnRcdEn & gRcdPara.evnStoEn);
	return(gRcdPara.evnRcdEn);
}

/*=================================================================================================
** @Name      : u8 RcdGetDataRcdEnStateAPI(void)
** @Input     : void
** @Output    : 运行数据记录存储功能使能状态 0:禁用(未初始化) 1:使能(初始化完成)
** @Function  : 获取运行数据记录存储功能使能状态
** @The notes : 配置使能时反映是否初始化
**===============================================================================================*/
u8 RcdGetDataRcdEnStateAPI(void)
{
	return(gRcdPara.dataStoEn);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

