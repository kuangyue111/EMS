/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : DEVSCI.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : RS485通讯管理函数模块
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPUART.h"
#include "DEVGPIO.h"
#include "osif.h"
#include "PublicPara.h"
#include "DEVSCI.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gSysHigParaRO_101[];         /*系统硬件参数设置*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u8 SCIEnableSendPin(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : RS485发送管脚使能(232或485时使用)
** @The notes : 接收禁止
**===========================================================================================*/
static u8 SCIEnableSendPin(e_SCINUM sciNum);

/*=============================================================================================
** @Name      : static void SCIEnableRcvPin(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : RS485接收管脚使能(232或485时使用)
** @The notes : 发送禁止
**===========================================================================================*/
static u8 SCIEnableRcvPin(e_SCINUM sciNum);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 DEVSCIResetInit(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 串口复位重新初始化
** @The notes : 用户调用
**===========================================================================================*/
u8 DEVSCIResetInit(e_SCINUM sciNum)
{
    u16 bps = 0;
    u8 result = FALSE;

    switch(sciNum)
    {
        case eSCI0:
            //远程口波特率
            bps = gSysHigParaRO_101[eSHWPara101_RemoteRate];
            //result = BSPSCIInit(eSCI0, SCI0_BPS, Bus_CLOCK);
            if(SCI0_BPS != bps)
            {
                result = BSPSCIInit(eSCI0, bps, Bus_CLOCK);
            }
            break;
            
        case eSCI1:
            //就地口波特率
            bps = gSysHigParaRO_101[eSHWPara101_LocalRate];
            //result = BSPSCIInit(eSCI1, SCI1_BPS, Bus_CLOCK);
            if(SCI1_BPS != bps)
            {
                result = BSPSCIInit(eSCI1, bps, Bus_CLOCK);
            }
            break;
        
        case eSCI2:
            //监控口波特率
            bps = gSysHigParaRO_101[eSHWPara101_MonitRate];
            //result = BSPSCIInit(eSCI2, SCI2_BPS, Bus_CLOCK);
            if(SCI2_BPS != bps)
            {
                result = BSPSCIInit(eSCI2, bps, Bus_CLOCK);
            }
            break;
            
        default:
            result = FALSE;
            break;
    }
    
    return(result);
}

/*=============================================================================================
** @Name      : u8 DEVSCISendDataOnISR(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : RS485中断方式发送串口数据
** @The notes : 直接发送,暂不实现
**===========================================================================================*/
u8 DEVSCISendDataOnISR(e_SCINUM sciNum, u8 *sendbuf, u8 size)
{
    /*SCI发送管脚使能(232或485时使用)*/
    (void)SCIEnableSendPin(sciNum);
    
    /*输入数据和启动发送中断*/
    BSPSCISendDataOnIRQ(sciNum, sendbuf, size);
    
    /*必须在SCI接收中断处理函数里切换为SCI接收管脚使能(232或485时使用)*/
    //(void)SCIEnableRcvPin(sciNum);

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : RS485询问方式发送串口数据
** @The notes : 直接发送,用户调用
**===========================================================================================*/
u8 DEVSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
{
    /*SCI发送管脚使能(232或485时使用)*/
    (void)SCIEnableSendPin(sciNum);
    
    /*输入和询问发送数据信息*/
    (void)BSPSCISendDataOnAsk(sciNum, sendbuf, size);
    
    /*SCI接收管脚使能(232或485时使用)*/
    (void)SCIEnableRcvPin(sciNum);
    
    return(TRUE);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u8 SCIEnableSendPin(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : RS485发送管脚使能(232或485时使用)
** @The notes : 接收禁止
**===========================================================================================*/
static u8 SCIEnableSendPin(e_SCINUM sciNum)
{
    if(sciNum >= eSCINUM)
    {
        return(FALSE);
    }
    
    switch(sciNum)
    {
        case eSCI0:
            (void)DEVGPIOSetPortState(ePOut_485EN0, ePSTATE_ON);
            break;
            
        case eSCI1:
            (void)DEVGPIOSetPortState(ePOut_485EN1, ePSTATE_ON);
            break;
            
        case eSCI2:
            (void)DEVGPIOSetPortState(ePOut_485EN2, ePSTATE_ON);
            break;
            
        default:
            return(FALSE);
    }
    
    BSPDelay(10);
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : static u8 SCIEnableSendPin(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : RS485接收管脚使能(232或485时使用)
** @The notes : 发送禁止
**===========================================================================================*/
static u8 SCIEnableRcvPin(e_SCINUM sciNum)
{
    if(sciNum >= eSCINUM)
    {
        return(FALSE);
    }
    
    switch(sciNum)
    {
        case eSCI0:
            (void)DEVGPIOSetPortState(ePOut_485EN0, ePSTATE_OFF);
            break;
            
        case eSCI1:
            (void)DEVGPIOSetPortState(ePOut_485EN1, ePSTATE_OFF);
            break;
            
        case eSCI2:
            (void)DEVGPIOSetPortState(ePOut_485EN2, ePSTATE_OFF);
            break;
            
        default:
            return(FALSE);
    }
    
    BSPDelay(5);
    
    return(TRUE);
}
