/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANInterface.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : CAN通信包用户接口处理
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "Authority.h"
#include "CellAFESamp.h"
#include "DiagInterface.h"
#include "RcdDisplayUser.h"
#include "GroupMsg.h"
#include "CANTabOper.h"
#include "CANTxCycle.h"
#include "CANUser.h"
#include "CANInterface.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
t_CANDevTab gCanDevTable[CAN_DEVTAB_NUM] = {0};                     /*CAN通信设备表*/
t_CANTxRcvTab gCanTxRcvTable[CAN_TRTAB_NUM] = {0};                  /*CAN通信收发处理表*/
t_CANTxCycTab gCanTxCycTable[CAN_TX_ALL_NUM] = {0};                 /*CAN通信周期发送处理表*/

t_CANLibIndexMap gCanIndexMap = {0};                                /*CAN通信包主索引映射信息*/

extern u16 gCANDevInfo_1[];                                         /*设备信息*/
extern u8 gCANBaseDef_3[];                                          /*通信包基本配置信息*/
extern u8 gCANTxCelDef_14[];                                        /*主动更新单体配置表*/
extern u16 gGBmuGenPara_102[];                                      /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];                                   /*主控高级参数值*/
extern u8 gGHardPara_104[];                                         /*主控硬件参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void BSPUserCfgInit(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信包所有初始化
** @The notes : 用户必须初始化调用
**===============================================================================================*/
void CANLibAllInit_API(void)
{
    u8 i = 0;

    /*通信口用户初始化*/
    CANCommuPortUserInit();

    /*通信包用户定义参数初始化*/
    gCanIndexMap.map90Index = CANMAP90_INDEX;
    gCanIndexMap.map91Index = CANMAP91_INDEX;
    gCanIndexMap.map92Index = CANMAP92_INDEX;
    gCanIndexMap.map93Index = CANMAP93_INDEX;
    gCanIndexMap.map94Index = CANMAP94_INDEX;
    gCanIndexMap.map95Index = CANMAP95_INDEX;

    gCanIndexMap.map100Index = CANMAP100_INDEX;
    gCanIndexMap.map101Index = CANMAP101_INDEX;
    gCanIndexMap.map102Index = CANMAP102_INDEX;
    gCanIndexMap.map103Index = CANMAP103_INDEX;
    gCanIndexMap.map104Index = CANMAP104_INDEX;

    gCanIndexMap.index90Type = CAN_DT_U16;
    gCanIndexMap.index91Type = CAN_DT_U16;
    gCanIndexMap.index92Type = CAN_DT_U16;
    gCanIndexMap.index93Type = CAN_DT_U16;
    gCanIndexMap.index94Type = CAN_DT_S8;
    gCanIndexMap.index95Type = CAN_DT_S8;

    gCanIndexMap.index100Type = CAN_DT_U16;
    gCanIndexMap.index101Type = CAN_DT_U16;
    gCanIndexMap.index102Type = CAN_DT_U16;
    gCanIndexMap.index103Type = CAN_DT_U16;
    gCanIndexMap.index104Type = CAN_DT_U8;

    /*通信包配置参数初始化*/
    gCANDevInfo_1[eCANDev1_Addr] = gGBmuGenPara_102[eBmuGenPara102_BmuDev];
    gCANDevInfo_1[eCANDev1_HW] = ((u16)DEVGPIOGetHWVersion() << 8) |(BMU_HW_VER & 0xff);

    /*单体信息发送长度参数初始化*/
    for(i = 0; i < gCANBaseDef_3[eCANRBas3_TxCelNum]; i++)
    {
        /*单体充电均衡状态信息*/
        if(90 == gCANTxCelDef_14[1 + (8 * i)])
        {
        	/*有主动均衡*/
        	if(gGHardPara_104[eGHardPara104_BalCirc] > 0)
        	{
        		/*根据电池节数发送子索引长度*/
                gCANTxCelDef_14[3 + (8 * i)] = (gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] + 15) / 16;/*子索引长度*/
        	}
        	/*无主动均衡*/
        	else
        	{
            	/*不发充电送衡状态*/
                gCANTxCelDef_14[5 + (8 * i)] = 0;/*发送周期清零*/
                gCANTxCelDef_14[6 + (8 * i)] = 0;/*发送周期清零*/
        	}
        }
        /*单体放电均衡状态信息*/
        else if(91 == gCANTxCelDef_14[1 + (8 * i)])
        {
            /*根据电池节数发送子索引长度*/
            gCANTxCelDef_14[3 + (8 * i)] = (gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] + 15) / 16;/*子索引长度*/
        }
        /*单体电压信息1*/
        else if(92 == gCANTxCelDef_14[1 + (8 * i)])
        {
        	/*电池节数超过最大子索引*/
        	if(gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] > 240)
        	{
                /*发送最大长度*/
                gCANTxCelDef_14[3 + (8 * i)] = 240;/*子索引长度*/
        	}
        	/*电池节数小于最大子索引*/
        	else
        	{
                /*根据电池节数发送子索引长度*/
                gCANTxCelDef_14[3 + (8 * i)] = gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum];/*子索引长度*/
        	}
        }
        /*单体电压信息2*/
        else if(93 == gCANTxCelDef_14[1 + (8 * i)])
        {
        	/*电池节数超过最大子索引*/
        	if(gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] > 240)
        	{
                /*发送最大长度*/
                gCANTxCelDef_14[3 + (8 * i)] = gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] - 240;/*子索引长度*/
        	}
        	/*电池节数没有放到93索引*/
        	else
        	{
            	/*不发93索引电压信息态*/
                gCANTxCelDef_14[5 + (8 * i)] = 0;/*发送周期清零*/
                gCANTxCelDef_14[6 + (8 * i)] = 0;/*发送周期清零*/
        	}
        }
        /*单体温度信息1*/
        else if(94 == gCANTxCelDef_14[1 + (8 * i)])
        {
        	/*温度个数超过最大子索引*/
        	if((gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum]) > 240)
        	{
                /*发送最大长度*/
                gCANTxCelDef_14[3 + (8 * i)] = 240;/*子索引长度*/
        	}
        	/*温度个数小于最大子索引*/
        	else
        	{
                /*根据电池节数发送子索引长度*/
                gCANTxCelDef_14[3 + (8 * i)] = gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum];/*子索引长度*/
        	}
        }
        /*单体温度信息2*/
        else if(95 == gCANTxCelDef_14[1 + (8 * i)])
        {
        	/*温度个数超过最大子索引*/
        	if((gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum]) > 240)
        	{
                /*发送最大长度*/
                gCANTxCelDef_14[3 + (8 * i)] = (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum]) - 240;/*子索引长度*/
        	}
        	/*温度个数没有放到95索引*/
        	else
        	{
            	/*不发95索引电压信息态*/
                gCANTxCelDef_14[5 + (8 * i)] = 0;/*发送周期清零*/
                gCANTxCelDef_14[6 + (8 * i)] = 0;/*发送周期清零*/
        	}
        }
    }

    /*通信包设备表初始化*/
    CANDeviceTableInit();

    /*通信包收发通信表初始化*/
    if(gCanTxRcvTable != NULL)
    {
    	for(i = 0; i < gCANBaseDef_3[eCANRBas3_TRTabNum]; i++)
    	{
    		/*清空所有收发通信表*/
    		CANClrTxRcvTabInfo(i);
    	}
    }

    /*通信包周期发送表初始化*/
    CANTxCycleTabInit();
}

/*=================================================================================================
** @Name      : u8 CANRcvShieldDeal_Hook(u32 canID, u8 canNum)
** @Input     : canID:报文ID  canNum:CAN通道号
** @Output    : TURE:需屏蔽 FALSE:不需屏蔽
** @Function  : 接收到需屏蔽的报文用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
u8 CANRcvShieldDeal_Hook(u32 canID, u8 canNum)
{
    /*广播报文可能不需要接收处理*/
	if(0xff == ((canID >> 8) & 0xff))
	{
		/*同级BMU和从控信息不处理*/
		if(/*((canID & 0xff) >= BMU_BAS_DEVADDR) && */((canID & 0xff) < SMU_BAS_DEVADDR))
		{
			return(TRUE);
		}
	}

	return(FALSE);
}

/*=================================================================================================
** @Name      : void CANRcvVerUserDeal_Hook(u8 saddr, u8 canNum)
** @Input     : saddr:源地址  canNum:CAN通道号
** @Output    : void
** @Function  : 接收到校时报文后用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
void CANRcvVerUserDeal_Hook(u8 saddr, u8 canNum)
{
    /*将时间信息更新到系统时钟*/
	SysClockExtVerifyAPI();
}

/*=================================================================================================
** @Name      : void CANRcvBreUserDeal_Hook(u8 saddr, u8 canNum)
** @Input     : saddr:源地址  canNum:CAN通道号
** @Output    : void
** @Function  : 接收到心跳报文后用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
void CANRcvBreUserDeal_Hook(u8 saddr, u8 canNum)
{
    /*通信诊断计数*/
	DiagRcvDevMsgAddAPI(saddr);
	DiagRcvEcuRepCheckAPI(saddr);
}

/*=================================================================================================
** @Name      : u8 CANRcvRupUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : saddr:源地址  mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收到请求修改报文后用户处理
** @The notes : 用户需要时补充完善,对相应索引值进行处理
**===============================================================================================*/
u8 CANRcvRupUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
{
    /*通信诊断计数*/
	DiagRcvDevMsgAddAPI(saddr);
	DiagRcvEcuRepCheckAPI(saddr);

    /*非上层设备控制不执行*/
	if(saddr <= gCANDevInfo_1[eCANDev1_Addr])
	{
		/*权限非法*/
		return(eCANERRREAS_AUTHERR);
	}

    /*执行请求修改命令*/
    if(2 == mIndex)                     /*写设备序列号*/
    {
    	/*执行写设备序列号处理*/
    	return(RcvCanDeviceSerialCfgDeal(sIndex, sIndexNum, canNum));
    }
    /*执行请求修改命令*/
    else if(22 == mIndex)               /*写参数批量配置值*/
    {
    	/*执行写参数批量配置写数据处理*/
    	return(RcvCanDnloadParaWRDeal(sIndex, sIndexNum, canNum));
    }
    /*执行请求修改命令*/
    else if(39 == mIndex)               /*切换历史记录日期*/
    {
        /*执行设置历史记录读写日期处理*/
    	return(RcvCanHisRecordRWDateDeal(sIndex, sIndexNum, canNum));
    }
    /*执行请求修改命令*/
    else if(85 == mIndex)               /*权限密码*/
    {
    	/*执行权限密码处理*/
    	return(RcvCanGroupPasswordDeal(sIndex, sIndexNum, canNum));
    }
    else if(86 == mIndex)               /*系统控制*/
    {
    	/*执行系统控制处理*/
    	return(RcvCanGroupCtrlCmdDeal(saddr, sIndex, sIndexNum, canNum));
    }
    else if(87 == mIndex)               /*用户控制*/
    {
    	/*执行用户控制处理*/
    	return(RcvCanUserCtrlCmdDeal(sIndex, sIndexNum, canNum));
    }
    else if(88 == mIndex)               /*系统调试*/
    {
    	/*执行系统调试处理*/
    	return(RcvCanGroupDebugCmdDeal(sIndex, sIndexNum, canNum));
    }
    else if(105 == mIndex)              /*系统通用参数设置*/
    {
    	/*执行系统通用参数设置处理*/
    	return(RcvCanSystemGenParaCfgDeal(sIndex, sIndexNum, canNum));
    }
    else if(106 == mIndex)              /*系统硬件参数设置*/
    {
    	/*执行系统硬件参数设置处理*/
    	return(RcvCanSystemHWParaCfgDeal(sIndex, sIndexNum, canNum));
    }
    else if(107 == mIndex)              /*主控通用参数设置*/
    {
    	/*执行主控通用参数设置处理*/
    	return(RcvCanGroupGenParaCfgDeal(sIndex, sIndexNum, canNum));
    }
    else if(108 == mIndex)              /*主控高级参数设置*/
    {
    	/*执行主控高级参数设置处理*/
    	return(RcvCanGroupHigParaCfgDeal(sIndex, sIndexNum, canNum));
    }
    else if(109 == mIndex)              /*主控硬件参数设置*/
    {
    	/*执行主控硬件参数设置处理*/
    	return(RcvCanGroupHWParaCfgDeal(sIndex, sIndexNum, canNum));
    }

    return(eCANERRREAS_INDEXERR);
}

/*=================================================================================================
** @Name      : void CANRcvReqUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : saddr:源地址  mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : void
** @Function  : 接收到请求查询报文后(查询回复前)用户处理
** @The notes : 用户需要时补充完善,更新相应索引值(不自动跟新的值)
**===============================================================================================*/
void CANRcvReqUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
{
    /*查询系统时钟当前时间*/
	if(0 == mIndex)
	{
	    /*更新系统时钟显示*/
		SysClockDisplayAPI();
	}
    /*查询主控设备当前条码序列号*/
	else if(2 == mIndex)
	{
		if(0 == sIndex)
		{
			/*读设备序列号到显示区域*/
			SysDevSerialDispayAPI();
		}
	}
    /*查询故障或事件记录状态信息*/
	else if(30 == mIndex)
	{
		if(0 == sIndex)
		{
			/*更新状态(数目)信息*/
			ErrEvntRcdUpdateStateAPI();
		}
	}
    /*查询故障记录详细信息*/
	else if(31 == mIndex)
	{
		if(0 == sIndex)
		{
			/*更新显示区数据记录详细信息*/
			RcvCanErrRcdReadInfoDeal(sIndex, sIndexNum, canNum);
		}
	}
    /*查询事件记录详细信息*/
	else if(32 == mIndex)
	{
		if(0 == sIndex)
		{
			/*更新显示区数据记录详细信息*/
			RcvCanEventRcdReadInfoDeal(sIndex, sIndexNum, canNum);
		}
	}
    /*查询故障录波详细信息*/
	else if(33 == mIndex)
	{
		if(0 == sIndex)
		{
			/*更新显示区数据记录详细信息*/
			RcvCanWaveRcdReadInfoDeal(sIndex, sIndexNum, canNum);
		}
	}
    /*查询DOD记录详细信息*/
	else if(34 == mIndex)
	{
		if(0 == sIndex)
		{
			/*更新显示区数据记录详细信息*/
			RcvCanDodRcdReadInfoDeal(sIndex, sIndexNum, canNum);
		}
	}
    /*查询数据记录状态信息*/
	else if((35 == mIndex) || (36 == mIndex))
	{
		if(0 == sIndex)
		{
			/*更新状态(数目)信息*/
			DataRcdUpdateRcdStateAPI();
		}
	}
    /*查询数据记录详细信息*/
	else if(37 == mIndex)
	{
		if(0 == sIndex)
		{
			/*更新显示区数据记录详细信息*/
			RcvCanDataRcdDisplayInfoDeal(sIndex, sIndexNum, canNum);
		}
	}
}

/*=================================================================================================
** @Name      : void CANRcvCycUpUserDeal_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域 DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收到周期更新报文后用户处理
** @The notes : 用户需要时补充完善,处理接收周期发送的长帧、单帧、单体和参数信息
**===============================================================================================*/
void CANRcvCycUpUserDeal_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 saddr = 0;

	saddr = ID & 0xff;

    /*通信诊断计数*/
	DiagRcvDevMsgAddAPI(saddr);
	DiagRcvEcuRepCheckAPI(saddr);
}

/*=================================================================================================
** @Name      : void CANSendMsgUser_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域 DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信包发送报文处理
** @The notes : 用户必须补充完善
**===============================================================================================*/
void CANSendMsgUser_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 i = 0;
	t_CANMSG msg;

	if(DLC > 8)
	{
		return;
	}

	msg.ID = ID;
	msg.Len = DLC;

	for(i = 0; i < DLC; i++)
	{
		msg.Data[i] = pdata[i];
	}

	//不满8byte则补0满8byte
	msg.Len = 8;
	for(i = DLC; i < 8; i++)
	{
		msg.Data[i] = 0;
	}

    /*发送CAN报文*/
	BSPCANSendQin((e_CANNUM)canNum, &msg);
}

/*=================================================================================================
** @Name      : u8 CANTxCycUpUserEnState_Hook(e_CANFunCode funCode, u8 mIndex, u8 canNum)
** @Input     : funCode:功能码  mIndex:主索引号 canNum:CAN通道号
** @Output    : 用户允许发送状态 TRUE:允许 FALSE:禁止
** @Function  : 判断用户允许发送指定主索引报文状态
** @The notes : 用户需要时补充完善,对指定主索引在指定状态下才允许发送(比如调试信息)
**===============================================================================================*/
u8 CANTxCycUpUserEnState_Hook(e_CANFunCode funCode, u8 mIndex, u8 canNum)
{
	static u8 sDebugInfoTime = 10;

	/*调试信息(长帧)*/
	if((eCANFUN_UPM == funCode) && ((82 == mIndex) || (83 == mIndex)))
	{
		/*调试模式允许发送*/
		if(eMODE_DEBUG == GetGWorkModeAPI())
		{
			sDebugInfoTime = 10;
			return(TRUE);
		}
		/*非调试模式*/
		else
		{
			/*非调试模式允许发送完10次*/
			if(sDebugInfoTime > 0)
			{
				sDebugInfoTime--;
				return(TRUE);
			}
			/*非调试模式且已发送完10次发送*/
			else
			{
				return(FALSE);
			}
		}
	}
	/*单体电压信息(单体帧)*/
	else if((eCANFUN_UPC == funCode) && ((92 == mIndex) || (93 == mIndex)))
	{
		/*采样完成1遍*/
		if(1 == GetCellVSampFinFlagAPI())
		{
			return(TRUE);
		}
		/*采样未完成1遍*/
		else
		{
			/*不发送*/
			return(FALSE);
		}
	}
	/*单体温度信息(单体帧)*/
	else if((eCANFUN_UPC == funCode) && ((94 == mIndex) || (95 == mIndex)))
	{
		/*采样完成1遍*/
		if(1 == GetCellTSampFinFlagAPI())
		{
			return(TRUE);
		}
		/*采样未完成1遍*/
		else
		{
			/*不发送*/
			return(FALSE);
		}
	}
	/*单体内阻信息(长帧)*/
	else if((eCANFUN_UPM == funCode) && ((96 == mIndex) || (97 == mIndex)))
	{
		/*采样完成1遍*/
		if(1 == GetCellRSampFinFlagAPI())
		{
			return(TRUE);
		}
		/*采样未完成1遍*/
		else
		{
			/*不发送*/
			return(FALSE);
		}
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : t_CANMapTrans CANIndexMapTransUser_Hook(e_CANFunCode funCode, u8 mIndex, u8 DLC)
** @Input     : funCode:功能码  mIndex:映射主索引号 DLC:报文长度
** @Output    : void
** @Function  : CAN通信包将映射报文信息转换成真实信息
** @The notes : 用户必须补充完善,单体信息和参数信息报文的映射
**===============================================================================================*/
t_CANMapTrans CANIndexMapTransUser_Hook(e_CANFunCode funCode, u8 mIndex, u8 DLC)
{
    /*根据报文映射信息[功能码/主索引/报文长度]转换成真实索引信息*/
	t_CANMapTrans canMapInfo = {0};

    //周期主动更新单体报文
    if(eCANFUN_UPC == funCode)
    {
    	/*单体充电均衡状态信息*/
    	if(mIndex < gCanIndexMap.map91Index)
    	{
    		canMapInfo.realMIndex = 90;
    		canMapInfo.realSIndex = mIndex * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index90Type;
    	}
    	/*单体放电均衡状态信息*/
    	else if(mIndex < gCanIndexMap.map92Index)
    	{
    		canMapInfo.realMIndex = 91;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map91Index) * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index91Type;
    	}
    	/*单体电压信息1*/
    	else if(mIndex < gCanIndexMap.map93Index)
    	{
    		canMapInfo.realMIndex = 92;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map92Index) * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index92Type;
    	}
    	/*单体电压信息2*/
    	else if(mIndex < gCanIndexMap.map94Index)
    	{
    		canMapInfo.realMIndex = 93;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map93Index) * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index93Type;
    	}
    	/*单体温度信息1*/
    	else if(mIndex < gCanIndexMap.map95Index)
    	{
    		canMapInfo.realMIndex = 94;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map94Index) * 8;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index94Type;
    	}
    	/*单体温度信息2*/
    	else
    	{
    		canMapInfo.realMIndex = 95;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map95Index) * 8;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index95Type;
    	}
    }
    //周期主动更新参数报文
    else if(eCANFUN_UPP == funCode)
    {
    	/*系统通用参数*/
    	if(mIndex < gCanIndexMap.map101Index)
    	{
    		canMapInfo.realMIndex = 100;
    		canMapInfo.realSIndex = mIndex * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index100Type;
    	}
    	/*系统硬件参数*/
    	else if(mIndex < gCanIndexMap.map102Index)
    	{
    		canMapInfo.realMIndex = 101;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map101Index) * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index101Type;
    	}
    	/*设备通用参数*/
    	else if(mIndex < gCanIndexMap.map103Index)
    	{
    		canMapInfo.realMIndex = 102;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map102Index) * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index102Type;
    	}
    	/*设备高级参数*/
    	else if(mIndex < gCanIndexMap.map104Index)
    	{
    		canMapInfo.realMIndex = 103;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map103Index) * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index103Type;
    	}
    	/*设备硬件参数*/
    	else
    	{
    		canMapInfo.realMIndex = 104;
    		canMapInfo.realSIndex = (mIndex - gCanIndexMap.map104Index) * 4;
    		canMapInfo.realSIndexNum = DLC / gCanIndexMap.index104Type;
    	}
    }
    //非周期主动更新帧报文
    else
    {
		canMapInfo.realMIndex = 0xff;
		canMapInfo.realSIndex = 0xff;
		canMapInfo.realSIndexNum = 0xff;
    }

    return(canMapInfo);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
