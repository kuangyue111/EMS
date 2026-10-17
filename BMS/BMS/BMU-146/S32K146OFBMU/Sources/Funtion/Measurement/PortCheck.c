/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : PortCheck.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 绝缘检测模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "GroupPara.h"
#include "PortCheck.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGPortStateInfo_56[];        /*输入输出口状态信息数组*/
extern u16 gGBmuHigLevPara_103[];       /*主控高级参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void PortStateCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 实时更新IO状态到相应寄存器
** @The notes : 10ms调用
**===========================================================================================*/
void PortStateCheckTask(void)
{
    u16 inDryState = 0;
    u16 inBackState = 0;
    u16 outDryState = 0;
    u16 safState = 0;
    u16 swhCtrlState = 0;
    u16 lvSwhCtrlState = 0;
    u16 gndSwhState = 0;
    u16 inInsideState = 0;
    u16 outInsideState = 0;

    inDryState = DEVGPIOGetInputDryState();                 /*读取6路输入干接点信号*/
    inBackState = DEVGPIOGetSwhBackState();                 /*读取3路输入高低边回检信号*/
    outDryState = DEVGPIOGetOutputDryState();               /*读取3路输出干接点信号*/
    swhCtrlState = DEVGPIOGetCtrlHSwitchState();            /*读取9路高边开关控制信号*/
    lvSwhCtrlState = DEVGPIOGetCtrlLSwitchState();          /*获取2路低边开关控制状态*/
    gndSwhState = DEVGPIOGetOutPortState(ePOut_ISO_PGND);   /*获取高压采样GND继电器开关控制状态*/

    inInsideState = DEVGPIOGetHWVersion();                  /*读取3路硬件版本号信号*/
    outInsideState = SampGetHigAFERelyStateAPI();           /*读取4路高压继电器控制状态信号*/

    if(1 == GetGIsoIoReadyStateAPI())                       /*断路器闭合就绪*/
    {
        BitSet(safState, 0);                                /*1为断路器合*/
    }

    if(0 == GetGFusIoExpStateAPI())                         /*熔断器闭合正常*/
    {
        BitSet(safState, 1);                                /*1为熔断器合*/
    }

    if(1 == GetGSmogIoWarStateAPI())                        /*烟感器报警烟感发生*/
    {
        BitSet(safState, 2);                                /*1为烟感器告警*/
    }

    if(1 == GetGEmergIoButtonStateAPI())                    /*急停按钮断开急停发生*/
    {
        BitSet(safState, 3);                                /*1为急停按钮合*/
    }

    if(1 == GetGFireIoExpStateAPI())                        /*消防信号闭合消防发生*/
    {
        BitSet(safState, 4);                                /*1为消防告警*/
    }

    if(1 == GetGGasConIoExpStateAPI())                      /*可燃气体信号断开气体告警发生*/
    {
        BitSet(safState, 5);                                /*1为气体告警*/
    }

    if(1 == DEVGPIOGetInPortState(ePIn_HALFAU))             /*霍尔过载回检信号*/
    {
        BitSet(inInsideState, 3);
    }

    if(1 == DEVGPIOGetInPortState(ePIn_128EWM))             /*辅助MCU心跳回检信号*/
    {
        BitSet(inInsideState, 4);
    }

    if(1 == DEVGPIOGetOutPortState(ePOut_PwHold))           /*电源保持控制信号*/
    {
        BitSet(outInsideState, 4);
    }

    if(1 == DEVGPIOGetOutPortState(ePOut_K4Second))         /*K4第二路控制信号*/
    {
        BitSet(outInsideState, 5);
    }

    if(1 == DEVGPIOGetOutPortState(ePOut_128EWM_OUT))       /*主MCU心跳输出信号*/
    {
        BitSet(outInsideState, 6);
    }

    if(1 == DEVGPIOGetOutPortState(ePOut_LTC6820EN1))       /*菊花链1唤醒控制状态*/
    {
        BitSet(outInsideState, 7);
    }

    if(1 == DEVGPIOGetOutPortState(ePOut_LTC6820EN2))       /*菊花链2唤醒控制状态*/
    {
        BitSet(outInsideState, 8);
    }

    /*更新输出输出口当前状态*/
    gGPortStateInfo_56[ePortStateInfo56_Input] = (inBackState << 13) | ((inDryState & 0x3f) << 6) | (safState & 0x3f);
    gGPortStateInfo_56[ePortStateInfo56_Output] = ((gndSwhState & 0x01) << 15) | ((lvSwhCtrlState & 0x03) << 13) | ((outDryState & 0x07) << 10) | (swhCtrlState & 0x1ff);

    /*更新内部输出输出口当前状态*/
    gGPortStateInfo_56[ePortStateInfo56_InInside] = inInsideState;
    gGPortStateInfo_56[ePortStateInfo56_OutInside] = outInsideState;
}

/*=============================================================================================
** @Name      : u8 GetGPwrIoButtonStateAPI(void)
** @Input     : void
** @Output    : 启停按钮状态：0松开   1按下
** @Function  : 获取启停按钮状态(自复位按钮，常态为断开(低电平))
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGPwrIoButtonStateAPI(void)
{
    /*未使能该端口检测*/
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_PwrPor])
    {
        return(0);
    }

    /*常闭高电平(低电平0为断开按下按钮)*/
    //if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_PwrPor]) != gGBmuHigLevPara_103[eBmuHigLevPara103_PwrPor])
    /*常开低电平(高电平1为闭合按下按钮)[默认配置值:1]*/
    if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_PwrPor]) == gGBmuHigLevPara_103[eBmuHigLevPara103_PwrPor])
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*=============================================================================================
** @Name      : u8 GetGIsoIoReadyStateAPI(void)
** @Input     : void
** @Output    : 断路器就绪状态:0断开 1闭合(就绪)
** @Function  : 获取断路器状态,手动开关，常态为闭合(高电平)
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGIsoIoReadyStateAPI(void)
{
    /*未使能该端口检测*/
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_IsoPort])
    {
        return(1);
    }

    /*常闭高电平(低电平0为断开[异常])*/
    if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_IsoPort]) != gGBmuHigLevPara_103[eBmuHigLevPara103_IsoPort])
    /*常开低电平(高电平1为断开[异常])*/
    //if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_IsoPort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_IsoPort])
    {
        return(0);
    }
    else
    {
        return(1);
    }
}

/*=============================================================================================
** @Name      : u8 GetGSmogIoWarStateAPI(void)
** @Input     : void
** @Output    : 烟感状态:0正常 1报警
** @Function  : 获取烟感报警状态,反馈信号，常态为闭合(高电平)
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGSmogIoWarStateAPI(void)
{
    /*未使能该端口检测*/
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_SmogPort])
    {
        return(0);
    }

    /*常闭高电平(低电平0为烟雾报警)*/
    //if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_SmogPort]) != gGBmuHigLevPara_103[eBmuHigLevPara103_SmogPort])
    /*常开低电平(高电平1为烟雾报警)[默认配置值:8]*/
    if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_SmogPort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_SmogPort])
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*=============================================================================================
** @Name      : u8 GetGEmergIoButtonStateAPI(void)
** @Input     : void
** @Output    : 急停按钮状态:0正常 1按下（急停）
** @Function  : 获取急停按钮状态
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGEmergIoButtonStateAPI(void)
{
    /*未使能该端口检测*/
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_EmergPort])
    {
        return(0);
    }

    /*常闭高电平(低电平0为按下急停)[电源柜24V供到电池柜中继闭合,按下电源柜急停断开24V则断开继电器]*/
    if(((1 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_EmergPort], 15))/*最高位表示常态:1常闭[默认配置值:32784]*/
        && ((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_EmergPort]) != (gGBmuHigLevPara_103[eBmuHigLevPara103_EmergPort] & 0x7fff)))
        /*常开低电平(高电平1为按下急停)[急停按钮接常开反馈信号]*/
        || ((0 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_EmergPort], 15))/*最高位表示常态:0常开[默认配置值:16]*/
        && ((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_EmergPort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_EmergPort])))
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*=============================================================================================
** @Name      : u8 GetGFusIoExpStateAPI(void)
** @Input     : void
** @Output    : 0:正常 1:熔断(异常)
** @Function  : 获取熔断器端口异常状态，低电平0为断开
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGFusIoExpStateAPI(void)
{
    /*未使能该端口检测*/
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_FusPort])
    {
        return(0);
    }

    /*常闭高电平(低电平0为断开[熔断])*/
    if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_FusPort]) != gGBmuHigLevPara_103[eBmuHigLevPara103_FusPort])
    /*常开低电平(高电平1为断开[熔断])*/
    //if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_FusPort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_FusPort])
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*=============================================================================================
** @Name      : u8 GetGThundIoExpStateAPI(void)
** @Input     : void
** @Output    : 0:正常 1:报警
** @Function  : 获取防雷器故障端口异常状态,低电平0为防雷器故障
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGThundIoExpStateAPI(void)
{
    /*未使能该端口检测*/
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_ThundPort])
    {
        return(0);
    }

    /*常闭高电平(低电平0为防雷器故障)*/
    if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_ThundPort]) != gGBmuHigLevPara_103[eBmuHigLevPara103_ThundPort])
    /*常开低电平(高电平1为防雷器故障)*/
    //if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_ThundPort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_ThundPort])
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*=============================================================================================
** @Name      : u8 GetGThundIoExpStateAPI(void)
** @Input     : void
** @Output    : 0:正常 1:报警
** @Function  : 获取消防故障端口异常状态,高电平1为消防故障
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGFireIoExpStateAPI(void)
{
    /*未使能该端口检测*/
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_FirePort])
    {
        return(0);
    }

    /*常闭高电平(低电平0为消防报警)[气溶胶]*/
    if(((1 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_FirePort], 15))/*最高位表示常态:1常闭[默认配置值:32800]*/
        && ((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_FirePort]) != (gGBmuHigLevPara_103[eBmuHigLevPara103_FirePort] & 0x7fff)))
        /*常开低电平(高电平1为消防报警)[七氟丙烷]*/
        || ((0 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_FirePort], 15))/*最高位表示常态:0常开[默认配置值:32]*/
        && ((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_FirePort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_FirePort])))
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*=============================================================================================
** @Name      : u8 GetGGasConIoExpStateAPI(void)
** @Input     : void
** @Output    : 0:正常 1:报警
** @Function  : 获取可燃气体浓度告警端口异常状态,低电平0为浓度告警
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGGasConIoExpStateAPI(void)
{
    /*未使能该端口检测*/
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_GasPort])
    {
        return(0);
    }

    /*常闭高电平(低电平0为浓度告警)[国标/CE版]*/
    if(((1 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_GasPort], 15))/*最高位表示常态:1常闭[默认配置值:32770]*/
        && ((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_GasPort]) != (gGBmuHigLevPara_103[eBmuHigLevPara103_GasPort] & 0x7fff)))
        /*常开低电平(高电平1为浓度告警)[UL版]*/
        || ((0 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_GasPort], 15))/*最高位表示常态:0常开[默认配置值:2]*/
        && ((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_GasPort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_GasPort])))
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*=============================================================================================
** @Name      : u8 GetGLeakIoExpStateAPI(void)
** @Input     : void
** @Output    : 0:正常 1:一级故障 2:二级告警 3:检测异常
** @Function  : 获取绝缘故障告警端口异常状态,告警口低电平0为检测异常,故障口高电平1为绝缘故障
** @The notes : 用户调用
**===========================================================================================*/
u8 GetGLeakIoExpStateAPI(void)
{
//    /*未使能该端口检测*/
//    if((0 == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakPort]) && (0 == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakerPort]))
//    {
//        return(0);
//    }
//
//    /*常闭高电平(低电平0为绝缘一级故障)*/
//    //if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_LeakerPort]) != gGBmuHigLevPara_103[eBmuHigLevPara103_LeakerPort])
//    /*常开低电平(高电平1为绝缘一级故障)*/
//    if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_LeakerPort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakerPort])
//    {
//        return(1);
//    }
//    /*常闭高电平(低电平0为绝缘检测失效)*/
//    else if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_LeakPort]) != gGBmuHigLevPara_103[eBmuHigLevPara103_LeakPort])
//    /*常开低电平(高电平1为绝缘检测失效)*/
//    //else if((DEVGPIOGetInputDryState() & gGBmuHigLevPara_103[eBmuHigLevPara103_LeakPort]) == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakPort])
//    {
//        return(3);
//    }
//    else
//    {
//        return(0);
//    }
    return(0);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/

