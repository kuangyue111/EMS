/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbTriSend.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包触发处理函数
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPUserAPI.h"
#include "MbUserDef.h"
#include "MbHandle.h"
#include "MbTriSend.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
sTriCycMsg MBTriCycMsg[MBSTATIONNUM]; //触发发送信息

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void MbTrigSendTimerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 触发信息发送计时任务
** @The notes : 2ms周期调用一次
**===============================================================================================*/
void MbTrigSendTimerTask(void)
{
	u8 i = 0;

	for(i = 0; i < cMBStationNum; i++)
	{
		//触发标识已使能
		if(MBTriCycMsg[i].flag == 1)
		{
			//触发发送周期计时，若计时时间到，则发送信息
			MBTriCycMsg[i].time++;
			
			if(MBTriCycMsg[i].time >= MBTriCycMsg[i].cyc) 
			{
			    /*注意：在ARM平台，此句不能够正常执行，原因是.arg会发生更改*/
			    
			    //调用触发发送函数
				(*(MBTriCycMsg[i].fun))(MBTriCycMsg[i].arg);

				//触发发送时间清零，重新计时
				MBTriCycMsg[i].time = 0; 
			}
			//MBTriCycMsg[i].flag = 0;
		}
	}
}

/*=================================================================================================
** @Name      : u8 MbTrigSendMsgCfgAPI(u8 id, u16 cycle, void (*fun)(void *arg), void *arg)
** @Input     : id:触发信息通道号,cyc:触发信息周期1表示2ms,void (*fun)(void *arg)触发函数,*arg:触发函数参数
** @Output    : 配置结果:1:成功 0:失败
** @Function  : 触发函数发送信息配置
** @The notes : 初始化配置调用
**===============================================================================================*/
u8 MbTrigSendMsgCfgAPI(u8 id, u16 cycle, void (*fun)(void *arg), void *arg)
{
	//参数越界判断
	if(id >= cMBStationNum)
	{
		return(FALSE);
	}
	else
	{
		MBTriCycMsg[id].flag = 0;
		MBTriCycMsg[id].cyc = cycle;
		MBTriCycMsg[id].fun = fun;
		MBTriCycMsg[id].arg = arg;
		MBTriCycMsg[id].time = cycle;
		return(TRUE);
	}
}

/*=================================================================================================
** @Name      : u8 MbSetTrigFlagStateAPI(u8 id, u8 flag)
** @Input     : id:触发信息通道号,flag:触发标识(0:不触发 1:触发)
** @Output    : 触发设置结果:1成功 0失败
** @Function  : 设置触发标识状态
** @The notes :
**===============================================================================================*/
u8 MbSetTrigFlagStateAPI(u8 id, u8 flag)
{
	//参数越界判断
	if(id >= cMBStationNum)
	{
		return(FALSE);
	}
	else
	{
		MBTriCycMsg[id].flag = flag;
		return(TRUE);
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
