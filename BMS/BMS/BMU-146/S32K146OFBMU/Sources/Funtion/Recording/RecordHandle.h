/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : RecordHandle.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 事件/数据记录模块处理
** @Instructions : 故障事件、动作事件和历史数据记录
**===============================================================================================*/
#ifndef _RECORDHANDLE_H
#define _RECORDHANDLE_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define RCD_ERRINFO_LEN             12                                  /*一条故障记录长度(Word)*/
#define RCD_ERRBUFF_NUM             10                                  /*故障记录缓存条数*/
#define RCD_ERRBUFF_LEN             (RCD_ERRINFO_LEN*RCD_ERRBUFF_NUM)   /*故障记录缓存区长度(Word)*/

#define RCD_EVNINFO_LEN             12                                  /*一条事件记录长度(Word)*/
#define RCD_EVNBUFF_NUM             20                                  /*事件记录缓存条数*/
#define RCD_EVNBUFF_LEN             (RCD_ERRINFO_LEN*RCD_ERRBUFF_NUM)   /*事件记录缓存区长度(Word)*/

#define RCD_WAVINFO_LEN             56                                  /*一条故障录波长度(Word)*/
#define RCD_DATINFO_LEN             56                                  /*一条数据记录长度(Word)*/
#define RCD_DODINFO_LEN             10                                  /*一条DOD记录长度(Word)*/

#define RCD_STOR_MAXDAYS            31                                  /*记录存储最大存储天数*/
#define RCD_STOR_WRP_LEN            11                                  /*记录存储读写指针记录长度(包括存储有效标志)*/
#define RCD_STOR_STATE_FLAG         0x5a5a                              /*记录存储状态信息读写有效标志(Word)*/

#define RCD_STA_INFO_STOR_LEN       (RCD_STOR_WRP_LEN+RCD_STOR_MAXDAYS+RCD_STOR_MAXDAYS) /*故障记录存储状态信息读写指针存储长度(包括存储有效标志/读写指针/每天起始扇区指针/每天记录数目)*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
/*故障/事件记录缓冲区状态信息*/
typedef struct
{
    u8 wPoint;                          /*当前写指针*/
    u8 rPoint;                          /*当前读指针*/
    u8 idNum;                           /*编号序号*/
    u8 rcdNum;                          /*剩余记录条数*/
}t_RcdBuffState;

/*故障/事件/数据记录显示状态信息*/
typedef struct
{
    u8 dDPoint;                         /*当前显示日指针[0,30]*/
    u8 dSPoint;                         /*当前显示扇区指针[0,29]*/
    u16 dNPoint;                        /*当前显示扇区内序号指针[0,49]*/
    u16 dDaNum;                         /*显示当天剩余条数*/
}t_RcdDispState;

/*故障/事件记录信息格式*/
typedef struct
{
    u8 type;                            /*事件类型*/
    u16 code;                           /*事件识别码(包括前缀码)*/
    u16 info1;                          /*事件附加信息1*/
    u16 info2;                          /*事件附加信息2*/
    u16 info3;                          /*事件附加信息3*/
    u16 info4;                          /*事件附加信息4*/
    u16 info5;                          /*事件附加信息5*/
}t_RcdCreateInfo;

/*故障/事件记录存储格式*/
typedef struct
{
    u16 id;                             /*事件序号*/
    u16 codeH;                          /*事件码H*/
    u16 codeL;                          /*事件码L*/
    u16 year;                           /*事件时间-年*/
    u16 MD;                             /*事件时间-月/日*/
    u16 HM;                             /*事件时间-时/分*/
    u16 sec;                            /*事件时间-秒*/
    u16 info1;                          /*事件附加信息1*/
    u16 info2;                          /*事件附加信息2*/
    u16 info3;                          /*事件附加信息3*/
    u16 info4;                          /*事件附加信息4*/
    u16 info5;                          /*事件附加信息5*/
}t_RcdStoreInfo;

/*历史DOD记录存储格式*/
typedef struct
{
    u16 id;                             /*记录序号*/
    u16 year;                           /*记录时间-年*/
    u16 MD;                             /*记录时间-月/日*/
    u16 HM;                             /*记录时间-时/分*/
    u16 sec;                            /*记录时间-秒*/
    u16 dod;                            /*本次DOD值*/
    u16 chgC;                           /*充电电量(1AH)*/
    u16 dhgC;                           /*放电电量(1AH)*/
    u16 chgE;                           /*充电电能(0.1kWh)*/
    u16 dhgE;                           /*放电电能(0.1kWh)*/
}t_DodStoreInfo;

/*故障/事件记录缓冲区状态信息*/
typedef struct
{
    u8 errRcdEn;                        /*故障记录使能*/
    u8 errStoEn;                        /*故障存储使能*/
    u16 errStoDNum;                     /*故障存储一天最大记录条数*/

    u8 evnRcdEn;                        /*故障记录使能*/
    u8 evnStoEn;                        /*故障存储使能*/
    u16 evnStoDNum;                     /*故障存储一天最大记录条数*/

    u8 wavRcdEn;                        /*故障录波使能*/
    u8 wavStoEn;                        /*故障录波存储使能*/
    u16 wavStoDNum;                     /*故障录波存储一天最大记录条数*/

    u8 dataStoEn;                       /*历史数据存储使能*/

    u8 dodStoEn;                        /*统计DOD值存储使能*/
}t_RcdPara;

/*各天记录存储信息索引*/
typedef enum
{
	eRcdStorState_flag = 0,             /*有效标志[0x5a5a]*/
	eRcdStorState_wDate,                /*当前写日期[1,65535]*/

	eRcdStorState_wDayP,                /*当前写日指针[0,30]*/
	eRcdStorState_wSecP,                /*当前写扇区指针[0,29]*/
	eRcdStorState_wNumP,                /*当前写扇区内序号指针[0,49]*/

	eRcdStorState_rDayP,                /*当前读日指针[0,30]*/
	eRcdStorState_rSecP,                /*当前读扇区指针[0,29]*/
	eRcdStorState_rNumP,                /*当前读序号指针[0,49]*/

	eRcdStorState_allDay,               /*总存储天数*/
	eRcdStorState_allNum,               /*读取总剩余条数(DOD总记录数目)*/
	eRcdStorState_remNum,               /*读取当天剩余条数(DOD读取剩余数目)*/

	eRcdStorState_dayFSec = RCD_STOR_WRP_LEN,                       /*一天内记录起始扇区*/
	eRcdStorState_eDayNum = RCD_STOR_WRP_LEN + RCD_STOR_MAXDAYS,    /*一天内记录数目*/

	eRcdStorState_Len,			    	/*各天记录存储信息索引长度*/
}e_RcdStorStateInfo;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void RcdStoreStateInit(void)
** @Input     : void
** @Output    : void
** @Function  : 记录存储状态信息初始化(记录读写指针和数目)
** @The notes : 模块参数后/用户读写记录前调用
**===============================================================================================*/
void RcdStoreStateInit(void);

/*=================================================================================================
** @Name      : void RcdStoreHandleTask(void)
** @Input     : void
** @Output    : void
** @Function  : 记录存储处理任务
** @The notes : 600ms周期
**===============================================================================================*/
void RcdStoreHandleTask(void);

/*=================================================================================================
** @Name      : void RcdCreateErrRcdInfo_API(t_ RcdCreateInfo *rcdInfo)
** @Input     : rcdInfo:记录信息
** @Output    : void
** @Function  : 创建一条故障记录
** @The notes : 用户需要产生一条记录时调用(写队列缓存)
**===============================================================================================*/
void RcdCreateErrRcdInfoAPI(t_RcdCreateInfo *rcdInfo);

/*=================================================================================================
** @Name      : void RcdCreateErrRcdInfo_API(t_ RcdCreateInfo *rcdInfo)
** @Input     : rcdInfo:记录信息
** @Output    : void
** @Function  : 创建一条事件记录
** @The notes : 用户需要产生一条记录时调用(写队列缓存)
**===============================================================================================*/
void RcdCreateEventRcdInfoAPI(t_RcdCreateInfo *rcdInfo);

/*=================================================================================================
** @Name      : void RcdCreateDataRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:记录数据信息 len:记录长度
** @Output    : void
** @Function  : 创建一条运行数据记录
** @The notes : 用户需要产生一条记录时调用(直接写FLASH存储)
**===============================================================================================*/
void RcdCreateDataRcdInfoAPI(u16 *data, u8 len);

/*=================================================================================================
** @Name      : void RcdCreateWaveRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:记录数据信息 len:记录长度
** @Output    : void
** @Function  : 创建一条故障录波记录
** @The notes : 用户需要产生一条记录时调用(直接写FLASH存储)
**===============================================================================================*/
void RcdCreateWaveRcdInfoAPI(u16 *data, u8 len);

/*=================================================================================================
** @Name      : void RcdCreateDodRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:需记录的DOD值/充电量/放电量/充电能/放电能 len:记录信息个数(5个有效)
** @Output    : void
** @Function  : 创建一条DOD信息记录
** @The notes : 用户需要产生一条记录时调用(记录一条在缓存区)
**===============================================================================================*/
void RcdCreateDodRcdInfoAPI(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdReadOneErrRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的故障记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条故障记录
** @The notes : 读取存储区或缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneErrRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdReadOneEventRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条事件记录
** @The notes : 读取存储区或缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneEventRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdReadOneWaveRcdUserAPI(u8 conFlag, u16 *info, u8 len)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的故障录波记录信息 len:读信息长度
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条故障录波记录
** @The notes : 从最早记录开始读取存储区或缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneWaveRcdUserAPI(u8 conFlag, u16 *info, u8 len);

/*=================================================================================================
** @Name      : u8 RcdReadOneDodRcdUserAPI(u8 conFlag, t_DodStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的DOD记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条DOD记录
** @The notes : 从最早记录开始读取存储区或缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneDodRcdUserAPI(u8 conFlag, t_DodStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneErrRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) info:读取到的故障记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条故障记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneErrRcdUserAPI(u8 date, t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneEventRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条事件记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneEventRcdUserAPI(u8 date, t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneDataRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) data:读取到的数据记录 len:读取长度
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条数据记录
** @The notes : 从最新记录开始读取存储区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneDataRcdUserAPI(u8 date, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneWaveRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) data:读取到的故障录波记录 len:读取长度
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条故障录波记录
** @The notes : 从最新记录开始读取存储区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneWaveRcdUserAPI(u8 date, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneDodRcdUserAPI(t_DodStoreInfo *info)
** @Input     : info:读取到的DOD记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条DOD记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
u8 RcdDisplayOneDodRcdUserAPI(t_DodStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeErrRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日首页显示历史故障记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeErrRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeEventRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日首页显示历史事件记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeEventRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeDataRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日第一条显示历史数据记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeDataRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeWaveRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日第一条显示历史故障录波记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeWaveRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeDodRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到最新第一条显示历史DOD记录
** @The notes : 只移动读显示指针地址到当前最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
u8 RcdJumpDispHomeDodRcdUserAPI(void);

/*=================================================================================================
 ** @Name      : u8 RcdClrDateErrRcdUserAPI(u8 date)
 ** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
 ** @Output    : 清除结果 TRUE:成功 FALSE:失败
 ** @Function  : 清除指定日期故障记录存储信息
 ** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
 **===============================================================================================*/
u8 RcdClrDateErrRcdUserAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdClrDateEventRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期事件记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
u8 RcdClrDateEventRcdUserAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdClrDateDataRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期运行数据记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
u8 RcdClrDateDataRcdUserAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdClrDateWaveRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期故障录波记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
u8 RcdClrDateWaveRcdUserAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdClrAllDodRcdUserAPI(void)
** @Input     : voiid
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除所有DOD记录存储信息
** @The notes :
**===============================================================================================*/
u8 RcdClrAllDodRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdReadOneErrQue_API(t_RcdStoreInfo *info)
** @Input     : info:读取到的故障记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 从故障记录缓存区读取一条故障记录
** @The notes : 读取缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneErrQueAPI(t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdReadOneEventQue_API(t_RcdStoreInfo *info)
** @Input     : info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 从事件记录缓存区读取一条事件记录
** @The notes : 读取缓存区,只读一次
**===============================================================================================*/
u8 RcdReadOneEventQueAPI(t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdGetDodBuffInfoAPI(t_DodStoreInfo *info)
** @Input     : info:当前DOD记录缓存信息
** @Output    : 或取结果 TRUE:成功 FALSE:失败
** @Function  : 获取当前DOD记录缓存信息
** @The notes : 缓存只记录一条DOD记录信息
**===============================================================================================*/
u8 RcdGetDodBuffInfoAPI(t_DodStoreInfo *info);

/*=================================================================================================
** @Name      : u16 RcdGetErrRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 故障记录读取剩余总数目
** @Function  : 获取故障记录读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
u16 RcdGetErrRcdReadRemNumAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetEventRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 事件记录读取剩余总数目
** @Function  : 获取事件记录读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
u16 RcdGetEventRcdReadRemNumAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetWaveRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 故障录波读取剩余总数目
** @Function  : 获取故障录波读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
u16 RcdGetWaveRcdReadRemNumAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetDodRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 历史DOD记录读取剩余总数目
** @Function  : 获取历史DOD记录读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
u16 RcdGetDodRcdReadRemNumAPI(void);

/*=================================================================================================
** @Name      : u8 RcdGetDataRcdNowDaysAPI(void)
** @Input     : void
** @Output    : 数据记录当前记录天数[0,31]
** @Function  : 获取数据记录当前记录天数
** @The notes :
**===============================================================================================*/
u8 RcdGetDataRcdNowDaysAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdAllNumAPI(void)
** @Input     : void
** @Output    : 数据记录当前总记录条数
** @Function  : 获取数据记录当前总记录条数
** @The notes :
**===============================================================================================*/
u16 RcdGetDataRcdAllNumAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdDateNumAPI(u8 date)
** @Input     : date:日期[0,30]
** @Output    : 指定日期数据记录条数
** @Function  : 获取指定日期数据记录条数
** @The notes : 将日期转化为实际位置指针
**===============================================================================================*/
u16 RcdGetDataRcdDateNumAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdGetDataRcdDisplayDateAPI(void)
** @Input     : void
** @Output    : 数据记录当前显示记录日期[0,30]
** @Function  : 获取数据记录当前显示记录日期
** @The notes : 将当前位置指针转化为日期
**===============================================================================================*/
u8 RcdGetDataRcdDisplayDateAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdDisPlayRemNumAPI(void)
** @Input     : void
** @Output    : 数据记录显示当天剩余记录条数
** @Function  : 获取数据记录显示当天剩余记录条数
** @The notes :
**===============================================================================================*/
u16 RcdGetDataRcdDisPlayRemNumAPI(void);

/*=================================================================================================
** @Name      : u8 RcdGetWavRcdStorReadFlagAPI(void)
** @Input     : void
** @Output    : 故障录波存储读写状态标志 0:未读取(可存储) 1:开始读取 2:正在读取
** @Function  : 获取故障录波存储读写状态标志
** @The notes :
**===============================================================================================*/
u8 RcdGetWavRcdStorReadFlagAPI(void);

#endif
