/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPISRHook.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : BSP中断服务函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "BSPCAN.h"
#include "BSPUART.h"
#include "BSPSPI.h"
#include "BSPI2C.h"
#include "BSPFTM.h"
#include "PublicInfo.h"
#include "CurrSample.h"
#include "CANInterface.h"
#include "MbInterface.h"
#include "UserTactic.h"
#include "BSPISRHook.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
//static u32 sPwmTime = 0;        /*记录PWM发生时间*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void SysTick_ISR_Hook(void)
** @Input     : void
** @Output    : void
** @Function  : 系统闹铃定时中断HOOK
** @The notes : 用户不能使用
**===========================================================================================*/
void SysTick_ISR_Hook(void)
{
	;
}

/*=============================================================================================
** @Name      : void Time_ISR_Hook(e_FTMNum Channel)
** @Input     : Channel:TIM通道[eTIM1, eTIM3]
** @Output    : void
** @Function  : 定时中断HOOK
** @The notes : eFIM0用于PWM,不能使用
**===========================================================================================*/
void Time_ISR_Hook(e_FTMNum Channel)
{
	//static u8 sTime = 0;
	static u8 sCount = 0;

    if(Channel == eFTM1)                    /*0.1ms周期*/
    {
        if(BSPGetOSMillSecTimeAPI() < 1)    /*上电等待初始化不能执行防止出错*/
        {
            return;
        }

        ModbusRcvTickTask();                /*modbus帧接收处理(0.1ms)*/
    }
    else if(Channel == eFTM2)               /*PWM捕获*/
    {
    	/*
        //检测PWM是否停止
        if((0 != sPwmTime)
            && (ABS(sPwmTime, BSPGetOSMillSecTimeAPI()) >= 10))
        {
            //清除慢充枪CP输入PW波的占空比和频率
            sPwmTime = 0;
            InputGroupDcSideInfo(eDcSide83_MCPDC, 0);
            InputGroupDcSideInfo(eDcSide83_MCPFC, 0);
        }
        */
    }
    else if(Channel == eFTM3)               /*1ms周期*/
    {
        if(BSPGetOSMillSecTimeAPI() < 1)   /*上电等待初始化不能执行防止出错*/
        {
            return;
        }

        ISRCurrCapAddTask();	            /*定时中断电流积分任务*/

        SysClockCountTask();		        /*系统1ms系统时钟计时任务*/

        MbClientTimerTask();                /*Modbus通讯包计时任务(1ms)*/
//      if(0 == sTime)
//      {
//          MbTrigSendTimerTask();          /*Modbus通信包定时触发任务(2ms)[发送报文太久不能在中断调用]*/
//          sTime = 1;
//      }
//      else
//      {
//          sTime = 0;
//      }

        CANLibTxRcvHandleTask_API();        /*CAN通信报文收发处理任务(收发通信表处理),1ms*/

        if(sCount >= 100)
        {
        	HeartToSlaveMcuTask();          /*与辅助MCU心跳处理任务(监控心跳IO),100ms*/
        	sCount = 0;
        }
        else
        {
        	sCount++;
        }
    }
}

/*=============================================================================================
** @Name      : void SPI_ISR_Hook(e_SPINum SPINum)
** @Input     : SPINum:SPI通道
** @Output    : void
** @Function  : SPI中断处理HOOK
** @The notes :
**===========================================================================================*/
void SPI_ISR_Hook(e_SPINum SPINum)
{
    (void)SPINum;
}

/*=============================================================================================
** @Name      : void CAN_ISR_Err_Hook(e_CANNUM CANNum)
** @Input     : CANNum:CAN通道
** @Output    : void
** @Function  : CAN错误中断处理HOOK
** @The notes :
**===========================================================================================*/
void CAN_ISR_Err_Hook(e_CANNUM CANNum)
{
    (void)CANNum;
}

/*=============================================================================================
** @Name      : void CAN_ISR_Send_Hook(e_CANNUM CANNum)
** @Input     : CANNum:CAN通道
** @Output    : void
** @Function  : CAN发送中断处理HOOK
** @The notes :
**===========================================================================================*/
void CAN_ISR_Send_Hook(e_CANNUM CANNum)
{
    (void)CANNum;
}

/*=============================================================================================
** @Name      : void CAN_ISR_Rcv_Hook(e_CANNUM CANNum)
** @Input     : CANNum:CAN通道
** @Output    : void
** @Function  : CAN接收中断处理HOOK
** @The notes :
**===========================================================================================*/
void CAN_ISR_Rcv_Hook(e_CANNUM CANNum)
{
    (void)CANNum;
}

/*=============================================================================================
** @Name      : void SCI_ISR_Send_Hook(e_SCINUM SCINum)
** @Input     : SCINum:SCI通道
** @Output    : void
** @Function  : SCI发送中断处理HOOK
** @The notes :
**===========================================================================================*/
void SCI_ISR_Send_Hook(e_SCINUM SCINum)
{
    //必须在SCI发送中断处理函数里切换为SCI接收管脚使能(232或485时使用)
    //(void)DEVSCIEnableRcvPin(SCINum);
}

/*=============================================================================================
** @Name      : void SCI_ISR_Rcv_Hook(e_SCINUM SCINum)
** @Input     : SCINum:SCI通道
** @Output    : void
** @Function  : SCI接收中断处理HOOK
** @The notes :
**===========================================================================================*/
void SCI_ISR_Rcv_Hook(e_SCINUM SCINum)
{
    (void)SCINum;
}

/*=============================================================================================
** @Name      : void IIC_ISR_Send_Hook(e_I2CNum IICNum)
** @Input     : IICNum:IIC通道
** @Output    : void
** @Function  : IIC发送中断处理HOOK
** @The notes :
**===========================================================================================*/
void IIC_ISR_Send_Hook(e_I2CNum IICNum)
{
    (void)IICNum;
}

/*=============================================================================================
** @Name      : void IIC_ISR_Rcv_Hook(e_I2CNum IICNum)
** @Input     : IICNum:IIC通道
** @Output    : void
** @Function  : IIC接收中断处理HOOK
** @The notes :
**===========================================================================================*/
void IIC_ISR_Rcv_Hook(e_I2CNum IICNum)
{
    (void)IICNum;
}

/*=============================================================================================
** @Name      : void PWM_ISR_Rcv_Hook(u16 duty, u32 frequency)
** @Input     : 捕获到的数据:duty:占空比 % frequency:PWM频率 Hz
** @Output    : void
** @Function  : PWM波捕获用户接收中断处理
** @The notes :
**===========================================================================================*/
void PWM_ISR_Rcv_Hook(u16 duty, u32 frequency)
{
	/*
    //占空比不能超过100%
    if(duty > 100)
    {
        duty = 100;
    }
    
    //记录慢充枪CP输入PWM发生时间
    sPwmTime = BSPGetOSMillSecTimeAPI();
    
    //记录慢充枪CP输入PW波的占空比和频率
    InputGroupDcSideInfo(eDcSide83_MCPDC, duty);
    InputGroupDcSideInfo(eDcSide83_MCPFC, frequency);
    */
}


