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
//#include "BSPI2C.h"
#include "BSPFTM.h"
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
//	u8 canNum = 0;

    if(Channel == eFTM1)                /*0.1ms周期*/
    {
    	//BSPCANRcvIntHandle(eCAN0);
    	//BSPCANRcvIntHandle(eCAN1);
    	//BSPCANRcvIntHandle(eCAN2);
    }
//    else if(Channel == eFTM2)           /*PWM捕获*/
//    {
//    	;
//    }
//    else if(Channel == eFTM3)           /*1ms周期*/
//    {
//    	;
//    }
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

///*=============================================================================================
//** @Name      : void IIC_ISR_Send_Hook(e_I2CNum IICNum)
//** @Input     : IICNum:IIC通道
//** @Output    : void
//** @Function  : IIC发送中断处理HOOK
//** @The notes :
//**===========================================================================================*/
//void IIC_ISR_Send_Hook(e_I2CNum IICNum)
//{
//    (void)IICNum;
//}
//
///*=============================================================================================
//** @Name      : void IIC_ISR_Rcv_Hook(e_I2CNum IICNum)
//** @Input     : IICNum:IIC通道
//** @Output    : void
//** @Function  : IIC接收中断处理HOOK
//** @The notes :
//**===========================================================================================*/
//void IIC_ISR_Rcv_Hook(e_I2CNum IICNum)
//{
//    (void)IICNum;
//}

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
    sPwmTime = BSPGetCosTimer();
    
    //记录慢充枪CP输入PW波的占空比和频率
    InputGroupDcSideInfo(eDcSide83_MCPDC, duty);
    InputGroupDcSideInfo(eDcSide83_MCPFC, frequency);
    */
}


