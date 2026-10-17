/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileDnMsg.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控与下层设备通信CAN报文处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PilePara.h"
#include "PublicPara.h"
#include "PileInfo.h"
#include "GroupInfo.h"
#include "Authority.h"
#include "PileDiagCheck.h"
#include "PileTactInterface.h"
#include "PileDnMsg.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gSysGenParaRO_100[];						/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  					/*堆控通用参数值*/

extern u16 gGSysStateInfo_500[][GINDEX50_LEN];		/*系统状态信息数组*/
extern u16 gGLimCPInfo_510[][GINDEX51_LEN];			/*电流功率限值数组*/
extern u16 gGLimVInfo_520[][GINDEX52_LEN];			/*电压限值信息数组*/
extern u16 gGAlarmInfo_530[][GINDEX53_LEN];			/*告警信息数组*/
extern u16 gGFaultInfo_540[][GINDEX54_LEN]; 		/*故障信息数组*/
extern u16 gGErrDetail_550[][GINDEX55_LEN]; 		/*故障定位信息数组*/
extern u16 gGPortStateInfo_560[][GINDEX56_LEN];		/*输入输出口状态信息数组*/
extern s16 gGSysMainInfo_600[][GINDEX60_LEN];		/*系统整体信息数组*/
extern u16 gGSysCapInfo_610[][GINDEX61_LEN];		/*系统电量信息数组*/
extern s16 gGSysCellInfo_620[][GINDEX62_LEN]; 		/*系统单体信息数组*/
extern u16 gGVoltCharInfo_630[][GINDEX63_LEN]; 		/*电压特征信息数组*/
extern s16 gGTempCharInfo_640[][GINDEX64_LEN]; 		/*温度特征信息数组*/
extern u16 gGStatistTime_700[][GINDEX70_LEN]; 		/*时间统计信息数组*/
extern u16 gGStatistErr_710[][GINDEX71_LEN]; 		/*故障统计信息数组*/
extern u16 gGStatistCapEner_720[][GINDEX72_LEN];	/*当前能量统计信息数组*/
extern u32 gGStatistHisCap_730[][GINDEX73_LEN];		/*累计电量统计信息数组*/
extern u32 gGStatistHisEner_740[][GINDEX74_LEN];	/*累计电能统计信息数组*/
extern u16 gGStatistDhgDeep_750[][GINDEX75_LEN];	/*放电深度统计信息数组*/
extern u16 gGAuthorInfo_800[][GINDEX80_LEN]; 		/*权限级别状态信息数组*/
extern u16 gGCtrlResult_810[][GINDEX81_LEN];		/*控制结果状态信息数组*/
extern u16 gGDebugResult_820[][GINDEX82_LEN];		/*调试结果状态信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void PileRcvDnMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 堆控下层(主控)CAN通信报文接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void PileRcvDnMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 saddr = 0;
	u8 index = 0;
	u8 funct = 0;
	u8 group = 0;

    /*解析CAN帧ID*/
	saddr = ID & 0xff;
	index = (ID >> 16) & 0xff;
	funct = (ID >> 25) & 0x0f;

    /*非主控报文(或非同一堆控下电池组)*/
    if((saddr < BMU_BAS_DEVADDR) || (saddr >= SMU_BAS_DEVADDR)
        || (ChangPileGAddrToSIdAPI(saddr) != GetPileSelfStackIdAPI()))
    {
    	return;
    }

    /*计算组号*/
    group = (saddr - BMU_BAS_DEVADDR) % gSysGenParaRO_100[eSGenPara100_AllGNum];

    /*检测电池组通信状态*/
    PileRcvGroupMsgCountAddAPI(group);

    /*根据CAN ID功能码处理报文*/
    switch(funct)
    {
        /*主动更新(长帧)报文*/
        case 8:
        	PileRcvCanGLongInfoDeal(group, index, pdata, DLC, canNum);
            break;

        /*主动发送(单帧)报文*/
        case 10:
        	PileRcvCanGSingleInfoDeal(group, index, pdata, DLC, canNum);
            break;

        default:
            break;
    }
}

//================================== ===========================================================
//函数名称	: u8 PileSendDnGroupBreathMsg_API(void)
//输入参数	: void
//输出参数	: TRUE:成功 FALSE:失败
//函数功能	: 堆控向电池组下发心跳帧报文
//注意事项	: 向CAN0发报文(1s)
//=============================================================================================
u8 PileSendDnGroupBreathMsg_API(void)
{
    u8 fun = 0;
    u8 saddr = 0;
    u8 daddr = 0;
    t_CANMSG msg;
    static u8 sTime = 0;

    fun = 1;                                                //功能码(请求修改)
    saddr = (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev];   //堆控设备地址
    daddr = 0xff;                  							//广播地址

    msg.ID = (((u32)fun & 0x0f) << 25) | ((u32)0x01 << 24) | ((u32)1 << 16)
                | ((u32)daddr << 8) | (u32)saddr;

    msg.Len = 8;

    msg.Data[0] = (u8)(sTime & 0xff);	    //心跳计数
    msg.Data[1] = DEVGPIOGetHWVersion();    //硬件版本号
    msg.Data[2] = (u8)(SMU_DEV_ID & 0xff);
    msg.Data[3] = (u8)((SMU_DEV_ID >> 8) & 0xff);
    msg.Data[4] = (u8)(BMU_SW_VERL & 0xff);
    msg.Data[5] = (u8)((BMU_SW_VERL >> 8) & 0xff);
    msg.Data[6] = (u8)(BMU_SW_VERH & 0xff);
    msg.Data[7] = (u8)((BMU_SW_VERH >> 24) & 0xff);

    (void)BSPCANSendQin(eCAN0,&msg);
    sTime++;

    return(TRUE);
}

//================================== ===========================================================
//函数名称	: u8 PileSendDnGroupOperAuthCmdMsg_API(u8 group, u8 auth)
//输入参数	: group:组号[0,15] 0xff:广播所有组 auth:权限命令(0:普通 1:运维 2:高级 3:超级)
//输出参数	: TRUE:成功 FALSE:失败
//函数功能	: 堆控向电池组下发操作权命令报文
//注意事项	: 向CAN0发报文(主索引85,不包括控制自身所在组)
//=============================================================================================
u8 PileSendDnGroupOperAuthCmdMsg_API(u8 group, u8 auth)
{
    //u8 i = 0;
    u8 fun = 0;
    u8 saddr = 0;
    u8 daddr = 0;
    u16 passName = 0;
    u16 passWord = 0;
    t_CANMSG msg;

    //子索引错误
    if(auth > eAuthL_Admin)
    {
        return(FALSE);
    }

    //根据权限取相应密码
	if(eAuthL_Admin == auth)
	{
		//超级权限
		passName = AUTHORITY_NAME3;
		passWord = AUTHORITY_PWORD3;
	}
	else if(eAuthL_Service == auth)
	{
		//高级权限
		passName = AUTHORITY_NAME2;
		passWord = AUTHORITY_PWORD2;
	}
	else if(eAuthL_Operater == auth)
	{
		//运维权限
		passName = AUTHORITY_NAME1;
		passWord = AUTHORITY_PWORD1;
	}
	else
	{
		//普通权限
		passName = AUTHORITY_NAMECLR;
		passWord = AUTHORITY_PWORDCLR;
	}

    //控制自身电池组
    if(group == GetSysSelfGroupIdAPI())
    {
        //控制自身(不包括)
    	//PileSetSelfGroupOperAuthCmdAPI(eAuthP_Can0, 0, passName);
    	//PileSetSelfGroupOperAuthCmdAPI(eAuthP_Can0, 1, passWord);
        return(TRUE);
    }
    //控制单组电池组组号非法
    else if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    	&& (group != 0xff))
    {
        return(FALSE);
    }

//    //广播所有电池组
//    if(0xff == group)
//    {
//        //记录命令
//        for(i = 0; i < gBsuSysRunPara_57[eRunPara57_ManagGNum]; i++)
//        {
//        	if(i < GetSysSelfGroupIdAPI())
//        	{
//        		gGAuthorCtrl_850[i][0] = passName;
//        		gGAuthorCtrl_850[i][1] = passWord;
//        	}
//        	else if(group > GetSysSelfGroupIdAPI())
//        	{
//        		gGAuthorCtrl_850[i - 1][0] = passName;
 //        		gGAuthorCtrl_850[i - 1][1] = passWord;
//        	}
//        	else
//        	{
//        		;
//        	}
//        }
//
//        //控制自身(不包括)
//        //PileSetSelfGroupOperAuthCmdAPI(eAuthP_Can0, 0, passName);
//        //PileSetSelfGroupOperAuthCmdAPI(eAuthP_Can0, 1, passWord);
//    }
//    else
//    {
//		//记录命令
//		if(group < GetSysSelfGroupIdAPI())
//		{
//			gGAuthorCtrl_850[group][0] = passName;
//			gGAuthorCtrl_850[group][1] = passWord;
//		}
//		else if(group > GetSysSelfGroupIdAPI())
//		{
//			gGAuthorCtrl_850[group - 1][0] = passName;
//			gGAuthorCtrl_850[group - 1][1] = passWord;
//		}
//    }

    fun = 2;                                                //功能码(请求修改)
    saddr = (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev];   //堆控设备地址
    daddr = ChangPileGIdToGAddrAPI(group);                  //主控目标地址

    msg.ID = (((u32)fun & 0x0f) << 25) | ((u32)0x01 << 24) | ((u32)85 << 16)
                | ((u32)daddr << 8) | (u32)saddr;

    msg.Len = 6;

    msg.Data[0] = 0xc2;	    //2个子索引
    msg.Data[1] = 0;        //起始索引
    msg.Data[2] = (u8)(passName & 0xff);
    msg.Data[3] = (u8)((passName >> 8) & 0xff);
    msg.Data[4] = (u8)(passWord & 0xff);
    msg.Data[5] = (u8)((passWord >> 8) & 0xff);
    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0,&msg);

    return(TRUE);
}

//================================== ===========================================================
//函数名称	: u8 PileSendDnGroupSysCtrlCmdMsg_API(u8 group, u8 index, u16 cmd)
//输入参数	: group:组号[0,15] 0xff:广播所有组 index:子索引号[0,2] cmd:命令
//输出参数	: TRUE:成功 FALSE:失败
//函数功能	: 堆控向电池组下发系统控制命令报文
//注意事项	: 向CAN0发报文(主索引86,不包括控制自身所在组)
//=============================================================================================
u8 PileSendDnGroupSysCtrlCmdMsg_API(u8 group, u8 index, u16 cmd)
{
    //u8 i = 0;
    u8 fun = 0;
    u8 saddr = 0;
    u8 daddr = 0;
    t_CANMSG msg;

    //子索引错误
    if(index >= GINDEX86_LEN)
    {
        return(FALSE);
    }

    //控制自身电池组
    if(group == GetSysSelfGroupIdAPI())
    {
        //控制自身(不包括)
        //PileSetSelfGroupSysCtrlCmdAPI(index, cmd);
        return(TRUE);
    }
    //控制单组电池组组号非法
    else if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    	&& (group != 0xff))
    {
        return(FALSE);
    }

//    //广播所有电池组
//    if(0xff == group)
//    {
//        //记录命令
//        for(i = 0; i < gBsuSysRunPara_57[eRunPara57_ManagGNum]; i++)
//        {
//        	if(i < GetSysSelfGroupIdAPI())
//        	{
//        		gGSysCmdCtrl_860[i][index] = cmd;
//        	}
//        	else if(group > GetSysSelfGroupIdAPI())
//        	{
//        		gGSysCmdCtrl_860[i - 1][index] = cmd;
//        	}
//        	else
//        	{
//        		;
//        	}
//        }
//
//        //控制自身(不包括)
//        //PileSetSelfGroupSysCtrlCmdAPI(index, cmd);
//    }
//    else
//    {
//		//记录命令
//		if(group < GetSysSelfGroupIdAPI())
//		{
//			gGSysCmdCtrl_860[group][index] = cmd;
//		}
//		else if(group > GetSysSelfGroupIdAPI())
//		{
//			gGSysCmdCtrl_860[group - 1][index] = cmd;
//		}
//    }

    fun = 2;                                                //功能码(请求修改)
    saddr = (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev];   //堆控设备地址
    daddr = ChangPileGIdToGAddrAPI(group);                  //主控目标地址

    msg.ID = (((u32)fun & 0x0f) << 25) | ((u32)0x01 << 24) | ((u32)86 << 16)
                | ((u32)daddr << 8) | (u32)saddr;

    msg.Len = 4;

    msg.Data[0] = 0xc1;	    //一个子索引
    msg.Data[1] = index;    //起始索引
    msg.Data[2] = (u8)(cmd & 0xff);
    msg.Data[3] = (u8)((cmd >> 8) & 0xff);
    msg.Data[4] = 0;
    msg.Data[5] = 0;
    msg.Data[6] = 0;
    msg.Data[7] = 0;

    (void)BSPCANSendQin(eCAN0,&msg);

    return(TRUE);
}

//================================== ===========================================================
//函数名称	: u8 PileSendDnGroupUserCtrlCmdMsg_API(u8 group, u8 index, u8 len, u16 *cmd)
//输入参数	: group:组号[0,15] 0xff:广播所有组 index:起始子索引号[0,7] len:子索引长度[0, 8] *cmd:命令
//输出参数	: TRUE:成功 FALSE:失败
//函数功能	: 堆控向电池组下发用户控制命令报文
//注意事项	: 向CAN0发报文(主索引87,不包括控制自身所在组)
//=============================================================================================
u8 PileSendDnGroupUserCtrlCmdMsg_API(u8 group, u8 index, u8 len, u16 *cmd)
{
    u8 i = 0;
    u8 j = 0;
    u8 fun = 0;
    u8 saddr = 0;
    u8 daddr = 0;
    t_CANMSG msg;

    //子索引错误
    if(((index + len) > GINDEX87_LEN) || (0 == len))
    {
        return(FALSE);
    }

    //控制自身电池组
    if(group == GetSysSelfGroupIdAPI())
    {
        //控制自身(不包括)
        //PileSetSelfGroupUserCtrlCmdAPI(index, cmd);
        return(TRUE);
    }
    //控制单组电池组组号非法
    else if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    	&& (group != 0xff))
    {
        return(FALSE);
    }

//    //广播所有电池组
//    if(0xff == group)
//    {
//        //记录命令
//        for(i = 0; i < gBsuSysRunPara_57[eRunPara57_ManagGNum]; i++)
//        {
//            //记录命令
//            for(j = 0; j < len; j++)
//            {
//				if(i < GetSysSelfGroupIdAPI())
//				{
//					gGUserCmdCtrl_870[i][index + j] = cmd[j];
//				}
//				else if(group > GetSysSelfGroupIdAPI())
//				{
//					gGUserCmdCtrl_870[i - 1][index + j] = cmd[j];
//				}
//				else
//				{
//					;
//				}
//            }
//        }
//
//        //控制自身(不包括)
//        //PileSetSelfGroupUserCtrlCmdAPI(index, len, cmd);
//    }
//    else
//    {
//        //记录命令
//        for(j = 0; j < len; j++)
//        {
//			//记录命令
//			if(group < GetSysSelfGroupIdAPI())
//			{
//				gGUserCmdCtrl_870[group][index + j] = cmd[j];
//			}
//			else if(group > GetSysSelfGroupIdAPI())
//			{
//				gGUserCmdCtrl_870[group - 1][index + j] = cmd[j];
//			}
//        }
//    }

    fun = 2;                                                //功能码(请求修改)
    saddr = (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev];   //堆控设备地址
    daddr = ChangPileGIdToGAddrAPI(group);                  //主控目标地址

    msg.ID = (((u32)fun & 0x0f) << 25) | ((u32)0x01 << 24) | ((u32)87 << 16)
                | ((u32)daddr << 8) | (u32)saddr;

    for(i = 0; i < ((len + 2) / 3); i++)
    {
		for(j = 0; j < 3; j++)
		{
			msg.Data[2 + j * 2] = (u8)(cmd[i * 3 + j] & 0xff);
			msg.Data[3 + j * 2] = (u8)((cmd[i * 3 + j] >> 8) & 0xff);
			if((i * 3 + j) >= (len - 1))
			{
				break;
			}
		}

		msg.Data[0] = 0xc0 | (j + 1);	//子索引个数
		msg.Data[1] = index + i * 3;    //起始索引

		msg.Len = 2 + (j + 1) * 2;

		(void)BSPCANSendQin(eCAN0,&msg);
    }

    return(TRUE);
}

//=============================================================================================
//函数名称	: u8 PileSendDnGroupSysDebugCmdMsg_API(u8 group, u8 index, u8 len, u16 *cmd)
//输入参数	: group:组号[0,15] 0xff:广播所有组 index:起始子索引号[0,7] len:子索引长度[0, 8] *cmd:命令
//输出参数	: TRUE:成功 FALSE:失败
//函数功能	: 堆控向电池组下发系统调试命令报文
//注意事项	: 向CAN0发报文(主索引88,不包括控制自身所在组)
//=============================================================================================
u8 PileSendDnGroupSysDebugCmdMsg_API(u8 group, u8 index, u8 len, u16 *cmd)
{
    u8 i = 0;
    u8 j = 0;
    u8 fun = 0;
    u8 saddr = 0;
    u8 daddr = 0;
    t_CANMSG msg;

    //子索引错误
    if(((index + len) > GINDEX88_LEN) || (0 == len))
    {
        return(FALSE);
    }

    //控制自身电池组
    if(group == GetSysSelfGroupIdAPI())
    {
        //控制自身(不包括)
        //PileSetSelfGroupSysDebugCmdAPI(index, cmd);
        return(TRUE);
    }
    //控制单组电池组组号非法
    else if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    	&& (group != 0xff))
    {
        return(FALSE);
    }

//    //广播所有电池组
//    if(0xff == group)
//    {
//        //记录命令
//        for(i = 0; i < gBsuSysRunPara_57[eRunPara57_ManagGNum]; i++)
//        {
//            //记录命令
//            for(j = 0; j < len; j++)
//            {
//				if(i < GetSysSelfGroupIdAPI())
//				{
//					gGDebugCmdCtrl_880[i][index + j] = cmd[j];
//				}
//				else if(group > GetSysSelfGroupIdAPI())
//				{
//					gGDebugCmdCtrl_880[i - 1][index + j] = cmd[j];
//				}
//				else
//				{
//					;
//				}
//            }
//        }
//
//        //控制自身(不包括)
//        //PileSetSelfGroupSysDebugCmdAPI(index, len, cmd);
//    }
//    else
//    {
//        //记录命令
//        for(j = 0; j < len; j++)
//        {
//			//记录命令
//			if(group < GetSysSelfGroupIdAPI())
//			{
//				gGDebugCmdCtrl_880[group][index + j] = cmd[j];
//			}
//			else if(group > GetSysSelfGroupIdAPI())
//			{
//				gGDebugCmdCtrl_880[group - 1][index + j] = cmd[j];
//			}
//        }
//    }

    fun = 2;                                                //功能码(请求修改)
    saddr = (u8)gPSmuGenParaR_102[eSmuGenPara102_SmuDev];   //堆控设备地址
    daddr = ChangPileGIdToGAddrAPI(group);                  //主控目标地址

    msg.ID = (((u32)fun & 0x0f) << 25) | ((u32)0x01 << 24) | ((u32)88 << 16)
                | ((u32)daddr << 8) | (u32)saddr;

    for(i = 0; i < ((len + 2) / 3); i++)
    {
		for(j = 0; j < 3; j++)
		{
			msg.Data[2 + j * 2] = (u8)(cmd[i * 3 + j] & 0xff);
			msg.Data[3 + j * 2] = (u8)((cmd[i * 3 + j] >> 8) & 0xff);
			if((i * 3 + j) >= (len - 1))
			{
				break;
			}
		}

		msg.Data[0] = 0xc0 | (j + 1);	//子索引个数
		msg.Data[1] = index + i * 3;    //起始索引

		msg.Len = 2 + (j + 1) * 2;

		(void)BSPCANSendQin(eCAN0,&msg);
    }

    return(TRUE);
}

/*=================================================================================================
** @Name      : void PileRcvCanGSingleInfoDeal(u8 group, u8 index, u8 len, u8 canNum)
** @Input     : group,:电池组号 index:主索引号 *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收下级电池组单帧信息CAN报文处理
** @The notes : 接收周期上传单帧信息调用
**===============================================================================================*/
void PileRcvCanGSingleInfoDeal(u8 group, u8 index, u8 *pdata, u8 DLC, u8 canNum)
{
    //U16/S16数据主索引
    if((index >= 50) && (index <= 84))
    {
    	//接收U16数据类型单帧信息处理
    	RcvCanGroupSingleInfoU16(group, index, pdata, DLC);
    }
    //U32数据主索引
    else if((index >= 73) && (index <= 74))
    {
    	//接收U32数据类型单帧信息处理
    	RcvCanGroupSingleInfoU32(group, index, pdata, DLC);
    }
}

/*=================================================================================================
** @Name      : void PileRcvCanGLongInfoDeal(u8 group, u8 index, u8 len, u8 canNum)
** @Input     : group,:电池组号 index:主索引号 *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收下级电池组长帧信息CAN报文处理
** @The notes : 接收周期上传长帧信息调用
**===============================================================================================*/
void PileRcvCanGLongInfoDeal(u8 group, u8 index, u8 *pdata, u8 DLC, u8 canNum)
{
    //U16/S16数据主索引
    if((index >= 82) && (index <= 82))
    {
    	//接收U16数据类型单帧信息处理
    	RcvCanGroupLongInfoU16(group, index, pdata, DLC);
    }
}

/*=================================================================================================
** @Name      : RcvCanGroupSingleInfoU16(u8 group, u8 index, u8 *pdata, u8 DLC)
** @Input     : group:组号 index:主索引号 *pdata:CAN数据域  DLC:数据域长度
** @Output    : void
** @Function  : 接收U16数据类型单帧信息CAN报文处理
** @The notes : 接收U16类型单帧信息调用
**===============================================================================================*/
void RcvCanGroupSingleInfoU16(u8 group, u8 index, u8 *pdata, u8 DLC)
{
    u8 i = 0;
    u8 sindexLen = 0;
    u16 *pInfo;

    /*组号错误*/
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
    	return;
    }

    //组号转化为接收缓冲区索引号(接收缓存区不包括自身信息)
	if(group > GetSysSelfGroupIdAPI())
	{
		group = group - 1;
	}
    //组号为自身信息不解析(辅助MCU启用)
	else if(group == GetSysSelfGroupIdAPI())
	{
		return;
	}

    //主索引50信息
    if(50 == index)
    {
    	//取相应数组
    	pInfo = gGSysStateInfo_500[group];
    	sindexLen = GINDEX50_LEN;
    }
    else if(51 == index)
	{
		//取相应数组
		pInfo = gGLimCPInfo_510[group];
    	sindexLen = GINDEX51_LEN;
	}
    else if(52 == index)
	{
		//取相应数组
		pInfo = gGLimVInfo_520[group];
    	sindexLen = GINDEX52_LEN;
	}
    else if(53 == index)
	{
		//取相应数组
		pInfo = gGAlarmInfo_530[group];
    	sindexLen = GINDEX53_LEN;
	}
    else if(54 == index)
	{
		//取相应数组
		pInfo = gGFaultInfo_540[group];
    	sindexLen = GINDEX54_LEN;
	}
    else if(55 == index)
	{
		//取相应数组
		pInfo = gGErrDetail_550[group];
    	sindexLen = GINDEX55_LEN;
	}
    else if(56 == index)
	{
		//取相应数组
		pInfo = gGPortStateInfo_560[group];
    	sindexLen = GINDEX56_LEN;
	}
    else if(60 == index)
	{
		//取相应数组
		pInfo = (u16*)gGSysMainInfo_600[group];
    	sindexLen = GINDEX60_LEN;
	}
    else if(61 == index)
	{
		//取相应数组
		pInfo = gGSysCapInfo_610[group];
    	sindexLen = GINDEX61_LEN;
	}
    else if(62 == index)
	{
		//取相应数组
		pInfo = (u16*)gGSysCellInfo_620[group];
    	sindexLen = GINDEX62_LEN;
	}
    else if(63 == index)
	{
		//取相应数组
		pInfo = gGVoltCharInfo_630[group];
    	sindexLen = GINDEX63_LEN;
	}
    else if(64 == index)
	{
		//取相应数组
		pInfo = (u16*)gGTempCharInfo_640[group];
    	sindexLen = GINDEX64_LEN;
	}
    else if(70 == index)
	{
		//取相应数组
		pInfo = gGStatistTime_700[group];
    	sindexLen = GINDEX70_LEN;
	}
    else if(71 == index)
	{
		//取相应数组
		pInfo = gGStatistErr_710[group];
    	sindexLen = GINDEX71_LEN;
	}
    else if(72 == index)
	{
		//取相应数组
		pInfo = gGStatistCapEner_720[group];
    	sindexLen = GINDEX72_LEN;
	}
    else if(75 == index)
	{
		//取相应数组
		pInfo = gGStatistDhgDeep_750[group];
    	sindexLen = GINDEX75_LEN;
	}
    else if(80 == index)
	{
		//取相应数组
		pInfo = gGAuthorInfo_800[group];
    	sindexLen = GINDEX80_LEN;
	}
    else if(81 == index)
	{
		//取相应数组
		pInfo = gGCtrlResult_810[group];
    	sindexLen = GINDEX81_LEN;
	}
    else
    {
    	return;
    }

	if(DLC >= 2)
	{
		//获取报文数据
		for(i = 0; i < (DLC / 2); i++)
		{
			if(i < sindexLen)
			{
				pInfo[i] = ((u16)pdata[i * 2 + 1] << 8) | pdata[i * 2];
			}
		}
	}
}

/*=================================================================================================
** @Name      : RcvCanGroupSingleInfoU32(u8 group, u8 index, u8 *pdata, u8 DLC)
** @Input     : group:组号 index:主索引号 *pdata:CAN数据域  DLC:数据域长度
** @Output    : void
** @Function  : 接收U32数据类型单帧信息CAN报文处理
** @The notes : 接收U32类型单帧信息调用
**===============================================================================================*/
void RcvCanGroupSingleInfoU32(u8 group, u8 index, u8 *pdata, u8 DLC)
{
    u8 i = 0;
    u8 sindexLen = 0;
    u32 *pInfo;

    /*组号错误*/
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
    	return;
    }

    //组号转化为接收缓冲区索引号(接收缓存区不包括自身信息)
	if(group > GetSysSelfGroupIdAPI())
	{
		group = group - 1;
	}
    //组号为自身信息不解析(辅助MCU启用)
	else if(group == GetSysSelfGroupIdAPI())
	{
		return;
	}

    //主索引73信息
    if(73 == index)
    {
    	//取相应数组
    	pInfo = gGStatistHisCap_730[group];
    	sindexLen = GINDEX73_LEN;
    }
    else if(74 == index)
	{
		//取相应数组
		pInfo = gGStatistHisEner_740[group];
    	sindexLen = GINDEX74_LEN;
	}
    else
    {
    	return;
    }

	if(DLC >= 4)
	{
		//获取报文数据
		for(i = 0; i < (DLC / 4); i++)
		{
			if(i < sindexLen)
			{
				pInfo[i] = ((u16)pdata[i * 4 + 1] << 8) | pdata[i * 4];
			}
		}
	}
}

/*=================================================================================================
** @Name      : RcvCanGroupLongInfoU16(u8 group, u8 index, u8 *pdata, u8 DLC)
** @Input     : group:组号 index:主索引号 *pdata:CAN数据域  DLC:数据域长度
** @Output    : void
** @Function  : 接收U16数据类型长帧信息CAN报文处理
** @The notes : 接收U16类型长帧信息调用
**===============================================================================================*/
void RcvCanGroupLongInfoU16(u8 group, u8 index, u8 *pdata, u8 DLC)
{
    u8 i = 0;
    u8 len = 0;
    u8 sindex = 0;
    u8 sindexLen = 0;
    u16 *pInfo = 0;

    /*组号错误*/
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
    	return;
    }

    //组号转化为接收缓冲区索引号(接收缓存区不包括自身信息)
	if(group > GetSysSelfGroupIdAPI())
	{
		group = group - 1;
	}
    //组号为自身信息不解析(辅助MCU启用)
	else if(group == GetSysSelfGroupIdAPI())
	{
		return;
	}

    //主索引82信息
    if(82 == index)
    {
    	//取相应数组
    	pInfo = gGDebugResult_820[group];
    	sindexLen = GINDEX82_LEN;
    }
    else
    {
    	return;
    }

	if(DLC >= 4)
	{
		sindex = pdata[1];
		len = pdata[0] & 0x0f;

		//获取报文数据
		for(i = 0; (i * 2 + 4) <= DLC; i++)
		{
			if((i < len) && ((sindex + i) < sindexLen))
			{
				pInfo[sindex + i] = ((u16)pdata[i * 2 + 3] << 8) | pdata[i * 2 + 2];
			}
		}
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
