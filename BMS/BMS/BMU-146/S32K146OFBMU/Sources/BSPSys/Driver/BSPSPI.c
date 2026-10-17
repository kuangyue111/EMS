/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPSPI.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : SPIͨѶ��������ģ��
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "status.h"
#include "spi_pal.h"
#include "spi1_pal.h"
#include "spi2_pal.h"
#include "spi3_pal.h"
#include "flexio_spi1.h"
#include "flexio_spi_driver.h"
#include "lpspi_shared_function.h"
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
static u8 RxBuffer[eSPINUM] = {0};      //SPI�������ݻ��壬one frame lenth is 16bits

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 SpiReceiveData(e_SPINum spiNum)
** @Input     : spiNum:SPIͨ����
** @Output    : void
** @Function  : ��ȡSPI��������
** @The notes :
**===========================================================================================*/
static u16 SpiReceiveData(e_SPINum spiNum);

/*=============================================================================================
** @Name      : static void SPI_I2S_SendData(e_SPINum spiNum, u16 data)
** @Input     : spiNum:SPIͨ����  data:��Ҫ���͵�����
** @Output    : void
** @Function  : SPI��������
** @The notes :
**===========================================================================================*/
static void SPI_I2S_SendData(e_SPINum spiNum, u16 data);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPSPIInit(e_SPINum spiNum)
** @Input     : spiNum:SPIͨ����
** @Output    : ��ʼ����� TRUE:�ɹ� FALSE:ʧ��
** @Function  : SPI����ģ���ʼ��
** @The notes : BSP����
**===========================================================================================*/
u8 BSPSPIInit(e_SPINum spiNum)
{
    /*ģ��δʹ�ܲ����г�ʼ��*/
    if(((eSPI0 == spiNum) && (1 != gBSPFunCfg.Bits.SPI0En))
        || ((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI2 == spiNum) && (1 != gBSPFunCfg.Bits.SPI2En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(FALSE);
    }
    switch(spiNum)
    {
    	/*SBC��ԴоƬͨѶ��ʼ��*/
		case eSPI0:
			//PINS_DRV_SetMuxModeSel(PORTE, ePIN_6, PORT_MUX_ALT1);//LPSPI0_CS
			PINS_DRV_SetMuxModeSel(PORTE, ePIN_2, PORT_MUX_ALT2);  //LPSPI0_SOUT
			PINS_DRV_SetMuxModeSel(PORTE, ePIN_0, PORT_MUX_ALT2);  //LPSPI0_SCK
			PINS_DRV_SetMuxModeSel(PORTE, ePIN_1, PORT_MUX_ALT2);  //LPSPI0_SIN
			//��ʼ��SPI1
			SPI_MasterInit(&spi1_palInstance, &spi1_MasterConfig0);
			break;

    	/*��ѹ����оƬLTC2949ͨѶ*/
		case eSPI1:
			//��ʼ��ͨѶ�ܽ�
			//PINS_DRV_SetMuxModeSel(PORTB, ePIN_17, PORT_MUX_ALT1);//LPSPI0_CS
			PINS_DRV_SetMuxModeSel(PORTB, ePIN_16, PORT_MUX_ALT3);  //LPSPI0_SOUT
			PINS_DRV_SetMuxModeSel(PORTB, ePIN_14, PORT_MUX_ALT3);  //LPSPI0_SCK
			PINS_DRV_SetMuxModeSel(PORTB, ePIN_15, PORT_MUX_ALT3);  //LPSPI0_SIN
			//��ʼ��SPI2
			SPI_MasterInit(&spi2_palInstance, &spi2_MasterConfig0);
			break;

		/*�ջ���ͨѶ��ʼ��*/
		case eSPI2:
			//��ʼ��ͨѶ�ܽ�
			//PINS_DRV_SetMuxModeSel(PORTC, ePIN_14, PORT_MUX_ALT1);//LPSPI0_CS
			//PINS_DRV_SetMuxModeSel(PORTE, ePIN_13, PORT_MUX_ALT1);//LPSPI0_CS
			PINS_DRV_SetMuxModeSel(PORTC, ePIN_1, PORT_MUX_ALT3);   //LPSPI0_SOUT
			PINS_DRV_SetMuxModeSel(PORTC, ePIN_15, PORT_MUX_ALT3);  //LPSPI0_SCK
			PINS_DRV_SetMuxModeSel(PORTC, ePIN_0, PORT_MUX_ALT3);   //LPSPI0_SIN
			//��ʼ��SPI3
			SPI_MasterInit(&spi3_palInstance, &spi3_MasterConfig0);
			break;

		/*�ⲿFLASHͨѶ,flexio����*/
		case eSPI3:
			//��ʼ��ͨѶ�ܽ�
			//PINS_DRV_SetMuxModeSel(PORTE, ePIN_16, PORT_MUX_ALT1);//LPSPI1_CS
			PINS_DRV_SetMuxModeSel(PORTD, ePIN_0, PORT_MUX_ALT6);   //LPSPI1_SOUT
			PINS_DRV_SetMuxModeSel(PORTE, ePIN_15, PORT_MUX_ALT6);  //LPSPI1_SCK
			PINS_DRV_SetMuxModeSel(PORTD, ePIN_1, PORT_MUX_ALT6);   //LPSPI1_SIN
			//��ʼ��SPI4
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
** @Input     : spiNum:SPIͨ����
** @Output    : SPI����״̬ 0:���� 1:æ
** @Function  : ��ȡSPI����״̬
** @The notes : �û�����
**===========================================================================================*/
u8 BSPSPIGetWorkState(e_SPINum spiNum)
{
    /*ģ��δʹ�ܲ����г�ʼ��*/
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
** @Input     : spiNum:SPIͨ���� data ��Ҫ���͵�����
** @Output    : TRUE:�ɹ� FALSE:ʧ��
** @Function  : SPI��������
** @The notes : �û�����
**===========================================================================================*/
u8 BSPSPISendData(e_SPINum spiNum, u16 data)
{
//    u16 cnt = 0;
//    u32 txRemainingBytes = 0;
//    u32 rxRemainingBytes = 0;
    
    /*ģ��δʹ�ܲ����г�ʼ��*/
    if(((eSPI0 == spiNum) && (1 != gBSPFunCfg.Bits.SPI0En))
        || ((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI2 == spiNum) && (1 != gBSPFunCfg.Bits.SPI2En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(FALSE);
    }
    
    switch(spiNum)
    {
        /*SPI1����*/
        case eSPI0:
            /*SPI��æ*/
            if(1 == sSpiWorkState[spiNum])
            {
                return(FALSE);
            }
            
            /*SPI�л�Ϊæ*/
            sSpiWorkState[spiNum] = 1;

            /*����һ���ֽ�*/
            SPI_I2S_SendData(eSPI0, data);
            
            /*��ȡ��������*/
            sSpiRcvData[spiNum] = SpiReceiveData(eSPI0);

            /*SPI�л�Ϊ����*/
            sSpiWorkState[spiNum] = 0;
            break;
            
        /*SPI2����*/
        case eSPI1:
            /*SPI��æ*/
            if(1 == sSpiWorkState[spiNum])
            {
                return(FALSE);
            }
            
            /*SPI�л�Ϊæ*/
            sSpiWorkState[spiNum] = 1;
            /*����һ���ֽ�*/
            SPI_I2S_SendData(eSPI1, data);

            /*��ȡ��������*/
            sSpiRcvData[spiNum] = SpiReceiveData(eSPI1);
            
            /*SPI�л�Ϊ����*/
            sSpiWorkState[spiNum] = 0;
            break;
            
		/*SPI3����*/
		case eSPI2:
			/*SPI��æ*/
			if(1 == sSpiWorkState[spiNum])
			{
				return(FALSE);
			}

			/*SPI�л�Ϊæ*/
			sSpiWorkState[spiNum] = 1;

			/*����һ���ֽ�*/
			SPI_I2S_SendData(eSPI2, data);

			/*��ȡ��������*/
			sSpiRcvData[spiNum] = SpiReceiveData(eSPI2);

			/*SPI�л�Ϊ����*/
			sSpiWorkState[spiNum] = 0;
			break;

		//SPI4����
		case eSPI3:
			/*SPI��æ*/
			if(1 == sSpiWorkState[spiNum])
			{
				return(FALSE);
			}

			/*SPI�л�Ϊæ*/
			sSpiWorkState[spiNum] = 1;

			/*����һ���ֽ�*/
			SPI_I2S_SendData(eSPI3, data);

			/*��ȡ��������*/
			sSpiRcvData[spiNum] = SpiReceiveData(eSPI3);

			/*SPI�л�Ϊ����*/
			sSpiWorkState[spiNum] = 0;
			break;
            
        default:
            return(FALSE);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u16 BSPSPIGetRcvData(e_SPINum spiNum)
** @Input     : spiNum:SPIͨ����
** @Output    : void
** @Function  : ���SPI���յ��Ĵ���������
** @The notes : �û�����
**===========================================================================================*/
u16 BSPSPIGetRcvData(e_SPINum spiNum)
{
    /*ģ��δʹ�ܲ����г�ʼ��*/
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
** @Input     : spiNum:SPIͨ����
** @Output    : void
** @Function  : SPI�����жϺ���
** @The notes : SPI�жϵ���,�ݲ�ʹ��
**===========================================================================================*/
void BSPSPIRcvIntHandle(e_SPINum spiNum)
{
    if(spiNum >= eSPINUM)
    {
        return;
    }
    
    sSpiWorkState[spiNum] = 0;          //SPI�л�Ϊ����
    
    //SPI_ISR_Hook(spiNum);               //SPI�жϴ���
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 SpiReceiveData(e_SPINum spiNum)
** @Input     : spiNum:SPIͨ����
** @Output    : void
** @Function  : ��ȡSPI��������
** @The notes :
**===========================================================================================*/
static u16 SpiReceiveData(e_SPINum spiNum)
{
	u16 errNum1 = 0;
	u16 errNum2 = 0;
	u16 errNum3 = 0;
	u16 receiveData = 0;

	if(eSPI0 == spiNum)
	{
		//receiveData = RxBuffer[spiNum];
		while((LPSPI0->SR & LPSPI_SR_RDF_MASK) >> LPSPI_SR_RDF_SHIFT == 0)/* Wait at least one RxFIFO entry */
		{
			if(errNum1 > 2000)/*��ֹ������*/
			{
				return(0xffff);
			}
			errNum1++;
		}
		receiveData = LPSPI0->RDR;       /* Read received data */
		LPSPI0->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
	}
	else if(eSPI1 == spiNum)
	{
		//receiveData = RxBuffer[spiNum];
		while((LPSPI1->SR & LPSPI_SR_RDF_MASK) >> LPSPI_SR_RDF_SHIFT == 0)/* Wait at least one RxFIFO entry */
		{
			if(errNum2 > 2000)/*��ֹ������*/
			{
				return(0xffff);
			}
			errNum2++;
		}
		receiveData = LPSPI1->RDR;       /* Read received data */
		LPSPI1->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
	}
	else if(eSPI2 == spiNum)
	{
		//receiveData = RxBuffer[spiNum];
		while((LPSPI2->SR & LPSPI_SR_RDF_MASK) >> LPSPI_SR_RDF_SHIFT == 0)/* Wait at least one RxFIFO entry */
		{
			if(errNum3 > 2000)/*��ֹ������*/
			{
				return(0xffff);
			}
			errNum3++;
		}

		receiveData = LPSPI2->RDR;       /* Read received data */
		LPSPI2->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
	}
	else
	{
		receiveData = RxBuffer[spiNum];
	}

	return(receiveData);
}

/*=============================================================================================
** @Name      : static void SPI_I2S_SendData(e_SPINum spiNum, u16 data)
** @Input     : spiNum:SPIͨ����  data:��Ҫ���͵�����
** @Output    : void
** @Function  : SPI��������
** @The notes :
**===========================================================================================*/
static void SPI_I2S_SendData(e_SPINum spiNum, u16 data)
{
	u16 errNum1 = 0;
	u16 errNum2 = 0;
	u16 errNum3 = 0;
	/*�ٷ��ײ�ⷢ��һ��u8���ݣ������Ƿ��ͳ�ʱʱ��*/
	if(eSPI0 == spiNum)
	{
		//SPI_MasterTransfer(&spi1_palInstance, &data, &RxBuffer[eSPI0], 1);
		while((LPSPI0->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0)/* Wait for Tx FIFO available */
		{
			if(errNum1 > 2000)/*��ֹ������*/
			{
				return;
			}
			errNum1++;
		}

		LPSPI0->TDR = data; 			   /* Transmit data */
		LPSPI0->SR |= LPSPI_SR_TDF_MASK;   /* Clear TDF flag */
	}
	else if(eSPI1 == spiNum)
	{
		//SPI_MasterTransfer(&spi2_palInstance, &data, &RxBuffer[eSPI1], 1);
		while((LPSPI1->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0)/* Wait for Tx FIFO available */
		{
			if(errNum2 > 2000)/*��ֹ������*/
			{
				return;
			}
			errNum2++;
		}

		LPSPI1->TDR = data; 			   /* Transmit data */
		LPSPI1->SR |= LPSPI_SR_TDF_MASK;   /* Clear TDF flag */
	}
	else if(eSPI2 == spiNum)
	{
		//SPI_MasterTransfer(&spi3_palInstance, &data, &Spi2RxBuffer, 1);
		while((LPSPI2->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0)/* Wait for Tx FIFO available */
		{
			if(errNum3 > 2000)/*��ֹ������*/
			{
				return;
			}
			errNum3++;
		}

		LPSPI2->TDR = data; 			   /* Transmit data */
		LPSPI2->SR |= LPSPI_SR_TDF_MASK;   /* Clear TDF flag */
	}
	else
	{
		FLEXIO_SPI_DRV_MasterTransfer(&flexio_spi_master_state, (const u8*)&data, &RxBuffer[eSPI3], 1);
	}
}



