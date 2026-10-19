/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : AuxDevMsg.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组与辅助设备通信CAN报文处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "DiagInterface.h"
#include "TacInterface.h"
#include "AuxDevTactic.h"
#include "AuxDevMsg.h"

#if(1 == BMU_IMEU_COM_EN)
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

static u8 sRcvCanNum = 0;               /*IMEU通信CAN口*/
extern u16 gGImeuRcvInfo_204[]; 	    /*IMEU辅助板接收信息*/
extern s8 gGImeuTempInfo_205[]; 		/*IMEU辅助板温湿度信息*/
extern u16 gGBmuGenPara_102[];  		/*主控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u32 CanBuildRupAuxMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
** @Input     : func:功能码 index:主索引 dadrr:目标地址 saddr:源地址
** @Output    : CAN报文ID
** @Function  : 用户创建上传CAN报文ID
** @The notes : 电池组内部下行报文
**===============================================================================================*/
static u32 CanBuildRupAuxMsgID(u8 func, u8 index, u8 dadrr, u8 saddr);

/*=================================================================================================
** @Name      : static u8 Crc_ComputeCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length)
** @Input     : ApplicationId:帧ID低16位 Crc_DataPtr:需要计算校验码的数组 Crc_Length:数组长度
** @Output    : void
** @Function  : 按SAE J1850 方式计算CRC值
** @The notes :
**===============================================================================================*/
static u8 Crc_ComputeCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length);
#endif

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CanRcvImeu2MsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信IMEU2接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void CanRcvImeu2MsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	#if(1 == BMU_IMEU_COM_EN)
	u8 i = 0;
	u8 saddr = 0;
	u8 daddr = 0;
	u8 index = 0;
	u8 funct = 0;

	/*解析CAN帧ID*/
	saddr = ID & 0xff;
	daddr = (ID >> 8) & 0xff;
	index = (ID >> 16) & 0xff;
	funct = (ID >> 25) & 0x0f;

	/*接收IMEU2报文设置BMU地址*/
	TacRcvImeuMsgAddAPI(saddr);

	/*非IMEU2报文,非发送给BMU报文,非对应CAN通道*/
	if(((canNum != eCAN1) && (canNum != eCAN2)) || ((IMEU_BAS_DEVADDR + GetGroupOffsetDevAddrAPI()) != saddr)
		//|| (((BMU_BAS_DEVADDR + GetGroupOffsetDevAddrAPI()) != daddr) && (0xff  != daddr))
		|| ((gGBmuGenPara_102[eBmuGenPara102_BmuDev] != daddr) && (0xff  != daddr))
		|| (DLC < 8) || (funct != 10))
	{
		return;
	}

	/*记录通信CAN口*/
	sRcvCanNum = canNum;

	/*接收IMEU2报文计数*/
	DiagRcvDevMsgAddAPI(saddr);

	/*按信息索引解析接收到的IMEU2报文信息*/
	switch(index)
	{
		/*1# ~ 8#PT1000温度值*/
		case 203:
			for(i = 0; i < 8; i++)
			{
				if(((s8)pdata[i] > 126) || ((s8)pdata[i] < -41))
				{
					gGImeuTempInfo_205[eIMEU2Info205_TP10001 + i] = -41;
				}
				else
				{
					gGImeuTempInfo_205[eIMEU2Info205_TP10001 + i] = pdata[i];
				}
			}
			break;

		/*9#PT1000温度值、10#PT1000温度值*/
		case 204:
			for(i = 0; i < 2; i++)
			{
				if(((s8)pdata[i] > 126) || ((s8)pdata[i] < -41))
				{
					gGImeuTempInfo_205[eIMEU2Info205_TP10009 + i] = -41;
				}
				else
				{
					gGImeuTempInfo_205[eIMEU2Info205_TP10009 + i] = pdata[i];
				}
			}

			gGImeuRcvInfo_204[eIMEU2Info204_FloodHz] = (((u16)pdata[3] << 8) | pdata[2]);
			gGImeuRcvInfo_204[eIMEU2Info204_PressSen] = (((u16)pdata[5] << 8) | pdata[4]);
			gGImeuRcvInfo_204[eIMEU2Info204_VibraSen] = (((u16)pdata[7] << 8) | pdata[6]);
			break;

		/*1#温湿度传感器温度值、1#温湿度传感器湿度值、2#温湿度传感器温度值、2#温湿度传感器湿度值、3#温湿度传感器温度值、3#温湿度传感器湿度值、空调柜内当前温度、空调柜内当前湿度*/
		case 205:
			for(i = 0; i < 8; i++)
			{
				//温度取有效值
				if(0 == (i % 2))
				{
					if(((s8)pdata[i] > 126) || ((s8)pdata[i] < -41))
					{
						gGImeuTempInfo_205[eIMEU2Info205_THSenT1 + i] = -41;
					}
					else
					{
						gGImeuTempInfo_205[eIMEU2Info205_THSenT1 + i] = pdata[i];
					}
				}
				//湿度取有效值
				else
				{
					if(pdata[i] > 100)
					{
						gGImeuTempInfo_205[eIMEU2Info205_THSenT1 + i] = 0;
					}
					else
					{
						gGImeuTempInfo_205[eIMEU2Info205_THSenT1 + i] = pdata[i];
					}
				}
			}
			break;

		/*可燃气体传感器1读数、可燃气体传感器2读数、可燃气体传感器3读数、 排风机转速回采*/
		case 206:
			for(i = 0; i < 4; i++)
			{
				gGImeuRcvInfo_204[eIMEU2Info204_GasConc1 + i] = ((u16)pdata[i * 2 + 1] << 8) | pdata[i * 2];
			}
			break;

		/*1#风扇转速回采、2#风扇转速回采、3#风扇转速回采、4#风扇转速回采*/
		case 207:
//			for(i = 0; i < 4; i++)
//			{
//				gGImeu2SampInfo_211[i + eIMEU2Info211_Fan0Back] = (pdata[i * 2 + 1] << 8) | pdata[i * 2];
//			}
			break;

		/*DI回检状态、可燃气体传感器通信告警状态、百叶窗关闭信号*/
		case 208:
			for(i = 0; i < 4; i++)
			{
				/*DI回检信息*/
				if(i < 3)
				{
					gGImeuRcvInfo_204[eIMEU2Info204_DiSign + i] = ((u16)pdata[i * 2 + 1] << 8) | pdata[i * 2];
				}
			}
			break;

		/*空调信息*/
		case 209:
			for(i = 0; i < 8; i++)
			{
				gGImeuTempInfo_205[eIMEU2Info205_AirState + i] = pdata[i];
			}
			break;

		/*除湿开启温度、除湿停止温度、 除湿开启湿度、 除湿停止湿度*/
		case 210:
			for(i = 0; i < 4; i++)
			{
				gGImeuTempInfo_205[eIMEU2Info205_AirDonT + i] = pdata[i];
			}
			break;

		/*同飞空调新增信息*/
		case 211:
			/*设置确认是TF空调类型*/
			TacSetAuxDevAirTypeAPI(1);

			for(i = 0; i < 6; i++)
			{
				if(i < 2)
				{
					gGImeuTempInfo_205[eIMEU2Info205_TFAirState + i] = pdata[i];
				}
				else if(i < 4)
				{
					gGImeuTempInfo_205[eIMEU2Info205_DehumDiff + i - 2] = pdata[i];
				}
				else
				{
					if(((s16)(((s16)pdata[5+(i-4)*2] << 8) | (s16)pdata[4+(i-4)*2]) > 126) || ((s16)(((s16)pdata[5+(i-4)*2] << 8) | (s16)pdata[4+(i-4)*2]) < -41))
					{
						gGImeuTempInfo_205[eIMEU2Info205_AirCoilerT + i - 4] = -41;
					}
					else
					{
						gGImeuTempInfo_205[eIMEU2Info205_AirCoilerT + i - 4] = (s8)(((s16)pdata[5+(i-4)*2] << 8) | (s16)pdata[4+(i-4)*2]);
					}
				}
			}
			break;

		default:
			break;
	}
	#endif
}

/*=================================================================================================
** @Name      : void SendCanImeu2SelfCtrlMsg220(u8 index, u8 cmd)
** @Input     : index:控制索引 cmd:控制命令
** @Output    : void
** @Function  : 发送IMEU2用户自定义控制信息CAN报文
** @The notes : 主索引220
**===============================================================================================*/
void SendCanImeu2SelfCtrlMsg220(u8 index, u8 cmd)
{
	#if(1 == BMU_IMEU_COM_EN)
    t_CANMSG msg;

    //索引有效
    if(index >= 4)
    {
    	return;
    }

    /*请求修改帧发送220索引*/
    msg.ID = CanBuildRupAuxMsgID(2, 220, (IMEU_BAS_DEVADDR + GetGroupOffsetDevAddrAPI()), (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev]);

    /*报文长度*/
    msg.Len = 8;

    /*220索引信息组成请求修改报文(带校验码)*/
    msg.Data[0] = 0xc1;
	msg.Data[1] = index;
	msg.Data[2] = cmd;
	msg.Data[3] = 0;
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = Crc_ComputeCRC8((msg.ID & 0xffff), &msg.Data[0], 7);

    /*向CAN1通道发送*/
	if(1 == sRcvCanNum)
	{
        (void)BSPCANSendQin(eCAN1, &msg);
	}
	/*向CAN2通道发送*/
	else if(1 == sRcvCanNum)
	{
		(void)BSPCANSendQin(eCAN2, &msg);
	}
	/*同时向CAN1/CAN2通道发送*/
	else
	{
        (void)BSPCANSendQin(eCAN1, &msg);
		(void)BSPCANSendQin(eCAN2, &msg);
	}
	#endif
}

/*=================================================================================================
** @Name      : void SendCanImeu2AirCtrlMsg221(u8 index, s8 cmd)
** @Input     : index:控制索引 cmd:控制命令或设置参数值
** @Output    : void
** @Function  : 发送IMEU2空调控制和参数设置信息CAN报文
** @The notes : 主索引221
**===============================================================================================*/
void SendCanImeu2AirCtrlMsg221(u8 index, s8 cmd)
{
	#if(1 == BMU_IMEU_COM_EN)
    t_CANMSG msg;

    //索引有效
    if(index >= eEuAirCmd_Len)
    {
    	return;
    }

    /*请求修改帧发送221索引*/
    msg.ID = CanBuildRupAuxMsgID(2, 221, (IMEU_BAS_DEVADDR + GetGroupOffsetDevAddrAPI()), (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev]);

    /*报文长度*/
    msg.Len = 8;

    /*220索引信息组成请求修改报文(带校验码)*/
    msg.Data[0] = 0xc1;
	msg.Data[1] = index;
	msg.Data[2] = (u8)cmd;
	msg.Data[3] = 0;
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = Crc_ComputeCRC8((msg.ID & 0xffff), &msg.Data[0], 7);

    /*向CAN1通道发送*/
	if(1 == sRcvCanNum)
	{
        (void)BSPCANSendQin(eCAN1, &msg);
	}
	/*向CAN2通道发送*/
	else if(1 == sRcvCanNum)
	{
		(void)BSPCANSendQin(eCAN2, &msg);
	}
	/*同时向CAN1/CAN2通道发送*/
	else
	{
        (void)BSPCANSendQin(eCAN1, &msg);
		(void)BSPCANSendQin(eCAN2, &msg);
	}
	#endif
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
#if(1 == BMU_IMEU_COM_EN)
/*=================================================================================================
** @Name      : static u32 CanBuildRupAuxMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
** @Input     : func:功能码 index:主索引 dadrr:目标地址 saddr:源地址
** @Output    : CAN报文ID
** @Function  : 用户创建上传CAN报文ID
** @The notes : 电池组内部下行报文
**===============================================================================================*/
static u32 CanBuildRupAuxMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
{
	u32 id = 0;

	id = ((u32)func << 25) | ((u32)1 << 24) | ((u32)index << 16) | ((u32)dadrr << 8) | saddr;

	return(id);
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
