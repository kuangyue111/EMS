/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : SlaveMsg.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 从控CAN报文处理模块
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
#include "CellAFESamp.h"
#include "DiagInterface.h"
#include "GTBmmMsg.h"

//使能兼容高特从控BMM通信信息
#if((1 == BMU_BSU_COM_TYPE) && (1 == BMU_BSU_CAN_TYPE))
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gGBmuHigLevPara_103[]; 			/*主控高级参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void SendCanGTBmmReadDataMsg10(u8 slave, u8 type, u16 avgVolt, s16 curr, s8 onTemp, s8 offTemp)
** @Input     : slave:从控序号[0,31] type:读取数据类型[0x01电压；0x02温度；0x03电流；0x04均衡状态；0x05掉线状态；0x08 概要信息;0xFF所有数据] avgVolt:组内平均电压(均衡用)[1mV] curr:组电流[0.1A] onTemp:开启风扇温度[1℃]
** @Output    : void
** @Function  : 发送请求高特从控BMM信息CAN报文
** @The notes : 250ms周期
**===============================================================================================*/
static void SendCanGTBmmReadDataMsg10(u8 slave, u8 type, u16 avgVolt, s16 curr, s8 onTemp, s8 offTemp);

/*=================================================================================================
** @Name      : static void RcvGTSlaveCellVoltInfo11(u8 slave, u8 *pdata, u8 len)
** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收GT从控BMM单体电压信息
** @The notes : 一个从控对应一个电池Pack
**===============================================================================================*/
static void RcvGTSlaveCellVoltInfo11(u8 slave, u8 *pdata, u8 len);

/*=================================================================================================
 ** @Name      : static void RcvGTSlaveCellTempInfo12(u8 slave, u8 *pdata, u8 len)
 ** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收GT从控BMM单体温度信息
 ** @The notes : 一个从控对应一个电池Pack
 **===============================================================================================*/
 static void RcvGTSlaveCellTempInfo12(u8 slave, u8 *pdata, u8 len);

 /*=================================================================================================
 ** @Name      : static void RcvGTSlaveBalanStateInfo15(u8 slave, u8 *pdata, u8 len)
 ** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收GT从控BMM单体均衡状态信息
 ** @The notes : 一个从控对应一个电池Pack
 **===============================================================================================*/
 static void RcvGTSlaveBalanStateInfo15(u8 slave, u8 *pdata, u8 len);

 /*=================================================================================================
 ** @Name      : static void RcvGTSlaveLineOffInfo16(u8 slave, u8 *pdata, u8 len)
 ** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收GT从控BMM单体电压采样掉线状态信息
 ** @The notes : 一个从控对应一个电池Pack
 **===============================================================================================*/
 static void RcvGTSlaveLineOffInfo16(u8 slave, u8 *pdata, u8 len);

 /*=================================================================================================
 ** @Name      : static void RcvGTSlaveMainInfo19(u8 slave, u8 *pdata, u8 len)
 ** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收GT从控BMM模块概要信息
 ** @The notes : 一个从控对应一个电池Pack
 **===============================================================================================*/
 static void RcvGTSlaveMainInfo19(u8 slave, u8 *pdata, u8 len);

 /*=================================================================================================
 ** @Name      : static void RcvGTSlaveDoStateInfo33(u8 slave, u8 *pdata, u8 len)
 ** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收GT从控BMM输出干接点DO状态信息
 ** @The notes : 一个从控对应一个电池Pack
 **===============================================================================================*/
 static void RcvGTSlaveDoStateInfo33(u8 slave, u8 *pdata, u8 len);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupSendDnGTBmmMsg_API(void)
** @Input     : void
** @Output    : void
** @Function  : 主控下发高特从控BMM协议CAN通信报文任务
** @The notes : 用户10ms周期接口调用
**===============================================================================================*/
void GroupSendDnGTBmmMsg_API(void)
{
	static u8 sSlave = 0;

	/*逐个向从控发送*/
	SendCanGTBmmReadDataMsg10(sSlave, 0xff, GetGCellAvgVoltAPI(), GetGSampOutCurrAPI(), (s8)gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp], (s8)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]);

	sSlave++;
	if(sSlave >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
	{
		sSlave = 0;
	}

	//上报主从通信中断或上电10s(不检通信断线)
	if((eErr_B == DiagGetErrLastStateAPI(eERR_ID_INT_COFF)) || ((BSPGetOSMillSecTimeAPI() >= 10000) && (0 == DiagGetFilErrSortAPI(eERR_ID_INT_COFF))))
	{
		//记录单体电压和温度采样完成一遍
		WriteCellVSampFinFlagHook(1);
		WriteCellTSampFinFlagHook(1);
	}
}

/*=================================================================================================
** @Name      : void CanRcvGTSlaveMsgHandleAPI(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 兼容GT电池从控协议CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvGTSlaveMsgHandleAPI(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 rcvDev = 0;
	u8 aimDev = 0;
	u8 rcvCmd = 0;
	u16 rcvpro = 0;

	rcvDev = ID & 0xff;
	aimDev = (ID >> 8) & 0xff;
	rcvCmd = (ID >> 16) & 0xff;
	rcvpro = (ID >> 24) & 0xffff;

	/*只接收从控报文*/
	if((eCAN1 != canNum) || (DLC < 2)
		|| (0xf5 != aimDev) ||  (rcvDev < 0x80) || (rcvDev > 0xaf)
		|| (rcvpro != 0x18) ||  (rcvCmd < 0x11)  || (rcvCmd > 0x33))
	{
		return;
	}

	/*记录能接收到报文通信正常*/
	DiagRcvDevMsgAddAPI(rcvDev - 0x80);

	/*BMM单体电压信息报文*/
	if(0x11 == rcvCmd)
	{
		RcvGTSlaveCellVoltInfo11((rcvDev - 0x80), pdata, DLC);
	}
	/*BMM单体温度信息报文*/
	else if(0x12 == rcvCmd)
	{
		RcvGTSlaveCellTempInfo12((rcvDev - 0x80), pdata, DLC);
	}
	/*BMM均衡状态信息报文*/
	else if(0x15 == rcvCmd)
	{
		RcvGTSlaveBalanStateInfo15((rcvDev - 0x80), pdata, DLC);
	}
	/*BMM采集掉线状态信息报文*/
	else if(0x16 == rcvCmd)
	{
		RcvGTSlaveLineOffInfo16((rcvDev - 0x80), pdata, DLC);
	}
	/*BMM模块概要信息报文*/
	else if(0x19 == rcvCmd)
	{
		RcvGTSlaveMainInfo19((rcvDev - 0x80), pdata, DLC);
	}
	/*BMM模块概要信息报文*/
	else if(0x33 == rcvCmd)
	{
		RcvGTSlaveDoStateInfo33((rcvDev - 0x80), pdata, DLC);
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void SendCanGTBmmReadDataMsg10(u8 slave, u8 type, u16 avgVolt, s16 curr, s8 onTemp, s8 offTemp)
** @Input     : slave:从控序号[0,31] type:读取数据类型[0x01电压；0x02温度；0x03电流；0x04均衡状态；0x05掉线状态；0x08 概要信息;0xFF所有数据] avgVolt:组内平均电压(均衡用)[1mV] curr:组电流[0.1A] onTemp:开启风扇温度[1℃]
** @Output    : void
** @Function  : 发送请求高特从控BMM信息CAN报文
** @The notes : 250ms周期
**===============================================================================================*/
static void SendCanGTBmmReadDataMsg10(u8 slave, u8 type, u16 avgVolt, s16 curr, s8 onTemp, s8 offTemp)
{
    t_CANMSG msg;
    u16 data = 0;

    //从控序号无效
    if(slave >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
    {
    	return;
    }

    /*请求帧ID*/
    msg.ID = 0x181000f5 | (((u16)(0x80 + slave)) << 8);

    /*报文长度*/
    msg.Len = 8;

    /*请求帧数据域(高位在前)*/
    msg.Data[0] = type;
	msg.Data[1] = 0;
	msg.Data[2] = (avgVolt >> 8) & 0xff;
	msg.Data[3] = avgVolt & 0xff;

	if(curr < -16000)
	{
		data = 0;
	}
	else if(curr > 16000)
	{
		data = 32000;
	}
	else
	{
		data = curr + 16000;
	}

	msg.Data[4] = (data >> 8) & 0xff;
	msg.Data[5] = data & 0xff;

	if(onTemp < -40)
	{
		data = 0;
	}
	else
	{
		data = onTemp + 40;
	}
	msg.Data[6] = data & 0xff;


	if(offTemp < -40)
	{
		data = 0;
	}
	else
	{
		data = offTemp + 40;
	}
	msg.Data[7] = data & 0xff;

    /*向CAN1通道发送*/
    (void)BSPCANSendQin(eCAN1, &msg);
}

/*=================================================================================================
** @Name      : static void RcvGTSlaveCellVoltInfo11(u8 slave, u8 *pdata, u8 len)
** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收GT从控BMM单体电压信息
** @The notes : 一个从控对应一个电池Pack
**===============================================================================================*/
static void RcvGTSlaveCellVoltInfo11(u8 slave, u8 *pdata, u8 len)
{
	u8 i = 0;
	u8 cellNum = 0;
	u16 cellVolt = 0;
	static u8 rcvCellNum = 0;
	static u32 rcvCellBit = 0;

    /*序号正确*/
	if((slave < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (len >= 4)
		&& (pdata[0] > 0) && (pdata[0] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]))
	{
		//每帧1~3节(根据帧长度判断)
		for(i = 0; i < 3; i++)
		{
			//只解析有效长度
			if((3 + (i * 2)) < len)
			{
				cellNum = pdata[0] - 1 + i;

				//电压正常
				if(0 == BitGet(pdata[1], i))
				{
					//获取电压
					cellVolt = ((u16)pdata[2 + (i * 2)] << 8) | pdata[3 + (i * 2)];
				}
				//电压异常
				else
				{
					//电压清零
					cellVolt = 0;
				}

				/*采样电压值判断断线*/
				DiagCheckCellVOffLFlagAPI(slave, cellNum, cellVolt);

				/*输出采样电压值*/
				WriteSampCellVoltHook(slave, cellNum, cellVolt);
			}
			else
			{
				break;
			}
		}

		//首次接收到该从控信息
		if(0 == BitGet(rcvCellBit, slave))
		{
			//计数接收到从控数目
			rcvCellNum++;
			if(rcvCellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
			{
				//记录单体电压采样完成一遍
				WriteCellVSampFinFlagHook(1);
			}
		}
		//记录该从控已接收
		BitSet(rcvCellBit, slave);
	}
}

/*=================================================================================================
** @Name      : static void RcvGTSlaveCellTempInfo12(u8 slave, u8 *pdata, u8 len)
** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收GT从控BMM单体温度信息
** @The notes : 一个从控对应一个电池Pack
**===============================================================================================*/
static void RcvGTSlaveCellTempInfo12(u8 slave, u8 *pdata, u8 len)
{
	u8 i = 0;
	u8 cellNum = 0;
	s8 cellTemp = 0;
	static u8 rcvCellNum = 0;
	static u32 rcvCellBit = 0;

    /*序号正确*/
	if((slave < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (len >= 3)
		&& (pdata[0] > 0) && (pdata[0] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]))
	{
		//每帧1~6节(根据帧长度判断)
		for(i = 0; i < 6; i++)
		{
			//只解析有效长度
			if((2 + i) < len)
			{
				cellNum = pdata[0] - 1 + i;

				//温度正常
				if(0 == BitGet(pdata[1], i))
				{
					//获取温度
					cellTemp = (s8)((s16)pdata[2 + i] - 40);
				}
				//温度异常
				else
				{
					//温度清零
					cellTemp = -41;
				}

				/*采样温度值判断断线*/
				DiagCheckCellTOffLFlagAPI(slave, cellNum, cellTemp);

				/*输出采样温度值*/
				WriteSampCellTempHook(slave, cellNum, cellTemp);
			}
			else
			{
				break;
			}
		}

		//首次接收到该从控信息
		if(0 == BitGet(rcvCellBit, slave))
		{
			//计数接收到从控数目
			rcvCellNum++;
			if(rcvCellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
			{
				//记录单体温度采样完成一遍
				WriteCellTSampFinFlagHook(1);
			}
		}
		//记录该从控已接收
		BitSet(rcvCellBit, slave);
	}
}

/*=================================================================================================
** @Name      : static void RcvGTSlaveBalanStateInfo15(u8 slave, u8 *pdata, u8 len)
** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收GT从控BMM单体均衡状态信息
** @The notes : 一个从控对应一个电池Pack
**===============================================================================================*/
static void RcvGTSlaveBalanStateInfo15(u8 slave, u8 *pdata, u8 len)
{
	u16 state = 0;

    /*序号正确*/
	if((slave < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (len >= 4))
	{
		state = ((u16)pdata[2] << 8) | pdata[3];

		//被动均衡
		if(0 == pdata[0])
		{
			//输入放电均衡状态
			InputBsuDhgBalanState(slave, state);
		}
		//主动均衡
		else
		{
			//输入充电均衡状态
			InputBsuChgBalanState(slave, state);
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvGTSlaveLineOffInfo16(u8 slave, u8 *pdata, u8 len)
** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收GT从控BMM单体电压采样掉线状态信息
** @The notes : 一个从控对应一个电池Pack
**===============================================================================================*/
static void RcvGTSlaveLineOffInfo16(u8 slave, u8 *pdata, u8 len)
{
    /*序号正确*/
	//if((slave < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (len >= 5))
	{
		//输出单体电压采样断线状态(按位表示)
		;
	}
}

/*=================================================================================================
** @Name      : static void RcvGTSlaveMainInfo19(u8 slave, u8 *pdata, u8 len)
** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收GT从控BMM模块概要信息
** @The notes : 一个从控对应一个电池Pack
**===============================================================================================*/
static void RcvGTSlaveMainInfo19(u8 slave, u8 *pdata, u8 len)
{
    /*序号正确*/
	//if((slave < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (len >= 6))
	{
		//解析电池模组总压/温度/供电电压信息
		;
	}
}

/*=================================================================================================
** @Name      : static void RcvGTSlaveDoStateInfo33(u8 slave, u8 *pdata, u8 len)
** @Input     : slave:从控序号[0, 31] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收GT从控BMM输出干接点DO状态信息
** @The notes : 一个从控对应一个电池Pack
**===============================================================================================*/
static void RcvGTSlaveDoStateInfo33(u8 slave, u8 *pdata, u8 len)
{
    /*序号正确*/
	//if((slave < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (len >= 1))
	{
		//解析电池模组总压/温度/供电电压信息
		;
	}
}
#endif
