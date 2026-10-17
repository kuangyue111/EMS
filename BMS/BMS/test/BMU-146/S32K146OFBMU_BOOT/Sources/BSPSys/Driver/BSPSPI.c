/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPSPI.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : SPI通讯驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "status.h"
//#include "spi_pal.h"
#include "lpspiCom1.h"
//#include "spi1_pal.h"
//#include "spi2_pal.h"
//#include "spi3_pal.h"
#include "flexio_spi1.h"
#include "flexio_spi_driver.h"
#include "lpspi_shared_function.h"
#include "lpspi_master_driver.h"
#include "BSPConfig.h"
#include "BSPTypeDef.h"
#include "BSPISRHook.h"
#include "BSPGPIO.h"
#include "BSPSPI.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;
flexio_spi_master_state_t flexio_spi_master_state;
/* Allocate the memory necessary for the FlexIO state structures */
flexio_device_state_t flexIODeviceState;
static u8 sSpiWorkState[eSPINUM] = {0};
static u16 sSpiRcvData[eSPINUM] = {0};
static u8 RxBuffer[eSPINUM] = {0};      //SPI接收数据缓冲，one frame lenth is 16bits

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 SpiReceiveData(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : void
** @Function  : 获取SPI接收数据
** @The notes :
**===========================================================================================*/
static u16 SpiReceiveData(e_SPINum spiNum);

/*=============================================================================================
** @Name      : static void SPI_I2S_SendData(e_SPINum spiNum, u16 data)
** @Input     : spiNum:SPI通道号  data:需要发送的数据
** @Output    : void
** @Function  : SPI发送数据
** @The notes :
**===========================================================================================*/
static void SPI_I2S_SendData(e_SPINum spiNum, u16 data);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPSPIInit(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : SPI驱动模块初始化
** @The notes : BSP调用
**===========================================================================================*/
u8 BSPSPIInit(e_SPINum spiNum)
{
    /*模块未使能不进行初始化*/
    if(((eSPI0 == spiNum) && (1 != gBSPFunCfg.Bits.SPI0En))
        || ((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI2 == spiNum) && (1 != gBSPFunCfg.Bits.SPI2En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(FALSE);
    }
    switch(spiNum)
    {
    	/*SBC电源芯片通讯初始化*/
		case eSPI0:
			//PINS_DRV_SetMuxModeSel(PORTE, ePIN_6, PORT_MUX_ALT1);//LPSPI0_CS
			PINS_DRV_SetMuxModeSel(PORTE, ePIN_2, PORT_MUX_ALT2);  //LPSPI0_SOUT
			PINS_DRV_SetMuxModeSel(PORTE, ePIN_0, PORT_MUX_ALT2);  //LPSPI0_SCK
			PINS_DRV_SetMuxModeSel(PORTE, ePIN_1, PORT_MUX_ALT2);  //LPSPI0_SIN
			//初始化SPI1
			//SPI_MasterInit(&spi1_palInstance, &spi1_MasterConfig0);
			LPSPI_DRV_MasterInit(0, &lpspiCom1State, &lpspiCom1_MasterConfig0);
			break;

//    	/*高压采样芯片LTC2949通讯*/
//		case eSPI1:
//			//初始化通讯管脚
//			//PINS_DRV_SetMuxModeSel(PORTB, ePIN_17, PORT_MUX_ALT1);//LPSPI0_CS
//			PINS_DRV_SetMuxModeSel(PORTB, ePIN_16, PORT_MUX_ALT3);  //LPSPI0_SOUT
//			PINS_DRV_SetMuxModeSel(PORTB, ePIN_14, PORT_MUX_ALT3);  //LPSPI0_SCK
//			PINS_DRV_SetMuxModeSel(PORTB, ePIN_15, PORT_MUX_ALT3);  //LPSPI0_SIN
//			//初始化SPI2
//			SPI_MasterInit(&spi2_palInstance, &spi2_MasterConfig0);
//			break;
//
//		/*菊花链通讯初始化*/
//		case eSPI2:
//			//初始化通讯管脚
//			//PINS_DRV_SetMuxModeSel(PORTC, ePIN_14, PORT_MUX_ALT1);//LPSPI0_CS
//			//PINS_DRV_SetMuxModeSel(PORTE, ePIN_13, PORT_MUX_ALT1);//LPSPI0_CS
//			PINS_DRV_SetMuxModeSel(PORTC, ePIN_1, PORT_MUX_ALT3);   //LPSPI0_SOUT
//			PINS_DRV_SetMuxModeSel(PORTC, ePIN_15, PORT_MUX_ALT3);  //LPSPI0_SCK
//			PINS_DRV_SetMuxModeSel(PORTC, ePIN_0, PORT_MUX_ALT3);   //LPSPI0_SIN
//			//初始化SPI3
//			SPI_MasterInit(&spi3_palInstance, &spi3_MasterConfig0);
//			break;

		/*外部FLASH通讯,flexio配置*/
		case eSPI3:
			//初始化通讯管脚
			//PINS_DRV_SetMuxModeSel(PORTE, ePIN_16, PORT_MUX_ALT1);//LPSPI1_CS
			PINS_DRV_SetMuxModeSel(PORTD, ePIN_0, PORT_MUX_ALT6);   //LPSPI1_SOUT
			PINS_DRV_SetMuxModeSel(PORTE, ePIN_15, PORT_MUX_ALT6);  //LPSPI1_SCK
			PINS_DRV_SetMuxModeSel(PORTD, ePIN_1, PORT_MUX_ALT6);   //LPSPI1_SIN
			//初始化SPI4
		    /* Init FlexIO device */
		    FLEXIO_DRV_InitDevice(INST_FLEXIO_SPI1, &flexIODeviceState);
		    /* Call the init function for FlexIO SPI driver */
		    FLEXIO_SPI_DRV_MasterInit(INST_FLEXIO_SPI1, &flexio_spi1_MasterConfig0, &flexio_spi_master_state);
			break;

		default:
			break;

    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPSPIGetWorkState(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : SPI工作状态 0:空闲 1:忙
** @Function  : 获取SPI工作状态
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPSPIGetWorkState(e_SPINum spiNum)
{
    /*模块未使能不进行初始化*/
    if(((eSPI0 == spiNum) && (1 != gBSPFunCfg.Bits.SPI0En))
        || ((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI2 == spiNum) && (1 != gBSPFunCfg.Bits.SPI2En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(0xff);
    }
    
    if(spiNum >= eSPINUM)
    {
        return(0xff);
    }
    
    return(sSpiWorkState[spiNum]);
}

/*=============================================================================================
** @Name      : u8 BSPSPISendData(e_SPINum spiNum, u16 data)
** @Input     : spiNum:SPI通道号 data 需要发送的数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SPI发送数据
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPSPISendData(e_SPINum spiNum, u16 data)
{
    /*模块未使能不进行初始化*/
    if(((eSPI0 == spiNum) && (1 != gBSPFunCfg.Bits.SPI0En))
        || ((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI2 == spiNum) && (1 != gBSPFunCfg.Bits.SPI2En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(FALSE);
    }
    
    switch(spiNum)
    {
        /*SPI1发送*/
        case eSPI0:
            /*SPI正忙*/
            if(1 == sSpiWorkState[spiNum])
            {
                return(FALSE);
            }
            
            /*SPI切换为忙*/
            sSpiWorkState[spiNum] = 1;

            /*发送一个字节*/
            SPI_I2S_SendData(eSPI0, data);
            
            /*读取接收数据*/
            sSpiRcvData[spiNum] = SpiReceiveData(eSPI0);

            /*SPI切换为空闲*/
            sSpiWorkState[spiNum] = 0;
            break;
#if 0
        /*SPI2发送*/
        case eSPI1:
            /*SPI正忙*/
            if(1 == sSpiWorkState[spiNum])
            {
                return(FALSE);
            }
            
            /*SPI切换为忙*/
            sSpiWorkState[spiNum] = 1;

            /*等待SPI空闲*/
            while(SPI_GetStatus(&spi2_palInstance) == STATUS_BUSY)
            {
                cnt++;
                /*防止死机计数*/
                if(cnt >= 10000)
                {
                    cnt = 0;
                    
                    /*返回错误*/
                    return(FALSE);
                }
            }

            cnt = 0;

            /*发送一个字节*/
            SPI_I2S_SendData(eSPI1, data);

            /*等待接收完毕*/
            while(SPI_GetStatus(&spi2_palInstance) == STATUS_BUSY)
            {
                cnt++;
                /*防止死机计数*/
                if(cnt >= 1000)
                {
                    cnt = 0;
                    
                    //SPI切换为空闲
                    sSpiWorkState[spiNum] = 0;
                    
                    /*回复默认值*/
                    sSpiRcvData[spiNum] = 0xffff;
                    return(FALSE);
                }
            }

            cnt = 0;

            /*读取接收数据*/
            sSpiRcvData[spiNum] = SpiReceiveData(eSPI1);
            
            /*SPI切换为空闲*/
            sSpiWorkState[spiNum] = 0;
            break;
            
		/*SPI3发送*/
		case eSPI2:
			/*SPI正忙*/
			if(1 == sSpiWorkState[spiNum])
			{
				return(FALSE);
			}

			/*SPI切换为忙*/
			sSpiWorkState[spiNum] = 1;

			/*等待SPI空闲*/
			while(SPI_GetStatus(&spi3_palInstance) == STATUS_BUSY)
			{
				cnt++;
				/*防止死机计数*/
				if(cnt >= 1000)
				{
					cnt = 0;

					/*返回错误*/
					return(FALSE);
				}
			}

			cnt = 0;

			/*发送一个字节*/
			SPI_I2S_SendData(eSPI2, data);

//			/*等待接收完毕*/
//			while(SPI_GetStatus(&spi3_palInstance) == STATUS_BUSY)
//			{
//				cnt++;
//				/*防止死机计数*/
//				if(cnt >= 100)
//				{
//					cnt = 0;
//
//					/*SPI切换为空闲*/
//					sSpiWorkState[spiNum] = 0;
//
//					/*回复默认值*/
//					sSpiRcvData[spiNum] = 0xffff;
//					return(sSpiRcvData[spiNum]);
//				}
//			}

			cnt = 0;

			/*读取接收数据*/
			sSpiRcvData[spiNum] = SpiReceiveData(eSPI2);

			/*SPI切换为空闲*/
			sSpiWorkState[spiNum] = 0;
			break;
#endif
		//SPI4发送
		case eSPI3:
			/*SPI正忙*/
			if(1 == sSpiWorkState[spiNum])
			{
				return(FALSE);
			}

			/*SPI切换为忙*/
			sSpiWorkState[spiNum] = 1;

			/*发送一个字节*/
			SPI_I2S_SendData(eSPI3, data);

			/*读取接收数据*/
			sSpiRcvData[spiNum] = SpiReceiveData(eSPI3);

			/*SPI切换为空闲*/
			sSpiWorkState[spiNum] = 0;
			break;

        default:
            return(FALSE);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u16 BSPSPIGetRcvData(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : void
** @Function  : 获得SPI接收到寄存器的数据
** @The notes : 用户调用
**===========================================================================================*/
u16 BSPSPIGetRcvData(e_SPINum spiNum)
{
    /*模块未使能不进行初始化*/
    if(((eSPI0 == spiNum) && (1 != gBSPFunCfg.Bits.SPI0En))
        || ((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI2 == spiNum) && (1 != gBSPFunCfg.Bits.SPI2En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(0xffff);
    }

    return(sSpiRcvData[spiNum]);
}

/*=============================================================================================
** @Name      : void BSPSPIRcvIntHandle(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : void
** @Function  : SPI接收中断函数
** @The notes : SPI中断调用,暂不使用
**===========================================================================================*/
void BSPSPIRcvIntHandle(e_SPINum spiNum)
{
    if(spiNum >= eSPINUM)
    {
        return;
    }
    
    sSpiWorkState[spiNum] = 0;          //SPI切换为空闲
    
    //SPI_ISR_Hook(spiNum);               //SPI中断处理
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 SpiReceiveData(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : void
** @Function  : 获取SPI接收数据
** @The notes :
**===========================================================================================*/
static u16 SpiReceiveData(e_SPINum spiNum)
{
	u16 errNum1 = 0;
//	u16 errNum2 = 0;
//	u16 errNum3 = 0;
	u16 receiveData = 0;

	if(eSPI0 == spiNum)
	{
		//receiveData = RxBuffer[spiNum];
		while((LPSPI0->SR & LPSPI_SR_RDF_MASK) >> LPSPI_SR_RDF_SHIFT == 0)/* Wait at least one RxFIFO entry */
		{
			if(errNum1 > 2000)/*防止程序卡死*/
			{
				return(0xffff);
			}
			errNum1++;
		}
		receiveData = LPSPI0->RDR;       /* Read received data */
		LPSPI0->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
	}
//	else if(eSPI1 == spiNum)
//	{
//		//receiveData = RxBuffer[spiNum];
//		while((LPSPI1->SR & LPSPI_SR_RDF_MASK) >> LPSPI_SR_RDF_SHIFT == 0)/* Wait at least one RxFIFO entry */
//		{
//			if(errNum2 > 2000)/*防止程序卡死*/
//			{
//				return(0xffff);
//			}
//			errNum2++;
//		}
//		receiveData = LPSPI1->RDR;       /* Read received data */
//		LPSPI1->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
//	}
//	else if(eSPI2 == spiNum)
//	{
//		//receiveData = RxBuffer[spiNum];
//		while((LPSPI2->SR & LPSPI_SR_RDF_MASK) >> LPSPI_SR_RDF_SHIFT == 0)/* Wait at least one RxFIFO entry */
//		{
//			if(errNum3 > 2000)/*防止程序卡死*/
//			{
//				return(0xffff);
//			}
//			errNum3++;
//		}
//
//		receiveData = LPSPI2->RDR;       /* Read received data */
//		LPSPI2->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
//	}
	else
	{
		receiveData = RxBuffer[spiNum];
	}

	return(receiveData);
}

/*=============================================================================================
** @Name      : static void SPI_I2S_SendData(e_SPINum spiNum, u16 data)
** @Input     : spiNum:SPI通道号  data:需要发送的数据
** @Output    : void
** @Function  : SPI发送数据
** @The notes :
**===========================================================================================*/
static void SPI_I2S_SendData(e_SPINum spiNum, u16 data)
{
	u16 errNum1 = 0;
//	u16 errNum2 = 0;
//	u16 errNum3 = 0;

	/*官方底层库发送一个u8数据，不考虑发送超时时间*/
	if(eSPI0 == spiNum)
	{
		//SPI_MasterTransfer(&spi1_palInstance, &data, &RxBuffer[eSPI0], 1);
		while((LPSPI0->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0)/* Wait for Tx FIFO available */
		{
			if(errNum1 > 2000)/*防止程序卡死*/
			{
				return;
			}
			errNum1++;
		}

		LPSPI0->TDR = data; 			   /* Transmit data */
		LPSPI0->SR |= LPSPI_SR_TDF_MASK;   /* Clear TDF flag */
	}
//	else if(eSPI1 == spiNum)
//	{
//		//SPI_MasterTransfer(&spi2_palInstance, &data, &RxBuffer[eSPI1], 1);
//		while((LPSPI1->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0)/* Wait for Tx FIFO available */
//		{
//			if(errNum2 > 2000)/*防止程序卡死*/
//			{
//				return;
//			}
//			errNum2++;
//		}
//
//		LPSPI1->TDR = data; 			   /* Transmit data */
//		LPSPI1->SR |= LPSPI_SR_TDF_MASK;   /* Clear TDF flag */
//	}
//	else if(eSPI2 == spiNum)
//	{
//		//SPI_MasterTransfer(&spi3_palInstance, &data, &Spi2RxBuffer, 1);
//		while((LPSPI2->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0)/* Wait for Tx FIFO available */
//		{
//			if(errNum3 > 2000)/*防止程序卡死*/
//			{
//				return;
//			}
//			errNum3++;
//		}
//
//		LPSPI2->TDR = data; 			   /* Transmit data */
//		LPSPI2->SR |= LPSPI_SR_TDF_MASK;   /* Clear TDF flag */
//	}
	else
	{
		FLEXIO_SPI_DRV_MasterTransfer(&flexio_spi_master_state, (const u8*)&data, &RxBuffer[eSPI3], 1);
	}
}

