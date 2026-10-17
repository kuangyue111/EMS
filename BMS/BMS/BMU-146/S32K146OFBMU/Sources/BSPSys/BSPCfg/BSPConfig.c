/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPConfig.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 各驱动模块初始化接口
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPADC.h"
#include "BSPCAN.h"
#include "BSPEEPROM.h"
#include "BSPFLASH.h"
#include "BSPFTM.h"
#include "BSPGPIO.h"
#include "BSPI2C.h"
#include "BSPMCG.h"
#include "BSPPWM.h"
#include "BSPRTC.h"
#include "BSPSPI.h"
#include "BSPUART.h"
#include "BSPWDOG.h"
#include "BSPLPIT.h"
#include "BSPDMA.h"
#include "BSPIRQ.h"
#include "DEVGPIO.h"
#include "DEVADC.h"
#include "RX8010.h"
#include "EepM24512.h"
#include "SST26VF064.h"
#include "GroupInfo.h"
#include "HigAFESamp.h"
#include "CellAFESamp.h"
#include "Fs6503_PowerManag.h"
#include "BSPConfig.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
volatile u_BSPFunCfg gBSPFunCfg;
static s_BSPParaCfg gBSPParaCfg;
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPUserCfgInit(void)
** @Input     : void
** @Output    : void
** @Function  : BSP驱动功能用户配置模块参数初始化
** @The notes : Main()函数调用，上电初始化
**===========================================================================================*/
void BSPUserCfgInit(void)
{
	/*读取驱动模块使能配置*/
	gBSPFunCfg.Bits.FTM0En = FTM0_FUN_EN;		/*定时器1使能配置*/
	gBSPFunCfg.Bits.FTM1En = FTM1_FUN_EN;		/*定时器2使能配置*/
	gBSPFunCfg.Bits.FTM2En = FTM2_FUN_EN;		/*定时器3使能配置*/
	gBSPFunCfg.Bits.FTM3En = FTM3_FUN_EN;		/*定时器4使能配置*/

	gBSPFunCfg.Bits.CAN0En = CAN0_FUN_EN;		/*CAN0使能配置*/
	gBSPFunCfg.Bits.CAN1En = CAN1_FUN_EN;		/*CAN1使能配置*/
	gBSPFunCfg.Bits.CAN2En = CAN2_FUN_EN;		/*CAN2使能配置*/

	gBSPFunCfg.Bits.SCI0En = SCI0_FUN_EN;		/*SCI0使能配置*/
	gBSPFunCfg.Bits.SCI1En = SCI1_FUN_EN;		/*SCI1使能配置*/
	gBSPFunCfg.Bits.SCI2En = SCI2_FUN_EN;		/*SCI2使能配置*/

	gBSPFunCfg.Bits.SPI0En = SPI0_FUN_EN;       /*SPI1使能配置*/
	gBSPFunCfg.Bits.SPI1En = SPI1_FUN_EN;		/*SPI2使能配置*/
	gBSPFunCfg.Bits.SPI2En = SPI2_FUN_EN;		/*SPI3使能配置*/
	gBSPFunCfg.Bits.SPI3En = SPI3_FUN_EN;		/*SPI4使能配置*/

	gBSPFunCfg.Bits.I2C0En = I2C0_FUN_EN;     	/*I2C0使能配置*/
	gBSPFunCfg.Bits.I2C1En = I2C1_FUN_EN;       /*I2C1使能配置*/

	gBSPFunCfg.Bits.GPIOEn = GPIO_FUN_EN;       /*GPIO使能配置*/
	gBSPFunCfg.Bits.ADCEn  = ADC_FUN_EN;        /*ADC使能配置*/
	gBSPFunCfg.Bits.RTCEn  = RTC_FUN_EN;        /*RTC使能配置*/
	gBSPFunCfg.Bits.PWMEn  = PWM_FUN_EN;        /*PWM使能配置*/
	gBSPFunCfg.Bits.EEPEn  = EEP_FUN_EN;        /*EEP使能配置*/
	gBSPFunCfg.Bits.FLASHEn= FLASH_FUN_EN;      /*FLASH使能配置*/
	gBSPFunCfg.Bits.WDOGEn = WDOG_FUN_EN;       /*wog使能配置*/
	gBSPFunCfg.Bits.LPITEn = LPIT_FUN_EN;       /*LPIT使能配置*/
	gBSPFunCfg.Bits.DMAEn  = DMA_FUN_EN;        /*DMA使能配置*/

	/*读取驱动模块功能参数配置信息*/
	gBSPParaCfg.OscRate    = OSC_CLOCK;		   /*晶振频率(1MHz)*/
	gBSPParaCfg.BusRate    = Bus_CLOCK;        /*BUS频率(1MHz)*/
	gBSPParaCfg.Ftm0Time   = FTM0_TIMES;       /*FTM1定时中断周期(1μs)*/
	gBSPParaCfg.Ftm1Time   = FTM1_TIMES;       /*FTM1定时中断周期(1μs)*/
	gBSPParaCfg.Ftm2Time   = FTM2_TIMES;       /*FTM2定时中断周期(1μs)*/
	gBSPParaCfg.Ftm3Time   = FTM3_TIMES;       /*FTM3定时中断周期(1μs)*/
	gBSPParaCfg.Can0Rate   = CAN0_BPS;         /*CAN0通信波特率(1kbps)*/
	gBSPParaCfg.Can1Rate   = CAN1_BPS;         /*CAN1通信波特率(1kbps)*/
	gBSPParaCfg.Can2Rate   = CAN2_BPS;         /*CAN2通信波特率(1kbps)*/
	gBSPParaCfg.Sci0Rate   = SCI0_BPS;         /*SCI0通信波特率(1bps)*/
	gBSPParaCfg.Sci1Rate   = SCI1_BPS;         /*SCI1通信波特率(1bps)*/
	gBSPParaCfg.Sci2Rate   = SCI2_BPS;         /*SCI2通信波特率(1bps)*/
	gBSPParaCfg.Spi0Rate   = SPI0_BPS;         /*SPI0通信波特率(1Mbps)*/
	gBSPParaCfg.Spi1Rate   = SPI1_BPS;         /*SPI1通信波特率(1Mbps)*/
	gBSPParaCfg.Spi2Rate   = SPI2_BPS;         /*SPI2通信波特率(1Mbps)*/
	gBSPParaCfg.Spi3Rate   = SPI3_BPS;         /*SPI3通信波特率(1Mbps)*/
	//gBSPParaCfg.I2cRate;           		   /*I2C通信波特率(1Kbps)*/
	gBSPParaCfg.WdogTime   = WATCH_DOG_FREQ;   /*WDOG周期(1ms)*/
}

/*=============================================================================================
** @Name      : void BSPUserCfgInit(void)
** @Input     : void
** @Output    : void
** @Function  : 所有底层驱动模块进行初始化
** @The notes : Main()函数调用，上电初始化
**===========================================================================================*/
void BSPDriverInit(void)
{
	if(1 == gBSPFunCfg.Bits.WDOGEn)                                  		    /*内部看门狗模块使能*/
	{
		(void)BSPWatchDogInit(gBSPParaCfg. WdogTime);                		    /*初始化看门狗*/
	}

	(void)BSPCRGInit(gBSPParaCfg.OscRate, gBSPParaCfg.BusRate);         	    /*初始化时钟倍频*/

    BSPWatchDogTask();                                                          /*喂狗*/

    if(1 == gBSPFunCfg.Bits.SCI0En)                                             /*SCI_0使能*/
    {
        (void)BSPSCIInit(eSCI0, gBSPParaCfg.Sci0Rate, gBSPParaCfg.BusRate);     /*初始化SCI_0*/
    }

    if(1 == gBSPFunCfg.Bits.SCI1En)                                             /*SCI_1使能*/
    {
        (void)BSPSCIInit(eSCI1, gBSPParaCfg.Sci1Rate, gBSPParaCfg.BusRate);     /*初始化SCI_1*/
    }

    if(1 == gBSPFunCfg.Bits.SCI2En)                                             /*SCI_2使能*/
    {
        (void)BSPSCIInit(eSCI2, gBSPParaCfg.Sci2Rate, gBSPParaCfg.BusRate);     /*初始化SCI_2*/
    }

    if(1 == gBSPFunCfg.Bits.FTM0En)                                             /*第一路FTM使能*/
    {
    	/*第一路定时器用作输出PWM,初始化由PWM模块完成初始化*/
        //(void)BSPFTMInit(eFTM0, gBSPParaCfg.Ftm0Time, gBSPParaCfg.BusRate);   /*初始化第一路FTM*/
    }

    if(1 == gBSPFunCfg.Bits.FTM1En)                                             /*第二路FTM使能*/
    {
        (void)BSPFTMInit(eFTM1, gBSPParaCfg.Ftm1Time, gBSPParaCfg.BusRate);     /*初始化第二路FTM*/
    }

    if(1 == gBSPFunCfg.Bits.FTM2En)                                             /*第三路FTM使能*/
    {
    	/*第三路定时器用作PWM输入捕获*/
        (void)BSPFTMInit(eFTM2, gBSPParaCfg.Ftm2Time, gBSPParaCfg.BusRate);     /*初始化第三路FTM*/
    }

    if(1 == gBSPFunCfg.Bits.FTM3En)                                             /*第四路FTM使能*/
    {
        (void)BSPFTMInit(eFTM3, gBSPParaCfg.Ftm3Time, gBSPParaCfg.BusRate);     /*初始化第四路FTM*/
    }

    BSPWatchDogTask();                                                          /*喂狗*/

    BSPCanBufInit();                                                            /*CAN模块接收/发送缓冲区初始化*/

    if(1 == gBSPFunCfg.Bits.CAN0En)                                             /*CAN0使能*/
    {
        (void)BSPCANInit(eCAN0, gBSPParaCfg.BusRate, gBSPParaCfg.Can0Rate);     /*初始化CAN0*/
    }

    if(1 == gBSPFunCfg.Bits.CAN1En)                                             /*CAN1使能*/
    {
        (void)BSPCANInit(eCAN1, gBSPParaCfg.BusRate, gBSPParaCfg.Can1Rate);     /*初始化CAN1*/
    }

    if(1 == gBSPFunCfg.Bits.CAN2En)                                             /*CAN1使能*/
    {
        (void)BSPCANInit(eCAN2, gBSPParaCfg.BusRate, gBSPParaCfg.Can2Rate);     /*初始化CAN2*/
    }

    BSPWatchDogTask();                                                          /*喂狗*/

    if(1 == gBSPFunCfg.Bits.FLASHEn)										    /*内部FLASH使能*/
    {
        (void)BSPINTFLASHInit(gBSPParaCfg.BusRate);                             /*初始化内部FLASH*/
    }

    if(1 == gBSPFunCfg.Bits.EEPEn)                                              /*内部EEPROM使能*/
    {
        (void)BSPINTEEPInit(gBSPParaCfg.BusRate);                               /*初始化内部EEPROM*/
    }

    BSPWatchDogTask();                                    /*喂狗*/

    if(1 == gBSPFunCfg.Bits.SPI0En)                       /*SPI_0使能*/
    {
        (void)BSPSPIInit(eSPI0);                          /*初始化SPI_0*/
    }

    if(1 == gBSPFunCfg.Bits.SPI1En)                       /*SPI_1使能*/
    {
        (void)BSPSPIInit(eSPI1);                          /*初始化SPI_1*/
    }

    if(1 == gBSPFunCfg.Bits.SPI2En)                       /*SPI_2使能*/
    {
        (void)BSPSPIInit(eSPI2);                          /*初始化SPI_2*/
    }

    if(1 == gBSPFunCfg.Bits.SPI3En)                       /*SPI_3使能*/
    {
        (void)BSPSPIInit(eSPI3);                          /*初始化SPI_3*/
    }

    if(1 == gBSPFunCfg.Bits.I2C0En)                       /*IIC_0使能*/
    {
        (void)BSPI2CInit(eIIC0);                          /*初始化内部IIC_0*/
    }

    if(1 == gBSPFunCfg.Bits.I2C1En)                       /*IIC_1使能*/
    {
        (void)BSPI2CInit(eIIC1);                          /*初始化内部IIC_1*/
    }

    if(1 == gBSPFunCfg.Bits.PWMEn)                        /*PWM使能*/
    {
    	BSPPWMInit(ePWM_MOD0, ePWM_CH6, 1, 10000, 40000); /*初始化PWM(输出[频率2K])*/
    }

    if(1 == gBSPFunCfg.Bits.LPITEn)						  /*LPIT使能*/
    {
    	BSPLPITInit(eLPIT0);							  /*初始化LPIT*/
    }

    if(1 == gBSPFunCfg.Bits.DMAEn)						  /*DMA使能*/
    {
    	BSPDMAInit();							  		  /*初始化DMA*/
    }

    BSPWatchDogTask();                                    /*喂狗*/
    DEVDriverInit();							          /*初始化外围硬件*/
}

/*=============================================================================================
** @Name      : void BSPUserCfgInit(void)
** @Input     : void
** @Output    : void
** @Function  : 所有外围设备驱动模块进行初始化
** @The notes : Main()函数调用，上电初始化。必须在底层驱动初始化完成后调用
**===========================================================================================*/
void DEVDriverInit(void)
{
	(void)DEVGPIOInit();   		/*初始化GPIO端口*/

	(void)DEVADCInit();       	/*初始化ADC设备通道*/

	(void)Fs6503Init();			/*电源管理芯片初始化*/

	(void)DEVRTCRX8010Init();	/*外置RTC时钟芯片初始化*/

	(void)EXTEep24M512Init();	/*外部EEPROM芯片初始化*/
	(void)EXTFlashSST26Init();	/*外部FLASH芯片初始化*/
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
