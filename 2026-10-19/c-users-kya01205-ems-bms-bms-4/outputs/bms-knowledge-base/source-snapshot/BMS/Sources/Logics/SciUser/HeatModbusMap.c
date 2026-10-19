/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : HeatModbusMap.c
** @Founder      : ZSYZ342
** @Date         : 2019.11.1
** @Function     : 热管理Modbus通信地址映射模块
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
#include "DiagCheck.h"
#include "GroupTactic.h"
#include "HeatModbusMap.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gGImluReadInfo_202[]; 	/*IMLU1接收信息*/

static u16 sDefaultValueRO = 0;		/*数据异常默认值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16 *ModbusFindSlaveInfoMaping(u8 fncCode, u8 sciNum, u16 address, u8 stationID)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找所有信息寄存器所对应的地址指针
** @The notes : 放在Modbus通讯包服务端(主机)接收处理HOOK
**===============================================================================================*/
u16 *ModbusFindSlaveInfoMaping(u8 fncCode, u8 sciNum, u16 address, u8 stationID)
{
    u16* temp = (void *)0;

	//接收空调信息

	//接收IMLU信息
    if(stationID == MODBUS_SALVE_ADDR_IMLU1)
    {
    	//通信检测
    	DiagRcvDevMsgAddAPI(IMLU_BAS_DEVADDR);

    	//获取接收从机信息寄存器地址
    	if((address >= MB_IMLU_RO_BASE_ADDR) && (address < MB_IMLU_RO_END_ADDR))
    	{
    		temp = ModbusFindFanAlarmInfoROAddrMap(fncCode, sciNum, address);
    	}
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindDeviceInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在设备信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindFanAlarmInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
	u8 index = 0;
    u16* temp = (void *)0;

    //只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }

    index = address - MB_IMLU_RO_BASE_ADDR;
    temp = SciGetFanCtrlAlarmInfoRO(index);

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetFanCtrlAlarmInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetFanCtrlAlarmInfoRO(u8 index)
{
	if(index < eIMLU1Info202_Len)
	{
		return(&gGImluReadInfo_202[index]);
	}

	return(&sDefaultValueRO);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
