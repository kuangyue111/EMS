/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaInterface.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块用户处理接口
** @Instructions : 
**===============================================================================================*/
#ifndef _PARAINTERFACE_H
#define _PARAINTERFACE_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "ParaRead.h"
#include "ParaIntFlash.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
/*********************************配置参数读写配置标志*********************************************/
#define PARA_EEPADDR_TAB_LEN            (19)                /*参数配置查表的长度*/

#define PARA_CFG_FLAG_U8                (0x5A)              /*参数配置U8配置标志*/
#define PARA_CFG_FLAG_U16               (0x5A5A)            /*参数配置U16配置标志*/
#define PARA_CFG_FLAG_U32               (0x5A5A5A5A)        /*参数配置U32配置标志*/

/*********************************配置参数EEPRM地址配置标志****************************************/
/*=================================================================================================
** @保留EEP地址 [0,50)
**===============================================================================================*/

/*=================================================================================================
** @BOOT参数EEP地址分配 [50,100)
**===============================================================================================*/

/*=================================================================================================
** @系统硬件参数EEP地址分配 [100,300)
**===============================================================================================*/
#define PARA_DEV_SERIAL_ADDR            (100)               /*设备序列号配置起始地址(1*24=24)*/
#define PARA_DEV_SERIAL_LEN             (24)                /*设备序列号地址长度*/

#define PARA_SYSHW_PARA_ADDR            (200)               /*系统硬件参数配置起始地址(2*15=30)*/
#define PARA_SYSHW_PARA_LEN             (15)                /*系统硬件参数配置地址长度*/

/*=================================================================================================
** @采样校准KB参数EEP地址分配 [300,500)
**===============================================================================================*/
#define PARA_ZERO_C_CORR_ADDR           (300)               /*电流零飘修正值起始地址(2*2=4)*/
#define PARA_ZERO_C_CORR_LEN            (2)                 /*电流零飘修正值地址长度*/

#define PARA_HCHGC_CORR_KB_ADDR         (304)               /*霍尔充电电流标定KB值起始地址(4*3=12)*/
#define PARA_HCHGC_CORR_KB_LEN          (3)                 /*霍尔充电电流标定KB值地址长度*/

#define PARA_HDHGC_CORR_KB_ADDR         (316)               /*霍尔放电电流标定KB值起始地址(4*3=12)*/
#define PARA_HDHGC_CORR_KB_LEN          (3)                 /*霍尔放电电流标定KB值地址长度*/

#define PARA_SCHGC_CORR_KB_ADDR         (328)               /*分流器充电电流标定KB值起始地址(4*3=12)*/
#define PARA_SCHGC_CORR_KB_LEN          (3)                 /*分流器充电电流标定KB值地址长度*/

#define PARA_SDHGC_CORR_KB_ADDR         (340)               /*分流器放电电流标定KB值起始地址(4*3=12)*/
#define PARA_SDHGC_CORR_KB_LEN          (3)                 /*分流器放电电流标定KB值地址长度*/

#define PARA_SUMV_CORR_KB_ADDR          (352)               /*总压校准KB值记录参数起始地址(4*3=12)*/
#define PARA_SUMV_CORR_KB_LEN           (3)                 /*总压校准KB值记录参数地址长度*/

#define PARA_PREV_CORR_KB_ADDR          (364)               /*预充电压校准KB值记录参数起始地址(4*3=12)*/
#define PARA_PREV_CORR_KB_LEN           (3)                 /*预充电压校准KB值记录参数地址长度*/

#define PARA_LOAV_CORR_KB_ADDR          (376)               /*负载电压校准KB值记录参数起始地址(4*3=12)*/
#define PARA_LOAV_CORR_KB_LEN           (3)                 /*负载电压校准KB值记录参数地址长度*/

#define PARA_POSV_CORR_KB_ADDR          (388)               /*Link+电压校准KB值记录参数起始地址(4*3=12)*/
#define PARA_POSV_CORR_KB_LEN           (3)                 /*Link+电压校准KB值记录参数地址长度*/

#define PARA_NEGV_CORR_KB_ADDR          (400)               /*Link-电压校准KB值记录参数起始地址(4*3=12)*/
#define PARA_NEGV_CORR_KB_LEN           (3)                 /*Link-电压校准KB值记录参数地址长度*/

#define PARA_SCHHC_CORR_KB_ADDR         (412)               /*分流器充电大电流标定KB值起始地址(4*3=12)*/
#define PARA_SCHHC_CORR_KB_LEN          (3)                 /*分流器充电大电流标定KB值地址长度*/

#define PARA_SDHHC_CORR_KB_ADDR         (424)               /*分流器放电大电流标定KB值起始地址(4*3=12)*/
#define PARA_SDHHC_CORR_KB_LEN          (3)                 /*分流器放电大电流标定KB值地址长度*/

/*=================================================================================================
** @系统时钟和内部值EEP地址分配 [500,650)
**===============================================================================================*/
#define PARA_SYS_CLOCK_ADDR             (500)               /*系统时钟记录起始地址(2*5=10)*/
#define PARA_SYS_CLOCK_LEN              (5)                 /*系统时钟记录地址长度*/

#define PARA_MONERR_TIME_ADDR           (510)               /*当月历史故障记录时间起始地址(2*2=4)*/
#define PARA_MONERR_TIME_LEN            (2)                 /*当月历史故障记录时间地址长度*/

#define PARA_NOW_CAP_SOH_ADDR           (514)               /*当前SOH信息起始地址(2*2=4)*/
#define PARA_NOW_CAP_SOH_LEN            (2)                 /*当前SOH信息地址长度*/

#define PARA_CHGTIMES_STA_ADDR          (540)               /*充放电次数计算状态值起始地址(4*2=8)*/
#define PARA_CHGTIMES_STA_LEN           (2)                 /*充放电次数计算状态值地址长度*/

#define PARA_CAPENER_CORR_ADDR          (548)               /*容量能量自动标定检测参数起始地址(4*19=76)*/
#define PARA_CAPENER_CORR_LEN           (19)                /*容量能量自动标定检测参数参数地址长度*/

/*=================================================================================================
** @容量能量计算值EEP地址分配 [650,700)
**===============================================================================================*/
#define PARA_CAP_FORM_ADDR              (650)               /*SOC容量结构起始地址(4*5=20)*/
#define PARA_CAP_FORM_LEN               (5)                 /*SOC容量结构地址长度*/

#define PARA_ENER_FORM_ADDR             (670)               /*SOE能量结构起始地址(4*5=20)*/
#define PARA_ENER_FORM_LEN              (5)                 /*SOE能量结构地址充电*/

/*=================================================================================================
** @统计值EEP地址分配 [700,800)
**===============================================================================================*/
#define PARA_SYS_RUNTIME_ADDR           (700)               /*系统运行时间起始地址(2*2=4)*/
#define PARA_SYS_RUNTIME_LEN            (2)                 /*系统运行时间地址长度*/

#define PARA_CHG_TIMES_ADDR             (704)               /*充放电次数起始地址(2*2=4)*/
#define PARA_CHG_TIMES_LEN              (2)                 /*充放电次数地址长度*/

#define PARA_HIS_ERRNUM_ADDR            (708)               /*历史故障告警数目起始地址(2*5=10)*/
#define PARA_HIS_ERRNUM_LEN             (5)                 /*历史故障告警数目地址长度*/

#define PARA_DOD_INFO_ADDR              (718)               /*放电深度DOD统计值起始地址(2*5=10)*/
#define PARA_DOD_INFO_LEN               (5)                 /*放电深度DOD统计值地址长度*/

#define PARA_SOS_INFO_ADDR              (728)               /*安全等级SOS统计值起始地址(2*5=10)*/
#define PARA_SOS_INFO_LEN               (5)                 /*安全等级SOS统计值地址长度*/

#define PARA_HIS_CHGCAP_ADDR            (750)               /*历史充放电量起始地址(4*3=12)*/
#define PARA_HIS_CHGCAP_LEN             (3)                 /*历史充放电量地址充电*/

#define PARA_HIS_CHGENER_ADDR           (762)               /*历史充放电能起始地址(4*3=12)*/
#define PARA_HIS_CHGENER_LEN            (3)                 /*历史充放电能地址长度*/

/*=================================================================================================
** @系统通用参数EEP地址分配 [800,1000)
**===============================================================================================*/
#define PARA_SYSWORK_MOD_ADDR           (800)               /*系统工作模式设置起始地址(2*2=4)*/
#define PARA_SYSWORK_MOD_LEN            (2)                 /*系统工作模式设置地址长度*/

#define PARA_SYSGEN_PARA_ADDR           (850)               /*系统通用参数配置起始地址(2*9=18)*/
#define PARA_SYSGEN_PARA_LEN            (9)                 /*系统通用参数配置地址长度*/

/*=================================================================================================
** @设备参数配置EEP地址分配 [1000,2000)
**===============================================================================================*/
#define PARA_DEVGEN_PARA_ADDR           (1000)              /*主控设备通用参数配置起始地址(2*82=164)*/
#define PARA_DEVGEN_PARA_LEN            (82)                /*主控设备通用参数配置地址长度*/

#define PARA_DEVHIG_PARA_ADDR           (1200)              /*主控设备高级参数配置起始地址(2*71=142)*/
#define PARA_DEVHIG_PARA_LEN            (71)                /*主控设备高级参数配置地址长度*/

#define PARA_DEVHW_PARA_ADDR            (1400)              /*主控设备硬件参数配置起始地址(1*24=24)*/
#define PARA_DEVHW_PARA_LEN             (53)                /*主控设备硬件参数配置地址长度*/

/*=================================================================================================
** @参数表配置EEP地址分配 [2000,4000)
**===============================================================================================*/

/*=================================================================================================
** @临时参数配置EEP地址分配 [10,100)
**===============================================================================================*/
#define PARA_SMUWORK_MOD_ADDR           (80)                /*堆控工作模式设置起始地址(2*2=4)*/
#define PARA_SMUWORK_MOD_LEN            (2)                 /*堆控工作模式设置地址长度*/

/*=================================================================================================
** @参数备份地址划分
**===============================================================================================*/
#define PARA_SEC_PARA_LEN               (496)        	                /*参数备份1个扇区数据长度(byte数)[FLASH要求地址和长度必须是8的倍数]*/

#define EEP_SEC0_START_ADDR             (0)        	                    /*参数备份第0块EEPROM扇区起始地址(定值参数)*/
#define EEP_SEC0_CRC_ADDR               (496)                           /*参数备份第0块EEPROM扇区校验码地址(定值参数)*/
#define EEP_SEC1_START_ADDR             (500)                           /*参数备份第1块EEPROM扇区起始地址(变值参数)*/
#define EEP_SEC1_CRC_ADDR               (996)                           /*参数备份第1块EEPROM扇区校验码地址(变值参数)*/
#define EEP_SEC2_START_ADDR             (1000)                          /*参数备份第2块EEPROM扇区起始地址(配置参数)*/
#define EEP_SEC2_CRC_ADDR               (1496)                          /*参数备份第2块EEPROM扇区校验码地址(配置参数)*/

#define FLA_COPY_START_ADDR             (0x00F8000UL)                   /*参数备份写FLASH扇区起始地址(应用程序参数起始FLASH地址)*/

#define FLA_SEC0_START_ADDR             (FLA_COPY_START_ADDR)           /*参数备份第0块FLASH扇区起始地址(定值参数)*/
#define FLA_SEC0_CRC_ADDR               (FLA_COPY_START_ADDR+496)       /*参数备份第0块FLASH扇区校验码地址(定值参数)*/
#define FLA_SEC1_START_ADDR             (FLA_COPY_START_ADDR+4096)      /*参数备份第1块FLASH扇区起始地址(变值参数)*/
#define FLA_SEC1_CRC_ADDR               (FLA_COPY_START_ADDR+4592)      /*参数备份第1块FLASH扇区校验码地址(变值参数)*/
#define FLA_SEC2_START_ADDR             (FLA_COPY_START_ADDR+8192)      /*参数备份第2块FLASH扇区起始地址(配置参数)*/
#define FLA_SEC2_CRC_ADDR               (FLA_COPY_START_ADDR+8688)      /*参数备份第2块FLASH扇区校验码地址(配置参数)*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ParaReadStoreDataByte(u16 addr, u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取指定长度字节数
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreDataByte(u16 addr, u8 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreDevSerial(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电流零飘修正值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreDevSerial(u8 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreCorrZeroCurr(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电流零飘修正值初始化(有符号,单位10mA)
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreCorrZeroCurr(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取霍尔充电电流校准KB值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreHalChgCurrCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreHalDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取霍尔放电电流校准KB值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreHalDhgCurrCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreShuChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取分流器充电电流校准KB值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreShuChgCurrCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreShuDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取分流器放电电流校准KB值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreShuDhgCurrCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreShuChHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取分流器充电大电流校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreShuChHCurrCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreShuDhHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取分流器放电大电流校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreShuDhHCurrCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreSumVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电池总电压校准KB值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreSumVoltCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStorePreVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取预充电压校准KB值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStorePreVoltCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreLoaVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取预负载压校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreLoaVoltCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStorePosVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取Link+电压校准KB值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStorePosVoltCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreNegVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取Link-电压校准KB值初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreNegVoltCorrKB(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreSysClock(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统时钟(北京时间)初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreSysClock(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreMonErrTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取月故障统计时间(年月)初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreMonErrTime(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreNowCapSoh(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取当前容量SOH值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreNowCapSoh(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreChgTimesState(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取充放电次数检测状态初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreChgTimesState(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreCapEnerCorrInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取总容量总能量自动标定信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreCapEnerCorrInfo(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreCapFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电量计算结构信息初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreCapFormInfo(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreCapFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电能计算结构信息初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreEnerFormInfo(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreSysRunTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统运行(上电)时长初始化 1Hour
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreSysRunTime(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreChgDhgTimes(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function(u: 从存储区读取充放电次数初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreChgDhgTimes(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreHisErrNum(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取历史故障告警数目初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreHisErrNum(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreDODStaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取放电深度统计信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreDODStaInfo(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreSOSLockInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取安全等级SOS锁定信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreSOSLockInfo(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreHisChgDhgCap(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取累计充放电电量初始化 1AH
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreHisChgDhgCap(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreHisChgDhgEner(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取累计充放电电能初始化 0.1kWh
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreHisChgDhgEner(u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreGroupGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电池组主控通用参数信息初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreGroupGenParaInfo(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreGroupHigParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电池组主控高级参数信息初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreGroupHigParaInfo(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreGroupHWParaInfo(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电池组主控硬件参数信息初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreGroupHWParaInfo(u8 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreSystemWorkMode(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统工作模式
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreSystemWorkMode(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreSystemGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统通用参数信息初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreSystemGenParaInfo(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaReadStoreSystemHWParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统硬件参数信息初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
extern u8 ParaReadStoreSystemHWParaInfo(u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreDevSerial(u8 index, u8 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数) len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入设备序列号
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreDevSerial(u8 index, u8 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreCorrZeroCurr(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电流零飘修正值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreCorrZeroCurr(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHalChgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入霍尔充电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreHalChgCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHalDhgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入霍尔放电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreHalDhgCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreShuChgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入分流器充电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreShuChgCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreShuDhgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入分流器放电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreShuDhgCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreShuChHCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入分流器大充电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreShuChHCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreShuDhHCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入分流器大放电电流校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreShuDhHCurrCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSumVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电池总电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreSumVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStorePreVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入预充电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStorePreVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreLoaVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入负载电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreLoaVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStorePosVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入Link+电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStorePosVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreNegVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入Link-电压校准KB值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreNegVoltCorrKB(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSysClock(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统时钟(北京时间)
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreSysClock(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreMonErrTime(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入月故障统计时间(年月)
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreMonErrTime(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreNowCapSoh(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入当前总容量SOH值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreNowCapSoh(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreChgTimesState(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入充放电次数检测状态
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreChgTimesState(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreCapEnerCorrInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入总容量总能量自动标定信息值
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreCapEnerCorrInfo(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreCapFormInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电量计算结构信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreCapFormInfo(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreEnerFormInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电能计算结构信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreEnerFormInfo(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSysRunTime(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统运行(上电)时长 1Hour
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreSysRunTime(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreChgDhgTimes(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入充放电次数
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreChgDhgTimes(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHisErrNum(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入历史故障告警数目
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreHisErrNum(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreDODStaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入放电深度统计信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreDODStaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSOSLockInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入安全状态SOS锁定信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreSOSLockInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHisChgDhgCap(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入累计充放电电量 1AH
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreHisChgDhgCap(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreHisChgDhgEner(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入累计充放电电能 0.1kWh
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreHisChgDhgEner(u8 index, u32 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreGroupGenParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电池组主控通用参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreGroupGenParaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreGroupHigParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电池组主控高级参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreGroupHigParaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreGroupHWParaInfo(u8 index, u8 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入电池组主控硬件参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreGroupHWParaInfo(u8 index, u8 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSystemWorkMode(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统系统工作模式信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreSystemWorkMode(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSystemGenParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统设备通用参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreSystemGenParaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSystemHWParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据  len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统设备硬件参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
extern u8 ParaWriteStoreSystemHWParaInfo(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Name      : void ParaDownloadInit(void
** @Input     : void
** @Output    : void
** @Function  : 参数批量下载功能初始化
** @The notes :
**===============================================================================================*/
extern void ParaDownloadInit(void);

/*=================================================================================================
** @Name      : void ParaChangDownloadState(void)
** @Input     : void
** @Output    : void
** @Function  : 切换参数批量下载状态
** @The notes : 接收到配置状态切换命令后调用
**===============================================================================================*/
extern void ParaChangDownloadState(void);

/*=================================================================================================
** @Name      : void ParaDownloadWriteParaHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 参数批量下载读写参数处理
** @The notes : 接收完读写命令和写入参数信息后调用
**===============================================================================================*/
extern void ParaDownloadWriteParaHandle(void);

/*=================================================================================================
** @Name      : void ParaStoreCopyTask(void)
** @Input     : void
** @Output    : void
** @Function  : 参数存储备份任务
** @The notes : 100ms周期调用
**===============================================================================================*/
extern void ParaStoreCopyTask(void);

/*=================================================================================================
** @Name      : void ParaStoreParaInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组存储的参数初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
void ParaStoreParaInfoInit(void);

/*=================================================================================================
** @Name      : u8 ParaCheckExtEepReadyHook(void)
** @Input     : void
** @Output    : TRUE:就绪 FALSE:异常
** @Function  : 检测外部EEPROM正常就绪状态
** @The notes : 用户必须补充完善
**===============================================================================================*/
u8 ParaCheckExtEepReadyHook(void);

/*=================================================================================================
** @Name      : u8 ParaGetStateChangReqCopyHook(void)
** @Input     : void
** @Output    : TRUE:请求备份 FALSE:无请求
** @Function  : 获取状态变换请求参数备份状态
** @The notes : 用户必须补充完善
**===============================================================================================*/
u8 ParaGetStateChangReqCopyHook(void);

/*=================================================================================================
** @Name      : u8 *ParaGetDnloadStateInfoPtrHook(void)
** @Input     : void
** @Output    : 变量地址指针
** @Function  : 获取参数批量下载状态信息地址指针
** @The notes : 支持参数批量下载时用户必须补充完善
**===============================================================================================*/
u8 *ParaGetDnloadStateInfoPtrHook(void);

/*=================================================================================================
** @Name      : u8 *ParaGetDnloadReadInfoPtrHook(void)
** @Input     : void
** @Output    : 变量地址指针
** @Function  : 获取参数配置读取信息地址指针
** @The notes : 支持参数批量下载时用户必须补充完善
**===============================================================================================*/
u8 *ParaGetDnloadReadInfoPtrHook(void);

/*=================================================================================================
** @Name      : u8 *ParaGetDnloadWriteInfoPtrHook(void)
** @Input     : void
** @Output    : 变量地址指针
** @Function  : 获取参数配置写入信息地址指针
** @The notes : 支持参数批量下载时用户必须补充完善
**===============================================================================================*/
u8 *ParaGetDnloadWriteInfoPtrHook(void);

/*=================================================================================================
** @Name      : u8 ParaWriteParaUserHandleHook(u16 addr, u8 *para, u8 len)
** @Input     : addr:参数EEPROM地址 para:参数值 len:长度
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 参数写入存储区用户处理
** @The notes : 支持参数批量下载时用户必须补充完善
**===============================================================================================*/
u8 ParaWriteParaUserHandleHook(u16 addr, u8 *para, u8 len);

/*=================================================================================================
** @Name      : u8 ParaRefactoryUserHandleHook(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 参数存储区恢复出厂设置值用户处理
** @The notes : 擦除 各段参数配置标志
**===============================================================================================*/
u8 ParaRefactoryUserHandleHook(void);

#endif
