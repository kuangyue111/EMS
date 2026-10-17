/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CHGHandle.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 快充通信包通信处理接口
** @Instructions : CAN通信
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "CHGFrame.h"
#include "CHGInterface.h"
#include "CHGHandle.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
/*=======================================校时信息=================================================*/
u_CHGErrCode gCHGMsgErrCode;            /*充电机报文通信超时故障码*/
u32 gCHGCountTime = 0;                  /*快充计时(单位:10ms)*/
u8 gCHGNowStage = 0;                    /*当前快充阶段*/

u8 gCHGHandStagStep = 0;                /*握手阶段步骤*/
u8 gCHGCfgStagStep = 0;                 /*配置阶段步骤*/
u8 gCHGChgStagStep = 0;                 /*充电阶段步骤*/

u8 gCHGRcvInfo[eCHGRCV_LEN] =
{
    0,                                  /*接收CHM*/
    0,                                  /*接收CRM*/
    0                                   /**/
};

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void CHGHandStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充握手阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGHandStageSendMsg(void);

/*=================================================================================================
** @Name      : static void CHGCfgStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充参数配置阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGCfgStageSendMsg(void);

/*=================================================================================================
** @Name      : static void CHGChgStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充充电阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGChgStageSendMsg(void);

/*=================================================================================================
** @Name      : static void CHGStopStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充终止阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGStopStageSendMsg(void);

/*=================================================================================================
** @Name      : static void CHGFinStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充结束阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGFinStageSendMsg(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CHGLibSendMsgTask(void)
** @Input     : void
** @Output    : void
** @Function  : 快充通信包通信报文发送任务
** @The notes : 必须10ms周期调用
**===============================================================================================*/
void CHGLibSendMsgTask(void)
{
    u8 i = 0;

	/*快充计时*/
	gCHGCountTime++;

    /*不同阶段发送发送不同报文(每个阶段出现异常时均发送错误报文BEM)*/
    switch(gCHGNowStage)
    {
        /*空闲阶段段*/
        case eCHGSTAG_IDLE:
            /*不发送*/
            break;

        /*连接阶段*/
        case eCHGSTAG_LINK:
            /*不发送,数据复位*/
        	for(i = 0; i < eCHGRCV_LEN; i++)
        	{
        		gCHGRcvInfo[i] = 0;
        	}

        	gCHGCountTime = 0;
        	gCHGHandStagStep = 0;
        	gCHGCfgStagStep = 0;
        	gCHGChgStagStep = 0;
        	gCHGMsgErrCode.Word = 0;
            break;

        /*握手阶段*/
        case eCHGSTAG_HAND:
            /*发送BHM和BRM*/
        	CHGHandStageSendMsg();
            break;

        /*配置阶段*/
        case eCHGSTAG_CFG:
            /*发送BCP和BRO*/
        	CHGCfgStageSendMsg();
            break;

        /*充电阶段*/
        case eCHGSTAG_CHG:
            /*发送BCL/BCS和BSM/BMV/BMT*/
        	CHGChgStageSendMsg();
            break;

        /*终止阶段*/
        case eCHGSTAG_STO:
            /*发送BST*/
            CHGStopStageSendMsg();
            break;

        /*结束阶段*/
        case eCHGSTAG_FIN:
            /*发送BSD*/
        	CHGFinStageSendMsg();

        	/*清除接收到的握手信息防止再次走流程*/
        	for(i = 0; i <= eCHGRCV_CCS; i++)
        	{
        		gCHGRcvInfo[i] = 0;
        	}
            break;

        /*无效阶段*/
        default:
            /*不发送*/
            break;
    }
}

/*=================================================================================================
** @Name      : void CHGLibRcvMsgHandleAPI(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 快充通信报文接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void CHGLibRcvMsgHandleAPI(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
    u16 maxVolt = 0;
    u16 minVolt = 0;
    u16 minCurr = 0;

	/*非充电桩通信口报文*/
	if(CHG_CAN_NUM != canNum)
	{
		return;
	}

    /*根据CAN ID处理报文*/
    switch(ID)
    {
        /*接收到CHM*/
        case 0x1826f456:
        	CHGInputRcvChgerInfoAPI(eCHGRCV_CHM, 1);
            break;

        /*接收到CRM*/
        case 0x1801F456:
            /*接收到CRM确认*/
        	if(0xAA == pdata[0])
        	{
                CHGInputRcvChgerInfoAPI(eCHGRCV_CRM, CHG_RCV_CRM_CON);
        	}
            /*接收到CRM报文*/
        	else
        	{
                CHGInputRcvChgerInfoAPI(eCHGRCV_CRM, CHG_RCV_CRM_MSG);
        	}
            break;

        /*接收到CML*/
        case 0x1808F456:
        	/*解析充电桩参数是否适合*/
            maxVolt = ((u16)pdata[1] << 8) | pdata[0];
            minVolt = ((u16)pdata[3] << 8) | pdata[2];
            minCurr = ((u16)pdata[7] << 8) | pdata[6];
            /*最小充电电流0.1A,偏移-400A(带负号)*/
            if(minCurr >= 4000)
            {
            	/*超过400A>0表示3放电*/
            	minCurr = 0;
            }
            else
            {
            	/*充电负号转化为正数*/
            	minCurr = 4000 - minCurr;
            }

            /*电池电压必须在充电桩电压之间,充电桩最小电流小于充电电流限值*/
            if((minVolt <= CHGGetGroupSumVolt_Hook()) && ((CHGGetGroupSumVolt_Hook() + 100) <= maxVolt)
            	&& (((minCurr + 50) <= CHGGetGroupReqCurrLim_Hook()) || (0 == CHGGetGroupReqCurrLim_Hook())))
            {
                CHGInputRcvChgerInfoAPI(eCHGRCV_CML, CHG_RCV_CML_CON);
            }
            else
            {
                CHGInputRcvChgerInfoAPI(eCHGRCV_CML, CHG_RCV_CML_MSG);
            }
            break;

        /*接收到CRO*/
        case 0x100AF456:
            /*接收到CRO确认*/
        	if(0xAA == pdata[0])
        	{
                CHGInputRcvChgerInfoAPI(eCHGRCV_CRO, CHG_RCV_CRO_CON);
        	}
            /*接收到CRO报文*/
        	else
        	{
                CHGInputRcvChgerInfoAPI(eCHGRCV_CRO, CHG_RCV_CRO_MSG);
        	}
            break;

        /*接收到CCS*/
        case 0x1812F456:
            CHGInputRcvChgerInfoAPI(eCHGRCV_CCS, 1);
            break;

        /*接收到CST*/
        case 0x101AF456:
            CHGInputRcvChgerInfoAPI(eCHGRCV_CST, 1);
            if((pdata[0] > 0) || (pdata[1] > 0)
            	|| ((pdata[2] & 0x0f) > 0) || ((pdata[3] & 0x0f) > 0))
            {
            	CHGInputRcvChgerInfoAPI(eCHGRCV_STOP, 1);
            }
            break;

        /*接收到CSD*/
        case 0x181DF456:
            CHGInputRcvChgerInfoAPI(eCHGRCV_CSD, 1);
            break;

        /*接收到长帧回复*/
        case 0x1CECF456:
            /*握手阶段长帧BRM*/
        	if(eCHGSTAG_HAND == gCHGNowStage)
        	{
				/*接收到长帧BRM应答(允许接收长帧)*/
				if(0x11 == pdata[0])
				{
					CHGInputRcvChgerInfoAPI(eCHGRCV_ABRM, 1);
				}
				/*接收到长帧BRM确认(长帧接收完毕)*/
				else if(0x13 == pdata[0])
				{
					CHGInputRcvChgerInfoAPI(eCHGRCV_CBRM, 1);
				}
        	}
            /*配置阶段长帧BCP*/
        	else if(eCHGSTAG_CFG == gCHGNowStage)
        	{
				/*接收到长帧BCP应答(允许接收长帧)*/
				if(0x11 == pdata[0])
				{
					CHGInputRcvChgerInfoAPI(eCHGRCV_ABCP, 1);
				}
				/*接收到长帧BCP确认(长帧接收完毕)*/
				else if(0x13 == pdata[0])
				{
					CHGInputRcvChgerInfoAPI(eCHGRCV_CBCP, 1);
				}
        	}
            /*充电阶段长帧BCS*/
        	else if(eCHGSTAG_CHG == gCHGNowStage)
        	{
				/*接收到长帧BCS应答(允许接收长帧)*/
				if(0x11 == pdata[0])
				{
					CHGInputRcvChgerInfoAPI(eCHGRCV_ABCS, 1);
				}
				/*接收到长帧BCS确认(长帧接收完毕)*/
				else if(0x13 == pdata[0])
				{
					CHGInputRcvChgerInfoAPI(eCHGRCV_CBCS, 1);
				}
        	}
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : void CHGSetNowStageAPI(u8 stage)
** @Input     : stage:当前充电阶段值
** @Output    : void
** @Function  : 设置当前充电阶段值
** @The notes :
**===============================================================================================*/
void CHGSetNowStageAPI(u8 stage)
{
	gCHGNowStage = stage;
}

/*=================================================================================================
** @Name      : void CHGInputRcvChgerInfoAPI(e_CHGRcvMsg index, u8 info)
** @Input     : index:信息索引 info:接收信息
** @Output    : void
** @Function  : 输入接收充电桩报文状态和信息
** @The notes :
**===============================================================================================*/
void CHGInputRcvChgerInfoAPI(e_CHGRcvMsg index, u8 info)
{
	if(index < eCHGRCV_LEN)
	{
		gCHGRcvInfo[index] = info;
	}
}

/*=================================================================================================
** @Name      : u8 CHGGetRcvChgerInfoAPI(u8 index)
** @Input     : index:信息索引
** @Output    : 接收充电桩报文状态和信息
** @Function  : 获取接收充电桩报文状态和信息
** @The notes :
**===============================================================================================*/
u8 CHGGetRcvChgerInfoAPI(u8 index)
{
	if(index < eCHGRCV_LEN)
	{
		return(gCHGRcvInfo[index]);
	}
	else
	{
		return(0);
	}
}

/*=================================================================================================
** @Name      : void CHGMarkRcvCSTTimeOutErrCodeAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 记录接收充电桩充电终止报文CST超时故障码
** @The notes :
**===============================================================================================*/
void CHGMarkRcvCSTTimeOutErrCodeAPI(void)
{
	gCHGMsgErrCode.Bits.CST_STOP_OT = 1;
}

/*=================================================================================================
** @Name      : void CHGMarkRcvCSDTimeOutErrCodeAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 记录接收充电桩充电统计报文CSD超时故障码
** @The notes :
**===============================================================================================*/
void CHGMarkRcvCSDTimeOutErrCodeAPI(void)
{
	gCHGMsgErrCode.Bits.CSD_STAT_OT = 1;
}

/*=================================================================================================
** @Name      : u32 CHGGetChgMsgErrCodeAPI(void)
** @Input     : void
** @Output    : 通信报文超时故障码
** @Function  : 获取充电桩通信报文超时故障码
** @The notes :
**===============================================================================================*/
u32 CHGGetChgMsgErrCodeAPI(void)
{
	return((u32)gCHGMsgErrCode.Word);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void CHGHandStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充握手阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGHandStageSendMsg(void)
{
	static u8 sTime = 0;
	static u32 sErrTime = 0;

	switch(gCHGHandStagStep)
	{
        /*发送BHM(单帧)*/
        case 0:
        	CHGBMSSendBHMMsg();
        	sErrTime = gCHGCountTime;
        	gCHGHandStagStep = 1;
	        sTime = 0;
	        break;

	    /*检测充电桩回复*/
	    case 1:
	        /*收到充电桩CRM(第一次请求辨识报文)*/
	        if(CHG_RCV_CRM_MSG == gCHGRcvInfo[eCHGRCV_CRM])
	        {
	        	/*发送BRM首帧TPCM_RTS*/
	        	CHGBMSSendBRMMsg(0);

	        	/*进入发送BRM*/
	        	gCHGHandStagStep = 2;
	        	sErrTime = gCHGCountTime;
                sTime = 0;
	        }
		    /*未收到充电桩CHM*/
	        else
	        {
	        	/*发送BHM,250ms周期*/
	        	if(sTime >= 25)
	        	{
	        		/*60s超时*/
	        		if((gCHGCountTime - sErrTime) >= 6000)
	        		{
	        			/*设置错误码,发送错误报文*/
	        			gCHGMsgErrCode.Bits.CRM_00_OT = 1;
	                    CHGBMSSendBEMMsg();
	        		}
	        		else
	        		{
	        			/*未收到应答*/
	        			/*发送BHM报文*/
	                    CHGBMSSendBHMMsg();
	        		}
	                sTime = 0;
	        	}
	        	else
	        	{
	        		sTime++;
	        	}
	        }
	        break;

		/*发送BRM(多帧)*/
	    case 2:
	    	/*未收到充电桩CRM(第二次握手辨识确认)或未收到BRM长帧结束应答消息TPCM_EM*/
	        if((CHG_RCV_CRM_CON != gCHGRcvInfo[eCHGRCV_CRM]) || (1 != gCHGRcvInfo[eCHGRCV_CBRM]))
	        {
	        	/*5s超时*/
	        	if((gCHGCountTime - sErrTime) >= 500)
	        	{
	                /*设置错误码,发送错误报文*/
	        		if(0 == sTime )
	        		{
	        			gCHGMsgErrCode.Bits.CRM_AA_OT = 1;
	                    CHGBMSSendBEMMsg();
	        		}
	        	}
	        	else
	        	{
	        		/*未收到充电桩BRM长帧应答TPCM_CTS(响应者)*/
	        		if(1 != gCHGRcvInfo[eCHGRCV_ABRM])
	        		{
	        			/*250ms周期*/
	        			if(0 == sTime)
	        			{
	        		        /*发送BRM长帧首帧TPCM_RTS*/
		                    (void)CHGBMSSendBRMMsg(0);
	        			}
	        		}
	        		/*收到充电桩BRM长帧应答TPCM_CTS(响应者)*/
	        		else
	        		{
	        		    /*发送BRM长帧后续帧TPCM_DT*/
		                if(TRUE == CHGBMSSendBRMMsg(1))
		                {
		                	/*长帧完成清空应答等待是否需要下一轮*/
		                	gCHGRcvInfo[eCHGRCV_ABRM] = 0;
		                }
	        		}
	        	}
		        sTime++;

	        	/*250ms周期*/
	        	if(sTime >= 25)
	        	{
                    sTime = 0;
	        	}
	        }
		    break;

		/*其他值*/
	    default:
        	gCHGHandStagStep = 0;
	        sTime = 0;
	        break;
	}
}

/*=================================================================================================
** @Name      : static void CHGCfgStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充参数配置阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGCfgStageSendMsg(void)
{
	static u8 sTime = 0;
	static u32 sErrTime = 0;

	switch(gCHGCfgStagStep)
	{
        /*发送BCP(多帧)*/
        case 0:
        	/*发送BCP长帧首帧TPCM_RTS*/
        	(void)CHGBMSSendBCPMsg(0);
        	sErrTime = gCHGCountTime;
        	gCHGCfgStagStep = 1;
	        sTime = 0;
	        break;

		/*发送BCP(多帧)*/
	    case 1:
	    	/*未收到充电桩CML或未收到BCP长帧结束应答消息TPCM_EM*/
	        if((CHG_RCV_CML_CON != gCHGRcvInfo[eCHGRCV_CML]) || (1 != gCHGRcvInfo[eCHGRCV_CBCP]))
	        {
	        	/*5s超时*/
	        	if((gCHGCountTime - sErrTime) >= 500)
	        	{
	                /*设置错误码,发送错误报文*/
	        		if(0 == sTime )
	        		{
	        			gCHGMsgErrCode.Bits.CML_MAXC_OT = 1;
	                    CHGBMSSendBEMMsg();
	        		}
	        	}
	        	else
	        	{
	        		/*未收到充电桩BCP长帧应答TPCM_CTS(响应者)*/
	        		if(1 != gCHGRcvInfo[eCHGRCV_ABCP])
	        		{
	        			/*500ms周期*/
	        			if(0 == sTime)
	        			{
	        		        /*发送BRM长帧首帧TPCM_RTS*/
		                    (void)CHGBMSSendBCPMsg(0);
	        			}
	        		}
	        		/*收到充电桩BCP长帧应答TPCM_CTS(响应者)*/
	        		else
	        		{
	        		    /*发送BCP长帧后续帧TPCM_DT*/
		                if(TRUE == CHGBMSSendBCPMsg(1))
		                {
		                	/*长帧完成清空应答等待是否需要下一轮*/
		                	gCHGRcvInfo[eCHGRCV_ABCP] = 0;
		                }
	        		}
	        	}
		        sTime++;

	        	/*500ms周期*/
	        	if(sTime >= 50)
	        	{
                    sTime = 0;
	        	}
	        }
	    	/*长帧BCP传输完成收到充电桩CML*/
	        else
	        {
	        	/*发送BRO*/
	        	CHGBMSSendBROMsg();
		       	gCHGCfgStagStep = 2;
	        	sErrTime = gCHGCountTime;
			    sTime = 0;
	        }
		    break;

	    /*发送BRO(单帧)*/
	    case 2:
	        /*未收到充电桩CRO确认0xAA*/
	        if(CHG_RCV_CRO_CON != gCHGRcvInfo[eCHGRCV_CRO])
	        {
	        	/*发送BRO,250ms周期*/
	        	if(sTime >= 25)
	        	{
	        		/*报文未接收5s超时,接收未确认60s超时*/
	        		if((((gCHGCountTime - sErrTime) >= 500) && (CHG_RCV_CRO_MSG != gCHGRcvInfo[eCHGRCV_CRO])) || ((gCHGCountTime - sErrTime) >= 6000))
	        		{
	        			/*设置错误码,发送错误报文*/
	        			gCHGMsgErrCode.Bits.CRO_READY_OT = 1;
	                    CHGBMSSendBEMMsg();
	        		}
	        		else
	        		{
	        			/*未收到应答*/
	        			/*发送BRO报文*/
	        			CHGBMSSendBROMsg();
	        		}
	                sTime = 0;
	        	}
	        	else
	        	{
	        		sTime++;
	        	}
	        }
		    break;

		/*其他值*/
	    default:
        	gCHGCfgStagStep = 0;
	        sTime = 0;
	        break;
	}
}

/*=================================================================================================
** @Name      : static void CHGChgStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充充电阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGChgStageSendMsg(void)
{
	static u8 sTime = 0;
	static u32 sErrTime = 0;

	switch(gCHGChgStagStep)
	{
        /*发送BCL(单帧)*/
        case 0:
        	/*发送BCL单帧*/
        	CHGBMSSendBCLMsg();
        	sErrTime = gCHGCountTime;
        	gCHGChgStagStep = 1;
	        sTime = 0;
	        break;

		/*发送BCL单帧和BCS(多帧)*/
	    case 1:
	    	/*未收到充电桩CML或未收到BCS长帧结束应答消息TPCM_EM*/
	        if((1 != gCHGRcvInfo[eCHGRCV_CCS]) || (1 != gCHGRcvInfo[eCHGRCV_CBCS]))
	        {
	        	/*1s超时*/
	        	if((gCHGCountTime - sErrTime) >= 100)
	        	{
	                /*首次超时或间隔250ms设置错误码,发送错误报文*/
	        		if(((gCHGCountTime - sErrTime) == 100) || (0 == sTime ))
	        		{
	        			gCHGMsgErrCode.Bits.CCS_CHGS_OT = 1;
	                    CHGBMSSendBEMMsg();
	        		}
	        	}
	        	else
	        	{
	        		/*50ms周期发送BCL*/
	        		if(49 == (sTime % 50))
	        		{
	        			CHGBMSSendBCLMsg();
	        		}

	        		/*未收到充电桩BCS长帧应答TPCM_CTS(响应者)*/
	        		if(1 != gCHGRcvInfo[eCHGRCV_ABCS])
	        		{
	        			/*250ms周期*/
	        			if(0 == sTime)
	        			{
	        		        /*发送BCS长帧首帧TPCM_RTS*/
		                    (void)CHGBMSSendBCSMsg(0);
	        			}
	        		}
	        		/*收到充电桩BCS长帧应答TPCM_CTS(响应者)*/
	        		else
	        		{
	        		    /*发送BCS长帧后续帧TPCM_DT*/
		                if(TRUE == CHGBMSSendBCSMsg(1))
		                {
		                	/*长帧完成清空应答等待是否需要下一轮*/
		                	gCHGRcvInfo[eCHGRCV_ABCS] = 0;
		                }
	        		}
	        	}
		        sTime++;

	        	/*250ms周期*/
	        	if(sTime >= 25)
	        	{
                    sTime = 0;
	        	}
	        }
	    	/*长帧BCS传输完成收到充电桩CCS*/
	        else
	        {
	        	/*发送BSM(单帧)*/
	        	CHGBMSSendBSMMsg();
	        	gCHGChgStagStep = 2;
	        	sErrTime = gCHGCountTime;
		        sTime++;
	        }
		    break;

	    /*等待250ms周期*/
	    case 2:
    		/*50ms周期发送BCL*/
    		if(4 == (sTime % 5))
    		{
    			CHGBMSSendBCLMsg();
    		}

	        sTime++;
	        /*达到250周期*/
	        if(sTime >= 24)
	        {
	        	/*清除本次接收CCS和BCS应答*/
	        	gCHGRcvInfo[eCHGRCV_CCS] = 0;
	        	gCHGRcvInfo[eCHGRCV_ABCS] = 0;
	        	gCHGRcvInfo[eCHGRCV_CBCS] = 0;

		        /*返回*/
	        	gCHGChgStagStep = 0;
	        	sErrTime = 0;
		        sTime = 0;
	        }
		    break;

		#if 0
	    /*发送BMV(可选长帧)*/
	    case 2:
	    	/*继续计时*/
	        sTime++;

	    	/*未收到BMV长帧结束应答消息TPCM_EM*/
	        if(1 != gCHGRcvInfo[eCHGRCV_CBMV])
	        {
	        	/*未收到充电桩BMV长帧应答TPCM_CTS(响应者)*/
	        	if(1 != gCHGRcvInfo[eCHGRCV_ABMV])
	        	{
	        		/*发送BCS长帧首帧TPCM_RTS*/
		            (void)CHGBMSSendBMVMsg(0);
	       		}
	       		/*收到充电桩BCS长帧应答TPCM_CTS(响应者)*/
	       		else
	        	{
	        	    /*发送BCS长帧后续帧TPCM_DT*/
		            if(TRUE == CHGBMSSendBMVMsg(1))
		            {
		               	/*长帧完成清空应答等待是否需要下一轮*/
		               	gCHGRcvInfo[eCHGRCV_ABMV] = 0;
		            }
	        	}
	        }
	    	/*长帧BMV传输完成*/
	        else
	        {
	        	/*发送BMT首帧(多帧)*/
	            (void)CHGBMSSendBMTMsg(0);
		       	gCHGChgStagStep = 3;
	        	sErrTime = gCHGCountTime;
			    sTime++;
	        }
		    break;
			#endif

		/*其他值*/
	    default:
        	gCHGChgStagStep = 0;
	        sTime = 0;
	        break;
	}
}

/*=================================================================================================
** @Name      : static void CHGStopStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充终止阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGStopStageSendMsg(void)
{
	/*10ms周期发送BST*/
    CHGBMSSendBSTMsg();
}

/*=================================================================================================
** @Name      : static void CHGFinStageSendMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 快充结束阶段报文发送任务
** @The notes : 主任务10ms周期调用
**===============================================================================================*/
static void CHGFinStageSendMsg(void)
{
	static u8 sTime = 25;

	/*250ms周期发送BST*/
	if(sTime >= 25)
	{
        CHGBMSSendBSDMsg();
        sTime = 0;
	}
	else
	{
		/*有故障则250ms周期发送BEM*/
		if((10 ==sTime) && (gCHGMsgErrCode.Word > 0))
		{
			CHGBMSSendBEMMsg();
		}

		sTime++;
	}
}



