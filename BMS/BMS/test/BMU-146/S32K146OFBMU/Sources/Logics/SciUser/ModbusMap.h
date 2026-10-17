/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ModbusMap.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组主控Modbus通信地址映射模块
** @Instructions : 
**===============================================================================================*/
#ifndef _MODBUSMAP_H
#define _MODBUSMAP_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
/*******总体地址划分**************************************************************************/
#define MB_LOAD_BASE_ADDR           (0)             //在线升级烧录寄存器起始地址(包括)
#define MB_LOAD_BASE_END            (12)            //在线升级烧录寄存器结束地址(不包括)
#define MB_DEVR_BASE_ADDR           (90)            //读设备参数信息寄存器起始地址(包括)
#define MB_DEVR_BASE_END            (98)            //读设备参数信息寄存器结束地址(不包括)
#define MB_PROR_BASE_ADDR           (100)           //读项目参数信息寄存器起始地址(包括)
#define MB_PROR_BASE_END            (115)           //读项目参数信息寄存器结束地址(不包括)
#define MB_SOFR_BASE_ADDR           (120)           //读故障记录信息寄存器起始地址(包括)
#define MB_SOFR_BASE_END            (134)           //读故障记录信息寄存器结束地址(不包括)
#define MB_SOER_BASE_ADDR           (134)           //读事件记录信息寄存器起始地址(包括)
#define MB_SOER_BASE_END            (148)           //读事件记录信息寄存器结束地址(不包括)
#define MB_DTCR_BASE_ADDR           (148)           //读故障录波记录信息寄存器起始地址(包括)
#define MB_DTCR_BASE_END            (349)           //读故障录波记录信息寄存器结束地址(不包括)
#define MB_RCDR_BASE_ADDR           (350)           //读历史数据记录信息寄存器起始地址(包括)
#define MB_RCDR_BASE_END            (446)           //读历史数据记录信息寄存器结束地址(不包括)
#define MB_CFMW_BASE_ADDR           (446)           //写历史记录读确认寄存器起始地址(包括)
#define MB_CFMW_BASE_END            (450)           //写历史记录读确认寄存器结束地址(不包括)

#define MB_TIMR_BASE_ADDR           (450)           //读校时信息寄存器起始地址(包括)
#define MB_TIMR_BASE_END            (454)           //读校时信息寄存器结束地址(不包括)
#define MB_TIMW_BASE_ADDR           (455)           //写校时信息寄存器起始地址(包括)
#define MB_TIMW_BASE_END            (460)           //写校时信息寄存器结束地址(不包括)

#define MB_SERR_BASE_ADDR           (460)           //读设备序列号信息寄存器起始地址(包括)
#define MB_SERR_BASE_END            (475)           //读设备序列号信息寄存器结束地址(不包括)
#define MB_SERW_BASE_ADDR           (475)           //写设备序列号信息寄存器起始地址(包括)
#define MB_SERW_BASE_END            (490)           //写设备序列号信息寄存器结束地址(不包括)

#define MB_GIDR_BASE_ADDR           (490)           //读当前组号信息寄存器起始地址(包括)
#define MB_GIDR_BASE_END            (495)           //读当前组号信息寄存器结束地址(不包括)
#define MB_GIDW_BASE_ADDR           (495)           //写当前组号信息寄存器起始地址(包括)
#define MB_GIDW_BASE_END            (499)           //写当前组号信息寄存器结束地址(不包括)

#define MB_PWDW_BASE_ADDR           (500)           //写权限密码命令信息寄存器起始地址(包括)
#define MB_PWDW_BASE_END            (502)           //写权限密码命令信息寄存器结束地址(不包括)

#define MB_SYSW_BASE_ADDR           (502)           //写系统控制命令信息寄存器起始地址(包括)
#define MB_SYSW_BASE_END            (550)           //写系统控制命令信息寄存器结束地址(不包括)
#define MB_USEW_BASE_ADDR           (550)           //写用户控制命令信息寄存器起始地址(包括)
#define MB_USEW_BASE_END            (600)           //写用户控制命令信息寄存器结束地址(不包括)
#define MB_DBGW_BASE_ADDR           (600)           //写系统调试命令信息寄存器起始地址(包括)
#define MB_DBGW_BASE_END            (650)           //写系统调试命令信息寄存器结束地址(不包括)

#define MB_SPAW_BASE_ADDR           (650)           //写系统参数配置命令信息寄存器起始地址(包括)
#define MB_SPAW_BASE_END            (685)           //写系统参数配置命令信息寄存器结束地址(不包括)
#define MB_PPAW_BASE_ADDR           (700)           //写堆控参数配置命令信息寄存器起始地址(包括)
#define MB_PPAW_BASE_END            (800)           //写堆控参数配置命令信息寄存器结束地址(不包括)
#define MB_GPAW_BASE_ADDR           (800)           //写主控参数配置命令信息寄存器起始地址(包括)
#define MB_GPAW_BASE_END            (990)           //写主控参数配置命令信息寄存器结束地址(不包括)
#define MB_HPAW_BASE_ADDR           (990)           //写触摸屏口参数配置命令信息寄存器起始地址(包括)
#define MB_HPAW_BASE_END            (992)           //写触摸屏口参数配置命令信息寄存器结束地址(不包括)

#define MB_PWDR_BASE_ADDR           (1000)          //读系统当前权限信息寄存器起始地址(包括)
#define MB_PWDR_BASE_END            (1002)          //读系统当前权限信息寄存器结束地址(不包括)

#define MB_SCMR_BASE_ADDR           (1002)          //读系统控制响应结果信息寄存器起始地址(包括)
#define MB_SCMR_BASE_END            (1050)          //读系统控制响应结果信息寄存器结束地址(不包括)
#define MB_UCMR_BASE_ADDR           (1050)          //读用户控制响应结果信息寄存器起始地址(包括)
#define MB_UCMR_BASE_END            (1100)          //读用户控制响应结果信息寄存器结束地址(不包括)
#define MB_DBGR_BASE_ADDR           (1100)          //读系统调试响应结果信息寄存器起始地址(包括)
#define MB_DBGR_BASE_END            (1150)          //读系统调试响应结果信息寄存器结束地址(不包括)

#define MB_SPAR_BASE_ADDR           (1150)          //读系统通用参数配置响应结果信息寄存器起始地址(包括)
#define MB_SPAR_BASE_END            (1200)          //读系统通用参数配置响应结果信息寄存器结束地址(不包括)
#define MB_PPAR_BASE_ADDR           (1200)          //读堆控参数配置响应结果信息寄存器起始地址(包括)
#define MB_PPAR_BASE_END            (1300)          //读堆控参数配置响应结果信息寄存器结束地址(不包括)
#define MB_GPAR_BASE_ADDR           (1300)          //读16组主控参数配置响应结果信息寄存器起始地址(包括)
#define MB_GPAR_BASE_END            (4500)          //读16组主控信息寄存器结束地址(不包括)
#define MB_GPAR_ADDR_LEN            (200)           //读每组主控参数配置响应结果信息寄存器地址长度

#define MB_PIFR_BASE_ADDR           (4500)          //读堆控信息寄存器起始地址(包括)
#define MB_PIFR_BASE_END            (5000)          //读堆控信息寄存器结束地址(不包括)

#define MB_GIFR_BASE_ADDR           (5000)          //读主控信息寄存器起始地址(包括)
#define MB_GIFR_BASE_END            (37000)         //读16组主控信息寄存器结束地址(不包括)
#define MB_GIFR_ADDR_LEN            (2000)          //读每组主控信息寄存器地址长度

/*******设备内部地址换分**************************************************************************/
/*系统参数配置区域地址分配*/
#define MB_SGPAW_ADDR_START         (650)           //写系统通用参数配置命令信息寄存器起始地址(包括)
#define MB_SGPAW_ADDR_END           (659)           //写系统通用参数配置命令信息寄存器结束地址(不包括)
#define MB_SHPAW_ADDR_START         (670)           //写系统硬件参数配置命令信息寄存器起始地址(包括)
#define MB_SHPAW_ADDR_END           (685)           //写系统硬件参数配置命令信息寄存器结束地址(不包括)

#define MB_SGPAR_ADDR_START         (1150)          //读系统通用参数配置命令信息寄存器起始地址(包括)
#define MB_SGPAR_ADDR_END           (1158)          //读系统通用参数配置命令信息寄存器结束地址(不包括)
#define MB_SHPAR_ADDR_START         (1170)          //读系统硬件参数配置命令信息寄存器起始地址(包括)
#define MB_SHPAR_ADDR_END           (1185)          //读系统硬件参数配置命令信息寄存器结束地址(不包括)

//主控参数配置区域地址分配
#define MB_GGPAW_ADDR_START         (800)           //写主控通用参数配置命令信息寄存器起始地址(包括)
#define MB_GGPAW_ADDR_END           (900)           //写主控通用参数配置命令信息寄存器结束地址(不包括)
#define MB_GMPAW_ADDR_START         (900)           //写主控高级参数配置命令信息寄存器起始地址(包括)
#define MB_GMPAW_ADDR_END           (960)           //写主控高级参数配置命令信息寄存器结束地址(不包括)
#define MB_GHPAW_ADDR_START         (960)           //写主控硬件参数配置命令信息寄存器起始地址(包括)
#define MB_GHPAW_ADDR_END           (990)           //写主控硬件参数配置命令信息寄存器结束地址(不包括)

#define MB_GGPAR_ADDR_START         (1300)          //读主控通用参数配置响应结果信息寄存器起始地址(包括)
#define MB_GGPAR_ADDR_END           (1400)          //读主控通用参数配置响应结果信息寄存器结束地址(不包括)
#define MB_GMPAR_ADDR_START         (1400)          //读主控高级参数配置响应结果信息寄存器起始地址(包括)
#define MB_GMPAR_ADDR_END           (1460)          //读主控高级参数配置响应结果信息寄存器结束地址(不包括)
#define MB_GHPAR_ADDR_START         (1460)          //读主控硬件参数配置响应结果信息寄存器起始地址(包括)
#define MB_GHPAR_ADDR_END           (1490)          //读主控硬件参数配置响应结果信息寄存器结束地址(不包括)

//堆控信息区域地址分配
#define MB_BPU_STA_ADDR_START       (4500)          //读堆控状态信息只读起始寄存器偏移地址(包括)
#define MB_BPU_STA_ADDR_END         (4504)          //读堆控状态信息只读结束寄存器偏移地址(不包括)

#define MB_BPU_LIM_ADDR_START       (4504)          //读堆控限值信息只读起始寄存器偏移地址(包括)
#define MB_BPU_LIM_ADDR_END         (4510)          //读堆控限值信息只读结束寄存器偏移地址(不包括)

#define MB_BPU_ERR_ADDR_START       (4510)          //读堆控故障告警信息只读起始寄存器偏移地址(包括)
#define MB_BPU_ERR_ADDR_END         (4523)          //读堆控故障告警信息只读结束寄存器偏移地址(不包括)

#define MB_BPU_IO_ADDR_START        (4523)          //读堆控输入输出IO信息只读起始寄存器偏移地址(包括)
#define MB_BPU_IO_ADDR_END          (4525)          //读堆控输入输出IO信息只读结束寄存器偏移地址(不包括)

#define MB_BPU_GST_ADDR_START       (4525)          //读堆控各电池组状态信息只读起始寄存器偏移地址(包括)
#define MB_BPU_GST_ADDR_END         (4528)          //读堆控各电池组状态信息只读结束寄存器偏移地址(不包括)

#define MB_BPU_DAT_ADDR_START       (4530)          //读堆控数据量信息只读起始寄存器偏移地址(包括)
#define MB_BPU_DAT_ADDR_END         (4570)          //读堆控数据量信息只读结束寄存器偏移地址(不包括)

#define MB_BPU_ENE_ADDR_START       (4570)          //读堆控电度量信息只读起始寄存器偏移地址(包括)
#define MB_BPU_ENE_ADDR_END         (4600)          //读堆控电度量信息只读结束寄存器偏移地址(不包括)

#define MB_BPU_HIER_ADDR_START      (4600)          //读堆控历史故障记录信息只读起始寄存器偏移地址(包括)
#define MB_BPU_HIER_ADDR_END        (4780)          //读堆控历史故障记录信息只读结束寄存器偏移地址(不包括)

#define MB_BPU_HIEV_ADDR_START      (4800)          //读堆控历史事件记录信息只读起始寄存器偏移地址(包括)
#define MB_BPU_HIEV_ADDR_END        (4980)          //读堆控历史事件记录信息只读结束寄存器偏移地址(不包括)

//主控信息区域地址分配
#define MB_BMU_STA_ADDR_START       (5000)          //读主控状态信息只读起始寄存器偏移地址(包括)
#define MB_BMU_STA_ADDR_END         (5004)          //读主控状态信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_LIM_ADDR_START       (5004)          //读主控限值信息只读起始寄存器偏移地址(包括)
#define MB_BMU_LIM_ADDR_END         (5010)          //读主控限值信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_ERR_ADDR_START       (5010)          //读主控故障告警信息只读起始寄存器偏移地址(包括)
#define MB_BMU_ERR_ADDR_END         (5018)          //读主控故障告警信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_IO_ADDR_START        (5018)          //读主控输入输出IO信息只读起始寄存器偏移地址(包括)
#define MB_BMU_IO_ADDR_END          (5020)          //读主控输入输出IO信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_DAT_ADDR_START       (5030)          //读主控数据量信息只读起始寄存器偏移地址(包括)
#define MB_BMU_DAT_ADDR_END         (5070)          //读主控数据量信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_ENE_ADDR_START       (5070)          //读主控电度量信息只读起始寄存器偏移地址(包括)
#define MB_BMU_ENE_ADDR_END         (5100)          //读主控电度量信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_HIER_ADDR_START      (5100)          //读主控历史故障记录信息只读起始寄存器偏移地址(包括)
#define MB_BMU_HIER_ADDR_END        (5280)          //读主控历史故障记录信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_HIEV_ADDR_START      (5300)          //读主控历史事件记录信息只读起始寄存器偏移地址(包括)
#define MB_BMU_HIEV_ADDR_END        (5480)          //读主控历史事件记录信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_INT_ADDR_START       (5500)          //读主控内部信息只读起始寄存器偏移地址(包括)
#define MB_BMU_INT_ADDR_END         (5600)          //读主控内部信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_CELCB_ADDR_START     (5600)          //读主控单体均充状态只读起始寄存器偏移地址(包括)
#define MB_BMU_CELCB_ADDR_END       (5625)          //读主控单体均充状态只读结束寄存器偏移地址(不包括)

#define MB_BMU_CELDB_ADDR_START     (5625)          //读主控单体均放状态只读起始寄存器偏移地址(包括)
#define MB_BMU_CELDB_ADDR_END       (5650)          //读主控单体均放状态只读结束寄存器偏移地址(不包括)

#define MB_BMU_CELV_ADDR_START      (5700)          //读主控单体电压信息只读起始寄存器偏移地址(包括)
#define MB_BMU_CELV_ADDR_END        (6100)          //读主控单体电压信息只读结束寄存器偏移地址(不包括)

#define MB_BMU_CELT_ADDR_START      (6100)          //读主控单体温度信息只读起始寄存器偏移地址(包括)
#define MB_BMU_CELT_ADDR_END        (6500)          //读主控单体温度信息只读结束寄存器偏移地址(不包括)

/*=================================================================================================
** @Data type definition
**===============================================================================================*/

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16* ModbusFindAllInfoAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找所有信息寄存器所对应的地址指针
** @The notes : 放在Modbus通讯包服务端(从机)接收处理HOOK
**===============================================================================================*/
u16* ModbusFindAllInfoAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindLoadInfoWRAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在线升级信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindLoadInfoWRAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindDeviceInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在设备信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindDeviceInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindProjInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在项目参数信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindProjInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindSofInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找故障记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSofInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindSoeInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找事件记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSoeInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindDtcInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找故障录波记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindDtcInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindDataRcdROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找历史数据记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindDataRcdROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindHisAffirmWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找历史记录确认命令信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindHisAffirmWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 data);

/*=================================================================================================
** @Name      : u16* ModbusFindVerTInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找系统当前时间信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindVerTInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindVerTInfoWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写系统校时信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindVerTInfoWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindSerialNumROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找设备序列号寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSerialNumROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindSerialNumWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写设备序列号信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindSerialNumWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupIdROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找当前读写电池组号信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupIdROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupIdWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写当前读写电池组号信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindGroupIdWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindPasswordWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入权限密码寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindPasswordWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindSysCtrlWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入系统控制命令寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindSysCtrlWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindUserCtrlWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入用户控制命令寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindUserCtrlWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindSysDebugWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入系统调试命令寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindSysDebugWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindSysParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入系统通用参数配置寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindSysParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindPileParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入堆控参数配置寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindPileParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入主控参数配置寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindGroupParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindGHMIParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入触摸屏口参数配置寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindGHMIParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data);

/*=================================================================================================
** @Name      : u16* ModbusFindPasswordROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读权限级别寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPasswordROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindSysCtrlROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读系统控制响应结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSysCtrlROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindUserCtrlROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读用户控制响应结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindUserCtrlROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindSysDebugROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读系统调试响应结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSysDebugROAddrMap(u8 fncCode, u8 sciNum, u16 address);


/*=================================================================================================
** @Name      : u16* ModbusFindSystemParaROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读系统参数配置结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSystemParaROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileParaROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控参数配置结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileParaROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupParaROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控参数配置结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupParaROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindPileStateInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileStateInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileLimInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控限值信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileLimInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileErrInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控故障信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileErrInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileIoStateROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控输入输出口信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileIoStateROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileGStateROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控各电池组状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileGStateROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileDataROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控数据量信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileDataROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileEnerROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控电能量信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileEnerROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileHisErrROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控历史故障记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileHisErrROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindPileHisEvnROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控历史事件记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileHisEvnROAddrMap(u8 fncCode, u8 sciNum, u16 address);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupStateInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupStateInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupLimInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控限值信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupLimInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupErrInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控故障告警信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupErrInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupIoStateROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控输入输出口状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupIoStateROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupDataROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控数据量信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupDataROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupEnerROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控电能量信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupEnerROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupHisErrROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控历史故障记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupHisErrROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupHisEvnROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控历史事件记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupHisEvnROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindGroupIntInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控内部从控信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupIntInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindCellChgBalanROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控单体均衡状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindCellChgBalanROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindCellDhgBalanROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控单体均放状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindCellDhgBalanROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindCellVoltROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控单体电压值信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindCellVoltROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

/*=================================================================================================
** @Name      : u16* ModbusFindCellTempROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控单体温度值信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindCellTempROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group);

#endif
