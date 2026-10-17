/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPGPIO.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 输入输出IO驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPGPIO_H
#define _BSPGPIO_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*I/O端口类型名称*/
typedef enum
{
    ePOR_A = 0,
    ePOR_B,
    ePOR_C,
    ePOR_D,
    ePOR_E,

    ePOR_NUM
}eGPIOPORT;

/*PORT管脚功能*/
typedef enum
{
    ePORT_PIN_DISABLED = 0,
    ePORT_MUX_AS_GPIO,
    ePORT_MUX_ALT2,
    ePORT_MUX_ALT3,
    ePORT_MUX_ALT4,
    ePORT_MUX_ALT5,
    ePORT_MUX_ALT6,
    ePORT_MUX_ALT7,
    ePORT_MUX_NUM

}eGPIOPORTMUX;

/*I/O端口管脚序号(某一类型下)*/
typedef enum
{
    ePIN_0 = 0,
    ePIN_1,
    ePIN_2,
    ePIN_3,
    ePIN_4,
    ePIN_5,
    ePIN_6,
    ePIN_7,
    ePIN_8,
    ePIN_9,
    ePIN_10,
    ePIN_11,
    ePIN_12,
    ePIN_13,
    ePIN_14,
    ePIN_15,
    ePIN_16,
    ePIN_17,
    ePIN_NUM,
    ePIN_ALL = 0xff
}eGPIOPIN;

/*I/O状态*/
typedef enum
{
    eIOLOW = 0,             //低电平
    eIOHIG = 1,             //高电平
    
    eIOSTATE                //IO状态数目
}e_IOState;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPGPIOInit(eGPIOPORT port, eGPIOPIN pin, eGPIOPORTMUX portmode, u8 dir)
** @Input     : port:端口 pin:第几个引脚  portmode:管脚类型  dir:方向(0输入 1输出)
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : TRUE:初始化成功 FALSE:初始化失败
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOInit(eGPIOPORT port, eGPIOPIN pin, eGPIOPORTMUX portmode, u8 dir);

/*=============================================================================================
** @Name      : u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data)
** @Input     : port:端口 pin:第几个引脚 data:端口值(0低电平 1高电平)
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : 设置GPIO端口输出电平值
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data);

/*=============================================================================================
** @Name      : u8 BSPGPIOGetOutPortState(eGPIOPORT port, eGPIOPIN pin)
** @Input     : port:端口     pin:第几个引脚
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : GPIO端口当前电平值[0低电平,1高电平] 0xff:参数输入错误
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOGetOutPortState(eGPIOPORT port, eGPIOPIN pin);

/*=============================================================================================
** @Name      : u8 BSPGPIOGetInPortState(eGPIOPORT port, eGPIOPIN pin)
** @Input     : port:端口     pin:第几个引脚
** @Output    : GPIO端口当前电平值[0低电平,1高电平] 0xff:参数输入错误
** @Function  : 查询GPIO输入端口当前电平值
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOGetInPortState(eGPIOPORT port, eGPIOPIN pin);

#endif

