/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPI2C.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : IIC驱动函数模块
** @Instructions : IIC用模拟IIC可靠性较高,不驱动片内IIC模块
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPConfig.h"
#include "BSPTypeDef.h"
#include "BSPGPIO.h"
#include "i2c1.h"
#include "i2c_pal.h"
#include "pins_driver.h"
#include "BSPI2C.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPI2CInit(e_I2CNum i2cNum)
** @Input     : I2CNum: I2C通道号
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : I2C驱动模块初始化
** @The notes : 接口服务层调用，上电初始化
**===========================================================================================*/
u8 BSPI2CInit(e_I2CNum i2cNum)
{
    /*模块未使能不进行初始化*/
    if(((eIIC0 == i2cNum) && (1 != gBSPFunCfg.Bits.I2C0En))
        || ((eIIC1 == i2cNum) && (1 != gBSPFunCfg.Bits.I2C1En)))
    {
        return(FALSE);
    }

    switch(i2cNum)
    {
    	/*RTC通讯*/
		case eIIC0:
			/*初始化I2C管脚管脚功能*/
			PINS_DRV_SetMuxModeSel(PORTA, ePIN_3, PORT_MUX_ALT3);  //LPI2C0_SCL
			PINS_DRV_SetMuxModeSel(PORTA, ePIN_2, PORT_MUX_ALT3);  //LPI2C0_SDA
			/*初始化I2C0,主机模式*/
			I2C_MasterInit(&i2c1_instance, &i2c1_MasterConfig0);
			break;

		/*EEPROM通讯*/
		case eIIC1:
			break;

		default:
			break;
    }

    return TRUE;
}

/*=============================================================================================
** @Name      : u8 BSPI2CMasterSendData(e_I2CNum sciNum, u8 *sendbuf, u8 size)
** @Input     : i2cNum: i2c通道号   *sendbuf:发送的数据   size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : i2c主机模式发送数据
** @The notes : 直接发送,用户调用
**===========================================================================================*/
u8 BSPI2CMasterSendData(e_I2CNum i2cNum, u8 *sendbuf, u8 size)
{
	switch(i2cNum)
	{
		case eIIC0:
			//I2C_MasterSendData(&i2c1_instance, &sendbuf, size, true);
			break;

		case eIIC1:
			//I2C_MasterSendData(&i2c1_instance, &sendbuf, size, true);
			break;

		default:
			break;
	}
    return TRUE;
}

/*=============================================================================================
** @Name      : u8 BSPI2CMasterReceiveData(e_I2CNum i2cNum, u8 *rxbuf, u8 size)
** @Input     : i2cNum: i2c通道号   *sendbuf:发送的数据   size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : i2c主机模式接收数据
** @The notes : 直接发送,用户调用
**===========================================================================================*/
u8 BSPI2CMasterReceiveData(e_I2CNum i2cNum, u8 *rxbuf, u8 size)
{
	switch(i2cNum)
	{
		case eIIC0:
			//I2C_MasterReceiveData(&i2c1_instance, &rxbuf, size, true);
			break;

		case eIIC1:
			//I2C_MasterReceiveData(&i2c1_instance, &rxbuf, size, true);
			break;

		default:
			break;
	}
    return TRUE;
}

/*=============================================================================================
** @Name      : status_t BSPGetI2CMasterTransferStatus(e_I2CNum i2cNum, u32 bytesRemaining)
** @Input     : i2cNum: i2c通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 获取i2c主机模式数据状态
** @The notes : 用户调用
**===========================================================================================*/
status_t BSPGetI2CMasterTransferStatus(e_I2CNum i2cNum, u32 bytesRemaining)
{
	status_t i2cstatus = STATUS_SUCCESS;

	switch(i2cNum)
	{
		case eIIC0:
			//i2cstatus = I2C_MasterGetTransferStatus(&i2c1_instance, &bytesRemaining);
			break;

		case eIIC1:
			//i2cstatus = I2C_MasterGetTransferStatus(&i2c1_instance, &bytesRemaining);
			break;

		default:
			break;
	}
    return i2cstatus;
}
