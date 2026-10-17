/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaWrite.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块写入参数处理
** @Instructions : 
**===============================================================================================*/
#ifndef _PARAWRITE_H
#define _PARAWRITE_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ParaWriteStoreDevSerial(u8 index, u8 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数) len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入设备序列号
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreDevSerial(u8 index, u8 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreCorrZeroCurr(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电流零飘修正值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreCorrZeroCurr(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHalChgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入霍尔充电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreHalChgCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHalDhgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入霍尔放电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreHalDhgCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreShuChgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入分流器充电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreShuChgCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreShuDhgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入分流器放电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreShuDhgCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreShuChHCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入分流器大充电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreShuChHCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreShuDhHCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入分流器大放电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreShuDhHCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSumVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电池总电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSumVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStorePreVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入预充电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStorePreVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreLoaVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入负载电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreLoaVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreLoaVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入负载电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreLoaVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStorePosVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入Link+电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStorePosVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreNegVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入Link-电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreNegVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSysClock(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统时钟(北京时间)
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSysClock(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreMonErrTime(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入月故障统计时间(年月)
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreMonErrTime(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreNowCapSoh(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入当前总容量SOH值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreNowCapSoh(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreChgTimesState(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入充放电次数检测状态
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreChgTimesState(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreCapEnerCorrInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入总容量总能量自动标定信息值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreCapEnerCorrInfo(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreCapFormInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电量计算结构信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreCapFormInfo(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreEnerFormInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电能计算结构信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreEnerFormInfo(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSysRunTime(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统运行(上电)时长 1Hour
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSysRunTime(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreChgDhgTimes(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入充放电次数
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreChgDhgTimes(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHisErrNum(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入历史故障告警数目
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreHisErrNum(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreDODStaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入放电深度统计信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreDODStaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSOSLockInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入安全状态SOS锁定信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSOSLockInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHisChgDhgCap(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入累计充放电电量 1AH
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreHisChgDhgCap(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHisChgDhgEner(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入累计充放电电能 0.1kWh
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreHisChgDhgEner(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreGroupGenParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电池组主控通用参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreGroupGenParaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreGroupHigParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电池组主控高级参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreGroupHigParaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreGroupHWParaInfo(u8 index, u8 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电池组主控硬件参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreGroupHWParaInfo(u8 index, u8 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSystemWorkMode(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统系统工作模式信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSystemWorkMode(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSystemGenParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统设备通用参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSystemGenParaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSystemHWParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据  len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统设备硬件参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSystemHWParaInfo(u8 index, u16 *data, u8 len);

#endif
