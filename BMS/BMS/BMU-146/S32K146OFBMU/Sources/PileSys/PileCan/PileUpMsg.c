/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileUpMsg.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控与上层设备通信CAN报文处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "PilePara.h"
#include "PileInfo.h"
#include "PileDiagCheck.h"
#include "PileTactUser.h"
#include "PileUserAPI.h"
#include "PileUpMsg.h"

#include "CANUser.h"
#include "Authority.h"
#include "GroupMsg.h"
//#include "TacUser.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPSysStateInfo_50[];			/*系统状态信息数组*/
extern u16 gPLimCPInfo_51[];			/*电流功率限值数组*/
extern u16 gPLimVInfo_52[];				/*电压限值信息数组*/
extern u16 gPAlarmInfo_53[];			/*告警信息数组*/
extern u16 gPFaultInfo_54[]; 			/*故障信息数组*/
extern u16 gPNotesInfo_55[];			/*提示信息数组*/
extern u16 gPGroupState_57[]; 			/*电池组状态信息数组*/

extern s16 gPSysMainInfo_60[];			/*系统整体信息数组*/
extern u16 gPSysCapInfo_61[];			/*系统电量信息数组*/
extern u16 gPVoltCharInfo_62[]; 		/*电压特征信息(最大值)数组*/
extern u16 gPVoltCharInfo_63[];			/*电压特征信息(最小值)数组*/
extern s16 gPTempCharInfo_64[]; 		/*温度特征信息(最大值)数组*/
extern s16 gPTempCharInfo_65[];			/*温度特征信息(最小值)数组*/
extern u16 gPSumVCharInfo_66[];			/*总压特征信息数组*/
extern s16 gPCurrCharInfo_67[];			/*电流特征信息数组*/

extern u16 gPStatistTime_70[]; 			/*时间统计信息数组*/
extern u16 gPStatistErr_71[]; 			/*故障统计信息数组*/
extern u16 gPStatistCapEner_72[];		/*当前能量统计信息数组*/
extern u32 gPStatistHisCap_73[];		/*累计电量统计信息数组*/
extern u32 gPStatistHisEner_74[];		/*累计电能统计信息数组*/
extern u16 gPStatistDhgDeep_75[];		/*放电深度统计信息数组*/

extern u16 gPAuthorInfo_80[]; 			/*权限级别状态信息数组*/
extern u16 gPDebugResult_82[];			/*调试结果状态信息数组*/

extern u16 gPAuthorCtrl_85[];			/*权限密码登录信息数组*/
extern u16 gPSysCmdCtrl_86[];			/*系统控制命令信息数组*/
extern u16 gPDebugCtrl_88[];			/*系统调试命令信息数组*/

extern u16 gSysGenParaRO_100[];  		/*系统通用参数设置*/
extern u16 gSysHigParaRO_101[];  		/*系统硬件参数设置*/
extern u16 gPSmuGenParaR_102[];  		/*堆控通用参数值*/

extern u16 gSysGenParaRW_105[];  		/*系统通用设置*/
extern u16 gSysHigParaRW_106[];  		/*系统高级设置*/
extern u16 gPSmuGenParaWR_107[];  		/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u32 PileCanBuildUpMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
** @Input     : func:功能码 index:主索引 dadrr:目标地址 saddr:原地址
** @Output    : CAN报文ID
** @Function  : 用户创建上传CAN报文ID
** @The notes : 堆控上行报文
**===============================================================================================*/
static u32 PileCanBuildUpMsgID(u8 func, u8 index, u8 dadrr, u8 saddr);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void PileRcvUpMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 堆控上层(PCS)CAN通信报文接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void PileRcvUpMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 saddr = 0;
	u8 daddr = 0;
	u8 index = 0;
	u8 funct = 0;

    /*解析CAN帧ID*/
	saddr = ID & 0xff;
	daddr = (ID >> 8) & 0xff;
	index = (ID >> 16) & 0xff;
	funct = (ID >> 25) & 0x0f;

    /*堆控地址冲突检测(判断是否启用堆控系统)*/
	CheckRcvPileAddrRepAPI(saddr);

    /*未启用为堆控系统不解析堆控命令*/
    if(1 != GetPileSysRunFlagAPI())
    {
    	return;
    }

    /*非自身报文/非广播报文*/
    if((gPSmuGenParaR_102[eSmuGenPara102_SmuDev] != daddr) && (0xff != daddr))
    {
    	return;
    }

    /*接收上层设备信息*/
    if((PCS_BAS_DEVADDR + (gPSmuGenParaR_102[eSmuGenPara102_SmuDev] - SMU_BAS_DEVADDR)) == saddr)
    {
    	/*与上层通信检测*/
    	PileRcvPcsMsgCountAddAPI(saddr);
    }

    /*根据CAN ID功能码处理报文*/
    switch(funct)
    {
        /*校时信息报文*/
        case 0:
            break;

        /*请求修改命令报文*/
        case 2:
        	PileRcvCanReqUpdateCmdDeal(index, pdata, DLC, canNum);
            break;

        /*请求查询命令报文*/
        case 5:
			#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
        	PileRcvCanReqReadCmdDeal(index, pdata, DLC, canNum);
			#endif
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : void PileSendUpMsg500By50ms_API(void)
** @Input     : void
** @Output    : void
** @Function  : 堆控上传1s周期CAN通信报文任务
** @The notes : 用户50ms周期接口调用
**===============================================================================================*/
void PileSendUpMsg1sBy50ms_API(void)
{
	static u8 sTime1s = 0;
	static u8 sTime3s = 0;

	switch(sTime1s)
	{
		/*500ms发送系统状态信息*/
		case 0:
		case 10:
			SendCanPileSystemStateInfoMsg50();
			break;

		/*500ms发送电流功率限值信息*/
		case 1:
		case 11:
			SendCanPileLimCurrPwrInfoMsg51();
			break;

		/*1s发送总压限值信息*/
		case 2:
			SendCanPileLimSumVoltInfoMsg52();
			break;

		/*1s发送告警信息*/
		case 12:
			SendCanPileAlarmInfoMsg53();
			break;

		/*1s发送系统提示信息和各组电池状态信息*/
		case 3:
			SendCanPileNotesInfoMsg55();
			SendCanPileGroupStateInfoMsg57();
			break;

		/*1s发送故障信息*/
		case 13:
			SendCanPileFaultInfoMsg54();
			break;

		/*500ms发送系统整体信息*/
		case 4:
		case 14:
			SendCanPileSysMainInfoMsg60();
			break;

		/*500ms发送系统电能量信息*/
		case 5:
		case 15:
			SendCanPileSysCapInfoMsg61();
			break;

		/*1s发送电压特征信息1*/
		case 6:
			SendCanPileVoltCharInfoMsg62();
			break;

		/*1s发送电压特征信息2*/
		case 7:
			SendCanPileVoltCharInfoMsg63();
			break;

		/*1s发送温度特征信息1*/
		case 16:
			SendCanPileTempCharInfoMsg64();
			break;

		/*1s发送温度特征信息2*/
		case 17:
			SendCanPileTempCharInfoMsg65();
			break;

		/*1s发送总压特征信息*/
		case 18:
			SendCanPileSumVCharInfoMsg66();
			break;

		/*1s发送电流特征信息*/
		case 19:
			SendCanPileCurrCharInfoMsg67();
			#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
			/*500ms发送系统调试结果信息(4帧)*/
			SendCanPileDebugResultInfoMsg82();
			#endif
			break;

		/*1s转3s周期*/
		case 8:
			if(0 == sTime3s)
			{
				/*3s发送时间统计信息*/
				SendCanPileStatistTimeInfoMsg70();
				//sTime3s = 1;
			}
			else if(1 == sTime3s)
			{
				#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
				/*3s发送故障数目统计信息*/
				SendCanPileStatistErrInfoMsg71();
				#endif
				//sTime3s = 2;
			}
			else
			{
				#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
				/*3s发送单次电量信息*/
				SendCanPileStatistCapInfoMsg72();
				#endif
				//sTime3s = 0;
			}
			break;

		/*1s转3s周期*/
		case 9:
			if(0 == sTime3s)
			{
				#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
				/*3s发送系统DOD和权限信息*/
				SendCanPileStatistDodInfoMsg75();
				#endif
				SendCanPileSysAuthorInfoMsg80();
				sTime3s = 1;
			}
			else if(1 == sTime3s)
			{
				#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
				/*3s发送累计电量信息*/
				SendCanPileStatistHisCapInfoMsg73();
				#endif
				sTime3s = 2;
			}
			else
			{
				#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
				/*3s发送累计电能信息*/
				SendCanPileStatistHisEnerInfoMsg74();
				#endif
				sTime3s = 0;
			}
			#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
			/*500ms发送系统调试结果信息(4帧)*/
			SendCanPileDebugResultInfoMsg82();
			#endif
			break;

		default:
			break;
	}

	/*50ms计时20次转化为1s周期*/
	if(sTime1s >= 19)
	{
		sTime1s = 0;
	}
	else
	{
		sTime1s++;
	}
}

/*=================================================================================================
** @Name      : void PileRcvCanReqUpdateCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:主索引号 *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收请求修改命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void PileRcvCanReqUpdateCmdDeal(u8 index, u8 *pdata, u8 DLC, u8 canNum)
{
    u8 i = 0;
    u8 len = 0;
    u8 sindex = 0;

    //切换密码
    if(85 == index)
    {
    	if(DLC >= 4)
    	{
    		sindex = pdata[1];
    		len = pdata[0] & 0x0f;

    		//获取报文数据
			for(i = 0; (i * 2 + 4) <= DLC; i++)
			{
				if((i < len) && ((sindex + i) < PINDEX85_LEN))
				{
					gPAuthorCtrl_85[sindex + i] = ((u16)pdata[i * 2 + 3] << 8) | pdata[i * 2 + 2];
				}
			}

    		//执行命令
			RcvCanPilePasswordDeal(sindex, len, canNum);
    	}
    }
    //系统控制
    else if(86 == index)
    {
    	if(DLC >= 4)
    	{
    		sindex = pdata[1];
    		len = pdata[0] & 0x0f;

    		//获取报文数据
			for(i = 0; (i * 2 + 4) <= DLC; i++)
			{
				if((i < len) && ((sindex + i) < PINDEX86_LEN))
				{
					gPSysCmdCtrl_86[sindex + i] = ((u16)pdata[i * 2 + 3] << 8) | pdata[i * 2 + 2];
				}
			}

    		//执行命令
			RcvCanPileCtrlCmdDeal(sindex, len, canNum);
    	}
    }
	#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
    //用户控制
    else if(87 == index)
    {
    	if(DLC >= 4)
    	{
    		sindex = pdata[1];
    		len = pdata[0] & 0x0f;

    		//获取报文数据
			for(i = 0; (i * 2 + 4) <= DLC; i++)
			{
				if((i < len) && ((sindex + i) < PINDEX87_LEN))
				{
					//gPUserCtrl_87[sindex + i] = ((u16)pdata[i * 2 + 3] << 8) | pdata[i * 2 + 2];
				}
			}

    		//执行命令
			//RcvUserCtrlCmdDeal(sindex, len, canNum);
    	}
    }
    //系统调试
    else if(88 == index)
    {
    	if(DLC >= 4)
    	{
    		sindex = pdata[1];
    		len = pdata[0] & 0x0f;

    		//获取报文数据
			for(i = 0; (i * 2 + 4) <= DLC; i++)
			{
				if((i < len) && ((sindex + i) < PINDEX88_LEN))
				{
					gPDebugCtrl_88[sindex + i] = ((u16)pdata[i * 2 + 3] << 8) | pdata[i * 2 + 2];
				}
			}

    		//执行命令
			RcvCanPileDebugCmdDeal(sindex, len, canNum);
    	}
    }
    //系统通用参数设置
    else if(105 == index)
    {
    	if(DLC >= 4)
    	{
    		sindex = pdata[1];
    		len = pdata[0] & 0x0f;

    		//获取报文数据
			for(i = 0; (i * 2 + 4) <= DLC; i++)
			{
				if((i < len) && ((sindex + i) < SINDEX105_LEN))
				{
					gSysGenParaRW_105[sindex + i] = ((u16)pdata[i * 2 + 3] << 8) | pdata[i * 2 + 2];
				}
			}

    		//执行命令
			RcvCanSystemGenParaCfgDeal(sindex, len, canNum);
    	}
    }
    //系统硬件参数设置
    else if(106 == index)
    {
    	if(DLC >= 4)
    	{
    		sindex = pdata[1];
    		len = pdata[0] & 0x0f;

    		//获取报文数据
			for(i = 0; (i * 2 + 4) <= DLC; i++)
			{
				if((i < len) && ((sindex + i) < SINDEX106_LEN))
				{
					gSysHigParaRW_106[sindex + i] = ((u16)pdata[i * 2 + 3] << 8) | pdata[i * 2 + 2];
				}
			}

    		//执行命令
			RcvCanSystemHWParaCfgDeal(sindex, len, canNum);
    	}
    }
    //系统通用参数设置
    else if(107 == index)
    {
    	if(DLC >= 4)
    	{
    		sindex = pdata[1];
    		len = pdata[0] & 0x0f;

    		//获取报文数据
			for(i = 0; (i * 2 + 4) <= DLC; i++)
			{
				if((i < len) && ((sindex + i) < PINDEX107_LEN))
				{
					gPSmuGenParaWR_107[sindex + i] = ((u16)pdata[i * 2 + 3] << 8) | pdata[i * 2 + 2];
				}
			}

    		//执行命令
			RcvCanPileGenParaCfgDeal(sindex, len, canNum);
    	}
    }
	#endif
}

/*=================================================================================================
** @Name      : void RcvCanPilePasswordDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收电池堆权限密码命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void RcvCanPilePasswordDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*控制命令索引错误*/
    if(((index + len) > PINDEX85_LEN) || (0 == len))
    {
        return;
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //权限密码高位
            case 0:
            	PileSetSelfGroupOperAuthCmdAPI(eAuthP_Can0 + canNum, 0, gPAuthorCtrl_85[0]);
				break;

            //权限密码低位
            case 1:
            	/*执行密码检测*/
            	PileSetSelfGroupOperAuthCmdAPI(eAuthP_Can0 + canNum, 1, gPAuthorCtrl_85[1]);
            	SendCanPileSysAuthorInfoMsg80();
                break;

            default:
                break;
        }
    }
}

/*=================================================================================================
** @Name      : void RcvCanPileCtrlCmdDeal(u8 index, u8 len)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收电池堆控制命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void RcvCanPileCtrlCmdDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return;
    }

    /*控制命令索引错误*/
    if(((index + len) > PINDEX86_LEN) || (0 == len))
    {
        return;
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //工作状态控制
            case eSysCmdCtrl86_State:
            	RcvPileWorkStateCmdAPI(eAuthP_Can0 + canNum);
                break;

            //工作模式切换
            case eSysCmdCtrl86_Mode:
            	RcvPileWorkModeCmdAPI(eAuthP_Can0 + canNum);
                break;

            //系统清除命令
            case eSysCmdCtrl86_ClrCmd:
            	RcvPileSysClrCmdAPI(eAuthP_Can0 + canNum);
                break;

            default:
                break;
        }
    }
}

#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Name      : void RcvCanPileDebugCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收电池堆调试命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void RcvCanPileDebugCmdDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return;
    }

    /*调试命令索引错误*/
    if(((index + len) > PINDEX88_LEN) || (0 == len))
    {
        return;
    }

    for(i = index; i < (index + len); i++)
    {
    	//调试命令设置
        if(i < eDbgCmd88_Affirm)
        {
            RcvPileDebugOperCmdAPI(i);
        }
        //调试确认
        else if(i == eDbgCmd88_Affirm)
        {
            RcvPileDebugAffirmCmdAPI(eAuthP_Can0 + canNum);
        }
    }
}

/*=================================================================================================
** @Name      : void RcvCanPileGenParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收堆控通用参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void RcvCanPileGenParaCfgDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return;
    }

    /*设置命令索引错误*/
    if(((index + len) > PINDEX107_LEN) || (0 == len))
    {
        return;
    }

    for(i = index; i < (index + len); i++)
    {
        /*写入参数处理*/
        //RcvPileGenParaCfgAPI(i, eAuthP_Can0 + canNum);
    }
}
#endif

/*=================================================================================================
** @Name      : void SendCanPileSystemStateInfoMsg50(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统整体状态信息CAN报文
** @The notes : 主索引50
**===============================================================================================*/
void SendCanPileSystemStateInfoMsg50(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送50索引*/
    msg.ID = PileCanBuildUpMsgID(10, 50, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*50索引信息组成单帧报文*/
    for(i = 0; i < PINDEX50_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPSysStateInfo_50[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPSysStateInfo_50[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileLimCurrPwrInfoMsg51(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电流功率限值信息CAN报文
** @The notes : 主索引51
**===============================================================================================*/
void SendCanPileLimCurrPwrInfoMsg51(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送51索引*/
    msg.ID = PileCanBuildUpMsgID(10, 51, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*51索引信息组成单帧报文*/
    for(i = 0; i < PINDEX51_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPLimCPInfo_51[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPLimCPInfo_51[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileLimSumVoltInfoMsg52(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统总压限值信息CAN报文
** @The notes : 主索引52
**===============================================================================================*/
void SendCanPileLimSumVoltInfoMsg52(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送52索引*/
    msg.ID = PileCanBuildUpMsgID(10, 52, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*52索引信息组成单帧报文*/
    for(i = 0; i < PINDEX52_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPLimVInfo_52[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPLimVInfo_52[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileAlarmInfoMsg53(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统告警信息CAN报文
** @The notes : 主索引53
**===============================================================================================*/
void SendCanPileAlarmInfoMsg53(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送53索引*/
    msg.ID = PileCanBuildUpMsgID(10, 53, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*53索引信息组成单帧报文*/
    for(i = 0; i < PINDEX53_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPAlarmInfo_53[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPAlarmInfo_53[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileFaultInfoMsg54(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统故障信息CAN报文
** @The notes : 主索引54
**===============================================================================================*/
void SendCanPileFaultInfoMsg54(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送54索引*/
    msg.ID = PileCanBuildUpMsgID(10, 54, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*54索引信息组成单帧报文*/
    for(i = 0; i < PINDEX54_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPFaultInfo_54[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPFaultInfo_54[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileNotesInfoMsg55(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统提醒信息CAN报文
** @The notes : 主索引55
**===============================================================================================*/
void SendCanPileNotesInfoMsg55(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送55索引*/
    msg.ID = PileCanBuildUpMsgID(10, 55, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*55索引信息组成单帧报文*/
    for(i = 0; i < PINDEX55_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPNotesInfo_55[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPNotesInfo_55[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileGroupStateInfoMsg57(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统各电池组状态信息CAN报文
** @The notes : 主索引57
**===============================================================================================*/
void SendCanPileGroupStateInfoMsg57(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送57索引*/
    msg.ID = PileCanBuildUpMsgID(10, 57, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*57索引信息组成单帧报文*/
    for(i = 0; i < PINDEX57_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPGroupState_57[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPGroupState_57[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileSysMainInfoMsg60(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统主要信息CAN报文
** @The notes : 主索引60
**===============================================================================================*/
void SendCanPileSysMainInfoMsg60(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送60索引*/
    msg.ID = PileCanBuildUpMsgID(10, 60, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*60索引信息组成单帧报文*/
    for(i = 0; i < PINDEX60_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPSysMainInfo_60[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPSysMainInfo_60[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileSysCapInfoMsg61(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电能量信息CAN报文
** @The notes : 主索引61
**===============================================================================================*/
void SendCanPileSysCapInfoMsg61(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送61索引*/
    msg.ID = PileCanBuildUpMsgID(10, 61, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*61索引信息组成单帧报文*/
    for(i = 0; i < PINDEX61_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPSysCapInfo_61[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPSysCapInfo_61[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileVoltCharInfoMsg62(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电压特征信息1CAN报文
** @The notes : 主索引62
**===============================================================================================*/
void SendCanPileVoltCharInfoMsg62(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送62索引*/
    msg.ID = PileCanBuildUpMsgID(10, 62, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*62索引信息组成单帧报文*/
    for(i = 0; i < PINDEX62_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPVoltCharInfo_62[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPVoltCharInfo_62[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileVoltCharInfoMsg63(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电压特征信息2CAN报文
** @The notes : 主索引63
**===============================================================================================*/
void SendCanPileVoltCharInfoMsg63(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送63索引*/
    msg.ID = PileCanBuildUpMsgID(10, 63, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*63索引信息组成单帧报文*/
    for(i = 0; i < PINDEX63_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPVoltCharInfo_63[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPVoltCharInfo_63[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileTempCharInfoMsg64(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统温度特征信息1CAN报文
** @The notes : 主索引64
**===============================================================================================*/
void SendCanPileTempCharInfoMsg64(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送64索引*/
    msg.ID = PileCanBuildUpMsgID(10, 64, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*64索引信息组成单帧报文*/
    for(i = 0; i < PINDEX64_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPTempCharInfo_64[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPTempCharInfo_64[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileTempCharInfoMsg65(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统温度特征信息2CAN报文
** @The notes : 主索引65
**===============================================================================================*/
void SendCanPileTempCharInfoMsg65(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送65索引*/
    msg.ID = PileCanBuildUpMsgID(10, 65, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*65索引信息组成单帧报文*/
    for(i = 0; i < PINDEX65_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPTempCharInfo_65[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPTempCharInfo_65[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileSumVCharInfoMsg66(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统总压特征信息CAN报文
** @The notes : 主索引66
**===============================================================================================*/
void SendCanPileSumVCharInfoMsg66(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送66索引*/
    msg.ID = PileCanBuildUpMsgID(10, 66, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*66索引信息组成单帧报文*/
    for(i = 0; i < PINDEX66_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPSumVCharInfo_66[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPSumVCharInfo_66[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileCurrCharInfoMsg67(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电流特征信息CAN报文
** @The notes : 主索引67
**===============================================================================================*/
void SendCanPileCurrCharInfoMsg67(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送67索引*/
    msg.ID = PileCanBuildUpMsgID(10, 67, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*67索引信息组成单帧报文*/
    for(i = 0; i < PINDEX66_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPCurrCharInfo_67[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPCurrCharInfo_67[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileStatistTimeInfoMsg70(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统时间统计信息CAN报文
** @The notes : 主索引70
**===============================================================================================*/
void SendCanPileStatistTimeInfoMsg70(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送70索引*/
    msg.ID = PileCanBuildUpMsgID(10, 70, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*70索引信息组成单帧报文*/
    for(i = 0; i < PINDEX70_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPStatistTime_70[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPStatistTime_70[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Name      : void SendCanPileStatistErrInfoMsg71(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统故障数目统计信息CAN报文
** @The notes : 主索引71
**===============================================================================================*/
void SendCanPileStatistErrInfoMsg71(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送71索引*/
    msg.ID = PileCanBuildUpMsgID(10, 71, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*71索引信息组成单帧报文*/
    for(i = 0; i < PINDEX71_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPStatistErr_71[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPStatistErr_71[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileStatistCapInfoMsg72(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统单次电量统计信息CAN报文
** @The notes : 主索引72
**===============================================================================================*/
void SendCanPileStatistCapInfoMsg72(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送72索引*/
    msg.ID = PileCanBuildUpMsgID(10, 72, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*72索引信息组成单帧报文*/
    for(i = 0; i < PINDEX72_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPStatistCapEner_72[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPStatistCapEner_72[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileStatistHisCapInfoMsg73(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统累计电量统计信息CAN报文
** @The notes : 主索引73
**===============================================================================================*/
void SendCanPileStatistHisCapInfoMsg73(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送73索引*/
    msg.ID = PileCanBuildUpMsgID(10, 73, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*73索引信息组成单帧报文*/
    for(i = 0; i < PINDEX73_LEN; i++)
    {
    	if(i < 2)
    	{
			msg.Data[i * 4] = gPStatistHisCap_73[i] & 0xff;
			msg.Data[i * 4 + 1] = (gPStatistHisCap_73[i] >> 8) & 0xff;
			msg.Data[i * 4 + 2] = (gPStatistHisCap_73[i] >> 16) & 0xff;
			msg.Data[i * 4 + 3] = (gPStatistHisCap_73[i] >> 24) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 4;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileStatistHisEnerInfoMsg74(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统累计电能统计信息CAN报文
** @The notes : 主索引74
**===============================================================================================*/
void SendCanPileStatistHisEnerInfoMsg74(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送74索引*/
    msg.ID = PileCanBuildUpMsgID(10, 74, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*74索引信息组成单帧报文*/
    for(i = 0; i < PINDEX74_LEN; i++)
    {
    	if(i < 2)
    	{
			msg.Data[i * 4] = gPStatistHisEner_74[i] & 0xff;
			msg.Data[i * 4 + 1] = (gPStatistHisEner_74[i] >> 8) & 0xff;
			msg.Data[i * 4 + 2] = (gPStatistHisEner_74[i] >> 16) & 0xff;
			msg.Data[i * 4 + 3] = (gPStatistHisEner_74[i] >> 24) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 4;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void SendCanPileStatistDodInfoMsg75(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统放电深度统计信息CAN报文
** @The notes : 主索引75
**===============================================================================================*/
void SendCanPileStatistDodInfoMsg75(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送75索引*/
    msg.ID = PileCanBuildUpMsgID(10, 75, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*75索引信息组成单帧报文*/
    for(i = 0; i < PINDEX75_LEN; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = gPStatistDhgDeep_75[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPStatistDhgDeep_75[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}
#endif

/*=================================================================================================
** @Name      : void SendCanPileSysAuthorInfoMsg80(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统权限信息CAN报文
** @The notes : 主索引80
**===============================================================================================*/
void SendCanPileSysAuthorInfoMsg80(void)
{
	u8 i = 0;
    t_CANMSG msg;

    /*单帧发送80索引*/
    msg.ID = PileCanBuildUpMsgID(10, 80, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*80索引信息组成单帧报文*/
    for(i = 0; i < PINDEX80_LEN; i++)
    {
    	if(i < 4)
    	{
    		gPAuthorInfo_80[i] = GetSysAuthorityInfoAPI(i);
			msg.Data[i * 2] = gPAuthorInfo_80[i] & 0xff;
			msg.Data[i * 2 + 1] = (gPAuthorInfo_80[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*报文长度*/
    msg.Len = i * 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Name      : void SendCanPileDebugResultInfoMsg82(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统调试结果信息CAN报文
** @The notes : 主索引82
**===============================================================================================*/
void SendCanPileDebugResultInfoMsg82(void)
{
	u8 i = 0;
	static u8 sTime = 0;
	static u8 sIndex = 0;
    t_CANMSG msg;

    /*非调试模式不发*/
    if(eMODE_DEBUG != GetPileWorkModeAPI())
    {
    	/*退出调试模式再发10次*/
    	if(sTime >= 10)
    	{
    		sIndex = 0;
			return;
    	}
    }
    else
    {
    	sTime = 0;
    }

    /*长帧发送82索引*/
    msg.ID = PileCanBuildUpMsgID(10, 82, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*82索引信息组成长帧报文*/
    msg.Data[1] = 0xc0 | sIndex;
    for(i = 0; i < 3; i++)
    {
    	if(sIndex < PINDEX82_LEN)
    	{
			msg.Data[i * 2 + 2] = gPDebugResult_82[sIndex] & 0xff;
			msg.Data[i * 2 + 3] = (gPDebugResult_82[sIndex] >> 8) & 0xff;
			sIndex++;
    	}
    	else
    	{
    		break;
    	}
    }
    msg.Data[0] = 0xc0 | ((sIndex - msg.Data[1]) & 0x0f);

    /*发送索引数*/
    if(sIndex >= PINDEX82_LEN)
    {
    	sTime++;
    	sIndex = 0;
    }

    /*报文长度*/
    msg.Len = i * 2 + 2;

    /*向CAN0通道发送*/
    (void)BSPCANSendQin(eCAN0, &msg);
}

/*=================================================================================================
** @Name      : void PileRcvCanReqReadCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:主索引号 *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收请求查询命令CAN报文处理
** @The notes : CAN通信包接收请求查询处理调用
**===============================================================================================*/
void PileRcvCanReqReadCmdDeal(u8 index, u8 *pdata, u8 DLC, u8 canNum)
{
    u8 len = 0;
    u8 sindex = 0;

    //查询堆控设备信息
    if(1 == index)
    {
    	if(DLC >= 3)
    	{
    		sindex = pdata[1];
    		len = pdata[2];

    		//获取应答数据并发送
    		AnswCanPileDevInfoMsg1(sindex, len);
    	}
    }
    //查询系统通用参数
    else if(100 == index)
    {
    	if(DLC >= 3)
    	{
    		sindex = pdata[1];
    		len = pdata[2];

    		//获取应答数据并发送
    		AnswCanPileSysGenParaMsg100(sindex, len);
    	}
    }
    //查询系统硬件参数
    else if(101 == index)
    {
    	if(DLC >= 3)
    	{
    		sindex = pdata[1];
    		len = pdata[2];

    		//获取应答数据并发送
    		AnswCanPileSysHWParaMsg101(sindex, len);
    	}
    }
    //查询堆控通用参数
    if(102 == index)
    {
    	if(DLC >= 3)
    	{
    		sindex = pdata[1];
    		len = pdata[2];

    		//获取应答数据并发送
    		//AnswCanPileGenParaMsg102(sindex, len);
    	}
    }
}

/*=================================================================================================
** @Name      : void AnswCanPileDevInfoMsg1(void)
** @Input     : void
** @Output    : void
** @Function  : 应答查询堆控设备信息CAN报文
** @The notes : 主索引1
**===============================================================================================*/
void AnswCanPileDevInfoMsg1(u8 index, u8 len)
{
	u8 i = 0;
	u8 j = 0;
	u8 times = 0;
	u8 sindex = 0;
    t_CANMSG msg;
    u16 devInfo[CANINDEX1_LEN] = {0};

    /*查询参数错误*/
    if(((index + len) > CANINDEX1_LEN) || (0 == len))
    {
    	return;
    }

    devInfo[0] = SMU_DEV_ID;                     						/*设备识别码*/
    devInfo[1] = gPSmuGenParaR_102[eSmuGenPara102_SmuDev];  			/*设备地址*/
	devInfo[2] = ((u16)DEVGPIOGetHWVersion() << 8) |(BMU_HW_VER & 0xff);/*硬件版本号*/
	devInfo[3] = BMU_SW_VERL;                    						/*软件支线版本号*/
	devInfo[4] = BMU_SW_VERH;                   						/*软件主线版本号*/
	devInfo[5] = 0x0100;                         						/*BSP版本号*/
	devInfo[6] = 0x0100;                         						/*CAN协议包版本号*/
	devInfo[7] = 0x0100;                         						/*ModBus协议包版本号*/

    /*长帧发送1索引*/
    msg.ID = PileCanBuildUpMsgID(6, 1, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*需发送帧数*/
    sindex = index;
    times = (len + 2) / 3;

    for(i = 0; i < times; i++)
    {
		/*1索引信息组成长帧报文*/
		msg.Data[1] = sindex;
		for(j = 0; j < 3; j++)
		{
			if((sindex < CANINDEX1_LEN) && (sindex < (index + len)))
			{
				msg.Data[j * 2 + 2] = devInfo[sindex] & 0xff;
				msg.Data[j * 2 + 3] = (devInfo[sindex] >> 8) & 0xff;
				sindex++;
			}
			else
			{
				break;
			}
		}
		msg.Data[0] = 0xc0 | ((sindex - msg.Data[1]) & 0x0f);

		/*报文长度*/
		msg.Len = j * 2 + 2;

		/*向CAN0通道发送*/
		(void)BSPCANSendQin(eCAN0, &msg);
    }
}

/*=================================================================================================
** @Name      : void SendCanPileDebugResultInfoMsg82(void)
** @Input     : void
** @Output    : void
** @Function  : 应答查询系统通用参数信息CAN报文
** @The notes : 主索引100
**===============================================================================================*/
void AnswCanPileSysGenParaMsg100(u8 index, u8 len)
{
	u8 i = 0;
	u8 j = 0;
	u8 times = 0;
	u8 sindex = 0;
    t_CANMSG msg;

    /*查询参数错误*/
    if(((index + len) > SINDEX100_LEN) || (0 == len))
    {
    	return;
    }

    /*长帧发送100索引*/
    msg.ID = PileCanBuildUpMsgID(6, 100, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*需发送帧数*/
    sindex = index;
    times = (len + 2) / 3;

    for(i = 0; i < times; i++)
    {
		/*100索引信息组成长帧报文*/
		msg.Data[1] = sindex;
		for(j = 0; j < 3; j++)
		{
			if((sindex < SINDEX100_LEN) && (sindex < (index + len)))
			{
				msg.Data[j * 2 + 2] = gSysGenParaRO_100[sindex] & 0xff;
				msg.Data[j * 2 + 3] = (gSysGenParaRO_100[sindex] >> 8) & 0xff;
				sindex++;
			}
			else
			{
				break;
			}
		}
		msg.Data[0] = 0xc0 | ((sindex - msg.Data[1]) & 0x0f);

		/*报文长度*/
		msg.Len = j * 2 + 2;

		/*向CAN0通道发送*/
		(void)BSPCANSendQin(eCAN0, &msg);
    }
}

/*=================================================================================================
** @Name      : void AnswCanPileSysHWParaMsg101(void)
** @Input     : void
** @Output    : void
** @Function  : 应答查询系统硬件参数信息CAN报文
** @The notes : 主索引101
**===============================================================================================*/
void AnswCanPileSysHWParaMsg101(u8 index, u8 len)
{
	u8 i = 0;
	u8 j = 0;
	u8 times = 0;
	u8 sindex = 0;
    t_CANMSG msg;

    /*查询参数错误*/
    if(((index + len) > SINDEX101_LEN) || (0 == len))
    {
    	return;
    }

    /*长帧发送100索引*/
    msg.ID = PileCanBuildUpMsgID(6, 101, 0xff, (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);

    /*需发送帧数*/
    sindex = index;
    times = (len + 2) / 3;

    for(i = 0; i < times; i++)
    {
		/*100索引信息组成长帧报文*/
		msg.Data[1] = sindex;
		for(j = 0; j < 3; j++)
		{
			if((sindex < SINDEX101_LEN) && (sindex < (index + len)))
			{
				msg.Data[j * 2 + 2] = gSysHigParaRO_101[sindex] & 0xff;
				msg.Data[j * 2 + 3] = (gSysHigParaRO_101[sindex] >> 8) & 0xff;
				sindex++;
			}
			else
			{
				break;
			}
		}
		msg.Data[0] = 0xc0 | ((sindex - msg.Data[1]) & 0x0f);

		/*报文长度*/
		msg.Len = j * 2 + 2;

		/*向CAN0通道发送*/
		(void)BSPCANSendQin(eCAN0, &msg);
    }
}
#endif

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u32 PileCanBuildUpMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
** @Input     : func:功能码 index:主索引 dadrr:目标地址 saddr:原地址
** @Output    : CAN报文ID
** @Function  : 用户创建上传CAN报文ID
** @The notes : 堆控上行报文
**===============================================================================================*/
static u32 PileCanBuildUpMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
{
	u32 id = 0;

	id = ((u32)func << 25) | ((u32)index << 16) | ((u32)dadrr << 8) | saddr;

	return(id);
}

#endif
