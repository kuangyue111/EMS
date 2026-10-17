/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : RecordInterface.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 事件/数据记录模块接口
** @Instructions : 
**===============================================================================================*/
#ifndef _RECORDINTERFACE_H
#define _RECORDINTERFACE_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "RecordHandle.h"
#include "RecordUser.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define RCD_HISERR_EN               1                                       /*故障记录功能使能*/
#define RCD_ERRWR_EN                1                                       /*故障记录存储功能使能*/
#define RCD_ERRWS_START_ADDR        0x200000                                /*故障记录存储状态记录起始地址(2M开始)*/
#define RCD_ERRWS_END_ADDR          0x201000                                /*故障记录存储状态记录结束地址(不包含)[一个扇区4K]*/
#define RCD_ERRWD_START_ADDR        0x201000                                /*故障记录存储信息记录起始地址*/
#define RCD_ERRWD_END_ADDR          0x300000                                /*故障记录存储信息记录结束地址(不包含)[1M][400*31条]*/

#define RCD_ERRWD_SEC_LEN           4096                                    /*故障记录存储信息扇区长度(EEPROM设置为0)*/
#define RCD_ERRWD_SECNUM            50                                      /*故障记录存储信息一个扇区记录条数*/
#define RCD_ERRWD_DSEC_NUM          8                                       /*故障记录存储信息一天占用扇区个数(EEPROM设置为1)*/
#define RCD_ERRWD_DAYNUM            (RCD_ERRWD_SECNUM*RCD_ERRWD_DSEC_NUM)   /*故障记录存储数据一天记录条数*/

#define RCD_EVENT_EN                1                                       /*事件记录功能使能*/
#define RCD_EVNWR_EN                1                                       /*事件记录存储功能使能*/
#define RCD_EVNWS_START_ADDR        0x300000                                /*事件记录存储状态记录起始地址(3M开始)*/
#define RCD_EVNWS_END_ADDR          0x301000                                /*事件记录存储状态记录结束地址(不包含)[一个扇区4K]*/
#define RCD_EVNWD_START_ADDR        0x301000                                /*事件记录存储信息记录起始地址*/
#define RCD_EVNWD_END_ADDR          0x400000                                /*事件记录存储信息记录结束地址(不包含)[1M][400*31条]*/

#define RCD_EVNWD_SEC_LEN           4096                                    /*事件记录存储信息扇区长度(EEPROM设置为0)*/
#define RCD_EVNWD_SECNUM            50                                      /*事件记录存储信息一个扇区记录条数*/
#define RCD_EVNWD_DSEC_NUM          8                                       /*事件记录存储信息一天占用扇区个数(EEPROM设置为1)*/
#define RCD_EVNWD_DAYNUM            (RCD_EVNWD_SECNUM*RCD_EVNWD_DSEC_NUM)   /*事件记录存储数据一天记录条数*/

#define RCD_FATWAVE_EN              1                                       /*故障录波记录功能使能*/
#define RCD_WAVWR_EN                1                                       /*故障录波记录存储功能使能*/
#define RCD_WAVWS_START_ADDR        0x400000                                /*故障录波记录存储状态记录起始地址(4M开始)*/
#define RCD_WAVWS_END_ADDR          0x401000                                /*故障录波记录存储状态记录起始结束地址(不包含)[一个扇区4K]*/
#define RCD_WAVWD_START_ADDR        0x401000                                /*故障录波记录存储信息记录起始地址*/
#define RCD_WAVWD_END_ADDR          0x500000                                /*故障录波记录存储信息记录结束地址(不包含)[1M]*/

#define RCD_WAVWD_SEC_LEN           4096                                    /*故障录波记录存储信息扇区长度*/
#define RCD_WAVWD_SECNUM            37                                      /*故障录波记录存储信息一个扇区记录条数(4096/55/2)*/
#define RCD_WAVWD_DSEC_NUM          6                                       /*故障录波记录存储信息一天使用的扇区个数(EEPROM设置为1)*/
#define RCD_WAVWD_DAYNUM            (RCD_WAVWD_SECNUM*RCD_WAVWD_DSEC_NUM)   /*故障录波记录存储信息一天记录条数*/

#define RCD_RUNDATA_EN              1                                       /*运行数据记录功能使能*/
#define RCD_DATWS_START_ADDR        0x500000                                /*运行数据记录存储状态记录起始地址(5M开始)*/
#define RCD_DATWS_END_ADDR          0x501000                                /*运行数据记录存储状态记录起始结束地址(不包含)[一个扇区4K]*/
#define RCD_DATWD_START_ADDR        0x501000                                /*运行数据记录存储信息记录起始地址*/
#define RCD_DATWD_END_ADDR          0x700000                                /*运行数据记录存储信息记录结束地址(不包含)[2M]*/

#define RCD_DATWD_SEC_LEN           4096                                    /*运行数据记录存储信息扇区长度*/
#define RCD_DATWD_SECNUM            36                                      /*运行数据记录存储信息一个扇区记录条数(4096/56/2)*/
#define RCD_DATWD_DSEC_NUM          16                                      /*运行数据记录存储信息一天使用的扇区个数(EEPROM设置为1)*/
#define RCD_DATWD_DAYNUM            (RCD_DATWD_SECNUM*RCD_DATWD_DSEC_NUM)   /*运行数据记录存储信息一天记录条数*/
#define RCD_DATWD_CYCLE_T           (220)                                   /*运行数据记录存储数据周期(1s)*/
//#define RCD_DATWD_CYCLE_T         (2500)                                  /*运行数据记录存储数据周期(1s)*/

#define RCD_HISDOD_EN               1                                       /*历史DOD记录功能使能*/
#define RCD_DODWS_START_ADDR        0x700000                                /*历史DOD记录存储状态记录起始地址(7M开始)*/
#define RCD_DODWS_END_ADDR          0x701000                                /*历史DOD记录存储状态记录起始结束地址(不包含)[一个扇区4K]*/
#define RCD_DODWD_START_ADDR        0x701000                                /*历史DOD记录存储信息记录起始地址*/
#define RCD_DODWD_END_ADDR          0x780000                                /*历史DOD记录存储信息记录结束地址(不包含)[1M]*/

#define RCD_DODWD_SEC_LEN           4096                                    /*历史DOD记录存储信息扇区长度*/
#define RCD_DODWD_SEC_NUM           204                                     /*历史DOD记录存储信息一个扇区记录条数(4096/10/2)*/
#define RCD_DODWD_ASECNUM           50                                      /*历史DOD记录存储信息总共使用的扇区个数(EEPROM设置为1)*/
#define RCD_DODWD_ALLNUM            (RCD_DODWD_SEC_NUM*RCD_DODWD_ASECNUM)   /*历史DOD记录存储信息总共记录条数*/

#define RCD_CLR_ALLFLASH_EN         0                                       /*日期变更首先擦除一天所有FLASH扇区使能(不使能则为擦除一个扇区写一个扇区)*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/

/*故障录波类型*/
typedef enum
{
	eFatWavSort_CVH2 = 1,               /*单体过压二级故障录波*/
	eFatWavSort_CVH1 = 2,               /*单体过压一级故障录波*/
	eFatWavSort_CVL2 = 3,               /*单体欠压二级故障录波*/
	eFatWavSort_CVL1 = 4,               /*单体欠压一级故障录波*/

	eFatWavSort_HVH2 = 5,               /*总压过压二级故障录波*/
	eFatWavSort_HVH1 = 6,               /*总压过压一级故障录波*/
	eFatWavSort_HVL2 = 7,               /*总压欠压二级故障录波*/
	eFatWavSort_HVL1 = 8,               /*总压欠压一级故障录波*/

	eFatWavSort_CTH2 = 9,               /*充电过温二级故障录波*/
	eFatWavSort_CTH1 = 10,              /*充电过温一级故障录波*/
	eFatWavSort_CTL2 = 11,              /*充电低温二级故障录波*/
	eFatWavSort_CTL1 = 12,              /*充电低温一级故障录波*/

	eFatWavSort_DTH2 = 13,              /*放电过温二级故障录波*/
	eFatWavSort_DTH1 = 14,              /*放电过温一级故障录波*/
	eFatWavSort_DTL2 = 15,              /*放电低温二级故障录波*/
	eFatWavSort_DTL1 = 16,              /*放电低温一级故障录波*/

	eFatWavSort_CCO2 = 17,              /*充电过流二级故障录波*/
	eFatWavSort_CCO1 = 18,              /*充电过流一级故障录波*/
	eFatWavSort_DCO2 = 19,              /*放电过流二级故障录波*/
	eFatWavSort_DCO1 = 20,              /*放电过流一级故障录波*/

	eFatWavSort_CCOL = 21,              /*充电电流超限故障录波*/
	eFatWavSort_DCOL = 22,              /*放电电流超限故障录波*/
}e_RcdFatWavSort;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 RcdGetErrRcdEnStateAPI(void)
** @Input     : void
** @Output    : 故障记录存储功能使能状态 0:禁用(未初始化) 1:使能(初始化完成)
** @Function  : 获取故障记录存储功能使能状态
** @The notes : 配置使能时反映是否初始化
**===============================================================================================*/
extern u8 RcdGetErrRcdEnStateAPI(void);

/*=================================================================================================
** @Name      : u8 RcdGetEventRcdEnStateAPI(void)
** @Input     : void
** @Output    : 事件记录存储功能使能状态 0:禁用(未初始化) 1:使能(初始化完成)
** @Function  : 获取事件记录存储功能使能状态
** @The notes : 配置使能时反映是否初始化
**===============================================================================================*/
extern u8 RcdGetEventRcdEnStateAPI(void);

/*=================================================================================================
** @Name      : u8 RcdGetDataRcdEnStateAPI(void)
** @Input     : void
** @Output    : 运行数据记录存储功能使能状态 0:禁用(未初始化) 1:使能(初始化完成)
** @Function  : 获取运行数据记录存储功能使能状态
** @The notes : 配置使能时反映是否初始化
**===============================================================================================*/
extern u8 RcdGetDataRcdEnStateAPI(void);

/*=================================================================================================
 ** @Name      : void RcdStoreStateInit(void)
 ** @Input     : void
 ** @Output    : void
 ** @Function  : 记录存储状态信息初始化(记录读写指针和数目)
 ** @The notes : 模块参数后/用户读写记录前调用
 **===============================================================================================*/
extern void RcdStoreStateInit(void);

/*=================================================================================================
** @Name      : void RcdStoreHandleTask(void)
** @Input     : void
** @Output    : void
** @Function  : 记录存储处理任务
** @The notes : 600ms周期
**===============================================================================================*/
extern void RcdStoreHandleTask(void);

/*=================================================================================================
** @Name      : void RcdFaultWaveHandleTask(void)
** @Input     : void
** @Output    : void
** @Function  : 故障录波处理任务
** @The notes : 100ms周期
**===============================================================================================*/
extern void RcdFaultWaveHandleTask(void);

/*=================================================================================================
** @Name      : void RcdSetFaultWaveTrigAPI(e_RcdFatWavSort sort)
** @Input     : sort:录波类型[0:无效记录 1:单体二级过压,2:单体一级过压,3:单体二级欠压,4:单体一级欠压;5:总压二级过压,6:总压一级过压,7:总压二级欠压,8:总压一级欠压;9:充电二级过温;10:充电一级过温;11:充电二级低温;12:充电一级低温;13:放电二级过温;14:放电一级过温;15:放电二级低温;16:放电一级低温;17:充电二级过流;18:充电一级过流;19:放电二级过流;20:放电一级过流;21:充电超限值;22:放电超限值]
** @Output    : void
** @Function  : 设置触发开始录波
** @The notes : 产生故障需要录波时调用
**===============================================================================================*/
extern void RcdSetFaultWaveTrigAPI(e_RcdFatWavSort sort);

/*=================================================================================================
** @Name      : void RcdCreateErrRcdInfo_API(t_ RcdCreateInfo *rcdInfo)
** @Input     : rcdInfo:记录信息
** @Output    : void
** @Function  : 创建一条故障记录
** @The notes : 用户需要产生一条记录时调用(写队列缓存)
**===============================================================================================*/
extern void RcdCreateErrRcdInfoAPI(t_RcdCreateInfo *rcdInfo);

/*=================================================================================================
** @Name      : void RcdCreateErrRcdInfo_API(t_ RcdCreateInfo *rcdInfo)
** @Input     : rcdInfo:记录信息
** @Output    : void
** @Function  : 创建一条事件记录
** @The notes : 用户需要产生一条记录时调用(写队列缓存)
**===============================================================================================*/
extern void RcdCreateEventRcdInfoAPI(t_RcdCreateInfo *rcdInfo);

/*=================================================================================================
** @Name      : void RcdCreateDataRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:记录数据信息 len:记录长度
** @Output    : void
** @Function  : 创建一条运行数据记录
** @The notes : 用户需要产生一条记录时调用(直接写FLASH存储)
**===============================================================================================*/
extern void RcdCreateDataRcdInfoAPI(u16 *data, u8 len);

/*=================================================================================================
** @Name      : void RcdCreateWaveRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:记录数据信息 len:记录长度
** @Output    : void
** @Function  : 创建一条故障录波记录
** @The notes : 用户需要产生一条记录时调用(直接写FLASH存储)
**===============================================================================================*/
extern void RcdCreateWaveRcdInfoAPI(u16 *data, u8 len);

/*=================================================================================================
** @Name      : void RcdCreateDodRcdInfoAPI(u16 *data, u8 len)
** @Input     : data:需记录的DOD值/充电量/放电量/充电能/放电能 len:记录信息个数(5个有效)
** @Output    : void
** @Function  : 创建一条DOD信息记录
** @The notes : 用户需要产生一条记录时调用(记录一条在缓存区)
**===============================================================================================*/
extern void RcdCreateDodRcdInfoAPI(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdReadOneErrRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的故障记录
** @Output    : void读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条故障记录
** @The notes : 读取存储区或缓存区,只读一次
**===============================================================================================*/
extern u8 RcdReadOneErrRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdReadOneEventRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条事件记录
** @The notes : 读取存储区或缓存区,只读一次
**===============================================================================================*/
extern u8 RcdReadOneEventRcdUserAPI(u8 conFlag, t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdReadOneWaveRcdUserAPI(u8 conFlag, u16 *info, u8 len)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的故障录波记录信息 len:读信息长度
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条故障录波记录
** @The notes : 从最早记录开始读取存储区或缓存区,只读一次
**===============================================================================================*/
extern u8 RcdReadOneWaveRcdUserAPI(u8 conFlag, u16 *info, u8 len);

/*=================================================================================================
** @Name      : u8 RcdReadOneDodRcdUserAPI(u8 conFlag, t_DodStoreInfo *info)
** @Input     : conFlag:确认读取标志[0仅读取 1确认下一条] info:读取到的DOD记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 读取一条DOD记录
** @The notes : 从最早记录开始读取存储区或缓存区,只读一次
**===============================================================================================*/
extern u8 RcdReadOneDodRcdUserAPI(u8 conFlag, t_DodStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneErrRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) info:读取到的故障记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条故障记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
extern u8 RcdDisplayOneErrRcdUserAPI(u8 date, t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneEventRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条事件记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
extern u8 RcdDisplayOneEventRcdUserAPI(u8 date, t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneDataRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) data:读取到的数据记录 len:读取长度
** @Output    : void
** @Function  : 显示一条数据记录
** @The notes : 从最新记录开始读取存储区,可重复读
**===============================================================================================*/
extern u8 RcdDisplayOneDataRcdUserAPI(u8 date, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneWaveRcdUserAPI(u8 date, t_RcdStoreInfo *info)
** @Input     : date:显示日期(第几天) data:读取到的故障录波记录 len:读取长度
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条故障录波记录
** @The notes : 从最新记录开始读取存储区,可重复读
**===============================================================================================*/
extern u8 RcdDisplayOneWaveRcdUserAPI(u8 date, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdDisplayOneDodRcdUserAPI(t_DodStoreInfo *info)
** @Input     : info:读取到的DOD记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 显示一条DOD记录
** @The notes : 从最新记录开始读取存储区或缓存区,可重复读
**===============================================================================================*/
extern u8 RcdDisplayOneDodRcdUserAPI(t_DodStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeErrRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日首页显示历史故障记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
extern u8 RcdJumpDispHomeErrRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeEventRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日首页显示历史事件记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
extern u8 RcdJumpDispHomeEventRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeDataRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日第一条显示历史数据记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
extern u8 RcdJumpDispHomeDataRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeWaveRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到当日第一条显示历史故障录波记录
** @The notes : 只移动读显示指针地址到当日最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
extern u8 RcdJumpDispHomeWaveRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdJumpDispHomeDodRcdUserAPI(void)
** @Input     : void
** @Output    : 跳转结果 TRUE:成功 FALSE:失败
** @Function  : 跳转到最新第一条显示历史DOD记录
** @The notes : 只移动读显示指针地址到当前最新记录,跳转成功后可以读取;只能在当天内跳转
**===============================================================================================*/
extern u8 RcdJumpDispHomeDodRcdUserAPI(void);

/*=================================================================================================
 ** @Name      : u8 RcdClrDateErrRcdUserAPI(u8 date)
 ** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
 ** @Output    : 清除结果 TRUE:成功 FALSE:失败
 ** @Function  : 清除指定日期故障记录存储信息
 ** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
 **===============================================================================================*/
extern u8 RcdClrDateErrRcdUserAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdClrDateEventRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期事件记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
extern u8 RcdClrDateEventRcdUserAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdClrDateDataRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期运行数据记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
extern u8 RcdClrDateDataRcdUserAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdClrDateWaveRcdUserAPI(u8 date)
** @Input     : date:清除日期(第几天) [0:当天 1-30:前N天 0xff:全部日期所有记录]
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除指定日期故障录波记录存储信息
** @The notes : 日期从当日往前推第几天(仅1天,非连续几天)
**===============================================================================================*/
extern u8 RcdClrDateWaveRcdUserAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdClrAllDodRcdUserAPI(void)
** @Input     : voiid
** @Output    : 清除结果 TRUE:成功 FALSE:失败
** @Function  : 清除所有DOD记录存储信息
** @The notes :
**===============================================================================================*/
extern u8 RcdClrAllDodRcdUserAPI(void);

/*=================================================================================================
** @Name      : u8 RcdReadOneErrQue_API(t_RcdStoreInfo *info)
** @Input     : info:读取到的故障记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 从故障记录缓存区读取一条故障记录
** @The notes : 读取缓存区,只读一次
**===============================================================================================*/
extern u8 RcdReadOneErrQueAPI(t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdReadOneEventQue_API(t_RcdStoreInfo *info)
** @Input     : info:读取到的事件记录
** @Output    : 读取结果 TRUE:成功 FALSE:失败
** @Function  : 从事件记录缓存区读取一条事件记录
** @The notes : 读取缓存区,只读一次
**===============================================================================================*/
extern u8 RcdReadOneEventQueAPI(t_RcdStoreInfo *info);

/*=================================================================================================
** @Name      : u8 RcdGetDodBuffInfoAPI(t_DodStoreInfo *info)
** @Input     : info:当前DOD记录缓存信息
** @Output    : 或取结果 TRUE:成功 FALSE:失败
** @Function  : 获取当前DOD记录缓存信息
** @The notes : 缓存只记录一条DOD记录信息
**===============================================================================================*/
extern u8 RcdGetDodBuffInfoAPI(t_DodStoreInfo *info);

/*=================================================================================================
** @Name      : u16 RcdGetErrRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 故障记录读取剩余总数目
** @Function  : 获取故障记录读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
extern u16 RcdGetErrRcdReadRemNumAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetEventRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 事件记录读取剩余总数目
** @Function  : 获取事件记录读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
extern u16 RcdGetEventRcdReadRemNumAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetWaveRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 故障录波读取剩余总数目
** @Function  : 获取故障录波读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
extern u16 RcdGetWaveRcdReadRemNumAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetDodRcdReadRemNumAPI(void)
** @Input     : void
** @Output    : 历史DOD记录读取剩余总数目
** @Function  : 获取历史DOD记录读取剩余总数目
** @The notes : 当前记录总数目(已读取的已清除)
**===============================================================================================*/
extern u16 RcdGetDodRcdReadRemNumAPI(void);

/*=================================================================================================
** @Name      : u8 RcdGetDataRcdNowDaysAPI(void)
** @Input     : void
** @Output    : 数据记录当前记录天数[0,31]
** @Function  : 获取数据记录当前记录天数
** @The notes :
**===============================================================================================*/
extern u8 RcdGetDataRcdNowDaysAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdAllNumAPI(void)
** @Input     : void
** @Output    : 数据记录当前总记录条数
** @Function  : 获取数据记录当前总记录条数
** @The notes :
**===============================================================================================*/
extern u16 RcdGetDataRcdAllNumAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdDateNumAPI(u8 date)
** @Input     : date:日期[0,30]
** @Output    : 指定日期数据记录条数
** @Function  : 获取指定日期数据记录条数
** @The notes : 将日期转化为实际位置指针
**===============================================================================================*/
extern u16 RcdGetDataRcdDateNumAPI(u8 date);

/*=================================================================================================
** @Name      : u8 RcdGetDataRcdDisplayDateAPI(void)
** @Input     : void
** @Output    : 数据记录当前显示记录日期[0,30]
** @Function  : 获取数据记录当前显示记录日期
** @The notes : 将当前位置指针转化为日期
**===============================================================================================*/
extern u8 RcdGetDataRcdDisplayDateAPI(void);

/*=================================================================================================
** @Name      : u16 RcdGetDataRcdDisPlayRemNumAPI(void)
** @Input     : void
** @Output    : 数据记录显示当天剩余记录条数
** @Function  : 获取数据记录显示当天剩余记录条数
** @The notes :
**===============================================================================================*/
extern u16 RcdGetDataRcdDisPlayRemNumAPI(void);

/*=================================================================================================
** @Name      : u8 RcdGetFatWavStoreStopReqHook(void)
** @Input     : void
** @Output    : 故障录波停止数据记录请求状态 0:无请求 1:请求
** @Function  : 获取故障录波停止数据记录请求状态
** @The notes : 用户请求需要暂停缓存当前录波数据
**===============================================================================================*/
u8 RcdGetFatWavStoreStopReqHook(void);

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
** @Input     : *len:指定单体电压过压故障录波数据缓存区长度
** @Output    : 单体电压过压故障录波数据缓存区起始地址指针
** @Function  : 获取单体电压过压故障录波数据缓存区起始地址指针
** @The notes : 用户需要单体过压故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len);

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
** @Input     : *len:指定单体电压欠压故障录波数据缓存区长度
** @Output    : 单体电压故障录波数据缓存区起始地址指针
** @Function  : 获取单体电压欠压故障录波数据缓存区起始地址指针
** @The notes : 用户需要单体欠压故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCVLBuffPtrHook(u8 *len);

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
** @Input     : *len:指定单体温度过温故障录波数据缓存区长度
** @Output    : 单体电压过温故障录波数据缓存区起始地址指针
** @Function  : 获取单体电压过温故障录波数据缓存区起始地址指针
** @The notes : 用户需要单体过温故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCTHBuffPtrHook(u8 *len);

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
** @Input     : *len:指定单体温度低温故障录波数据缓存区长度
** @Output    : 单体电压低温故障录波数据缓存区起始地址指针
** @Function  : 获取单体电压低温故障录波数据缓存区起始地址指针
** @The notes : 用户需要单体低温故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCTLBuffPtrHook(u8 *len);

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreHVBuffPtrHook(u8 *len)
** @Input     : *len:指定总压过欠压故障录波数据缓存区长度
** @Output    : 总压过欠压故障录波数据缓存区起始地址指针
** @Function  : 获取总压过欠压故障录波数据缓存区起始地址指针
** @The notes : 用户需要总压过欠压故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreHVBuffPtrHook(u8 *len);

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCOBuffPtrHook(u8 *len)
** @Input     : *len:指定充放电过流故障录波数据缓存区长度
** @Output    : 充放电过流故障录波数据缓存区起始地址指针
** @Function  : 获取充放电过流故障录波数据缓存区起始地址指针
** @The notes : 用户需要充放电过流故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCOBuffPtrHook(u8 *len);

/*=================================================================================================
** @Name      : u8 RcdGetStoreStopRcdStateHook(void)
** @Input     : void
** @Output    : 用户停止记录历史记录状态 0:不停止 1:停止
** @Function  : 获取用户停止记录历史记录状态
** @The notes : 用户请求需要暂停记录；历史记录
**===============================================================================================*/
u8 RcdGetStoreStopRcdStateHook(void);

/*=================================================================================================
** @Name      : u16 RcdGetGroupMaxVoltHook(void)
** @Input     : void
** @Output    : 电池组单体最大电压 1mV
** @Function  : 获取电池组单体最大电压
** @The notes :
**===============================================================================================*/
u16 RcdGetGroupMaxVoltHook(void);

/*=================================================================================================
** @Name      : u16 RcdGetGroupMinVoltHook(void)
** @Input     : void
** @Output    : 电池组单体最小电压 1mV
** @Function  : 获取电池组单体最小电压
** @The notes :
**===============================================================================================*/
u16 RcdGetGroupMinVoltHook(void);

/*=================================================================================================
** @Name      : s16 RcdGetGroupMinTempHook(void)
** @Input     : void
** @Output    : 电池组单体最大温度 1℃
** @Function  : 获取电池组单体最大温度
** @The notes :
**===============================================================================================*/
s16 RcdGetGroupMaxTempHook(void);

/*=================================================================================================
** @Name      : s16 RcdGetGroupMinTempHook(void)
** @Input     : void
** @Output    : 电池组单体最小温度 1℃
** @Function  : 获取电池组单体最小温度
** @The notes :
**===============================================================================================*/
s16 RcdGetGroupMinTempHook(void);

/*=================================================================================================
** @Name      : u16 RcdGetGroupSumVoltHook(void)
** @Input     : void
** @Output    : 电池组总电压 0.1V
** @Function  : 获取电池组总电压
** @The notes : 采样总压
**===============================================================================================*/
u16 RcdGetGroupSumVoltHook(void);

/*=================================================================================================
** @Name      : u16 RcdGetGroupOutCurrHook(void)
** @Input     : void
** @Output    : 电池组总电流 0.1A
** @Function  : 获取电池组总电流
** @The notes :
**===============================================================================================*/
s16 RcdGetGroupOutCurrHook(void);

/*=================================================================================================
** @Name      : u16 RcdGetGroupWorkStateHook(void)
** @Input     : void
** @Output    : 电池组工作状态[0,6]
** @Function  : 获取电池组当前工作状态
** @The notes : e_WorkState
**===============================================================================================*/
u16 RcdGetGroupWorkStateHook(void);

/*=================================================================================================
** @Name      : u16 RcdGetGroupProStateHook(void)
** @Input     : void
** @Output    : 电池组保护状态[0,5]
** @Function  : 获取电池组当前保护状态
** @The notes : e_ProState
**===============================================================================================*/
u16 RcdGetGroupProStateHook(void);

/*=================================================================================================
** @Name      : u8 RcdGetGroupDevAddrHook(void)
** @Input     : void
** @Output    : 电池组设备地址
** @Function  : 获取电池组设备地址
** @The notes :
**===============================================================================================*/
u8 RcdGetGroupDevAddrHook(void);

/*=================================================================================================
** @Name      : u8 RcdGetGRunDataRcdReqHook(void)
** @Input     : void
** @Output    : 电池组运行数据存储请求状态
** @Function  : 获取电池组运行数据存储请求状态
** @The notes : 定周期请求或状态变化时请求
**===============================================================================================*/
u8 RcdGetGRunDataRcdReqHook(void);

/*=================================================================================================
** @Name      : u8 RcdGetGroupRunDataHook(u16 *data, u8 len)
** @Input     : data:运行数据 len:数据长度
** @Output    : 成功或失败
** @Function  : 获取运行数据(数组)
** @The notes : 需要存储的数据
**===============================================================================================*/
u8 RcdGetGroupRunDataHook(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdReadRcdStoreInfoHook(u8 sort, u32 addr, u16 *data, u8 len)
** @Input     : sort:存储类别(0故障记录 1事件记录 2数据记录 3:故障录波 4:DOD记录)addr:起始地址 data:存储数据 len:数据长度
** @Output    : 读成功或失败
** @Function  : 读取存储记录信息
** @The notes :
**===============================================================================================*/
u8 RcdReadRcdStoreInfoHook(u8 sort, u32 addr, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdWriteRcdStoreInfoHook(u8 sort, u32 addr, u16 *data, u8 len)
** @Input     : sort:存储类别(0故障记录 1事件记录 2数据记录 3:故障录波 4:DOD记录)addr:起始地址 data:存储数据 len:数据长度
** @Output    : 写成功或失败
** @Function  : 记录信息写存储
** @The notes :
**===============================================================================================*/
u8 RcdWriteRcdStoreInfoHook(u8 sort, u32 addr, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 RcdEraseRcdStoreSectorHook(u8 sort, u32 addr)
** @Input     : sort:存储类别(0故障记录 1事件记录 2数据记录 3:故障录波 4:DOD记录)addr:起始地址
** @Output    : 擦除成功或失败
** @Function  : 擦除记录扇区
** @The notes : 擦除FLASH扇区
**===============================================================================================*/
u8 RcdEraseRcdStoreSectorHook(u8 sort, u32 addr);

#endif
