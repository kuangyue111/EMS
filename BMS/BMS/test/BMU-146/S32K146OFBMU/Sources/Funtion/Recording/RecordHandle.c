/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : RecordHandle.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 事件/数据记录模块处理
** @Instructions : 故障事件、动作事件和历史数据记录
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "RecordWave.h"
#include "RecordUser.h"
#include "RecordInterface.h"
#include "RecordHandle.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static t_RcdBuffState sErrRcdBuffState = {0};               /*故障记录缓存区读写状态信息(当前读写缓存区指针)*/
static t_RcdBuffState sEvnRcdBuffState = {0};               /*事件记录缓存区读写状态信息(当前读写缓存区指针)*/

static t_RcdDispState sErrRcdDispState = {0};               /*故障记录显示读写状态信息(记录当前读写EEPROM或FLASH指针)*/
static t_RcdDispState sEvnRcdDispState = {0};               /*事件记录显示读写状态信息(记录当前读写EEPROM或FLASH指针)*/
static t_RcdDispState sDatRcdDispState = {0};               /*运行数据记录显示读写状态信息(记录当前读写FLASH指针)*/
static t_RcdDispState sWavRcdDispState = {0};               /*故障录波记录显示读写状态信息(记录当前读写FLASH指针)*/
static t_RcdDispState sDodRcdDispState = {0};               /*DOD记录显示读写状态信息(记录当前读写FLASH指针)*/

static u16 sErrRcdBuff[RCD_ERRBUFF_LEN] = {0};              /*缓存故障记录*/
static u16 sEvnRcdBuff[RCD_ERRBUFF_LEN] = {0};              /*缓存事件记录*/
static t_DodStoreInfo sDodRcdBuffInfo = {0};                /*缓存DOD记录*/

static u16 sErrRcdStorState[RCD_STA_INFO_STOR_LEN] = {0};   /*故障记录存储状态信息读写指针存储长度(包括存储有效标志/读写指针/每天起始扇区指针[31天]/每天记录数目[31天])*/
static u16 sEvnRcdStorState[RCD_STA_INFO_STOR_LEN] = {0};   /*事件记录存储状态信息读写指针存储长度(包括存储有效标志/读写指针/每天起始扇区指针[31天]/每天记录数目[31天])*/
static u16 sDatRcdStorState[RCD_STA_INFO_STOR_LEN] = {0};   /*运行数据记录存储状态信息读写指针存储长度(包括存储有效标志/读写指针/每天起始扇区指针[31天]/每天记录数目[31天])*/
static u16 sWavRcdStorState[RCD_STA_INFO_STOR_LEN] = {0};   /*故障录波记录存储状态信息读写指针存储长度(包括存储有效标志/读写指针/每天起始扇区指针[31天]/每天记录数目[31天])*/
static u16 sDodRcdStorState[RCD_STOR_WRP_LEN] = {0};        /*DOD记录存储状态信息读写指针存储长度(DOD不按天存储,仅记录存储有效标志和读写指针*/

static u8 sErrRcdStorReadBusyFlag = 0;                      /*故障记录读写存储器忙标志(1:开始读取 2:正在读取)*/
static u8 sEvnRcdStorReadBusyFlag = 0;                      /*事件记录读写存储器忙标志(1:开始读取 2:正在读取)*/
static u8 sDatRcdStorReadBusyFlag = 0;                      /*数据记录读写存储器忙标志(1:开始读取 2:正在读取)*/
static u8 sWavRcdStorReadBusyFlag = 0;                      /*故障录波读写存储器忙标志(1:开始读取 2:正在读取)*/
static u8 sDodRcdStorReadBusyFlag = 0;                      /*DOD记录读写存储器忙标志(1:开始读取 2:正在读取)*/

extern t_RcdPara gRcdPara;                                  /*存储模块参数*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u32 RcdCalcDataRcdAimAddr(u8 date, u8 sec, u16 num)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录) date:第几天 sec:扇区号 num:记录序号
** @Output    : 记录存储写目标地址
** @Function  : 计算记录存储目标地址
** @The notes : 根据读写指针计算故障记录/事件记录/数据记录存储地址
**===============================================================================================*/
static u32 RcdCalcRecordAimAddr(u8 sort, u8 date, u8 sec, u16 num);

/*=================================================================================================
** @Name      : static void RcdWriteRecodStoreStateInfo(u8 sort)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录)
** @Output    : void
** @Function  : 写记录存储状态信息到Flash
** @The notes : 包括有效标志/读写指针/每天记录起始扇区/每天记录数目
**===============================================================================================*/
static void RcdWriteRecodStoreStateInfo(u8 sort);

/*=================================================================================================
** @Name      : static void RcdResetNowDayStoreReady(u8 sort)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录)
** @Output    : void
** @Function  : 复位当天存储为就绪状态
** @The notes : 擦除当天需要写的FLASH,当天记录数目为0才能调用
**===============================================================================================*/
static void RcdResetNowDayStoreReady(u8 sort);

/*=================================================================================================
** @Name      : static void RcdResetNextDayStoreReady(u8 sort)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波)
** @Output    : void
** @Function  : 复位下一天存储为就绪状态
** @The notes : 写指针下移1天,擦除一天需要写的FLASH(DOD不按天存储无需准备下一天)
**===============================================================================================*/
static void RcdResetNextDayStoreReady(u8 sort);

/*=================================================================================================
** @Name      : static u8 RcdEraseOneDayStoreReady(u8 sort, u8 step)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录) step:擦除步骤(0起始 1非开始擦除)
** @Output    : TRUE:擦除完成 FALSE:未擦完(需再次执行该函数)
** @Function  : 分多次擦除一天存储扇区为就绪状态
** @The notes : 擦除当天所有FLASH后重新写记录状态信息
**===============================================================================================*/
static u8 RcdEraseOneDayStoreReady(u8 sort, u8 step);

/*=================================================================================================
** @Name      : static void RcdStoreErrRcdInfoDeal(t_RcdStoreInfo *storeInfo)
** @Input     : storeInfo:需要存储的记录
** @Output    : void
** @Function  : 存储一条故障记录
** @The notes : 将记录写FLASH
**===============================================================================================*/
static void RcdStoreErrRcdInfoDeal(t_RcdStoreInfo *storeInfo);

/*=================================================================================================
** @Name      : static void RcdStoreEventRcdInfoDeal(t_RcdStoreInfo *storeInfo)
** @Input     : storeInfo:需要存储的记录
** @Output    : void
** @Function  : 存储一条事件记录
** @The notes : 将记录写FLASH
**===============================================================================================*/
static void RcdStoreEventRcdInfoDeal(t_RcdStoreInfo *storeInfo);

/*=================================================================================================
** @Name      : static void RcdStoreDodRcdInfoDeal(t_DodStoreInfo *storeInfo)
** @Input     : storeInfo:需记录的历史DOD记录信息
** @Output    : void
** @Function  : 存储一条历史DOD记录
** @The notes : 将记录写FLASH
**===============================================================================================*/
static void RcdStoreDodRcdInfoDeal(t_DodStoreInfo *storeInfo);

/*=================================================================================================
** @Name      : static void RcdStoreFlashClrEventRcdAPI(u8 sort)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录)
** @Output    : void
** @Function  : 事件存储空间FLASH擦写事件记录
** @The notes : 直接将事件写FLASH
**===============================================================================================*/
static void RcdStoreFlashClrEventRcdAPI(u8 sort);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void RcdStoreStateInit(void)
** @Input     : void
** @Output    : void
** @Function  : 记录存储状态信息初始化(记录读写指针和数目)
** @The notes : 模块参数后/用户读写记录前调用
**===============================================================================================*/
void RcdStoreStateInit(void)
{
	u8 i = 0;
	u8 err = 0;
	u32 sumNum = 0;

    /*记录存储模块用户参数初始化*/
	RcdStoreParaInitHook();

    /*故障录波缓存区初始化*/
	RcdFaultWaveBuffInit();

    /*故障记录初始化*/
	if((1 == gRcdPara.errRcdEn)
		&& (1 == gRcdPara.errStoEn))
	{
		/*从外部FLASH读取故障记录状态信息*/
		if((TRUE == RcdReadRcdStoreInfoHook(0, RCD_ERRWS_START_ADDR, sErrRcdStorState, RCD_STA_INFO_STOR_LEN))
			&& (RCD_STOR_STATE_FLAG == sErrRcdStorState[eRcdStorState_flag]) && (sErrRcdStorState[eRcdStorState_allDay] > 0))
		{
			/*校验每天记录状态值是否有效*/
			for(i = 0; i < RCD_STOR_MAXDAYS; i++)
			{
			    /*每天记录数目或扇区数错误*/
				if((sErrRcdStorState[eRcdStorState_eDayNum + i] > RCD_ERRWD_DAYNUM)
					|| (sErrRcdStorState[eRcdStorState_dayFSec + i] >= RCD_ERRWD_DSEC_NUM))
				{
					err = 1;
					sErrRcdStorState[eRcdStorState_eDayNum + i] = 0;
					sErrRcdStorState[eRcdStorState_dayFSec + i] = 0;
				}
				sumNum += sErrRcdStorState[eRcdStorState_eDayNum + i];
			}

			/*校验剩余记录总数值是否有效*/
			if(sumNum < sErrRcdStorState[eRcdStorState_allNum])
			{
				err = 1;

				//总数目出错只算每天累计总数目
				sErrRcdStorState[eRcdStorState_allNum] = sumNum;

				//读指针恢复到最早一天
				//未写满31天或写到最后一天位置
				if((sErrRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
					|| (sErrRcdStorState[eRcdStorState_wDayP] == (RCD_STOR_MAXDAYS - 1)))
				{
					//读起始天为最早一天
					sErrRcdStorState[eRcdStorState_rDayP] = 0;
					sErrRcdStorState[eRcdStorState_rSecP] = 0;
					sErrRcdStorState[eRcdStorState_rNumP] = 0;
				}
				//写满31天回头写
				else
				{
					//读起始天为写指针后一天
					sErrRcdStorState[eRcdStorState_rDayP] = sErrRcdStorState[eRcdStorState_wDayP] + 1;
					sErrRcdStorState[eRcdStorState_rSecP] = 0;
					sErrRcdStorState[eRcdStorState_rNumP] = 0;
				}
			}

			/*校验记录天数值是否有效*/
			if(sErrRcdStorState[eRcdStorState_allDay] > RCD_STOR_MAXDAYS)
			{
				err = 1;
				sErrRcdStorState[eRcdStorState_allDay] = RCD_STOR_MAXDAYS;
			}

			/*校验记录写天指针是否有效*/
			if(sErrRcdStorState[eRcdStorState_wDayP] >= sErrRcdStorState[eRcdStorState_allDay])
			{
				err = 1;
				sErrRcdStorState[eRcdStorState_wDayP] = 0;
			}

			/*校验记录写天指针是否有效*/
			if(sErrRcdStorState[eRcdStorState_rDayP] >= sErrRcdStorState[eRcdStorState_allDay])
			{
				err = 1;
				sErrRcdStorState[eRcdStorState_rDayP] = 0;
			}

			/*校验记录剩余数值是否有效*/
			if(sErrRcdStorState[eRcdStorState_remNum] > (sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_rDayP]]))
			{
				err = 1;
				sErrRcdStorState[eRcdStorState_remNum] = sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
			}

			/*校记有异常*/
			if(1 == err)
			{
				/*重写更新状态信息*/
				RcdWriteRecodStoreStateInfo(0);
			}
		}
		else
		{
			/*当前记录读写指针清零*/
			for(i = 0; i < RCD_STA_INFO_STOR_LEN; i++)
			{
				sErrRcdStorState[i] = 0;
			}
		}

		/*显示记录指针指向最新写记录*/
		sErrRcdDispState.dDPoint = sErrRcdStorState[eRcdStorState_wDayP];
		sErrRcdDispState.dDaNum = sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]];

		/*有记录*/
		if(sErrRcdDispState.dDaNum > 0)
		{
			/*扇区指针和序号指针取当天最后写结束位置(当天写满条数则认为最末扇区为写结束位置)*/
			sErrRcdDispState.dSPoint = (sErrRcdDispState.dDaNum - 1) / RCD_ERRWD_SECNUM;
			sErrRcdDispState.dNPoint = (sErrRcdDispState.dDaNum - 1) % RCD_ERRWD_SECNUM;
		}
		else
		{
			/*起始扇区*/
			sErrRcdDispState.dSPoint = 0;
			sErrRcdDispState.dNPoint = 0;
		}
	}

    /*事件记录初始化*/
	if((1 == gRcdPara.evnRcdEn)
		&& (1 == gRcdPara.evnStoEn))
	{
		/*从外部FLASH读取事件记录状态信息*/
		if((TRUE == RcdReadRcdStoreInfoHook(1, RCD_EVNWS_START_ADDR, sEvnRcdStorState, RCD_STA_INFO_STOR_LEN))
			&& (RCD_STOR_STATE_FLAG == sEvnRcdStorState[eRcdStorState_flag]) && (sEvnRcdStorState[eRcdStorState_allDay] > 0))
		{
			err = 0;
			sumNum = 0;
			/*校验每天记录状态值是否有效*/
			for(i = 0; i < RCD_STOR_MAXDAYS; i++)
			{
			    /*每天记录数目或扇区数错误*/
				if((sEvnRcdStorState[eRcdStorState_eDayNum + i] > RCD_EVNWD_DAYNUM)
					|| (sEvnRcdStorState[eRcdStorState_dayFSec + i] >= RCD_EVNWD_DSEC_NUM))
				{
					err = 1;
					sEvnRcdStorState[eRcdStorState_eDayNum + i] = 0;
					sEvnRcdStorState[eRcdStorState_dayFSec + i] = 0;
				}
				sumNum += sEvnRcdStorState[eRcdStorState_eDayNum + i];
			}

			/*校验剩余记录总数值是否有效*/
			if(sumNum < sEvnRcdStorState[eRcdStorState_allNum])
			{
				err = 1;

				//总数目出错只算每天累计总数目
				sEvnRcdStorState[eRcdStorState_allNum] = sumNum;

				//读指针恢复到最早一天
				//未写满31天或写到最后一天位置
				if((sEvnRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
					|| (sEvnRcdStorState[eRcdStorState_wDayP] == (RCD_STOR_MAXDAYS - 1)))
				{
					//读起始天为最早一天
					sEvnRcdStorState[eRcdStorState_rDayP] = 0;
					sEvnRcdStorState[eRcdStorState_rSecP] = 0;
					sEvnRcdStorState[eRcdStorState_rNumP] = 0;
				}
				//写满31天回头写
				else
				{
					//读起始天为写指针后一天
					sEvnRcdStorState[eRcdStorState_rDayP] = sEvnRcdStorState[eRcdStorState_wDayP] + 1;
					sEvnRcdStorState[eRcdStorState_rSecP] = 0;
					sEvnRcdStorState[eRcdStorState_rNumP] = 0;
				}
			}

			/*校验记录天数值是否有效*/
			if(sEvnRcdStorState[eRcdStorState_allDay] > RCD_STOR_MAXDAYS)
			{
				err = 1;
				sEvnRcdStorState[eRcdStorState_allDay] = RCD_STOR_MAXDAYS;
			}

			/*校验记录写天指针是否有效*/
			if(sEvnRcdStorState[eRcdStorState_wDayP] >= RCD_STOR_MAXDAYS)
			{
				err = 1;
				sEvnRcdStorState[eRcdStorState_wDayP] = 0;
			}

			/*校验记录写天指针是否有效*/
			if(sEvnRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
			{
				err = 1;
				sEvnRcdStorState[eRcdStorState_rDayP] = 0;
			}

			/*校验记录剩余数值是否有效*/
			if(sEvnRcdStorState[eRcdStorState_remNum] > (sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_rDayP]]))
			{
				err = 1;
				sEvnRcdStorState[eRcdStorState_remNum] = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
			}

			/*校记有异常*/
			if(1 == err)
			{
				/*重写更新状态信息*/
				RcdWriteRecodStoreStateInfo(1);
			}
		}
		else
		{
			/*当前记录读写指针清零*/
			for(i = 0; i < RCD_STA_INFO_STOR_LEN; i++)
			{
				sEvnRcdStorState[i] = 0;
			}
		}

		/*显示记录指针指向最新写记录*/
		sEvnRcdDispState.dDPoint = sEvnRcdStorState[eRcdStorState_wDayP];
		sEvnRcdDispState.dDaNum = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]];

		/*有记录*/
		if(sEvnRcdDispState.dDaNum > 0)
		{
			/*扇区指针和序号指针取当天最后写结束位置(当天写满条数则认为最末扇区为写结束位置)*/
			sEvnRcdDispState.dSPoint = (sEvnRcdDispState.dDaNum - 1) / RCD_EVNWD_SECNUM;
			sEvnRcdDispState.dNPoint = (sEvnRcdDispState.dDaNum - 1) % RCD_EVNWD_SECNUM;
		}
		else
		{
			/*起始扇区*/
			sEvnRcdDispState.dSPoint = 0;
			sEvnRcdDispState.dNPoint = 0;
		}
	}

    /*运行数据记录初始化*/
	if(1 == gRcdPara.dataStoEn)
	{
		/*从外部FLASH读取数据记录状态信息*/
		if((TRUE == RcdReadRcdStoreInfoHook(2, RCD_DATWS_START_ADDR, sDatRcdStorState, RCD_STA_INFO_STOR_LEN))
			&& (RCD_STOR_STATE_FLAG == sDatRcdStorState[eRcdStorState_flag]) && (sDatRcdStorState[eRcdStorState_allDay] > 0))
		{
			err = 0;
			sumNum = 0;
			/*校验每天记录状态值是否有效*/
			for(i = 0; i < RCD_STOR_MAXDAYS; i++)
			{
			    /*每天记录数目或扇区数错误*/
				if((sDatRcdStorState[eRcdStorState_eDayNum + i] > RCD_DATWD_DAYNUM)
					|| (sDatRcdStorState[eRcdStorState_dayFSec + i] >= RCD_DATWD_DSEC_NUM))
				{
					err = 1;
					sDatRcdStorState[eRcdStorState_eDayNum + i] = 0;
					sDatRcdStorState[eRcdStorState_dayFSec + i] = 0;
				}
				sumNum += sDatRcdStorState[eRcdStorState_eDayNum + i];
			}

			/*校记录验总数值是否有效*/
			if(sumNum != sDatRcdStorState[eRcdStorState_allNum])
			{
				err = 1;
				sDatRcdStorState[eRcdStorState_allNum] = sumNum;
			}

			/*校验记录天数值是否有效*/
			if(sDatRcdStorState[eRcdStorState_allDay] > RCD_STOR_MAXDAYS)
			{
				err = 1;
				sDatRcdStorState[eRcdStorState_allDay] = RCD_STOR_MAXDAYS;
			}

			/*校验记录写天指针是否有效*/
			if(sDatRcdStorState[eRcdStorState_wDayP] >= RCD_STOR_MAXDAYS)
			{
				err = 1;
				sDatRcdStorState[eRcdStorState_wDayP] = 0;
			}

			/*校验记录写天指针是否有效*/
			if(sDatRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
			{
				err = 1;
				sDatRcdStorState[eRcdStorState_rDayP] = 0;
			}

			/*校验记录剩余数值是否有效*/
			if(sDatRcdStorState[eRcdStorState_remNum] > (sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_rDayP]]))
			{
				err = 1;
				sDatRcdStorState[eRcdStorState_remNum] = sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_rDayP]];
			}

			/*校记有异常*/
			if(1 == err)
			{
				/*重写更新状态信息*/
				RcdWriteRecodStoreStateInfo(2);
			}
		}
		else
		{
			/*当前记录读写指针清零*/
			for(i = 0; i < RCD_STA_INFO_STOR_LEN; i++)
			{
				sDatRcdStorState[i] = 0;
			}
		}

		/*显示记录指针指向最新写记录*/
		sDatRcdDispState.dDPoint = sDatRcdStorState[eRcdStorState_wDayP];
		sDatRcdDispState.dDaNum = sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]];

		/*有记录*/
		if(sDatRcdDispState.dDaNum > 0)
		{
			/*扇区指针和序号指针取当天最后写结束位置(当天写满条数则认为最末扇区为写结束位置)*/
			sDatRcdDispState.dSPoint = (sDatRcdDispState.dDaNum - 1) / RCD_DATWD_SECNUM;
			sDatRcdDispState.dNPoint = (sDatRcdDispState.dDaNum - 1) % RCD_DATWD_SECNUM;
		}
		else
		{
			/*起始扇区*/
			sDatRcdDispState.dSPoint = 0;
			sDatRcdDispState.dNPoint = 0;
		}
	}

    /*故障录波初始化*/
	if((1 == gRcdPara.wavRcdEn)
		&& (1 == gRcdPara.wavStoEn))
	{
		/*从外部FLASH读取故障录波记录状态信息*/
		if((TRUE == RcdReadRcdStoreInfoHook(3, RCD_WAVWS_START_ADDR, sWavRcdStorState, RCD_STA_INFO_STOR_LEN))
			&& (RCD_STOR_STATE_FLAG == sWavRcdStorState[eRcdStorState_flag]) && (sWavRcdStorState[eRcdStorState_allDay] > 0))
		{
			err = 0;
			sumNum = 0;
			/*校验每天记录状态值是否有效*/
			for(i = 0; i < RCD_STOR_MAXDAYS; i++)
			{
			    /*每天记录数目或扇区数错误*/
				if((sWavRcdStorState[eRcdStorState_eDayNum + i] > RCD_WAVWD_DAYNUM)
					|| (sWavRcdStorState[eRcdStorState_dayFSec + i] >= RCD_WAVWD_DSEC_NUM))
				{
					err = 1;
					sWavRcdStorState[eRcdStorState_eDayNum + i] = 0;
					sWavRcdStorState[eRcdStorState_dayFSec + i] = 0;
				}
				sumNum += sWavRcdStorState[eRcdStorState_eDayNum + i];
			}

			/*校验剩余记录总数值是否有效*/
			if(sumNum < sWavRcdStorState[eRcdStorState_allNum])
			{
				err = 1;

				//总数目出错只算每天累计总数目
				sWavRcdStorState[eRcdStorState_allNum] = sumNum;

				//读指针恢复到最早一天
				//未写满31天或写到最后一天位置
				if((sWavRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
					|| (sWavRcdStorState[eRcdStorState_wDayP] == (RCD_STOR_MAXDAYS - 1)))
				{
					//读起始天为最早一天
					sWavRcdStorState[eRcdStorState_rDayP] = 0;
					sWavRcdStorState[eRcdStorState_rSecP] = 0;
					sWavRcdStorState[eRcdStorState_rNumP] = 0;
				}
				//写满31天回头写
				else
				{
					//读起始天为写指针后一天
					sWavRcdStorState[eRcdStorState_rDayP] = sWavRcdStorState[eRcdStorState_wDayP] + 1;
					sWavRcdStorState[eRcdStorState_rSecP] = 0;
					sWavRcdStorState[eRcdStorState_rNumP] = 0;
				}
			}

			/*校验记录天数值是否有效*/
			if(sWavRcdStorState[eRcdStorState_allDay] > RCD_STOR_MAXDAYS)
			{
				err = 1;
				sWavRcdStorState[eRcdStorState_allDay] = RCD_STOR_MAXDAYS;
			}

			/*校验记录写天指针是否有效*/
			if(sWavRcdStorState[eRcdStorState_wDayP] >= RCD_STOR_MAXDAYS)
			{
				err = 1;
				sWavRcdStorState[eRcdStorState_wDayP] = 0;
			}

			/*校验记录写天指针是否有效*/
			if(sWavRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
			{
				err = 1;
				sWavRcdStorState[eRcdStorState_rDayP] = 0;
			}

			/*校验记录剩余数值是否有效*/
			if(sWavRcdStorState[eRcdStorState_remNum] > (sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_rDayP]]))
			{
				err = 1;
				sWavRcdStorState[eRcdStorState_remNum] = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
			}

			/*校记有异常*/
			if(1 == err)
			{
				/*重写更新状态信息*/
				RcdWriteRecodStoreStateInfo(3);
			}
		}
		else
		{
			/*当前记录读写指针清零*/
			for(i = 0; i < RCD_STA_INFO_STOR_LEN; i++)
			{
				sWavRcdStorState[i] = 0;
			}
		}

		/*显示记录指针指向最新写记录*/
		sWavRcdDispState.dDPoint = sWavRcdStorState[eRcdStorState_wDayP];
		sWavRcdDispState.dDaNum = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]];

		/*有记录*/
		if(sWavRcdDispState.dDaNum > 0)
		{
			/*扇区指针和序号指针取当天最后写结束位置(当天写满条数则认为最末扇区为写结束位置)*/
			sWavRcdDispState.dSPoint = (sWavRcdDispState.dDaNum - 1) / RCD_WAVWD_SECNUM;
			sWavRcdDispState.dNPoint = (sWavRcdDispState.dDaNum - 1) % RCD_WAVWD_SECNUM;
		}
		else
		{
			/*起始扇区*/
			sWavRcdDispState.dSPoint = 0;
			sWavRcdDispState.dNPoint = 0;
		}
	}

    /*历史DOD记录初始化*/
	if(1 == gRcdPara.dodStoEn)
	{
		/*从外部FLASH读取DOD记录状态信息*/
		if((TRUE == RcdReadRcdStoreInfoHook(4, RCD_DODWS_START_ADDR, sDodRcdStorState, RCD_STOR_WRP_LEN))
			&& (RCD_STOR_STATE_FLAG == sDodRcdStorState[eRcdStorState_flag]) && (sDodRcdStorState[eRcdStorState_allNum] > 0))
		{
			err = 0;
			/*校记录验总数值是否有效*/
			if(sDodRcdStorState[eRcdStorState_allNum] > RCD_DODWD_ALLNUM)
			{
				err = 1;
				sDodRcdStorState[eRcdStorState_allNum] = RCD_DODWD_ALLNUM;
			}

			/*校验记录剩余数值是否有效*/
			if(sDodRcdStorState[eRcdStorState_remNum] > sDodRcdStorState[eRcdStorState_allNum])
			{
				err = 1;
				sDodRcdStorState[eRcdStorState_remNum] = sDodRcdStorState[eRcdStorState_allNum];
			}

			/*校记有异常*/
			if(1 == err)
			{
				/*重写更新状态信息*/
				RcdWriteRecodStoreStateInfo(4);
			}
		}
		else
		{
			/*当前记录读写指针清零*/
			for(i = 0; i < RCD_STOR_WRP_LEN; i++)
			{
				sDodRcdStorState[i] = 0;
			}
		}

		/*显示记录指针指向最新写记录*/
		sDodRcdDispState.dDPoint = sDodRcdStorState[eRcdStorState_wDayP];
		sDodRcdDispState.dDaNum = sDodRcdStorState[eRcdStorState_allNum];

		/*有记录*/
		if(sDodRcdDispState.dDaNum > 0)
		{
			/*扇区指针和序号指针取当天最后写结束位置(当天写满条数则认为最末扇区为写结束位置)*/
			sDodRcdDispState.dSPoint = (sDodRcdDispState.dDaNum - 1) / RCD_DODWD_SEC_NUM;
			sDodRcdDispState.dNPoint = (sDodRcdDispState.dDaNum - 1) % RCD_DODWD_SEC_NUM;
		}
		else
		{
			/*起始扇区*/
			sDodRcdDispState.dSPoint = 0;
			sDodRcdDispState.dNPoint = 0;
		}

		/*DOD记录缓存区清除*/
		sDodRcdBuffInfo.dod = 0xffff;
		sDodRcdBuffInfo.chgC = 0;
		sDodRcdBuffInfo.dhgC = 0;
		sDodRcdBuffInfo.chgE = 0;
		sDodRcdBuffInfo.dhgE = 0;
	}
}

/*=================================================================================================
** @Name      : void RcdStoreHandleTask(void)
** @Input     : void
** @Output    : void
** @Function  : 记录存储处理任务
** @The notes : 600ms周期
**===============================================================================================*/
void RcdStoreHandleTask(void)
{
	u16 nowDate = 0;
	static u8 sTime = 0;
	static u8 sState = 0;
	t_RcdStoreInfo info;
	u16 data[RCD_DATINFO_LEN] = {0};

	/*用户需要停止写记录*/
	if(1 == RcdGetStoreStopRcdStateHook())
	{
		sTime = 0;
		sState = 0;
		return;
	}

	/*检测故障记录是否正在读取*/
	if(1 == sErrRcdStorReadBusyFlag)
	{
		sTime = 0;
		sState = 0;
		sErrRcdStorReadBusyFlag = 2;
	}

	/*检测事件记录是否正在读取*/
	if(1 == sEvnRcdStorReadBusyFlag)
	{
		sTime = 0;
		sState = 0;
		sEvnRcdStorReadBusyFlag = 2;
	}

	/*检测数据记录是否正在读取*/
	if(1 == sDatRcdStorReadBusyFlag)
	{
		sTime = 0;
		sState = 0;
		sDatRcdStorReadBusyFlag = 2;
	}

	/*检测故障录波记录是否正在读取*/
	if(1 == sWavRcdStorReadBusyFlag)
	{
		sTime = 0;
		sState = 0;
		sWavRcdStorReadBusyFlag = 2;
	}

	/*检测DOD记录是否正在读取*/
	if(1 == sDodRcdStorReadBusyFlag)
	{
		sTime = 0;
		sState = 0;
		sDodRcdStorReadBusyFlag = 2;
	}

	switch(sState)
	{
		/*检测是否进行存储*/
		case 0:
			/*正在读取记录*/
			if((sErrRcdStorReadBusyFlag > 0) || (sEvnRcdStorReadBusyFlag > 0) || (sDatRcdStorReadBusyFlag > 0) || (sWavRcdStorReadBusyFlag > 0) || (sDodRcdStorReadBusyFlag > 0))
			{
				/*10s未进行读取*/
				if(sTime >= 17)
				{
					/*读状态恢复为空闲*/
					sTime = 0;
					sErrRcdStorReadBusyFlag = 0;
					sEvnRcdStorReadBusyFlag = 0;
					sDatRcdStorReadBusyFlag = 0;
					sWavRcdStorReadBusyFlag = 0;
					sDodRcdStorReadBusyFlag = 0;
				}
				else
				{
					sTime++;
				}
			}
			/*存储可写*/
			else
			{
				/*当前日期转换成记录格式(高8位:年,中8位:月,低16位:日)*/
				nowDate = ((GetSysDateClockAPI().year & 0x0f) << 12) | ((GetSysDateClockAPI().month & 0x0f) << 8) | (GetSysDateClockAPI().day & 0xff);

				/*事件记录存储使能且日期变更*/
				if((1 == gRcdPara.evnStoEn) && (sEvnRcdStorState[eRcdStorState_wDate] != nowDate))
				{
					/*间隔2s(防止连续擦除)*/
					if(sTime >= 3)
					{
						/*日期有效且当天已有记录*/
						if((0 != sEvnRcdStorState[eRcdStorState_wDate]) && (sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] > 0))
						{
							/*存储日期指针下移一天,擦除新的一天区域*/
							sEvnRcdStorState[eRcdStorState_wDate] = nowDate;
							RcdResetNextDayStoreReady(1);

							/*进入FLASH分次擦除*/
							sState = 120;
						}
						/*日期无效或当天无记录*/
						else
						{
							/*日期无效(清除记录)则擦除当天区域和重新记录数据记录状态信息*/
							if(0 == sEvnRcdStorState[eRcdStorState_wDate])
							{
								/*仅更新日期(存储指针不变)*/
								sEvnRcdStorState[eRcdStorState_wDate] = nowDate;

								/*复位擦除当天所有扇区*/
								RcdResetNowDayStoreReady(1);

								/*进入FLASH分次擦除*/
								sState = 120;
							}

							/*日期递增但前一天无记录,仅更新日期(存储指针不变)*/
							sEvnRcdStorState[eRcdStorState_wDate] = nowDate;
						}
						sTime = 0;
					}
					else
					{
						/*日期无效(清除记录不计事件,会被擦除)或当天已有记录,当天或下一天需要擦除FLASH*/
						if(/*(0 == sEvnRcdStorState[eRcdStorState_wDate]) || */(sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] > 0))
						{
							/*先记录一条事件*/
							if(1 == sTime)
							{
								/*内部事件直接写FLASH防止丢失*/
								RcdStoreFlashClrEventRcdAPI(1);
							}
						}
						sTime++;
					}
				}
				/*故障记录存储使能且日期变更*/
				else if((1 == gRcdPara.errStoEn) && (sErrRcdStorState[eRcdStorState_wDate] != nowDate))
				{
					/*间隔2s(防止连续擦除)*/
					if(sTime >= 3)
					{
						/*日期有效且当天已有记录*/
						if((0 != sErrRcdStorState[eRcdStorState_wDate]) && (sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]] > 0))
						{
							/*存储日期指针下移一天,擦除新的一天区域*/
							sErrRcdStorState[eRcdStorState_wDate] = nowDate;
							RcdResetNextDayStoreReady(0);

							/*进入FLASH分次擦除*/
							sState = 110;
						}
						/*日期无效或当天无记录*/
						else
						{
							/*日期无效(清除记录)则擦除当天区域和重新记录数据记录状态信息*/
							if(0 == sErrRcdStorState[eRcdStorState_wDate])
							{
								/*仅更新日期(存储指针不变)*/
								sErrRcdStorState[eRcdStorState_wDate] = nowDate;

								/*复位擦除当天所有扇区*/
								RcdResetNowDayStoreReady(0);

								/*进入FLASH分次擦除*/
								sState = 110;
							}

							/*日期递增但前一天无记录,仅更新日期(存储指针不变)*/
							sErrRcdStorState[eRcdStorState_wDate] = nowDate;
						}
						sTime = 0;
					}
					else
					{
						/*日期无效或当天已有记录,当天或下一天需要擦除FLASH*/
						if((0 == sErrRcdStorState[eRcdStorState_wDate]) || (sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]] > 0))
						{
							/*先记录一条事件*/
							if(1 == sTime)
							{
								/*内部事件直接写FLASH防止丢失*/
								RcdStoreFlashClrEventRcdAPI(0);
							}
						}
						sTime++;
					}
				}
				/*运行数据记录存储使能且日期变更*/
				else if((1 == gRcdPara.dataStoEn) && (sDatRcdStorState[eRcdStorState_wDate] != nowDate))
				{
					/*间隔2s(防止连续擦除)*/
					if(sTime >= 3)
					{
						/*日期有效且当天已有记录*/
						if((0 != sDatRcdStorState[eRcdStorState_wDate]) && (sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]] > 0))
						{
							/*存储日期指针下移一天,擦除新的一天区域*/
							sDatRcdStorState[eRcdStorState_wDate] = nowDate;
							RcdResetNextDayStoreReady(2);

							/*进入FLASH分次擦除*/
							sState = 130;
						}
						/*日期无效或当天无记录*/
						else
						{
							/*日期无效(清除记录)则擦除当天区域和重新记录数据记录状态信息*/
							if(0 == sDatRcdStorState[eRcdStorState_wDate])
							{
								/*仅更新日期(存储指针不变)*/
								sDatRcdStorState[eRcdStorState_wDate] = nowDate;

								/*复位擦除当天所有扇区*/
								RcdResetNowDayStoreReady(2);

								/*进入FLASH分次擦除*/
								sState = 130;
							}

							/*日期递增但前一天无记录,仅更新日期(存储指针不变)*/
							sDatRcdStorState[eRcdStorState_wDate] = nowDate;
						}
						sTime = 0;
					}
					else
					{
						/*日期无效或当天已有记录,当天或下一天需要擦除FLASH*/
						if((0 == sDatRcdStorState[eRcdStorState_wDate]) || (sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]] > 0))
						{
							/*先记录一条事件*/
							if(1 == sTime)
							{
								/*内部事件直接写FLASH防止丢失*/
								RcdStoreFlashClrEventRcdAPI(2);
							}
						}
						sTime++;
					}
				}
				/*故障录波记录存储使能且日期变更*/
				else if((1 == gRcdPara.wavStoEn) && (sWavRcdStorState[eRcdStorState_wDate] != nowDate))
				{
					/*间隔2s(防止连续擦除)*/
					if(sTime >= 3)
					{
						/*日期有效且当天已有记录*/
						if((0 != sWavRcdStorState[eRcdStorState_wDate]) && (sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]] > 0))
						{
							/*存储日期指针下移一天,擦除新的一天区域*/
							sWavRcdStorState[eRcdStorState_wDate] = nowDate;
							RcdResetNextDayStoreReady(3);

							/*进入FLASH分次擦除*/
							sState = 140;
						}
						/*日期无效或当天无记录*/
						else
						{
							/*日期无效(清除记录)则擦除当天区域和重新记录数据记录状态信息*/
							if(0 == sWavRcdStorState[eRcdStorState_wDate])
							{
								/*仅更新日期(存储指针不变)*/
								sWavRcdStorState[eRcdStorState_wDate] = nowDate;

								/*复位擦除当天所有扇区*/
								RcdResetNowDayStoreReady(3);

								/*进入FLASH分次擦除*/
								sState = 140;
							}

							/*日期递增但前一天无记录,仅更新日期(存储指针不变)*/
							sWavRcdStorState[eRcdStorState_wDate] = nowDate;
						}
						sTime = 0;
					}
					else
					{
						/*日期无效或当天已有记录,当天或下一天需要擦除FLASH*/
						if((0 == sWavRcdStorState[eRcdStorState_wDate]) || (sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]] > 0))
						{
							/*先记录一条事件*/
							if(1 == sTime)
							{
								/*内部事件直接写FLASH防止丢失*/
								RcdStoreFlashClrEventRcdAPI(3);
							}
						}
						sTime++;
					}
				}
				/*非日期变化*/
				else
				{
					/*计时清零*/
					sTime = 0;

					/*需要存储一条运行数据*/
					if((1 == gRcdPara.dataStoEn) && (0 == sDatRcdStorReadBusyFlag) && (TRUE == RcdGetGRunDataRcdReqHook()))
					{
						/*当天尚未有历史数据记录数据需要清除FLASH则先记录事件*/
						if((0 == sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]])
							&& ((sDatRcdStorState[eRcdStorState_wSecP] > 0) || (sDatRcdStorState[eRcdStorState_wNumP] > 0)))
						{
							/*内部事件直接写FLASH防止丢失*/
							RcdStoreFlashClrEventRcdAPI(2);
						}
						/*进入运行数据存储*/
						sState = 3;
					}
					/*需要存储一条故障录波*/
					else if((1 == gRcdPara.wavStoEn) && (0 == sWavRcdStorReadBusyFlag) && (eFatWavState_Fin == RcdGetFaultWaveStateAPI()))
					{
						/*当天尚未有故障录波数据需要清除FLASH则先记录事件*/
						if((0 == sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]])
							&& ((sWavRcdStorState[eRcdStorState_wSecP] > 0) || (sWavRcdStorState[eRcdStorState_wNumP] > 0)))
						{
							/*内部事件直接写FLASH防止丢失*/
							RcdStoreFlashClrEventRcdAPI(3);
						}
						/*进入故障录波存储*/
						sState = 4;
					}
					/*需要存储一个DOD记录*/
					else if((1 == gRcdPara.dodStoEn) && (0 == sDodRcdStorReadBusyFlag) && (0xffff != sDodRcdBuffInfo.dod))
					{
						/*尚未记录有DOD记录数据需要清除FLASH则先记录事件*/
						if(0 == sDodRcdStorState[eRcdStorState_allNum])
						{
							/*内部事件直接写FLASH防止丢失*/
							RcdStoreFlashClrEventRcdAPI(4);
						}
						/*进入DOD记录存储*/
						sState = 5;
					}
					/*缓存区有故障记录需要存储*/
					else if((1 == gRcdPara.errStoEn) && (0 == sErrRcdStorReadBusyFlag) && (sErrRcdBuffState.rcdNum > 0))
					{
						/*当天尚未有故障记录数据需要清除FLASH则先记录事件*/
						if((0 == sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]])
							&& ((sErrRcdStorState[eRcdStorState_wSecP] > 0) || (sErrRcdStorState[eRcdStorState_wNumP] > 0)))
						{
							/*内部事件直接写FLASH防止丢失*/
							RcdStoreFlashClrEventRcdAPI(0);
						}
						/*进入故障存储*/
						sState = 1;
					}
					/*缓存区有事件记录需要存储*/
					else if((1 == gRcdPara.evnStoEn) && (0 == sEvnRcdStorReadBusyFlag) && (sEvnRcdBuffState.rcdNum > 0))
					{
						/*当天尚未有事件记录数据需要清除FLASH则先记录事件*/
						if((0 == sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]])
							&& ((sEvnRcdStorState[eRcdStorState_wSecP] > 0) || (sEvnRcdStorState[eRcdStorState_wNumP] > 0)))
						{
							/*内部事件直接写FLASH防止丢失*/
							RcdStoreFlashClrEventRcdAPI(1);
						}
						/*进入事件存储*/
						sState = 2;
					}
				}
			}
			break;

		/*从故障记录缓存区取出记录存储*/
		case 1:
			/*当天无记录但写指针非起始位置,需清理FLAH*/
			if((0 == sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]])
				&& ((sErrRcdStorState[eRcdStorState_wSecP] > 0) || (sErrRcdStorState[eRcdStorState_wNumP] > 0)))
			{
				/*格式化当天需要的FLASH*/
				RcdResetNowDayStoreReady(0);
				//sState = 10;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 111;
			}
			/*缓存区有故障记录*/
			else if(sErrRcdBuffState.rcdNum > 0)
			{
				/*从缓存区取出1条记录*/
				if(TRUE == RcdReadOneErrQueAPI(&info))
				{
					/*将记录写存储*/
					RcdStoreErrRcdInfoDeal(&info);
				}
				//sState = 0;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 100;
			}
			else
			{
				sState = 0;
			}
			break;

		/*从故障记录缓存区取出记录存储*/
		case 10:
			/*缓存区有故障记录*/
			if(sErrRcdBuffState.rcdNum > 0)
			{
				/*从缓存区取出1条记录*/
				if(TRUE == RcdReadOneErrQueAPI(&info))
				{
					/*将记录写存储*/
					RcdStoreErrRcdInfoDeal(&info);
				}
			}
			//sState = 0;
			/*增加时间间隔,不连续操作FLASH*/
			sState = 100;
			break;

		/*从事件记录缓存区取出记录存储*/
		case 2:
			/*当天无记录但写指针非起始位置,需清理FLAH*/
			if((0 == sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]])
				&& ((sEvnRcdStorState[eRcdStorState_wSecP] > 0) || (sEvnRcdStorState[eRcdStorState_wNumP] > 0)))
			{
				/*格式化当天需要的FLASH*/
				RcdResetNowDayStoreReady(1);
				//sState = 20;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 121;
			}
			/*缓存区有故障记录*/
			else if(sEvnRcdBuffState.rcdNum > 0)
			{
				/*从缓存区取出1条记录*/
				if(TRUE == RcdReadOneEventQueAPI(&info))
				{
					/*将记录写存储*/
					RcdStoreEventRcdInfoDeal(&info);
				}
				//sState = 0;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 100;
			}
			else
			{
				sState = 0;
			}
			break;

		/*从事件记录缓存区取出记录存储*/
		case 20:
			/*缓存区有故障记录*/
			if(sEvnRcdBuffState.rcdNum > 0)
			{
				/*从缓存区取出1条记录*/
				if(TRUE == RcdReadOneEventQueAPI(&info))
				{
					/*将记录写存储*/
					RcdStoreEventRcdInfoDeal(&info);
				}
			}
			//sState = 0;
			/*增加时间间隔,不连续操作FLASH*/
			sState = 100;
			break;

		/*获取运行数据存储一条记录*/
		case 3:
			/*当天无记录但写指针非起始位置,需清理FLAH*/
			if((0 == sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]])
				&& ((sDatRcdStorState[eRcdStorState_wSecP] > 0) || (sDatRcdStorState[eRcdStorState_wNumP] > 0)))
			{
				/*格式化当天需要的FLASH*/
				RcdResetNowDayStoreReady(2);
				//sState = 30;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 131;
			}
			/*获取运行数据*/
			else if(TRUE == RcdGetGroupRunDataHook(data, RCD_DATINFO_LEN))
			{
				/*将运行数据写存储*/
				RcdCreateDataRcdInfoAPI(data, RCD_DATINFO_LEN);
				//sState = 0;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 100;
			}
			else
			{
				sState = 0;
			}
			break;

		/*获取运行数据存储一条记录*/
		case 30:
			/*获取运行数据*/
			if(TRUE == RcdGetGroupRunDataHook(data, RCD_DATINFO_LEN))
			{
				/*将运行数据写存储*/
				RcdCreateDataRcdInfoAPI(data, RCD_DATINFO_LEN);
			}
			//sState = 0;
			/*增加时间间隔,不连续操作FLASH*/
			sState = 100;
			break;

		/*获取故障录波存储一条记录*/
		case 4:
			/*当天无记录但写指针非起始位置,需清理FLAH*/
			if((0 == sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]])
				&& ((sWavRcdStorState[eRcdStorState_wSecP] > 0) || (sWavRcdStorState[eRcdStorState_wNumP] > 0)))
			{
				/*格式化当天需要的FLASH*/
				RcdResetNowDayStoreReady(3);
				//sState = 40;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 141;
			}
			/*获取录波数据*/
			else if(TRUE == RcdGetFaultWaveInfoAPI(data, RCD_WAVINFO_LEN))
			{
				/*将录波数据写存储*/
				RcdCreateWaveRcdInfoAPI(data, RCD_WAVINFO_LEN);
				//sState = 0;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 100;
			}
			else
			{
				sState = 0;
			}
			break;

		/*获取故障录波存储一条记录*/
		case 40:
			/*获取录波数据*/
			if(TRUE == RcdGetFaultWaveInfoAPI(data, RCD_WAVINFO_LEN))
			{
				/*将录波数据写存储*/
				RcdCreateWaveRcdInfoAPI(data, RCD_WAVINFO_LEN);
			}
			//sState = 0;
			/*增加时间间隔,不连续操作FLASH*/
			sState = 100;
			break;

		/*将DOD记录缓存区记录存储*/
		case 5:
			/*尚未记录有DOD记录,需清理FLAH*/
			if(0 == sDodRcdStorState[eRcdStorState_allNum])
			{
				/*格式化所有需要的FLASH*/
				RcdResetNowDayStoreReady(4);
				//sState = 50;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 151;
			}
			/*缓存区有故障记录*/
			else if(0xffff != sDodRcdBuffInfo.dod)
			{
				/*将记录写存储*/
				RcdStoreDodRcdInfoDeal(&sDodRcdBuffInfo);
				sDodRcdBuffInfo.dod = 0xffff;
				//sState = 0;
				/*增加时间间隔,不连续操作FLASH*/
				sState = 100;
			}
			else
			{
				sState = 0;
			}
			break;

		/*将DOD记录缓存区记录存储*/
		case 50:
			/*缓存区DOD记录有效*/
			if(0xffff != sDodRcdBuffInfo.dod)
			{
				/*将记录写存储*/
				RcdStoreDodRcdInfoDeal(&sDodRcdBuffInfo);
				sDodRcdBuffInfo.dod = 0xffff;
			}
			//sState = 0;
			/*增加时间间隔,不连续操作FLASH*/
			sState = 100;
			break;

		/*FLASH读写后时间间隔500ms*/
		case 100:
			sState = 0;
			break;

		/*擦除故障记录FLASH(分次擦除)*/
		case 110:
		case 111:
			/*使能擦除当天所有FLASH扇区*/
			#if(1 == RCD_CLR_ALLFLASH_EN)
			/*擦除当天所有FLASH扇区*/
			if(TRUE == RcdEraseOneDayStoreReady(0, 1))
			{
				/*内部事件直接写FLASH防止丢失*/
				RcdStoreFlashClrEventRcdAPI(5);

				/*擦新的一天FLASH,返回重新检测*/
				if(110 == sState)
				{
					sState = 0;
				}
				/*擦当天FLASH,返回写记录*/
				else
				{
					sState = 10;
				}
			}
			#else
			/*擦新的一天FLASH,返回重新检测*/
			if(110 == sState)
			{
				sState = 0;
			}
			/*擦当天FLASH,返回写记录*/
			else
			{
				sState = 10;
			}
			#endif
			break;

		/*擦除事件记录FLASH(分次擦除)*/
		case 120:
		case 121:
			/*使能擦除当天所有FLASH扇区*/
			#if(1 == RCD_CLR_ALLFLASH_EN)
			/*擦新当天所有FLASH扇区*/
			if(TRUE == RcdEraseOneDayStoreReady(1, 1))
			{
				/*内部事件直接写FLASH防止丢失*/
				RcdStoreFlashClrEventRcdAPI(6);

				/*擦新的一天FLASH,返回重新检测*/
				if(120 == sState)
				{
					sState = 0;
				}
				/*擦当天FLASH,返回写记录*/
				else
				{
					sState = 20;
				}
			}
			#else
			/*擦新的一天FLASH,返回重新检测*/
			if(120 == sState)
			{
				sState = 0;
			}
			/*擦当天FLASH,返回写记录*/
			else
			{
				sState = 20;
			}
			#endif
			break;

		/*擦除运行数据FLASH(分次擦除)*/
		case 130:
		case 131:
			/*使能擦除当天所有FLASH扇区*/
			#if(1 == RCD_CLR_ALLFLASH_EN)
			/*擦新当天所有FLASH扇区*/
			if(TRUE == RcdEraseOneDayStoreReady(2, 1))
			{
				/*内部事件直接写FLASH防止丢失*/
				RcdStoreFlashClrEventRcdAPI(7);

				/*擦新的一天FLASH,返回重新检测*/
				if(130 == sState)
				{
					sState = 0;
				}
				/*擦当天FLASH,返回写记录*/
				else
				{
					sState = 30;
				}
			}
			#else
			/*擦新的一天FLASH,返回重新检测*/
			if(130 == sState)
			{
				sState = 0;
			}
			/*擦当天FLASH,返回写记录*/
			else
			{
				sState = 30;
			}
			#endif
			break;

		/*擦除故障录波FLASH后(分次擦除)*/
		case 140:
		case 141:
			/*使能擦除当天所有FLASH扇区*/
			#if(1 == RCD_CLR_ALLFLASH_EN)
			/*擦新当天所有FLASH扇区*/
			if(TRUE == RcdEraseOneDayStoreReady(3, 1))
			{
				/*内部事件直接写FLASH防止丢失*/
				RcdStoreFlashClrEventRcdAPI(8);

				/*擦新的一天FLASH,返回重新检测*/
				if(140 == sState)
				{
					sState = 0;
				}
				/*擦当天FLASH,返回写记录*/
				else
				{
					sState = 40;
				}
			}
			#else
			/*擦新的一天FLASH,返回重新检测*/
			if(140 == sState)
			{
				sState = 0;
			}
			/*擦当天FLASH,返回写记录*/
			else
			{
				sState = 40;
			}
			#endif
			break;

		/*擦除历史DOD记录FLASH(分次擦除)*/
		case 151:
			/*使能擦除所有FLASH扇区*/
			#if(1 == RCD_CLR_ALLFLASH_EN)
			/*擦完所有FLASH扇区,返回写记录*/
			if(TRUE == RcdEraseOneDayStoreReady(4, 1))
			{
				/*内部事件直接写FLASH防止丢失*/
				RcdStoreFlashClrEventRcdAPI(9);

				sState = 50;
			}
			#else
			/*擦完FLASH,返回写记录*/
			sState = 50;
			#endif
			break;

		default:
			sState = 0;
			break;
	}
}

/*=================================================================================================
** @Name      : void RcdCreateErrRcdInfo_API(t_ RcdCreateInfo *rcdInfo)
** @Input     : rcdInfo:记录信息
** @Output    : void
** @Function  : 创建一条故障记录
** @The notes : 用户需要产生一条记录时调用(写队列缓存)
**===============================================================================================*/
void RcdCreateErrRcdInfoAPI(t_RcdCreateInfo *rcdInfo)
{
	t_SysClock sysClock;

	/*故障记录未使能*/
	if(0 == gRcdPara.errRcdEn)
	{
		return;
	}

	/*故障记录放入缓存区*/
	sErrRcdBuff[sErrRcdBuffState.wPoint] = sErrRcdBuffState.idNum;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 1] = ((u16)RcdGetGroupDevAddrHook() << 8) | rcdInfo->type;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 2] = rcdInfo->code;

	/*故障时间信息*/
	sysClock = GetSysDateClockAPI();
	sErrRcdBuff[sErrRcdBuffState.wPoint + 3] = sysClock.year;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 4] = ((u16)sysClock.month << 8) | sysClock.day;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 5] = ((u16)sysClock.hour << 8) | sysClock.minute;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 6] = sysClock.second;

	/*故障附加信息*/
	sErrRcdBuff[sErrRcdBuffState.wPoint + 7] = rcdInfo->info1;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 8] = rcdInfo->info2;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 9] = rcdInfo->info3;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 10] = rcdInfo->info4;
	sErrRcdBuff[sErrRcdBuffState.wPoint + 11] = rcdInfo->info5;

	/*序号数目增1*/
	sErrRcdBuffState.idNum++;
	/*写指针下移*/
	sErrRcdBuffState.wPoint += RCD_ERRINFO_LEN;

	/*写指针到最后*/
	if(sErrRcdBuffState.wPoint >= RCD_ERRBUFF_LEN)
	{
		/*写指针回头*/
		sErrRcdBuffState.wPoint = 0;
	}

	/*队列写满*/
	if(sErrRcdBuffState.rcdNum >= RCD_ERRBUFF_NUM)
	{
		/*读指针下移到最早一条(将要被写入位置)*/
		sErrRcdBuffState.rPoint = sErrRcdBuffState.wPoint;
	}
	/*队列未写满*/
	else
	{
		/*队列中记录条数增1*/
		sErrRcdBuffState.rcdNum++;
	}
}

/*=================================================================================================
** @Name      : void RcdCreateErrRcdInfo_API(t_ RcdCreateInfo *rcdInfo)
** @Input     : rcdInfo:记录信息
** @Output    : void
** @Function  : 创建一条事件记录
** @The notes : 用户需要产生一条记录时调用(写队列缓存)
**===============================================================================================*/
void RcdCreateEventRcdInfoAPI(t_RcdCreateInfo *rcdInfo)
{
	t_SysClock sysClock;

	/*故障记录未使能*/
	if(0 == gRcdPara.evnRcdEn)
	{
		return;
	}

	/*队列写满,防止刚上电最早的事件被覆盖*/
	if((sEvnRcdBuffState.wPoint >= (RCD_EVNBUFF_NUM - 1))
		&& ((/*eWORK_INIT == RcdGetGroupWorkStateHook())
		|| ((eWORK_START != RcdGetGroupWorkStateHook())
		&& */(BSPGetOSMillSecTimeAPI() <= 120000))))
	{
		/*不记录*/
		return;
	}

	/*事件记录放入缓存区*/
	sEvnRcdBuff[sEvnRcdBuffState.wPoint] = sEvnRcdBuffState.idNum;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 1] = ((u16)RcdGetGroupDevAddrHook() << 8) | rcdInfo->type;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 2] = rcdInfo->code;

	/*事件时间信息*/
	sysClock = GetSysDateClockAPI();
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 3] = sysClock.year;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 4] = ((u16)sysClock.month << 8) | sysClock.day;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 5] = ((u16)sysClock.hour << 8) | sysClock.minute;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 6] = sysClock.second;

	/*事件附加信息*/
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 7] = rcdInfo->info1;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 8] = rcdInfo->info2;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 9] = rcdInfo->info3;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 10] = rcdInfo->info4;
	sEvnRcdBuff[sEvnRcdBuffState.wPoint + 11] = rcdInfo->info5;

	/*序号数目增1*/
	sEvnRcdBuffState.idNum++;
	/*写指针下移*/
	sEvnRcdBuffState.wPoint += RCD_EVNINFO_LEN;

	/*写指针到最后*/
	if(sEvnRcdBuffState.wPoint >= RCD_EVNBUFF_LEN)
	{
		/*写指针回头*/
		sEvnRcdBuffState.wPoint = 0;
	}

	/*队列写满*/
	if(sEvnRcdBuffState.rcdNum >= RCD_EVNBUFF_NUM)
	{
		/*读指针下移到最早一条(将要被写入位置)*/
		sEvnRcdBuffState.rPoint = sEvnRcdBuffState.wPoint;
	}
	/*队列未写满*/
	else
	{
		/*队列中记录条数增1*/
		sEvnRcdBuffState.rcdNum++;
	}
}

/*=================================================================================================
** @Name      : void RcdCreateDataRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:记录数据信息 len:记录长度
** @Output    : void
** @Function  : 创建一条运行数据记录
** @The notes : 用户需要产生一条记录时调用(直接写FLASH存储)
**===============================================================================================*/
void RcdCreateDataRcdInfoAPI(u16 *data, u8 len)
{
	u32 regAddr = 0;
	static u8 sFlag = 0;

	/*数据记录未使能和写就绪*/
	if((0 == gRcdPara.dataStoEn) || (sDatRcdStorReadBusyFlag > 0))
	{
		return;
	}

	/*参数错误*/
	if((data == NULL) || (len > RCD_DATINFO_LEN))
	{
		return;
	}

	/*计算当前写地址,将信息写EEPROM或FLASH*/
	regAddr = RcdCalcRecordAimAddr(2, sDatRcdStorState[eRcdStorState_wDayP], sDatRcdStorState[eRcdStorState_wSecP], sDatRcdStorState[eRcdStorState_wNumP]);

	/*刚上电扇区起始未擦除(写记录但未写状态时重新上电)*/
	if((0 == sDatRcdStorState[eRcdStorState_wNumP]) && (0 == sFlag))
	{
		/*擦除本扇区FLASH*/
		(void)RcdEraseRcdStoreSectorHook(2, regAddr);
		sFlag = 1;
	}

	/*将信息写EEPROM或FLASH*/
	(void)RcdWriteRcdStoreInfoHook(2, regAddr, data, len);

	/*记录数目增加*/
	sDatRcdStorState[eRcdStorState_eDayNum + sDatRcdStorState[eRcdStorState_wDayP]]++;
	sDatRcdStorState[eRcdStorState_allNum]++;

	/*未计算记录天数*/
	if(0 == sDatRcdStorState[eRcdStorState_allDay])
	{
		/*记录天数累加*/
		sDatRcdStorState[eRcdStorState_allDay]++;
	}

	/*正在读当天数目*/
	if(sDatRcdStorState[eRcdStorState_rDayP] == sDatRcdStorState[eRcdStorState_wDayP])
	{
		/*读剩余数目+1*/
		sDatRcdStorState[eRcdStorState_remNum]++;
	}

	/*写指针下移一个*/
	sDatRcdStorState[eRcdStorState_wNumP]++;
	/*写满1个扇区*/
	if(sDatRcdStorState[eRcdStorState_wNumP] >= RCD_DATWD_SECNUM)
	{
		/*写下一个扇区*/
		sDatRcdStorState[eRcdStorState_wNumP] = 0;
		sDatRcdStorState[eRcdStorState_wSecP]++;

		/*写到1天扇区末尾*/
		if(sDatRcdStorState[eRcdStorState_wSecP] >= RCD_DATWD_DSEC_NUM)
		{
			/*写指针返回起始扇区*/
			sDatRcdStorState[eRcdStorState_wSecP] = 0;

			/*当天记录起始扇区下移1个扇区*/
			sDatRcdStorState[eRcdStorState_dayFSec + (u8)sDatRcdStorState[eRcdStorState_wDayP]] = 1;

			/*数目减少一个扇区数目*/
			sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]] -= RCD_DATWD_SECNUM;
			sDatRcdStorState[eRcdStorState_allNum] -= RCD_DATWD_SECNUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(2, sDatRcdStorState[eRcdStorState_wDayP], sDatRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(2, regAddr);
		}
		/*写满1天记录需要覆盖已写记录*/
		else if(sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]] >= RCD_DATWD_DAYNUM)
		{
			/*当天记录起始扇区下移1个扇区*/
			sDatRcdStorState[eRcdStorState_dayFSec + (u8)sDatRcdStorState[eRcdStorState_wDayP]] = sDatRcdStorState[eRcdStorState_wSecP] + 1;

			/*数目减少一个扇区数目*/
			sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]] -= RCD_DATWD_SECNUM;
			sDatRcdStorState[eRcdStorState_allNum] -= RCD_DATWD_SECNUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(2, sDatRcdStorState[eRcdStorState_wDayP], sDatRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(2, regAddr);
		}
		/*未使能擦除所有FLASH扇区(每个扇区都要先擦除再写)*/
		#if(0 == RCD_CLR_ALLFLASH_EN)
		/*扇区未用完，进入下一个扇区*/
		else
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(2, sDatRcdStorState[eRcdStorState_wDayP], sDatRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(2, regAddr);
		}
		#endif

		/*写指针下移1个扇区后写正在读取的位置*/
		if((sDatRcdStorState[eRcdStorState_wDayP] == sDatRcdStorState[eRcdStorState_rDayP])
			&& (sDatRcdStorState[eRcdStorState_wSecP] == sDatRcdStorState[eRcdStorState_rSecP]))
		{
			/*读下移一个扇区*/
			sDatRcdStorState[eRcdStorState_rSecP]++;

			/*读到最后一个扇区*/
			if(sDatRcdStorState[eRcdStorState_rSecP] >= RCD_DATWD_DSEC_NUM)
			{
				/*返回起始扇区*/
				sDatRcdStorState[eRcdStorState_rSecP] = 0;
			}

			/*当天读剩余数目减少该扇区未读取的*/
			sDatRcdStorState[eRcdStorState_remNum] -= (RCD_DATWD_SECNUM - sDatRcdStorState[eRcdStorState_rNumP]);

			/*读起始位置*/
			sDatRcdStorState[eRcdStorState_rNumP] = 0;
		}

		/*写满一个扇区需要更新记录状态信息(写满一个扇区更新一次,防止擦写次数过多)*/
		RcdWriteRecodStoreStateInfo(2);
		sFlag = 2;
	}

	/*正在显示当天数目*/
	if(sDatRcdDispState.dDPoint == sDatRcdStorState[eRcdStorState_wDayP])
	{
		/*更新当天显示记录为当天数目(重新显示)*/
		sDatRcdDispState.dDaNum = sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]];
	}

	/*增加一条记录重新记录数据记录状态信息(会导致擦写次数过多)*/
	//RcdWriteRecodStoreStateInfo(2);
	/*未切换到下一个扇区(确保刚上电的数据被记录下来)*/
	if(2 != sFlag)
	{
		/*刚上电所写第一个扇区逐条记录(确保刚上电的数据被记录下来)*/
		RcdWriteRecodStoreStateInfo(2);
	}
}

/*=================================================================================================
** @Name      : void RcdCreateWaveRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:记录数据信息 len:记录长度
** @Output    : void
** @Function  : 创建一条故障录波记录
** @The notes : 用户需要产生一条记录时调用(直接写FLASH存储)
**===============================================================================================*/
void RcdCreateWaveRcdInfoAPI(u16 *data, u8 len)
{
	u32 regAddr = 0;

	/*数据记录未使能和写就绪*/
	if((0 == gRcdPara.wavStoEn) || (sWavRcdStorReadBusyFlag > 0))
	{
		return;
	}

	/*参数错误*/
	if((data == NULL) || (len > RCD_WAVINFO_LEN))
	{
		return;
	}

	/*计算当前写地址,将信息写EEPROM或FLASH*/
	regAddr = RcdCalcRecordAimAddr(3, sWavRcdStorState[eRcdStorState_wDayP], sWavRcdStorState[eRcdStorState_wSecP], sWavRcdStorState[eRcdStorState_wNumP]);
	(void)RcdWriteRcdStoreInfoHook(3, regAddr, data, len);

	/*记录数目增加*/
	sWavRcdStorState[eRcdStorState_eDayNum + sWavRcdStorState[eRcdStorState_wDayP]]++;
	sWavRcdStorState[eRcdStorState_allNum]++;

	/*未计算记录天数*/
	if(0 == sWavRcdStorState[eRcdStorState_allDay])
	{
    	/*记录天数累加*/
		sWavRcdStorState[eRcdStorState_allDay]++;
	}

	/*正在读当天数目*/
	if(sWavRcdStorState[eRcdStorState_rDayP] == sWavRcdStorState[eRcdStorState_wDayP])
	{
		/*读剩余数目+1*/
		sWavRcdStorState[eRcdStorState_remNum]++;
	}

	/*写指针下移一个*/
	sWavRcdStorState[eRcdStorState_wNumP]++;
	/*写满1个扇区*/
	if(sWavRcdStorState[eRcdStorState_wNumP] >= RCD_WAVWD_SECNUM)
	{
		/*写下一个扇区*/
		sWavRcdStorState[eRcdStorState_wNumP] = 0;
		sWavRcdStorState[eRcdStorState_wSecP]++;

		/*写到1天扇区末尾*/
		if(sWavRcdStorState[eRcdStorState_wSecP] >= RCD_WAVWD_DSEC_NUM)
		{
			/*写指针返回起始扇区*/
			sWavRcdStorState[eRcdStorState_wSecP] = 0;

			/*当天记录起始扇区下移1个扇区*/
			sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_wDayP]] = 1;

			/*数目减少一个扇区数目*/
			sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]] -= RCD_WAVWD_SECNUM;
			sWavRcdStorState[eRcdStorState_allNum] -= RCD_WAVWD_SECNUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(3, sWavRcdStorState[eRcdStorState_wDayP], sWavRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(3, regAddr);
		}
		/*写满1天记录需要覆盖已写记录*/
		else if(sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]] >= RCD_WAVWD_DAYNUM)
		{
			/*当天记录起始扇区下移1个扇区*/
			sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_wDayP]] = sWavRcdStorState[eRcdStorState_wSecP] + 1;

			/*数目减少一个扇区数目*/
			sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]] -= RCD_WAVWD_SECNUM;
			sWavRcdStorState[eRcdStorState_allNum] -= RCD_WAVWD_SECNUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(3, sWavRcdStorState[eRcdStorState_wDayP], sWavRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(3, regAddr);
		}
		/*未使能擦除所有FLASH扇区(每个扇区都要先擦除再写)*/
		#if(0 == RCD_CLR_ALLFLASH_EN)
		/*扇区未用完，进入下一个扇区*/
		else
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(3, sWavRcdStorState[eRcdStorState_wDayP], sWavRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(3, regAddr);
		}
		#endif

		/*写指针下移1个扇区后写正在读取的位置*/
		if((sWavRcdStorState[eRcdStorState_wDayP] == sWavRcdStorState[eRcdStorState_rDayP])
			&& (sWavRcdStorState[eRcdStorState_wSecP] == sWavRcdStorState[eRcdStorState_rSecP]))
		{
			/*读下移一个扇区*/
			sWavRcdStorState[eRcdStorState_rSecP]++;

			/*读到最后一个扇区*/
			if(sWavRcdStorState[eRcdStorState_rSecP] >= RCD_WAVWD_DSEC_NUM)
			{
				/*返回起始扇区*/
				sWavRcdStorState[eRcdStorState_rSecP] = 0;
			}

			/*当天读剩余数目减少该扇区未读取的*/
			sWavRcdStorState[eRcdStorState_remNum] -= (RCD_WAVWD_SECNUM - sWavRcdStorState[eRcdStorState_rNumP]);

			/*读起始位置*/
			sWavRcdStorState[eRcdStorState_rNumP] = 0;
		}
	}

	/*正在显示当天数目*/
	if(sWavRcdDispState.dDPoint == sWavRcdStorState[eRcdStorState_wDayP])
	{
		/*更新当天显示记录为当天数目(重新显示)*/
		sWavRcdDispState.dDaNum = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]];
	}

	/*重新记录录波记录状态信息*/
	RcdWriteRecodStoreStateInfo(3);
}

/*=================================================================================================
** @Name      : void RcdCreateDodRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:需记录的DOD值/充电量/放电量/充电能/放电能 len:记录信息个数(5个有效)
** @Output    : void
** @Function  : 创建一条DOD信息记录
** @The notes : 用户需要产生一条记录时调用(记录一条在缓存区)
**===============================================================================================*/
void RcdCreateDodRcdInfoAPI(u16 *data, u8 len)
{
	t_SysClock sysClock;

	//输入信息正确
	if((NULL != data) && (5 == len))
	{
		/*序号*/
		sDodRcdBuffInfo.id++;

		/*DOD记录时间信息*/
		sysClock = GetSysDateClockAPI();
		sDodRcdBuffInfo.year = sysClock.year;
		sDodRcdBuffInfo.MD = ((u16)sysClock.month << 8) | sysClock.day;
		sDodRcdBuffInfo.HM = ((u16)sysClock.hour << 8) | sysClock.minute;
		sDodRcdBuffInfo.sec = sysClock.second;

		/*DOD记录值信息*/
		sDodRcdBuffInfo.dod = data[0];
		sDodRcdBuffInfo.chgC = data[1];
		sDodRcdBuffInfo.dhgC = data[2];
		sDodRcdBuffInfo.chgE = data[3];
		sDodRcdBuffInfo.dhgE = data[4];
	}
}

/*=================================================================================================
** @Name      : u8 RcdReadOneErrRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的故障记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条故障记录
** @The notes : 从最早记录开始读取存储区或缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneErrRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info)
{
	u16 i = 0;
	u32 regAddr = 0;
	u16 rdInfo[RCD_ERRINFO_LEN] = {0};

	/*故障记录未使能*/
	if(0 == gRcdPara.errRcdEn)
	{
		return(FALSE);
	}

	/*输入参数错误*/
	if((conFlag > 1) || (info == NULL))
	{
		return(FALSE);
	}

	/*故障记录存储使能则从存储区读取*/
	if(1 == gRcdPara.errStoEn)
	{
		/*总剩余条数为0则停止读取*/
		if(0 == sErrRcdStorState[eRcdStorState_allNum])
		{
			/*读指针与写指针同步*/
			sErrRcdStorState[eRcdStorState_remNum] = 0;
			sErrRcdStorState[eRcdStorState_rDayP] = sErrRcdStorState[eRcdStorState_wDayP];
			sErrRcdStorState[eRcdStorState_rSecP] = sErrRcdStorState[eRcdStorState_wSecP];
			sErrRcdStorState[eRcdStorState_rNumP] = sErrRcdStorState[eRcdStorState_wNumP];
			return(FALSE);
		}

		/*读取当天数目为零*/
		if(0 == sErrRcdStorState[eRcdStorState_remNum])
		{
			/*当前读日期不是当前写日期(可读下一天)*/
			if(sErrRcdStorState[eRcdStorState_rDayP] != sErrRcdStorState[eRcdStorState_wDayP])
			{
				/*从下一天开始找出31天中记录数目不为0的日期*/
				for(i = 0; i < sErrRcdStorState[eRcdStorState_allDay]; i++)
				{
					sErrRcdStorState[eRcdStorState_rDayP]++;
					if(sErrRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
					{
						sErrRcdStorState[eRcdStorState_rDayP] = 0;
					}

					/*下一天数目不为0*/
					if(sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_rDayP]] > 0)
					{
						/*获取下一天记录数目*/
						sErrRcdStorState[eRcdStorState_remNum] = sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
						/*获取下一天起始扇区*/
						sErrRcdStorState[eRcdStorState_rSecP] = sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
						sErrRcdStorState[eRcdStorState_rNumP] = 0;
						/*停止查找*/
						break;
					}
				}
			}

			/*未找到可读日期*/
			if(0 == sErrRcdStorState[eRcdStorState_remNum])
			{
				/*可读数目为0,读指针与写指针同步*/
				sErrRcdStorState[eRcdStorState_allNum] = 0;
				sErrRcdStorState[eRcdStorState_rDayP] = sErrRcdStorState[eRcdStorState_wDayP];
				sErrRcdStorState[eRcdStorState_rSecP] = sErrRcdStorState[eRcdStorState_wSecP];
				sErrRcdStorState[eRcdStorState_rNumP] = sErrRcdStorState[eRcdStorState_wNumP];
				return(FALSE);
			}
		}

		/*读指针与写指针一致则读完成*/
		if((sErrRcdStorState[eRcdStorState_rDayP] == sErrRcdStorState[eRcdStorState_wDayP])
			&& (sErrRcdStorState[eRcdStorState_rSecP] == sErrRcdStorState[eRcdStorState_wSecP])
			&& (sErrRcdStorState[eRcdStorState_rNumP] == sErrRcdStorState[eRcdStorState_wNumP]))
		{
			/*无可读记录*/
			sErrRcdStorState[eRcdStorState_allNum] = 0;
			sErrRcdStorState[eRcdStorState_remNum] = 0;
			return(FALSE);
		}

		/*开始读记录*/
		sErrRcdStorReadBusyFlag = 1;

		/*计算当前读地址,从存储区读取一条记录*/
		regAddr = RcdCalcRecordAimAddr(0, sErrRcdStorState[eRcdStorState_rDayP], sErrRcdStorState[eRcdStorState_rSecP], sErrRcdStorState[eRcdStorState_rNumP]);

		/*读取成功*/
		if(TRUE == RcdReadRcdStoreInfoHook(0, regAddr, rdInfo, RCD_ERRINFO_LEN))
		{
			/*读取到记录信息*/
			info->id = rdInfo[0];
			info->codeH = rdInfo[1];
			info->codeL = rdInfo[2];
			info->year = rdInfo[3];
			info->MD = rdInfo[4];
			info->HM = rdInfo[5];
			info->sec = rdInfo[6];
			info->info1 = rdInfo[7];
			info->info2 = rdInfo[8];
			info->info3 = rdInfo[9];
			info->info4 = rdInfo[10];
			info->info5 = rdInfo[11];

			sErrRcdStorState[eRcdStorState_allNum]--;
			sErrRcdStorState[eRcdStorState_remNum]--;

			/*读完全部记录*/
			if(0 == sErrRcdStorState[eRcdStorState_allNum])
			{
				/*读指针与写指针同步*/
				sErrRcdStorState[eRcdStorState_remNum] = 0;
				sErrRcdStorState[eRcdStorState_rDayP] = sErrRcdStorState[eRcdStorState_wDayP];
				sErrRcdStorState[eRcdStorState_rSecP] = sErrRcdStorState[eRcdStorState_wSecP];
				sErrRcdStorState[eRcdStorState_rNumP] = sErrRcdStorState[eRcdStorState_wNumP];
			}
			/*当天还有记录*/
			else if(sErrRcdStorState[eRcdStorState_remNum] > 0)
			{
				sErrRcdStorState[eRcdStorState_rNumP]++;
				/*读完一个扇区*/
				if(sErrRcdStorState[eRcdStorState_rNumP] >= RCD_ERRWD_SECNUM)
				{
					/*下一个扇区*/
					sErrRcdStorState[eRcdStorState_rNumP] = 0;
					sErrRcdStorState[eRcdStorState_rSecP]++;
					/*读到一天的扇区末尾*/
					if(sErrRcdStorState[eRcdStorState_rSecP] >= RCD_ERRWD_DSEC_NUM)
					{
						/*扇区指针回头*/
						sErrRcdStorState[eRcdStorState_rSecP] = 0;
					}
				}
			}
			/*当天数目为0*/
			else
			{
				/*当前读日期不是当前写日期(可读下一天)*/
				if(sErrRcdStorState[eRcdStorState_rDayP] != sErrRcdStorState[eRcdStorState_wDayP])
				{
					/*查找有可读记录的下一天*/
					for(i = 0; i < sErrRcdStorState[eRcdStorState_allDay]; i++)
					{
						/*下一天*/
						sErrRcdStorState[eRcdStorState_rDayP]++;

						/*读完最末一天*/
						if(sErrRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
						{
							/*天指针回头*/
							sErrRcdStorState[eRcdStorState_rDayP] = 0;
						}

						/*获取下一天起始数目*/
						sErrRcdStorState[eRcdStorState_remNum] = sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
						/*获取下一天起始扇区*/
						sErrRcdStorState[eRcdStorState_rSecP] = sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
						sErrRcdStorState[eRcdStorState_rNumP] = 0;

						/*该天有可读记录*/
						if(sErrRcdStorState[eRcdStorState_remNum] > 0)
						{
							/*停止查找*/
							break;
						}
					}
				}

				/*未找到可读记录的下一天*/
				if(0 == sErrRcdStorState[eRcdStorState_remNum])
				{
					/*无剩余可读记录,读指针与写指针同步*/
					sErrRcdStorState[eRcdStorState_allNum] = 0;
					sErrRcdStorState[eRcdStorState_rDayP] = sErrRcdStorState[eRcdStorState_wDayP];
					sErrRcdStorState[eRcdStorState_rSecP] = sErrRcdStorState[eRcdStorState_wSecP];
					sErrRcdStorState[eRcdStorState_rNumP] = sErrRcdStorState[eRcdStorState_wNumP];
				}
			}

			/*读指针与写指针一致则读完成*/
			if((sErrRcdStorState[eRcdStorState_rDayP] == sErrRcdStorState[eRcdStorState_wDayP])
				&& (sErrRcdStorState[eRcdStorState_rSecP] == sErrRcdStorState[eRcdStorState_wSecP])
				&& (sErrRcdStorState[eRcdStorState_rNumP] == sErrRcdStorState[eRcdStorState_wNumP]))
			{
				/*无可读记录*/
				sErrRcdStorState[eRcdStorState_allNum] = 0;
				sErrRcdStorState[eRcdStorState_remNum] = 0;
			}

			/*确认读取才记录更新的*/
			if(1 == conFlag)
			{
				/*每读完一个扇区则重写一次记录状态信息*/
				RcdWriteRecodStoreStateInfo(0);
			}

			return(TRUE);
		}
		/*读存储区失败*/
		else
		{
			return(FALSE);
		}
	}
	/*未故障记录存储则从缓存区读取*/
	else
	{
		/*缓存区无记录*/
		if(0 == sErrRcdBuffState.rcdNum)
		{
			return(FALSE);
		}

		/*从缓存区读一条记录*/
		return(RcdReadOneErrQueAPI(info));
	}
}

/*=================================================================================================
** @Name      : u8 RcdReadOneEventRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条事件记录
** @The notes : 从最早记录开始读取存储区或缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneEventRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info)
{
	u8 i = 0;
	u32 regAddr = 0;
	u16 rdInfo[RCD_EVNINFO_LEN] = {0};

	/*故障记录未使能*/
	if(0 == gRcdPara.evnRcdEn)
	{
		return(FALSE);
	}

	/*输入参数错误*/
	if((conFlag > 1) || (info == NULL))
	{
		return(FALSE);
	}

	/*故障记录存储使能则从存储区读取*/
	if(1 == gRcdPara.evnStoEn)
	{
		/*总剩余条数为0则停止读取*/
		if(0 == sEvnRcdStorState[eRcdStorState_allNum])
		{
			/*读指针与写指针同步*/
			sEvnRcdStorState[eRcdStorState_remNum] = 0;
			sEvnRcdStorState[eRcdStorState_rDayP] = sEvnRcdStorState[eRcdStorState_wDayP];
			sEvnRcdStorState[eRcdStorState_rSecP] = sEvnRcdStorState[eRcdStorState_wSecP];
			sEvnRcdStorState[eRcdStorState_rNumP] = sEvnRcdStorState[eRcdStorState_wNumP];
			return(FALSE);
		}

		/*当前读当天无剩余记录可读*/
		if(0 == sEvnRcdStorState[eRcdStorState_remNum])
		{
			/*当前读日期不是当前写日期(可读下一天)*/
			if(sEvnRcdStorState[eRcdStorState_rDayP] != sEvnRcdStorState[eRcdStorState_wDayP])
			{
				/*查找有可读记录的下一天*/
				for(i = 0; i < sEvnRcdStorState[eRcdStorState_allDay]; i++)
				{
					/*下一天*/
					sEvnRcdStorState[eRcdStorState_rDayP]++;

					/*读完最末一天*/
					if(sEvnRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
					{
						sEvnRcdStorState[eRcdStorState_rDayP] = 0;
					}

					/*该天有可读记录*/
					if(sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_rDayP]] > 0)
					{
						/*记录下一天当天剩余数目和读起始扇区*/
						sEvnRcdStorState[eRcdStorState_remNum] = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
						sEvnRcdStorState[eRcdStorState_rSecP] = sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
						sEvnRcdStorState[eRcdStorState_rNumP] = 0;
						/*停止查找*/
						break;
					}
				}
			}

			/*未找到有可读记录*/
			if(0 == sEvnRcdStorState[eRcdStorState_remNum])
			{
				/*可读数目为0,读指针与写指针同步*/
				sEvnRcdStorState[eRcdStorState_allNum] = 0;
				sEvnRcdStorState[eRcdStorState_rDayP] = sEvnRcdStorState[eRcdStorState_wDayP];
				sEvnRcdStorState[eRcdStorState_rSecP] = sEvnRcdStorState[eRcdStorState_wSecP];
				sEvnRcdStorState[eRcdStorState_rNumP] = sEvnRcdStorState[eRcdStorState_wNumP];
				return(FALSE);
			}
		}

		/*读指针与写指针一致则读完成*/
		if((sEvnRcdStorState[eRcdStorState_rDayP] == sEvnRcdStorState[eRcdStorState_wDayP])
			&& (sEvnRcdStorState[eRcdStorState_rSecP] == sEvnRcdStorState[eRcdStorState_wSecP])
			&& (sEvnRcdStorState[eRcdStorState_rNumP] == sEvnRcdStorState[eRcdStorState_wNumP]))
		{
			/*无可读记录*/
			sEvnRcdStorState[eRcdStorState_allNum] = 0;
			sEvnRcdStorState[eRcdStorState_remNum] = 0;
			return(FALSE);
		}

		/*开始读记录*/
		sEvnRcdStorReadBusyFlag = 1;

		/*计算当前读地址,从存储区读取一条记录*/
		regAddr = RcdCalcRecordAimAddr(1, sEvnRcdStorState[eRcdStorState_rDayP], sEvnRcdStorState[eRcdStorState_rSecP], sEvnRcdStorState[eRcdStorState_rNumP]);

		/*读取成功*/
		if(TRUE == RcdReadRcdStoreInfoHook(1, regAddr, rdInfo, RCD_EVNINFO_LEN))
		{
			/*读取到记录信息*/
			info->id = rdInfo[0];
			info->codeH = rdInfo[1];
			info->codeL = rdInfo[2];
			info->year = rdInfo[3];
			info->MD = rdInfo[4];
			info->HM = rdInfo[5];
			info->sec = rdInfo[6];
			info->info1 = rdInfo[7];
			info->info2 = rdInfo[8];
			info->info3 = rdInfo[9];
			info->info4 = rdInfo[10];
			info->info5 = rdInfo[11];

			sEvnRcdStorState[eRcdStorState_allNum]--;
			sEvnRcdStorState[eRcdStorState_remNum]--;

			/*读完全部记录*/
			if(0 == sEvnRcdStorState[eRcdStorState_allNum])
			{
				/*读指针与写指针同步*/
				sEvnRcdStorState[eRcdStorState_remNum] = 0;
				sEvnRcdStorState[eRcdStorState_rDayP] = sEvnRcdStorState[eRcdStorState_wDayP];
				sEvnRcdStorState[eRcdStorState_rSecP] = sEvnRcdStorState[eRcdStorState_wSecP];
				sEvnRcdStorState[eRcdStorState_rNumP] = sEvnRcdStorState[eRcdStorState_wNumP];
			}
			/*读当天还有记录可读*/
			else if(sEvnRcdStorState[eRcdStorState_remNum] > 0)
			{
				/*读指针下移*/
				sEvnRcdStorState[eRcdStorState_rNumP]++;

				/*读完一个扇区*/
				if(sEvnRcdStorState[eRcdStorState_rNumP] >= RCD_EVNWD_SECNUM)
				{
					/*下一个扇区*/
					sEvnRcdStorState[eRcdStorState_rNumP] = 0;
					sEvnRcdStorState[eRcdStorState_rSecP]++;

					/*读到一天末尾扇区*/
					if(sEvnRcdStorState[eRcdStorState_rSecP] >= RCD_EVNWD_DSEC_NUM)
					{
						/*返回一天首个扇区*/
						sEvnRcdStorState[eRcdStorState_rSecP] = 0;
					}
				}
			}
			/*读完一天的记录*/
			else
			{
				/*当前读日期不是当前写日期(可读下一天)*/
				if(sEvnRcdStorState[eRcdStorState_rDayP] != sEvnRcdStorState[eRcdStorState_wDayP])
				{
					/*查找有可读记录的下一天*/
					for(i = 0; i < sEvnRcdStorState[eRcdStorState_allDay]; i++)
					{
						/*下一天*/
						sEvnRcdStorState[eRcdStorState_rDayP]++;

						/*读完最末一天*/
						if(sEvnRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
						{
							sEvnRcdStorState[eRcdStorState_rDayP] = 0;
						}

						/*获取下一天当天剩余数目和读起始扇区*/
						sEvnRcdStorState[eRcdStorState_remNum] = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
						sEvnRcdStorState[eRcdStorState_rSecP] = sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
						sEvnRcdStorState[eRcdStorState_rNumP] = 0;

						/*该天有可读记录*/
						if(sEvnRcdStorState[eRcdStorState_remNum] > 0)
						{
							/*停止查找*/
							break;
						}
					}
				}

				/*未找到可读记录的下一天*/
				if(0 == sEvnRcdStorState[eRcdStorState_remNum])
				{
					/*无剩余可读记录,读指针与写指针同步*/
					sEvnRcdStorState[eRcdStorState_allNum] = 0;
					sEvnRcdStorState[eRcdStorState_rDayP] = sEvnRcdStorState[eRcdStorState_wDayP];
					sEvnRcdStorState[eRcdStorState_rSecP] = sEvnRcdStorState[eRcdStorState_wSecP];
					sEvnRcdStorState[eRcdStorState_rNumP] = sEvnRcdStorState[eRcdStorState_wNumP];
				}
			}

			/*读指针与写指针一致则读完成*/
			if((sEvnRcdStorState[eRcdStorState_rDayP] == sEvnRcdStorState[eRcdStorState_wDayP])
				&& (sEvnRcdStorState[eRcdStorState_rSecP] == sEvnRcdStorState[eRcdStorState_wSecP])
				&& (sEvnRcdStorState[eRcdStorState_rNumP] == sEvnRcdStorState[eRcdStorState_wNumP]))
			{
				/*无可读记录*/
				sEvnRcdStorState[eRcdStorState_allNum] = 0;
				sEvnRcdStorState[eRcdStorState_remNum] = 0;
			}

			/*确认读取才记录更新的*/
			if(1 == conFlag)
			{
				/*每读完一天则重写一次记录状态信息*/
				RcdWriteRecodStoreStateInfo(1);
			}

			return(TRUE);
		}
		/*读存储区失败*/
		else
		{
			return(FALSE);
		}
	}
	/*未故障记录存储则从缓存区读取*/
	else
	{
		/*缓存区无记录*/
		if(0 == sEvnRcdBuffState.rcdNum)
		{
			return(FALSE);
		}

		/*从缓存区读一条记录*/
		return(RcdReadOneEventQueAPI(info));
	}
}

/*=================================================================================================
** @Name      : u8 RcdReadOneWaveRcdUserAPI(u8 conFlag, u16 *info, u8 len)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的故障录波记录信息 len:读信息长度
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条故障录波记录
** @The notes : 从最早记录开始读取存储区或缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneWaveRcdUserAPI(u8 conFlag, u16 *info, u8 len)
{
	u8 i = 0;
	u32 regAddr = 0;

	/*故障录波未使能*/
	if(0 == gRcdPara.wavRcdEn)
	{
		return(FALSE);
	}

	/*输入参数错误*/
	if((conFlag > 1) || (info == NULL) || (len > RCD_WAVINFO_LEN))
	{
		return(FALSE);
	}

	/*故障记录存储使能则从存储区读取*/
	if(1 == gRcdPara.wavStoEn)
	{
		/*总剩余条数为0则停止读取*/
		if(0 == sWavRcdStorState[eRcdStorState_allNum])
		{
			/*读指针与写指针同步*/
			sWavRcdStorState[eRcdStorState_remNum] = 0;
			sWavRcdStorState[eRcdStorState_rDayP] = sWavRcdStorState[eRcdStorState_wDayP];
			sWavRcdStorState[eRcdStorState_rSecP] = sWavRcdStorState[eRcdStorState_wSecP];
			sWavRcdStorState[eRcdStorState_rNumP] = sWavRcdStorState[eRcdStorState_wNumP];
			return(FALSE);
		}

		/*当前读当天无剩余记录可读*/
		if(0 == sWavRcdStorState[eRcdStorState_remNum])
		{
			/*当前读日期不是当前写日期(可读下一天)*/
			if(sWavRcdStorState[eRcdStorState_rDayP] != sWavRcdStorState[eRcdStorState_wDayP])
			{
				/*查找有可读记录的下一天*/
				for(i = 0; i < sWavRcdStorState[eRcdStorState_allDay]; i++)
				{
					/*下一天*/
					sWavRcdStorState[eRcdStorState_rDayP]++;

					/*读完最末一天*/
					if(sWavRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
					{
						sWavRcdStorState[eRcdStorState_rDayP] = 0;
					}

					/*该天有可读记录*/
					if(sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_rDayP]] > 0)
					{
						/*获取下一天当天剩余数目和读起始扇区*/
						sWavRcdStorState[eRcdStorState_remNum] = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
						sWavRcdStorState[eRcdStorState_rSecP] = sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
						sWavRcdStorState[eRcdStorState_rNumP] = 0;
						/*停止查找*/
						break;
					}
				}
			}

			/*未找到有可读记录*/
			if(0 == sWavRcdStorState[eRcdStorState_remNum])
			{
				/*可读数目为0,读指针与写指针同步*/
				sWavRcdStorState[eRcdStorState_allNum] = 0;
				sWavRcdStorState[eRcdStorState_rDayP] = sWavRcdStorState[eRcdStorState_wDayP];
				sWavRcdStorState[eRcdStorState_rSecP] = sWavRcdStorState[eRcdStorState_wSecP];
				sWavRcdStorState[eRcdStorState_rNumP] = sWavRcdStorState[eRcdStorState_wNumP];
				return(FALSE);
			}
		}

		/*读指针与写指针一致则读完成*/
		if((sWavRcdStorState[eRcdStorState_rDayP] == sWavRcdStorState[eRcdStorState_wDayP])
			&& (sWavRcdStorState[eRcdStorState_rSecP] == sWavRcdStorState[eRcdStorState_wSecP])
			&& (sWavRcdStorState[eRcdStorState_rNumP] == sWavRcdStorState[eRcdStorState_wNumP]))
		{
			/*无可读记录*/
			sWavRcdStorState[eRcdStorState_allNum] = 0;
			sWavRcdStorState[eRcdStorState_remNum] = 0;
			return(FALSE);
		}

		/*开始读记录*/
		sWavRcdStorReadBusyFlag = 1;

		/*计算当前读地址,从存储区读取一条记录*/
		regAddr = RcdCalcRecordAimAddr(3, sWavRcdStorState[eRcdStorState_rDayP], sWavRcdStorState[eRcdStorState_rSecP], sWavRcdStorState[eRcdStorState_rNumP]);

		/*读取成功*/
		if(TRUE == RcdReadRcdStoreInfoHook(3, regAddr, info, len))
		{
			sWavRcdStorState[eRcdStorState_allNum]--;
			sWavRcdStorState[eRcdStorState_remNum]--;

			/*读完全部记录*/
			if(0 == sWavRcdStorState[eRcdStorState_allNum])
			{
				/*读指针与写指针同步*/
				sWavRcdStorState[eRcdStorState_remNum] = 0;
				sWavRcdStorState[eRcdStorState_rDayP] = sWavRcdStorState[eRcdStorState_wDayP];
				sWavRcdStorState[eRcdStorState_rSecP] = sWavRcdStorState[eRcdStorState_wSecP];
				sWavRcdStorState[eRcdStorState_rNumP] = sWavRcdStorState[eRcdStorState_wNumP];
			}
			/*读当天还有记录可读*/
			else if(sWavRcdStorState[eRcdStorState_remNum] > 0)
			{
				/*读指针下移*/
				sWavRcdStorState[eRcdStorState_rNumP]++;

				/*读完一个扇区*/
				if(sWavRcdStorState[eRcdStorState_rNumP] >= RCD_WAVWD_SECNUM)
				{
					/*下一个扇区*/
					sWavRcdStorState[eRcdStorState_rNumP] = 0;
					sWavRcdStorState[eRcdStorState_rSecP]++;

					/*读到一天末尾扇区*/
					if(sWavRcdStorState[eRcdStorState_rSecP] >= RCD_WAVWD_DSEC_NUM)
					{
						/*返回一天首个扇区*/
						sWavRcdStorState[eRcdStorState_rSecP] = 0;
					}
				}
			}
			/*读完一天的记录*/
			else
			{
				/*当前读日期不是当前写日期(可读下一天)*/
				if(sWavRcdStorState[eRcdStorState_rDayP] != sWavRcdStorState[eRcdStorState_wDayP])
				{
					/*查找有可读记录的下一天*/
					for(i = 0; i < sWavRcdStorState[eRcdStorState_allDay]; i++)
					{
						/*下一天*/
						sWavRcdStorState[eRcdStorState_rDayP]++;

						/*读完最末一天*/
						if(sWavRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
						{
							sWavRcdStorState[eRcdStorState_rDayP] = 0;
						}

						/*获取下一天当天剩余数目和读起始扇区*/
						sWavRcdStorState[eRcdStorState_remNum] = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
						sWavRcdStorState[eRcdStorState_rSecP] = sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
						sWavRcdStorState[eRcdStorState_rNumP] = 0;

						/*该天有可读记录*/
						if(sWavRcdStorState[eRcdStorState_remNum] > 0)
						{
							/*停止查找*/
							break;
						}
					}
				}

				/*未找到可读记录的下一天*/
				if(0 == sWavRcdStorState[eRcdStorState_remNum])
				{
					/*无剩余可读记录,读指针与写指针同步*/
					sWavRcdStorState[eRcdStorState_allNum] = 0;
					sWavRcdStorState[eRcdStorState_rDayP] = sWavRcdStorState[eRcdStorState_wDayP];
					sWavRcdStorState[eRcdStorState_rSecP] = sWavRcdStorState[eRcdStorState_wSecP];
					sWavRcdStorState[eRcdStorState_rNumP] = sWavRcdStorState[eRcdStorState_wNumP];
				}
			}

			/*读指针与写指针一致则读完成*/
			if((sWavRcdStorState[eRcdStorState_rDayP] == sWavRcdStorState[eRcdStorState_wDayP])
				&& (sWavRcdStorState[eRcdStorState_rSecP] == sWavRcdStorState[eRcdStorState_wSecP])
				&& (sWavRcdStorState[eRcdStorState_rNumP] == sWavRcdStorState[eRcdStorState_wNumP]))
			{
				/*无可读记录*/
				sWavRcdStorState[eRcdStorState_allNum] = 0;
				sWavRcdStorState[eRcdStorState_remNum] = 0;
			}

			/*确认读取才记录更新的*/
			if(1 == conFlag)
			{
				/*每读完一天则重写一次记录状态信息*/
				RcdWriteRecodStoreStateInfo(3);
			}

			return(TRUE);
		}
		/*读存储区失败*/
		else
		{
			return(FALSE);
		}
	}
	/*未使能故障记录存储则从缓存区读取*/
	else
	{
		/*录波缓存区无记录*/
		if(eFatWavState_Fin != RcdGetFaultWaveStateAPI())
		{
			return(FALSE);
		}

		/*从录波缓存区读一条记录*/
		return(RcdGetFaultWaveInfoAPI(info, len));
	}
}

/*=================================================================================================
** @Name      : u8 RcdReadOneDodRcdUserAPI(u8 conFlag, t_DodStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的DOD记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条DOD记录
** @The notes : 从最早记录开始读取存储区或缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneDodRcdUserAPI(u8 conFlag, t_DodStoreInfo *info)
{
	u32 regAddr = 0;
	u16 rdInfo[RCD_DODINFO_LEN] = {0};

	/*输入参数错误*/
	if((conFlag > 1) || (info == NULL))
	{
		return(FALSE);
	}

	/*DOD记录存储使能则从存储区读取*/
	if(1 == gRcdPara.dodStoEn)
	{
		/*当前读当天无剩余记录可读*/
		if(0 == sDodRcdStorState[eRcdStorState_remNum])
		{
			/*复位读指针*/
			sDodRcdStorState[eRcdStorState_rSecP] = sDodRcdStorState[eRcdStorState_wSecP];
			sDodRcdStorState[eRcdStorState_rNumP] = sDodRcdStorState[eRcdStorState_wNumP];
			return(FALSE);
		}

		/*读指针与写指针一致则读完成*/
		if((sDodRcdStorState[eRcdStorState_rSecP] == sDodRcdStorState[eRcdStorState_wSecP])
			&& (sDodRcdStorState[eRcdStorState_rNumP] == sDodRcdStorState[eRcdStorState_wNumP]))
		{
			/*无可读记录*/
			sDodRcdStorState[eRcdStorState_remNum] = 0;
			return(FALSE);
		}

		/*开始读记录*/
		sDodRcdStorReadBusyFlag = 1;

		/*计算当前读地址,从存储区读取一条记录*/
		regAddr = RcdCalcRecordAimAddr(4, 0, sDodRcdStorState[eRcdStorState_rSecP], sDodRcdStorState[eRcdStorState_rNumP]);

		/*读取成功*/
		if(TRUE == RcdReadRcdStoreInfoHook(4, regAddr, rdInfo, RCD_DODINFO_LEN))
		{
			/*读取到记录信息*/
			info->id = rdInfo[0];
			info->year = rdInfo[1];
			info->MD = rdInfo[2];
			info->HM = rdInfo[3];
			info->sec = rdInfo[4];
			info->dod = rdInfo[5];
			info->chgC = rdInfo[6];
			info->dhgC = rdInfo[7];
			info->chgE = rdInfo[8];
			info->dhgE = rdInfo[9];

			sDodRcdStorState[eRcdStorState_remNum]--;

			/*读完所有记录*/
			if(0 == sDodRcdStorState[eRcdStorState_remNum])
			{
				/*复位读指针跟随写指针*/
				sDodRcdStorState[eRcdStorState_rSecP] = sDodRcdStorState[eRcdStorState_wSecP];
				sDodRcdStorState[eRcdStorState_rNumP] = sDodRcdStorState[eRcdStorState_wNumP];
			}
			/*读当天还有记录可读*/
			else
			{
				/*读指针下移*/
				sDodRcdStorState[eRcdStorState_rNumP]++;

				/*读完一个扇区*/
				if(sDodRcdStorState[eRcdStorState_rNumP] >= RCD_DODWD_SEC_NUM)
				{
					/*下一个扇区*/
					sDodRcdStorState[eRcdStorState_rNumP] = 0;
					sDodRcdStorState[eRcdStorState_rSecP]++;

					/*读到一天末尾扇区*/
					if(sDodRcdStorState[eRcdStorState_rSecP] >= RCD_DODWD_ASECNUM)
					{
						/*返回一天首个扇区*/
						sDodRcdStorState[eRcdStorState_rSecP] = 0;
					}
				}
			}

			/*确认读取才记录更新的*/
			if(1 == conFlag)
			{
				/*重写一次记录状态信息*/
				RcdWriteRecodStoreStateInfo(4);
			}

			return(TRUE);
		}
		/*读存储区失败*/
		else
		{
			return(FALSE);
		}
	}
	/*未DOD记录存储则从缓存区读取*/
	else
	{
		/*缓存区无记录*/
		if(0xffff == sDodRcdBuffInfo.dod)
		{
			return(FALSE);
		}

		/*返回缓存区记录值*/
		info->id = sDodRcdBuffInfo.id;
		info->year = sDodRcdBuffInfo.year;
		info->MD = sDodRcdBuffInfo.MD;
		info->HM = sDodRcdBuffInfo.HM;
		info->sec = sDodRcdBuffInfo.sec;
		info->dod = sDodRcdBuffInfo.dod;
		info->chgC = sDodRcdBuffInfo.chgC;
		info->dhgC = sDodRcdBuffInfo.dhgC;
		info->chgE = sDodRcdBuffInfo.chgE;
		info->dhgE = sDodRcdBuffInfo.dhgE;

		sDodRcdBuffInfo.dod = 0xffff;

		return(TRUE);
	}
}

/*=================================================================================================
** @Name      : u8 RcdDisplayOneErrRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) info:读取到的故障记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条故障记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneErrRcdUserAPI(u8 date, t_RcdStoreInfo *info)
{
	u8 index = 0;
	u32 regAddr = 0;
	u16 rdInfo[RCD_ERRINFO_LEN] = {0};
	static u8 sHisDate = 0xff;

	/*故障记录未使能*/
	if(0 == gRcdPara.errRcdEn)
	{
		return(FALSE);
	}

	/*输入参数错误*/
	if((date >= sErrRcdStorState[eRcdStorState_allDay]) || (info == NULL))
	{
		sHisDate = 0xff;
		return(FALSE);
	}

	/*故障记录存储使能则从存储区读取*/
	if(1 == gRcdPara.errStoEn)
	{
		/*日期变换*/
		if(date != sHisDate)
		{
			/*将日期转化为天指针*/
			if(date <= sErrRcdStorState[eRcdStorState_wDayP])
			{
				/*写当日作为第0天*/
				index = sErrRcdStorState[eRcdStorState_wDayP] - date;
			}
			/*显示日期在当前所写日期之后*/
			else
			{
				/*未写满31天,目标日期越界*/
				if(sErrRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
				{
					sHisDate = 0xff;
					return(FALSE);
				}
				/*写满31天则从最后往前推*/
				else
				{
					index = date - sErrRcdStorState[eRcdStorState_wDayP];
					index = RCD_STOR_MAXDAYS - index;
				}
			}

			/*更新新一天指针*/
			sErrRcdDispState.dDPoint = index;

			/*记录当天可显示记录数目*/
			sErrRcdDispState.dDaNum = sErrRcdStorState[eRcdStorState_eDayNum + index];

			/*当天有记录*/
			if(sErrRcdDispState.dDaNum > 0)
			{
				/*当天写满且起始扇区非第一个扇区*/
				if((sErrRcdDispState.dDaNum >= (RCD_ERRWD_DAYNUM - RCD_ERRWD_SECNUM))
					&& (sErrRcdStorState[eRcdStorState_dayFSec + index] > 0))
				{
					/*最新记录扇区为起始扇区前一个*/
					sErrRcdDispState.dSPoint = sErrRcdStorState[eRcdStorState_dayFSec + index] - 1;

					/*前一个扇区无记录*/
					if(0 == sErrRcdStorState[eRcdStorState_wNumP])
					{
						/*非第一个扇区*/
						if(sErrRcdDispState.dSPoint > 0)
						{
							/*再取一个扇区*/
							sErrRcdDispState.dSPoint -= 1;
						}
						/*第一个扇区*/
						else
						{
							/*取最后一个扇区*/
							sErrRcdDispState.dSPoint = RCD_ERRWD_DSEC_NUM - 1;
						}
					}
				}
				/*当天未写满*/
				else
				{
					/*显示最后写扇区*/
					sErrRcdDispState.dSPoint = (sErrRcdDispState.dDaNum - 1) / RCD_ERRWD_SECNUM;
				}

				/*指向显示最新一条记录位置*/
				sErrRcdDispState.dNPoint = (sErrRcdDispState.dDaNum - 1) % RCD_ERRWD_SECNUM;
			}
			/*当天无记录*/
			else
			{
				/*复位在起始位置*/
				sErrRcdDispState.dSPoint = 0;
				sErrRcdDispState.dNPoint = 0;

				sHisDate = 0xff;
				return(FALSE);
			}
		}
		/*显示当天记录*/
		else if(0 == date)
		{
			/*显示天指针取当前写天指针*/
			sErrRcdDispState.dDPoint = sErrRcdStorState[eRcdStorState_wDayP];

			/*当天记录未显示(将显示指针复位到最新记录)*/
			if(sErrRcdDispState.dDaNum == sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]])
			{
				/*当天有记录*/
				if(sErrRcdDispState.dDaNum > 0)
				{
					/*当天写满且起始扇区非第一个扇区*/
					if((sErrRcdDispState.dDaNum >= (RCD_ERRWD_DAYNUM - RCD_ERRWD_SECNUM))
						&& (sErrRcdStorState[eRcdStorState_dayFSec + sErrRcdStorState[eRcdStorState_wDayP]] > 0))
					{
						/*最新记录扇区为起始扇区前一个*/
						sErrRcdDispState.dSPoint = sErrRcdStorState[eRcdStorState_dayFSec + sErrRcdStorState[eRcdStorState_wDayP]] - 1;

						/*前一个扇区无记录*/
						if(0 == sErrRcdStorState[eRcdStorState_wNumP])
						{
							/*非第一个扇区*/
							if(sErrRcdDispState.dSPoint > 0)
							{
								/*再取一个扇区*/
								sErrRcdDispState.dSPoint -= 1;
							}
							/*第一个扇区*/
							else
							{
								/*取最后一个扇区*/
								sErrRcdDispState.dSPoint = RCD_ERRWD_DSEC_NUM - 1;
							}
						}
					}
					/*当天未写满*/
					else
					{
						/*显示最后写扇区*/
						sErrRcdDispState.dSPoint = (sErrRcdDispState.dDaNum - 1) / RCD_ERRWD_SECNUM;
					}

					/*指向显示最新一条记录位置*/
					sErrRcdDispState.dNPoint = (sErrRcdDispState.dDaNum - 1) % RCD_ERRWD_SECNUM;
				}
				/*当天无记录*/
				else
				{
					/*复位在起始位置*/
					sErrRcdDispState.dSPoint = 0;
					sErrRcdDispState.dNPoint = 0;

					sHisDate = 0xff;
					return(FALSE);
				}
			}
		}
		sHisDate = date;

		if(0 == sErrRcdDispState.dDaNum)
		{
			return(FALSE);
		}

		/*开始读记录*/
		sErrRcdStorReadBusyFlag = 1;

		/*计算当前读地址,从存储区读取一条记录*/
		regAddr = RcdCalcRecordAimAddr(0, sErrRcdDispState.dDPoint, sErrRcdDispState.dSPoint, sErrRcdDispState.dNPoint);

		/*读取成功*/
		if(TRUE == RcdReadRcdStoreInfoHook(0, regAddr, rdInfo, RCD_ERRINFO_LEN))
		{
			/*读取到记录信息*/
			info->id = rdInfo[0];
			info->codeH = rdInfo[1];
			info->codeL = rdInfo[2];
			info->year = rdInfo[3];
			info->MD = rdInfo[4];
			info->HM = rdInfo[5];
			info->sec = rdInfo[6];
			info->info1 = rdInfo[7];
			info->info2 = rdInfo[8];
			info->info3 = rdInfo[9];
			info->info4 = rdInfo[10];
			info->info5 = rdInfo[11];

			sErrRcdDispState.dDaNum--;

			/*当天还有可读记录*/
			if(sErrRcdDispState.dDaNum > 0)
			{
				/*未读完一个扇区*/
				if(sErrRcdDispState.dNPoint > 0)
				{
					sErrRcdDispState.dNPoint--;
				}
				/*读完一个扇区*/
				else
				{
					/*切换前一个扇区*/
					sErrRcdDispState.dNPoint = RCD_ERRWD_SECNUM - 1;

					/*未读完一天的扇区*/
					if(sErrRcdDispState.dSPoint > 0)
					{
						/*前一个扇区*/
						sErrRcdDispState.dSPoint--;
					}
					/*读完一天的扇区*/
					else
					{
						/*显示结束*/
						sErrRcdDispState.dDaNum = 0;
					}
				}
			}

			return(TRUE);
		}
		/*读存储区失败*/
		else
		{
			return(FALSE);
		}
	}
	/*未事件故障记录存储则从缓存区读取*/
	else
	{
		/*缓存区无记录*/
		if(0 == sErrRcdBuffState.rcdNum)
		{
			return(FALSE);
		}

		/*从缓存区读一条记录*/
		return(RcdReadOneErrQueAPI(info));
	}
}

/*=================================================================================================
** @Name      : u8 RcdDisplayOneEventRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条事件记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneEventRcdUserAPI(u8 date, t_RcdStoreInfo *info)
{
	u8 index = 0;
	u32 regAddr = 0;
	u16 rdInfo[RCD_EVNINFO_LEN] = {0};
	static u8 sHisDate = 0xff;

	/*事件记录未使能*/
	if(0 == gRcdPara.evnRcdEn)
	{
		return(FALSE);
	}

	/*输入参数错误*/
	if((date >= sEvnRcdStorState[eRcdStorState_allDay]) || (info == NULL))
	{
		sHisDate = 0xff;
		return(FALSE);
	}

	/*事件记录存储使能则从存储区读取*/
	if(1 == gRcdPara.evnStoEn)
	{
		/*日期变换*/
		if(date != sHisDate)
		{
			/*将日期转化为天指针*/
			if(date <= sEvnRcdStorState[eRcdStorState_wDayP])
			{
				/*写当日作为第0天*/
				index = sEvnRcdStorState[eRcdStorState_wDayP] - date;
			}
			/*显示日期在当前所写日期之后*/
			else
			{
				/*未写满31天,目标日期越界*/
				if(sEvnRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
				{
					sHisDate = 0xff;
					return(FALSE);
				}
				/*写满31天则从最后往前推*/
				else
				{
					index = date - sEvnRcdStorState[eRcdStorState_wDayP];
					index = RCD_STOR_MAXDAYS - index;
				}
			}

			/*更新新一天指针*/
			sEvnRcdDispState.dDPoint = index;

			/*记录当天可显示记录数目*/
			sEvnRcdDispState.dDaNum = sEvnRcdStorState[eRcdStorState_eDayNum + index];

			/*当天有记录*/
			if(sEvnRcdDispState.dDaNum > 0)
			{
				/*当天写满且起始扇区非第一个扇区*/
				if((sEvnRcdDispState.dDaNum >= (RCD_EVNWD_DAYNUM - RCD_EVNWD_SECNUM))
					&& (sEvnRcdStorState[eRcdStorState_dayFSec + index] > 0))
				{
					/*最新记录扇区为起始扇区前一个*/
					sEvnRcdDispState.dSPoint = sEvnRcdStorState[eRcdStorState_dayFSec + index] - 1;

					/*前一个扇区无记录*/
					if(0 == sEvnRcdStorState[eRcdStorState_wNumP])
					{
						/*非第一个扇区*/
						if(sEvnRcdDispState.dSPoint > 0)
						{
							/*再取一个扇区*/
							sEvnRcdDispState.dSPoint -= 1;
						}
						/*第一个扇区*/
						else
						{
							/*取最后一个扇区*/
							sEvnRcdDispState.dSPoint = RCD_EVNWD_DSEC_NUM - 1;
						}
					}
				}
				/*当天未写满*/
				else
				{
					/*显示最后写扇区*/
					sEvnRcdDispState.dSPoint = (sEvnRcdDispState.dDaNum - 1) / RCD_EVNWD_SECNUM;
				}

				/*指向显示最新一条记录位置*/
				sEvnRcdDispState.dNPoint = (sEvnRcdDispState.dDaNum - 1) % RCD_EVNWD_SECNUM;
			}
			/*当天无记录*/
			else
			{
				/*复位在起始位置*/
				sEvnRcdDispState.dSPoint = 0;
				sEvnRcdDispState.dNPoint = 0;

				sHisDate = 0xff;
				return(FALSE);
			}
		}
		/*显示当天记录*/
		else if(0 == date)
		{
			/*显示天指针取当前写天指针*/
			sEvnRcdDispState.dDPoint = sEvnRcdStorState[eRcdStorState_wDayP];

			/*当天记录未显示(将显示指针复位到最新记录)*/
			if(sEvnRcdDispState.dDaNum == sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]])
			{
				/*当天有记录*/
				if(sEvnRcdDispState.dDaNum > 0)
				{
					/*当天写满且起始扇区非第一个扇区*/
					if((sEvnRcdDispState.dDaNum >= (RCD_EVNWD_DAYNUM - RCD_EVNWD_SECNUM))
						&& (sEvnRcdStorState[eRcdStorState_dayFSec + sEvnRcdStorState[eRcdStorState_wDayP]] > 0))
					{
						/*最新记录扇区为起始扇区前一个*/
						sEvnRcdDispState.dSPoint = sEvnRcdStorState[eRcdStorState_dayFSec + sEvnRcdStorState[eRcdStorState_wDayP]] - 1;

						/*前一个扇区无记录*/
						if(0 == sEvnRcdStorState[eRcdStorState_wNumP])
						{
							/*非第一个扇区*/
							if(sEvnRcdDispState.dSPoint > 0)
							{
								/*再取一个扇区*/
								sEvnRcdDispState.dSPoint -= 1;
							}
							/*第一个扇区*/
							else
							{
								/*取最后一个扇区*/
								sEvnRcdDispState.dSPoint = RCD_EVNWD_DSEC_NUM - 1;
							}
						}
					}
					/*当天未写满*/
					else
					{
						/*显示最后写扇区*/
						sEvnRcdDispState.dSPoint = (sEvnRcdDispState.dDaNum - 1) / RCD_EVNWD_SECNUM;
					}

					/*指向显示最新一条记录位置*/
					sEvnRcdDispState.dNPoint = (sEvnRcdDispState.dDaNum - 1) % RCD_EVNWD_SECNUM;
				}
				/*当天无记录*/
				else
				{
					/*复位在起始位置*/
					sEvnRcdDispState.dSPoint = 0;
					sEvnRcdDispState.dNPoint = 0;

					sHisDate = 0xff;
					return(FALSE);
				}
			}
		}
		sHisDate = date;

		if(0 == sEvnRcdDispState.dDaNum)
		{
			return(FALSE);
		}

		/*开始读记录*/
		sEvnRcdStorReadBusyFlag = 1;

		/*计算当前读地址,从存储区读取一条记录*/
		regAddr = RcdCalcRecordAimAddr(1, sEvnRcdDispState.dDPoint, sEvnRcdDispState.dSPoint, sEvnRcdDispState.dNPoint);

		/*读取成功*/
		if(TRUE == RcdReadRcdStoreInfoHook(1, regAddr, rdInfo, RCD_EVNINFO_LEN))
		{
			/*读取到记录信息*/
			info->id = rdInfo[0];
			info->codeH = rdInfo[1];
			info->codeL = rdInfo[2];
			info->year = rdInfo[3];
			info->MD = rdInfo[4];
			info->HM = rdInfo[5];
			info->sec = rdInfo[6];
			info->info1 = rdInfo[7];
			info->info2 = rdInfo[8];
			info->info3 = rdInfo[9];
			info->info4 = rdInfo[10];
			info->info5 = rdInfo[11];

			sEvnRcdDispState.dDaNum--;

			/*还有可读记录*/
			if(sEvnRcdDispState.dDaNum > 0)
			{
				/*未读完一个扇区*/
				if(sEvnRcdDispState.dNPoint > 0)
				{
					sEvnRcdDispState.dNPoint--;
				}
				/*读完一个扇区*/
				else
				{
					/*前一个扇区最后一条记录*/
					sEvnRcdDispState.dNPoint = RCD_EVNWD_SECNUM - 1;

					/*未到达当天起始扇区*/
					if(sEvnRcdDispState.dSPoint > 0)
					{
						/*前一个扇区*/
						sEvnRcdDispState.dSPoint--;
					}
					/*到达当天起始扇区*/
					else
					{
						/*返回最末扇区*/
						sEvnRcdDispState.dSPoint = RCD_EVNWD_DSEC_NUM - 1;
					}
				}
			}

			return(TRUE);
		}
		/*读存储区失败*/
		else
		{
			return(FALSE);
		}
	}
	/*未使能事件记录存储则从缓存区读取*/
	else
	{
		/*缓存区无记录*/
		if(0 == sEvnRcdBuffState.rcdNum)
		{
			return(FALSE);
		}

		/*从缓存区读一条记录*/
		return(RcdReadOneEventQueAPI(info));
	}
}

/*=================================================================================================
** @Name      : u8 RcdDisplayOneDataRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) data:读取到的数据记录 len:读取长度
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条数据记录
** @The notes : 从最新记录开始读取存储区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneDataRcdUserAPI(u8 date, u16 *data, u8 len)
{
	u8 index = 0;
	u32 regAddr = 0;
	static u8 sHisDate = 0xff;

	/*数据存储未使能*/
	if(0 == gRcdPara.dataStoEn)
	{
		return(FALSE);
	}

	/*输入参数错误*/
	if((date >= sDatRcdStorState[eRcdStorState_allDay]) || (data == NULL) || (len > RCD_DATINFO_LEN))
	{
		sHisDate = 0xff;
		return(FALSE);
	}

	/*日期变换*/
	if(date != sHisDate)
	{
		/*将日期转化为天指针*/
		if(date <= sDatRcdStorState[eRcdStorState_wDayP])
		{
			/*写当日作为第0天*/
			index = sDatRcdStorState[eRcdStorState_wDayP] - date;
		}
		/*显示日期在当前所写日期之后*/
		else
		{
			/*未写满31天,目标日期越界*/
			if(sDatRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
			{
				sHisDate = 0xff;
				return(FALSE);
			}
			/*写满31天则从最后往前推*/
			else
			{
				index = date - sDatRcdStorState[eRcdStorState_wDayP];
				index = RCD_STOR_MAXDAYS - index;
			}
		}

		/*更新新一天指针*/
		sDatRcdDispState.dDPoint = index;

		/*记录当天可显示记录数目*/
		sDatRcdDispState.dDaNum = sDatRcdStorState[eRcdStorState_eDayNum + index];

		/*当天有记录*/
		if(sDatRcdDispState.dDaNum > 0)
		{
			/*当天写满且起始扇区非第一个扇区*/
			if((sDatRcdDispState.dDaNum >= (RCD_DATWD_DAYNUM - RCD_DATWD_SECNUM))
				&& (sDatRcdStorState[eRcdStorState_dayFSec + index] > 0))
			{
				/*最新记录扇区为起始扇区前一个*/
				sDatRcdDispState.dSPoint = sDatRcdStorState[eRcdStorState_dayFSec + index] - 1;

				/*前一个扇区无记录*/
				if(0 == sDatRcdStorState[eRcdStorState_wNumP])
				{
					/*非第一个扇区*/
					if(sDatRcdDispState.dSPoint > 0)
					{
						/*再取一个扇区*/
						sDatRcdDispState.dSPoint -= 1;
					}
					/*第一个扇区*/
					else
					{
						/*取最后一个扇区*/
						sDatRcdDispState.dSPoint = RCD_DATWD_DSEC_NUM - 1;
					}
				}
			}
			/*当天未写满*/
			else
			{
				/*显示最后写扇区*/
				sDatRcdDispState.dSPoint = (sDatRcdDispState.dDaNum - 1) / RCD_DATWD_SECNUM;
			}

			/*指向显示最新一条记录位置*/
			sDatRcdDispState.dNPoint = (sDatRcdDispState.dDaNum - 1) % RCD_DATWD_SECNUM;
		}
		/*当天无记录*/
		else
		{
			/*复位在起始位置*/
			sDatRcdDispState.dSPoint = 0;
			sDatRcdDispState.dNPoint = 0;

			sHisDate = 0xff;
			return(FALSE);
		}
	}
	/*显示当天记录*/
	else if(0 == date)
	{
		/*显示天指针取当前写天指针*/
		sDatRcdDispState.dDPoint = sDatRcdStorState[eRcdStorState_wDayP];

		/*当天记录未显示(将显示指针复位到最新记录)*/
		if(sDatRcdDispState.dDaNum == sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]])
		{
			/*当天有记录*/
			if(sDatRcdDispState.dDaNum > 0)
			{
				/*当天写满且起始扇区非第一个扇区*/
				if((sDatRcdDispState.dDaNum >= (RCD_DATWD_DAYNUM - RCD_DATWD_SECNUM))
					&& (sDatRcdStorState[eRcdStorState_dayFSec + sDatRcdStorState[eRcdStorState_wDayP]] > 0))
				{
					/*最新记录扇区为起始扇区前一个*/
					sDatRcdDispState.dSPoint = sDatRcdStorState[eRcdStorState_dayFSec + sDatRcdStorState[eRcdStorState_wDayP]] - 1;

					/*前一个扇区无记录*/
					if(0 == sDatRcdStorState[eRcdStorState_wNumP])
					{
						/*非第一个扇区*/
						if(sDatRcdDispState.dSPoint > 0)
						{
							/*再取一个扇区*/
							sDatRcdDispState.dSPoint -= 1;
						}
						/*第一个扇区*/
						else
						{
							/*取最后一个扇区*/
							sDatRcdDispState.dSPoint = RCD_DATWD_DSEC_NUM - 1;
						}
					}
				}
				/*当天未写满*/
				else
				{
					/*显示最后写扇区*/
					sDatRcdDispState.dSPoint = (sDatRcdDispState.dDaNum - 1) / RCD_DATWD_SECNUM;
				}

				/*指向显示最新一条记录位置*/
				sDatRcdDispState.dNPoint = (sDatRcdDispState.dDaNum - 1) % RCD_DATWD_SECNUM;
			}
			/*当天无记录*/
			else
			{
				/*复位在起始位置*/
				sDatRcdDispState.dSPoint = 0;
				sDatRcdDispState.dNPoint = 0;

				sHisDate = 0xff;
				return(FALSE);
			}
		}
	}

	sHisDate = date;

	/*当天无可读取条数*/
	if(0 == sDatRcdDispState.dDaNum)
	{
		return(FALSE);
	}

	/*开始读记录*/
	sDatRcdStorReadBusyFlag = 1;

	/*计算当前读地址,从存储区读取一条记录*/
	regAddr = RcdCalcRecordAimAddr(2, sDatRcdDispState.dDPoint, sDatRcdDispState.dSPoint, sDatRcdDispState.dNPoint);

	/*读取成功*/
	if(TRUE == RcdReadRcdStoreInfoHook(2, regAddr, data, len))
	{
		/*当天可读取条数--*/
		sDatRcdDispState.dDaNum--;

		/*还有可读记录*/
		if(sDatRcdDispState.dDaNum > 0)
		{
			/*未读完一个扇区*/
			if(sDatRcdDispState.dNPoint > 0)
			{
				sDatRcdDispState.dNPoint--;
			}
			/*读完一个扇区*/
			else
			{
				/*前一个扇区最新记录处*/
				sDatRcdDispState.dNPoint = RCD_DATWD_SECNUM - 1;

				/*非一天的头个扇区*/
				if(sDatRcdDispState.dSPoint > 0)
				{
					/*前一个扇区*/
					sDatRcdDispState.dSPoint--;
				}
				/*读到一天的头个扇区*/
				else
				{
					/*最后一个扇区*/
					sDatRcdDispState.dSPoint = RCD_DATWD_DSEC_NUM - 1;
				}
			}
		}

		return(TRUE);
	}
	/*读存储区失败*/
	else
	{
		return(FALSE);
	}
}

/*=================================================================================================
** @Name      : u8 RcdDisplayOneWaveRcdUserAPI(u8 date, u16 *data, u8 len)
** @Input     : date:显示日期(第几天) data:读取到的故障录波记录 len:读取长度
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条故障录波记录
** @The notes : 从最新记录开始读取存储区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneWaveRcdUserAPI(u8 date, u16 *data, u8 len)
{
	u8 index = 0;
	u32 regAddr = 0;
	static u8 sHisDate = 0xff;

	/*故障录波未使能*/
	if(0 == gRcdPara.wavRcdEn)
	{
		return(FALSE);
	}

	/*输入参数错误*/
	if((date >= sWavRcdStorState[eRcdStorState_allDay]) || (data == NULL) || (len > RCD_WAVINFO_LEN))
	{
		sHisDate = 0xff;
		return(FALSE);
	}

	/*故障录波存储使能*/
	if(1 == gRcdPara.wavStoEn)
	{
		/*日期变换*/
		if(date != sHisDate)
		{
			/*将日期转化为天指针*/
			if(date <= sWavRcdStorState[eRcdStorState_wDayP])
			{
				/*写当日作为第0天*/
				index = sWavRcdStorState[eRcdStorState_wDayP] - date;
			}
			/*显示日期在当前所写日期之后*/
			else
			{
				/*未写满31天,目标日期越界*/
				if(sWavRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
				{
					sHisDate = 0xff;
					return(FALSE);
				}
				/*写满31天则从最后往前推*/
				else
				{
					index = date - sWavRcdStorState[eRcdStorState_wDayP];
					index = RCD_STOR_MAXDAYS - index;
				}
			}

			/*更新新一天指针*/
			sWavRcdDispState.dDPoint = index;

			/*记录当天可显示记录数目*/
			sWavRcdDispState.dDaNum = sWavRcdStorState[eRcdStorState_eDayNum + index];

			/*当天有记录*/
			if(sWavRcdDispState.dDaNum > 0)
			{
				/*当天写满且起始扇区非第一个扇区*/
				if((sWavRcdDispState.dDaNum >= (RCD_WAVWD_DAYNUM - RCD_WAVWD_SECNUM))
					&& (sWavRcdStorState[eRcdStorState_dayFSec + index] > 0))
				{
					/*最新记录扇区为起始扇区前一个*/
					sWavRcdDispState.dSPoint = sWavRcdStorState[eRcdStorState_dayFSec + index] - 1;

					/*前一个扇区无记录*/
					if(0 == sWavRcdStorState[eRcdStorState_wNumP])
					{
						/*非第一个扇区*/
						if(sWavRcdDispState.dSPoint > 0)
						{
							/*再取一个扇区*/
							sWavRcdDispState.dSPoint -= 1;
						}
						/*第一个扇区*/
						else
						{
							/*取最后一个扇区*/
							sWavRcdDispState.dSPoint = RCD_WAVWD_DSEC_NUM - 1;
						}
					}
				}
				/*当天未写满*/
				else
				{
					/*显示最后写扇区*/
					sWavRcdDispState.dSPoint = (sWavRcdDispState.dDaNum - 1) / RCD_WAVWD_SECNUM;
				}

				/*指向显示最新一条记录位置*/
				sWavRcdDispState.dNPoint = (sWavRcdDispState.dDaNum - 1) % RCD_WAVWD_SECNUM;
			}
			/*当天无记录*/
			else
			{
				/*复位在起始位置*/
				sWavRcdDispState.dSPoint = 0;
				sWavRcdDispState.dNPoint = 0;

				sHisDate = 0xff;
				return(FALSE);
			}
		}
		/*显示当天记录*/
		else if(0 == date)
		{
			/*显示天指针取当前写天指针*/
			sWavRcdDispState.dDPoint = sWavRcdStorState[eRcdStorState_wDayP];

			/*当天记录未显示(将显示指针复位到最新记录)*/
			if(sWavRcdDispState.dDaNum == sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]])
			{
				/*当天有记录*/
				if(sWavRcdDispState.dDaNum > 0)
				{
					/*当天写满且起始扇区非第一个扇区*/
					if((sWavRcdDispState.dDaNum >= (RCD_WAVWD_DAYNUM - RCD_WAVWD_SECNUM))
						&& (sWavRcdStorState[eRcdStorState_dayFSec + sWavRcdStorState[eRcdStorState_wDayP]] > 0))
					{
						/*最新记录扇区为起始扇区前一个*/
						sWavRcdDispState.dSPoint = sWavRcdStorState[eRcdStorState_dayFSec + sWavRcdStorState[eRcdStorState_wDayP]] - 1;

						/*前一个扇区无记录*/
						if(0 == sWavRcdStorState[eRcdStorState_wNumP])
						{
							/*非第一个扇区*/
							if(sWavRcdDispState.dSPoint > 0)
							{
								/*再取一个扇区*/
								sWavRcdDispState.dSPoint -= 1;
							}
							/*第一个扇区*/
							else
							{
								/*取最后一个扇区*/
								sWavRcdDispState.dSPoint = RCD_DATWD_DSEC_NUM - 1;
							}
						}
					}
					/*当天未写满*/
					else
					{
						/*显示最后写扇区*/
						sWavRcdDispState.dSPoint = (sWavRcdDispState.dDaNum - 1) / RCD_WAVWD_SECNUM;
					}

					/*指向显示最新一条记录位置*/
					sWavRcdDispState.dNPoint = (sWavRcdDispState.dDaNum - 1) % RCD_WAVWD_SECNUM;
				}
				/*当天无记录*/
				else
				{
					/*复位在起始位置*/
					sWavRcdDispState.dSPoint = 0;
					sWavRcdDispState.dNPoint = 0;

					sHisDate = 0xff;
					return(FALSE);
				}
			}
		}

		sHisDate = date;

		/*当天无可读取条数*/
		if(0 == sWavRcdDispState.dDaNum)
		{
			return(FALSE);
		}

		/*开始读记录*/
		sWavRcdStorReadBusyFlag = 1;

		/*计算当前读地址,从存储区读取一条记录*/
		regAddr = RcdCalcRecordAimAddr(3, sWavRcdDispState.dDPoint, sWavRcdDispState.dSPoint, sWavRcdDispState.dNPoint);

		/*读取成功*/
		if(TRUE == RcdReadRcdStoreInfoHook(3, regAddr, data, len))
		{
			/*当天可读取条数--*/
			sWavRcdDispState.dDaNum--;

			/*还有可读记录*/
			if(sWavRcdDispState.dDaNum > 0)
			{
				/*未读完一个扇区*/
				if(sWavRcdDispState.dNPoint > 0)
				{
					sWavRcdDispState.dNPoint--;
				}
				/*读完一个扇区*/
				else
				{
					/*前一个扇区最新记录处*/
					sWavRcdDispState.dNPoint = RCD_WAVWD_SECNUM - 1;

					/*非一天的头个扇区*/
					if(sWavRcdDispState.dSPoint > 0)
					{
						/*前一个扇区*/
						sWavRcdDispState.dSPoint--;
					}
					/*读到一天的头个扇区*/
					else
					{
						/*最后一个扇区*/
						sWavRcdDispState.dSPoint = RCD_WAVWD_DSEC_NUM - 1;
					}
				}
			}

			return(TRUE);
		}
		/*读存储区失败*/
		else
		{
			return(FALSE);
		}
	}
	/*未使能故障录波存储则从缓存区读取*/
	else
	{
		/*录波缓存区无记录*/
		if(eFatWavState_Fin != RcdGetFaultWaveStateAPI())
		{
			return(FALSE);
		}

		/*从录波缓存区读一条记录*/
		return(RcdGetFaultWaveInfoAPI(data, len));
	}
}

/*=================================================================================================
** @Name      : u8 RcdDisplayOneDodRcdUserAPI(t_DodStoreInfo *info)
** @Input     : info:读取到的DOD记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条DOD记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneDodRcdUserAPI(t_DodStoreInfo *info)
{
	u32 regAddr = 0;
	u16 rdInfo[RCD_DODINFO_LEN] = {0};

	/*DOD记录存储使能则从存储区读取*/
	if(1 == gRcdPara.dodStoEn)
	{
		if(0 == sDodRcdDispState.dDaNum)
		{
			return(FALSE);
		}

		/*当天记录未显示(将显示指针复位到最新记录)*/
		if(sDodRcdDispState.dDaNum == sDodRcdStorState[eRcdStorState_allNum])
		{
			/*当前有记录*/
			if(sDodRcdDispState.dDaNum > 0)
			{
				/*记录写满扇区*/
				if(sDodRcdDispState.dDaNum >= (RCD_DODWD_ALLNUM - RCD_DODWD_SEC_NUM))
				{
					/*最新记录扇区为正在写扇区*/
					sDodRcdDispState.dSPoint = sDodRcdStorState[eRcdStorState_wSecP];

					/*在写扇区无记录*/
					if(0 == sDodRcdStorState[eRcdStorState_wNumP])
					{
						/*非第一个扇区*/
						if(sDodRcdDispState.dSPoint > 0)
						{
							/*再取前一个扇区*/
							sDodRcdDispState.dSPoint -= 1;
						}
						/*第一个扇区*/
						else
						{
							/*取最后一个扇区*/
							sDodRcdDispState.dSPoint = RCD_DODWD_ASECNUM - 1;
						}
					}
				}
				/*未写满所有扇区*/
				else
				{
					/*显示最后写扇区*/
					sDodRcdDispState.dSPoint = (sDodRcdDispState.dDaNum - 1) / RCD_DODWD_SEC_NUM;
				}

				/*指向显示最新一条记录位置*/
				sDodRcdDispState.dNPoint = (sDodRcdDispState.dDaNum - 1) % RCD_DODWD_SEC_NUM;
			}
			/*当前无记录*/
			else
			{
				/*复位在起始位置*/
				sDodRcdDispState.dSPoint = 0;
				sDodRcdDispState.dNPoint = 0;

				return(FALSE);
			}
		}

		/*开始读记录*/
		sDodRcdStorReadBusyFlag = 1;

		/*计算当前读地址,从存储区读取一条记录*/
		regAddr = RcdCalcRecordAimAddr(4, 0, sDodRcdDispState.dSPoint, sDodRcdDispState.dNPoint);

		/*读取成功*/
		if(TRUE == RcdReadRcdStoreInfoHook(4, regAddr, rdInfo, RCD_DODINFO_LEN))
		{
			/*读取到记录信息*/
			info->id = rdInfo[0];
			info->year = rdInfo[1];
			info->MD = rdInfo[2];
			info->HM = rdInfo[3];
			info->sec = rdInfo[4];
			info->dod = rdInfo[5];
			info->chgC = rdInfo[6];
			info->dhgC = rdInfo[7];
			info->chgE = rdInfo[8];
			info->dhgE = rdInfo[9];

			sDodRcdDispState.dDaNum--;

			/*还有可读记录*/
			if(sDodRcdDispState.dDaNum > 0)
			{
				/*未读完一个扇区*/
				if(sDodRcdDispState.dNPoint > 0)
				{
					sDodRcdDispState.dNPoint--;
				}
				/*读完一个扇区*/
				else
				{
					/*前一个扇区最后一条记录*/
					sDodRcdDispState.dNPoint = RCD_DODWD_SEC_NUM - 1;

					/*未到达当天起始扇区*/
					if(sDodRcdDispState.dSPoint > 0)
					{
						/*前一个扇区*/
						sDodRcdDispState.dSPoint--;
					}
					/*到达当天起始扇区*/
					else
					{
						/*返回最末扇区*/
						sDodRcdDispState.dSPoint = RCD_DODWD_ASECNUM - 1;
					}
				}
			}

			return(TRUE);
		}
		/*读存储区失败*/
		else
		{
			return(FALSE);
		}
	}
	/*未DOD记录存储则从缓存区读取*/
	else
	{
		/*缓存区无记录*/
		if(0xffff == sDodRcdBuffInfo.dod)
		{
			return(FALSE);
		}

		/*返回缓存区记录值*/
		info->id = sDodRcdBuffInfo.id;
		info->year = sDodRcdBuffInfo.year;
		info->MD = sDodRcdBuffInfo.MD;
		info->HM = sDodRcdBuffInfo.HM;
		info->sec = sDodRcdBuffInfo.sec;
		info->dod = sDodRcdBuffInfo.dod;
		info->chgC = sDodRcdBuffInfo.chgC;
		info->dhgC = sDodRcdBuffInfo.dhgC;
		info->chgE = sDodRcdBuffInfo.chgE;
		info->dhgE = sDodRcdBuffInfo.dhgE;

		sDodRcdBuffInfo.dod = 0xffff;

		return(TRUE);
	}
}

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeErrRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日首页显示历史故障记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeErrRcdUserAPI(void)
{
	u8 date = 0;

	/*故障记录未使能*/
	if(0 == gRcdPara.errRcdEn)
	{
		return(FALSE);
	}

	/*故障记录存储未使能*/
	if(0 == gRcdPara.errStoEn)
	{
		return(FALSE);
	}

	/*获取当前日期*/
	date = sErrRcdDispState.dDPoint;

	/*有记录数目超过1页30条且当前不是显示首页则需要跳转*/
	//if((sErrRcdStorState[eRcdStorState_eDayNum + (u8)date] > 30)
	//	&& (sErrRcdDispState.dDaNum < (sErrRcdStorState[eRcdStorState_eDayNum + (u8)date] - 30)))
	/*有记录且当前不是显示第一条则需要跳转*/
	if((sErrRcdStorState[eRcdStorState_eDayNum + (u8)date] > 0)
		&& (sErrRcdDispState.dDaNum < sErrRcdStorState[eRcdStorState_eDayNum + (u8)date]))
	{
		/*当天剩余数目为总数目*/
		sErrRcdDispState.dDaNum = sErrRcdStorState[eRcdStorState_eDayNum + (u8)date];

		/*当天写满且起始扇区非第一个扇区*/
		if((sErrRcdDispState.dDaNum >= RCD_DATWD_SECNUM)
			&& (sErrRcdStorState[eRcdStorState_dayFSec + date] > 0))
		{
			/*最新记录扇区为起始扇区前一个*/
			sErrRcdDispState.dSPoint = sErrRcdStorState[eRcdStorState_dayFSec + date] - 1;
		}
		/*当天未写满*/
		else
		{
			/*显示最后写扇区*/
			sErrRcdDispState.dSPoint = (sErrRcdDispState.dDaNum - 1) / RCD_ERRWD_SECNUM;
		}

		/*显示序号指向显示最新一条记录位置*/
		sErrRcdDispState.dNPoint = (sErrRcdDispState.dDaNum - 1) % RCD_ERRWD_SECNUM;
	}
	/*当前显示首页不需要翻页*/
	else
	{
		return(FALSE);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeEventRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日首页显示历史事件记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeEventRcdUserAPI(void)
{
	u8 date = 0;

	/*事件记录未使能*/
	if(0 == gRcdPara.evnRcdEn)
	{
		return(FALSE);
	}

	/*事件记录存储未使能*/
	if(0 == gRcdPara.evnStoEn)
	{
		return(FALSE);
	}

	/*获取当前日期*/
	date = sEvnRcdDispState.dDPoint;

	/*有记录数目超过1页30条且当前不是显示首页则需要跳转*/
	//if((sEvnRcdStorState[eRcdStorState_eDayNum + (u8)date] > 30)
	//	&& (sEvnRcdDispState.dDaNum < (sEvnRcdStorState[eRcdStorState_eDayNum + (u8)date] - 30)))
	/*有记录且当前不是显示第一条则需要跳转*/
	if((sEvnRcdStorState[eRcdStorState_eDayNum + (u8)date] > 0)
		&& (sEvnRcdDispState.dDaNum < sEvnRcdStorState[eRcdStorState_eDayNum + (u8)date]))
	{
		/*当天剩余数目为总数目*/
		sEvnRcdDispState.dDaNum = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)date];

		/*当天写满且起始扇区非第一个扇区*/
		if((sEvnRcdDispState.dDaNum >= RCD_EVNWD_SECNUM)
			&& (sEvnRcdStorState[eRcdStorState_dayFSec + date] > 0))
		{
			/*最新记录扇区为起始扇区前一个*/
			sEvnRcdDispState.dSPoint = sEvnRcdStorState[eRcdStorState_dayFSec + date] - 1;
		}
		/*当天未写满*/
		else
		{
			/*显示最后写扇区*/
			sEvnRcdDispState.dSPoint = (sEvnRcdDispState.dDaNum - 1) / RCD_EVNWD_SECNUM;
		}

		/*显示序号指向显示最新一条记录位置*/
		sEvnRcdDispState.dNPoint = (sEvnRcdDispState.dDaNum - 1) % RCD_EVNWD_SECNUM;
	}
	/*当前显示首页不需要翻页*/
	else
	{
		return(FALSE);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeDataRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日第一条显示历史数据记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeDataRcdUserAPI(void)
{
	u8 date = 0;

	/*数据记录存储未使能*/
	if(0 == gRcdPara.dataStoEn)
	{
		return(FALSE);
	}

	/*获取当前日期*/
	date = sDatRcdDispState.dDPoint;

	/*有记录且当前不是显示第一条则需要跳转*/
	if((sDatRcdStorState[eRcdStorState_eDayNum + (u8)date] > 0)
		&& (sDatRcdDispState.dDaNum < sDatRcdStorState[eRcdStorState_eDayNum + (u8)date]))
	{
		/*当天剩余数目为总数目*/
		sDatRcdDispState.dDaNum = sDatRcdStorState[eRcdStorState_eDayNum + (u8)date];

		/*当天写满且起始扇区非第一个扇区*/
		if((sDatRcdDispState.dDaNum >= RCD_DATWD_SECNUM)
			&& (sDatRcdStorState[eRcdStorState_dayFSec + date] > 0))
		{
			/*最新记录扇区为起始扇区前一个*/
			sDatRcdDispState.dSPoint = sDatRcdStorState[eRcdStorState_dayFSec + date] - 1;
		}
		/*当天未写满*/
		else
		{
			/*显示最后写扇区*/
			sDatRcdDispState.dSPoint = (sDatRcdDispState.dDaNum - 1) / RCD_DATWD_SECNUM;
		}

		/*显示序号指向显示最新一条记录位置*/
		sDatRcdDispState.dNPoint = (sDatRcdDispState.dDaNum - 1) % RCD_DATWD_SECNUM;
	}
	/*当前显示首页不需要翻页*/
	else
	{
		return(FALSE);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeWaveRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日第一条显示历史故障录波记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeWaveRcdUserAPI(void)
{
	u8 date = 0;

	/*故障录波记录存储未使能*/
	if(0 == gRcdPara.wavStoEn)
	{
		return(FALSE);
	}

	/*获取当前日期*/
	date = sWavRcdDispState.dDPoint;

	/*有记录且当前不是显示第一条则需要跳转*/
	if((sWavRcdStorState[eRcdStorState_eDayNum + (u8)date] > 0)
		&& (sWavRcdDispState.dDaNum < sWavRcdStorState[eRcdStorState_eDayNum + (u8)date]))
	{
		/*当天剩余数目为总数目*/
		sWavRcdDispState.dDaNum = sWavRcdStorState[eRcdStorState_eDayNum + (u8)date];

		/*当天写满且起始扇区非第一个扇区*/
		if((sWavRcdDispState.dDaNum >= RCD_WAVWD_SECNUM)
			&& (sWavRcdStorState[eRcdStorState_dayFSec + date] > 0))
		{
			/*最新记录扇区为起始扇区前一个*/
			sWavRcdDispState.dSPoint = sWavRcdStorState[eRcdStorState_dayFSec + date] - 1;
		}
		/*当天未写满*/
		else
		{
			/*显示最后写扇区*/
			sWavRcdDispState.dSPoint = (sWavRcdDispState.dDaNum - 1) / RCD_WAVWD_SECNUM;
		}

		/*显示序号指向显示最新一条记录位置*/
		sWavRcdDispState.dNPoint = (sWavRcdDispState.dDaNum - 1) % RCD_WAVWD_SECNUM;
	}
	/*当前显示首页不需要翻页*/
	else
	{
		return(FALSE);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeDodRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到最新第一条显示历史DOD记录
** @The notes : 只移动读显示指针地址到当前最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeDodRcdUserAPI(void)
{
	/*DOD记录存储未使能*/
	if(0 == gRcdPara.dodStoEn)
	{
		return(FALSE);
	}

	/*有记录数目超过1页100条且当前不是显示首页则需要跳转*/
	//if((sDodRcdStorState[eRcdStorState_allNum] > 100)
	//	&& (sDodRcdDispState.dDaNum < (sDodRcdStorState[eRcdStorState_allNum] - 100)))
	/*有记录且当前不是显示第一条则需要跳转*/
	if((sDodRcdStorState[eRcdStorState_allNum] > 0)
		&& (sDodRcdDispState.dDaNum < sDodRcdStorState[eRcdStorState_allNum]))
	{
		/*当天剩余数目为总数目*/
		sDodRcdDispState.dDaNum = sDodRcdStorState[eRcdStorState_allNum];

		/*记录写满所有扇区*/
		if(sDodRcdDispState.dDaNum >= (RCD_DODWD_ALLNUM - RCD_DODWD_SEC_NUM))
		{
			/*非写扇区起始位置*/
			if(0 != sDodRcdStorState[eRcdStorState_wNumP])
			{
				/*最新记录扇区为在写扇区*/
				sDodRcdDispState.dSPoint = sDodRcdStorState[eRcdStorState_wSecP];
			}
			/*非写起始扇区*/
			else if(sDodRcdStorState[eRcdStorState_wSecP] > 0)
			{
				/*最新记录扇区为在写扇区前一个*/
				sDodRcdDispState.dSPoint = sDodRcdStorState[eRcdStorState_wSecP] - 1;
			}
			/*写起始扇区*/
			else
			{
				/*最新记录扇区为最后扇区*/
				sDodRcdDispState.dSPoint = RCD_DODWD_ASECNUM - 1;
			}
		}
		/*记录未写满*/
		else
		{
			/*显示最后写扇区*/
			sDodRcdDispState.dSPoint = (sDodRcdDispState.dDaNum - 1) / RCD_DODWD_SEC_NUM;
		}

		/*显示序号指向显示最新一条记录位置*/
		sDodRcdDispState.dNPoint = (sDodRcdDispState.dDaNum - 1) % RCD_DODWD_SEC_NUM;
	}
	/*当前显示首页不需要翻页*/
	else
	{
		return(FALSE);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdClrDateErrRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期故障记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
u8 RcdClrDateErrRcdUserAPI(u8 date)
{
	u8 i = 0;
	u8 index = 0;
	u32 regAddr = 0;

	/*故障记录未使能*/
	if(0 == gRcdPara.errStoEn)
	{
		return(FALSE);
	}

	/*清除所有记录*/
	if(0xff == date)
	{
		/*当前记录读写指针清零*/
		for(i = 0; i < RCD_STA_INFO_STOR_LEN; i++)
		{
			sErrRcdStorState[i] = 0;
		}

		/*当前显示记录清零*/
		sErrRcdDispState.dDPoint = 0;
		sErrRcdDispState.dSPoint = 0;
		sErrRcdDispState.dNPoint = 0;
		sErrRcdDispState.dDaNum = 0;

		/*记录状态有效标志*/
		sErrRcdStorState[eRcdStorState_flag] = RCD_STOR_STATE_FLAG;

		/*更新记录状态*/
		RcdWriteRecodStoreStateInfo(0);

		return(TRUE);
	}
	/*输入参数错误*/
	else if(date >= sErrRcdStorState[eRcdStorState_allDay])
	{
		return(FALSE);
	}

	/*将日期转化为天指针*/
	if(date <= sErrRcdStorState[eRcdStorState_wDayP])
	{
		/*写当日作为第0天*/
		index = sErrRcdStorState[eRcdStorState_wDayP] - date;
	}
	/*目标日期在当前所写日期之后*/
	else
	{
		/*未写满31天,目标日期越界*/
		if(sErrRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
			return(FALSE);
		}
		/*写满31天则从最后往前推*/
		else
		{
			index = date - sErrRcdStorState[eRcdStorState_wDayP];
			index = RCD_STOR_MAXDAYS - index;
		}
	}

	/*目标日期有记录才进行擦除*/
	if(sErrRcdStorState[eRcdStorState_eDayNum + index] > 0)
	{
		/*记录总数目少当天数目,该天记录清零*/
		sErrRcdStorState[eRcdStorState_allNum] -= sErrRcdStorState[eRcdStorState_eDayNum + (u8)index];
		sErrRcdStorState[eRcdStorState_eDayNum + (u8)index] = 0;

		/*该天记录起始扇区清零*/
		sErrRcdStorState[eRcdStorState_dayFSec + (u8)index] = 0;

		/*清除正在写当天则复位写指针*/
		if(sErrRcdStorState[eRcdStorState_wDayP] == index)
		{
			sErrRcdStorState[eRcdStorState_wSecP] = 0;
			sErrRcdStorState[eRcdStorState_wNumP] = 0;
		}

		/*清除操作直接复位读最旧记录*/
		/*已写满31天*/
		if(sErrRcdStorState[eRcdStorState_allDay] >= RCD_STOR_MAXDAYS)
		{
			/*当前正在写最末尾天*/
			if(sErrRcdStorState[eRcdStorState_wDayP] >= (RCD_STOR_MAXDAYS - 1))
			{
				/*读天指针回到初始位置*/
				sErrRcdStorState[eRcdStorState_rDayP] = 0;
			}
			/*当前不是写最末尾天*/
			else
			{
				/*读指针回到写指针后一天位置*/
				sErrRcdStorState[eRcdStorState_rDayP] = sErrRcdStorState[eRcdStorState_wDayP] + 1;
			}
		}
		/*未写满31天*/
		else
		{
			/*读天指针回到初始位置*/
			sErrRcdStorState[eRcdStorState_rDayP] = 0;
		}

		/*读起始扇区*/
		sErrRcdStorState[eRcdStorState_rSecP] = sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
		sErrRcdStorState[eRcdStorState_rNumP] = 0;

		/*读剩余数目为当天记录数目*/
		sErrRcdStorState[eRcdStorState_remNum] = sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_rDayP]];

		/*清除日期正在显示直接复位显示最新记录*/
		if(sErrRcdDispState.dDPoint == index)
		{
			sErrRcdDispState.dDPoint = sErrRcdStorState[eRcdStorState_wDayP];
			sErrRcdDispState.dSPoint = sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
			sErrRcdDispState.dNPoint = 0;
			sErrRcdDispState.dDaNum = sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]];
		}

		/*擦除该天所有扇区*/
		for(i = 0; i < RCD_ERRWD_DSEC_NUM; i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(0, index, i, 0);
			(void)RcdEraseRcdStoreSectorHook(0, regAddr);
		}

		/*更新记录状态*/
		RcdWriteRecodStoreStateInfo(0);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdClrDateEventRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期事件记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
u8 RcdClrDateEventRcdUserAPI(u8 date)
{
	u8 i = 0;
	u8 index = 0;
	u32 regAddr = 0;

	/*事件记录未使能*/
	if(0 == gRcdPara.evnStoEn)
	{
		return(FALSE);
	}

	/*清除所有记录*/
	if(0xff == date)
	{
		/*当前记录读写指针清零*/
		for(i = 0; i < RCD_STA_INFO_STOR_LEN; i++)
		{
			sEvnRcdStorState[i] = 0;
		}

		/*当前显示记录清零*/
		sEvnRcdDispState.dDPoint = 0;
		sEvnRcdDispState.dSPoint = 0;
		sEvnRcdDispState.dNPoint = 0;
		sEvnRcdDispState.dDaNum = 0;

		/*记录状态有效标志*/
		sEvnRcdStorState[eRcdStorState_flag] = RCD_STOR_STATE_FLAG;

		/*更新记录状态*/
		RcdWriteRecodStoreStateInfo(1);

		return(TRUE);
	}
	/*输入参数错误*/
	else if(date >= sEvnRcdStorState[eRcdStorState_allDay])
	{
		return(FALSE);
	}

	/*将日期转化为天指针*/
	if(date <= sEvnRcdStorState[eRcdStorState_wDayP])
	{
		/*写当日作为第0天*/
		index = sEvnRcdStorState[eRcdStorState_wDayP] - date;
	}
	/*目标日期在当前所写日期之后*/
	else
	{
		/*未写满31天,目标日期越界*/
		if(sEvnRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
			return(FALSE);
		}
		/*写满31天则从最后往前推*/
		else
		{
			index = date - sEvnRcdStorState[eRcdStorState_wDayP];
			index = RCD_STOR_MAXDAYS - index;
		}
	}

	/*目标日期有记录才进行擦除*/
	if(sEvnRcdStorState[eRcdStorState_eDayNum + index] > 0)
	{
		/*记录总数目少当天数目,该天记录清零*/
		sEvnRcdStorState[eRcdStorState_allNum] -= sEvnRcdStorState[eRcdStorState_eDayNum + (u8)index];
		sEvnRcdStorState[eRcdStorState_eDayNum + (u8)index] = 0;

		/*该天记录起始扇区清零*/
		sEvnRcdStorState[eRcdStorState_dayFSec + (u8)index] = 0;

		/*清除正在写当天则复位写指针*/
		if(sEvnRcdStorState[eRcdStorState_wDayP] == index)
		{
			sEvnRcdStorState[eRcdStorState_wSecP] = 0;
			sEvnRcdStorState[eRcdStorState_wNumP] = 0;
		}

		/*清除操作直接复位读最旧记录*/
		/*已写满31天*/
		if(sEvnRcdStorState[eRcdStorState_allDay] >= RCD_STOR_MAXDAYS)
		{
			/*当前正在写最末尾天*/
			if(sEvnRcdStorState[eRcdStorState_wDayP] >= (RCD_STOR_MAXDAYS - 1))
			{
				/*读天指针回到初始位置*/
				sEvnRcdStorState[eRcdStorState_rDayP] = 0;
			}
			/*当前不是写最末尾天*/
			else
			{
				/*读指针回到写指针后一天位置*/
				sEvnRcdStorState[eRcdStorState_rDayP] = sEvnRcdStorState[eRcdStorState_wDayP] + 1;
			}
		}
		/*未写满31天*/
		else
		{
			/*读天指针回到初始位置*/
			sEvnRcdStorState[eRcdStorState_rDayP] = 0;
		}

		/*读起始扇区*/
		sEvnRcdStorState[eRcdStorState_rSecP] = sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
		sEvnRcdStorState[eRcdStorState_rNumP] = 0;

		/*读剩余数目为当天记录数目*/
		sEvnRcdStorState[eRcdStorState_remNum] = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];

		/*清除日期正在显示直接复位显示最新记录*/
		if(sEvnRcdDispState.dDPoint == index)
		{
			sEvnRcdDispState.dDPoint = sEvnRcdStorState[eRcdStorState_wDayP];
			sEvnRcdDispState.dSPoint = sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
			sEvnRcdDispState.dNPoint = 0;
			sEvnRcdDispState.dDaNum = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]];
		}

		/*擦除该天所有扇区*/
		for(i = 0; i < RCD_EVNWD_DSEC_NUM; i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(1, index, i, 0);
			(void)RcdEraseRcdStoreSectorHook(1, regAddr);
		}

		/*更新记录状态*/
		RcdWriteRecodStoreStateInfo(1);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdClrDateDataRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期运行数据记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
u8 RcdClrDateDataRcdUserAPI(u8 date)
{
	u8 i = 0;
	u8 index = 0;
	u32 regAddr = 0;

	/*数据记录未使能*/
	if(0 == gRcdPara.dataStoEn)
	{
		return(FALSE);
	}

	/*清除所有记录*/
	if(0xff == date)
	{
		/*当前记录读写指针清零*/
		for(i = 0; i < RCD_STA_INFO_STOR_LEN; i++)
		{
			sDatRcdStorState[i] = 0;
		}

		/*当前显示记录清零*/
		sDatRcdDispState.dDPoint = 0;
		sDatRcdDispState.dSPoint = 0;
		sDatRcdDispState.dNPoint = 0;
		sDatRcdDispState.dDaNum = 0;

		/*记录状态有效标志*/
		sDatRcdStorState[eRcdStorState_flag] = RCD_STOR_STATE_FLAG;

		/*更新记录状态*/
		RcdWriteRecodStoreStateInfo(2);

		return(TRUE);
	}
	/*输入参数错误*/
	else if(date >= sDatRcdStorState[eRcdStorState_allDay])
	{
		return(FALSE);
	}

	/*将日期转化为天指针*/
	if(date <= sDatRcdStorState[eRcdStorState_wDayP])
	{
		/*写当日作为第0天*/
		index = sDatRcdStorState[eRcdStorState_wDayP] - date;
	}
	/*目标日期在当前所写日期之后*/
	else
	{
		/*未写满31天,目标日期越界*/
		if(sDatRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
			return(FALSE);
		}
		/*写满31天则从最后往前推*/
		else
		{
			index = date - sDatRcdStorState[eRcdStorState_wDayP];
			index = RCD_STOR_MAXDAYS - index;
		}
	}

	/*目标日期有记录才进行擦除*/
	if(sDatRcdStorState[eRcdStorState_eDayNum + index] > 0)
	{
		/*记录总数目少当天数目,该天记录清零*/
		sDatRcdStorState[eRcdStorState_allNum] -= sDatRcdStorState[eRcdStorState_eDayNum + (u8)index];
		sDatRcdStorState[eRcdStorState_eDayNum + (u8)index] = 0;

		/*该天记录起始扇区清零*/
		sDatRcdStorState[eRcdStorState_dayFSec + (u8)index] = 0;

		/*清除正在写当天则复位写指针*/
		if(sDatRcdStorState[eRcdStorState_wDayP] == index)
		{
			sDatRcdStorState[eRcdStorState_wSecP] = 0;
			sDatRcdStorState[eRcdStorState_wNumP] = 0;
		}

		/*清除操作直接复位读最旧记录*/
		/*已写满31天*/
		if(sDatRcdStorState[eRcdStorState_allDay] >= RCD_STOR_MAXDAYS)
		{
			/*当前正在写最末尾天*/
			if(sDatRcdStorState[eRcdStorState_wDayP] >= (RCD_STOR_MAXDAYS - 1))
			{
				/*读天指针回到初始位置*/
				sDatRcdStorState[eRcdStorState_rDayP] = 0;
			}
			/*当前不是写最末尾天*/
			else
			{
				/*读指针回到写指针后一天位置*/
				sDatRcdStorState[eRcdStorState_rDayP] = sDatRcdStorState[eRcdStorState_wDayP] + 1;
			}
		}
		/*未写满31天*/
		else
		{
			/*读天指针回到初始位置*/
			sDatRcdStorState[eRcdStorState_rDayP] = 0;
		}

		/*读起始扇区*/
		sDatRcdStorState[eRcdStorState_rSecP] = sDatRcdStorState[eRcdStorState_dayFSec + (u8)sDatRcdStorState[eRcdStorState_rDayP]];
		sDatRcdStorState[eRcdStorState_rNumP] = 0;

		/*读剩余数目为当天记录数目*/
		sDatRcdStorState[eRcdStorState_remNum] = sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_rDayP]];

		/*清除日期正在显示直接复位显示最新记录*/
		if(sDatRcdDispState.dDPoint == index)
		{
			sDatRcdDispState.dDPoint = sDatRcdStorState[eRcdStorState_wDayP];
			sDatRcdDispState.dSPoint = sDatRcdStorState[eRcdStorState_dayFSec + (u8)sDatRcdStorState[eRcdStorState_rDayP]];
			sDatRcdDispState.dNPoint = 0;
			sDatRcdDispState.dDaNum = sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]];
		}

		/*擦除该天所有扇区*/
		for(i = 0; i < RCD_DATWD_DSEC_NUM; i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(2, index, i, 0);
			(void)RcdEraseRcdStoreSectorHook(2, regAddr);
		}

		/*更新记录状态*/
		RcdWriteRecodStoreStateInfo(2);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdClrDateWaveRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期故障录波记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
u8 RcdClrDateWaveRcdUserAPI(u8 date)
{
	u8 i = 0;
	u8 index = 0;
	u32 regAddr = 0;

	/*数据记录未使能*/
	if(0 == gRcdPara.wavStoEn)
	{
		return(FALSE);
	}

	/*清除所有记录*/
	if(0xff == date)
	{
		/*当前记录读写指针清零*/
		for(i = 0; i < RCD_STA_INFO_STOR_LEN; i++)
		{
			sWavRcdStorState[i] = 0;
		}

		/*当前显示记录清零*/
		sWavRcdDispState.dDPoint = 0;
		sWavRcdDispState.dSPoint = 0;
		sWavRcdDispState.dNPoint = 0;
		sWavRcdDispState.dDaNum = 0;

		/*记录状态有效标志*/
		sWavRcdStorState[eRcdStorState_flag] = RCD_STOR_STATE_FLAG;

		/*更新记录状态*/
		RcdWriteRecodStoreStateInfo(3);

		return(TRUE);
	}
	/*输入参数错误*/
	else if(date >= sWavRcdStorState[eRcdStorState_allDay])
	{
		return(FALSE);
	}

	/*将日期转化为天指针*/
	if(date <= sWavRcdStorState[eRcdStorState_wDayP])
	{
		/*写当日作为第0天*/
		index = sWavRcdStorState[eRcdStorState_wDayP] - date;
	}
	/*目标日期在当前所写日期之后*/
	else
	{
		/*未写满31天,目标日期越界*/
		if(sWavRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
			return(FALSE);
		}
		/*写满31天则从最后往前推*/
		else
		{
			index = date - sWavRcdStorState[eRcdStorState_wDayP];
			index = RCD_STOR_MAXDAYS - index;
		}
	}

	/*目标日期有记录才进行擦除*/
	if(sWavRcdStorState[eRcdStorState_eDayNum + index] > 0)
	{
		/*记录总数目少当天数目,该天记录清零*/
		sWavRcdStorState[eRcdStorState_allNum] -= sWavRcdStorState[eRcdStorState_eDayNum + (u8)index];
		sWavRcdStorState[eRcdStorState_eDayNum + (u8)index] = 0;

		/*该天记录起始扇区清零*/
		sWavRcdStorState[eRcdStorState_dayFSec + (u8)index] = 0;

		/*清除正在写当天则复位写指针*/
		if(sWavRcdStorState[eRcdStorState_wDayP] == index)
		{
			sWavRcdStorState[eRcdStorState_wSecP] = 0;
			sWavRcdStorState[eRcdStorState_wNumP] = 0;
		}

		/*清除操作直接复位读最旧记录*/
		/*已写满31天*/
		if(sWavRcdStorState[eRcdStorState_allDay] >= RCD_STOR_MAXDAYS)
		{
			/*当前正在写最末尾天*/
			if(sWavRcdStorState[eRcdStorState_wDayP] >= (RCD_STOR_MAXDAYS - 1))
			{
				/*读天指针回到初始位置*/
				sWavRcdStorState[eRcdStorState_rDayP] = 0;
			}
			/*当前不是写最末尾天*/
			else
			{
				/*读指针回到写指针后一天位置*/
				sWavRcdStorState[eRcdStorState_rDayP] = sWavRcdStorState[eRcdStorState_wDayP] + 1;
			}
		}
		/*未写满31天*/
		else
		{
			/*读天指针回到初始位置*/
			sWavRcdStorState[eRcdStorState_rDayP] = 0;
		}

		/*读起始扇区*/
		sWavRcdStorState[eRcdStorState_rSecP] = sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
		sWavRcdStorState[eRcdStorState_rNumP] = 0;

		/*读剩余数目为当天记录数目*/
		sWavRcdStorState[eRcdStorState_remNum] = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_rDayP]];

		/*清除日期正在显示直接复位显示最新记录*/
		if(sWavRcdDispState.dDPoint == index)
		{
			sWavRcdDispState.dDPoint = sWavRcdStorState[eRcdStorState_wDayP];
			sWavRcdDispState.dSPoint = sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
			sWavRcdDispState.dNPoint = 0;
			sWavRcdDispState.dDaNum = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]];
		}

		/*擦除该天所有扇区*/
		for(i = 0; i < RCD_WAVWD_DSEC_NUM; i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(3, index, i, 0);
			(void)RcdEraseRcdStoreSectorHook(3, regAddr);
		}

		/*更新记录状态*/
		RcdWriteRecodStoreStateInfo(3);
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdClrAllDodRcdUserAPI(void)
** @Input     : voiid
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除所有DOD记录存储信息
** @The notes :
**===============================================================================================*/
u8 RcdClrAllDodRcdUserAPI(void)
{
	u8 i = 0;

	/*DOD记录未使能*/
	if(0 == gRcdPara.dodStoEn)
	{
		return(FALSE);
	}

	/*当前记录读写指针清零*/
	for(i = 0; i < RCD_STOR_WRP_LEN; i++)
	{
		sDodRcdStorState[i] = 0;
	}

	/*当前显示记录清零*/
	sDodRcdDispState.dDPoint = 0;
	sDodRcdDispState.dSPoint = 0;
	sDodRcdDispState.dNPoint = 0;
	sDodRcdDispState.dDaNum = 0;

	/*记录状态有效标志*/
	sDodRcdStorState[eRcdStorState_flag] = RCD_STOR_STATE_FLAG;

	/*更新记录状态*/
	RcdWriteRecodStoreStateInfo(4);

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdReadOneErrQueAPI(t_RcdStoreInfo *info)
** @Input     : info:读取到的故障记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 从故障记录缓存区读取一条故障记录
** @The notes : 读取缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneErrQueAPI(t_RcdStoreInfo *info)
{
	/*故障记录未使能*/
	if(0 == gRcdPara.errRcdEn)
	{
		return(FALSE);
	}

	/*缓存区无记录*/
	if(0 == sErrRcdBuffState.rcdNum)
	{
		return(FALSE);
	}

	/*读取记录信息*/
	info->id = sErrRcdBuff[sErrRcdBuffState.rPoint];
	info->codeH = sErrRcdBuff[sErrRcdBuffState.rPoint + 1];
	info->codeL = sErrRcdBuff[sErrRcdBuffState.rPoint + 2];
	info->year = sErrRcdBuff[sErrRcdBuffState.rPoint + 3];
	info->MD = sErrRcdBuff[sErrRcdBuffState.rPoint + 4];
	info->HM = sErrRcdBuff[sErrRcdBuffState.rPoint + 5];
	info->sec = sErrRcdBuff[sErrRcdBuffState.rPoint + 6];
	info->info1 = sErrRcdBuff[sErrRcdBuffState.rPoint + 7];
	info->info2 = sErrRcdBuff[sErrRcdBuffState.rPoint + 8];
	info->info3 = sErrRcdBuff[sErrRcdBuffState.rPoint + 9];
	info->info4 = sErrRcdBuff[sErrRcdBuffState.rPoint + 10];
	info->info5 = sErrRcdBuff[sErrRcdBuffState.rPoint + 11];

	/*记录数目减1*/
	sErrRcdBuffState.rcdNum--;

	/*读指针下移*/
	sErrRcdBuffState.rPoint += RCD_ERRINFO_LEN;

	/*读指针到最后*/
	if(sErrRcdBuffState.rPoint >= RCD_ERRBUFF_LEN)
	{
		/*读指针回头*/
		sErrRcdBuffState.rPoint = 0;
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdReadOneEventQueAPI(t_RcdStoreInfo *info)
** @Input     : info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 从事件记录缓存区读取一条事件记录
** @The notes : 读取缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneEventQueAPI(t_RcdStoreInfo *info)
{
	/*事件记录未使能*/
	if(0 == gRcdPara.evnRcdEn)
	{
		return(FALSE);
	}

	/*缓存区无记录*/
	if(0 == sEvnRcdBuffState.rcdNum)
	{
		return(FALSE);
	}

	/*读取记录信息*/
	info->id = sEvnRcdBuff[sEvnRcdBuffState.rPoint];
	info->codeH = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 1];
	info->codeL = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 2];
	info->year = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 3];
	info->MD = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 4];
	info->HM = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 5];
	info->sec = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 6];
	info->info1 = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 7];
	info->info2 = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 8];
	info->info3 = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 9];
	info->info4 = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 10];
	info->info5 = sEvnRcdBuff[sEvnRcdBuffState.rPoint + 11];

	/*记录数目减1*/
	sEvnRcdBuffState.rcdNum--;

	/*读指针下移*/
	sEvnRcdBuffState.rPoint += RCD_EVNINFO_LEN;

	/*读指针到最后*/
	if(sEvnRcdBuffState.rPoint >= RCD_EVNBUFF_LEN)
	{
		/*读指针回头*/
		sEvnRcdBuffState.rPoint = 0;
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdGetDodBuffInfoAPI(t_DodStoreInfo *info)
** @Input     : info:当前DOD记录缓存信息
** @Output    : 或取结果 TRUE:成功 FALSE:失败
** @Function  : 获取当前DOD记录缓存信息
** @The notes : 缓存只记录一条DOD记录信息
**===============================================================================================*/
u8 RcdGetDodBuffInfoAPI(t_DodStoreInfo *info)
{
	/*缓存区无记录*/
	if(0xffff == sDodRcdBuffInfo.dod)
	{
		return(FALSE);
	}

	/*获取记录信息*/
	info->id = sDodRcdBuffInfo.id;
	info->year = sDodRcdBuffInfo.year;
	info->MD = sDodRcdBuffInfo.MD;
	info->HM = sDodRcdBuffInfo.HM;
	info->sec = sDodRcdBuffInfo.sec;
	info->dod = sDodRcdBuffInfo.dod;
	info->chgC = sDodRcdBuffInfo.chgC;
	info->dhgC = sDodRcdBuffInfo.dhgC;
	info->chgE = sDodRcdBuffInfo.chgE;
	info->dhgE = sDodRcdBuffInfo.dhgE;

	return(TRUE);
}

/*=================================================================================================
** @Name      : u16 RcdGetErrRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 故障记录读取剩余总数目
** @Function  : 获取故障记录读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
u16 RcdGetErrRcdReadRemNumAPI(void)
{
	/*故障记录未使能*/
	if(0 == gRcdPara.errRcdEn)
	{
		return(0);
	}

	/*故障记录存储使能则从存储区读取*/
	if(1 == gRcdPara.errStoEn)
	{
		return(sErrRcdStorState[eRcdStorState_allNum]);
	}
	else
	{
		return(sErrRcdBuffState.rcdNum);
	}
}

/*=================================================================================================
** @Name      : u16 RcdGetEventRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 事件记录读取剩余总数目
** @Function  : 获取事件记录读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
u16 RcdGetEventRcdReadRemNumAPI(void)
{
	/*事件记录未使能*/
	if(0 == gRcdPara.evnRcdEn)
	{
		return(0);
	}

	/*事件记录存储使能则从存储区读取*/
	if(1 == gRcdPara.evnStoEn)
	{
		return(sEvnRcdStorState[eRcdStorState_allNum]);
	}
	else
	{
		return(sEvnRcdBuffState.rcdNum);
	}
}

/*=================================================================================================
** @Name      : u16 RcdGetWaveRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 故障录波读取剩余总数目
** @Function  : 获取故障录波读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
u16 RcdGetWaveRcdReadRemNumAPI(void)
{
	/*故障录波未使能*/
	if(0 == gRcdPara.wavRcdEn)
	{
		return(0);
	}

	/*故障录波存储使能则从存储区读取*/
	if(1 == gRcdPara.wavStoEn)
	{
		return(sWavRcdStorState[eRcdStorState_allNum]);
	}
	else
	{
		/*缓存区录波完成*/
		if(eFatWavState_Fin == RcdGetFaultWaveStateAPI())
		{
			return(1);
		}
		else
		{
			return(0);
		}
	}
}

/*=================================================================================================
** @Name      : u16 RcdGetDodRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 历史DOD记录读取剩余总数目
** @Function  : 获取历史DOD记录读取剩余总数目
** @The notes : 当前记录剩余数目(已读取的已清除,非按天存储)
**===============================================================================================*/
u16 RcdGetDodRcdReadRemNumAPI(void)
{
	/*DOD存储使能则从存储区读取*/
	if(1 == gRcdPara.dodStoEn)
	{
		return(sDodRcdStorState[eRcdStorState_remNum]);
	}
	else
	{
		/*缓存区有值*/
		if(0xffff != sDodRcdBuffInfo.dod)
		{
			return(1);
		}
		else
		{
			return(0);
		}
	}
}

/*=================================================================================================
** @Name      : u8 RcdGetDataRcdNowDaysAPI(void)
** @Input     : void
** @Output    : 数据记录当前记录天数[0,31]
** @Function  : 获取数据记录当前记录天数
** @The notes :
**===============================================================================================*/
u8 RcdGetDataRcdNowDaysAPI(void)
{
	return(sDatRcdStorState[eRcdStorState_allDay]);
}

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdAllNumAPI(void)
** @Input     : void
** @Output    : 数据记录当前总记录条数
** @Function  : 获取数据记录当前总记录条数
** @The notes :
**===============================================================================================*/
u16 RcdGetDataRcdAllNumAPI(void)
{
	return(sDatRcdStorState[eRcdStorState_allNum]);
}

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdDateNumAPI(u8 date)
** @Input     : date:日期[0,30]
** @Output    : 指定日期数据记录条数
** @Function  : 获取指定日期数据记录条数
** @The notes : 将日期转化为实际位置指针
**===============================================================================================*/
u16 RcdGetDataRcdDateNumAPI(u8 date)
{
	u8 index = 0;

	if(date < sDatRcdStorState[eRcdStorState_allDay])
	{
		/*日期在当前写位置前面*/
		if(date <= sDatRcdStorState[eRcdStorState_wDayP])
		{
			/*写当日作为第0天*/
			index = sDatRcdStorState[eRcdStorState_wDayP] - date;
		}
		/*显示日期在当前所写日期之后*/
		else
		{
			/*未写满31天,目标日期越界*/
			if(sDatRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
			{
				return(0);
			}
			/*写满31天则从最后往前推*/
			else
			{
				index = date - sDatRcdStorState[eRcdStorState_wDayP];
				index = RCD_STOR_MAXDAYS - index;
			}
		}

		return(sDatRcdStorState[eRcdStorState_eDayNum + index]);
	}
	else
	{
		return(0);
	}
}

/*=================================================================================================
** @Name      : u8 RcdGetDataRcdDisplayDateAPI(void)
** @Input     : void
** @Output    : 数据记录当前显示记录日期[0,30]
** @Function  : 获取数据记录当前显示记录日期
** @The notes : 将当前位置指针转化为日期
**===============================================================================================*/
u8 RcdGetDataRcdDisplayDateAPI(void)
{
	u8 date = 0;

	/*正在显示位置在写指针前面*/
	if(sDatRcdDispState.dDPoint <= sDatRcdStorState[eRcdStorState_wDayP])
	{
		/*写当日作为第0天*/
		date = sDatRcdStorState[eRcdStorState_wDayP] - sDatRcdDispState.dDPoint;
	}
	/*正在显示位置在写指针后面*/
	else
	{
		/*未写满31天,目标日期越界*/
		if(sDatRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
			return(0);
		}
		/*写满31天则从最后往前推*/
		else
		{
			date = RCD_STOR_MAXDAYS - sDatRcdDispState.dDPoint;
			date = date + sDatRcdStorState[eRcdStorState_wDayP];
		}
	}

	return(date);
}

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdDisPlayRemNumAPI(void)
** @Input     : void
** @Output    : 数据记录显示当天剩余记录条数
** @Function  : 获取数据记录显示当天剩余记录条数
** @The notes :
**===============================================================================================*/
u16 RcdGetDataRcdDisPlayRemNumAPI(void)
{
	return(sDatRcdDispState.dDaNum);
}

/*=================================================================================================
** @Name      : u8 RcdGetWavRcdStorReadFlagAPI(void)
** @Input     : void
** @Output    : 故障录波存储读写状态标志 0:未读取(可存储) 1:开始读取 2:正在读取
** @Function  : 获取故障录波存储读写状态标志
** @The notes :
**===============================================================================================*/
u8 RcdGetWavRcdStorReadFlagAPI(void)
{
	return(sWavRcdStorReadBusyFlag);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u32 RcdCalcDataRcdAimAddr(u8 date, u8 sec, u16 num)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录) date:第几天 sec:扇区号 num:记录序号
** @Output    : 记录存储写目标地址
** @Function  : 计算记录存储目标地址
** @The notes : 根据读写指针计算故障记录/事件记录/数据记录存储地址(均为U16的数据)
**===============================================================================================*/
static u32 RcdCalcRecordAimAddr(u8 sort, u8 date, u8 sec, u16 num)
{
    u32 regAddr = 0;

    /*故障记录*/
    if(0 == sort)
    {
        regAddr = RCD_ERRWD_START_ADDR + ((u32)date * RCD_ERRWD_DSEC_NUM * RCD_ERRWD_SEC_LEN) + ((u32)sec * RCD_ERRWD_SEC_LEN) + ((u32)num * RCD_ERRINFO_LEN * 2);
    }
    /*事件记录*/
    else if(1 == sort)
    {
    	regAddr = RCD_EVNWD_START_ADDR + ((u32)date * RCD_EVNWD_DSEC_NUM * RCD_EVNWD_SEC_LEN) + ((u32)sec * RCD_EVNWD_SEC_LEN) + ((u32)num * RCD_EVNINFO_LEN * 2);
    }
    /*数据记录*/
    else if(2 == sort)
    {
    	regAddr = RCD_DATWD_START_ADDR + ((u32)date * RCD_DATWD_DSEC_NUM * RCD_DATWD_SEC_LEN) + ((u32)sec * RCD_DATWD_SEC_LEN) + ((u32)num * RCD_DATINFO_LEN * 2);
    }
    /*故障录波记录*/
    else if(3 == sort)
    {
    	regAddr = RCD_WAVWD_START_ADDR + ((u32)date * RCD_WAVWD_DSEC_NUM * RCD_WAVWD_SEC_LEN) + ((u32)sec * RCD_WAVWD_SEC_LEN) + ((u32)num * RCD_WAVINFO_LEN * 2);
    }
    /*DOD记录(DOD不分日期记录)*/
    else if(4 == sort)
    {
    	regAddr = RCD_DODWD_START_ADDR + ((u32)sec * RCD_DODWD_SEC_LEN) + ((u32)num * RCD_DODINFO_LEN * 2);
    }
    /*错误*/
    else
    {
    	regAddr = 0xffffffff;
    }

    return(regAddr);
}

/*=================================================================================================
** @Name      : static void RcdWriteRecodStoreStateInfo(u8 sort)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录)
** @Output    : void
** @Function  : 写记录存储状态信息到Flash
** @The notes : 包括有效标志/读写指针/每天记录起始扇区/每天记录数目
**===============================================================================================*/
static void RcdWriteRecodStoreStateInfo(u8 sort)
{
	u16 *rcdState;
	u32 addr = 0;
	u8 len = 0;

    /*故障记录*/
    if(0 == sort)
    {
    	len = RCD_STA_INFO_STOR_LEN;
    	addr = RCD_ERRWS_START_ADDR;
    	rcdState = sErrRcdStorState;
    }
    /*事件记录*/
    else if(1 == sort)
    {
    	len = RCD_STA_INFO_STOR_LEN;
    	addr = RCD_EVNWS_START_ADDR;
    	rcdState = sEvnRcdStorState;
    }
    /*数据记录*/
    else if(2 == sort)
    {
    	len = RCD_STA_INFO_STOR_LEN;
    	addr = RCD_DATWS_START_ADDR;
    	rcdState = sDatRcdStorState;
    }
    /*故障录波记录*/
    else if(3 == sort)
    {
    	len = RCD_STA_INFO_STOR_LEN;
    	addr = RCD_WAVWS_START_ADDR;
    	rcdState = sWavRcdStorState;
    }
    /*DOD记录*/
    else if(4 == sort)
    {
    	len = RCD_STOR_WRP_LEN;
    	addr = RCD_DODWS_START_ADDR;
    	rcdState = sDodRcdStorState;
    }
    /*错误*/
    else
    {
    	return;
    }

    /*记录有效标志*/
    rcdState[eRcdStorState_flag] = RCD_STOR_STATE_FLAG;

    /*先擦除再写入*/
    (void)RcdEraseRcdStoreSectorHook(sort, addr);
    /*读写指针写FLASH存储*/
    (void)RcdWriteRcdStoreInfoHook(sort, addr, rcdState, len);
}

/*=================================================================================================
** @Name      : static void RcdResetNowDayStoreReady(u8 sort)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录)
** @Output    : void
** @Function  : 复位当天存储区为就绪状态
** @The notes : 擦除当天需要写的FLASH,当天记录数目为0才能调用
**===============================================================================================*/
static void RcdResetNowDayStoreReady(u8 sort)
{
//	u8 i = 0;
//	u32 regAddr = 0;

    /*故障记录*/
    if(0 == sort)
    {
		/*重置写指针*/
		sErrRcdStorState[eRcdStorState_wSecP] = 0;
		sErrRcdStorState[eRcdStorState_wNumP] = 0;

		/*重置当天记录数*/
		sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_wDayP]] = 0;
		sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]] = 0;

//		/*一次擦除该天所有扇区*/
//		for(i = 0; i < RCD_ERRWD_DSEC_NUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(0, sErrRcdStorState[eRcdStorState_wDayP], i, 0);
//			(void)RcdEraseRcdStoreSectorHook(0, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(0);

		/*分多次擦除该天所有扇区*/
		(void)RcdEraseOneDayStoreReady(sort, 0);
    }
    /*事件记录*/
    else if(1 == sort)
    {
		/*重置写指针*/
    	sEvnRcdStorState[eRcdStorState_wSecP] = 0;
    	sEvnRcdStorState[eRcdStorState_wNumP] = 0;

		/*重置当天记录数*/
		sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] = 0;
		sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] = 0;

//		/*一次擦除该天所有扇区*/
//		for(i = 0; i < RCD_EVNWD_DSEC_NUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(1, sEvnRcdStorState[eRcdStorState_wDayP], i, 0);
//			(void)RcdEraseRcdStoreSectorHook(1, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(1);

		/*分多次擦除该天所有扇区*/
		(void)RcdEraseOneDayStoreReady(sort, 0);
    }
    /*数据记录*/
    else if(2 == sort)
    {
		/*重置写指针*/
    	sDatRcdStorState[eRcdStorState_wSecP] = 0;
    	sDatRcdStorState[eRcdStorState_wNumP] = 0;

		/*重置当天记录数*/
    	sDatRcdStorState[eRcdStorState_dayFSec + (u8)sDatRcdStorState[eRcdStorState_wDayP]] = 0;
    	sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]] = 0;

//		/*一次擦除该天所有扇区*/
//		for(i = 0; i < RCD_DATWD_DSEC_NUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(2, sDatRcdStorState[eRcdStorState_wDayP], i, 0);
//			(void)RcdEraseRcdStoreSectorHook(2, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(2);

		/*分多次擦除该天所有扇区*/
    	(void)RcdEraseOneDayStoreReady(sort, 0);
    }
    /*故障录波记录*/
    else if(3 == sort)
    {
		/*重置写指针*/
    	sWavRcdStorState[eRcdStorState_wSecP] = 0;
    	sWavRcdStorState[eRcdStorState_wNumP] = 0;

		/*重置当天记录数*/
    	sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_wDayP]] = 0;
    	sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]] = 0;

//		/*一次擦除该天所有扇区*/
//		for(i = 0; i < RCD_DATWD_DSEC_NUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(3, sWavRcdStorState[eRcdStorState_wDayP], i, 0);
//			(void)RcdEraseRcdStoreSectorHook(3, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(3);

		/*分多次擦除该天所有扇区*/
    	(void)RcdEraseOneDayStoreReady(sort, 0);
    }
    /*DOD记录*/
    else if(4 == sort)
    {
		/*重置写指针*/
    	sDodRcdStorState[eRcdStorState_wSecP] = 0;
    	sDodRcdStorState[eRcdStorState_wNumP] = 0;

		/*重置读指针*/
    	sDodRcdStorState[eRcdStorState_rSecP] = 0;
    	sDodRcdStorState[eRcdStorState_rNumP] = 0;

//		/*一次擦除所有扇区*/
//		for(i = 0; i < RCD_DODWD_ASECNUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(4, 0, i, 0);
//			(void)RcdEraseRcdStoreSectorHook(4, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(4);

		/*分多次擦除该天所有扇区*/
    	(void)RcdEraseOneDayStoreReady(sort, 0);
    }
}

/*=================================================================================================
** @Name      : static void RcdResetNextDayStoreReady(u8 sort)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波)
** @Output    : void
** @Function  : 复位下一天存储为就绪状态
** @The notes : 写指针下移1天,擦除一天需要写的FLASH(DOD不按天存储无需准备下一天)
**===============================================================================================*/
static void RcdResetNextDayStoreReady(u8 sort)
{
//	u8 i = 0;
//	u32 regAddr = 0;

    /*故障记录*/
    if(0 == sort)
    {
    	/*写指针下移1天*/
    	sErrRcdStorState[eRcdStorState_wDayP]++;
    	sErrRcdStorState[eRcdStorState_wSecP] = 0;
		sErrRcdStorState[eRcdStorState_wNumP] = 0;

    	/*写指针到最后一天*/
		if(sErrRcdStorState[eRcdStorState_wDayP] >= RCD_STOR_MAXDAYS)
		{
	    	/*写指针回到初始位置*/
			sErrRcdStorState[eRcdStorState_wDayP] = 0;
		}

    	/*记录天数未达到最大天数*/
		if(sErrRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
	    	/*记录天数累加*/
			sErrRcdStorState[eRcdStorState_allDay]++;
		}

		/*记录总数目少当天数目,该天记录清零*/
		sErrRcdStorState[eRcdStorState_allNum] -= sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]];
		sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]] = 0;
		sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_wDayP]] = 0;

		/*读指针指向当天记录*/
		if(sErrRcdStorState[eRcdStorState_rDayP] == sErrRcdStorState[eRcdStorState_wDayP])
		{
	    	/*读指针下移1天*/
			sErrRcdStorState[eRcdStorState_rDayP]++;

	    	/*读指针到最后一天*/
			if(sErrRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
			{
		    	/*读指针回到初始位置*/
				sErrRcdStorState[eRcdStorState_rDayP] = 0;
			}

			/*读起始扇区*/
			sErrRcdStorState[eRcdStorState_rSecP] = sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
			sErrRcdStorState[eRcdStorState_rNumP] = 0;

			/*读剩余数目为当天记录数目*/
			sErrRcdStorState[eRcdStorState_remNum] = sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_rDayP]];
		}

		/*日期变化则显示最新记录*/
		sErrRcdDispState.dDPoint = sErrRcdStorState[eRcdStorState_wDayP];
		sErrRcdDispState.dSPoint = 0;
		sErrRcdDispState.dNPoint = 0;
		sErrRcdDispState.dDaNum = 0;

//		/*一次擦除该天所有扇区*/
//		for(i = 0; i < RCD_ERRWD_DSEC_NUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(0, sErrRcdStorState[eRcdStorState_wDayP], i, 0);
//			(void)RcdEraseRcdStoreSectorHook(0, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(0);

		/*分多次擦除该天所有扇区*/
		(void)RcdEraseOneDayStoreReady(sort, 0);
    }
    /*事件记录*/
    else if(1 == sort)
    {
    	/*写指针下移1天*/
    	sEvnRcdStorState[eRcdStorState_wDayP]++;
		sEvnRcdStorState[eRcdStorState_wSecP] = 0;
		sEvnRcdStorState[eRcdStorState_wNumP] = 0;

    	/*写指针到最后一天*/
		if(sEvnRcdStorState[eRcdStorState_wDayP] >= RCD_STOR_MAXDAYS)
		{
			/*写指针回到初始位置*/
			sEvnRcdStorState[eRcdStorState_wDayP] = 0;
		}

		/*记录天数未达到最大天数*/
		if(sEvnRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
			/*记录天数累加*/
			sEvnRcdStorState[eRcdStorState_allDay]++;
		}

		/*记录总数目少当天数目,该天记录清零*/
		sEvnRcdStorState[eRcdStorState_allNum] -= sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]];
		sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] = 0;
		sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] = 0;

		/*读指针指向当天记录*/
		if(sEvnRcdStorState[eRcdStorState_rDayP] == sEvnRcdStorState[eRcdStorState_wDayP])
		{
	    	/*读指针下移1天*/
			sEvnRcdStorState[eRcdStorState_rDayP]++;

	    	/*读指针到最后一天*/
			if(sEvnRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
			{
		    	/*读指针回到初始位置*/
				sEvnRcdStorState[eRcdStorState_rDayP] = 0;
			}

			/*读起始扇区*/
			sEvnRcdStorState[eRcdStorState_rSecP] = sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
			sEvnRcdStorState[eRcdStorState_rNumP] = 0;

			/*读剩余数目为当天记录数目*/
			sEvnRcdStorState[eRcdStorState_remNum] = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_rDayP]];
		}

		/*日期变化则显示最新记录*/
		sEvnRcdDispState.dDPoint = sEvnRcdStorState[eRcdStorState_wDayP];
		sEvnRcdDispState.dSPoint = 0;
		sEvnRcdDispState.dNPoint = 0;
		sEvnRcdDispState.dDaNum = 0;

//		/*一次擦除该天所有扇区*/
//		for(i = 0; i < RCD_EVNWD_DSEC_NUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(1, sEvnRcdStorState[eRcdStorState_wDayP], i, 0);
//			(void)RcdEraseRcdStoreSectorHook(1, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(1);

		/*分多次擦除该天所有扇区*/
		(void)RcdEraseOneDayStoreReady(sort, 0);
    }
    /*数据记录*/
    else if(2 == sort)
    {
    	/*写指针下移1天*/
		sDatRcdStorState[eRcdStorState_wDayP]++;
		sDatRcdStorState[eRcdStorState_wSecP] = 0;
		sDatRcdStorState[eRcdStorState_wNumP] = 0;

    	/*写指针到最后一天*/
		if(sDatRcdStorState[eRcdStorState_wDayP] >= RCD_STOR_MAXDAYS)
		{
			/*写指针回到初始位置*/
			sDatRcdStorState[eRcdStorState_wDayP] = 0;
		}

    	/*记录天数未达到最大天数*/
		if(sDatRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
			/*记录天数累加*/
			sDatRcdStorState[eRcdStorState_allDay]++;
		}

		/*记录总数目少当天数目,该天记录清零*/
		sDatRcdStorState[eRcdStorState_allNum] -= sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]];
		sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]] = 0;
		sDatRcdStorState[eRcdStorState_dayFSec + (u8)sDatRcdStorState[eRcdStorState_wDayP]] = 0;

		/*读指针指向当天记录*/
		if(sDatRcdStorState[eRcdStorState_rDayP] == sDatRcdStorState[eRcdStorState_wDayP])
		{
	    	/*读指针下移1天*/
			sDatRcdStorState[eRcdStorState_rDayP]++;

	    	/*读指针到最后一天*/
			if(sDatRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
			{
		    	/*读指针回到初始位置*/
				sDatRcdStorState[eRcdStorState_rDayP] = 0;
			}

			/*读起始扇区*/
			sDatRcdStorState[eRcdStorState_rSecP] = sDatRcdStorState[eRcdStorState_dayFSec + (u8)sDatRcdStorState[eRcdStorState_rDayP]];
			sDatRcdStorState[eRcdStorState_rNumP] = 0;

			/*读剩余数目为当天记录数目*/
			sDatRcdStorState[eRcdStorState_remNum] = sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_rDayP]];
		}

		/*日期变化则显示最新记录*/
		sDatRcdDispState.dDPoint = sDatRcdStorState[eRcdStorState_wDayP];
		sDatRcdDispState.dSPoint = 0;
		sDatRcdDispState.dNPoint = 0;
		sDatRcdDispState.dDaNum = 0;

//		/*一次擦除该天所有扇区*/
//		for(i = 0; i < RCD_DATWD_DSEC_NUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(2, sDatRcdStorState[eRcdStorState_wDayP], i, 0);
//			(void)RcdEraseRcdStoreSectorHook(2, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(2);

		/*分多次擦除该天所有扇区*/
		(void)RcdEraseOneDayStoreReady(sort, 0);
    }
    /*故障录波记录*/
    else if(3 == sort)
    {
    	/*写指针下移1天*/
		sWavRcdStorState[eRcdStorState_wDayP]++;
		sWavRcdStorState[eRcdStorState_wSecP] = 0;
		sWavRcdStorState[eRcdStorState_wNumP] = 0;

    	/*写指针到最后一天*/
		if(sWavRcdStorState[eRcdStorState_wDayP] >= RCD_STOR_MAXDAYS)
		{
			/*写指针回到初始位置*/
			sWavRcdStorState[eRcdStorState_wDayP] = 0;
		}

    	/*记录天数未达到最大天数*/
		if(sWavRcdStorState[eRcdStorState_allDay] < RCD_STOR_MAXDAYS)
		{
			/*记录天数累加*/
			sWavRcdStorState[eRcdStorState_allDay]++;
		}

		/*记录总数目少当天数目,该天记录清零*/
		sWavRcdStorState[eRcdStorState_allNum] -= sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]];
		sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]] = 0;
		sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_wDayP]] = 0;

		/*读指针指向当天记录*/
		if(sWavRcdStorState[eRcdStorState_rDayP] == sWavRcdStorState[eRcdStorState_wDayP])
		{
	    	/*读指针下移1天*/
			sWavRcdStorState[eRcdStorState_rDayP]++;

	    	/*读指针到最后一天*/
			if(sWavRcdStorState[eRcdStorState_rDayP] >= RCD_STOR_MAXDAYS)
			{
		    	/*读指针回到初始位置*/
				sWavRcdStorState[eRcdStorState_rDayP] = 0;
			}

			/*读起始扇区*/
			sWavRcdStorState[eRcdStorState_rSecP] = sWavRcdStorState[eRcdStorState_dayFSec + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
			sWavRcdStorState[eRcdStorState_rNumP] = 0;

			/*读剩余数目为当天记录数目*/
			sWavRcdStorState[eRcdStorState_remNum] = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_rDayP]];
		}

		/*日期变化则显示最新记录*/
		sWavRcdDispState.dDPoint = sWavRcdStorState[eRcdStorState_wDayP];
		sWavRcdDispState.dSPoint = 0;
		sWavRcdDispState.dNPoint = 0;
		sWavRcdDispState.dDaNum = 0;

//		/*一次擦除该天所有扇区*/
//		for(i = 0; i < RCD_WAVWD_DSEC_NUM; i++)
//		{
//			/*计算当前写地址,擦除该扇区*/
//			regAddr = RcdCalcRecordAimAddr(3, sWavRcdStorState[eRcdStorState_wDayP], i, 0);
//			(void)RcdEraseRcdStoreSectorHook(3, regAddr);
//		}
//
//		/*更新记录状态*/
//		RcdWriteRecodStoreStateInfo(3);

		/*分多次擦除该天所有扇区*/
		(void)RcdEraseOneDayStoreReady(sort, 0);
    }
}

/*=================================================================================================
** @Name      : static u8 RcdEraseOneDayStoreReady(u8 sort, u8 step)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录) step:擦除步骤(0起始 1非开始擦除)
** @Output    : TRUE:擦除完成 FALSE:未擦完(需再次执行该函数)
** @Function  : 分多次擦除一天存储扇区为就绪状态
** @The notes : 擦除当天所有FLASH后重新写记录状态信息
**===============================================================================================*/
static u8 RcdEraseOneDayStoreReady(u8 sort, u8 step)
{
	u8 i = 0;
	u8 secNum = 0;
	u32 regAddr = 0;
	static u8 sSecId[5] = {0};

    /*故障记录*/
    if(0 == sort)
    {
		/*首次擦除*/
    	if(0 == step)
    	{
    		/*当天0扇区开始*/
    		sSecId[sort] = 0;
    	}
		/*擦除完成*/
    	else if(sSecId[sort] >= RCD_ERRWD_DSEC_NUM)
    	{
			sSecId[sort] = 0;
			return(TRUE);
    	}

    	/*每次擦2扇区*/
		//secNum = 2;
		secNum = 1;
		if(secNum > (RCD_ERRWD_DSEC_NUM - sSecId[sort]))
		{
			secNum = RCD_ERRWD_DSEC_NUM - sSecId[sort];
		}

		/*分次擦除该天所有扇区*/
		for(i = sSecId[sort]; i < (sSecId[sort] + secNum); i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(0, sErrRcdStorState[eRcdStorState_wDayP], i, 0);
			(void)RcdEraseRcdStoreSectorHook(0, regAddr);
		}

		sSecId[sort] += secNum;

		/*擦除完成*/
		/*不需要擦除一天的扇区,只需擦除一个扇区写一个扇区(连续擦除可能导致任务调度异常)*/
		#if(1 == RCD_CLR_ALLFLASH_EN)
		if(sSecId[sort] >= RCD_ERRWD_DSEC_NUM)
		#endif
		{
			sSecId[sort] = 0;

			/*更新记录状态*/
			RcdWriteRecodStoreStateInfo(0);

			return(TRUE);
		}
    }
    /*事件记录*/
    else if(1 == sort)
    {
		/*首次擦除*/
    	if(0 == step)
    	{
    		/*当天0扇区开始*/
    		sSecId[sort] = 0;
    	}
		/*擦除完成*/
    	else if(sSecId[sort] >= RCD_EVNWD_DSEC_NUM)
    	{
			sSecId[sort] = 0;
			return(TRUE);
    	}

    	/*每次擦2扇区*/
		//secNum = 2;
		secNum = 1;
		if(secNum > (RCD_EVNWD_DSEC_NUM - sSecId[sort]))
		{
			secNum = RCD_EVNWD_DSEC_NUM - sSecId[sort];
		}

		/*分次擦除该天所有扇区*/
		for(i = sSecId[sort]; i < (sSecId[sort] + secNum); i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(1, sEvnRcdStorState[eRcdStorState_wDayP], i, 0);
			(void)RcdEraseRcdStoreSectorHook(1, regAddr);
		}

		sSecId[sort] += secNum;

		/*擦除完成*/
		/*不需要擦除一天的扇区,只需擦除一个扇区写一个扇区(连续擦除可能导致任务调度异常)*/
		#if(1 == RCD_CLR_ALLFLASH_EN)
		if(sSecId[sort] >= RCD_EVNWD_DSEC_NUM)
		#endif
		{
			sSecId[sort] = 0;

			/*更新记录状态*/
			RcdWriteRecodStoreStateInfo(1);

			return(TRUE);
		}
    }
    /*数据记录*/
    else if(2 == sort)
    {
		/*首次擦除*/
    	if(0 == step)
    	{
    		/*当天0扇区开始*/
    		sSecId[sort] = 0;
    	}
		/*擦除完成*/
    	else if(sSecId[sort] >= RCD_DATWD_DSEC_NUM)
    	{
			sSecId[sort] = 0;
			return(TRUE);
    	}

    	/*每次擦2扇区*/
		//secNum = 2;
		secNum = 1;
		if(secNum > (RCD_DATWD_DSEC_NUM - sSecId[sort]))
		{
			secNum = RCD_DATWD_DSEC_NUM - sSecId[sort];
		}

		/*分次擦除该天所有扇区*/
		for(i = sSecId[sort]; i < (sSecId[sort] + secNum); i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(2, sDatRcdStorState[eRcdStorState_wDayP], i, 0);
			(void)RcdEraseRcdStoreSectorHook(2, regAddr);
		}

		sSecId[sort] += secNum;

		/*擦除完成*/
		/*不需要擦除一天的扇区,只需擦除一个扇区写一个扇区(连续擦除可能导致任务调度异常)*/
		#if(1 == RCD_CLR_ALLFLASH_EN)
		if(sSecId[sort] >= RCD_DATWD_DSEC_NUM)
		#endif
		{
			sSecId[sort] = 0;

			/*更新记录状态*/
			RcdWriteRecodStoreStateInfo(2);

			return(TRUE);
		}
    }
    /*故障录波记录*/
    else if(3 == sort)
    {
		/*首次擦除*/
    	if(0 == step)
    	{
    		/*当天0扇区开始*/
    		sSecId[sort] = 0;
    	}
		/*擦除完成*/
    	else if(sSecId[sort] >= RCD_WAVWD_DSEC_NUM)
    	{
			sSecId[sort] = 0;
			return(TRUE);
    	}

    	/*每次擦2扇区*/
		//secNum = 2;
		secNum = 1;
		if(secNum > (RCD_WAVWD_DSEC_NUM - sSecId[sort]))
		{
			secNum = RCD_WAVWD_DSEC_NUM - sSecId[sort];
		}

		/*分次擦除该天所有扇区*/
		for(i = sSecId[sort]; i < (sSecId[sort] + secNum); i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(3, sWavRcdStorState[eRcdStorState_wDayP], i, 0);
			(void)RcdEraseRcdStoreSectorHook(3, regAddr);
		}

		sSecId[sort] += secNum;

		/*擦除完成*/
		/*不需要擦除一天的扇区,只需擦除一个扇区写一个扇区(连续擦除可能导致任务调度异常)*/
		#if(1 == RCD_CLR_ALLFLASH_EN)
		if(sSecId[sort] >= RCD_WAVWD_DSEC_NUM)
		#endif
		{
			sSecId[sort] = 0;

			/*更新记录状态*/
			RcdWriteRecodStoreStateInfo(3);

			return(TRUE);
		}
    }
    /*DOD记录*/
    else if(4 == sort)
    {
		/*首次擦除*/
    	if(0 == step)
    	{
    		/*当天0扇区开始*/
    		sSecId[sort] = 0;
    	}
		/*擦除完成*/
    	else if(sSecId[sort] >= RCD_DODWD_ASECNUM)
    	{
			sSecId[sort] = 0;
			return(TRUE);
    	}

    	/*每次擦4扇区*/
		//secNum = 4;
		secNum = 1;
		if(secNum > (RCD_DODWD_ASECNUM - sSecId[sort]))
		{
			secNum = RCD_DODWD_ASECNUM - sSecId[sort];
		}

		/*分次擦除该天所有扇区*/
		for(i = sSecId[sort]; i < (sSecId[sort] + secNum); i++)
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(4, 0, i, 0);
			(void)RcdEraseRcdStoreSectorHook(4, regAddr);
		}

		sSecId[sort] += secNum;

		/*擦除完成*/
		/*不需要擦除一天的扇区,只需擦除一个扇区写一个扇区(连续擦除可能导致任务调度异常)*/
		#if(1 == RCD_CLR_ALLFLASH_EN)
		if(sSecId[sort] >= RCD_DODWD_ASECNUM)
		#endif
		{
			sSecId[sort] = 0;

			/*更新记录状态*/
			RcdWriteRecodStoreStateInfo(4);

			return(TRUE);
		}
    }
    /*非指定记录类型*/
    else
    {
        return(TRUE);
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : static void RcdStoreErrRcdInfoDeal(t_RcdStoreInfo *storeInfo)
** @Input     : storeInfo:需要存储的记录
** @Output    : void
** @Function  : 存储一条故障记录
** @The notes : 将记录写FLASH
**===============================================================================================*/
static void RcdStoreErrRcdInfoDeal(t_RcdStoreInfo *storeInfo)
{
	u32 regAddr = 0;
	u16 data[RCD_ERRINFO_LEN] = {0};

	/*故障记录未使能和写就绪*/
	if((0 == gRcdPara.errStoEn) || (sErrRcdStorReadBusyFlag > 0))
	{
		return;
	}

	/*获得数据*/
	data[0] = storeInfo->id;
	data[1] = storeInfo->codeH;
	data[2] = storeInfo->codeL;
	data[3] = storeInfo->year;
	data[4] = storeInfo->MD;
	data[5] = storeInfo->HM;
	data[6] = storeInfo->sec;
	data[7] = storeInfo->info1;
	data[8] = storeInfo->info2;
	data[9] = storeInfo->info3;
	data[10] = storeInfo->info4;
	data[11] = storeInfo->info5;

	/*计算当前写地址,将信息写EEPROM或FLASH*/
	regAddr = RcdCalcRecordAimAddr(0, sErrRcdStorState[eRcdStorState_wDayP], sErrRcdStorState[eRcdStorState_wSecP], sErrRcdStorState[eRcdStorState_wNumP]);
	(void)RcdWriteRcdStoreInfoHook(0, regAddr, data, RCD_ERRINFO_LEN);

	/*记录数目增加*/
	sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]]++;
	sErrRcdStorState[eRcdStorState_allNum]++;

	/*未计算记录天数*/
	if(0 == sErrRcdStorState[eRcdStorState_allDay])
	{
    	/*记录天数累加*/
		sErrRcdStorState[eRcdStorState_allDay]++;
	}

	/*正在读当天数目*/
	if(sErrRcdStorState[eRcdStorState_rDayP] == sErrRcdStorState[eRcdStorState_wDayP])
	{
		/*当天读剩余数目增加*/
		sErrRcdStorState[eRcdStorState_remNum]++;
	}

	/*写指针下移一个*/
	sErrRcdStorState[eRcdStorState_wNumP]++;
	/*写满1个扇区*/
	if(sErrRcdStorState[eRcdStorState_wNumP] >= RCD_ERRWD_SECNUM)
	{
		/*写下一个扇区*/
		sErrRcdStorState[eRcdStorState_wNumP] = 0;
		sErrRcdStorState[eRcdStorState_wSecP]++;

		/*写到1天扇区末尾*/
		if(sErrRcdStorState[eRcdStorState_wSecP] >= RCD_ERRWD_DSEC_NUM)
		{
			/*返回起始扇区*/
			sErrRcdStorState[eRcdStorState_wSecP] = 0;

			/*当天记录起始扇区下移1个扇区*/
			sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_wDayP]] = 1;

			/*数目减少一个扇区数目*/
			sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]] -= RCD_ERRWD_SECNUM;
			sErrRcdStorState[eRcdStorState_allNum] -= RCD_ERRWD_SECNUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(0, sErrRcdStorState[eRcdStorState_wDayP], sErrRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(0, regAddr);
		}
		/*写满1天记录需要覆盖已写记录*/
		else if(sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]] >= RCD_ERRWD_DAYNUM)
		{
			/*当天记录起始扇区下移1个扇区*/
			sErrRcdStorState[eRcdStorState_dayFSec + (u8)sErrRcdStorState[eRcdStorState_wDayP]] = sErrRcdStorState[eRcdStorState_wSecP] + 1;

			/*数目减少一个扇区数目*/
			sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]] -= RCD_ERRWD_SECNUM;
			sErrRcdStorState[eRcdStorState_allNum] -= RCD_ERRWD_SECNUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(0, sErrRcdStorState[eRcdStorState_wDayP], sErrRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(0, regAddr);
		}
		/*未使能擦除所有FLASH扇区(每个扇区都要先擦除再写)*/
		#if(0 == RCD_CLR_ALLFLASH_EN)
		/*扇区未用完，进入下一个扇区*/
		else
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(0, sErrRcdStorState[eRcdStorState_wDayP], sErrRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(0, regAddr);
		}
		#endif

		/*写指针下移1个扇区后写正在读取的位置*/
		if((sErrRcdStorState[eRcdStorState_wDayP] == sErrRcdStorState[eRcdStorState_rDayP])
			&& (sErrRcdStorState[eRcdStorState_wSecP] == sErrRcdStorState[eRcdStorState_rSecP]))
		{
			/*读下移一个扇区*/
			sErrRcdStorState[eRcdStorState_rSecP]++;

			/*读到最后一个扇区*/
			if(sErrRcdStorState[eRcdStorState_rSecP] >= RCD_ERRWD_DSEC_NUM)
			{
				/*返回起始扇区*/
				sErrRcdStorState[eRcdStorState_rSecP] = 0;
			}

			/*剩余数目减少该扇区未读取的*/
			sErrRcdStorState[eRcdStorState_remNum] -= (RCD_ERRWD_SECNUM - sErrRcdStorState[eRcdStorState_rNumP]);

			/*读起始位置*/
			sErrRcdStorState[eRcdStorState_rNumP] = 0;
		}
	}

	/*正在显示当天数目*/
	if(sErrRcdDispState.dDPoint == sErrRcdStorState[eRcdStorState_wDayP])
	{
		/*更新当天显示记录为当天数目(重新显示)*/
		sErrRcdDispState.dDaNum = sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]];
	}

	/*重新记录故障记录状态信息*/
	RcdWriteRecodStoreStateInfo(0);
}

/*=================================================================================================
** @Name      : static void RcdStoreEventRcdInfoDeal(t_RcdStoreInfo *storeInfo)
** @Input     : storeInfo:需要存储的记录
** @Output    : void
** @Function  : 存储一条事件记录
** @The notes : 将记录写FLASH
**===============================================================================================*/
static void RcdStoreEventRcdInfoDeal(t_RcdStoreInfo *storeInfo)
{
	u32 regAddr = 0;
	u16 data[RCD_EVNINFO_LEN] = {0};

	/*事件记录未使能和写就绪*/
	if((0 == gRcdPara.evnStoEn) || (sEvnRcdStorReadBusyFlag > 0))
	{
		return;
	}

	/*获得数据*/
	data[0] = storeInfo->id;
	data[1] = storeInfo->codeH;
	data[2] = storeInfo->codeL;
	data[3] = storeInfo->year;
	data[4] = storeInfo->MD;
	data[5] = storeInfo->HM;
	data[6] = storeInfo->sec;
	data[7] = storeInfo->info1;
	data[8] = storeInfo->info2;
	data[9] = storeInfo->info3;
	data[10] = storeInfo->info4;
	data[11] = storeInfo->info5;

	/*计算当前写地址,将信息写EEPROM或FLASH*/
	regAddr = RcdCalcRecordAimAddr(1, sEvnRcdStorState[eRcdStorState_wDayP], sEvnRcdStorState[eRcdStorState_wSecP], sEvnRcdStorState[eRcdStorState_wNumP]);
	(void)RcdWriteRcdStoreInfoHook(1, regAddr, data, RCD_EVNINFO_LEN);

	/*记录数目增加*/
	sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]]++;
	sEvnRcdStorState[eRcdStorState_allNum]++;

	/*未计算记录天数*/
	if(0 == sEvnRcdStorState[eRcdStorState_allDay])
	{
    	/*记录天数累加*/
		sEvnRcdStorState[eRcdStorState_allDay]++;
	}

	/*正在读当天数目*/
	if(sEvnRcdStorState[eRcdStorState_rDayP] == sEvnRcdStorState[eRcdStorState_wDayP])
	{
		/*当天读剩余数目增加*/
		sEvnRcdStorState[eRcdStorState_remNum]++;
	}

	/*写指针下移一个*/
	sEvnRcdStorState[eRcdStorState_wNumP]++;
	/*写满1个扇区*/
	if(sEvnRcdStorState[eRcdStorState_wNumP] >= RCD_EVNWD_SECNUM)
	{
		/*写下一个扇区*/
		sEvnRcdStorState[eRcdStorState_wNumP] = 0;
		sEvnRcdStorState[eRcdStorState_wSecP]++;

		/*写到1天扇区末尾*/
		if(sEvnRcdStorState[eRcdStorState_wSecP] >= RCD_EVNWD_DSEC_NUM)
		{
			/*返回起始扇区*/
			sEvnRcdStorState[eRcdStorState_wSecP] = 0;

			/*当天记录起始扇区下移1个扇区*/
			sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] = 1;

			/*数目减少一个扇区数目*/
			sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] -= RCD_EVNWD_SECNUM;
			sEvnRcdStorState[eRcdStorState_allNum] -= RCD_EVNWD_SECNUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(1, sEvnRcdStorState[eRcdStorState_wDayP], sEvnRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(1, regAddr);
		}
		/*写满1天记录需要覆盖已写记录*/
		else if(sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] >= RCD_EVNWD_DAYNUM)
		{
			/*当天记录起始扇区下移1个扇区*/
			sEvnRcdStorState[eRcdStorState_dayFSec + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] = sEvnRcdStorState[eRcdStorState_wSecP] + 1;

			/*数目减少一个扇区数目*/
			sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] -= RCD_EVNWD_SECNUM;
			sEvnRcdStorState[eRcdStorState_allNum] -= RCD_EVNWD_SECNUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(1, sEvnRcdStorState[eRcdStorState_wDayP], sEvnRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(1, regAddr);
		}
		/*未使能擦除所有FLASH扇区(每个扇区都要先擦除再写)*/
		#if(0 == RCD_CLR_ALLFLASH_EN)
		/*扇区未用完，进入下一个扇区*/
		else
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(1, sEvnRcdStorState[eRcdStorState_wDayP], sEvnRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(1, regAddr);
		}
		#endif

		/*写指针下移1个扇区后写正在读取的位置*/
		if((sEvnRcdStorState[eRcdStorState_wDayP] == sEvnRcdStorState[eRcdStorState_rDayP])
			&& (sEvnRcdStorState[eRcdStorState_wSecP] == sEvnRcdStorState[eRcdStorState_rSecP]))
		{
			/*读下移一个扇区*/
			sEvnRcdStorState[eRcdStorState_rSecP]++;

			/*读到最后一个扇区*/
			if(sEvnRcdStorState[eRcdStorState_rSecP] >= RCD_EVNWD_DSEC_NUM)
			{
				/*返回起始扇区*/
				sEvnRcdStorState[eRcdStorState_rSecP] = 0;
			}

			/*剩余数目减少该扇区未读取的*/
			sEvnRcdStorState[eRcdStorState_remNum] -= (RCD_EVNWD_SECNUM - sEvnRcdStorState[eRcdStorState_rNumP]);

			/*读起始位置*/
			sEvnRcdStorState[eRcdStorState_rNumP] = 0;
		}
	}

	/*正在显示当天数目*/
	if(sEvnRcdDispState.dDPoint == sEvnRcdStorState[eRcdStorState_wDayP])
	{
		/*更新当天显示记录为当天数目(重新显示)*/
		sEvnRcdDispState.dDaNum = sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]];
	}

	/*重新记录事件记录状态信息*/
	RcdWriteRecodStoreStateInfo(1);
}

/*=================================================================================================
** @Name      : static void RcdStoreDodRcdInfoDeal(t_DodStoreInfo *storeInfo)
** @Input     : storeInfo:需记录的历史DOD记录信息
** @Output    : void
** @Function  : 存储一条历史DOD记录
** @The notes : 将记录写FLASH
**===============================================================================================*/
static void RcdStoreDodRcdInfoDeal(t_DodStoreInfo *storeInfo)
{
	u32 regAddr = 0;
	u16 data[RCD_DODINFO_LEN] = {0};

	/*DOD记录未使能和写就绪*/
	if((0 == gRcdPara.dodStoEn) || (sDodRcdStorReadBusyFlag > 0))
	{
		return;
	}

	/*获得数据*/
	data[0] = storeInfo->id;
	data[1] = storeInfo->year;
	data[2] = storeInfo->MD;
	data[3] = storeInfo->HM;
	data[4] = storeInfo->sec;
	data[5] = storeInfo->dod;
	data[6] = storeInfo->chgC;
	data[7] = storeInfo->dhgC;
	data[8] = storeInfo->chgE;
	data[9] = storeInfo->dhgE;

	/*计算当前写地址,将信息写EEPROM或FLASH*/
	regAddr = RcdCalcRecordAimAddr(4, 0, sDodRcdStorState[eRcdStorState_wSecP], sDodRcdStorState[eRcdStorState_wNumP]);
	(void)RcdWriteRcdStoreInfoHook(4, regAddr, data, RCD_DODINFO_LEN);

	/*记录数目增加*/
	sDodRcdStorState[eRcdStorState_allNum]++;

	/*当天读剩余数目增加*/
	sDodRcdStorState[eRcdStorState_remNum]++;

	/*写指针下移一个*/
	sDodRcdStorState[eRcdStorState_wNumP]++;
	/*写满1个扇区*/
	if(sDodRcdStorState[eRcdStorState_wNumP] >= RCD_DODWD_SEC_NUM)
	{
		/*写下一个扇区*/
		sDodRcdStorState[eRcdStorState_wNumP] = 0;
		sDodRcdStorState[eRcdStorState_wSecP]++;

		/*写到最末尾扇区*/
		if(sDodRcdStorState[eRcdStorState_wSecP] >= RCD_DODWD_ASECNUM)
		{
			/*返回起始扇区*/
			sDodRcdStorState[eRcdStorState_wSecP] = 0;

			/*总数目减少一个扇区数目*/
			sDodRcdStorState[eRcdStorState_allNum] -= RCD_DODWD_SEC_NUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(4, 0, sDodRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(4, regAddr);
		}
		/*写满记录需要覆盖已写记录*/
		else if(sDodRcdStorState[eRcdStorState_allNum] >= RCD_DODWD_ALLNUM)
		{
			/*总数目减少一个扇区数目*/
			sDodRcdStorState[eRcdStorState_allNum] -= RCD_DODWD_SEC_NUM;

			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(4, 0, sDodRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(4, regAddr);
		}
		/*未使能擦除所有FLASH扇区(每个扇区都要先擦除再写)*/
		#if(0 == RCD_CLR_ALLFLASH_EN)
		/*扇区未用完，进入下一个扇区*/
		else
		{
			/*计算当前写地址,擦除该扇区*/
			regAddr = RcdCalcRecordAimAddr(4, 0, sDodRcdStorState[eRcdStorState_wSecP], 0);
			(void)RcdEraseRcdStoreSectorHook(4, regAddr);
		}
		#endif

		/*写指针下移1个扇区后写正在读取的位置*/
		if(sDodRcdStorState[eRcdStorState_wSecP] == sDodRcdStorState[eRcdStorState_rSecP])
		{
			/*读下移一个扇区*/
			sDodRcdStorState[eRcdStorState_rSecP]++;
			/*读扇区起始位置*/
			sDodRcdStorState[eRcdStorState_rNumP] = 0;

			/*读到最后一个扇区*/
			if(sDodRcdStorState[eRcdStorState_rSecP] >= RCD_DODWD_ASECNUM)
			{
				/*返回起始扇区*/
				sDodRcdStorState[eRcdStorState_rSecP] = 0;
			}

			/*剩余数目减少该扇区未读取的*/
			sDodRcdStorState[eRcdStorState_remNum] -= (RCD_DODWD_SEC_NUM - sDodRcdStorState[eRcdStorState_rNumP]);
		}

		/*计算当前写地址,擦除该扇区(0记录时已全部擦除干净.仅覆盖写时擦除)*/
		//regAddr = RcdCalcRecordAimAddr(4, 0, sDodRcdStorState[eRcdStorState_wSecP], 0);
		//(void)RcdEraseRcdStoreSectorHook(4, regAddr);
	}

	/*更新显示记录为当前总记录数目(重新显示)*/
	sDodRcdDispState.dDaNum = sDodRcdStorState[eRcdStorState_allNum];

	/*重新记录DOD记录状态信息*/
	RcdWriteRecodStoreStateInfo(4);
}

/*=================================================================================================
** @Name      : static void RcdStoreFlashClrEventRcdAPI(u8 sort)
** @Input     : sort:记录类别(0故障记录 1事件记录 2数据记录 3故障录波 4DOD记录)
** @Output    : void
** @Function  : 事件存储空间FLASH擦写事件记录
** @The notes : 直接将事件写FLASH
**===============================================================================================*/
static void RcdStoreFlashClrEventRcdAPI(u8 sort)
{
	t_SysClock sysClock;
	t_RcdStoreInfo info;

	/*事件码信息*/
	info.id = sEvnRcdBuffState.idNum;
	info.codeH = ((u16)RcdGetGroupDevAddrHook() << 8) | eEVN_RCDSORT_SELF;
	info.codeL = EVN_RCD_SELFCODE_FST + 1 + sort;

	/*事件时间信息*/
	sysClock = GetSysDateClockAPI();
	info.year = sysClock.year;
	info.MD = ((u16)sysClock.month << 8) | sysClock.day;
	info.HM = ((u16)sysClock.hour << 8) | sysClock.minute;
	info.sec = sysClock.second;

	/*事件附加信息*/
	info.info1 = ((u16)RcdGetGroupProStateHook() << 8) | (RcdGetGroupWorkStateHook() & 0x00ff);
	info.info2 = RcdGetGroupOutCurrHook();

	/*故障记录*/
	if((0 == sort) || (5 == sort))
	{
		info.info3 = (sErrRcdStorState[eRcdStorState_wDayP] << 8) | (sErrRcdStorState[eRcdStorState_wSecP] & 0x00ff);
		info.info4 = ((u16)sErrRcdBuffState.rcdNum << 10) | (sErrRcdStorState[eRcdStorState_eDayNum + (u8)sErrRcdStorState[eRcdStorState_wDayP]] & 0x03ff);
		info.info5 = sErrRcdStorState[eRcdStorState_allNum];
	}
	/*事件记录*/
	else if((1 == sort) || (6 == sort))
	{
		info.info3 = (sEvnRcdStorState[eRcdStorState_wDayP] << 8) | (sEvnRcdStorState[eRcdStorState_wSecP] & 0x00ff);
		info.info4 = ((u16)sEvnRcdBuffState.rcdNum << 10) | (sEvnRcdStorState[eRcdStorState_eDayNum + (u8)sEvnRcdStorState[eRcdStorState_wDayP]] & 0x03ff);
		info.info5 = sEvnRcdStorState[eRcdStorState_allNum];
	}
	/*数据记录*/
	else if((2 == sort) || (7 == sort))
	{
		info.info3 = (sDatRcdStorState[eRcdStorState_wDayP] << 8) | (sDatRcdStorState[eRcdStorState_wSecP] & 0x00ff);
		info.info4 = sDatRcdStorState[eRcdStorState_eDayNum + (u8)sDatRcdStorState[eRcdStorState_wDayP]];
		info.info5 = sDatRcdStorState[eRcdStorState_allNum];
	}
	/*故障录波*/
	else if((3 == sort) || (8 == sort))
	{
		info.info3 = (sWavRcdStorState[eRcdStorState_wDayP] << 8) | (sWavRcdStorState[eRcdStorState_wSecP] & 0x00ff);
		info.info4 = sWavRcdStorState[eRcdStorState_eDayNum + (u8)sWavRcdStorState[eRcdStorState_wDayP]] & 0x03ff;
		if(eFatWavState_Fin == RcdGetFaultWaveStateAPI())
		{
			info.info4 |= 0x0400;
		}
		info.info5 = sWavRcdStorState[eRcdStorState_allNum];
	}
	/*DOD记录*/
	else
	{
		info.info3 = (sDodRcdStorState[eRcdStorState_wSecP] << 8) | (sDodRcdStorState[eRcdStorState_wNumP] & 0x00ff);
		if(0xffff != sDodRcdBuffInfo.dod)
		{
			info.info4 = 1;
		}
		else
		{
			info.info4 = 0;
		}
		info.info5 = sDodRcdStorState[eRcdStorState_allNum];
	}

	/*事件记录序号增1*/
	sEvnRcdBuffState.idNum++;

	/*将记录写存储*/
	RcdStoreEventRcdInfoDeal(&info);
}
