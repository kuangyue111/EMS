/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPGPIO.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 输入输出IO驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "pins_driver.h"
#include "BSPGPIO.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static GPIO_Type *GPIOChangeToGPIOType(eGPIOPORT port)
** @Input     : port:端口
** @Output    : 端口类型
** @Function  : 转换端口对应的类型
** @The notes :
**===========================================================================================*/
static GPIO_Type *GPIOChangeToGPIOType(eGPIOPORT port);

/*=============================================================================================
** @Name      : static PORT_Type *GPIOChangeToPORTType(eGPIOPORT port)
** @Input     : port:端口
** @Output    : 端口类型
** @Function  : 转换端口对应的类型
** @The notes :
**===========================================================================================*/
static PORT_Type *GPIOChangeToPORTType(eGPIOPORT port);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPGPIOInit(eGPIOPORT port, eGPIOPIN pin, eGPIOPORTMUX portmode, u8 dir)
** @Input     : port:端口 pin:第几个引脚  portmode:管脚类型  dir:方向(0输入 1输出)
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : TRUE:初始化成功 FALSE:初始化失败
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOInit(eGPIOPORT port, eGPIOPIN pin, eGPIOPORTMUX portmode, u8 dir)
{
	PORT_Type *PORTx;
    GPIO_Type *GPIOx;
    
    //模块未使能不进行初始化
    if(1 != gBSPFunCfg.Bits.GPIOEn)
    {
        return(FALSE);
    }
    
    if((port >= ePOR_NUM) || ((pin >= ePIN_NUM) && (pin != ePIN_ALL)) || (dir >= 2))
    {
        return(FALSE);
    }

    /*转换为对应的PORT类型*/
    PORTx = GPIOChangeToPORTType(port);
    /*转换为对应的GPIO类型*/
    GPIOx = GPIOChangeToGPIOType(port);

    PINS_DRV_SetMuxModeSel(PORTx, pin, (port_mux_t)portmode);         /*设置PORT管脚功能*/
    PINS_DRV_SetPinDirection(GPIOx, pin, (port_data_direction_t)dir); /*初始化GPIO方向*/

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data)
** @Input     : port:端口 pin:第几个引脚 data:端口值(0低电平 1高电平)
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : 设置GPIO端口输出电平值
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data)
{
    GPIO_Type *GPIOx;
    
    if(1 != gBSPFunCfg.Bits.GPIOEn)
    {
        return(FALSE);
    }
    
    if((port >= ePOR_NUM) || ((pin >= ePIN_NUM) && (pin != ePIN_ALL)) || (data >= eIOSTATE))
    {
        return(FALSE);
    }

    /*转换为对应的端口类型*/
    GPIOx = GPIOChangeToGPIOType(port);

    /*设置高电平*/
    if(eIOHIG == data)
    {
    	PINS_DRV_WritePin(GPIOx, pin, data);
    }
    else
    {
    	PINS_DRV_WritePin(GPIOx, pin, data);
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPGPIOGetOutPortState(eGPIOPORT port, eGPIOPIN pin)
** @Input     : port:端口     pin:第几个引脚
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : GPIO端口当前电平值[0低电平,1高电平] 0xff:参数输入错误
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOGetOutPortState(eGPIOPORT port, eGPIOPIN pin)
{
	u32 outputData = 0;
    GPIO_Type *GPIOx;

    if(1 != gBSPFunCfg.Bits.GPIOEn)
    {
        return(0xff);
    }

    if((port >= ePOR_NUM) || ((pin >= ePIN_NUM) && (pin != ePIN_ALL)))
    {
        return(0xff);
    }

    /*转换为对应的端口类型*/
    GPIOx = GPIOChangeToGPIOType(port);

    outputData = PINS_DRV_GetPinsOutput(GPIOx);/*获取整组IO的电平值，每个bit表示一个IO，bit0表示第一个IO*/

    if(1 == BitGet(outputData, pin))
    {
        return(eIOHIG);
    }
    else
    {
        return(eIOLOW);
    }
}

/*=============================================================================================
** @Name      : u8 BSPGPIOGetInPortState(eGPIOPORT port, eGPIOPIN pin)
** @Input     : port:端口     pin:第几个引脚
** @Output    : GPIO端口当前电平值[0低电平,1高电平] 0xff:参数输入错误
** @Function  : 查询GPIO输入端口当前电平值
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOGetInPortState(eGPIOPORT port, eGPIOPIN pin)
{
	u32 inputData = 0;
    GPIO_Type *GPIOx;
    
    if(1 != gBSPFunCfg.Bits.GPIOEn)
    {
        return(0xff);
    }
    
    if((port >= ePOR_NUM) || ((pin >= ePIN_NUM) && (pin != ePIN_ALL)))
    {
        return(0xff);
    }

    /*转换为对应的端口类型*/
    GPIOx = GPIOChangeToGPIOType(port);
    
    inputData = PINS_DRV_ReadPins(GPIOx);/*获取整组IO的电平值*/

    if(1 == BitGet(inputData, pin))
    {
        return(eIOHIG);
    }
    else
    {
        return(eIOLOW);
    }
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static GPIO_Type *GPIOChangeToGPIOType(eGPIOPORT port)
** @Input     : port:端口
** @Output    : 端口类型
** @Function  : 转换端口对应的类型
** @The notes :
**===========================================================================================*/
static GPIO_Type *GPIOChangeToGPIOType(eGPIOPORT port)
{
	GPIO_Type *data = (void *)0;

    switch(port)
    {
        case ePOR_A:
            data = PTA;
            break;

        case ePOR_B:
            data = PTB;
            break;

        case ePOR_C:
            data = PTC;
            break;

        case ePOR_D:
            data = PTD;
            break;

        case ePOR_E:
            data = PTE;
            break;

        default:
            data = (void *)0;
            break;
    }

    return(data);
}

/*=============================================================================================
** @Name      : static PORT_Type *GPIOChangeToPORTType(eGPIOPORT port)
** @Input     : port:端口
** @Output    : 端口类型
** @Function  : 转换端口对应的类型
** @The notes :
**===========================================================================================*/
static PORT_Type *GPIOChangeToPORTType(eGPIOPORT port)
{
	PORT_Type *data = (void *)0;

    switch(port)
    {
        case ePOR_A:
            data = PORTA;
            break;

        case ePOR_B:
            data = PORTB;
            break;

        case ePOR_C:
            data = PORTC;
            break;

        case ePOR_D:
            data = PORTD;
            break;

        case ePOR_E:
            data = PORTE;
            break;

        default:
            data = (void *)0;
            break;
    }

    return(data);
}
