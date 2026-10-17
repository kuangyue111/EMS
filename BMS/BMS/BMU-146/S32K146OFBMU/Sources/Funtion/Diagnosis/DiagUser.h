/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : DiagUser.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组故障诊断模块用户接口
** @Instructions : 
**===============================================================================================*/
#ifndef _DIAGUSER_H
#define _DIAGUSER_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BalanTactic.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define DIAG_SLAVE_NUM              16                      /*监测从控数目*/
#define DIAG_WAIT_CELL_T            5                       /*等待电芯信息就绪时间(1s)*/
#define DIAG_LVPWR_WAR_T            80                      /*低压电源告警温度(1℃)*/
#define DIAG_LVPWR_EXT_T            100                     /*低压电源极限温度(1℃)*/
#define DIAG_BALAN_PRO_T            BALAN_PAS_PRO_TEMP      /*单体均衡保护温度(1℃)*/
#define DIAG_BALAN_EXT_T            110                     /*单体均衡温度极限值(1℃)*/

#define ALARM_DATATYPE              16                      /*告警数据类型*/
#define ALARM_INDEX_LEN             3                       /*告警索引个数*/

#define FAULT_DATATYPE              16                      /*故障数据类型*/
#define FAULT_INDEX_LEN             3                       /*故障索引个数*/

#define ERR_RCD_ERRINFO_LEN         6                       /*一条历史故障信息长度(Word)*/
#define ERR_RCD_WARCODE_FST         0x0100                  /*故障记录告警码前缀*/
#define ERR_RCD_ERRCODE_FST         0x0200                  /*故障记录故障码前缀*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
/*故障记录类型*/
typedef enum
{
    eERR_RCDSORT_WARSET = 1,                /*告警产生*/
    eERR_RCDSORT_WARCLR = 2,                /*告警消除*/
    eERR_RCDSORT_ERRSET = 3,                /*故障产生*/
    eERR_RCDSORT_ERRCLR = 4,                /*故障消除*/

    eERR_RCDSORT_LEN                        /*故障类型*/
}eErr_RcdSort;

/*用户自定义故障记录ID*/
typedef enum
{
    eUDef_ErrRcd_OSReset  = 0xa0,           /*RTOS异常复位*/
    eUDef_ErrRcd_VSampErr = 0xa1,           /*单体电压采样值失效*/
    eUDef_ErrRcd_TSampErr = 0xa2,           /*单体温度采样值失效*/
    eUDef_ErrRcd_CellVErr = 0xa3,           /*单体采样电压值异常*/
    eUDef_ErrRcd_CellTErr = 0xa4,           /*单体采样温度值异常*/
    eUDef_ErrRcd_SlaveOff = 0xa5,           /*接收从控信息中断*/
    eUDef_ErrRcd_HallCErr = 0xa6,           /*霍尔电流采样异常*/
    eUDef_ErrRcd_ShunCErr = 0xa7,           /*分流器电流采样异常*/
    eUDef_ErrRcd_TWOCDiff = 0xa8,           /*俩通道电流采样差异过大*/
    eUDef_ErrRcd_SmcuErr  = 0xa9,           /*辅助MCU异常*/
    eUDef_ErrRcd_SmgWarn  = 0xaa,           /*烟感信号异常*/
    eUDef_ErrRcd_LuComErr = 0xab,           /*IMLU1通信异常*/
    eUDef_ErrRcd_EuComErr = 0xac,           /*IMEU2通信异常*/
    eUDef_ErrRcd_ArComErr = 0xad,           /*空调通信异常*/
    eUDef_ErrRcd_THComErr = 0xae,           /*温湿度传感器通信异常*/
    eUDef_ErrRcd_GsComErr = 0xaf,           /*气体传感器通信异常*/

    eUDef_ErrRcd_CellVExt = 0xb0,           /*单体电压超过极限值*/
    eUDef_ErrRcd_CellTExt = 0xb1,           /*单体温度超过极限值*/
    eUDef_ErrRcd_BalaTExt = 0xb2,           /*均衡板温超过极限值*/
    eUDef_ErrRcd_DCBTExt  = 0xb3,           /*低压板温超过极限值*/
    eUDef_ErrRcd_CurrExt  = 0xb4,           /*输出电流超过极限值*/
    eUDef_ErrRcd_CellVDn  = 0xb5,           /*单体电压压降过快*/
    eUDef_ErrRcd_CellTUp  = 0xb6,           /*单体温度温升过快*/
    eUDef_ErrRcd_CurrIdle = 0xb7,           /*电池组待机电流异常*/
    eUDef_ErrRcd_CellVPro = 0xb8,           /*同一单体充放电保护*/
    eUDef_ErrRcd_CellTPro = 0xb9,           /*同一单体多次温度保护*/
    eUDef_ErrRcd_CapDrop  = 0xba,           /*总容量衰减严重异常*/
    eUDef_ErrRcd_DodLack  = 0xbb,           /*充放深度长期不足*/
    eUDef_ErrRcd_ChgEffe  = 0xbc,           /*充放电效率异常*/
    eUDef_ErrRcd_SosMode  = 0xbd,           /*SOS模型推断异常*/
    eUDef_ErrRcd_ReSosErr = 0xbe,           /*实时SOS等级异常*/
    eUDef_ErrRcd_PrSosErr = 0xbf,           /*预测SOS等级异常*/

}UDef_ErrRcd_Id;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void DiagErrDisplayInit(void)
** @Input     : void
** @Output    : void
** @Function  : 故障显示信息初始化
** @The notes : 读取初始显示的历史故障,用户任务单次调用(避免占用太长系统初始化时间)
**===============================================================================================*/
void DiagErrDisplayInit(void);

/*=================================================================================================
** @Name      : void DiagErrDisplayTask(void)
** @Input     : void
** @Output    : void
** @Function  : 故障显示任务
** @The notes : 100ms周期,将滤波故障转入故障显示位
**===============================================================================================*/
void DiagErrDisplayTask(void);

/*=================================================================================================
** @Name      : void DiagUserSelfErrRecord(u8 code, eErr_RcdSort sort, u16 rcdInfo0,  u16 rcdInfo1)
** @Input     : id:用户故障码  sort:记录故障类型  1告警产生 3故障产生 rcdInfo0:需记录的信息0 rcdInfo1:需记录的信息1
** @Output    : void
** @Function  : 用户增加自定义故障记录(用户自定义仅记录故障告警产生)
** @The notes : 用户需要产生自定义故障时调用
**===============================================================================================*/
void DiagUserSelfErrRecord(u8 code, eErr_RcdSort sort, u16 rcdInfo0,  u16 rcdInfo1);

/*=================================================================================================
** @Name      : void DiagChangHisErrDisplayAPI(u8 date, u8 home)
** @Input     : date:历史故障日期[0,30]:0-30天前记录 home:读首页标志 0:读当前记录 1:读当日首页记录
** @Output    : void
** @Function  : 切换历史故障显示信息
** @The notes : 按照日期读取指定时间的历史故障显示(读30条)
**===============================================================================================*/
void DiagChangHisErrDisplayAPI(u8 date, u8 home);

/*=================================================================================================
** @Name      : void DiagClrHisErrDisplayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除历史故障显示信息
** @The notes : 显示区域清零
**===============================================================================================*/
void DiagClrHisErrDisplayAPI(void);

/*=================================================================================================
** @Name      : void DiagChangHisWaveDisplayAPI(u8 date, u8 home)
** @Input     : date:历史故障录波日期[0,30]:0-30天前记录 home:读首页标志 0:读当前记录 1:读当日首页记录
** @Output    : void
** @Function  : 切换历史故障录波显示信息
** @The notes : 按照日期读取指定时间的历史故障录波显示(读1条)
**===============================================================================================*/
void DiagChangHisWaveDisplayAPI(u8 date, u8 home);

/*=================================================================================================
** @Name      : void DiagClrHisWaveDisplayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除历史故障录波显示信息
** @The notes : 显示区域清零
**===============================================================================================*/
void DiagClrHisWaveDisplayAPI(void);

/*=================================================================================================
** @Name      : void DiagRcdHisErrInfoRecordAPI(u8 id, u8 sort)
** @Input     : id:故障码ID sort:记录故障类型  1告警产生 2告警消除 3故障产生 4故障消除
** @Output    : void
** @Function  : 用户记录历史故障事件信息
** @The notes : 需要故障产生前记录事件时调用
**===============================================================================================*/
void DiagRcdHisErrInfoRecordAPI(u8 id, u8 sort);

#endif
