/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ModbusMap.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组主控Modbus通信地址映射模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "SciRcdRead.h"
#include "SciInfoPoint.h"
#include "SciCtrlHandle.h"
#include "SciParaHandle.h"
#include "ModbusMap.h"

#if(1 == PILE_MANAG_EN)
#include "PileSciInfoPt.h"
#include "PileSciParaHd.h"
#include "PileSciCtrlHd.h"
#endif

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16* ModbusFindAllInfoAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找所有信息寄存器所对应的地址指针
** @The notes : 放在Modbus通讯包服务端(从机)接收处理HOOK
**===============================================================================================*/
u16* ModbusFindAllInfoAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u16* temp = (void *)0;

    //查找项目在线升级信息寄存器地址
    if(address < MB_LOAD_BASE_END)
    {
        temp = ModbusFindLoadInfoWRAddrMap(fncCode, sciNum, address);
    }
    //查找设备参数信息寄存器地址
    else if((address >= MB_DEVR_BASE_ADDR) && (address < MB_DEVR_BASE_END))
    {
        temp = ModbusFindDeviceInfoROAddrMap(fncCode, sciNum, address);
    }
    //查找项目参数信息寄存器地址
    else if((address >= MB_PROR_BASE_ADDR) && (address < MB_PROR_BASE_END))
    {
        temp = ModbusFindProjInfoROAddrMap(fncCode, sciNum, address);
    }
    //查找故障记录信息寄存器地址
    else if((address >= MB_SOFR_BASE_ADDR) && (address < MB_SOFR_BASE_END))
    {
        temp = ModbusFindSofInfoROAddrMap(fncCode, sciNum, address);
    }
    //查找事件记录信息寄存器地址
    else if((address >= MB_SOER_BASE_ADDR) && (address < MB_SOER_BASE_END))
    {
        temp = ModbusFindSoeInfoROAddrMap(fncCode, sciNum, address);
    }
    //查找故障录波记录信息寄存器地址
    else if((address >= MB_DTCR_BASE_ADDR) && (address < MB_DTCR_BASE_END))
    {
        temp = ModbusFindDtcInfoROAddrMap(fncCode, sciNum, address);
    }
    //查找历史数据记录信息寄存器地址
    else if((address >= MB_RCDR_BASE_ADDR) && (address < MB_RCDR_BASE_END))
    {
        temp = ModbusFindDataRcdROAddrMap(fncCode, sciNum, address);
    }
    //查找历史记录读确认信息寄存器地址
    else if((address >= MB_CFMW_BASE_ADDR) && (address < MB_CFMW_BASE_END))
    {
        temp = ModbusFindHisAffirmWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找读校时信息寄存器地址
    else if((address >= MB_TIMR_BASE_ADDR) && (address < MB_TIMR_BASE_END))
    {
        temp = ModbusFindVerTInfoROAddrMap(fncCode, sciNum, address);
    }
    //查找写校时信息寄存器地址
    else if((address >= MB_TIMW_BASE_ADDR) && (address < MB_TIMW_BASE_END))
    {
        temp = ModbusFindVerTInfoWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找读序列号信息寄存器地址
    else if((address >= MB_SERR_BASE_ADDR) && (address < MB_SERR_BASE_END))
    {
        temp = ModbusFindSerialNumROAddrMap(fncCode, sciNum, address);
    }
    //查找写序列号信息寄存器地址
    else if((address >= MB_SERW_BASE_ADDR) && (address < MB_SERW_BASE_END))
    {
        temp = ModbusFindSerialNumWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找读电池组号信息寄存器地址
    else if((address >= MB_GIDR_BASE_ADDR) && (address < MB_GIDR_BASE_END))
    {
        temp = ModbusFindGroupIdROAddrMap(fncCode, sciNum, address);
    }
    //查找写电池组号信息寄存器地址
    else if((address >= MB_GIDW_BASE_ADDR) && (address < MB_GIDW_BASE_END))
    {
        temp = ModbusFindGroupIdWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找写权限密码寄存器地址
    else if((address >= MB_PWDW_BASE_ADDR) && (address < MB_PWDW_BASE_END))
    {
        temp = ModbusFindPasswordWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找写系统控制命令寄存器地址
    else if((address >= MB_SYSW_BASE_ADDR) && (address < MB_SYSW_BASE_END))
    {
        temp = ModbusFindSysCtrlWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找写用户控制命令寄存器地址
    else if((address >= MB_USEW_BASE_ADDR) && (address < MB_USEW_BASE_END))
    {
        temp = ModbusFindUserCtrlWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找写系统调试命令寄存器地址
    else if((address >= MB_DBGW_BASE_ADDR) && (address < MB_DBGW_BASE_END))
    {
        temp = ModbusFindSysDebugWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找写系统参数设置命令寄存器地址
    else if((address >= MB_SPAW_BASE_ADDR) && (address < MB_SPAW_BASE_END))
    {
        temp = ModbusFindSysParaWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找写堆控参数设置命令寄存器地址
    else if((address >= MB_PPAW_BASE_ADDR) && (address < MB_PPAW_BASE_END))
    {
        temp = ModbusFindPileParaWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找写主控参数设置命令寄存器地址(各主控地址共用,切换组号)
    else if((address >= MB_GPAW_BASE_ADDR) && (address < MB_GPAW_BASE_END))
    {
        temp = ModbusFindGroupParaWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找写主控触控屏口参数设置命令寄存器地址(各主控/各参数地址共用,切换组号/索引号)
    else if((address >= MB_HPAW_BASE_ADDR) && (address < MB_HPAW_BASE_END))
    {
        temp = ModbusFindGHMIParaWRAddrMap(fncCode, sciNum, address, data);
    }
    //查找读权限级别寄存器地址
    else if((address >= MB_PWDR_BASE_ADDR) && (address < MB_PWDR_BASE_END))
    {
        temp = ModbusFindPasswordROAddrMap(fncCode, sciNum, address);
    }
    //查找读系统控制响应结果寄存器地址
    else if((address >= MB_SCMR_BASE_ADDR) && (address < MB_SCMR_BASE_END))
    {
        temp = ModbusFindSysCtrlROAddrMap(fncCode, sciNum, address);
    }
    //查找读用户控制响应结果寄存器地址
    else if((address >= MB_UCMR_BASE_ADDR) && (address < MB_UCMR_BASE_END))
    {
        temp = ModbusFindUserCtrlROAddrMap(fncCode, sciNum, address);
    }
    //查找读系统调试响应结果寄存器地址
    else if((address >= MB_DBGR_BASE_ADDR) && (address < MB_DBGR_BASE_END))
    {
        temp = ModbusFindSysDebugROAddrMap(fncCode, sciNum, address);
    }
    //查找读系统参数配置结果信息寄存器地址
    else if((address >= MB_SPAR_BASE_ADDR) && (address < MB_SPAR_BASE_END))
    {
        temp = ModbusFindSystemParaROAddrMap(fncCode, sciNum, address);
    }
    //查找读堆控参数配置结果信息寄存器地址
    else if((address >= MB_PPAR_BASE_ADDR) && (address < MB_PPAR_BASE_END))
    {
        temp = ModbusFindPileParaROAddrMap(fncCode, sciNum, address);
    }
    //查找读主控常规参数配置结果寄存器地址(各电池组地址平铺)
    else if((address >= MB_GPAR_BASE_ADDR) && (address < MB_GPAR_BASE_END))
    {
        //group = (address - MB_GPAR_BASE_ADDR) / MB_GPAR_ADDR_LEN;                     //求电池组号(只有一组)
        address = MB_GPAR_BASE_ADDR + (address - MB_GPAR_BASE_ADDR) % MB_GPAR_ADDR_LEN; //转化为第一组主控内地址

        temp = ModbusFindGroupParaROAddrMap(fncCode, sciNum, address, 0);               //查找组内参数配置结果
    }
    //查找读堆控数据信息寄存器地址
    else if((address >= MB_PIFR_BASE_ADDR) && (address < MB_PIFR_BASE_END))
    {
        if(address < MB_BPU_STA_ADDR_END)                                               //查找堆控状态信息寄存器地址
        {
            temp = ModbusFindPileStateInfoROAddrMap(fncCode, sciNum, address);
        }
        else if((address >= MB_BPU_LIM_ADDR_START) && (address < MB_BPU_LIM_ADDR_END))  //查找堆控限值信息寄存器地址
        {
            temp = ModbusFindPileLimInfoROAddrMap(fncCode, sciNum, address);
        }
        else if((address >= MB_BPU_ERR_ADDR_START) && (address < MB_BPU_ERR_ADDR_END))  //查找堆控故障告警信息寄存器地址
        {
            temp = ModbusFindPileErrInfoROAddrMap(fncCode, sciNum, address);
        }
        else if((address >= MB_BPU_IO_ADDR_START) && (address < MB_BPU_IO_ADDR_END))    //查找堆控输入输出IO状态信息寄存器地址
        {
            temp = ModbusFindPileIoStateROAddrMap(fncCode, sciNum, address);
        }
        else if((address >= MB_BPU_GST_ADDR_START) && (address < MB_BPU_GST_ADDR_END))  //查找堆控各电池组状态信息寄存器地址
        {
            temp = ModbusFindPileGStateROAddrMap(fncCode, sciNum, address);
        }
        else if((address >= MB_BPU_DAT_ADDR_START) && (address < MB_BPU_DAT_ADDR_END))  //查找堆控数据量信息寄存器地址
        {
            temp = ModbusFindPileDataROAddrMap(fncCode, sciNum, address);
        }
        else if((address >= MB_BPU_ENE_ADDR_START) && (address < MB_BPU_ENE_ADDR_END))  //查找堆控电度量信息寄存器地址
        {
            temp = ModbusFindPileEnerROAddrMap(fncCode, sciNum, address);
        }
        else if((address >= MB_BPU_HIER_ADDR_START) && (address < MB_BPU_HIER_ADDR_END))//查找堆控历史故障记录信息寄存器地址
        {
            temp = ModbusFindPileHisErrROAddrMap(fncCode, sciNum, address);
        }
        else if((address >= MB_BPU_HIEV_ADDR_START) && (address < MB_BPU_HIEV_ADDR_END))//查找堆控历史事件记录信息寄存器地址
        {
            temp = ModbusFindPileHisEvnROAddrMap(fncCode, sciNum, address);
        }
        else if(3 == fncCode)
        {
            temp = SciGetPtrDefaultValueRO();                                            //读保留地址
        }
        else
        {
            temp = (void *)0;
        }
    }
    //查找主控数据信息寄存器地址(各电池组地址平铺)
    else if((address >= MB_GIFR_BASE_ADDR) && (address < MB_GIFR_BASE_END))
    {
        //group = (address - MB_GIFR_BASE_ADDR) / MB_GIFR_ADDR_LEN;                     //求主控组号(只有一组)
        address = MB_GIFR_BASE_ADDR + ((address - MB_GIFR_BASE_ADDR) % MB_GIFR_ADDR_LEN);//转化为第一组主控内地址

        if(address < MB_BMU_STA_ADDR_END)                                               //查找主控状态信息寄存器地址
        {
            temp = ModbusFindGroupStateInfoROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_LIM_ADDR_START) && (address < MB_BMU_LIM_ADDR_END))  //查找主控限值信息寄存器地址
        {
            temp = ModbusFindGroupLimInfoROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_ERR_ADDR_START) && (address < MB_BMU_ERR_ADDR_END))  //查找主控故障告警信息寄存器地址
        {
            temp = ModbusFindGroupErrInfoROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_IO_ADDR_START) && (address < MB_BMU_IO_ADDR_END))    //查找主控输入输出IO状态信息寄存器地址
        {
            temp = ModbusFindGroupIoStateROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_DAT_ADDR_START) && (address < MB_BMU_DAT_ADDR_END))  //查找主控数据量信息寄存器地址
        {
            temp = ModbusFindGroupDataROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_ENE_ADDR_START) && (address < MB_BMU_ENE_ADDR_END))  //查找主控电度量信息寄存器地址
        {
            temp = ModbusFindGroupEnerROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_HIER_ADDR_START) && (address < MB_BMU_HIER_ADDR_END))//查找主控历史故障记录信息寄存器地址
        {
            temp = ModbusFindGroupHisErrROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_HIEV_ADDR_START) && (address < MB_BMU_HIEV_ADDR_END))//查找主控历史故障记录信息寄存器地址
        {
            temp = ModbusFindGroupHisEvnROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_INT_ADDR_START) && (address < MB_BMU_INT_ADDR_END))//查找主控内部信息寄存器地址
        {
            temp = ModbusFindGroupIntInfoROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_CELCB_ADDR_START) && (address < MB_BMU_CELCB_ADDR_END))//查找主控单体均充状态信息寄存器地址
        {
            temp = ModbusFindCellChgBalanROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_CELDB_ADDR_START) && (address < MB_BMU_CELDB_ADDR_END))//查找主控单体均放状态信息寄存器地址
        {
            temp = ModbusFindCellDhgBalanROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_CELV_ADDR_START) && (address < MB_BMU_CELV_ADDR_END))//查找主控单体电压信息寄存器地址
        {
            temp = ModbusFindCellVoltROAddrMap(fncCode, sciNum, address, 0);
        }
        else if((address >= MB_BMU_CELT_ADDR_START) && (address < MB_BMU_CELT_ADDR_END))//查找主控单体温度信息寄存器地址
        {
            temp = ModbusFindCellTempROAddrMap(fncCode, sciNum, address, 0);
        }
        else if(3 == fncCode)
        {
            temp = SciGetPtrDefaultValueRO();                                           //读保留地址
        }
        else
        {
            temp = (void *)0;
        }
    }
    else if(3 == fncCode)
    {
        temp = SciGetPtrDefaultValueRO();                                               //读保留地址
    }
    else if((6 == fncCode) || (16 == fncCode))
    {
        temp = SciGetPtrDefaultValueWR();                                               //写保留地址
    }
    else
    {
        temp = (void *)0;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindLoadInfoWRAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在线升级信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindLoadInfoWRAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    //RS485烧录接收处理
    //temp = RcvRS485LoadMsgDeal_API(fncCode, sciNum, address);
    temp = SciGetPtrDefaultValueWR();

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindDeviceInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在设备信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindDeviceInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //地址正确
    if(address >= MB_DEVR_BASE_ADDR)
    {
        //计算对应信息索引
        index = address - MB_DEVR_BASE_ADDR;

        //获取设备信息相关信息
        temp = SciGetPtrDeviceInfoRO(index);
    }
    //地址错误
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindProjInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在项目参数信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindProjInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //地址正确
    if(address >= MB_PROR_BASE_ADDR)
    {
        //计算对应信息索引
        index = address - MB_PROR_BASE_ADDR;

        //获取项目参数相关信息
        temp = SciGetPtrProjectInfoRO(index);
    }
    //地址错误
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSofInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找故障记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSofInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //寄存器只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //查询读故障记录状态
        case MB_SOFR_BASE_ADDR:
            temp = SciGetPtrReadSofStateRO();
            break;

        //查询故障数目
        case MB_SOFR_BASE_ADDR + 1:
            temp = SciGetPtrSofAllNumRO();
            break;

        //查询故障序号
        case MB_SOFR_BASE_ADDR + 2:
        //查询故障定位码
        case MB_SOFR_BASE_ADDR + 3:
        //查询故障编码
        case MB_SOFR_BASE_ADDR + 4:
        //查询故障时间信息
        case MB_SOFR_BASE_ADDR + 5:
        case MB_SOFR_BASE_ADDR + 6:
        case MB_SOFR_BASE_ADDR + 7:
        case MB_SOFR_BASE_ADDR + 8:
        //查询故障附加信息
        case MB_SOFR_BASE_ADDR + 9:
        case MB_SOFR_BASE_ADDR + 10:
        case MB_SOFR_BASE_ADDR + 11:
        case MB_SOFR_BASE_ADDR + 12:
        case MB_SOFR_BASE_ADDR + 13:
            index = (u8)(address - MB_SOFR_BASE_ADDR - 2);
            temp = SciGetPtrSofInfoRO(index);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSoeInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找事件记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSoeInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //寄存器只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //查询读事件状态
        case MB_SOER_BASE_ADDR:
            temp = SciGetPtrReadSoeStateRO();
            break;

        //查询事件数目
        case MB_SOER_BASE_ADDR + 1:
            temp = SciGetPtrSoeAllNumRO();
            break;

        //查询事件序号
        case MB_SOER_BASE_ADDR + 2:
        //查询事件定位码
        case MB_SOER_BASE_ADDR + 3:
        //查询事件编码
        case MB_SOER_BASE_ADDR + 4:
        //查询事件时间信息
        case MB_SOER_BASE_ADDR + 5:
        case MB_SOER_BASE_ADDR + 6:
        case MB_SOER_BASE_ADDR + 7:
        case MB_SOER_BASE_ADDR + 8:
        //查询事件附加信息
        case MB_SOER_BASE_ADDR + 9:
        case MB_SOER_BASE_ADDR + 10:
        case MB_SOER_BASE_ADDR + 11:
        case MB_SOER_BASE_ADDR + 12:
        case MB_SOER_BASE_ADDR + 13:
            index = (u8)(address - MB_SOER_BASE_ADDR - 2);
            temp = SciGetPtrSoeInfoRO(index);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindDtcInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找故障录波记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindDtcInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    //u8 index = 0;
    u16* temp = (void*)0;

    //寄存器只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    temp = SciGetPtrDefaultValueRO();

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindDataRcdROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找历史数据记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindDataRcdROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //寄存器只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    //查询读数据记录状态
    if(address == MB_RCDR_BASE_ADDR)
    {
        temp = SciGetPtrReadSodStateRO();
    }
    //查询读数据记录总数目
    else if((address >= (MB_RCDR_BASE_ADDR + 1)) && (address < (MB_RCDR_BASE_ADDR + 4)))
    {
        index = address - MB_RCDR_BASE_ADDR - 1;
        temp = SciGetPtrReadSodAllNumRO(index);
    }
    //查询读数据记录每天数目
    else if((address >= (MB_RCDR_BASE_ADDR + 4)) && (address < (MB_RCDR_BASE_ADDR + 36)))
    {
        index = address - MB_RCDR_BASE_ADDR - 4;
        temp = SciGetPtrReadSodDayNumRO(index);
    }
    //查询读数据记录每天数目
    else if(address >= (MB_RCDR_BASE_ADDR + 36))
    {
        index = address - MB_RCDR_BASE_ADDR - 36;
        temp = SciGetPtrReadSodInfoRO(index);
    }
    //预留地址
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindHisAffirmWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找历史记录确认命令信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindHisAffirmWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 data)
{
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    switch(address)
    {
        //写故障记录读确认
        case MB_CFMW_BASE_ADDR:
            temp = SciGetPtrRdSofAffirmWR(fncCode, sciNum, data);
            break;

        //写读事件记录读确认
        case MB_CFMW_BASE_ADDR + 1:
            temp = SciGetPtrRdSoeAffirmWR(fncCode, sciNum, data);
            break;

        //写故障录波记录读确认
        case MB_CFMW_BASE_ADDR + 2:
            //temp = SciGetPtrRdDtcAffirmWR(fncCode, sciNum, data);
            temp = SciGetPtrDefaultValueWR();
            break;

        //写运行数据记录读确认
        case MB_CFMW_BASE_ADDR + 3:
            temp = SciGetPtrRdSodAffirmWR(fncCode, sciNum, data);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueWR();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindVerTInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找系统当前时间信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindVerTInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //寄存器只读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //查询时间-年
        case MB_TIMR_BASE_ADDR:
        //查询时间-月-日
        case MB_TIMR_BASE_ADDR + 1:
        //查询时间-时-分
        case MB_TIMR_BASE_ADDR + 2:
        //查询时间-秒
        case MB_TIMR_BASE_ADDR + 3:
            index = (u8)(address - MB_TIMR_BASE_ADDR);
            temp = SciGetPtrVerTimeInfoRO(index);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindVerTInfoWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写系统校时信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindVerTInfoWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    switch(address)
    {
        //写入时间-年
        case MB_TIMW_BASE_ADDR:
        //写入时间-月-日
        case MB_TIMW_BASE_ADDR + 1:
        //写入时间-时-分
        case MB_TIMW_BASE_ADDR + 2:
        //写入时间-秒
        case MB_TIMW_BASE_ADDR + 3:
        //写入时间-确认
        case MB_TIMW_BASE_ADDR + 4:
            index = (u8)(address - MB_TIMW_BASE_ADDR);
            temp = SciGetPtrVerTimeInfoWR(fncCode, sciNum, index, data);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueWR();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSerialNumROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找设备序列号寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSerialNumROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //寄存器只读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    if((address >= MB_SERR_BASE_ADDR) && (address < MB_SERR_BASE_END))
    {
        //求索引
        index = address - MB_SERR_BASE_ADDR;

        //读取序列号
        temp = SciGetPtrSerialNumInfoRO(index);
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSerialNumWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写设备序列号信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindSerialNumWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        return(temp);
    }

    if((address >= MB_SERW_BASE_ADDR) && (address < MB_SERW_BASE_END))
    {
        //求索引
        index = address - MB_SERW_BASE_ADDR;

        //写入序列号
        temp = SciGetPtrSerialNumInfoWR(fncCode, sciNum, index, data);
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupIdROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找当前读写电池组号信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupIdROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //寄存器只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //参数配置组号
        case MB_GIDR_BASE_ADDR:
            temp = SciGetPtrParaCfgGroupId();
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupIdWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写当前读写电池组号信息寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindGroupIdWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //写入参数配置组号
        case MB_GIDW_BASE_ADDR:
            temp = SciGetPtrParaCfgGroupId();
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueWR();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPasswordWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入权限密码寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindPasswordWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    switch(address)
    {
        //权限用户名
        case MB_PWDW_BASE_ADDR:
        case MB_PWDW_BASE_ADDR + 1:
            index = address - MB_PWDW_BASE_ADDR;
            #if(1 == PILE_MANAG_EN)
            temp = SciGetPtrPileAuthPwdWR(fncCode, sciNum, index, data);
            #else
            temp = SciGetPtrAuthPasswordWR(fncCode, sciNum, index, data);
            #endif
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueWR();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSysCtrlWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入系统控制命令寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindSysCtrlWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    switch(address)
    {
        #if(1 == PILE_MANAG_EN)
        //系统工作状态控制
        case MB_SYSW_BASE_ADDR:
            temp = SciGetPtrPileWStateCmdWR(fncCode, sciNum, data);
            break;

        //系统工作模式切换
        case MB_SYSW_BASE_ADDR + 1:
            temp = SciGetPtrPileWModeCmdWR(fncCode, sciNum, data);
            break;

        //清除记录命令
        case MB_SYSW_BASE_ADDR + 2:
            temp = SciGetPtrPileSysClrCmdWR(fncCode, sciNum, data);
            break;

        #else
        //系统工作状态控制
        case MB_SYSW_BASE_ADDR:
            temp = SciGetPtrWorkStateCmdWR(fncCode, sciNum, data);
            break;

        //系统工作模式切换
        case MB_SYSW_BASE_ADDR + 1:
            temp = SciGetPtrWorkModeCmdWR(fncCode, sciNum, data);
            break;

        //清除记录命令
        case MB_SYSW_BASE_ADDR + 2:
            temp = SciGetPtrSysClrCmdWR(fncCode, sciNum, data);
            break;
        #endif

        //预留地址
        default:
            temp = SciGetPtrDefaultValueWR();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindUserCtrlWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入用户控制命令寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindUserCtrlWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    switch(address)
    {
        //用户控制(串口不进行用户控制)
        case MB_USEW_BASE_ADDR:
            temp = SciGetPtrDefaultValueWR();
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueWR();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSysDebugWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入系统调试命令寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindSysDebugWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    switch(address)
    {
        //调试类型
        case MB_DBGW_BASE_ADDR:
        //调试电池组编号
        case MB_DBGW_BASE_ADDR + 1:
        //调试BMU编号
        case MB_DBGW_BASE_ADDR + 2:
        //调试端口编号
        case MB_DBGW_BASE_ADDR + 3:
        //调试状态
        case MB_DBGW_BASE_ADDR + 4:
        //调试电池
        case MB_DBGW_BASE_ADDR + 5:
        //调试保留
        case MB_DBGW_BASE_ADDR + 6:
        //调试确认
        case MB_DBGW_BASE_ADDR + 7:
            index = (u8)(address - MB_DBGW_BASE_ADDR);
            #if(1 == PILE_MANAG_EN)
            temp = SciGetPtrPileDebugCmdWR(fncCode, sciNum, index, data);
            #else
            temp = SciGetPtrSysDebugCmdWR(fncCode, sciNum, index, data);
            #endif
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueWR();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSysParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入系统通用参数配置寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindSysParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    //系统通用参数地址正确
    if((address >= MB_SGPAW_ADDR_START) && (address < MB_SGPAW_ADDR_END))
    {
        index = address - MB_SGPAW_ADDR_START;
        temp = SciGetPtrSysGenParaCfgWR(fncCode, sciNum, index, data);
    }
    //系统硬件参数地址正确
    else if((address >= MB_SHPAW_ADDR_START) && (address < MB_SHPAW_ADDR_END))
    {
        index = address - MB_SHPAW_ADDR_START;
        temp = SciGetPtrSysHWParaCfgWR(fncCode, sciNum, index, data);
    }
    //地址错误
    else
    {
        temp = SciGetPtrDefaultValueWR();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入堆控参数配置寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindPileParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    //参数地址正确
    if((address >= MB_PPAW_BASE_ADDR) && (address < MB_PPAW_BASE_END))
    {
        #if(1 == PILE_MANAG_EN)
        temp = SciGetPtrPileGenParaCfgWR(fncCode, sciNum, (address - MB_PPAW_BASE_ADDR), data);
        #else
        temp = SciGetPtrDefaultValueWR();
        #endif
    }
    //地址错误
    else
    {
        temp = SciGetPtrDefaultValueWR();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入主控参数配置寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindGroupParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    //主控通用参数
    if((address >= MB_GGPAW_ADDR_START) && (address < MB_GGPAW_ADDR_END))
    {
        //求配置参数序号
        index = (u8)(address - MB_GGPAW_ADDR_START);
        temp = SciGetPtrGroupGenParaCfgWR(fncCode, sciNum, index, data);
    }
    //主控高级参数
    else if((address >= MB_GMPAW_ADDR_START) && (address < MB_GMPAW_ADDR_END))
    {
        //求配置参数序号
        index = (u8)(address - MB_GMPAW_ADDR_START);
        temp = SciGetPtrGroupHigParaCfgWR(fncCode, sciNum, index, data);
    }
    //主控硬件参数
    else if((address >= MB_GHPAW_ADDR_START) && (address < MB_GHPAW_ADDR_END))
    {
        //求配置参数序号
        index = (u8)(address - MB_GHPAW_ADDR_START);
        temp = SciGetPtrGroupHWParaCfgWR(fncCode, sciNum, index, data);
    }
    //非法参数
    else
    {
        temp = SciGetPtrDefaultValueWR();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGHMIParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 data:写入数据(读无效)
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找写入触摸屏口参数配置寄存器所对应的地址指针
** @The notes : 读/写
**===============================================================================================*/
u16* ModbusFindGHMIParaWRAddrMap(u8 fncCode, u8 sciNum, u16 address, u16 data)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读写
    if((3 != fncCode) && (6 != fncCode) && (0x10 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }

    switch(address)
    {
        //参数值写入
        case MB_HPAW_BASE_ADDR:
        //参数配置确认
        case MB_HPAW_BASE_ADDR + 1:
            index = (u8)(address - MB_HPAW_BASE_ADDR);
            temp = SciGetPtrGroupHMIParaCfgWR(fncCode, sciNum, index, data);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueWR();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPasswordROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读权限级别寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPasswordROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //用户级别
        case MB_PWDR_BASE_ADDR:
        //权限端口号
        case MB_PWDR_BASE_ADDR + 1:
            index = address - MB_PWDR_BASE_ADDR;
            #if(1 == PILE_MANAG_EN)
            temp = SciGetPtrPileAuthPwdRO(index);
            #else
            temp = SciGetPtrAuthPasswordRO(index);
            #endif
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSysCtrlROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读系统控制响应结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSysCtrlROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //
        case MB_SCMR_BASE_ADDR:
            temp = SciGetPtrDefaultValueRO();
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindUserCtrlROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读用户控制响应结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindUserCtrlROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //
        case MB_UCMR_BASE_ADDR:
            temp = SciGetPtrDefaultValueRO();
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindSysDebugROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读系统调试响应结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSysDebugROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        //调试类型
        case MB_DBGR_BASE_ADDR:
        //调试电池组编号
        case MB_DBGR_BASE_ADDR + 1:
        //调试BMU编号
        case MB_DBGR_BASE_ADDR + 2:
        //调试端口
        case MB_DBGR_BASE_ADDR + 3:
        //调试状态
        case MB_DBGR_BASE_ADDR + 4:
        //调试电池
        case MB_DBGR_BASE_ADDR + 5:
        //调试保留
        case MB_DBGR_BASE_ADDR + 6:
        //调试读EEPROM参数低16位
        case MB_DBGR_BASE_ADDR + 7:
        //调试读EEPROM参数高16位
        case MB_DBGR_BASE_ADDR + 8:
        //调试结果
        case MB_DBGR_BASE_ADDR + 9:
        //采样校准类型
        case MB_DBGR_BASE_ADDR + 10:
        //采样校准状态
        case MB_DBGR_BASE_ADDR + 11:
        //采样校准结果
        case MB_DBGR_BASE_ADDR + 12:
            //求调试结果序号
            index = (u8)(address - MB_DBGR_BASE_ADDR);
            #if(1 == PILE_MANAG_EN)
            temp = SciGetPtrPileDebugResultRO(index);
            #else
            temp = SciGetPtrSysDebugResultRO(index);
            #endif
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}


/*=================================================================================================
** @Name      : u16* ModbusFindSystemParaROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读系统参数配置结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindSystemParaROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    //系统通用参数地址正确
    if((address >= MB_SGPAR_ADDR_START) && (address < MB_SGPAR_ADDR_END))
    {
        //求配置参数序号
        index = (u8)(address - MB_SGPAR_ADDR_START);
        temp = SciGetPtrSystemGenParaCfgRO(index);
    }
    //系统硬件地址正确
    else if((address >= MB_SHPAR_ADDR_START) && (address < MB_SHPAR_ADDR_END))
    {
        //求配置参数序号
        index = (u8)(address - MB_SHPAR_ADDR_START);
        temp = SciGetPtrSystemHWParaCfgRO(index);
    }
    //非法地址
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileParaROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控参数配置结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileParaROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    //地址正确
    if((address >= MB_PPAR_BASE_ADDR) && (address < MB_PPAR_BASE_END))
    {
        #if(1 == PILE_MANAG_EN)
        //求配置参数序号
        temp = SciGetPtrPileGenParaCfgRO((u8)(address - MB_PPAR_BASE_ADDR));
        #else
        temp = SciGetPtrDefaultValueRO();
        #endif
    }
    //非法地址
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupParaROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控参数配置结果信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupParaROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    //只有1组
    (void)group;

    //主控常规参数
    if((address >= MB_GGPAR_ADDR_START) && (address < MB_GGPAR_ADDR_END))
    {
        //求配置参数序号
        index = (u8)(address - MB_GGPAR_ADDR_START);
        temp = SciGetPtrGroupGenParaCfgRO(index);
    }
    //主控高级参数
    else if((address >= MB_GMPAR_ADDR_START) && (address < MB_GMPAR_ADDR_END))
    {
        //求配置参数序号
        index = (u8)(address - MB_GMPAR_ADDR_START);
        temp = SciGetPtrGroupHigParaCfgRO(index);
    }
    //主控硬件参数
    else if((address >= MB_GHPAR_ADDR_START) && (address < MB_GHPAR_ADDR_END))
    {
        //求配置参数序号
        index = (u8)(address - MB_GHPAR_ADDR_START);
        temp = SciGetPtrGroupHWParaCfgRO(index);
    }
    //非法参数
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileStateInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileStateInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        #if(1 == PILE_MANAG_EN)
        //电池堆电池保护状态
        case MB_BPU_STA_ADDR_START:
            temp = SciGetPtrPileStateInfoRO(0);
            break;

        //电池堆工作状态
        case MB_BPU_STA_ADDR_START + 1:
            temp = SciGetPtrPileStateInfoRO(1);
            break;

        //电池堆充放电状态
        case MB_BPU_STA_ADDR_START + 2:
            temp = SciGetPtrPileStateInfoRO(2);
            break;

        //电池堆工作模式
        case MB_BPU_STA_ADDR_START + 3:
            temp = SciGetPtrPileStateInfoRO(3);
            break;

        #else
        //电池堆电池保护状态
        case MB_BPU_STA_ADDR_START:
            temp = SciGetPtrGroupStateInfoRO(0);
            break;

        //电池堆工作状态
        case MB_BPU_STA_ADDR_START + 1:
            temp = SciGetPtrGroupStateInfoRO(1);
            break;

        //电池堆充放电状态
        case MB_BPU_STA_ADDR_START + 2:
            temp = SciGetPtrGroupStateInfoRO(2);
            break;

        //电池堆工作模式
        case MB_BPU_STA_ADDR_START + 3:
            temp = SciGetPtrGroupStateInfoRO(3);
            break;
        #endif

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileLimInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控限值信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileLimInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        #if(1 == PILE_MANAG_EN)
        //电池堆充电电流限值
        case MB_BPU_LIM_ADDR_START:
            temp = SciGetPtrPileLimitInfoRO(0);
            break;

        //电池堆放电电流限值
        case MB_BPU_LIM_ADDR_START + 1:
            temp = SciGetPtrPileLimitInfoRO(1);
            break;

        //电池堆充电功率限值
        case MB_BPU_LIM_ADDR_START + 2:
            temp = SciGetPtrPileLimitInfoRO(2);
            break;

        //电池堆放电功率限值
        case MB_BPU_LIM_ADDR_START + 3:
            temp = SciGetPtrPileLimitInfoRO(3);
            break;

        //电池堆充电电压限值
        case MB_BPU_LIM_ADDR_START + 4:
            temp = SciGetPtrPileLimitInfoRO(4);
            break;

        //电池堆放电电压限值
        case MB_BPU_LIM_ADDR_START + 5:
            temp = SciGetPtrPileLimitInfoRO(5);
            break;

        #else
        //电池堆充电电流限值
        case MB_BPU_LIM_ADDR_START:
            temp = SciGetPtrGroupLimitInfoRO(0);
            break;

        //电池堆放电电流限值
        case MB_BPU_LIM_ADDR_START + 1:
            temp = SciGetPtrGroupLimitInfoRO(1);
            break;

        //电池堆充电功率限值
        case MB_BPU_LIM_ADDR_START + 2:
            temp = SciGetPtrGroupLimitInfoRO(2);
            break;

        //电池堆放电功率限值
        case MB_BPU_LIM_ADDR_START + 3:
            temp = SciGetPtrGroupLimitInfoRO(3);
            break;

        //电池堆充电电压限值
        case MB_BPU_LIM_ADDR_START + 4:
            temp = SciGetPtrGroupLimitInfoRO(4);
            break;

        //电池堆放电电压限值
        case MB_BPU_LIM_ADDR_START + 5:
            temp = SciGetPtrGroupLimitInfoRO(5);
            break;
        #endif

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileErrInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控故障信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileErrInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        #if(1 == PILE_MANAG_EN)
        //堆告警信息
        case MB_BPU_ERR_ADDR_START:
        case MB_BPU_ERR_ADDR_START + 1:
        case MB_BPU_ERR_ADDR_START + 2:
        case MB_BPU_ERR_ADDR_START + 3:
        //堆故障信息
        case MB_BPU_ERR_ADDR_START + 4:
        case MB_BPU_ERR_ADDR_START + 5:
        case MB_BPU_ERR_ADDR_START + 6:
        case MB_BPU_ERR_ADDR_START + 7:
        //堆提示信息
        case MB_BPU_ERR_ADDR_START + 8:
        case MB_BPU_ERR_ADDR_START + 9:
        case MB_BPU_ERR_ADDR_START + 10:
        case MB_BPU_ERR_ADDR_START + 11:
            temp = SciGetPtrPileErrInfoRO((u8)(address - MB_BPU_ERR_ADDR_START));
            break;

        #else
        //堆告警信息
        case MB_BPU_ERR_ADDR_START:
        case MB_BPU_ERR_ADDR_START + 1:
        case MB_BPU_ERR_ADDR_START + 2:
        case MB_BPU_ERR_ADDR_START + 3:
        //堆故障信息
        case MB_BPU_ERR_ADDR_START + 4:
        case MB_BPU_ERR_ADDR_START + 5:
        case MB_BPU_ERR_ADDR_START + 6:
        case MB_BPU_ERR_ADDR_START + 7:
        //堆提示信息
        case MB_BPU_ERR_ADDR_START + 8:
        case MB_BPU_ERR_ADDR_START + 9:
        case MB_BPU_ERR_ADDR_START + 10:
        case MB_BPU_ERR_ADDR_START + 11:
            //temp = SciGetPtrGroupErrInfoRO((u8)(address - MB_BPU_ERR_ADDR_START));
            temp = SciGetPtrDefaultValueRO();
            break;
        #endif

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileIoStateROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控输入输出口信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileIoStateROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        #if(1 == PILE_MANAG_EN)
        //堆外部输入IO状态
        case MB_BPU_IO_ADDR_START:
        //堆外部输出IO状态
        case MB_BPU_IO_ADDR_START + 1:
        //堆内部输入IO状态
        case MB_BPU_IO_ADDR_START + 2:
        //堆内部输出IO状态
        case MB_BPU_IO_ADDR_START + 3:
            temp = SciGetPtrPileIoStateInfoRO((u8)(address - MB_BPU_IO_ADDR_START));
            break;

        #else
        //堆外部输入IO状态
        case MB_BPU_IO_ADDR_START:
        //堆外部输出IO状态
        case MB_BPU_IO_ADDR_START + 1:
        //堆内部输入IO状态
        case MB_BPU_IO_ADDR_START + 2:
        //堆内部输出IO状态
        case MB_BPU_IO_ADDR_START + 3:
            //temp = SciGetPtrGroupIoStateInfoRO((u8)(address - MB_BPU_IO_ADDR_START));
            temp = SciGetPtrDefaultValueRO();
            break;
        #endif

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileGStateROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控各电池组状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileGStateROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        #if(1 == PILE_MANAG_EN)
        //堆电池组主控使能状态
        case MB_BPU_GST_ADDR_START:
            temp = SciGetPtrPileIoStateInfoRO(4);
            break;

        //堆电池组主控正常状态
        case MB_BPU_GST_ADDR_START + 1:
            temp = SciGetPtrPileIoStateInfoRO(5);
            break;

        //堆电池组主控运行状态
        case MB_BPU_GST_ADDR_START + 2:
            temp = SciGetPtrPileIoStateInfoRO(6);
            break;

        //堆电池组主控故障状态
        case MB_BPU_GST_ADDR_START + 3:
            temp = SciGetPtrPileIoStateInfoRO(7);
            break;

        #else
        //堆电池组主控使能状态
        case MB_BPU_GST_ADDR_START:
        //堆电池组主控正常状态
        case MB_BPU_GST_ADDR_START + 1:
        //堆电池组主控运行状态
        case MB_BPU_GST_ADDR_START + 2:
        //堆电池组主控故障状态
        case MB_BPU_GST_ADDR_START + 3:
        #endif

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileDataROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控数据量信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileDataROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    switch(address)
    {
        #if(1 == PILE_MANAG_EN)
        //电池堆总电压
        case MB_BPU_DAT_ADDR_START:
            temp = SciGetPtrPileMainInfoRO(0);
            break;

        //电池堆输出电流
        case MB_BPU_DAT_ADDR_START + 1:
            temp = SciGetPtrPileMainInfoRO(1);
            break;

        //电池堆输出功率
        case MB_BPU_DAT_ADDR_START + 2:
            temp = SciGetPtrPileMainInfoRO(2);
            break;

        //电池堆外端预充电压
        case MB_BPU_DAT_ADDR_START + 3:
            temp = SciGetPtrPileMainInfoRO(3);
            break;

        //电池堆运行SOC
        case MB_BPU_DAT_ADDR_START + 4:
            temp = SciGetPtrPileCapInfoRO(0);
            break;

        //电池堆整体SOE
        case MB_BPU_DAT_ADDR_START + 5:
            temp = SciGetPtrPileCapInfoRO(1);
            break;

        //电池堆电池SOH
        case MB_BPU_DAT_ADDR_START + 6:
            temp = SciGetPtrPileCapInfoRO(2);
            break;

        //电池堆电池总容量
        case MB_BPU_DAT_ADDR_START + 7:
            temp = SciGetPtrPileCapInfoRO(3);
            break;

        //电池堆单体最大电压电池组号
        case MB_BPU_DAT_ADDR_START + 8:
            temp = SciGetPtrPileCellCharaInfoRO(0);
            break;

        //电池堆单体最大电压序号
        case MB_BPU_DAT_ADDR_START + 9:
            temp = SciGetPtrPileCellCharaInfoRO(1);
            break;

        //电池组单体最大电压值
        case MB_BPU_DAT_ADDR_START + 10:
            temp = SciGetPtrPileCellCharaInfoRO(2);
            break;

        //电池堆单体累加总压平均值
        case MB_BPU_DAT_ADDR_START + 11:
            temp = SciGetPtrPileCellCharaInfoRO(3);
            break;

        //电池堆单体最小电压电池组号
        case MB_BPU_DAT_ADDR_START + 12:
            temp = SciGetPtrPileCellCharaInfoRO(4);
            break;

        //电池堆单体最小电压序号
        case MB_BPU_DAT_ADDR_START + 13:
            temp = SciGetPtrPileCellCharaInfoRO(5);
            break;

        //电池堆单体最小电压值
        case MB_BPU_DAT_ADDR_START + 14:
            temp = SciGetPtrPileCellCharaInfoRO(6);
            break;

        //电池堆单体平均电压值
        case MB_BPU_DAT_ADDR_START + 15:
            temp = SciGetPtrPileCellCharaInfoRO(7);
            break;

        //电池堆单体最高温度电池组号
        case MB_BPU_DAT_ADDR_START + 16:
            temp = SciGetPtrPileCellCharaInfoRO(8);
            break;

        //电池堆单体最高温度序号
        case MB_BPU_DAT_ADDR_START + 17:
            temp = SciGetPtrPileCellCharaInfoRO(9);
            break;

        //电池堆单体最高温度值
        case MB_BPU_DAT_ADDR_START + 18:
            temp = SciGetPtrPileCellCharaInfoRO(10);
            break;

        //电池堆单体平均温度值
        case MB_BPU_DAT_ADDR_START + 19:
            temp = SciGetPtrPileCellCharaInfoRO(11);
            break;

        //电池堆单体最低温度电池组号
        case MB_BPU_DAT_ADDR_START + 20:
            temp = SciGetPtrPileCellCharaInfoRO(12);
            break;

        //电池堆单体最低温度序号
        case MB_BPU_DAT_ADDR_START + 21:
            temp = SciGetPtrPileCellCharaInfoRO(13);
            break;

        //电池堆单体最低温度值
        case MB_BPU_DAT_ADDR_START + 22:
            temp = SciGetPtrPileCellCharaInfoRO(14);
            break;

        //电池堆环境温度值
        case MB_BPU_DAT_ADDR_START + 23:
            temp = SciGetPtrPileCellCharaInfoRO(15);
            break;

        //电池堆最大总压电池组号
        case MB_BPU_DAT_ADDR_START + 24:
            temp = SciGetPtrPileSumVCCharaInfoRO(0);
            break;

        //电池堆最大总压电池组总压值
        case MB_BPU_DAT_ADDR_START + 25:
            temp = SciGetPtrPileSumVCCharaInfoRO(1);
            break;

        //电池堆最小总压电池组号
        case MB_BPU_DAT_ADDR_START + 26:
            temp = SciGetPtrPileSumVCCharaInfoRO(2);
            break;

        //电池堆最小总压电池组总压值
        case MB_BPU_DAT_ADDR_START + 27:
            temp = SciGetPtrPileSumVCCharaInfoRO(3);
            break;

        //电池堆最大电流电池组号
        case MB_BPU_DAT_ADDR_START + 28:
            temp = SciGetPtrPileSumVCCharaInfoRO(4);
            break;

        //电池堆最大电流电池组电流值
        case MB_BPU_DAT_ADDR_START + 29:
            temp = SciGetPtrPileSumVCCharaInfoRO(5);
            break;

        //电池堆最小电流电池组号
        case MB_BPU_DAT_ADDR_START + 30:
            temp = SciGetPtrPileSumVCCharaInfoRO(6);
            break;

        //电池堆最小电流电池组电流值
        case MB_BPU_DAT_ADDR_START + 31:
            temp = SciGetPtrPileSumVCCharaInfoRO(7);
            break;

        #else
        //电池堆总电压
        case MB_BPU_DAT_ADDR_START:
            temp = SciGetPtrGroupMainInfoRO(0);
            break;

        //电池堆输出电流
        case MB_BPU_DAT_ADDR_START + 1:
            temp = SciGetPtrGroupMainInfoRO(1);
            break;

        //电池堆输出功率
        case MB_BPU_DAT_ADDR_START + 2:
            temp = SciGetPtrGroupMainInfoRO(2);
            break;

        //电池堆外端预充电压
        case MB_BPU_DAT_ADDR_START + 3:
            temp = SciGetPtrGroupMainInfoRO(3);
            break;

        //电池堆整体SOC
        case MB_BPU_DAT_ADDR_START + 4:
            temp = SciGetPtrGroupCapInfoRO(0);
            break;

        //电池堆整体SOE
        case MB_BPU_DAT_ADDR_START + 5:
            temp = SciGetPtrGroupCapInfoRO(1);
            break;

        //电池堆电池SOH
        case MB_BPU_DAT_ADDR_START + 6:
            temp = SciGetPtrGroupCapInfoRO(2);
            break;

        //电池堆电池总容量
        case MB_BPU_DAT_ADDR_START + 7:
            temp = SciGetPtrGroupCapInfoRO(3);
            break;

        //电池堆单体最大电压电池组号
        case MB_BPU_DAT_ADDR_START + 8:
            temp = SciGetPtrDefaultValueRO();
            break;

        //电池堆单体最大电压序号
        case MB_BPU_DAT_ADDR_START + 9:
            temp = SciGetPtrGroupCharaInfoRO(0);
            break;

        //电池组单体最大电压值
        case MB_BPU_DAT_ADDR_START + 10:
            temp = SciGetPtrGroupCharaInfoRO(1);
            break;

        //电池堆单体累加总压平均值
        case MB_BPU_DAT_ADDR_START + 11:
            temp = SciGetPtrGroupCellInfoRO(0);
            break;

        //电池堆单体最小电压电池组号
        case MB_BPU_DAT_ADDR_START + 12:
            temp = SciGetPtrDefaultValueRO();
            break;

        //电池堆单体最小电压序号
        case MB_BPU_DAT_ADDR_START + 13:
            temp = SciGetPtrGroupCharaInfoRO(2);
            break;

        //电池堆单体最小电压值
        case MB_BPU_DAT_ADDR_START + 14:
            temp = SciGetPtrGroupCharaInfoRO(3);
            break;

        //电池堆单体平均电压值
        case MB_BPU_DAT_ADDR_START + 15:
            temp = SciGetPtrGroupCellInfoRO(1);
            break;

        //电池堆单体最高温度电池组号
        case MB_BPU_DAT_ADDR_START + 16:
            temp = SciGetPtrDefaultValueRO();
            break;

        //电池堆单体最高温度序号
        case MB_BPU_DAT_ADDR_START + 17:
            temp = SciGetPtrGroupCharaInfoRO(4);
            break;

        //电池堆单体最高温度值
        case MB_BPU_DAT_ADDR_START + 18:
            temp = SciGetPtrGroupCharaInfoRO(5);
            break;

        //电池堆单体平均温度值
        case MB_BPU_DAT_ADDR_START + 19:
            temp = SciGetPtrGroupCellInfoRO(2);
            break;

        //电池堆单体最低温度电池组号
        case MB_BPU_DAT_ADDR_START + 20:
            temp = SciGetPtrDefaultValueRO();
            break;

        //电池堆单体最低温度序号
        case MB_BPU_DAT_ADDR_START + 21:
            temp = SciGetPtrGroupCharaInfoRO(6);
            break;

        //电池堆单体最低温度值
        case MB_BPU_DAT_ADDR_START + 22:
            temp = SciGetPtrGroupCharaInfoRO(7);
            break;

        //电池堆环境温度值
        case MB_BPU_DAT_ADDR_START + 23:
            temp = SciGetPtrGroupCellInfoRO(3);
            break;

        //电池堆最大总压电池组号
        case MB_BPU_DAT_ADDR_START + 24:
            temp = SciGetPtrDefaultValueRO();
            break;

        //电池堆最大总压电池组总压值
        case MB_BPU_DAT_ADDR_START + 25:
            temp = SciGetPtrGroupMainInfoRO(0);
            break;

        //电池堆最小总压电池组号
        case MB_BPU_DAT_ADDR_START + 26:
            temp = SciGetPtrDefaultValueRO();
            break;

        //电池堆最小总压电池组总压值
        case MB_BPU_DAT_ADDR_START + 27:
            temp = SciGetPtrGroupMainInfoRO(0);
            break;

        //电池堆最大电流电池组号
        case MB_BPU_DAT_ADDR_START + 28:
            temp = SciGetPtrDefaultValueRO();
            break;

        //电池堆最大电流电池组电流值
        case MB_BPU_DAT_ADDR_START + 29:
            temp = SciGetPtrGroupMainInfoRO(1);
            break;

        //电池堆最小电流电池组号
        case MB_BPU_DAT_ADDR_START + 30:
            temp = SciGetPtrDefaultValueRO();
            break;

        //电池堆最小电流电池组电流值
        case MB_BPU_DAT_ADDR_START + 31:
            temp = SciGetPtrGroupMainInfoRO(1);
            break;
        #endif

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileEnerROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控电能量信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileEnerROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    switch(address)
    {
        #if(1 == PILE_MANAG_EN)
        //电池堆运行时长
        case MB_BPU_ENE_ADDR_START:
            temp = SciGetPtrPileStatistInfoRO(0);
            break;

        //电池堆可充电时间
        case MB_BPU_ENE_ADDR_START + 1:
            temp = SciGetPtrPileStatistInfoRO(1);
            break;

        //电池堆可放电时间
        case MB_BPU_ENE_ADDR_START + 2:
            temp = SciGetPtrPileStatistInfoRO(2);
            break;

        //电池堆充放电次数
        case MB_BPU_ENE_ADDR_START + 3:
            temp = SciGetPtrPileStatistInfoRO(3);
            break;

        //电池堆单月告警数目
        case MB_BPU_ENE_ADDR_START + 4:
            temp = SciGetPtrPileStatistInfoRO(4);
            break;

        //电池堆单月故障数目
        case MB_BPU_ENE_ADDR_START + 5:
            temp = SciGetPtrPileStatistInfoRO(5);
            break;

        //电池堆累计告警数目
        case MB_BPU_ENE_ADDR_START + 6:
            temp = SciGetPtrPileStatistInfoRO(6);
            break;

        //电池堆累计故障数目
        case MB_BPU_ENE_ADDR_START + 7:
            temp = SciGetPtrPileStatistInfoRO(7);
            break;

        //电池堆剩余电能
        case MB_BPU_ENE_ADDR_START + 8:
            temp = SciGetPtrPileStatistInfoRO(8);
            break;

        //电池堆总能量
        case MB_BPU_ENE_ADDR_START + 9:
            temp = SciGetPtrPileStatistInfoRO(9);
            break;

        //电池堆单次充电电量
        case MB_BPU_ENE_ADDR_START + 10:
            temp = SciGetPtrPileStatistInfoRO(10);
            break;

        //电池堆单次放电电量高位
        case MB_BPU_ENE_ADDR_START + 11:
            temp = SciGetPtrPileStatistInfoRO(11);
            break;

        //电池堆历史充电电量高位
        case MB_BPU_ENE_ADDR_START + 12:
            temp = SciGetPtrPileHisCapEnerInfoRO(sciNum, 0);
            break;

        //电池堆历史充电电量低位
        case MB_BPU_ENE_ADDR_START + 13:
            temp = SciGetPtrPileHisCapEnerInfoRO(sciNum, 1);
            break;

        //电池堆历史放电电量高位
        case MB_BPU_ENE_ADDR_START + 14:
            temp = SciGetPtrPileHisCapEnerInfoRO(sciNum, 2);
            break;

        //电池堆历史放电电量低位
        case MB_BPU_ENE_ADDR_START + 15:
            temp = SciGetPtrPileHisCapEnerInfoRO(sciNum, 3);
            break;

        //电池堆历史充电电能高位
        case MB_BPU_ENE_ADDR_START + 16:
            temp = SciGetPtrPileHisCapEnerInfoRO(sciNum, 4);
            break;

        //电池堆历史充电电能低位
        case MB_BPU_ENE_ADDR_START + 17:
            temp = SciGetPtrPileHisCapEnerInfoRO(sciNum, 5);
            break;

        //电池堆历史放电电能高位
        case MB_BPU_ENE_ADDR_START + 18:
            temp = SciGetPtrPileHisCapEnerInfoRO(sciNum, 6);
            break;

        //电池堆历史放电电能低位
        case MB_BPU_ENE_ADDR_START + 19:
            temp = SciGetPtrPileHisCapEnerInfoRO(sciNum, 7);
            break;

        #else
        //电池堆运行时长
        case MB_BPU_ENE_ADDR_START:
            temp = SciGetPtrGroupStatistInfoRO(0);
            break;

        //电池堆可充电时间
        case MB_BPU_ENE_ADDR_START + 1:
            temp = SciGetPtrGroupStatistInfoRO(1);
            break;

        //电池堆可放电时间
        case MB_BPU_ENE_ADDR_START + 2:
            temp = SciGetPtrGroupStatistInfoRO(2);
            break;

        //电池堆充放电次数
        case MB_BPU_ENE_ADDR_START + 3:
            temp = SciGetPtrGroupStatistInfoRO(3);
            break;

        //电池堆单月告警数目
        case MB_BPU_ENE_ADDR_START + 4:
            temp = SciGetPtrGroupStatistInfoRO(4);
            break;

        //电池堆单月故障数目
        case MB_BPU_ENE_ADDR_START + 5:
            temp = SciGetPtrGroupStatistInfoRO(5);
            break;

        //电池堆累计告警数目
        case MB_BPU_ENE_ADDR_START + 6:
            temp = SciGetPtrGroupStatistInfoRO(6);
            break;

        //电池堆累计故障数目
        case MB_BPU_ENE_ADDR_START + 7:
            temp = SciGetPtrGroupStatistInfoRO(7);
            break;

        //电池堆剩余电能
        case MB_BPU_ENE_ADDR_START + 8:
            temp = SciGetPtrGroupStatistInfoRO(8);
            break;

        //电池堆总能量
        case MB_BPU_ENE_ADDR_START + 9:
            temp = SciGetPtrGroupStatistInfoRO(9);
            break;

        //电池堆单次充电电量
        case MB_BPU_ENE_ADDR_START + 10:
            temp = SciGetPtrGroupStatistInfoRO(10);
            break;

        //电池堆单次放电电量高位
        case MB_BPU_ENE_ADDR_START + 11:
            temp = SciGetPtrGroupStatistInfoRO(11);
            break;

        //电池堆历史充电电量高位
        case MB_BPU_ENE_ADDR_START + 12:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 0);
            break;

        //电池堆历史充电电量低位
        case MB_BPU_ENE_ADDR_START + 13:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 1);
            break;

        //电池堆历史放电电量高位
        case MB_BPU_ENE_ADDR_START + 14:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 2);
            break;

        //电池堆历史放电电量低位
        case MB_BPU_ENE_ADDR_START + 15:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 3);
            break;

        //电池堆历史充电电能高位
        case MB_BPU_ENE_ADDR_START + 16:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 4);
            break;

        //电池堆历史充电电能低位
        case MB_BPU_ENE_ADDR_START + 17:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 5);
            break;

        //电池堆历史放电电能高位
        case MB_BPU_ENE_ADDR_START + 18:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 6);
            break;

        //电池堆历史放电电能低位
        case MB_BPU_ENE_ADDR_START + 19:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 7);
            break;
        #endif

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileHisErrROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控历史故障记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileHisErrROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    if(address >= MB_BPU_HIER_ADDR_START)
    {
        index = (u8)(address - MB_BPU_HIER_ADDR_START);

        #if(1 == PILE_MANAG_EN)
        temp = SciGetPtrPileHisErrRO(index);
        #else
        temp = SciGetPtrGroupHisErrRO(index);
        #endif
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindPileHisEvnROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读堆控历史事件记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindPileHisEvnROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口任意模式
    (void)sciNum;

    if(address >= MB_BPU_HIEV_ADDR_START)
    {
        index = (u8)(address - MB_BPU_HIEV_ADDR_START);

        #if(1 == PILE_MANAG_EN)
        temp = SciGetPtrPileHisEventRO(index);
        #else
        temp = SciGetPtrGroupHisEventRO(index);
        #endif
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupStateInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupStateInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    switch(address)
    {
        //电池组电池保护状态
        case MB_BMU_STA_ADDR_START:
            temp = SciGetPtrGroupStateInfoRO(0);
            break;

        //电池组工作状态
        case MB_BMU_STA_ADDR_START + 1:
            temp = SciGetPtrGroupStateInfoRO(1);
            break;

        //电池组充放电状态
        case MB_BMU_STA_ADDR_START + 2:
            temp = SciGetPtrGroupStateInfoRO(2);
            break;

        //电池组工作模式
        case MB_BMU_STA_ADDR_START + 3:
            temp = SciGetPtrGroupStateInfoRO(3);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupLimInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控限值信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupLimInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    switch(address)
    {
        //电池组充电电流限值
        case MB_BMU_LIM_ADDR_START:
            temp = SciGetPtrGroupLimitInfoRO(0);
            break;

        //电池组放电电流限值
        case MB_BMU_LIM_ADDR_START + 1:
            temp = SciGetPtrGroupLimitInfoRO(1);
            break;

        //电池组充电功率限值
        case MB_BMU_LIM_ADDR_START + 2:
            temp = SciGetPtrGroupLimitInfoRO(2);
            break;

        //电池组放电功率限值
        case MB_BMU_LIM_ADDR_START + 3:
            temp = SciGetPtrGroupLimitInfoRO(3);
            break;

        //电池组充电电压限值
        case MB_BMU_LIM_ADDR_START + 4:
            temp = SciGetPtrGroupLimitInfoRO(4);
            break;

        //电池组放电电压限值
        case MB_BMU_LIM_ADDR_START + 5:
            temp = SciGetPtrGroupLimitInfoRO(5);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupErrInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控故障告警信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupErrInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16 index = 0;
    u16* temp = (void*)0;

    if(3 != fncCode)                    //寄存器只读
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    switch(address)
    {
        //电池组告警信息
        case MB_BMU_ERR_ADDR_START:
        case MB_BMU_ERR_ADDR_START + 1:
        case MB_BMU_ERR_ADDR_START + 2:
        //电池组故障信息
        case MB_BMU_ERR_ADDR_START + 3:
        case MB_BMU_ERR_ADDR_START + 4:
        case MB_BMU_ERR_ADDR_START + 5:
        //电池组故障定位
        case MB_BMU_ERR_ADDR_START + 6:
        case MB_BMU_ERR_ADDR_START + 7:
            index = address - MB_BMU_ERR_ADDR_START;
            temp = SciGetPtrGroupErrInfoRO((u8)index);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupIoStateROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控输入输出口状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupIoStateROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16* temp = (void*)0;

    if(3 != fncCode)                    //寄存器只读
    {
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    switch(address)
    {
        //主控外部输入口状态信息
        case MB_BMU_IO_ADDR_START:
            temp = SciGetPtrGroupIoStateInfoRO(0);
            break;

        //主控外部输出口状态信息
        case MB_BMU_IO_ADDR_START + 1:
            temp = SciGetPtrGroupIoStateInfoRO(1);
            break;

        //主控内部输入口状态信息
        case MB_BMU_IO_ADDR_START + 2:
            temp = SciGetPtrGroupIoStateInfoRO(2);
            break;

        //主控内部输出口状态信息
        case MB_BMU_IO_ADDR_START + 3:
            temp = SciGetPtrGroupIoStateInfoRO(3);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupDataROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控数据量信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupDataROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    switch(address)
    {
        //电池组采样总电压
        case MB_BMU_DAT_ADDR_START:
            temp = SciGetPtrGroupMainInfoRO(0);
            break;

        //电池组输出电流
        case MB_BMU_DAT_ADDR_START + 1:
            temp = SciGetPtrGroupMainInfoRO(1);
            break;

        //电池组输出功率
        case MB_BMU_DAT_ADDR_START + 2:
            temp = SciGetPtrGroupMainInfoRO(2);
            break;

        //电池组预充电压
        case MB_BMU_DAT_ADDR_START + 3:
            temp = SciGetPtrGroupMainInfoRO(3);
            break;

        //电池组电池SOC
        case MB_BMU_DAT_ADDR_START + 4:
            temp = SciGetPtrGroupCapInfoRO(0);
            break;

        //电池组电池SOE
        case MB_BMU_DAT_ADDR_START + 5:
            temp = SciGetPtrGroupCapInfoRO(1);
            break;

        //电池组电池SOH
        case MB_BMU_DAT_ADDR_START + 6:
            temp = SciGetPtrGroupCapInfoRO(2);
            break;

        //电池组电池总容量
        case MB_BMU_DAT_ADDR_START + 7:
            temp = SciGetPtrGroupCapInfoRO(3);
            break;

        //电池组累计总电压值
        case MB_BMU_DAT_ADDR_START + 8:
            temp = SciGetPtrGroupCellInfoRO(0);
            break;

         //电池组单体平均电压值
        case MB_BMU_DAT_ADDR_START + 9:
            temp = SciGetPtrGroupCellInfoRO(1);
            break;

        //电池组单体平均温度值
        case MB_BMU_DAT_ADDR_START + 10:
            temp = SciGetPtrGroupCellInfoRO(2);
            break;

        //电池组环境温度
        case MB_BMU_DAT_ADDR_START + 11:
            temp = SciGetPtrGroupCellInfoRO(3);
            break;

        //电池组单体最大电压序号
        case MB_BMU_DAT_ADDR_START + 12:
            temp = SciGetPtrGroupCharaInfoRO(0);
            break;

        //电池组单体最大电压值
        case MB_BMU_DAT_ADDR_START + 13:
            temp = SciGetPtrGroupCharaInfoRO(1);
            break;

        //电池组单体最小电压序号
        case MB_BMU_DAT_ADDR_START + 14:
            temp = SciGetPtrGroupCharaInfoRO(2);
            break;

        //电池组单体最小电压值
        case MB_BMU_DAT_ADDR_START + 15:
            temp = SciGetPtrGroupCharaInfoRO(3);
            break;

        //电池组单体最大温度序号
        case MB_BMU_DAT_ADDR_START + 16:
            temp = SciGetPtrGroupCharaInfoRO(4);
            break;

        //组单体最大温度值
        case MB_BMU_DAT_ADDR_START + 17:
            temp = SciGetPtrGroupCharaInfoRO(5);
            break;

        //电池组单体最小温度序号
        case MB_BMU_DAT_ADDR_START + 18:
            temp = SciGetPtrGroupCharaInfoRO(6);
            break;

        //电池组单体最小温度值
        case MB_BMU_DAT_ADDR_START + 19:
            temp = SciGetPtrGroupCharaInfoRO(7);
            break;

        //电池组霍尔电流值
        case MB_BMU_DAT_ADDR_START + 20:
            temp = SciGetPtrGroupHigLowInfoRO(0);
            break;

        //电池组分流器电流值
        case MB_BMU_DAT_ADDR_START + 21:
            temp = SciGetPtrGroupHigLowInfoRO(1);
            break;

        //电池组分流器温度值
        case MB_BMU_DAT_ADDR_START + 22:
            temp = SciGetPtrGroupHigLowInfoRO(2);
            break;

        //电池组电源温度值
        case MB_BMU_DAT_ADDR_START + 23:
            temp = SciGetPtrGroupHigLowInfoRO(3);
            break;

        //电池组负极电压采样值
        case MB_BMU_DAT_ADDR_START + 24:
            temp = SciGetPtrGroupHigLowInfoRO(4);
            break;

        //电池组低压电源电压采样值
        case MB_BMU_DAT_ADDR_START + 25:
            temp = SciGetPtrGroupHigLowInfoRO(5);
            break;

        //电池组激活信号ACC电压采样值
        case MB_BMU_DAT_ADDR_START + 26:
            temp = SciGetPtrGroupHigLowInfoRO(6);
            break;

        //电池组霍尔电源电压采样值
        case MB_BMU_DAT_ADDR_START + 27:
            temp = SciGetPtrGroupHigLowInfoRO(7);
            break;

        //电池组充电枪KC供入电压采样值
        case MB_BMU_DAT_ADDR_START + 28:
            temp = SciGetPtrGroupHigLowInfoRO(8);
            break;

        //电池组充电枪CC2信号电压采样值
        case MB_BMU_DAT_ADDR_START + 29:
            temp = SciGetPtrGroupHigLowInfoRO(9);
            break;

        //电池组正极对地绝缘阻值
        case MB_BMU_DAT_ADDR_START + 30:
            temp = SciGetPtrGroupHigLowInfoRO(10);
            break;

        //电池组负极对地绝缘阻值
        case MB_BMU_DAT_ADDR_START + 31:
            temp = SciGetPtrGroupHigLowInfoRO(11);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupEnerROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控电能量信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupEnerROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    //不区分组号
    (void)group;

    switch(address)
    {
        //电池组运行时长
        case MB_BMU_ENE_ADDR_START:
            temp = SciGetPtrGroupStatistInfoRO(0);
            break;

        //电池组可充电时间
        case MB_BMU_ENE_ADDR_START + 1:
            temp = SciGetPtrGroupStatistInfoRO(1);
            break;

        //电池组可放电时间
        case MB_BMU_ENE_ADDR_START + 2:
            temp = SciGetPtrGroupStatistInfoRO(2);
            break;

        //电池组充放电次数
        case MB_BMU_ENE_ADDR_START + 3:
            temp = SciGetPtrGroupStatistInfoRO(3);
            break;

        //电池组当月告警数目
        case MB_BMU_ENE_ADDR_START + 4:
            temp = SciGetPtrGroupStatistInfoRO(4);
            break;

        //电池组当月故障数目
        case MB_BMU_ENE_ADDR_START + 5:
            temp = SciGetPtrGroupStatistInfoRO(5);
            break;

        //电池组累计告警数目
        case MB_BMU_ENE_ADDR_START + 6:
            temp = SciGetPtrGroupStatistInfoRO(6);
            break;

        //电池组累计故障数目
        case MB_BMU_ENE_ADDR_START + 7:
            temp = SciGetPtrGroupStatistInfoRO(7);
            break;

        //电池组剩余电能
        case MB_BMU_ENE_ADDR_START + 8:
            temp = SciGetPtrGroupStatistInfoRO(8);
            break;

        //电池组总能量
        case MB_BMU_ENE_ADDR_START + 9:
            temp = SciGetPtrGroupStatistInfoRO(9);
            break;

        //电池组单次充电电量
        case MB_BMU_ENE_ADDR_START + 10:
            temp = SciGetPtrGroupStatistInfoRO(10);
            break;

        //电池组单次放电电量
        case MB_BMU_ENE_ADDR_START + 11:
            temp = SciGetPtrGroupStatistInfoRO(11);
            break;

        //电池组历史充电电量高位
        case MB_BMU_ENE_ADDR_START + 12:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 0);
            break;

        //电池组历史充电电量低位
        case MB_BMU_ENE_ADDR_START + 13:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 1);
            break;

        //电池组历史放电电量高位
        case MB_BMU_ENE_ADDR_START + 14:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 2);
            break;

        //电池组历史放电电量低位
        case MB_BMU_ENE_ADDR_START + 15:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 3);
            break;

        //电池组历史充电电能高位
        case MB_BMU_ENE_ADDR_START + 16:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 4);
            break;

        //电池组历史充电电能低位
        case MB_BMU_ENE_ADDR_START + 17:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 5);
            break;

        //电池组历史放电电能高位
        case MB_BMU_ENE_ADDR_START + 18:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 6);
            break;

        //电池组历史放电电能低位
        case MB_BMU_ENE_ADDR_START + 19:
            temp = SciGetPtrGroupHisCapEnerInfoRO(sciNum, 7);
            break;

        //预留地址
        default:
            temp = SciGetPtrDefaultValueRO();
            break;
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupHisErrROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控历史故障记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupHisErrROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    if(address >= MB_BPU_HIER_ADDR_START)
    {
        index = (u8)(address - MB_BPU_HIER_ADDR_START);

        temp = SciGetPtrGroupHisErrRO(index);
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupHisEvnROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控历史事件记录信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupHisEvnROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u8 index = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    if(address >= MB_BPU_HIEV_ADDR_START)
    {
        index = (u8)(address - MB_BPU_HIEV_ADDR_START);

        temp = SciGetPtrGroupHisEventRO(index);
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindGroupIntInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控内部从控信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindGroupIntInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    if(address >= MB_BMU_INT_ADDR_START)
    {
        temp = SciGetPtrDefaultValueRO();
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindCellChgBalanROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控单体均衡状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindCellChgBalanROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16 num = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //不区分组号
    (void)group;

    if(address >= MB_BMU_CELCB_ADDR_START)
    {
        num = address - MB_BMU_CELCB_ADDR_START;
        temp = SciGetPtrGeroupCellChgBalanRO(num);
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindCellDhgBalanROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控单体均放状态信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindCellDhgBalanROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16 num = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //不区分组号
    (void)group;

    if(address >= MB_BMU_CELDB_ADDR_START)
    {
        num = address - MB_BMU_CELDB_ADDR_START;
        temp = SciGetPtrGeroupCellDhgBalanRO(num);
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindCellVoltROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控单体电压值信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindCellVoltROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16 num = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //任意端口
    (void)sciNum;
    //不区分组号
    (void)group;

    if(address >= MB_BMU_CELV_ADDR_START)
    {
        num = address - MB_BMU_CELV_ADDR_START;
        temp = SciGetPtrGeroupCellVoltStateRO(num);
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindCellTempROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址 group:组号[0,15]
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找读主控单体温度值信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindCellTempROAddrMap(u8 fncCode, u8 sciNum, u16 address, u8 group)
{
    u16 num = 0;
    u16* temp = (void*)0;

    //可读
    if(3 != fncCode)
    {
        return(temp);
    }

    //不区分组号
    (void)group;

    if(address >= MB_BMU_CELT_ADDR_START)
    {
        num = address - MB_BMU_CELT_ADDR_START;
        temp = SciGetPtrGeroupCellTempInfoRO(sciNum, num);
    }
    else
    {
        temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
