/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CatlMsg.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 兼容CATL电池协议CAN报文模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "InsuCheck.h"
#include "TacInterface.h"
#include "EnerInterface.h"
#include "DiagInterface.h"
#include "DiagUser.h"
#include "TacUser.h"
#include "CANUser.h"
#include "CatlMsg.h"

#if(1 == BMU_CATL_CAN_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
/* SAE J1850 crc lookup table */
static const u8 Crc8Table[256] =
{
	0x00u, 0x1du, 0x3au, 0x27u, 0x74u, 0x69u, 0x4eu, 0x53u, 0xe8u, 0xf5u, 0xd2u, 0xcfu, 0x9cu, 0x81u, 0xa6u, 0xbbu,
	0xcdu, 0xd0u, 0xf7u, 0xeau, 0xb9u, 0xa4u, 0x83u, 0x9eu, 0x25u, 0x38u, 0x1fu, 0x02u, 0x51u, 0x4cu, 0x6bu, 0x76u,
	0x87u, 0x9au, 0xbdu, 0xa0u, 0xf3u, 0xeeu, 0xc9u, 0xd4u, 0x6fu, 0x72u, 0x55u, 0x48u, 0x1bu, 0x06u, 0x21u, 0x3cu,
	0x4au, 0x57u, 0x70u, 0x6du, 0x3eu, 0x23u, 0x04u, 0x19u, 0xa2u, 0xbfu, 0x98u, 0x85u, 0xd6u, 0xcbu, 0xecu, 0xf1u,
	0x13u, 0x0eu, 0x29u, 0x34u, 0x67u, 0x7au, 0x5du, 0x40u, 0xfbu, 0xe6u, 0xc1u, 0xdcu, 0x8fu, 0x92u, 0xb5u, 0xa8u,
	0xdeu, 0xc3u, 0xe4u, 0xf9u, 0xaau, 0xb7u, 0x90u, 0x8du, 0x36u, 0x2bu, 0x0cu, 0x11u, 0x42u, 0x5fu, 0x78u, 0x65u,
	0x94u, 0x89u, 0xaeu, 0xb3u, 0xe0u, 0xfdu, 0xdau, 0xc7u, 0x7cu, 0x61u, 0x46u, 0x5bu, 0x08u, 0x15u, 0x32u, 0x2fu,
	0x59u, 0x44u, 0x63u, 0x7eu, 0x2du, 0x30u, 0x17u, 0x0au, 0xb1u, 0xacu, 0x8bu, 0x96u, 0xc5u, 0xd8u, 0xffu, 0xe2u,
	0x26u, 0x3bu, 0x1cu, 0x01u, 0x52u, 0x4fu, 0x68u, 0x75u, 0xceu, 0xd3u, 0xf4u, 0xe9u, 0xbau, 0xa7u, 0x80u, 0x9du,
	0xebu, 0xf6u, 0xd1u, 0xccu, 0x9fu, 0x82u, 0xa5u, 0xb8u, 0x03u, 0x1eu, 0x39u, 0x24u, 0x77u, 0x6au, 0x4du, 0x50u,
	0xa1u, 0xbcu, 0x9bu, 0x86u, 0xd5u, 0xc8u, 0xefu, 0xf2u, 0x49u, 0x54u, 0x73u, 0x6eu, 0x3du, 0x20u, 0x07u, 0x1au,
	0x6cu, 0x71u, 0x56u, 0x4bu, 0x18u, 0x05u, 0x22u, 0x3fu, 0x84u, 0x99u, 0xbeu, 0xa3u, 0xf0u, 0xedu, 0xcau, 0xd7u,
	0x35u, 0x28u, 0x0fu, 0x12u, 0x41u, 0x5cu, 0x7bu, 0x66u, 0xddu, 0xc0u, 0xe7u, 0xfau, 0xa9u, 0xb4u, 0x93u, 0x8eu,
	0xf8u, 0xe5u, 0xc2u, 0xdfu, 0x8cu, 0x91u, 0xb6u, 0xabu, 0x10u, 0x0du, 0x2au, 0x37u, 0x64u, 0x79u, 0x5eu, 0x43u,
	0xb2u, 0xafu, 0x88u, 0x95u, 0xc6u, 0xdbu, 0xfcu, 0xe1u, 0x5au, 0x47u, 0x60u, 0x7du, 0x2eu, 0x33u, 0x14u, 0x09u,
	0x7fu, 0x62u, 0x45u, 0x58u, 0x0bu, 0x16u, 0x31u, 0x2cu, 0x97u, 0x8au, 0xadu, 0xb0u, 0xe3u, 0xfeu, 0xd9u, 0xc4u
};

extern t_SysClock gSysClock;        /*系统时钟(北京时间)*/
extern u16 gCANDevInfo_1[];         /*设备信息*/
extern u16 gSysGenParaRO_100[];		/*系统通用参数设置结果*/
extern u16 gGBmuHigLevPara_103[];   /*主控高级参数值*/
extern u8 gGHardPara_104[]; 		/*主控硬件参数配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void SendUpCatlBMSStatusInfo8f(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS状态信息
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSStatusInfo8f(void);

/*=================================================================================================
** @Name      : static void SendUpCatlBMSSumData1Info90(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS求和信息1
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSSumData1Info90(void);

/*=================================================================================================
** @Name      : static void SendUpCatlBMSSumData2Info91(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS求和信息2
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSSumData2Info91(void);

/*=================================================================================================
** @Name      : static void SendUpCatlBMSSumData3Info93(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS求和信息3
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSSumData3Info93(void);

/*=================================================================================================
** @Name      : static void SendUpCatlBMSLimDataInfo92(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS限值信息3
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSLimDataInfo92(void);

/*=================================================================================================
** @Name      : static void SendUpCatlBMSAlarmInfo0701(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS报警码信息
** @The notes : 100ms周期
**===============================================================================================*/
static void SendUpCatlBMSAlarmInfo0701(void);

/*=================================================================================================
** @Name      : static void SendUpCatlBMSCharaInfo0b03(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS特征信息1
** @The notes : 100ms周期
**===============================================================================================*/
static void SendUpCatlBMSCharaInfo0b03(void);

/*=================================================================================================
** @Name      : static void SendUpCatlBMSCharaInfo0b04(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS特征信息2
** @The notes : 100ms周期
**===============================================================================================*/
static void SendUpCatlBMSCharaInfo0b04(void);

/*=================================================================================================
** @Name      : static void RcvCatlPCSStatusCtrlCmd8f(t_CANMSG *msg)
** @Input     : msg:接收的报文
** @Output    : void
** @Function  : 接收PCS状态控制命令
** @The notes :
**===============================================================================================*/
static void RcvCatlPCSStatusCtrlCmd8f(t_CANMSG *msg);

/*=================================================================================================
** @Name      : static void RcvCatlPCSTimingRTCCmd92(t_CANMSG *msg)
** @Input     : msg:接收的报文
** @Output    : void
** @Function  : 接收PCS校时控制命令
** @The notes :
**===============================================================================================*/
static void RcvCatlPCSTimingRTCCmd92(t_CANMSG *msg);

/*=================================================================================================
** @Name      : static u8 Crc_ComputeCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length)
** @Input     : ApplicationId:帧ID低16位 Crc_DataPtr:需要计算校验码的数组 Crc_Length:数组长度
** @Output    : void
** @Function  : 按SAE J1850 方式计算CRC值
** @The notes :
**===============================================================================================*/
static u8 Crc_ComputeCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CanRcvCatlMsgHandleAPI(t_CANMSG *msg, u8 canNum)
** @Input     : msg:接收到的报文 canNum:接收CAN通道号
** @Output    : void
** @Function  : 兼容CATL电池协议CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvCatlMsgHandleAPI(t_CANMSG *msg, u8 canNum)
{
	u8 rcvDev = 0;
	u8 rcvCmd = 0;
	u16 rcvpro = 0;

	/*仅单组运行时有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		return;
	}

	rcvDev = msg->ID & 0xff;
	rcvCmd = (msg->ID >> 8) & 0xff;
	rcvpro = (msg->ID >> 16) & 0xffff;

	/*只接收PCS报文*/
	if((rcvDev < 0xB9) || (rcvDev > 0xC2) || (rcvpro != 0x040C))
	{
		return;
	}

	/*PCS状态命令报文*/
	if(0x8f == rcvCmd)
	{
		RcvCatlPCSStatusCtrlCmd8f(msg);
	}
	/*PCS校时报文*/
	else if(0x92 == rcvCmd)
	{
		RcvCatlPCSTimingRTCCmd92(msg);
	}
}

/*=================================================================================================
** @Name      : void CanSendUpCatlMsgCycleTask(void)
** @Input     : void
** @Output    : void
** @Function  : 兼容CATL电池协议CAN报文周期发送任务
** @The notes : 5ms调用
**===============================================================================================*/
void CanSendUpCatlMsgCycleTask(void)
{
	static u8 sTime = 0;
	static u8 sFlag100ms = 0;

	/*仅单组运行时有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		sFlag100ms = 0;
		sTime = 0;
		return;
	}

	/*工装测试程序调试模式不发送*/
	#if(1 == BMU_FROCK_CAN_EN)
	if((eMODE_DEBUG == GetGWorkModeAPI())
		&& (3 == gGHardPara_104[eGHardPara104_CSampChan]))
	{
		sFlag100ms = 0;
		sTime = 0;
		return;
	}
	#endif

	/*5ms驱动50ms周期*/
	switch(sTime)
	{
		case 0:
			SendUpCatlBMSStatusInfo8f();
			sTime = 1;
			break;

		case 1:
			sTime = 2;
			break;

		case 2:
			SendUpCatlBMSSumData1Info90();
			sTime = 3;
			break;

		case 3:
			sTime = 4;
			break;

		case 4:
			SendUpCatlBMSSumData2Info91();
			sTime = 5;
			break;

		case 5:
			if(0 == sFlag100ms)
			{
				SendUpCatlBMSAlarmInfo0701();
			}
			sTime = 6;
			break;

		case 6:
			SendUpCatlBMSSumData3Info93();
			sTime = 7;
			break;

		case 7:
			sTime = 8;
			break;

		case 8:
			SendUpCatlBMSLimDataInfo92();
			sTime = 9;
			break;

		case 9:
			if(0 == sFlag100ms)
			{
				SendUpCatlBMSCharaInfo0b03();
				sFlag100ms = 1;
			}
			else
			{
				SendUpCatlBMSCharaInfo0b04();
				sFlag100ms = 0;
			}
			sTime = 0;
			break;

		default:
			sTime = 0;
			break;
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void SendUpCatlBMSStatusInfo8f(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS状态信息
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSStatusInfo8f(void)
{
	u16 data = 0;
    t_CANMSG msg;
    static u8 sAlive = 0;

    msg.ID = 0x040E8F91 + (gCANDevInfo_1[eCANDev1_Addr] - BMU_BAS_DEVADDR);

    msg.Len = 8;

    /*生命帧*/
    msg.Data[1] = sAlive & 0x0f;

    /*生命帧计数*/
    sAlive++;
	if(sAlive >= 16)
	{
		sAlive = 0;
	}

    /*系统运行或故障状态运行接触器全闭合*/
    if((eWORK_RUN == GetGWorkStateAPI())
    	|| ((eWORK_ERR == GetGWorkStateAPI())
    	&& (TRUE == TacGetGroupRunSwhAllOnStateAPI())))
    {
        /*高压上电完成*/
    	data = 1;
    }
    else
    {
        /*启动失败*/
    	if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_STARTUP_VL))
        	|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_STARTUP_TH))
	    	|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_STARTUP_ERR)))
    	{
            /*高压上电故障*/
        	data = 2;
    	}
        /*停机失败*/
    	else if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_STOP_CH))
	    	|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_STOP_ERR)))
    	{
            /*高压下电故障*/
        	data = 3;
    	}
    	else
    	{
            /*高压未上电*/
        	data = 0;
    	}
    }

    /*高压状态*/
    msg.Data[2] = data;

    /*初始化*/
	if(eWORK_INIT == GetGWorkStateAPI())
	{
		data = 0;
	}
    /*满充状态*/
	else if(ePRO_FULL == GetGProtectStateAPI())
	{
		data = 2;
	}
    /*满放状态*/
	else if(ePRO_EMPTY == GetGProtectStateAPI())
	{
		data = 3;
	}
	/*故障状态(充放电保护)*/
	else if((eWORK_ERR == GetGWorkStateAPI())
		|| (eErr_SysPro == DiagGetGroupNowErrLevelAPI(0))
		|| (TacGetGAskSysProFlagAPI() > 0))
	{
		data = 5;
	}
    /*告警状态(同时禁止充放电)*/
	else if((ePRO_SYSPRO == GetGProtectStateAPI())
		|| ((ePRO_SYSLIM == GetGProtectStateAPI())
		&& (0 == GetGChgLimCurrAPI())
		&& (0 == GetGDhgLimCurrAPI())))
	{
		data = 4;
	}
	/*正常状态*/
	else
	{
		data = 1;
	}

    /*高压状态+系统状态*/
    msg.Data[2] |= (data << 2);

    /*保留*/
    msg.Data[3] = 0;

    /*SOC,0.1%*/
	data = GetGBattSocAPI();
    msg.Data[4] = data & 0xff;
    msg.Data[5] = (data >> 8) & 0xff;

    /*保留*/
    msg.Data[6] = 0;
    msg.Data[7] = 0;

    /*计算CRC校验码*/
    msg.Data[0] = Crc_ComputeCRC8((msg.ID & 0xffff), &msg.Data[1], 7);

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(BMU_CATL_CAN_PORT, &msg);
}

/*=================================================================================================
** @Name      : static void SendUpCatlBMSSumData1Info90(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS求和信息1
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSSumData1Info90(void)
{
	u16 data = 0;
    t_CANMSG msg;

    msg.ID = 0x040E9091 + (gCANDevInfo_1[eCANDev1_Addr] - BMU_BAS_DEVADDR);

    msg.Len = 8;

    /*系统运行*/
    if(eWORK_RUN == GetGWorkStateAPI())
    {
        /*高压在线柜数*/
        msg.Data[1] = 1;
    }
    else
    {
        /*高压在线柜数*/
        msg.Data[1] = 0;
    }

    /*系统电压*/
    data = GetGSampSumVoltAPI();
    msg.Data[2] = data & 0xff;
    msg.Data[3] = (data >> 8) & 0xff;

    /*系统电流*/
    data = GetGSampOutCurrAPI() + 20000;
    msg.Data[4] = data & 0xff;
    msg.Data[5] = (data >> 8) & 0xff;

    /*保留*/
    msg.Data[6] = 0;
    msg.Data[7] = 0;

    /*计算CRC校验码*/
    msg.Data[0] = Crc_ComputeCRC8((msg.ID & 0xffff), &msg.Data[1], 7);

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(BMU_CATL_CAN_PORT, &msg);
}

/*=================================================================================================
** @Name      : static void SendUpCatlBMSSumData2Info91(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS求和信息2
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSSumData2Info91(void)
{
	u16 data = 0;
    t_CANMSG msg;

    msg.ID = 0x040E9191 + (gCANDevInfo_1[eCANDev1_Addr] - BMU_BAS_DEVADDR);

    msg.Len = 8;

    /*保留*/
    msg.Data[1] = 0;


    /*可放电能(剩余电能), 0.1kWh*/
    data = GetGroupNowEnerAPI() / 100;
    msg.Data[4] = data & 0xff;
    msg.Data[5] = (data >> 8) & 0xff;

    /*可充电能, 0.1kWh*/
    data = GetGroupTotalEnerAPI() / 100 - data;
    msg.Data[2] = data & 0xff;
    msg.Data[3] = (data >> 8) & 0xff;

    /*SOH,1%*/
	data = (GetGBattSohAPI() + 5) / 10;
    msg.Data[6] = data & 0xff;

    /*保留*/
    msg.Data[7] = 0;

    /*计算CRC校验码*/
    msg.Data[0] = Crc_ComputeCRC8((msg.ID & 0xffff), &msg.Data[1], 7);

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(BMU_CATL_CAN_PORT, &msg);
}

/*=================================================================================================
** @Name      : static void SendUpCatlBMSSumData3Info93(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS求和信息3
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSSumData3Info93(void)
{
	u16 data = 0;
    t_CANMSG msg;

    msg.ID = 0x040E9391 + (gCANDevInfo_1[eCANDev1_Addr] - BMU_BAS_DEVADDR);

    msg.Len = 8;

    /*电芯最大电压*/
    data = GetGCellMaxVoltAPI();
    msg.Data[1] = data & 0xff;
    msg.Data[2] = (data >> 8) & 0xff;

    /*电芯最小电压*/
    data = GetGCellMinVoltAPI();
    msg.Data[3] = data & 0xff;
    msg.Data[4] = (data >> 8) & 0xff;

    /*电芯最大温度*/
	data = GetGCellMaxTempAPI() + 50;
    msg.Data[5] = data & 0xff;

    /*电芯最小温度*/
	data = GetGCellMinTempAPI() + 50;
    msg.Data[6] = data & 0xff;

    /*协议版本V1.0*/
    //msg.Data[7] = 0x10;
    /*软件版本号*/
    msg.Data[7] = BMU_SW_VERL & 0xff;

    /*计算CRC校验码*/
    msg.Data[0] = Crc_ComputeCRC8((msg.ID & 0xffff), &msg.Data[1], 7);

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(BMU_CATL_CAN_PORT, &msg);
}

/*=================================================================================================
** @Name      : static void SendUpCatlBMSLimDataInfo92(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS限值信息
** @The notes : 50ms周期
**===============================================================================================*/
static void SendUpCatlBMSLimDataInfo92(void)
{
	u16 data = 0;
    t_CANMSG msg;

    msg.ID = 0x040E9291 + (gCANDevInfo_1[eCANDev1_Addr] - BMU_BAS_DEVADDR);

    msg.Len = 8;

    /*放电电流限值*/
    data = GetGDhgLimCurrAPI();
    msg.Data[0] = data & 0xff;
    msg.Data[1] = (data >> 8) & 0xff;

    /*充电电流限值*/
    data = GetGChgLimCurrAPI();
    msg.Data[2] = data & 0xff;
    msg.Data[3] = (data >> 8) & 0xff;

    /*最高总电压限值*/
	data = GetGChgLimSumVAPI();
    msg.Data[4] = data & 0xff;
    msg.Data[5] = (data >> 8) & 0xff;

    /*最低总电压限值*/
	data = GetGDhgLimSumVAPI();
    msg.Data[6] = data & 0xff;
    msg.Data[7] = (data >> 8) & 0xff;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(BMU_CATL_CAN_PORT, &msg);
}

/*=================================================================================================
** @Name      : static void SendUpCatlBMSAlarmInfo0701(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS报警码信息
** @The notes : 100ms周期
**===============================================================================================*/
static void SendUpCatlBMSAlarmInfo0701(void)
{
    t_CANMSG msg;
    u_CatlAlarmInfo alarm = {0};

    msg.ID = 0x0701ff91 + (gCANDevInfo_1[eCANDev1_Addr] - BMU_BAS_DEVADDR);

    msg.Len = 8;

    /*单体过压*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VH1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CEL_VH1 / ALARM_DATATYPE), (eERR_ID_CEL_VH1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CHV = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VH2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CEL_VH2 / ALARM_DATATYPE), (eERR_ID_CEL_VH2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CHV = 1;
    }
    else
    {
    	alarm.Bits.CHV = 0;
    }

    /*单体欠压*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VL1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CEL_VL1 / ALARM_DATATYPE), (eERR_ID_CEL_VL1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CLV = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VL2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CEL_VL2 / ALARM_DATATYPE), (eERR_ID_CEL_VL2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CLV = 1;
    }
    else
    {
    	alarm.Bits.CLV = 0;
    }

    /*单体过温*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_TH1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_DHG_TH1 / ALARM_DATATYPE), (eERR_ID_DHG_TH1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CHT = 3;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_TH1))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CHG_TH1 / ALARM_DATATYPE), (eERR_ID_CHG_TH1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CHT = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_TH2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CHG_TH2 / ALARM_DATATYPE), (eERR_ID_CHG_TH2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CHT = 1;
    }
    else
    {
    	alarm.Bits.CHT = 0;
    }

    /*单体低温*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_TL1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_DHG_TL1 / ALARM_DATATYPE), (eERR_ID_DHG_TL1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CLT = 3;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_TL1))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CHG_TL1 / ALARM_DATATYPE), (eERR_ID_CHG_TL1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CLT = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_TL2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CHG_TL2 / ALARM_DATATYPE), (eERR_ID_CHG_TL2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CLT = 1;
    }
    else
    {
    	alarm.Bits.CLT = 0;
    }

    /*箱体过压*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_SUM_VH1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_SUM_VH1 / ALARM_DATATYPE), (eERR_ID_SUM_VH1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.GHV = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_SUM_VH2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_SUM_VH2 / ALARM_DATATYPE), (eERR_ID_SUM_VH2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.GHV = 1;
    }
    else
    {
    	alarm.Bits.GHV = 0;
    }

    /*箱体欠压*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_SUM_VL1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_SUM_VL1 / ALARM_DATATYPE), (eERR_ID_SUM_VL1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.GLV = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_SUM_VL2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_SUM_VL2 / ALARM_DATATYPE), (eERR_ID_SUM_VL2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.GLV = 1;
    }
    else
    {
    	alarm.Bits.GLV = 0;
    }

    /*箱体过温*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_PER_TH1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_PER_TH1 / ALARM_DATATYPE), (eERR_ID_PER_TH1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.GHT = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_PER_TH2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_PER_TH2 / ALARM_DATATYPE), (eERR_ID_PER_TH2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.GHT = 1;
    }
    else
    {
    	alarm.Bits.GHT = 0;
    }

    /*箱体低温*/
    alarm.Bits.GLT = 0;

    /*充电过流*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_CO1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CHG_CO1 / ALARM_DATATYPE), (eERR_ID_CHG_CO1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.OCC = 3;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_CO2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CHG_CO2 / ALARM_DATATYPE), (eERR_ID_CHG_CO2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.OCC = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_COT))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CHG_COT / ALARM_DATATYPE), (eERR_ID_CHG_COT % ALARM_DATATYPE)))
    {
    	alarm.Bits.OCC = 1;
    }
    else
    {
    	alarm.Bits.OCC = 0;
    }

    /*放电过流*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_CO1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_DHG_CO1 / ALARM_DATATYPE), (eERR_ID_DHG_CO1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.ODC = 3;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_CO2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_DHG_CO2 / ALARM_DATATYPE), (eERR_ID_DHG_CO2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.ODC = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_COT))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_DHG_COT / ALARM_DATATYPE), (eERR_ID_DHG_COT % ALARM_DATATYPE)))
    {
    	alarm.Bits.ODC = 1;
    }
    else
    {
    	alarm.Bits.ODC = 0;
    }

    /*单体温差过大*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CT_DIF1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CT_DIF1 / ALARM_DATATYPE), (eERR_ID_CT_DIF1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CTDIFF = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CT_DIF2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CT_DIF2 / ALARM_DATATYPE), (eERR_ID_CT_DIF2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CTDIFF = 1;
    }
    else
    {
    	alarm.Bits.CTDIFF = 0;
    }

    /*预充未满启动失败*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_STARTUP_VL))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_STARTUP_VL - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_STARTUP_VL - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.UPERR = 1;
    }
    else
    {
    	alarm.Bits.UPERR = 0;
    }

    /*SOC低*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CAP_SL1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CAP_SL1 / ALARM_DATATYPE), (eERR_ID_CAP_SL1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.SOCL = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CAP_SL2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CAP_SL2 / ALARM_DATATYPE), (eERR_ID_CAP_SL2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.SOCL = 1;
    }
    else
    {
    	alarm.Bits.SOCL = 0;
    }

    /*高边驱动控制故障*/
    /*if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_K0RE_EXP))
        || (eErr_B == DiagGetErrFilStateAPI(eERR_ID_K1RE_EXP))
	    || (eErr_B == DiagGetErrFilStateAPI(eERR_ID_K2RE_EXP))
	    || (eErr_B == DiagGetErrFilStateAPI(eERR_ID_K3RE_EXP))
	    || (eErr_B == DiagGetErrFilStateAPI(eERR_ID_K7RE_EXP))
	    || (eErr_B == DiagGetErrFilStateAPI(eERR_ID_K8RE_EXP))
	    || (eErr_B == DiagGetErrFilStateAPI(eERR_ID_L0RE_EXP))
	    || (eErr_B == DiagGetErrFilStateAPI(eERR_ID_L2RE_EXP)))*/
    if((GetGFaultInfoAPI((eERR_ID_K0RE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE) & 0xff00) > 0)
    {
    	alarm.Bits.HSERR = 1;
    }
    else
    {
    	alarm.Bits.HSERR = 0;
    }

    /*单体压差过大*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CV_DIF1))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CV_DIF1 / ALARM_DATATYPE), (eERR_ID_CV_DIF1 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CVDIFF = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CV_DIF2))
    else if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CV_DIF2 / ALARM_DATATYPE), (eERR_ID_CV_DIF2 % ALARM_DATATYPE)))
    {
    	alarm.Bits.CVDIFF = 1;
    }
    else
    {
    	alarm.Bits.CVDIFF = 0;
    }

    /*均衡异常*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_BAL_CEXP))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_BAL_CEXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BAL_CEXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.BALERR = 2;
    }
    //else if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_BAL_TEXP))
    else if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_BAL_TEXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BAL_TEXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.BALERR = 1;
    }
    else
    {
    	alarm.Bits.BALERR = 0;
    }

    /*带载切断继电器*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_SWHOFF_CH))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_SWHOFF_CH - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_SWHOFF_CH - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.OCOFF = 1;
    }
    else
    {
    	alarm.Bits.OCOFF = 0;
    }

    /*预充继电器闭合故障*/
    //if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_K1PRE_EXP))
    if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_K1PRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_K1PRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    	&& ((DiagGetSwhStateVExpFlagAPI() & 0xf0) > 0))
    {
    	alarm.Bits.PREERR = 1;
    }
    else
    {
    	alarm.Bits.PREERR = 0;
    }

    /*主正继电器闭合故障*/
    //if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_K0MAI_EXP))
    if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_K0MAI_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_K0MAI_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    	&& ((DiagGetSwhStateVExpFlagAPI() & 0xf0) > 0))
    {
    	alarm.Bits.MAIERR = 1;
    }
    else
    {
    	alarm.Bits.MAIERR = 0;
    }

    /*主负继电器闭合故障*/
    //if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_K2NEG_EXP))
    if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_K2NEG_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_K2NEG_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    	&& ((DiagGetSwhStateVExpFlagAPI() & 0xf0) > 0))
    {
    	alarm.Bits.NEGERR = 1;
    }
    else
    {
    	alarm.Bits.NEGERR = 0;
    }

    /*主正继电器粘连故障*/
    //if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_K0MAI_EXP))
    if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_K0MAI_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_K0MAI_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    	&& (((DiagGetSwhStateVExpFlagAPI() & 0x0f) > 0)
    	|| (GetSwhAdhesErrFlagAPI() > 0)))
    {
    	alarm.Bits.MAIADH = 1;
    }
    else
    {
    	alarm.Bits.MAIADH = 0;
    }

    /*主负继电器粘连故障*/
    //if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_K2NEG_EXP))
    if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_K2NEG_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_K2NEG_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    	&& (((DiagGetSwhStateVExpFlagAPI() & 0x0f) > 0)
    	|| (GetSwhAdhesErrFlagAPI() > 0)))
    {
    	alarm.Bits.NEGADH = 1;
    }
    else
    {
    	alarm.Bits.NEGADH = 0;
    }

    /*充放电过流4级告警*/
    /*if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_CO1))
    	|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_CO1)))*/
    if((1 == BitGet(GetGAlarmInfoAPI(eERR_ID_CHG_CO1 / ALARM_DATATYPE), (eERR_ID_CHG_CO1 % ALARM_DATATYPE)))
    	|| (1 == BitGet(GetGAlarmInfoAPI(eERR_ID_DHG_CO1 / ALARM_DATATYPE), (eERR_ID_DHG_CO1 % ALARM_DATATYPE))))
    {
    	alarm.Bits.OC4 = 1;
    }
    else
	{
    	alarm.Bits.OC4 = 0;
	}

    /*单体电压测量故障(断线)*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_SAMP_VOFF))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_SAMP_VOFF - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_SAMP_VOFF - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.CVOFF = 1;
    }
    else
    {
    	alarm.Bits.CVOFF = 0;
    }

    /*单体温度测量故障(断线)*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_SAMP_TOFF))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_SAMP_TOFF - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_SAMP_TOFF - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.CTOFF = 1;
    }
    else
    {
    	alarm.Bits.CTOFF = 0;
    }

    /*内侧和累加和压差大于50v报警(总压采样异常)*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_HSAMP_EXP))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_HSAMP_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_HSAMP_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.SVERR = 1;
    }
    else
    {
    	alarm.Bits.SVERR = 0;
    }

    /*箱体电流测量故障(电流采样异常)*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_CSAMP_ERR))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_CSAMP_ERR - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_CSAMP_ERR - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.GCERR = 1;
    }
    else
    {
    	alarm.Bits.GCERR = 0;
    }

    /*MCAN通信故障(外部CAN)*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_EXT_COFF))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_EXT_COFF - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_EXT_COFF - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.MCANOFF = 1;
    }
    else
    {
    	alarm.Bits.MCANOFF = 0;
    }

    /*CSC或SBMU MCU故障(从控故障)*/
    alarm.Bits.CSCERR = 0;

    /*总压极限报警*/
    alarm.Bits.SVHEST = 0;

    /*CCAN通信故障(内部从控CAN)*/
    /*if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_COFF))
    	|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF)))*/
    if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_INT_COFF - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_INT_COFF - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    	|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_INT_LOFF - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_INT_LOFF - eERR_ID_ERRSTART) % FAULT_DATATYPE))))
    {
    	alarm.Bits.CCANOFF = 1;
    }
    else
    {
    	alarm.Bits.CCANOFF = 0;
    }

    /*SCAN通信故障(内部外围设备CAN)*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_PER_COFF))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_PER_COFF - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_PER_COFF - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.SCANOFF = 1;
    }
    else
    {
    	alarm.Bits.SCANOFF = 0;
    }

    /*模组所有电芯温度测量故障(从控采样失效)*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_SAMP_EXP))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_SAMP_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_SAMP_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.CTERR = 1;
    }
    else
    {
    	alarm.Bits.CTERR = 0;
    }

    /*单体温度极限值*/
    alarm.Bits.CTHEST = 0;

    /*单体电压极限值*/
    alarm.Bits.CVHEST = 0;

    /*霍尔12V电源异常*/
    alarm.Bits.LENPWO = 0;

    /*CSC 24V电源异常*/
    alarm.Bits.CSCPWO = 0;

    /*SBMU 24V电源异常(电源过温)*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_DCV_TH))
    if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_DCV_TH / ALARM_DATATYPE), (eERR_ID_DCV_TH % ALARM_DATATYPE)))
    {
    	alarm.Bits.SMUPWO = 1;
    }
    else
    {
    	alarm.Bits.SMUPWO = 0;
    }

    /*SBMU烟感报警*/
    //if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_BATT_SMOG))
    if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    {
    	alarm.Bits.SMGERR = 1;
    }
    else
    {
    	alarm.Bits.SMGERR = 0;
    }

    /*SBMU高压断路报警*/
    /*if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_FUSE_OFF))
    	|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_ISO_OFF)))*/
    if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_FUSE_OFF - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FUSE_OFF - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
    	|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_ISO_OFF - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_ISO_OFF - eERR_ID_ERRSTART) % FAULT_DATATYPE))))
    {
    	alarm.Bits.HVOFF = 1;
    }
    else
    {
    	alarm.Bits.HVOFF = 0;
    }

    /*高压互锁故障*/
    alarm.Bits.HVLOCK = 0;

    /*风机1故障*/
    alarm.Bits.FAN1ERR = 0;

    /*风机2故障*/
    alarm.Bits.FAN2ERR = 0;

    /*主正/主负继电器短电源故障*/
    alarm.Bits.SWHERR = 0;

    /*取值发送*/
    msg.Data[0] = alarm.Long.WordL & 0xff;
    msg.Data[1] = (alarm.Long.WordL >> 8) & 0xff;
    msg.Data[2] = (alarm.Long.WordL >> 16) & 0xff;
    msg.Data[3] = (alarm.Long.WordL >> 24) & 0xff;

    /*取值发送*/
    msg.Data[4] = alarm.Long.WordH & 0xff;
    msg.Data[5] = (alarm.Long.WordH >> 8) & 0xff;
    msg.Data[6] = (alarm.Long.WordH >> 16) & 0xff;
    msg.Data[7] = (alarm.Long.WordH >> 24) & 0xff;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(BMU_CATL_CAN_PORT, &msg);
}

/*=================================================================================================
** @Name      : static void SendUpCatlBMSCharaInfo0b03(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS特征信息1
** @The notes : 100ms周期
**===============================================================================================*/
static void SendUpCatlBMSCharaInfo0b03(void)
{
    t_CANMSG msg;
    u32 data = 0;

    msg.ID = 0x0b03ff91 + (gCANDevInfo_1[eCANDev1_Addr] - BMU_BAS_DEVADDR);

    msg.Len = 8;

    /*取值发送*/
    //msg.Data[0] = 0;

    /*单体电压最值位置*/
    data = (GetGCellMinVNumAPI() - 1) % gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] + 1;//最低电压所在从控内电池号
    data <<= 6;
    data |= (GetGCellMinVNumAPI() - 1) / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] + 1;//最低电压所在从控号
    data <<= 6;
    data |= (GetGCellMaxVNumAPI() - 1) % gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] + 1;//最高电压所在从控内电池号
    data <<= 6;
    data |= (GetGCellMaxVNumAPI() - 1) / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] + 1;//最高电压所在从控号

    /*取值发送*/
    msg.Data[1] = data & 0xff;
    msg.Data[2] = (data >> 8) & 0xff;
    msg.Data[3] = (data >> 16) & 0xff;

    /*取值发送*/
    data = GetGCellMaxVoltAPI();
    msg.Data[4] = data & 0xff;
    msg.Data[5] = (data >> 8) & 0xff;

    /*取值发送*/
    data = GetGCellMinVoltAPI();
    msg.Data[6] = data & 0xff;
    msg.Data[7] = (data >> 8) & 0xff;

    /*计算CRC校验码*/
    msg.Data[0] = Crc_ComputeCRC8((msg.ID & 0xffff), &msg.Data[1], 7);

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(BMU_CATL_CAN_PORT, &msg);
}

/*=================================================================================================
** @Name      : static void SendUpCatlBMSCharaInfo0b04(void)
** @Input     : void
** @Output    : void
** @Function  : 上传BMS特征信息2
** @The notes : 100ms周期
**===============================================================================================*/
static void SendUpCatlBMSCharaInfo0b04(void)
{
    t_CANMSG msg;
    u16 data = 0;

    msg.ID = 0x0b04ff91 + (gCANDevInfo_1[eCANDev1_Addr] - BMU_BAS_DEVADDR);

    msg.Len = 8;

    /*取值发送*/
    //msg.Data[0] = 0;

    data = GetGCellAvgVoltAPI();        //平均电压
    msg.Data[1] = data & 0xff;
    msg.Data[2] = (data >> 8) & 0xff;

    data = GetGCellMaxTempAPI() + 50;   //最高温度
    msg.Data[3] = data & 0xff;

    data = GetGCellMinTempAPI() + 50;   //最低温度
    msg.Data[4] = data & 0xff;

    data = GetGCellAvgTempAPI() + 50;   //平均温度
    msg.Data[5] = data & 0xff;

    data = (GetGCellMaxTNumAPI() - 1) / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum] + 1;//最高温度所在从控号
    msg.Data[6] = data & 0xff;

    data = (GetGCellMinTNumAPI() - 1) / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum] + 1;//最低温度所在从控号
    msg.Data[7] = data & 0xff;

    /*计算CRC校验码*/
    msg.Data[0] = Crc_ComputeCRC8((msg.ID & 0xffff), &msg.Data[1], 7);

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(BMU_CATL_CAN_PORT, &msg);
}

/*=================================================================================================
** @Name      : static void RcvCatlPCSStatusCtrlCmd8f(t_CANMSG *msg)
** @Input     : msg:接收的报文
** @Output    : void
** @Function  : 接收PCS状态控制命令
** @The notes :
**===============================================================================================*/
static void RcvCatlPCSStatusCtrlCmd8f(t_CANMSG *msg)
{
	u8 calcCrc = 0;

    /*计算校验码*/
	calcCrc = Crc_ComputeCRC8((msg->ID & 0xffff), &msg->Data[1], 7);

    /*校验码正确*/
	if(calcCrc == msg->Data[0])
	{
		/*高压上电*/
		if(2 == (msg->Data[2] & 0x03))
		{
	        //SysStateCtrlEventRecord(eCTRL_NORSTART, BMU_CATL_CAN_PORT);   //系统状态控制事件记录
	        TacSetGWorkStateCmdAPI(eCTRL_NORSTART);             			//设置控制状态命令

		}
		/*高压下电*/
		else if(3 == (msg->Data[2] & 0x03))
		{
	        //SysStateCtrlEventRecord(eCTRL_NORSTOP, BMU_CATL_CAN_PORT);    //系统状态控制事件记录
	        TacSetGWorkStateCmdAPI(eCTRL_NORSTOP);             				 //设置控制状态命令
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvCatlPCSTimingRTCCmd92(t_CANMSG *msg)
** @Input     : msg:接收的报文
** @Output    : void
** @Function  : 接收PCS校时控制命令
** @The notes :
**===============================================================================================*/
static void RcvCatlPCSTimingRTCCmd92(t_CANMSG *msg)
{
	u8 calcCrc = 0;

    /*计算校验码*/
	calcCrc = Crc_ComputeCRC8((msg->ID & 0xffff), &msg->Data[1], 7);

    /*校验码正确*/
	if(calcCrc == msg->Data[0])
	{
		/*时间正确*/
		if((msg->Data[2] < 12) && (msg->Data[3] < 31) && (msg->Data[4] < 24) && (msg->Data[5] < 60) && (msg->Data[6] < 60))
		{
	        //系统校时
			gSysClock.year = 2000 + msg->Data[1];
			gSysClock.month = msg->Data[2] + 1;
			gSysClock.day = msg->Data[3] + 1;
			gSysClock.hour = msg->Data[4];
			gSysClock.minute = msg->Data[5];
			gSysClock.second = msg->Data[6];
			gSysClock.msecond = 0;
		}
	}
}

/*=================================================================================================
** @Name      : static u8 Crc_ComputeCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length)
** @Input     : ApplicationId:帧ID低16位 Crc_DataPtr:需要计算校验码的数组 Crc_Length:数组长度
** @Output    : void
** @Function  : 按SAE J1850 方式计算CRC值
** @The notes :
**===============================================================================================*/
static u8 Crc_ComputeCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length)
{
	u16 count = 0; 		/* loop counter for CRC calculation */
	u8 crc_value = 0; 	/* CRC initial value */
	u8 XOR_out = 0; 	/* Value to XOR output */

	crc_value = 0xffu;
	XOR_out = 0xffu;
	crc_value = Crc8Table[crc_value ^ (u8)(ApplicationId & 0x00ffu)]; 	/* Calculate CRC of low byte of ApplicationId */
	crc_value = Crc8Table[crc_value ^ (u8)(ApplicationId >> 8)]; 		/* Calculate CRC of high byte of ApplicationId */

	for (count = 0; count < (u16)Crc_Length; count++) 					/* Calculate CRC of data block */
	{
		crc_value = Crc8Table[crc_value ^ Crc_DataPtr[count]];
	}
	crc_value = crc_value ^ XOR_out;

	/* Calculate the official checksum.*/
	return(crc_value);
}
#endif
