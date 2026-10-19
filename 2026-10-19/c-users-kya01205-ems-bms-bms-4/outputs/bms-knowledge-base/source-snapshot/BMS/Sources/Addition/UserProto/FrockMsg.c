/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : FrockMsg.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 测试工装协议CAN报文模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "CANUser.h"
#include "Authority.h"
#include "ParaInterface.h"
#include "TacInterface.h"
#include "TacUser.h"
#include "CellAFESamp.h"
#include "CurrSample.h"
#include "DiagCheck.h"
#include "OutCurrCorr.h"
#include "HigVoltCorr.h"
#include "FrockMsg.h"

#if(1 == BMU_FROCK_CAN_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sCheckSciCommuState[3] = {0, 0, 0};     	/*检测RS485通信状态[0:接收正常 1:异常]*/

extern u16 gGPortStateInfo_56[];    /*输入输出口状态信息数组*/
extern s16 gGSysMainInfo_60[];	    /*系统整体信息数组*/
extern u16 gGSysCapInfo_61[];	    /*系统电量信息数组*/
extern u16 gGSysCellInfo_62[];	    /*系统单体信息数组*/
extern u16 gGVoltCharInfo_63[];	    /*电压特征信息数组*/
extern s16 gGTempCharInfo_64[];	    /*温度特征信息数组*/
extern s16 gGDcCTInfo_65[];	    	/*直流电流温度信息数组*/
extern u16 gGLVSampInfo_66[];	    /*低压采样信息数组*/
extern u16 gGFstLeakInfo_67[];	    /*直流电路信息数组*/
extern s16 gGHsCurrInfo_68[];	    /*高边驱动电流信息数组1*/
extern s16 gGHsCurrInfo_69[];	    /*高边驱动电流信息数组2*/
extern u16 gGCalibrResult_83[];	    /*校准结果状态信息数组*/
extern u16 gGDebugResult_82[];		/*调试结果状态信息数组*/
extern u16 gGAuthorCtrl_85[];		/*权限密码登录信息数组*/
extern u16 gGDebugCtrl_88[];		/*系统调试命令信息数组*/
extern u16 gGCalibrCtrl_89[];	    /*采样校准命令信息数组*/
extern s16 gGReferSampInfo_200[];	/*参考采集信息*/

extern u16 gSysHigParaRO_101[];		/*系统硬件参数设置*/
extern u16 gGBmuHigLevPara_103[];   /*主控高级参数值*/
extern u8 gGHardPara_104[];         /*主控硬件参数配置信息*/

extern t_SysClock gSysClock;        /*系统时钟(北京时间)*/
extern u8 gCANSerialNum_2[];        /*条码序列号*/
extern u16 gCANDevInfo_1[];         /*设备信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ReplyUpBMSCanCommuState01(u8 canNum)
** @Input     : canNum:查询CAN通道号
** @Output    : void
** @Function  : 回复各路CAN通道通信情况
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCanCommuState01(u8 canNum);

/*=================================================================================================
** @Name      : static void ReplyUpBMSSciCommuState02(void)
** @Input     : void
** @Output    : void
** @Function  : 回复各路RS485通道通信情况
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSSciCommuState02(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSHigSampInfo03(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS高压采样值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSHigSampInfo03(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSCurrSampInfo04(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS电流采样值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCurrSampInfo04(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSInsulationInfo05(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS绝缘值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSInsulationInfo05(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSLowSampInfo06(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS低压采样值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSLowSampInfo06(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSFstChgInfo07(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS快充采样值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSFstChgInfo07(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSLowInsidInfo08(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS低压内部值1
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSLowInsidInfo08(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSLowInsidInfo09(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS低压内部值2
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSLowInsidInfo09(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSInPortStateInfo0a(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS输入DI状态信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSInPortStateInfo0a(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSOutPortStateInfo0b(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS输出DO状态信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSOutPortStateInfo0b(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSFlashStateInfo0c(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS FLASH状态信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSFlashStateInfo0c(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSSysTimeInfo0d(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS系统时间信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSSysTimeInfo0d(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSCurrCorrKBInfo0e(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS电流采样校准KB值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCurrCorrKBInfo0e(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSVoltCorrKBInfo0f(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS高压采样校准KB值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSVoltCorrKBInfo0f(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSCellVoltInfo10(u8 index)
** @Input     : index:报文索引
** @Output    : void
** @Function  : 回复BMS单体电压值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCellVoltInfo10(u8 index);

/*=================================================================================================
** @Name      : static void ReplyUpBMSCellTempInfo20(u8 index)
** @Input     : index:报文索引
** @Output    : void
** @Function  : 回复BMS单体温度值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCellTempInfo20(u8 index);

/*=================================================================================================
** @Name      : static void ReplyUpBMSBalanTempInfo22(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS单体温度值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSBalanTempInfo22(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSBalanStateInfo23(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS单体温度值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSBalanStateInfo23(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSBarCodeInfo2d(u8 num)
** @Input     : num:查询帧数 0:所有帧
** @Output    : void
** @Function  : 回复BMS条码信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSBarCodeInfo2d(u8 num);

/*=================================================================================================
** @Name      : static void ReplyUpBMSVersionInfo2e(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS版本号信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSVersionInfo2e(void);

/*=================================================================================================
** @Name      : static void ReplyUpBMSCorrStateInfo2f(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS采样通道校准状态信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCorrStateInfo2f(void);

/*=================================================================================================
 ** @Name      : static void RcvFrockCtrlBMSModeCmdDeal30(t_CANMSG *msg)
 ** @Input     : msg:接收到的控制报文
 ** @Output    : void
 ** @Function  : 接收工装控制BMS工作模式命令处理
 ** @The notes : CAN接收到控制命令时调用
 **===============================================================================================*/
 static void RcvFrockCtrlBMSModeCmdDeal30(t_CANMSG *msg);

 /*=================================================================================================
 ** @Name      : static void RcvFrockCtrlBMSSwichCmdDeal31(t_CANMSG *msg)
 ** @Input     : msg:接收到的控制报文
 ** @Output    : void
 ** @Function  : 接收工装控制BMS接触器命令处理
 ** @The notes : CAN接收到控制命令时调用
 **===============================================================================================*/
 static void RcvFrockCtrlBMSSwichCmdDeal31(t_CANMSG *msg);

 /*=================================================================================================
 ** @Name      : static void RcvFrockCtrlBMSTimeCmdDeal32(t_CANMSG *msg)
 ** @Input     : msg:接收到的控制报文
 ** @Output    : void
 ** @Function  : 接收工装控制BMS校时命令处理
 ** @The notes : CAN接收到控制命令时调用
 **===============================================================================================*/
 static void RcvFrockCtrlBMSTimeCmdDeal32(t_CANMSG *msg);

 /*=================================================================================================
 ** @Name      : static void RcvFrockCtrlBMSCorrCmdDeal33(t_CANMSG *msg)
 ** @Input     : msg:接收到的控制报文
 ** @Output    : void
 ** @Function  : 接收工装控制BMS校准命令处理
 ** @The notes : CAN接收到控制命令时调用
 **===============================================================================================*/
 static void RcvFrockCtrlBMSCorrCmdDeal33(t_CANMSG *msg);

 /*=================================================================================================
 ** @Name      : static void RcvFrockCtrlBMSRateCmdDeal34(t_CANMSG *msg)
 ** @Input     : msg:接收到的控制报文
 ** @Output    : void
 ** @Function  : 接收工装控制BMS通信波特率设置命令处理
 ** @The notes : CAN接收到控制命令时调用
 **===============================================================================================*/
 static void RcvFrockCtrlBMSRateCmdDeal34(t_CANMSG *msg);

 /*=================================================================================================
 ** @Name      : static void RcvFrockCtrlBMSBarCodeCmdDeal35(t_CANMSG *msg)
 ** @Input     : msg:接收到的控制报文
 ** @Output    : void
 ** @Function  : 接收工装控制BMS通信波特率设置命令处理
 ** @The notes : CAN接收到控制命令时调用
 **===============================================================================================*/
 static void RcvFrockCtrlBMSBarCodeCmdDeal35(t_CANMSG *msg);

 /*=================================================================================================
 ** @Name      : static void RcvFrockCtrlBMSBalanCmdDeal36(t_CANMSG *msg)
 ** @Input     : msg:接收到的控制报文
 ** @Output    : void
 ** @Function  : 接收工装控制BMS均衡控制命令处理
 ** @The notes : CAN接收到控制命令时调用
 **===============================================================================================*/
 static void RcvFrockCtrlBMSBalanCmdDeal36(t_CANMSG *msg);

 /*=================================================================================================
 ** @Name      : static void RcvFrockCtrlBMSSlaveTypeCmdDeal37(t_CANMSG *msg)
 ** @Input     : msg:接收到的控制报文
 ** @Output    : void
 ** @Function  : 接收工装控制BMS从控类型命令处理
 ** @The notes : CAN接收到控制命令时调用
 **===============================================================================================*/
 static void RcvFrockCtrlBMSSlaveTypeCmdDeal37(t_CANMSG *msg);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CanRcvFrockMsgHandleAPI(t_CANMSG *msg, u8 canNum)
** @Input     : msg:接收到的报文 canNum:接收CAN通道号
** @Output    : void
** @Function  : 测试工装模块CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvFrockMsgHandleAPI(t_CANMSG *msg, u8 canNum)
{
	u8 rcvCmd = 0;
	u8 rcvpro = 0;
	u16 rcvDev = 0;

	rcvDev = msg->ID & 0xffff;
	rcvCmd = (msg->ID >> 16) & 0xff;
	rcvpro = (msg->ID >> 24) & 0xff;

	/*命令不对*/
	if((0x00f0 != rcvDev) || (0x0B != rcvpro) || (msg->Len < 8))
	{
		return;
	}

	/*除通信测试其他命令只处理CAN0口*/
	if((eCAN0 != canNum) && (0x01 != rcvCmd))
	{
		return;
	}

	switch(rcvCmd)
	{
		case 0x01:
			//查询所有CAN后或指定单个CAN口
			if((0 == msg->Data[0]) || ((canNum + 1) == msg->Data[0]))
			{
				ReplyUpBMSCanCommuState01(canNum);
			}
			break;

		case 0x02:
			ReplyUpBMSSciCommuState02();
			break;

		case 0x03:
			ReplyUpBMSHigSampInfo03();
			break;

		case 0x04:
			ReplyUpBMSCurrSampInfo04();
			break;

		case 0x05:
			ReplyUpBMSInsulationInfo05();
			break;

		case 0x06:
			ReplyUpBMSLowSampInfo06();
			break;

		case 0x07:
			ReplyUpBMSFstChgInfo07();
			break;

		case 0x08:
			ReplyUpBMSLowInsidInfo08();
			break;

		case 0x09:
			ReplyUpBMSLowInsidInfo09();
			break;

		case 0x0a:
			ReplyUpBMSInPortStateInfo0a();
			break;

		case 0x0b:
			ReplyUpBMSOutPortStateInfo0b();
			break;

		case 0x0c:
			ReplyUpBMSFlashStateInfo0c();
			break;

		case 0x0d:
			ReplyUpBMSSysTimeInfo0d();
			break;

		case 0x0e:
			ReplyUpBMSCurrCorrKBInfo0e();
			break;

		case 0x0f:
			ReplyUpBMSVoltCorrKBInfo0f();
			break;

		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x18:
		case 0x19:
		case 0x1a:
		case 0x1b:
		case 0x1c:
		case 0x1d:
		case 0x1e:
		case 0x1f:
			/*未完成一遍采样不应答*/
			if(1 != GetCellVSampFinFlagAPI())
			{
				return;
			}
			ReplyUpBMSCellVoltInfo10(rcvCmd - 0x10);
			break;

		case 0x20:
		case 0x21:
			/*未完成一遍采样不应答*/
			if(1 != GetCellTSampFinFlagAPI())
			{
				return;
			}
			ReplyUpBMSCellTempInfo20(rcvCmd - 0x20);
			break;

		case 0x22:
			ReplyUpBMSBalanTempInfo22();
			break;

		case 0x23:
			ReplyUpBMSBalanStateInfo23();
			break;

		case 0x2d:
			ReplyUpBMSBarCodeInfo2d(msg->Data[0]);
			break;

		case 0x2e:
			ReplyUpBMSVersionInfo2e();
			break;

		case 0x2f:
			ReplyUpBMSCorrStateInfo2f();
			break;

		case 0x30:
			RcvFrockCtrlBMSModeCmdDeal30(msg);
			break;

		case 0x31:
			RcvFrockCtrlBMSSwichCmdDeal31(msg);
			break;

		case 0x32:
			RcvFrockCtrlBMSTimeCmdDeal32(msg);
			break;

		case 0x33:
			RcvFrockCtrlBMSCorrCmdDeal33(msg);
			break;

		case 0x34:
			RcvFrockCtrlBMSRateCmdDeal34(msg);
			break;

		case 0x35:
			RcvFrockCtrlBMSBarCodeCmdDeal35(msg);
			break;

		case 0x36:
			RcvFrockCtrlBMSBalanCmdDeal36(msg);
			break;

		case 0x37:
			RcvFrockCtrlBMSSlaveTypeCmdDeal37(msg);
			break;

		default:
			break;
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ReplyUpBMSCanCommuState01(u8 canNum)
** @Input     : canNum:查询CAN通道号
** @Output    : void
** @Function  : 回复各路CAN通道通信情况
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCanCommuState01(u8 canNum)
{
    t_CANMSG msg;

    msg.ID = 0x0B01F000;

    msg.Len = 8;

    msg.Data[0] = canNum + 1;                   /*CAN通道号*/
    msg.Data[1] = 0;                            /*CAN通道状态*/
    msg.Data[2] = 0;

    //波特率
    if(125 == gSysHigParaRO_101[eSHWPara101_CAN0Rate])
    {
    	msg.Data[3] = 1;
    }
    else if(250 == gSysHigParaRO_101[eSHWPara101_CAN0Rate])
    {
    	msg.Data[3] = 2;
    }
    else if(500 == gSysHigParaRO_101[eSHWPara101_CAN0Rate])
    {
    	msg.Data[3] = 3;
    }
    else if(1000 == gSysHigParaRO_101[eSHWPara101_CAN0Rate])
    {
    	msg.Data[3] = 4;
    }
    else
    {
    	msg.Data[3] = 0;
    }
    if(125 == gSysHigParaRO_101[eSHWPara101_CAN1Rate])
    {
    	msg.Data[4] = 1;
    }
    else if(250 == gSysHigParaRO_101[eSHWPara101_CAN1Rate])
    {
    	msg.Data[4] = 2;
    }
    else if(500 == gSysHigParaRO_101[eSHWPara101_CAN1Rate])
    {
    	msg.Data[4] = 3;
    }
    else if(1000 == gSysHigParaRO_101[eSHWPara101_CAN1Rate])
    {
    	msg.Data[4] = 4;
    }
    else
    {
    	msg.Data[4] = 0;
    }
    if(125 == gSysHigParaRO_101[eSHWPara101_CAN2Rate])
    {
    	msg.Data[5] = 1;
    }
    else if(250 == gSysHigParaRO_101[eSHWPara101_CAN2Rate])
    {
    	msg.Data[5] = 2;
    }
    else if(500 == gSysHigParaRO_101[eSHWPara101_CAN2Rate])
    {
    	msg.Data[5] = 3;
    }
    else if(1000 == gSysHigParaRO_101[eSHWPara101_CAN2Rate])
    {
    	msg.Data[5] = 4;
    }
    else
    {
    	msg.Data[5] = 0;
    }

    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin((e_CANNUM)canNum, &msg);//向查询CAN通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSSciCommuState02(void)
** @Input     : void
** @Output    : void
** @Function  : 回复各路RS485通道通信情况
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSSciCommuState02(void)
{
    t_CANMSG msg;

    msg.ID = 0x0B02F000;

    msg.Len = 8;

    //状态
    msg.Data[0] = sCheckSciCommuState[0];       /*RS485-0状态*/
    msg.Data[1] = sCheckSciCommuState[1];       /*RS485-1状态*/
    msg.Data[2] = sCheckSciCommuState[2];       /*RS485-2状态*/

    //波特率
    if(9600 == gSysHigParaRO_101[eSHWPara101_RemoteRate])
    {
    	msg.Data[3] = 1;
    }
    else
    {
    	msg.Data[3] = 2;
    }

    if(9600 == gSysHigParaRO_101[eSHWPara101_LocalRate])
    {
    	msg.Data[4] = 1;
    }
    else
    {
    	msg.Data[4] = 2;
    }

    if(9600 == gSysHigParaRO_101[eSHWPara101_MonitRate])
    {
    	msg.Data[5] = 1;
    }
    else
    {
    	msg.Data[5] = 2;
    }

    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSHigSampInfo03(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS高压采样值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSHigSampInfo03(void)
{
    t_CANMSG msg;

    msg.ID = 0x0B03F000;

    msg.Len = 8;

    //总压采样值(高位在前)
    msg.Data[0] = (gGSysMainInfo_60[eSysMainInfo60_SumV] >> 8) & 0xff;
    msg.Data[1] = gGSysMainInfo_60[eSysMainInfo60_SumV] & 0xff;

    //预充采样值(高位在前)
    msg.Data[2] = (gGSysMainInfo_60[eSysMainInfo60_PreV] >> 8) & 0xff;
    msg.Data[3] = gGSysMainInfo_60[eSysMainInfo60_PreV] & 0xff;

    //负压采样值(高位在前)
    msg.Data[4] = (gGLVSampInfo_66[eLVSamp66_NegV] >> 8) & 0xff;
    msg.Data[5] = gGLVSampInfo_66[eLVSamp66_NegV] & 0xff;

    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSCurrSampInfo04(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS电流采样值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCurrSampInfo04(void)
{
    t_CANMSG msg;
    s32 data = 0;

    msg.ID = 0x0B04F000;

    msg.Len = 8;

    //霍尔电流采样值(高位在前)
//    if((ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_HALFAU))      /*霍尔过载故障(低电平)*/
//    	|| (GetGLowHall5VVoltAPI() <= CURR_HALL5V_MINV)         /*霍尔5V供电异常*/
//		|| (gGDcCTInfo_65[eDcCTInfo65_HallSignV] <= 40)         /*采样值异常*/
//		|| (gGDcCTInfo_65[eDcCTInfo65_HallSignV] >= 470))
//    {
//    	//取实际采样值
//    	data = gGDcCTInfo_65[eDcCTInfo65_HallSignV];
//    }
//    else
//    {
//		data = (GetGRealCurr10mAAPI(eCADC_Hall) * CorrGetCurrCorrKBValue(eOCCorr_HalOutK) + CorrGetCurrCorrKBValue(eOCCorr_HalOutB)) / OC_CORR_BLOWUP;
//		data = data / 100 + 250;
//		if(data < 0)
//		{
//			data = 0;
//		}
//		else if(data > 500)
//		{
//			data = 500;
//		}
//    }
    data = gGDcCTInfo_65[eDcCTInfo65_HallSignV];

    msg.Data[0] = (data >> 8) & 0xff;
    msg.Data[1] = data & 0xff;

    //分流器采样值(高位在前)
//    if(GetGRealCurr10mAAPI(eCAFE_Shunt) < 0)/*充电*/
//    {
//        data = (GetGRealCurr10mAAPI(eCAFE_Shunt) * CorrGetCurrCorrKBValue(eOCCorr_ShuChgK) + CorrGetCurrCorrKBValue(eOCCorr_ShuChgB)) / OC_CORR_BLOWUP;
//
//        data = data / 4;
//        if(data > 0)
//        {
//            data = 0;
//        }
//    }
//    else if(GetGRealCurr10mAAPI(eCAFE_Shunt) > 0)/*放电*/
//    {
//        data = (GetGRealCurr10mAAPI(eCAFE_Shunt) * CorrGetCurrCorrKBValue(eOCCorr_ShuDhgK) + CorrGetCurrCorrKBValue(eOCCorr_ShuDhgB)) / OC_CORR_BLOWUP;
//
//        data = data / 4;
//        if(data < 0)
//        {
//            data = 0;
//        }
//    }
//    else
//    {
//    	/*采样失效*/
//    	if((gGDcCTInfo_65[eDcCTInfo65_ShutSignV] < -9000) || (gGDcCTInfo_65[eDcCTInfo65_ShutSignV] > 9000))
//    	{
//    		data = gGDcCTInfo_65[eDcCTInfo65_ShutSignV];
//    	}
//    	else
//    	{
//            data = 0;
//    	}
//    }
    data = gGDcCTInfo_65[eDcCTInfo65_ShutSignV];

    msg.Data[2] = (data >> 8) & 0xff;
    msg.Data[3] = data & 0xff;

    //环境温度采样值
    msg.Data[4] = gGSysCellInfo_62[eSysCellInfo62_EnvT] & 0xff;

    //分流器温度采样值
    msg.Data[5] = gGDcCTInfo_65[eDcCTInfo65_ShutT] & 0xff;

    //低压电源温度采样值
    msg.Data[6] = gGDcCTInfo_65[eDcCTInfo65_PowrT] & 0xff;

    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSInsulationInfo05(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS绝缘值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSInsulationInfo05(void)
{
    t_CANMSG msg;

    msg.ID = 0x0B05F000;

    msg.Len = 8;

    //正极绝缘值(高位在前)
    msg.Data[0] = (gGFstLeakInfo_67[eFstLeakInfo67_PasInsu] >> 8) & 0xff;
    msg.Data[1] = gGFstLeakInfo_67[eFstLeakInfo67_PasInsu] & 0xff;

    //负极绝缘值(高位在前)
    msg.Data[2] = (gGFstLeakInfo_67[eFstLeakInfo67_NegInsu] >> 8) & 0xff;
    msg.Data[3] = gGFstLeakInfo_67[eFstLeakInfo67_NegInsu] & 0xff;

    msg.Data[4] = 0;
    msg.Data[5] = 0;
    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSLowSampInfo06(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS低压采样值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSLowSampInfo06(void)
{
    t_CANMSG msg;

    msg.ID = 0x0B06F000;

    msg.Len = 8;

    //供电电压采样值(高位在前)
    msg.Data[0] = (gGLVSampInfo_66[eLVSamp66_PwrV] >> 8) & 0xff;
    msg.Data[1] = gGLVSampInfo_66[eLVSamp66_PwrV] & 0xff;

    //ACC电压采样值(高位在前)
    msg.Data[2] = (gGLVSampInfo_66[eLVSamp66_AccV] >> 8) & 0xff;
    msg.Data[3] = gGLVSampInfo_66[eLVSamp66_AccV] & 0xff;

    //霍尔电源电压采样值(高位在前)
    msg.Data[4] = (gGLVSampInfo_66[eLVSamp66_Hall5V] >> 8) & 0xff;
    msg.Data[5] = gGLVSampInfo_66[eLVSamp66_Hall5V] & 0xff;

    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSFstChgInfo07(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS快充采样值
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSFstChgInfo07(void)
{
    t_CANMSG msg;

    msg.ID = 0x0B07F000;

    msg.Len = 8;

    //KC电压值(高位在前)
    msg.Data[0] = (gGFstLeakInfo_67[eFstLeakInfo67_FcV] >> 8) & 0xff;
    msg.Data[1] = gGFstLeakInfo_67[eFstLeakInfo67_FcV] & 0xff;

    //CC2电压值(高位在前)
    msg.Data[2] = (gGFstLeakInfo_67[eFstLeakInfo67_CC2V] >> 8) & 0xff;
    msg.Data[3] = gGFstLeakInfo_67[eFstLeakInfo67_CC2V] & 0xff;

    msg.Data[4] = 0;
    msg.Data[5] = 0;
    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSLowInsidInfo08(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS低压内部值1
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSLowInsidInfo08(void)
{
    t_CANMSG msg;

    msg.ID = 0x0B08F000;

    msg.Len = 8;

    //MCU5V电压采样值(高位在前)
    msg.Data[0] = (gGReferSampInfo_200[eReferSampInfo200_McuPwr5V] >> 8) & 0xff;
    msg.Data[1] = gGReferSampInfo_200[eReferSampInfo200_McuPwr5V] & 0xff;

    //SBC5V(AUX通道)电压采样值(高位在前)
    msg.Data[2] = (gGReferSampInfo_200[eReferSampInfo200_SbcAux5V] >> 8) & 0xff;
    msg.Data[3] = gGReferSampInfo_200[eReferSampInfo200_SbcAux5V] & 0xff;

    //SBC芯片MUX电压采样值(高位在前)
    msg.Data[4] = (gGReferSampInfo_200[eReferSampInfo200_SbcMuxV] >> 8) & 0xff;
    msg.Data[5] = gGReferSampInfo_200[eReferSampInfo200_SbcMuxV] & 0xff;

    //PWM电压采样值(高位在前)
    msg.Data[6] = (gGReferSampInfo_200[eReferSampInfo200_PwmAdcV] >> 8) & 0xff;
    msg.Data[7] = gGReferSampInfo_200[eReferSampInfo200_PwmAdcV] & 0xff;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSLowInsidInfo09(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS低压内部值2
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSLowInsidInfo09(void)
{
    t_CANMSG msg;

    msg.ID = 0x0B09F000;

    msg.Len = 8;

    //Link+电压采样值(高位在前)
    msg.Data[0] = (gGReferSampInfo_200[eReferSampInfo200_Link1] >> 8) & 0xff;
    msg.Data[1] = gGReferSampInfo_200[eReferSampInfo200_Link1] & 0xff;

    //Link-电压采样值(高位在前)
    msg.Data[2] = (gGReferSampInfo_200[eReferSampInfo200_Link2] >> 8) & 0xff;
    msg.Data[3] = gGReferSampInfo_200[eReferSampInfo200_Link2] & 0xff;

    //LTC2949基准电压采样值(高位在前)
    msg.Data[4] = (gGReferSampInfo_200[eReferSampInfo200_Ltc2949Vref] >> 8) & 0xff;
    msg.Data[5] = gGReferSampInfo_200[eReferSampInfo200_Ltc2949Vref] & 0xff;

    //LTC2949 5V电压采样值(高位在前)
    msg.Data[6] = (gGReferSampInfo_200[eReferSampInfo200_Ltc2949_5V] >> 8) & 0xff;
    msg.Data[7] = gGReferSampInfo_200[eReferSampInfo200_Ltc2949_5V] & 0xff;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSInPortStateInfo0a(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS输入DI状态信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSInPortStateInfo0a(void)
{
	u16 state = 0;
    t_CANMSG msg;

    msg.ID = 0x0B0aF000;

    msg.Len = 8;

    state = gGPortStateInfo_56[ePortStateInfo56_Input] >> 6;

    msg.Data[0] = (state >> 0) & 0x01;
    msg.Data[1] = (state >> 1) & 0x01;
    msg.Data[2] = (state >> 2) & 0x01;
    msg.Data[3] = (state >> 3) & 0x01;
    msg.Data[4] = (state >> 4) & 0x01;
    msg.Data[5] = (state >> 5) & 0x01;
    msg.Data[6] = (state >> 6) & 0x01;

    msg.Data[7] = DEVGPIOGetHWVersion() & 0x07;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSOutPortStateInfo0b(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS输出DO状态信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSOutPortStateInfo0b(void)
{
	u16 state = 0;
    t_CANMSG msg;

    msg.ID = 0x0B0bF000;

    msg.Len = 8;

    state = DEVGPIOGetCtrlHSwitchState();

    //高边开关控制状态(低电平断开)
    msg.Data[0] = (state >> 8) & 0xff;
    msg.Data[1] = state & 0xff;

    //低边开关控制状态(低电平断开)
    state = DEVGPIOGetCtrlLSwitchState();
    msg.Data[2] = state & 0x03;

    //高边开关故障状态(低电平故障)
    //state = (DEVGPIOGetCtrlHSwitchState() & 0x0ff) | ((u16)DEVGPIOGetInPortState(ePIn_K9_Sta) << 8);
    state = 0x0ff | ((u16)DEVGPIOGetInPortState(ePIn_K9_Sta) << 8);
    msg.Data[3] = (state >> 8) & 0xff;
    msg.Data[4] = state & 0xff;

    //低边开关故障状态(低电平故障)
    state = (DEVGPIOGetInPortState(ePIn_LK1_Sta) & 0x01) | (DEVGPIOGetInPortState(ePIn_LK3_Sta) << 1);
    msg.Data[5] = state & 0xff;

    //干接点控制状态(低电平断开)
    msg.Data[6] = DEVGPIOGetOutputDryState() & 0x07;

    msg.Data[7] = GetGHigPGNDSwhStateAPI() & 0x01;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSFlashStateInfo0c(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS FLASH状态信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSFlashStateInfo0c(void)
{
    t_CANMSG msg;

    msg.ID = 0x0B0cF000;

    msg.Len = 8;

    /*外部EEPROM状态*/
    if(1 == DEVEXTEEPGetReadyFlag())
    {
    	msg.Data[0] = 0;
    }
    else
    {
        /*外部EEPROM异常则重新初始化*/
    	EXTEep24M512Init();
    	if(1 == DEVEXTEEPGetReadyFlag())
		{
			msg.Data[0] = 0;
		}
		else
    	{
    		msg.Data[0] = 1;
    	}
    }

    /*外部FLASH状态*/
    if(1 == DEVEXTFLASHGetReadyFlag())
    {
        msg.Data[1] = 0;
    }
    else
    {
        /*外部FLASH异常则重新初始化*/
    	EXTFlashSST26Init();
    	if(1 == DEVEXTFLASHGetReadyFlag())
		{
			msg.Data[1] = 0;
		}
		else
    	{
    		msg.Data[1] = 1;
    	}
    }

    /*外部RTC状态*/
    if(1 == DEVEXTRTCGetReadyFlagAPI())
    {
        msg.Data[2] = 0;
    }
    else
    {
        /*外部RTC异常则重新初始化*/
    	DEVRTCRX8010Init();
    	if(1 == DEVEXTRTCGetReadyFlagAPI())
		{
			msg.Data[1] = 0;
		}
		else
    	{
    		msg.Data[1] = 1;
    	}
    }

    /*菊花链级联状态*/
    if((GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmAdcV) < 100)
		|| (GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmCycle) < 1000))
	{
        msg.Data[3] = 1;
	}
	else
	{
	    msg.Data[3] = 0;
	}

    /*菊花链通信状态*/
    if(DiagGetRcvAllSlaveFlagAPI() == 0)
	{
        msg.Data[4] = 1;
	}
	else
	{
	    msg.Data[4] = 0;
	}

    /*高压通信态*/
    msg.Data[5] = 1;

    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSSysTimeInfo0d(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS系统时间信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSSysTimeInfo0d(void)
{
	u16 data = 0;
    t_CANMSG msg;

    msg.ID = 0x0B0dF000;

    msg.Len = 8;

    msg.Data[0] = (gSysClock.year >> 8) & 0xff;
    msg.Data[1] = gSysClock.year & 0xff;

    msg.Data[2] = gSysClock.month;
    msg.Data[3] = gSysClock.day;
    msg.Data[4] = gSysClock.hour;
    msg.Data[5] = gSysClock.minute;

    data = gSysClock.second * 1000 + gSysClock.msecond;
    msg.Data[6] = (data >> 8) & 0xff;
    msg.Data[7] = data & 0xff;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSCurrCorrKBInfo0e(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS电流采样校准KB值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCurrCorrKBInfo0e(void)
{
    t_CANMSG msg;
    s8 data = 0;

    msg.ID = 0x0B0eF000;

    msg.Len = 8;

    /*霍尔电流*/
    data = (s8)(CorrGetCurrCorrKBValue(eOCCorr_HalOutK) * 10 / OC_CORR_BLOWUP);
    msg.Data[0] = data & 0xff;
    data = (s8)(CorrGetCurrCorrKBValue(eOCCorr_HalOutB) * 10 / OC_CORR_BLOWUP);
    msg.Data[1] = data & 0xff;

    /*分流器充电电流*/
    data = (s8)(CorrGetCurrCorrKBValue(eOCCorr_ShuChgK) * 10 / OC_CORR_BLOWUP);
    msg.Data[2] = data & 0xff;
    data = (s8)(CorrGetCurrCorrKBValue(eOCCorr_ShuChgB) * 10 / OC_CORR_BLOWUP);
    msg.Data[3] = data & 0xff;

    /*分流器放电电流*/
    data = (s8)(CorrGetCurrCorrKBValue(eOCCorr_ShuDhgK) * 10 / OC_CORR_BLOWUP);
    msg.Data[4] = data & 0xff;
    data = (s8)(CorrGetCurrCorrKBValue(eOCCorr_ShuDhgB) * 10 / OC_CORR_BLOWUP);
    msg.Data[5] = data & 0xff;

    /*电池总压*/
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_SumVK) * 10 / HV_CORR_BLOWUP);
    msg.Data[6] = data & 0xff;
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_SumVB) * 10 / HV_CORR_BLOWUP);
    msg.Data[7] = data & 0xff;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSVoltCorrKBInfo0f(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS高压采样校准KB值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSVoltCorrKBInfo0f(void)
{
    s8 data = 0;
    t_CANMSG msg;

    msg.ID = 0x0B0fF000;

    msg.Len = 8;

    /*预充电压*/
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_PreVK) * 10 / HV_CORR_BLOWUP);
    msg.Data[0] = data & 0xff;
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_PreVB) * 10 / HV_CORR_BLOWUP);
    msg.Data[1] = data & 0xff;

    /*负载电压*/
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_LoaVK) * 10 / HV_CORR_BLOWUP);
    msg.Data[2] = data & 0xff;
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_LoaVB) * 10 / HV_CORR_BLOWUP);
    msg.Data[3] = data & 0xff;

    /*Link+电压*/
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_PosVK) * 10 / HV_CORR_BLOWUP);
    msg.Data[4] = data & 0xff;
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_PosVB) * 10 / HV_CORR_BLOWUP);
    msg.Data[5] = data & 0xff;

    /*Link-电压*/
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_NegVK) * 10 / HV_CORR_BLOWUP);
    msg.Data[6] = data & 0xff;
    data = (s8)(CorrGetVoltCorrKBValue(eHVCorr_NegVB) * 10 / HV_CORR_BLOWUP);
    msg.Data[7] = data & 0xff;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSCellVoltInfo10(u8 index)
** @Input     : index:报文索引
** @Output    : void
** @Function  : 回复BMS单体电压值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCellVoltInfo10(u8 index)
{
    u16 data = 0;
    t_CANMSG msg;

    if(index >= 0x10)
    {
        return;
    }

    msg.ID = 0x0B10F000 + ((u32)index << 16);

    msg.Len = 8;

    data = GetGCellVoltAPI(index * 4);
    msg.Data[0] = (data >> 8) & 0xff;
    msg.Data[1] = data & 0xff;

    data = GetGCellVoltAPI(index * 4 + 1);
    msg.Data[2] = (data >> 8) & 0xff;
    msg.Data[3] = data & 0xff;

    data = GetGCellVoltAPI(index * 4 + 2);
    msg.Data[4] = (data >> 8) & 0xff;
    msg.Data[5] = data & 0xff;

    data = GetGCellVoltAPI(index * 4 + 3);
    msg.Data[6] = (data >> 8) & 0xff;
    msg.Data[7] = data & 0xff;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSCellTempInfo20(u8 index)
** @Input     : index:报文索引
** @Output    : void
** @Function  : 回复BMS单体温度值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCellTempInfo20(u8 index)
{
    t_CANMSG msg;

    if(0 == index)
    {
		msg.ID = 0x0B20F000;

		msg.Len = 8;

		msg.Data[0] = GetGCellTempAPI(0) & 0xff;
		msg.Data[1] = GetGCellTempAPI(1) & 0xff;

		msg.Data[2] = GetGCellTempAPI(2) & 0xff;
		msg.Data[3] = GetGCellTempAPI(3) & 0xff;

		msg.Data[4] = GetGCellTempAPI(4) & 0xff;
		msg.Data[5] = GetGCellTempAPI(5) & 0xff;

		msg.Data[6] = GetGCellTempAPI(6) & 0xff;
		msg.Data[7] = GetGCellTempAPI(7) & 0xff;
    }
    else if(1 == index)
    {
		msg.ID = 0x0B21F000;

		msg.Len = 8;

		msg.Data[0] = GetGCellTempAPI(8) & 0xff;
		msg.Data[1] = GetGCellTempAPI(9) & 0xff;

		msg.Data[2] = GetGCellTempAPI(10) & 0xff;
		msg.Data[3] = GetGCellTempAPI(11) & 0xff;

		msg.Data[4] = GetGCellTempAPI(12) & 0xff;
		msg.Data[5] = GetGCellTempAPI(13) & 0xff;

		msg.Data[6] = GetGCellTempAPI(14) & 0xff;
		msg.Data[7] = GetGCellTempAPI(15) & 0xff;
    }
    else
    {
    	return;
    }

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSBalanTempInfo22(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS单体温度值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSBalanTempInfo22(void)
{
    t_CANMSG msg;

	msg.ID = 0x0B22F000;

	msg.Len = 8;

	//有均衡温度
	if(gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum] > 0)
	{
		msg.Data[0] = GetGBalanTempAPI(0) & 0xff;
		msg.Data[1] = GetGBalanTempAPI(1) & 0xff;

		msg.Data[2] = GetGBalanTempAPI(2) & 0xff;
		msg.Data[3] = GetGBalanTempAPI(3) & 0xff;

		msg.Data[4] = GetGBalanTempAPI(4) & 0xff;
		msg.Data[5] = GetGBalanTempAPI(5) & 0xff;

		msg.Data[6] = 0;
		msg.Data[7] = 0;
	}
	//没有均衡温度回复25℃确保直接检测通过
	else
	{
		msg.Data[0] = 25;
		msg.Data[1] = 25;

		msg.Data[2] = 25;
		msg.Data[3] = 25;

		msg.Data[4] = 25;
		msg.Data[5] = 25;

		msg.Data[6] = 0;
		msg.Data[7] = 0;
	}

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSBalanStateInfo23(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS单体温度值信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSBalanStateInfo23(void)
{
    t_CANMSG msg;

	msg.ID = 0x0B23F000;

	msg.Len = 8;

	msg.Data[0] = GetGDhgBalanStateAPI(0) & 0xff;
	msg.Data[1] = (GetGDhgBalanStateAPI(0) >> 8) & 0xff;

	msg.Data[2] = GetGDhgBalanStateAPI(1) & 0xff;
	msg.Data[3] = (GetGDhgBalanStateAPI(1) >> 8) & 0xff;

	msg.Data[4] = GetGDhgBalanStateAPI(2) & 0xff;
	msg.Data[5] = (GetGDhgBalanStateAPI(2) >> 8) & 0xff;

	msg.Data[6] = 0;
	msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSBarCodeInfo2d(u8 num)
** @Input     : num:查询帧数 0:所有帧
** @Output    : void
** @Function  : 回复BMS条码信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSBarCodeInfo2d(u8 num)
{
    t_CANMSG msg;

	msg.ID = 0x0B2dF000;

	msg.Len = 8;

	/*查询所有帧或仅第1帧*/
	if((0 == num) || (1 == num))
	{
		msg.Data[0] = 1;

		/*(高字符先发[高字符在低字节])*/
		msg.Data[1] = gCANSerialNum_2[0];

		msg.Data[2] = gCANSerialNum_2[1];
		msg.Data[3] = gCANSerialNum_2[2];

		msg.Data[4] = gCANSerialNum_2[3];
		msg.Data[5] = gCANSerialNum_2[4];

		msg.Data[6] = gCANSerialNum_2[5];
		msg.Data[7] = gCANSerialNum_2[6];

		(void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
	}

	/*查询所有帧或仅第2帧*/
	if((0 == num) || (2 == num))
	{
		msg.Data[0] = 2;

		/*(高字符先发[高字符在低字节])*/
		msg.Data[1] = gCANSerialNum_2[7];

		msg.Data[2] = gCANSerialNum_2[8];
		msg.Data[3] = gCANSerialNum_2[9];

		msg.Data[4] = gCANSerialNum_2[10];
		msg.Data[5] = gCANSerialNum_2[11];

		msg.Data[6] = gCANSerialNum_2[12];
		msg.Data[7] = gCANSerialNum_2[13];

		(void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
	}

	/*查询所有帧或仅第3帧*/
	if((0 == num) || (3 == num))
	{
		msg.Data[0] = 3;

		/*(高字符先发[高字符在低字节])*/
		msg.Data[1] = gCANSerialNum_2[14];

		msg.Data[2] = gCANSerialNum_2[15];
		msg.Data[3] = gCANSerialNum_2[16];

		msg.Data[4] = gCANSerialNum_2[17];
		msg.Data[5] = gCANSerialNum_2[18];

		msg.Data[6] = gCANSerialNum_2[19];
		msg.Data[7] = gCANSerialNum_2[20];

		(void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
	}

	/*查询所有帧或仅第4帧*/
	if((0 == num) || (4 == num))
	{
		msg.Data[0] = 4;

		/*(高字符先发[高字符在低字节])*/
		msg.Data[1] = gCANSerialNum_2[21];

		msg.Data[2] = gCANSerialNum_2[22];
		msg.Data[3] = 0;

		msg.Data[4] = 0;
		msg.Data[5] = 0;

		msg.Data[6] = 0;
		msg.Data[7] = 0;

		(void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
	}
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSVersionInfo2e(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS版本号信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSVersionInfo2e(void)
{
    t_CANMSG msg;

	msg.ID = 0x0B2eF000;

	msg.Len = 8;

	if(eMODE_DEBUG == GetGWorkModeAPI())
	{
		msg.Data[0] = 1;
	}
	else
	{
		msg.Data[0] = 0;
	}

	msg.Data[1] = (gCANDevInfo_1[2] >> 8) & 0xff;
	msg.Data[2] = gCANDevInfo_1[2] & 0xff;

	msg.Data[3] = (gCANDevInfo_1[3] >> 8) & 0xff;
	msg.Data[4] = gCANDevInfo_1[3] & 0xff;

	msg.Data[5] = gGHardPara_104[eGHardPara104_BmuSWVerHH];
	msg.Data[6] = 0;
	msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void ReplyUpBMSCorrStateInfo2f(void)
** @Input     : void
** @Output    : void
** @Function  : 回复BMS采样通道校准状态信息
** @The notes : CAN接收到查询命令时调用
**===============================================================================================*/
static void ReplyUpBMSCorrStateInfo2f(void)
{
    t_CANMSG msg;

	msg.ID = 0x0B2fF000;

	msg.Len = 8;

	/*无校准*/
	if(eCORR_INVALID == gGCalibrResult_83[eCorrRes83_CorrType])
	{
		msg.Data[0] = 0;
	}
	/*电流零点校准*/
	else if(eCORR_HANDZRO == gGCalibrResult_83[eCorrRes83_CorrType])
	{
		msg.Data[0] = 1;
	}
	/*霍尔校准*/
	else if((eCORR_HALOUTS == gGCalibrResult_83[eCorrRes83_CorrType]) || (eCORR_HALOUTD == gGCalibrResult_83[eCorrRes83_CorrType]))
	{
		msg.Data[0] = 2;
	}
	/*分流器充电校准*/
	else if((eCORR_SHUCHGS == gGCalibrResult_83[eCorrRes83_CorrType]) || (eCORR_SHUCHGD == gGCalibrResult_83[eCorrRes83_CorrType]))
	{
		msg.Data[0] = 3;
	}
	/*分流器放电校准*/
	else if((eCORR_SHUDHGS == gGCalibrResult_83[eCorrRes83_CorrType]) || (eCORR_SHUDHGD == gGCalibrResult_83[eCorrRes83_CorrType]))
	{
		msg.Data[0] = 4;
	}
	/*总压校准*/
	else if((eCORR_SUMVOLS == gGCalibrResult_83[eCorrRes83_CorrType]) || (eCORR_SUMVOLD == gGCalibrResult_83[eCorrRes83_CorrType]))
	{
		msg.Data[0] = 5;
	}
	/*负载电压校准*/
	else if((eCORR_PREVOLS == gGCalibrResult_83[eCorrRes83_CorrType]) || (eCORR_PREVOLD == gGCalibrResult_83[eCorrRes83_CorrType]))
	{
		msg.Data[0] = 6;
	}
	/*Link电压校准*/
	else if((eCORR_LINVOLS == gGCalibrResult_83[eCorrRes83_CorrType]) || (eCORR_LINVOLD == gGCalibrResult_83[eCorrRes83_CorrType]))
	{
		msg.Data[0] = 7;
	}
	/*其他*/
	else
	{
		msg.Data[0] = gGCalibrResult_83[eCorrRes83_CorrType] + 100;
	}

	msg.Data[1] = gGCalibrResult_83[eCorrRes83_CorrState];

	if(2 == gGCalibrResult_83[eCorrRes83_CorrState])
	{
		msg.Data[2] = 1;
	}
	else if(4 == gGCalibrResult_83[eCorrRes83_CorrState])
	{
		msg.Data[2] = 2;
	}
	else
	{
		msg.Data[2] = 0;
	}

    (void)BSPCANSendQin(eCAN0, &msg);		//向CAN0通道发送
}

/*=================================================================================================
** @Name      : static void RcvFrockCtrlBMSModeCmdDeal30(t_CANMSG *msg)
** @Input     : msg:接收到的控制报文
** @Output    : void
** @Function  : 接收工装控制BMS工作模式命令处理
** @The notes : CAN接收到控制命令时调用
**===============================================================================================*/
static void RcvFrockCtrlBMSModeCmdDeal30(t_CANMSG *msg)
{
	if(0 == msg->Data[0])
	{
		CANSetMsgCycleTxEnAPI(1);
		TacSetGroupWorkModeAPI(eMODE_AUTO);
	}
	else
	{
		CANSetMsgCycleTxEnAPI(0);
		TacSetGroupWorkModeAPI(eMODE_DEBUG);

		/*切换超级用户权限*/
		gGAuthorCtrl_85[0] = AUTHORITY_NAME3;
		gGAuthorCtrl_85[1] = AUTHORITY_PWORD3;
		RcvSysAuthorityCmdAPI(eAuthP_Can0);
	}

	msg->ID = 0x0B30F000;
    (void)BSPCANSendQin(eCAN0, msg);		//向CAN0通道回复
}

/*=================================================================================================
** @Name      : static void RcvFrockCtrlBMSSwichCmdDeal31(t_CANMSG *msg)
** @Input     : msg:接收到的控制报文
** @Output    : void
** @Function  : 接收工装控制BMS接触器命令处理
** @The notes : CAN接收到控制命令时调用
**===============================================================================================*/
static void RcvFrockCtrlBMSSwichCmdDeal31(t_CANMSG *msg)
{
	u8 i = 0;

	/*非调试模式或权限不正确不执行*/
	if((eMODE_DEBUG != GetGWorkModeAPI())
		|| (eAuthL_Admin != GetSysNowAuthorityAPI(eAuthP_Can0)))
	{
		return;
	}

	for(i = 0; i < 8; i++)
	{
		if(0 == BitGet(msg->Data[1], i))
		{
			(void)DEVGPIOSetPortState(ePOut_DRVK0 + i, ePSTATE_OFF);
		}
		else
		{
			(void)DEVGPIOSetPortState(ePOut_DRVK0 + i, ePSTATE_ON);
		}
	}

	if(0 == BitGet(msg->Data[0], 0))
	{
		(void)DEVGPIOSetPortState(ePOut_DRVK8, ePSTATE_OFF);
	}
	else
	{
		(void)DEVGPIOSetPortState(ePOut_DRVK8, ePSTATE_ON);
	}

	if(0 == BitGet(msg->Data[2], 0))
	{
		(void)DEVGPIOSetPortState(ePOut_LDRVK0, ePSTATE_OFF);
	}
	else
	{
		(void)DEVGPIOSetPortState(ePOut_LDRVK0, ePSTATE_ON);
	}

	if(0 == BitGet(msg->Data[2], 1))
	{
		(void)DEVGPIOSetPortState(ePOut_LDRVK2, ePSTATE_OFF);
	}
	else
	{
		(void)DEVGPIOSetPortState(ePOut_LDRVK2, ePSTATE_ON);
	}

	if(0 == BitGet(msg->Data[6], 0))
	{
		(void)DEVGPIOSetPortState(ePOut_STADRY0, ePSTATE_OFF);
	}
	else
	{
		(void)DEVGPIOSetPortState(ePOut_STADRY0, ePSTATE_ON);
	}

	if(0 == BitGet(msg->Data[6], 1))
	{
		(void)DEVGPIOSetPortState(ePOut_CHGDRY1, ePSTATE_OFF);
	}
	else
	{
		(void)DEVGPIOSetPortState(ePOut_CHGDRY1, ePSTATE_ON);
	}

	if(0 == BitGet(msg->Data[6], 2))
	{
		(void)DEVGPIOSetPortState(ePOut_DHGDRY2, ePSTATE_OFF);
	}
	else
	{
		(void)DEVGPIOSetPortState(ePOut_DHGDRY2, ePSTATE_ON);
	}

	/*高压侧GND继电器*/
	if(0 == BitGet(msg->Data[7], 0))
	{
		;
	}
	else
	{
		;
	}

	msg->ID = 0x0B31F000;
    (void)BSPCANSendQin(eCAN0, msg);		//向CAN0通道回复
}

/*=================================================================================================
** @Name      : static void RcvFrockCtrlBMSTimeCmdDeal32(t_CANMSG *msg)
** @Input     : msg:接收到的控制报文
** @Output    : void
** @Function  : 接收工装控制BMS校时命令处理
** @The notes : CAN接收到控制命令时调用
**===============================================================================================*/
static void RcvFrockCtrlBMSTimeCmdDeal32(t_CANMSG *msg)
{
	/*非调试模式或权限不正确不执行*/
	if((eMODE_DEBUG != GetGWorkModeAPI())
		|| (eAuthL_Admin != GetSysNowAuthorityAPI(eAuthP_Can0)))
	{
		return;
	}

	gSysClock.year = ((u16)msg->Data[0] << 8) | msg->Data[1];
	gSysClock.month = msg->Data[2];
	gSysClock.day = msg->Data[3];
	gSysClock.hour = msg->Data[4];
	gSysClock.minute = msg->Data[5];
	gSysClock.second = (((u16)msg->Data[6] << 8) | msg->Data[7]) / 1000;
	gSysClock.msecond = (((u16)msg->Data[6] << 8) | msg->Data[7]) % 1000;

	msg->ID = 0x0B32F000;
    (void)BSPCANSendQin(eCAN0, msg);		//向CAN0通道回复
}

/*=================================================================================================
** @Name      : static void RcvFrockCtrlBMSCorrCmdDeal33(t_CANMSG *msg)
** @Input     : msg:接收到的控制报文
** @Output    : void
** @Function  : 接收工装控制BMS校准命令处理
** @The notes : CAN接收到控制命令时调用
**===============================================================================================*/
static void RcvFrockCtrlBMSCorrCmdDeal33(t_CANMSG *msg)
{
	s32 data = 0;

	/*非调试模式或权限不正确不执行*/
	if((eMODE_DEBUG != GetGWorkModeAPI())
		|| (eAuthL_Admin != GetSysNowAuthorityAPI(eAuthP_Can0)))
	{
		return;
	}

	/*电流校零*/
	if(1 == msg->Data[0])
	{
		gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_HANDZRO;
	}
	/*霍尔电流双点校准*/
	else if(2 == msg->Data[0])
	{
		gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_HALOUTD;
	}
	/*分流器充电电流双点校准*/
	else if(3 == msg->Data[0])
	{
		gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_SHUCHGD;
	}
	/*分流器充电电流双点校准*/
	else if(4 == msg->Data[0])
	{
		gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_SHUDHGD;
	}
	/*电池总压双点校准*/
	else if(5 == msg->Data[0])
	{
		gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_SUMVOLD;
	}
	/*电池总压双点校准*/
	else if(6 == msg->Data[0])
	{
		gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_PREVOLD;
	}

	if(0 != (((u16)msg->Data[1] << 8) | msg->Data[2]))
	{
		data = (s16)(((u16)msg->Data[1] << 8) | msg->Data[2]);

//		/*霍尔校准,单位0.01V*/
//		if(eCORR_HALOUTD == gGCalibrCtrl_89[eCorrCmd89_Type])
//		{
//			/*基准电压转化为基准电流,单位0.01A*/
//			if(data < 250)//充电
//			{
//				gGCalibrCtrl_89[eCorrCmd89_Value] = 25000 - data * 100;
//			}
//			else
//			{
//				gGCalibrCtrl_89[eCorrCmd89_Value] = data * 100 - 25000;
//			}
//		}
//		/*分流器充电校准,单位0.01mV*/
//		else if(eCORR_SHUCHGD == gGCalibrCtrl_89[eCorrCmd89_Type])
//		{
//			/*基准电压转化为基准电流,单位0.01A*/
//			//gGCalibrCtrl_89[eCorrCmd89_Value] = ABS(data, 0) * 300 / 75;
//			gGCalibrCtrl_89[eCorrCmd89_Value] = ABS(data, 0) * 4;
//		}
//		/*分流器放电校准,单位0.01mV*/
//		else if(eCORR_SHUDHGD == gGCalibrCtrl_89[eCorrCmd89_Type])
//		{
//			/*基准电压转化为基准电流,单位0.01A*/
//			//gGCalibrCtrl_89[eCorrCmd89_Value] = data * 300 / 75;
//			gGCalibrCtrl_89[eCorrCmd89_Value] = data * 4;
//		}
//		/*高压校准,单位0.1V*/
//		else
		{
			/*基准电压不用转换,单位0.1V*/
			gGCalibrCtrl_89[eCorrCmd89_Value] = data;
		}
	}

	msg->ID = 0x0B33F000;
    (void)BSPCANSendQin(eCAN0, msg);		//向CAN0通道回复
}

/*=================================================================================================
** @Name      : static void RcvFrockCtrlBMSRateCmdDeal34(t_CANMSG *msg)
** @Input     : msg:接收到的控制报文
** @Output    : void
** @Function  : 接收工装控制BMS通信波特率设置命令处理
** @The notes : CAN接收到控制命令时调用
**===============================================================================================*/
static void RcvFrockCtrlBMSRateCmdDeal34(t_CANMSG *msg)
{
	u16 temp = 0;

	/*非调试模式或权限不正确不执行*/
	if((eMODE_DEBUG != GetGWorkModeAPI())
		|| (eAuthL_Admin != GetSysNowAuthorityAPI(eAuthP_Can0)))
	{
		return;
	}

	/*CAN0 125K*/
	if(1 == msg->Data[0])
	{
		temp = 125;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN0Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN0Rate] = 125;
		}
	}
	/*CAN0 250K*/
	else if(2 == msg->Data[0])
	{
		temp = 250;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN0Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN0Rate] = 250;
		}
	}
	/*CAN0 500K*/
	else if(3 == msg->Data[0])
	{
		temp = 500;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN0Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN0Rate] = 500;
		}
	}
	/*CAN0 1M*/
	else if(4 == msg->Data[0])
	{
		temp = 1000;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN0Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN0Rate] = 1000;
		}
	}

	/*CAN1 125K*/
	if(1 == msg->Data[1])
	{
		temp = 125;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN1Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN1Rate] = 125;
		}
	}
	/*CAN1 250K*/
	else if(2 == msg->Data[1])
	{
		temp = 250;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN1Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN1Rate] = 250;
		}
	}
	/*CAN1 500K*/
	else if(3 == msg->Data[1])
	{
		temp = 500;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN1Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN1Rate] = 500;
		}
	}
	/*CAN1 1M*/
	else if(4 == msg->Data[1])
	{
		temp = 1000;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN1Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN1Rate] = 1000;
		}
	}

	/*CAN2 125K*/
	if(1 == msg->Data[2])
	{
		temp = 125;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN2Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN2Rate] = 125;
		}
	}
	/*CAN2 250K*/
	else if(2 == msg->Data[2])
	{
		temp = 250;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN2Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN2Rate] = 250;
		}
	}
	/*CAN2 500K*/
	else if(3 == msg->Data[2])
	{
		temp = 500;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN2Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN2Rate] = 500;
		}
	}
	/*CAN2 1M*/
	else if(4 == msg->Data[2])
	{
		temp = 1000;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_CAN2Rate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_CAN2Rate] = 1000;
		}
	}

	/*RS485-0 9600*/
	if(1 == msg->Data[3])
	{
		temp = 9600;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_RemoteRate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_RemoteRate] = 9600;
		}
	}
	/*RS485-0 19200K*/
	else if(2 == msg->Data[3])
	{
		temp = 19200;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_RemoteRate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_RemoteRate] = 19200;
		}
	}

	/*RS485-1 9600*/
	if(1 == msg->Data[4])
	{
		temp = 9600;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_LocalRate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_LocalRate] = 9600;
		}
	}
	/*RS485-1 19200*/
	else if(2 == msg->Data[4])
	{
		temp = 19200;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_LocalRate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_LocalRate] = 19200;
		}
	}

	/*RS485-2 9600*/
	if(1 == msg->Data[5])
	{
		temp = 9600;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_MonitRate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_MonitRate] = 9600;
		}
	}
	/*RS485-2 19200*/
	else if(2 == msg->Data[5])
	{
		temp = 19200;

		/*写入成功*/
		if(TRUE == ParaWriteStoreSystemHWParaInfo(eSHWPara101_MonitRate, &temp, 1))
		{
			gSysHigParaRO_101[eSHWPara101_MonitRate] = 19200;
		}
	}

	msg->ID = 0x0B34F000;
    (void)BSPCANSendQin(eCAN0, msg);		//向CAN0通道回复
}

/*=================================================================================================
** @Name      : static void RcvFrockCtrlBMSBarCodeCmdDeal35(t_CANMSG *msg)
** @Input     : msg:接收到的控制报文
** @Output    : void
** @Function  : 接收工装控制BMS通信波特率设置命令处理
** @The notes : CAN接收到控制命令时调用
**===============================================================================================*/
static void RcvFrockCtrlBMSBarCodeCmdDeal35(t_CANMSG *msg)
{
	u8 i = 0;
	u8 flag = 0;
	u8 index = 0;

	/*非调试模式或权限不正确不执行*/
	if((eMODE_DEBUG != GetGWorkModeAPI())
		|| (eAuthL_Admin != GetSysNowAuthorityAPI(eAuthP_Can0)))
	{
		return;
	}

	/*4帧*/
	if((1 <= msg->Data[0]) && (msg->Data[0] <= 4))
	{
		for(i = 0; i < 7; i++)
		{
			/*(高字符先发[高字符在低字节])*/
			index = (msg->Data[0] * 7) - 7 + i;

			/*只取序列号长度*/
			if(index < CANINDEX2_LEN)
			{
				/*写入成功*/
				if(TRUE == ParaWriteStoreDevSerial(1 + index, &msg->Data[1 + i], 1))
				{
					gCANSerialNum_2[index] = msg->Data[1 + i];

					/*记录标志*/
					if((CANINDEX2_LEN - 1) == index)
					{
						flag = 0x5a;
						(void)ParaWriteStoreDevSerial(0, &flag, 1);
					}
				}
				/*写入失败*/
				else
				{
					/*清除显示值*/
					msg->Data[1 + i] = 0xff;
					gCANSerialNum_2[index] = 0xff;
				}
			}
		}
	}

	msg->ID = 0x0B35F000;
    (void)BSPCANSendQin(eCAN0, msg);		//向CAN0通道回复
}

/*=================================================================================================
** @Name      : static void RcvFrockCtrlBMSBalanCmdDeal36(t_CANMSG *msg)
** @Input     : msg:接收到的控制报文
** @Output    : void
** @Function  : 接收工装控制BMS均衡控制命令处理
** @The notes : CAN接收到控制命令时调用
**===============================================================================================*/
static void RcvFrockCtrlBMSBalanCmdDeal36(t_CANMSG *msg)
{
	/*非调试模式或权限不正确不执行*/
	if((eMODE_DEBUG != GetGWorkModeAPI())
		|| (eAuthL_Admin != GetSysNowAuthorityAPI(eAuthP_Can0)))
	{
		return;
	}

	gGDebugCtrl_88[eDbgCmd88_DbgType] = 0;
	gGDebugCtrl_88[eDbgCmd88_BMUid] = 1;
	gGDebugCtrl_88[eDbgCmd88_BSUid] = msg->Data[0];
	gGDebugCtrl_88[eDbgCmd88_BalType] = 2;
	gGDebugCtrl_88[eDbgCmd88_BalStat] = 2;
	gGDebugCtrl_88[eDbgCmd88_BalCellL] = ((u16)msg->Data[1] << 8) | msg->Data[2];
	gGDebugCtrl_88[eDbgCmd88_BalCellH] = msg->Data[3];
	gGDebugCtrl_88[eDbgCmd88_Affirm] = 1;

	/*执行调试命令*/
	RcvGroupDebugAffirmCmdAPI(eAuthP_Can0 + eCAN0);

	/*调试失败*/
	if(eDBGRES_SUCCE != gGDebugResult_82[eDbgRes82_DbgResult])
	{
		msg->Data[1] = 0;
		msg->Data[2] = 0;
	}

	msg->ID = 0x0B36F000;
    (void)BSPCANSendQin(eCAN0, msg);		//向CAN0通道回复
}

/*=================================================================================================
** @Name      : static void RcvFrockCtrlBMSSlaveTypeCmdDeal37(t_CANMSG *msg)
** @Input     : msg:接收到的控制报文
** @Output    : void
** @Function  : 接收工装控制BMS从控类型命令处理
** @The notes : CAN接收到控制命令时调用
**===============================================================================================*/
static void RcvFrockCtrlBMSSlaveTypeCmdDeal37(t_CANMSG *msg)
{
	if(msg->Data[0] <= 4)
	{
		/*写入成功*/
		if(TRUE == ParaWriteStoreGroupHWParaInfo(eGHardPara104_BmuSWVerHH, &msg->Data[0], 1))
		{
			/*显示最新值*/
			gGHardPara_104[eGHardPara104_BmuSWVerHH] = msg->Data[0];
		}
	}

	msg->ID = 0x0B37F000;
    (void)BSPCANSendQin(eCAN0, msg);		//向CAN0通道回复
}
#endif
