/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CHGFrame.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 快充通信包帧格式接口
** @Instructions : CAN通信
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "CHGInterface.h"
#include "CHGFrame.h"

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
** @Name      : void CHGBMSSendBHMMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 发送BHM报文(车辆端电池最大允许充电电压)
** @The notes : 握手阶段,250ms周期,单帧
**===============================================================================================*/
void CHGBMSSendBHMMsg(void)
{
	u32 ID = 0;
	u16 value = 0;
	u8 data[8] = {0};

	/*BHM报文ID*/
	ID = 0x182756f4;

	/*BHM报文数据域*/
	/*最高允许电压*/
	value = CHGGetGroupChgVoltLim_Hook();
	data[0] = value & 0xff;
	data[1] = (value >> 8) & 0xff;
	data[2] =  0xff;
	data[3] =  0xff;
	data[4] =  0xff;
	data[5] =  0xff;
	data[6] =  0xff;
	data[7] =  0xff;

	/*发送报文*/
	CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);
}

/*=================================================================================================
** @Name      : u8 CHGBMSSendBRMMsg(u8 time)
** @Input     : time:时序(0:首帧 1:剩余帧)
** @Output    : void
** @Function  : 发送BRM报文(电池多包数据)
** @The notes : 握手阶段,250ms周期,多帧
**===============================================================================================*/
u8 CHGBMSSendBRMMsg(u8 time)
{
	u32 ID = 0;
	u16 value = 0;
	u8 data[8] = {0};
	static u8 sTime = 0;

	/*发送首帧TPCM_RTS*/
	if(0 == time)
	{
		/*发送者BRM报文ID*/
		ID = 0x1CEC56F4;

		/*发送者BRM报文数据域*/
		data[0] = 0x10;                 /*协议控制字(16:表示请求发送多包数据)*/
		data[1] = 0x29;
		data[2] = 0x00;                 /*0x0029:全部数据 41 字节*/
		data[3] = 0x06;                 /*表示要发 6 包数据*/
		data[4] = 0xff;                 /*保留给SAE*/
		data[5] = 0x00;
		data[6] = 0x02;
		data[7] = 0x00;                 /*0x000200:参数群编号*/

		/*发送报文*/
		sTime = 0;
		CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

		return(FALSE);
	}
	/*发送剩余帧TPCM_DT*/
	else
	{
        /*第二帧*/
		if(0 == sTime)
		{
	        /*BRM起始报文ID*/
	        ID = 0x1CEB56F4;

            /*BRM报文数据域*/
			data[0] = 0x01;             /*第1包数据*/

			data[1] = 0x01;
			data[2] = 0x00;
			data[3] = 0x01;             /*0x010001:通信协议版本号*/

			data[4] = 0x03;             /*电池类型,磷酸铁锂电池*/

            /*电池额定容量0.1AH*/
			value = CHGGetGroupRatedCap_Hook();
			data[5] = value & 0xff;
			data[6] = (value >> 8) & 0xff;

            /*电池额定电压0.1V*/
			value = CHGGetGroupRatedVolt_Hook();
			data[7] = value & 0xff;     /*低位，高位在下一帧*/

			/*发送报文*/
			sTime = 1;
			CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

			return(FALSE);
		}
	    /*第三帧*/
		else if(1 == sTime)
		{
	        /*BRM中间报文ID*/
	        ID = 0x1CEB56F4;

	        /*BRM报文数据域*/
	        data[0] = 0x02;             /*第2包数据*/

	        /*电池额定电压0.1V*/
			value = CHGGetGroupRatedVolt_Hook();
			data[1] = (value >> 8) & 0xff;/*高位，低位在上一帧*/

			data[2] = 0xff;
			data[3] = 0xff;
			data[4] = 0xff;
			data[5] = 0xff;
			data[6] = 0xff;
			data[7] = 0xff;

			/*发送报文*/
			sTime = 2;
			CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

			return(FALSE);
		}
	    /*第四/五/六/七/八帧*/
		else
		{
	        /*BRM中间报文ID*/
	        ID = 0x1CEB56F4;

	        /*BRM报文数据域*/
	        data[0] = sTime + 1;        /*第3/4/5/6/7包数据*/

			data[1] = 0xff;
			data[2] = 0xff;
			data[3] = 0xff;
			data[4] = 0xff;
			data[5] = 0xff;
			data[6] = 0xff;
			data[7] = 0xff;

			/*发送报文*/
			sTime++;
			CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

			/*最后一帧*/
			if(sTime >= 7)
			{
				sTime = 0;
			    return(TRUE);
			}
			/*中间帧*/
			else
			{
			    return(FALSE);
			}
		}
	}
}

/*=================================================================================================
** @Name      : u8 CHGBMSSendBCPMsg(u8 time)
** @Input     : time:时序(0:首帧 1:剩余帧)
** @Output    : void
** @Function  : 发送BCP报文(参数配置多包数据)
** @The notes : 参数配置阶段,500ms周期,多帧
**===============================================================================================*/
u8 CHGBMSSendBCPMsg(u8 time)
{
	u32 ID = 0;
	u16 value = 0;
	u8 data[8] = {0};
	static u8 sTime = 0;

	/*发送首帧TPCM_RTS*/
	if(0 == time)
	{
		/*发送者BCP报文ID*/
		ID = 0x1CEC56F4;

		/*发送者BCP报文数据域*/
		data[0] = 0x10;                 /*协议控制字(16:表示请求发送多包数据)*/
		data[1] = 0x0d;
		data[2] = 0x00;                 /*0x000d:全部数据 13 字节*/
		data[3] = 0x02;                 /*表示要发 2 包数据*/
		data[4] = 0xff;                 /*保留给SAE*/
		data[5] = 0x00;
		data[6] = 0x06;
		data[7] = 0x00;                 /*0x000600:参数群编号*/

		/*发送报文*/
		sTime = 0;
		CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

		return(FALSE);
	}
	/*发送剩余帧TPCM_DT*/
	else
	{
        /*第二帧*/
		if(0 == sTime)
		{
	        /*BCP中间报文ID*/
	        ID = 0x1CEB56F4;

            /*BCP报文数据域*/
			data[0] = 0x01;             /*第1包数据*/

			value = CHGGetGroupChgCellVLim_Hook();
			data[1] = value & 0xff;
			data[2] = (value >> 8) & 0xff;/*充电截止单体电压 1mV*/

			value = (u16)((s16)CHGGetGroupChgCurrLim_Hook() - 4000);
			data[3] = value & 0xff;
			data[4] = (value >> 8) & 0xff;/*最大充电电流 0.1A,偏移-400A*/

            /*电池标称电能0.1kWh*/
			value = CHGGetGroupStandEner_Hook();
			data[5] = value & 0xff;
			data[6] = (value >> 8) & 0xff;

			/*最高允许电压*/
		    value = CHGGetGroupChgVoltLim_Hook();
			data[7] = value & 0xff;     /*低位，高位在下一帧*/

			/*发送报文*/
			sTime = 1;
			CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

			return(FALSE);
		}
	    /*第三帧*/
		else
		{
	        /*BCP中间报文ID*/
	        ID = 0x1CEB56F4;

	        /*BCP报文数据域*/
	        data[0] = 0x02;             /*第2包数据*/

	        /*最高允许电压*/
	        value = CHGGetGroupChgVoltLim_Hook();
			data[1] = (value >> 8) & 0xff;/*高位，低位在上一帧*/

			/*最高允许温度 1℃,偏移量-50℃*/
		    value = (u8)(CHGGetGroupChgTempHigLim_Hook() - 50);
			data[2] = value & 0xff;     /*充电高温限值限值*/

            /*当前SOC 0.1%*/
			value = CHGGetGroupSoc_Hook();
			data[3] = value & 0xff;
			data[4] = (value >> 8) & 0xff;

            /*当前总压 0.1V*/
			value = CHGGetGroupSumVolt_Hook();
			data[5] = value & 0xff;
			data[6] = (value >> 8) & 0xff;

			data[7] = 0xff;

			/*发送报文*/
			sTime = 0;
			CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

			return(TRUE);
		}
	}
}

/*=================================================================================================
** @Name      : void CHGBMSSendBROMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 发送BRO报文(BMS发送给充电机表示准备充电就绪)
** @The notes : 参数配置阶段,250ms周期,单帧
**===============================================================================================*/
void CHGBMSSendBROMsg(void)
{
	u32 ID = 0;
	u8 data[8] = {0};

	/*BRO报文ID*/
	ID = 0x100956F4;

	/*BRO报文数据域*/
	/*允许充电*/
	if(1 == CHGGetGroupChgAllowState_Hook())
	{
		/*BMS准备就绪*/
		data[0] = 0xAA;
	}
	else
	{
		/*BMS未就绪*/
		data[0] = 0x00;
	}

	data[1] = 0xff;
	data[2] = 0xff;
	data[3] = 0xff;
	data[4] = 0xff;
	data[5] = 0xff;
	data[6] = 0xff;
	data[7] = 0xff;

	/*发送报文*/
	CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);
}

/*=================================================================================================
** @Name      : void CHGBMSSendBROMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 发送BCL报文(电池充电需求报文)
** @The notes : 充电阶段,50ms周期,单帧
**===============================================================================================*/
void CHGBMSSendBCLMsg(void)
{
	u32 ID = 0;
	u16 value = 0;
	u8 data[8] = {0};

	/*BCL报文ID*/
	ID = 0x181056f4;

	/*BCL报文数据域*/
	/*请求电压需求 0.1V*/
	value = CHGGetGroupReqVoltLim_Hook();
	data[0] = value & 0xff;
	data[1] = (value >> 8) & 0xff;

	/*请求电流需求 0.1A, 偏移-400A*/
	value = (u16)((s16)4000 - (s16)CHGGetGroupReqCurrLim_Hook());
	data[2] = value & 0xff;
	data[3] = (value >> 8) & 0xff;

	/*1恒压模式/2恒流模式*/
	data[4] = 0x02;

	data[5] = 0xff;
	data[6] = 0xff;
	data[7] = 0xff;

	/*发送报文*/
	CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);
}

/*=================================================================================================
** @Name      : u8 CHGBMSSendBCSMsg(u8 time)
** @Input     : time:时序(0:首帧 1:剩余帧)
** @Output    : void
** @Function  : 发送BCP报文(电池充电总状态报文多包数据)
** @The notes : 充电阶段,250ms周期,多帧
**===============================================================================================*/
u8 CHGBMSSendBCSMsg(u8 time)
{
	u32 ID = 0;
	u16 value = 0;
	u8 data[8] = {0};
	static u8 sTime = 0;

	/*发送首帧TPCM_RTS*/
	if(0 == time)
	{
		/*发送者BCS报文ID*/
		ID = 0x1CEC56F4;

		/*发送者BCS报文数据域*/
		data[0] = 0x10;                 /*协议控制字(16:表示请求发送多包数据)*/
		data[1] = 0x09;
		data[2] = 0x00;                 /*0x0009:全部数据 9 字节*/
		data[3] = 0x02;                 /*表示要发 2 包数据*/
		data[4] = 0xff;                 /*保留给SAE*/
		data[5] = 0x00;
		data[6] = 0x11;
		data[7] = 0x00;                 /*0x001100:参数群编号*/

		/*发送报文*/
		sTime = 0;
		CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

		return(FALSE);
	}
	/*发送剩余帧TPCM_DT*/
	else
	{
        /*第二帧*/
		if(0 == sTime)
		{
	        /*BCS中间报文ID*/
	        ID = 0x1CEB56F4;

            /*BCS报文数据域*/
			data[0] = 0x01;             /*第1包数据*/

			/*充电电压测量值 0.1V*/
			value = CHGGetGroupChgInputVolt_Hook();
			data[1] = value & 0xff;
			data[2] = (value >> 8) & 0xff;

			/*充电电流测量值 0.1A,偏移-400A*/
			value = (u16)((s16)4000 - (s16)CHGGetGroupChgInputCurr_Hook());
			data[3] = value & 0xff;
			data[4] = (value >> 8) & 0xff;

            /*最高电池电压和电池号*/
			value = (CHGGetGroupMaxCellVNum_Hook() << 12) | (CHGGetGroupMaxCellVolt_Hook() & 0x0fff);
			data[5] = value & 0xff;
			data[6] = (value >> 8) & 0xff;

			/*当前SOC 1%*/
		    value = CHGGetGroupSoc_Hook() / 10;
			data[7] = value & 0xff;

			/*发送报文*/
			sTime = 1;
			CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

			return(FALSE);
		}
	    /*第三帧*/
		else
		{
	        /*BCS结束报文ID*/
	        ID = 0x1CEB56F4;

	        /*BCS报文数据域*/
	        data[0] = 0x02;             /*第2包数据*/

	        /*剩余充电时间 1min*/
	        value = CHGGetGroupChgRemainTime_Hook();
	        if(value > 600)
	        {
	        	value = 600;
	        }
			data[1] = value & 0xff;
			data[2] = (value >> 8) & 0xff;

			data[3] = 0xff;
			data[4] = 0xff;
			data[5] = 0xff;
			data[6] = 0xff;
			data[7] = 0xff;

			/*发送报文*/
			sTime = 0;
			CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);

			return(TRUE);
		}
	}
}

/*=================================================================================================
** @Name      : void CHGBMSSendBSMMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 发送BSM报文(电池信息)
** @The notes : 充电阶段,250ms周期,单帧
**===============================================================================================*/
void CHGBMSSendBSMMsg(void)
{
	u32 ID = 0;
	u16 value = 0;
	u8 data[8] = {0};

	/*BSM报文ID*/
	ID = 0x181356f4;

	/*BSM报文数据域*/
	/*单体最高电压电池号*/
	data[0] = CHGGetGroupMaxCellVNum_Hook();

	/*单体最高温度 1℃,偏移-50℃*/
	data[1] = (u8)(CHGGetGroupMaxCellTemp_Hook() + 50);

	/*单体最高温度号*/
	data[2] = CHGGetGroupMaxCellTNum_Hook();

	/*单体最低温度 1℃,偏移-50℃*/
	data[3] = (u8)(CHGGetGroupMinCellTemp_Hook() + 50);

	/*单体最低温度号*/
	data[4] = CHGGetGroupMinCellTNum_Hook();

	/*电池状态标志位1*/
	value = ((CHGGetGroupTHAlarmState_Hook() & 0x03) << 6)
			| ((CHGGetGroupChgCAlarmState_Hook() & 0x03) << 4)
			| ((CHGGetGroupSocAlarmState_Hook() & 0x03) << 2)
			| (CHGGetGroupCellVAlarmState_Hook() & 0x03);
	data[5] = value & 0xff;

	/*电池状态标志位2*/
	value = ((CHGGetGroupChgAllowState_Hook() & 0x03) << 4)
			| ((CHGGetGroupSwitchAlarmState_Hook() & 0x03) << 2)
			| (CHGGetGroupLeakAlarmState_Hook() & 0x03);
	data[6] = value & 0xff;

	data[7] = 0xff;

	/*发送报文*/
	CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);
}

/*=================================================================================================
** @Name      : void CHGBMSSendBSTMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 发送BST报文(中止充电报文)
** @The notes : 充电阶段,10ms周期,单帧
**===============================================================================================*/
void CHGBMSSendBSTMsg(void)
{
	u32 ID = 0;
	u32 value = 0;
	u8 data[8] = {0};

	/*BST报文ID*/
	ID = 0x101956F4;

	/*BST报文数据域*/
	/*电池组终止充电*/
	if(CHGGetGroupChgFinState_Hook() > 0)
	{
		/*终止充电原因*/
	    if(CHGGetGroupSoc_Hook() >= 1000)   /*达到SOC*/
	    {
		    BitSet(value, 0);
	    }
	    if(1 == CHGGetGroupSumVAlarmState_Hook())/*达到总压*/
	    {
	    	BitSet(value, 2);
	    }
	    if(1 == CHGGetGroupCellVAlarmState_Hook())/*达到单体电压*/
	    {
	    	BitSet(value, 4);
	    }

		/*终止充电故障原因*/
	    if(CHGGetGroupLeakAlarmState_Hook() > 0)/*绝缘故障*/
	    {
		    BitSet(value, 8);
	    }
	    /*if(CHGGetGroupSwhTHAlarmState_Hook() > 0)//连接器过温故障
	    {
		    BitSet(value, 10);
	    }*/
	    if(CHGGetGroupHWTHAlarmState_Hook() > 0)/*BMS元器件/连接器过温故障*/
	    {
		    BitSet(value, 12);
	    }
	    /*if(CHGGetGroupSwitchAlarmState_Hook() > 0)//充电连接器故障
	    {
		    BitSet(value, 14);
	    }*/
	    if(CHGGetGroupTHAlarmState_Hook() > 0)/*充电过温故障*/
	    {
		    BitSet(value, 16);
	    }
	    if(CHGGetGroupSwitchAlarmState_Hook() > 0)/*高压继电器故障*/
	    {
		    BitSet(value, 18);
	    }
	    /*if(CHGGetGroupDcSampAlarmState_Hook() > 0)监测点2电压检测故障
	    {
		    BitSet(value, 20);
	    }*/

	    if(CHGGetGroupChgCAlarmState_Hook() > 0)/*电流过大故障*/
	    {
		    BitSet(value, 24);
	    }

	    /*if(CHGGetGroupVoltErrAlarmState_Hook() > 0)//电压异常故障
	    {
		    BitSet(value, 26);
	    }*/

        /*其它故障*/
	    if((0 == value) || (CHG_RCV_CML_MSG == CHGGetRcvChgerInfoAPI(eCHGRCV_CML)))
	    {
		    BitSet(value, 22);
	    }
	}

	/*充电机终止充电*/
    if(CHGGetRcvChgerInfoAPI(eCHGRCV_STOP) > 0)
    {
    	BitSet(value, 6);
    }

    data[0] = value & 0xff;
    data[1] = (value >> 8) & 0xff;
    data[2] = (value >> 16) & 0xff;
    data[3] = (value >> 24) & 0xff;

	data[4] = 0xff;
	data[5] = 0xff;
	data[6] = 0xff;
	data[7] = 0xff;

	/*发送报文*/
	CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);
}

/*=================================================================================================
** @Name      : void BMS统计数据报文(void)
** @Input     : void
** @Output    : void
** @Function  : 发送BSD报文(BMS统计数据报文)
** @The notes : 结束阶段,250ms周期,单帧
**===============================================================================================*/
void CHGBMSSendBSDMsg(void)
{
	u32 ID = 0;
	u32 value = 0;
	u8 data[8] = {0};

	/*BSD报文ID*/
	ID = 0x181C56F4;

	/*BSD报文数据域*/
	/*SOC 1%*/
    value = CHGGetGroupSoc_Hook() / 10;
	data[0] = value & 0xff;

	/*单体最低电压 0.01V*/
    value = CHGGetGroupMinCellVolt_Hook();
	data[3] = value & 0xff;
	data[4] = (value >> 8) & 0xff;

	/*单体最高电压 0.01V*/
    value = CHGGetGroupMaxCellVolt_Hook();
	data[3] = value & 0xff;
	data[4] = (value >> 8) & 0xff;

	/*单体最低温度 1℃,偏移-50℃*/
	data[5] = (u8)(CHGGetGroupMinCellTemp_Hook() + 50);

	/*单体最低温度 1℃,偏移-50℃*/
	data[6] = (u8)(CHGGetGroupMinCellVolt_Hook() + 50);

	data[7] = 0xff;

	/*发送报文*/
	CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);
}

/*=================================================================================================
** @Name      : void CHGBMSSendBEMMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 发送BEM报文(BMS错误报文)
** @The notes : 充电故障时,单帧
**===============================================================================================*/
void CHGBMSSendBEMMsg(void)
{
	u32 ID = 0;
	u32 value = 0;
	u8 data[8] = {0};

	/*BEM报文ID*/
	ID = 0x081E56F4;

	/*BEM报文数据域*/
	/*通信故障码*/
    value = CHGGetChgMsgErrCodeAPI();

    data[0] = value & 0xff;
    data[1] = (value >> 8) & 0xff;
    data[2] = (value >> 16) & 0xff;
    data[3] = (value >> 24) & 0xff;

	data[4] = 0xff;
	data[5] = 0xff;
	data[6] = 0xff;
	data[7] = 0xff;

	/*发送报文*/
	CHGSendMsgUser_Hook (ID, data, 8, CHG_CAN_NUM);
}

