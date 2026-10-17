/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : DEVGPIO.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 输入输出IO管理函数模块
** @Instructions : 根据PCB外围设备编写管理函数
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "LowADCSamp.h"
#include "pins_driver.h"
#include "interrupt_manager.h"
#include "pin_mux.h"
#include "BSPGPIO.h"
#include "DEVGPIO.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sDevMulPortInitDirect = 0xff;      /*当前复用管脚被配置为出入输出状态：0xff:无效    1：输入  2：输出*/

static u8 sDevMulPortSwhCtrlState = 0;      /*按位表示复用端口接触器1-8控制状态(0断开 1闭合)*/
static u8 sDevMulPortSwhEnCtrlState = 0;    /*按位表示复用端口输出干接点控制状态(0断开 1闭合)[bit0~3为高边开关电流回检使能,bit4~5:低边开关1~2控制 bit6:高边开关9控制 bit7:高边开关电流采样选通]*/
static u8 sDevMulPortDryCtrlState = 0;      /*按位表示复用端口输出干接点控制状态(0断开 1闭合)[bit0~2为输出干接点控制,bit3:绝缘检测高边开关低压侧控制信号,bit4~5:LTC6820使能管脚,bit6:K4_2nd(低压电源第2路控制输出)*/
static u8 sDevMulPortInputState = 0;        /*按位表示复用端口输入干接点回检状态(0断开 1闭合)[bit0:HALL电源过载故障状态 bit1~2：低边开关控制回检状态  bit3:K9高边开关回检状态]*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u8 GPIOMulPinDirInit(u8 dir)
** @Input     : dir:初始化方向 0输入 1输出
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 输入输出复用IO管脚方向初始化
** @The notes : 复用管脚PD8~PD15
**===========================================================================================*/
static u8 GPIOMulPinDirInit(u8 dir);

/*=============================================================================================
** @Name      : static u8 GPIOMulPinSetAllOutputState(u8 state)
** @Input     : state:按位表示各复用IO输出状态 0低电平 1高电平
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 设置输入输出复用IO管脚输出状态
** @The notes : 复用管脚PD8~PD15
**===========================================================================================*/
static u8 GPIOMulPinSetAllOutputState(u8 state);

/*=============================================================================================
** @Name      : static u8 GPIOMulPinGetAllPortState(void)
** @Input     : void
** @Output    : 按位表示各复用IO管脚当前端口状态 0低电平 1高电平
** @Function  : 获取输入输出复用IO管脚当前端口状态
** @The notes : 复用管脚PD8~PD15
**===========================================================================================*/
static u8 GPIOMulPinGetAllPortState(void);

/*=============================================================================================
** @Name      : static void GPIOMulPortWriteAllSwhState(void)
** @Input     : void
** @Output    : void
** @Function  : 写入所有输入输出复用IO口接触器输出状态
** @The notes : 包括译码器设置,使能HS1-8控制
**===========================================================================================*/
static void GPIOMulPortWriteAllSwhState(void);

/*=============================================================================================
** @Name      : static void GPIOMulPortWriteAllDrvEnState(void)
** @Input     : void
** @Output    : void
** @Function  : 写入所有输入输出复用IO口回检诊断使能和低边开关输出状态
** @The notes : 包括译码器设置,使能HS1-4电流回检/LS1-2控制/HS9控制/HS电流通道片选
**===========================================================================================*/
static void GPIOMulPortWriteAllDrvEnState(void);

/*=============================================================================================
** @Name      : static void GPIOMulPortWriteAllDryState(void)
** @Input     : void
** @Output    : void
** @Function  : 写入所有输入输出复用IO口干接点输出状态和绝缘检测低边开关输出状态
** @The notes : 包括译码器设置,使能干接点1-3/PE控制/菊花链1-2/低压第2路控制
**===========================================================================================*/
static void GPIOMulPortWriteAllDryState(void);

/*=============================================================================================
** @Name      : static u8 GPIOMulPortReadAllDryState(void)
** @Input     : void
** @Output    : 按位表示各复用IO管脚输入干接点[0,5]状态 0开 1闭
** @Function  : 读取所有输入输出复用IO口干接点输入状态
** @The notes : 包括译码器设置,使能霍尔回检/LS1-2回检/HS9回检状态
**===========================================================================================*/
static u8 GPIOMulPortReadAllDryState(void);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 DEVGPIOInit(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : GPIO端口初始化
** @The notes : 只初始化板级提供的资源
**===========================================================================================*/
u8 DEVGPIOInit(void)
{
    u8 result = TRUE;
    //PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);/*S32K146 —— PE管脚配置初始化(暂不实现)*/

    //GPIO输入端口初始化
    result &= BSPGPIOInit(ePOR_B, ePIN_1, ePORT_MUX_AS_GPIO, 0);    /*输入干接点1(无源,外接信号与电源+串联接入,COM接电源-)*/
    result &= BSPGPIOInit(ePOR_B, ePIN_0, ePORT_MUX_AS_GPIO, 0);    /*输入干接点2(无源,外接信号与电源+串联接入,COM接电源-)*/
    result &= BSPGPIOInit(ePOR_A, ePIN_7, ePORT_MUX_AS_GPIO, 0);    /*输入干接点3(无源,外接信号与电源+串联接入,COM接电源-)*/
    result &= BSPGPIOInit(ePOR_A, ePIN_6, ePORT_MUX_AS_GPIO, 0);    /*输入干接点4(无源,外接信号与电源+串联接入,COM接电源-)*/
    result &= BSPGPIOInit(ePOR_E, ePIN_7, ePORT_MUX_AS_GPIO, 0);    /*输入干接点5(无源,外接信号与电源+串联接入,COM接电源-)*/
    result &= BSPGPIOInit(ePOR_B, ePIN_10, ePORT_MUX_AS_GPIO, 0);   /*输入干接点6(无源,外接信号与电源+串联接入,COM接电源-)*/

    result &= BSPGPIOInit(ePOR_C, ePIN_8, ePORT_MUX_AS_GPIO, 0);    /*硬件版本号端口1*/
    result &= BSPGPIOInit(ePOR_C, ePIN_9, ePORT_MUX_AS_GPIO, 0);    /*硬件版本号端口2*/
    result &= BSPGPIOInit(ePOR_C, ePIN_10, ePORT_MUX_AS_GPIO, 0);   /*硬件版本号端口3*/

    result &= BSPGPIOInit(ePOR_A, ePIN_14, ePORT_MUX_AS_GPIO, 0);   /*KEA128从芯片心跳回检端口*/

    result &= BSPGPIOInit(ePOR_A, ePIN_5, ePORT_MUX_AS_GPIO, 0);	/*FS6503_RST*/
    result &= BSPGPIOInit(ePOR_B, ePIN_8, ePORT_MUX_AS_GPIO, 0);	/*FS6503_INT*/

    //GPIO输出端口初始化
    result &= BSPGPIOInit(ePOR_A, ePIN_11, ePORT_MUX_AS_GPIO, 1);   /*板上LED*/
    result &= BSPGPIOInit(ePOR_B, ePIN_9, ePORT_MUX_AS_GPIO, 1);    /*自身电源保持使能*/

    result &= BSPGPIOInit(ePOR_B, ePIN_4, ePORT_MUX_AS_GPIO, 1);    /*485-1接收使能*/
    result &= BSPGPIOInit(ePOR_E, ePIN_9, ePORT_MUX_AS_GPIO, 1);    /*485-2接收使能*/
    result &= BSPGPIOInit(ePOR_D, ePIN_11, ePORT_MUX_AS_GPIO, 1);   /*485-3接收使能*/

    result &= BSPGPIOInit(ePOR_B, ePIN_11, ePORT_MUX_AS_GPIO, 1);   /*外部EEP 写保护(低电平)*/
    result &= BSPGPIOInit(ePOR_B, ePIN_17, ePORT_MUX_AS_GPIO, 1);   /*LTC2949片选*/

    result &= BSPGPIOInit(ePOR_E, ePIN_11, ePORT_MUX_AS_GPIO, 1);   /*外部FLASH 写保护(低电平)*/
    result &= BSPGPIOInit(ePOR_E, ePIN_16, ePORT_MUX_AS_GPIO, 1);   /*外部FLASH SPI片选*/

    result &= BSPGPIOInit(ePOR_C, ePIN_14, ePORT_MUX_AS_GPIO, 1);   /*菊花链LTC6820_1片选*/
    result &= BSPGPIOInit(ePOR_E, ePIN_13, ePORT_MUX_AS_GPIO, 1);   /*菊花链LTC6820_2片选*/

    //result &= BSPGPIOInit(ePOR_E, ePIN_1, ePORT_MUX_AS_GPIO, 1);  /*管脚复用功能译码器失效控制信号(低有效)*/
    result &= BSPGPIOInit(ePOR_D, ePIN_8, ePORT_MUX_AS_GPIO, 1);    /*管脚复用功能译码器片选(低有效)*/
    result &= BSPGPIOInit(ePOR_C, ePIN_13, ePORT_MUX_AS_GPIO, 1);   /*管脚复用功能译码器编码1*/
    result &= BSPGPIOInit(ePOR_C, ePIN_12, ePORT_MUX_AS_GPIO, 1);   /*管脚复用功能译码器编码2*/
    result &= BSPGPIOInit(ePOR_C, ePIN_11, ePORT_MUX_AS_GPIO, 1);   /*管脚复用功能译码器编码3*/

    result &= BSPGPIOInit(ePOR_E, ePIN_6, ePORT_MUX_AS_GPIO, 1);	/*电源管理芯片FS6503片选信号*/

    result &= BSPGPIOInit(ePOR_A, ePIN_17, ePORT_MUX_AS_GPIO, 1);	/*主MCU到KEA128从MCU的心跳信号*/
    result &= BSPGPIOInit(ePOR_E, ePIN_10, ePORT_MUX_AS_GPIO, 1);   /*主MCU到KEA128从MCU SPI片选*/

//    /* Enable interrupts on ports */
//    INT_SYS_EnableIRQ(PORTA_IRQn);/*FS6503_RST*/
//    INT_SYS_EnableIRQ(PORTB_IRQn);/*FS6503_INT*/
//    INT_SYS_EnableIRQGlobal();

    /*初始化平铺管脚初始电平*/
    (void)DEVGPIOSetPortState(ePOut_PwHold, ePSTATE_ON);            /*自身电源保持使能有效(高电平有效)*/
    (void)DEVGPIOSetPortState(ePOut_FlaWPr, ePSTATE_OFF);           /*外部FLASH写保护使能(低电平有效)*/
    (void)DEVGPIOSetPortState(ePOut_FlaCS, ePSTATE_ON);             /*外部FLASH SPI片选禁止(低电平有效)*/
    (void)DEVGPIOSetPortState(ePOut_LTC2949CS, ePSTATE_OFF);        /*高压采集2949 SPI片选禁止(高电平有效)*/
    (void)DEVGPIOSetPortState(ePOut_128SPI_CS, ePSTATE_ON);         /*从MCU SPI片选禁止(低电平有效)*/

    (void)DEVGPIOSetPortState(ePOut_LTC6820EN1, ePSTATE_ON);        /*单体采样菊花链芯片1使能有效(高电平有效)*/
    (void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_OFF);      /*单体采样菊花链芯片1片选有效(低电平有效)*/
    (void)DEVGPIOSetPortState(ePOut_LTC6820EN2, ePSTATE_OFF);       /*单体采样菊花链芯片2使能禁用(高电平有效)*/
    (void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_ON);       /*单体采样菊花链芯片1片选禁用(低电平有效)*/

    /*初始化复用管脚器件控制状态*/
    sDevMulPortSwhCtrlState = 0;
    sDevMulPortSwhEnCtrlState = 0;
    sDevMulPortDryCtrlState = 0;
    GPIOMulPortWriteAllSwhState();
    GPIOMulPortWriteAllDrvEnState();
    GPIOMulPortWriteAllDryState();

    /*获取复用管脚器件当前输入状态*/
    sDevMulPortInputState = GPIOMulPortReadAllDryState();

    return(result);
}

/*=============================================================================================
** @Name      : void DEVGPIOMulPortManagTask(void)
** @Input     : void
** @Output    : void
** @Function  : GPIO复用端口管控任务
** @The notes : 10ms
**===========================================================================================*/
void DEVGPIOMulPortManagTask(void)
{
    u8 i = 0;
    static u8 sState = 0;
    static u8 sSwhNum = 0;
    static u8 sHisSwhState = 0;
    static u8 sHisDrVEnState = 0;

	switch(sState)
	{
		//控制接触器
		case 0:
			//定周期控制
			#if(1 == IO_MUL_CTRL_TYPE)
			//复用管脚设置接触器控制状态
			GPIOMulPortWriteAllSwhState();
			#endif
			sState = 1;
			break;

		//控制输出干接点及片选回检接触器驱动电流回检芯片
		case 1:
			//当前非回检电流则改变回检通道回检新闭合的接触器
			//if((0 == DEVGetSampHSBackCurrState())
			//    || (3 == DEVGetSampHSBackCurrState()))
			if((0 == GetADCSampHSBackCurrState())
				|| (3 == GetADCSampHSBackCurrState()))
			{
				//清空高边驱动电流回检片选
				sHisDrVEnState = sDevMulPortSwhEnCtrlState;
				//(void)DEVGPIOSetPortState(ePOut_DRVK1K2EN, ePSTATE_OFF);
				//(void)DEVGPIOSetPortState(ePOut_DRVK3K4EN, ePSTATE_OFF);
				//(void)DEVGPIOSetPortState(ePOut_DRVK5K6EN, ePSTATE_OFF);
				//(void)DEVGPIOSetPortState(ePOut_DRVK7K8EN, ePSTATE_OFF);
				sDevMulPortSwhEnCtrlState &= 0xf0;

				//前4个主要接触器有新闭合接触器则判断是否过流
				if((sHisSwhState & 0x0f) != (sDevMulPortSwhCtrlState & 0x0f))
				{
					//只检测前4个接触器是否有新闭合
					for(i = 0; i < 4; i++)
					{
						//由断开变成闭合
						//if((0 == BitGet(sHisSwhState, i))
						//    && (1 == BitGet(sDevMulPortSwhCtrlState, i)))
						//状态变化
						if(BitGet(sHisSwhState, i) != BitGet(sDevMulPortSwhCtrlState, i))
						{
							//切换该路驱动电流需要回检(两路驱动共用一个芯片回检)
							(void)DEVGPIOSetPortState((e_OutputPort)(ePOut_DRVK1K2EN + (i / 2)), ePSTATE_ON);

							//选通高边驱动芯片通道
							if(0 == (i % 2))
							{
								//1/3/5/7路接触器选通第1路通道
								(void)DEVGPIOSetPortState(ePOut_DRVDSELALL, ePSTATE_OFF);
							}
							else
							{
								//2/4/6/8路接触器选通第2路通道
								(void)DEVGPIOSetPortState(ePOut_DRVDSELALL, ePSTATE_ON);
							}

							//记录该接触器已变化与回检
							if(1 == BitGet(sDevMulPortSwhCtrlState, i))
							{
								BitSet(sHisSwhState, i);
							}
							else
							{
								BitClr(sHisSwhState, i);
							}
							break;
						}
					}
				}

				//有接触器闭合需要马上回检电流则启动电流回检
				if((sDevMulPortSwhEnCtrlState & 0x0f) > 0)
				{
					//启动驱动电流回检采样
					SampADCCurrBackHSStart(1);
				}
				//无接触器闭合需要马上回检电流则按顺序回检
				else
				{
					//需等待ADC通道完全空闲才能切换
					if(0 == GetADCSampHSBackCurrState())
					{
						//记录当前接触器控制状态
						sHisSwhState = sDevMulPortSwhCtrlState;

						//片选下一电流回检通道
						(void)DEVGPIOSetPortState((e_OutputPort)(ePOut_DRVK1K2EN + (sSwhNum / 2)), ePSTATE_ON);

						//选通高边驱动芯片通道
						if(0 == (sSwhNum % 2))
						{
							//1/3/5/7路接触器选通第1路通道
							(void)DEVGPIOSetPortState(ePOut_DRVDSELALL, ePSTATE_OFF);
						}
						else
						{
							//2/4/6/8路接触器选通第2路通道
							(void)DEVGPIOSetPortState(ePOut_DRVDSELALL, ePSTATE_ON);
						}

						//切换下一通道
						sSwhNum++;
						if(sSwhNum >= 8)
						{
							sSwhNum = 0;
						}

						//正常进行驱动电流采集回检
						SampADCCurrBackHSStart(0);
					}
					//当前ADC通道未轮循一遍
					else
					{
						//恢复原来电流片选
						sDevMulPortSwhEnCtrlState |= (sHisDrVEnState & 0x0f);
					}
				}
			}

			GPIOMulPortWriteAllDrvEnState();                  //复用管脚设置驱动电流回检片选和和低边开关输出状态
			sHisDrVEnState = sDevMulPortSwhEnCtrlState;       //bit0~3：为高边开关电流回检使能,bit4~5:低边开关1~2控制 bit6:高边开关9控制 bit7:高边开关电流采样选通
			//定周期获取
			#if(1 == IO_MUL_CTRL_TYPE)
			//进入输出端口检测
			sState = 2;

			//单次触发获取
			#else
			//保持只切换高边驱动电流回检
			sState = 1;
			#endif
			break;

		//复用IO口干接点输出状态、LTC6820控制使能、绝缘检测低边开关输出状态,没有回检信号
		case 2:
			GPIOMulPortWriteAllDryState();
			sState = 3;
			break;

		//读取HLL电源过载故障信号、低边开关K1-、K3-、高边开关K9状态
		case 3:
			sDevMulPortInputState = GPIOMulPortReadAllDryState();       //获取复用管脚当前所有管脚状态
			sState = 0;
			break;

		default:
			sState = 0;
			break;
	}
}

/*=============================================================================================
** @Name      : void DEVGPIOCheckHSCurrState(u8 channel, u16 curr)
** @Input     : channel:高边驱动通道序号 curr:回检电流 0.01A
** @Output    : void
** @Function  : 检测高边驱动电流状态状态
** @The notes : 高边驱动电流判断接触器状态
**===========================================================================================*/
void DEVGPIOCheckHSCurrState(u8 channel, u16 curr)
{
    if(channel < 8)
    {
        ;
    }
}

/*=============================================================================================
** @Name      : u8 DEVGPIOGetInPortState(e_INPUTPORT port)
** @Input     : port:输入端口号
** @Output    : 端口状态[0:断开 1:闭合]
** @Function  : 获取GPIO输入端口状态
** @The notes :
**===========================================================================================*/
u8 DEVGPIOGetInPortState(e_INPUTPORT port)
{
    u8 state = 0;
    
    switch(port)
    {
        /*输入干接点1(无源,外接信号与电源+串联接入,COM接电源-)*/
        case ePIn_DRY0:
            state = BSPGPIOGetInPortState(ePOR_B, ePIN_1);

            /*低电平(0)代表闭合(用户根据实际修改)*/
            if(0 == state)
            {
                //state = ePSTATE_OFF;
                state = ePSTATE_ON;
            }
            else if(1 == state)
            {
                //state = ePSTATE_ON;
                state = ePSTATE_OFF;
            }
            break;
        
        /*输入干接点2(无源,外接信号与电源+串联接入,COM接电源-)*/
        case ePIn_DRY1:
            state = BSPGPIOGetInPortState(ePOR_B, ePIN_0);
            
            /*低电平(0)代表闭合(用户根据实际修改)*/
            if(0 == state)
            {
                //state = ePSTATE_OFF;
                state = ePSTATE_ON;
            }
            else if(1 == state)
            {
                //state = ePSTATE_ON;
                state = ePSTATE_OFF;
            }
            break;

		/*输入干接点3(无源,外接信号与电源+串联接入,COM接电源-)*/
		case ePIn_DRY2:
			state = BSPGPIOGetInPortState(ePOR_A, ePIN_7);

			//低电平(0)代表闭合(用户根据实际修改)*/
			if(0 == state)
			{
				//state = ePSTATE_OFF;
				state = ePSTATE_ON;
			}
			else if(1 == state)
			{
				//state = ePSTATE_ON;
				state = ePSTATE_OFF;
			}
			break;

		/*输入干接点4(无源,外接信号与电源+串联接入,COM接电源-)*/
		case ePIn_DRY3:
			state = BSPGPIOGetInPortState(ePOR_A, ePIN_6);

			/*低电平(0)代表闭合(用户根据实际修改)*/
			if(0 == state)
			{
				//state = ePSTATE_OFF;
				state = ePSTATE_ON;
			}
			else if(1 == state)
			{
				//state = ePSTATE_ON;
				state = ePSTATE_OFF;
			}
			break;

		/*输入干接点5(无源,外接信号与电源+串联接入,COM接电源-)*/
		case ePIn_DRY4:
			state = BSPGPIOGetInPortState(ePOR_E, ePIN_7);

			/*低电平(0)代表闭合(用户根据实际修改)*/
			if(0 == state)
			{
				//state = ePSTATE_OFF;
				state = ePSTATE_ON;
			}
			else if(1 == state)
			{
				//state = ePSTATE_ON;
				state = ePSTATE_OFF;
			}
			break;

		/*输入干接点6(无源,外接信号与电源+串联接入,COM接电源-)*/
		case ePIn_DRY5:
			state = BSPGPIOGetInPortState(ePOR_B, ePIN_10);

			/*低电平(0)代表闭合(用户根据实际修改)*/
			if(0 == state)
			{
				//state = ePSTATE_OFF;
				state = ePSTATE_ON;
			}
			else if(1 == state)
			{
				//state = ePSTATE_ON;
				state = ePSTATE_OFF;
			}
			break;

		/*硬件版本号端口1*/
		case ePIn_HWVER0:
			state = BSPGPIOGetInPortState(ePOR_C, ePIN_8);

			/*低电平(0)代表闭合(用户根据实际修改)*/
			if(0 == state)
			{
				state = ePSTATE_OFF;
				//state = ePSTATE_ON;
			}
			else if(1 == state)
			{
				state = ePSTATE_ON;
				//state = ePSTATE_OFF;
			}
			break;

		/*硬件版本号端口2*/
		case ePIn_HWVER1:
			state = BSPGPIOGetInPortState(ePOR_C, ePIN_9);

			/*低电平(0)代表闭合(用户根据实际修改)*/
			if(0 == state)
			{
				state = ePSTATE_OFF;
				//state = ePSTATE_ON;
			}
			else if(1 == state)
			{
				state = ePSTATE_ON;
				//state = ePSTATE_OFF;
			}
			break;

		/*硬件版本号端口3*/
		case ePIn_HWVER2:
			state = BSPGPIOGetInPortState(ePOR_C, ePIN_10);

			/*低电平(0)代表闭合(用户根据实际修改)*/
			if(0 == state)
			{
				state = ePSTATE_OFF;
				//state = ePSTATE_ON;
			}
			else if(1 == state)
			{
				state = ePSTATE_ON;
				//state = ePSTATE_OFF;
			}
			break;

		/*从芯片KEA128心跳信号回检端口*/
		case ePIn_128EWM:
			state = BSPGPIOGetInPortState(ePOR_A, ePIN_14);

			/*低电平(0)代表闭合(用户根据实际修改)*/
			if(0 == state)
			{
				state = ePSTATE_OFF;
				//state = ePSTATE_ON;
			}
			else if(1 == state)
			{
				state = ePSTATE_ON;
				//state = ePSTATE_OFF;
			}
			break;

		/*电流霍尔电源过载故障状态,复用管脚(故障时输入低电平)*/
		case ePIn_HALFAU:
			//state = BSPGPIOGetInPortState(ePOR_C, ePIN_10);
            //触发获取复用管脚输入状态
            #if(0 == IO_MUL_CTRL_TYPE)
            //获取复用管脚当前所有管脚状态
            sDevMulPortInputState = GPIOMulPortReadAllDryState();
            #endif
            state = BitGet(sDevMulPortInputState, ePMul_D0);

            //低电平(0)代表故障(用户根据实际修改)
            if(0 == state)
            {
                state = ePSTATE_OFF;
                //state = ePSTATE_ON;
            }
            else if(1 == state)
            {
                state = ePSTATE_ON;
                //state = ePSTATE_OFF;
            }
			break;

		/*K1-低边开关控制回检状态检测,复用管脚((闭合故障(短路)时为低电平,闭合正常/断开均为高)*/
		case ePIn_LK1_Sta:
			//state = BSPGPIOGetInPortState(ePOR_C, ePIN_10);
            //触发获取复用管脚输入状态
            #if(0 == IO_MUL_CTRL_TYPE)
            //获取复用管脚当前所有管脚状态
            sDevMulPortInputState = GPIOMulPortReadAllDryState();
            #endif
            state = BitGet(sDevMulPortInputState, ePMul_D1);

            //低电平(0)代表闭合(用户根据实际修改)
            if(0 == state)
            {
                state = ePSTATE_OFF;
                //state = ePSTATE_ON;
            }
            else if(1 == state)
            {
                state = ePSTATE_ON;
                //state = ePSTATE_OFF;
            }
			break;

		/*K3-低边开关控制回检状态检测,复用管脚(闭合故障(短路)时为低电平,闭合正常/断开均为高)*/
		case ePIn_LK3_Sta:
			//state = BSPGPIOGetInPortState(ePOR_C, ePIN_10);
            //触发获取复用管脚输入状态
            #if(0 == IO_MUL_CTRL_TYPE)
            //获取复用管脚当前所有管脚状态
            sDevMulPortInputState = GPIOMulPortReadAllDryState();
            #endif
            state = BitGet(sDevMulPortInputState, ePMul_D2);

            //低电平(0)代表闭合(用户根据实际修改)
            if(0 == state)
            {
                state = ePSTATE_OFF;
                //state = ePSTATE_ON;
            }
            else if(1 == state)
            {
                state = ePSTATE_ON;
                //state = ePSTATE_OFF;
            }
			break;

		/*K9高边开关控制回检状态检测,复用管脚(闭合故障(短路/开路)时为低电平,闭合正常/断开均为高)*/
		case ePIn_K9_Sta:
			//state = BSPGPIOGetInPortState(ePOR_C, ePIN_10);
            //触发获取复用管脚输入状态
            #if(0 == IO_MUL_CTRL_TYPE)
            //获取复用管脚当前所有管脚状态
            sDevMulPortInputState = GPIOMulPortReadAllDryState();
            #endif
            state = BitGet(sDevMulPortInputState, ePMul_D3);

            //低电平(0)代表闭合(用户根据实际修改)
            if(0 == state)
            {
                state = ePSTATE_OFF;
                //state = ePSTATE_ON;
            }
            else if(1 == state)
            {
                state = ePSTATE_ON;
                //state = ePSTATE_OFF;
            }
			break;

        default:
            state = 0xff;
            break;
    }
    
    return(state);
}

/*=============================================================================================
** @Name      : u8 DEVGPIOSetOutputState(e_OutputPort port, e_GPIOState state)
** @Input     : port:输出端口号 state:输出状态[0:断开 1:闭合]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 设置GPIO输出端口状态
** @The notes :
**===========================================================================================*/
u8 DEVGPIOSetPortState(e_OutputPort port, e_GPIOState state)
{
    u8 result = TRUE;
    e_IOState ioSign = eIOLOW;
    
    switch(port)
    {
        /*板上LED*/
        case ePOut_LED:
            /*低电平(0)代表闭合(用户根据实际修改)*/
            if(ePSTATE_ON == state)
            {
                ioSign = eIOLOW;
            }
            else if(ePSTATE_OFF == state)
            {
                ioSign = eIOHIG;
            }
            else
            {
                return(FALSE);
            }
            
            result = BSPGPIOSetPortState(ePOR_A, ePIN_11, ioSign);
            break;
            
        /*自身电源保持使能*/
        case ePOut_PwHold:
            /*低电平(0)代表闭合(用户根据实际修改)*/
            if(ePSTATE_ON == state)
            {
                ioSign = eIOHIG;
            }
            else if(ePSTATE_OFF == state)
            {
                ioSign = eIOLOW;
            }
            else
            {
                return(FALSE);
            }
            
            result = BSPGPIOSetPortState(ePOR_B, ePIN_9, ioSign);
            break;

        /*485-1接收使能*/
        case ePOut_485EN0:
            /*控制状态与电平一致*/
            ioSign = (e_IOState)state;
            
            result = BSPGPIOSetPortState(ePOR_B, ePIN_4, ioSign);
            break;
            
        /*485-2接收使能*/
        case ePOut_485EN1:
            //控制状态与电平一致
            ioSign = (e_IOState)state;
            
            result = BSPGPIOSetPortState(ePOR_E, ePIN_9, ioSign);
            break;
            
        /*485-3接收使能*/
        case ePOut_485EN2:
            /*控制状态与电平一致*/
            ioSign = (e_IOState)state;
            
            result = BSPGPIOSetPortState(ePOR_D, ePIN_11, ioSign);
            break;
            
        /*外部EEP写保护端口*/
        case ePOut_EepWPr:
            /*控制状态与电平一致*/
            ioSign = (e_IOState)state;
            
            result = BSPGPIOSetPortState(ePOR_B, ePIN_11, ioSign);
            break;
            
        /*LTC2949片选*/
        case ePOut_LTC2949CS:
            /*控制状态与电平一致*/
            ioSign = (e_IOState)state;

            result = BSPGPIOSetPortState(ePOR_B, ePIN_17, ioSign);
            break;

        /*外部FLASH写保护端口*/
        case ePOut_FlaWPr:
            /*控制状态与电平一致*/
            ioSign = (e_IOState)state;
            
            result = BSPGPIOSetPortState(ePOR_E, ePIN_11, ioSign);
            break;
            
		/*外部FLASH SPI1片选*/
		case ePOut_FlaCS:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			result = BSPGPIOSetPortState(ePOR_E, ePIN_16, ioSign);
			break;

		/*菊花链LTC6820_1片选*/
		case ePOut_LTC6820_CS1:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			result = BSPGPIOSetPortState(ePOR_C, ePIN_14, ioSign);
			break;

		/*菊花链LTC6820_2片选*/
		case ePOut_LTC6820_CS2:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			result = BSPGPIOSetPortState(ePOR_E, ePIN_13, ioSign);
			break;

		/*管脚复用功能译码器片选(低有效)*/
		case ePOut_DEC_SIO:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			result = BSPGPIOSetPortState(ePOR_D, ePIN_8, ioSign);
			break;

		/*管脚复用功能译码器编码1*/
		case ePOut_DEC_A0:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			result = BSPGPIOSetPortState(ePOR_C, ePIN_13, ioSign);
			break;

		/*管脚复用功能译码器编码2*/
		case ePOut_DEC_A1:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			result = BSPGPIOSetPortState(ePOR_C, ePIN_12, ioSign);
			break;

		/*管脚复用功能译码器编码3	*/
		case ePOut_DEC_A2:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			result = BSPGPIOSetPortState(ePOR_C, ePIN_11, ioSign);
			break;

		/*模拟IIC时钟线*/
		case ePOut_SimIIC_SCL:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;
			result = BSPGPIOSetPortState(ePOR_A, ePIN_3, ioSign);
			break;

		/*模拟IIC数据线*/
		case ePOut_SimIIC_SDA:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;
			result = BSPGPIOSetPortState(ePOR_A, ePIN_2, ioSign);
			break;

		/*FS6503片选信号*/
		case ePOut_SBCFS65_CS:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;
			result = BSPGPIOSetPortState(ePOR_E, ePIN_6, ioSign);
			break;

		/*主MCU到从KEA128芯片的心跳信号*/
		case ePOut_128EWM_OUT:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;
			result = BSPGPIOSetPortState(ePOR_A, ePIN_17, ioSign);
			break;

		/*主MCU到从KEA128芯片的SPI片选信号*/
		case ePOut_128SPI_CS:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;
			result = BSPGPIOSetPortState(ePOR_E, ePIN_10, ioSign);
			break;

		/*高边开关1,复用管脚*/
		case ePOut_DRVK0:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_E, ePIN_14, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhCtrlState, ePMul_D0);
            }
            else
            {
                BitClr(sDevMulPortSwhCtrlState, ePMul_D0);
            }

            //触发控制复用管脚输出状态
            #if(0 == IO_MUL_CTRL_TYPE)
            GPIOMulPortWriteAllSwhState();
            #endif
			break;

		/*高边开关2,复用管脚*/
		case ePOut_DRVK1:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_E, ePIN_12, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhCtrlState, ePMul_D1);
            }
            else
            {
                BitClr(sDevMulPortSwhCtrlState, ePMul_D1);
            }

            //触发控制复用管脚输出状态
            #if(0 == IO_MUL_CTRL_TYPE)
            GPIOMulPortWriteAllSwhState();
            #endif
			break;

		/*高边开关3,复用管脚*/
		case ePOut_DRVK2:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_8, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhCtrlState, ePMul_D2);
            }
            else
            {
                BitClr(sDevMulPortSwhCtrlState, ePMul_D2);
            }

            //触发控制复用管脚输出状态
            #if(0 == IO_MUL_CTRL_TYPE)
            GPIOMulPortWriteAllSwhState();
            #endif
			break;

		/*高边开关4,复用管脚*/
		case ePOut_DRVK3:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_11, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhCtrlState, ePMul_D3);
            }
            else
            {
                BitClr(sDevMulPortSwhCtrlState, ePMul_D3);
            }

            //触发控制复用管脚输出状态
            #if(0 == IO_MUL_CTRL_TYPE)
            GPIOMulPortWriteAllSwhState();
            #endif
			break;

		/*高边开关5,复用管脚*/
		case ePOut_DRVK4:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_11, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhCtrlState, ePMul_D4);
            }
            else
            {
                BitClr(sDevMulPortSwhCtrlState, ePMul_D4);
            }

            //触发控制复用管脚输出状态
            #if(0 == IO_MUL_CTRL_TYPE)
            GPIOMulPortWriteAllSwhState();
            #endif
			break;

		/*高边开关6,复用管脚*/
		case ePOut_DRVK5:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_11, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhCtrlState, ePMul_D5);
            }
            else
            {
                BitClr(sDevMulPortSwhCtrlState, ePMul_D5);
            }

            //触发控制复用管脚输出状态
            #if(0 == IO_MUL_CTRL_TYPE)
            GPIOMulPortWriteAllSwhState();
            #endif
			break;

		/*高边开关7,复用管脚*/
		case ePOut_DRVK6:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_11, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhCtrlState, ePMul_D6);
            }
            else
            {
                BitClr(sDevMulPortSwhCtrlState, ePMul_D6);
            }

            //触发控制复用管脚输出状态
            #if(0 == IO_MUL_CTRL_TYPE)
            GPIOMulPortWriteAllSwhState();
            #endif
			break;

		/*高边开关8,复用管脚*/
		case ePOut_DRVK7:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_11, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhCtrlState, ePMul_D7);
            }
            else
            {
                BitClr(sDevMulPortSwhCtrlState, ePMul_D7);
            }

            //触发控制复用管脚输出状态
            #if(0 == IO_MUL_CTRL_TYPE)
            GPIOMulPortWriteAllSwhState();
            #endif
			break;

		/*高边开关1\2使能管脚,复用管脚*/
		case ePOut_DRVK1K2EN:
			//控制状态与电平一致
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_E, ePIN_3, ioSign);
            if(eIOHIG == ioSign)
            {
                //只能有1路使能
            	sDevMulPortSwhEnCtrlState &= 0xf0;
                BitSet(sDevMulPortSwhEnCtrlState, ePMul_D0);
            }
            else
            {
                BitClr(sDevMulPortSwhEnCtrlState, ePMul_D0);
            }
			break;

		/*高边开关3\4使能管脚,复用管脚*/
		case ePOut_DRVK3K4EN:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_9, ioSign);
            if(eIOHIG == ioSign)
            {
                //只能有1路使能
            	sDevMulPortSwhEnCtrlState &= 0xf0;
                BitSet(sDevMulPortSwhEnCtrlState, ePMul_D1);
            }
            else
            {
                BitClr(sDevMulPortSwhEnCtrlState, ePMul_D1);
            }
			break;

		/*高边开关5\6使能管脚,复用管脚*/
		case ePOut_DRVK5K6EN:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_9, ioSign);
            if(eIOHIG == ioSign)
            {
                //只能有1路使能
            	sDevMulPortSwhEnCtrlState &= 0xf0;
                BitSet(sDevMulPortSwhEnCtrlState, ePMul_D2);
            }
            else
            {
                BitClr(sDevMulPortSwhEnCtrlState, ePMul_D2);
            }
			break;

		/*高边开关7\8使能管脚,复用管脚*/
		case ePOut_DRVK7K8EN:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_9, ioSign);
            if(eIOHIG == ioSign)
            {
                //只能有1路使能
            	sDevMulPortSwhEnCtrlState &= 0xf0;
                BitSet(sDevMulPortSwhEnCtrlState, ePMul_D3);
            }
            else
            {
                BitClr(sDevMulPortSwhEnCtrlState, ePMul_D3);
            }
			break;

		/*低边开关1,复用管脚*/
		case ePOut_LDRVK0:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_9, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhEnCtrlState, ePMul_D4);
            }
            else
            {
                BitClr(sDevMulPortSwhEnCtrlState, ePMul_D4);
            }
			break;

		/*低边开关2,复用管脚*/
		case ePOut_LDRVK2:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_9, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhEnCtrlState, ePMul_D5);
            }
            else
            {
                BitClr(sDevMulPortSwhEnCtrlState, ePMul_D5);
            }
			break;

		/*高边开关9,复用管脚*/
		case ePOut_DRVK8:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_11, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhEnCtrlState, ePMul_D6);
            }
            else
            {
                BitClr(sDevMulPortSwhEnCtrlState, ePMul_D6);
            }
			break;

		/*高边开关1\2\3\4\5\6\7\8回采电流选通,复用管脚*/
		case ePOut_DRVDSELALL:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_C, ePIN_8, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortSwhEnCtrlState, ePMul_D7);
            }
            else
            {
                BitClr(sDevMulPortSwhEnCtrlState, ePMul_D7);
            }
			break;

		/*系统状态输出干接点1(继电器无源开闭信号),复用管脚*/
		case ePOut_STADRY0:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_D, ePIN_12, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortDryCtrlState, ePMul_D0);
            }
            else
            {
                BitClr(sDevMulPortDryCtrlState, ePMul_D0);
            }
			break;

		/*充电允许输出干接点2(继电器无源开闭信号),复用管脚*/
		case ePOut_CHGDRY1:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_D, ePIN_11, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortDryCtrlState, ePMul_D1);
            }
            else
            {
                BitClr(sDevMulPortDryCtrlState, ePMul_D1);
            }
			break;

		/*放电允许输出干接点3(继电器无源开闭信号),复用管脚*/
		case ePOut_DHGDRY2:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_D, ePIN_10, ioSign);
            if(eIOHIG == ioSign)
            {
                BitSet(sDevMulPortDryCtrlState, ePMul_D2);
            }
            else
            {
                BitClr(sDevMulPortDryCtrlState, ePMul_D2);
            }
			break;

		/*绝缘检测低压侧PGND控制信号,复用管脚*/
		case ePOut_ISO_PGND:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_B, ePIN_10, ioSign);
			if(eIOHIG == ioSign)
			{
				BitSet(sDevMulPortDryCtrlState, ePMul_D3);
			}
			else
			{
				BitClr(sDevMulPortDryCtrlState, ePMul_D3);
			}
			break;

		/*菊花链LTC6820_1使能管脚*/
		case ePOut_LTC6820EN1:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_B, ePIN_10, ioSign);
			if(eIOHIG == ioSign)
			{
				BitSet(sDevMulPortDryCtrlState, ePMul_D4);
			}
			else
			{
				BitClr(sDevMulPortDryCtrlState, ePMul_D4);
			}
			break;

		/*菊花链LTC6820_2使能管脚*/
		case ePOut_LTC6820EN2:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_B, ePIN_10, ioSign);
			if(eIOHIG == ioSign)
			{
				BitSet(sDevMulPortDryCtrlState, ePMul_D5);
			}
			else
			{
				BitClr(sDevMulPortDryCtrlState, ePMul_D5);
			}
			break;

		/*低压电源第2路控制输出*/
		case ePOut_K4Second:
			/*控制状态与电平一致*/
			ioSign = (e_IOState)state;

			//result = BSPGPIOSetPortState(ePOR_B, ePIN_10, ioSign);
			if(eIOHIG == ioSign)
			{
				BitSet(sDevMulPortDryCtrlState, ePMul_D6);
			}
			else
			{
				BitClr(sDevMulPortDryCtrlState, ePMul_D6);
			}
			break;

        default:
            result = FALSE;
            break;
    }
    
    return(result);
}

/*=============================================================================================
** @Name      : u8 DEVGPIOGetOutPortState(e_OutputPort port)
** @Input     : port:输出端口号
** @Output    : 端口状态[0:断开 1:闭合]
** @Function  : 获取GPIO输出端口状态
** @The notes :
**===========================================================================================*/
u8 DEVGPIOGetOutPortState(e_OutputPort port)
{
    u8 state = 0;
    
    switch(port)
    {
        /*板上LED端口*/
        case ePOut_LED:
            /*低电平(0)代表闭合(用户根据实际修改)*/
            if(0 == BSPGPIOGetOutPortState(ePOR_A, ePIN_11))
            {
                state = ePSTATE_ON;
            }
            else
            {
                state = ePSTATE_OFF;
            }
            break;
            
        /*自身电源保持使能*/
        case ePOut_PwHold:
            /*低电平(0)代表闭合(用户根据实际修改)*/
            if(0 == BSPGPIOGetOutPortState(ePOR_B, ePIN_9))
            {
                state = ePSTATE_OFF;
            }
            else
            {
                state = ePSTATE_ON;
            }
            break;

        /*485-1接收使能*/
        case ePOut_485EN0:
            state = BSPGPIOGetOutPortState(ePOR_B, ePIN_4);
            break;
            
        /*485-2接收使能*/
        case ePOut_485EN1:
            state = BSPGPIOGetOutPortState(ePOR_E, ePIN_9);
            break;
            
        /*485-3接收使能*/
        case ePOut_485EN2:
            state = BSPGPIOGetOutPortState(ePOR_D, ePIN_11);
            break;
            
        /*外部EEP写保护端口*/
        case ePOut_EepWPr:
            state = BSPGPIOGetOutPortState(ePOR_B, ePIN_11);
            break;
            
		/*LTC2949片选*/
		case ePOut_LTC2949CS:
			state = BSPGPIOGetOutPortState(ePOR_B, ePIN_17);
			break;

        /*外部FLASH写保护端口*/
        case ePOut_FlaWPr:
            state = BSPGPIOGetOutPortState(ePOR_E, ePIN_11);
            break;
            
        /*外部FLASH SPI1片选CS*/
        case ePOut_FlaCS:
            state = BSPGPIOGetOutPortState(ePOR_E, ePIN_16);
            break;

		/*菊花链LTC6820_1片选*/
		case ePOut_LTC6820_CS1:
			state = BSPGPIOGetOutPortState(ePOR_C, ePIN_14);
			break;

		/*菊花链LTC6820_2片选*/
		case ePOut_LTC6820_CS2:
			state = BSPGPIOGetOutPortState(ePOR_E, ePIN_13);
			break;

		/*管脚复用功能译码器片选(低有效)*/
		case ePOut_DEC_SIO:
			state = BSPGPIOGetOutPortState(ePOR_D, ePIN_8);
			break;

		/*管脚复用功能译码器编码1*/
		case ePOut_DEC_A0:
			state = BSPGPIOGetOutPortState(ePOR_C, ePIN_13);
			break;

		/*管脚复用功能译码器编码2*/
		case ePOut_DEC_A1:
			state = BSPGPIOGetOutPortState(ePOR_C, ePIN_12);
			break;

		/*管脚复用功能译码器编码3	*/
		case ePOut_DEC_A2:
			state = BSPGPIOGetOutPortState(ePOR_C, ePIN_11);
			break;

		/*模拟IIC时钟线*/
		case ePOut_SimIIC_SCL:
			state = BSPGPIOGetOutPortState(ePOR_A, ePIN_3);
			break;

		/*模拟IIC数据线*/
		case ePOut_SimIIC_SDA:
			state = BSPGPIOGetOutPortState(ePOR_A, ePIN_2);
			break;

		/*主MCU到从KEA128芯片的心跳信号*/
		case ePOut_128EWM_OUT:
			state = BSPGPIOGetOutPortState(ePOR_A, ePIN_17);
			break;

		/*高边开关1,复用管脚*/
		case ePOut_DRVK0:
			//state = BSPGPIOGetOutPortState(ePOR_E, ePIN_14, ioSign);
            state = BitGet(sDevMulPortSwhCtrlState, ePMul_D0);
			break;

		/*高边开关2,复用管脚*/
		case ePOut_DRVK1:
			//state = BSPGPIOGetOutPortState(ePOR_E, ePIN_12, ioSign);
            state = BitGet(sDevMulPortSwhCtrlState, ePMul_D1);
			break;

		/*高边开关3,复用管脚*/
		case ePOut_DRVK2:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_8, ioSign);
			state = BitGet(sDevMulPortSwhCtrlState, ePMul_D2);
			break;

		/*高边开关4,复用管脚*/
		case ePOut_DRVK3:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_11, ioSign);
			state = BitGet(sDevMulPortSwhCtrlState, ePMul_D3);
			break;

		/*高边开关5,复用管脚*/
		case ePOut_DRVK4:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_11, ioSign);
			state = BitGet(sDevMulPortSwhCtrlState, ePMul_D4);
			break;

		/*高边开关6,复用管脚*/
		case ePOut_DRVK5:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_11, ioSign);
			state = BitGet(sDevMulPortSwhCtrlState, ePMul_D5);
			break;

		/*高边开关7,复用管脚*/
		case ePOut_DRVK6:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_11, ioSign);
			state = BitGet(sDevMulPortSwhCtrlState, ePMul_D6);
			break;

		/*高边开关8,复用管脚*/
		case ePOut_DRVK7:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_11, ioSign);
			state = BitGet(sDevMulPortSwhCtrlState, ePMul_D7);
			break;

		/*高边开关1\2使能管脚,复用管脚*/
		case ePOut_DRVK1K2EN:
			//state = BSPGPIOGetOutPortState(ePOR_E, ePIN_3, ioSign);
			state = BitGet(sDevMulPortSwhEnCtrlState, ePMul_D0);
			break;

		/*高边开关3\4使能管脚,复用管脚*/
		case ePOut_DRVK3K4EN:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_9, ioSign);
			state = BitGet(sDevMulPortSwhEnCtrlState, ePMul_D1);
			break;

		/*高边开关5\6使能管脚,复用管脚*/
		case ePOut_DRVK5K6EN:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_9, ioSign);
			state = BitGet(sDevMulPortSwhEnCtrlState, ePMul_D2);
			break;

		/*高边开关7\8使能管脚,复用管脚*/
		case ePOut_DRVK7K8EN:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_9, ioSign);
			state = BitGet(sDevMulPortSwhEnCtrlState, ePMul_D3);
			break;

		/*低边开关1,复用管脚*/
		case ePOut_LDRVK0:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_9, ioSign);
			state = BitGet(sDevMulPortSwhEnCtrlState, ePMul_D4);
			break;

		/*低边开关2,复用管脚*/
		case ePOut_LDRVK2:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_9, ioSign);
			state = BitGet(sDevMulPortSwhEnCtrlState, ePMul_D5);
			break;

		/*高边开关9,复用管脚*/
		case ePOut_DRVK8:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_11, ioSign);
			state = BitGet(sDevMulPortSwhEnCtrlState, ePMul_D6);
			break;

		/*高边开关1\2\3\4\5\6\7\8回采电流选通,复用管脚*/
		case ePOut_DRVDSELALL:
			//state = BSPGPIOGetOutPortState(ePOR_C, ePIN_8, ioSign);
			state = BitGet(sDevMulPortSwhEnCtrlState, ePMul_D7);
			break;

		/*系统状态输出干接点1(继电器无源开闭信号),复用管脚*/
		case ePOut_STADRY0:
			//state = BSPGPIOGetOutPortState(ePOR_D, ePIN_12, ioSign);
			state = BitGet(sDevMulPortDryCtrlState, ePMul_D0);
			break;

		/*充电允许输出干接点2(继电器无源开闭信号),复用管脚*/
		case ePOut_CHGDRY1:
			//state = BSPGPIOGetOutPortState(ePOR_D, ePIN_11, ioSign);
			state = BitGet(sDevMulPortDryCtrlState, ePMul_D1);
			break;

		/*放电允许输出干接点3(继电器无源开闭信号),复用管脚*/
		case ePOut_DHGDRY2:
			//state = BSPGPIOGetOutPortState(ePOR_D, ePIN_10, ioSign);
			state = BitGet(sDevMulPortDryCtrlState, ePMul_D2);
			break;

		/*绝缘检测低压侧PGND控制信号,复用管脚*/
		case ePOut_ISO_PGND:
			//state = BSPGPIOGetOutPortState(ePOR_B, ePIN_10, ioSign);
			state = BitGet(sDevMulPortDryCtrlState, ePMul_D3);
			break;

		/*菊花链LTC6820_1使能管脚*/
		case ePOut_LTC6820EN1:
			state = BitGet(sDevMulPortDryCtrlState, ePMul_D4);
			break;

		/*菊花链LTC6820_2使能管脚*/
		case ePOut_LTC6820EN2:
			state = BitGet(sDevMulPortDryCtrlState, ePMul_D5);
			break;

		/*低压电源第2路控制输出*/
		case ePOut_K4Second:
			state = BitGet(sDevMulPortDryCtrlState, ePMul_D6);
			break;

        default:
            state = 0xff;
            break;
    }
    
    return(state);
}

/*=============================================================================================
** @Name      : u16 DEVGPIOGetInputDryState(void)
** @Input     : void
** @Output    : 按位表示各输入干接点端口状态[0:断开 1:闭合]
** @Function  : 获取所有输入干接点端口状态
** @The notes : 共6路
**===========================================================================================*/
u16 DEVGPIOGetInputDryState(void)
{
    u8 i = 0;
    u16 state = 0;
    
    for(i = ePIn_DRY0; i <= ePIn_DRY5; i++)
    {
        if(1 == DEVGPIOGetInPortState((e_INPUTPORT)i))
        {
            BitSet(state, (i - ePIn_DRY0));
        }
    }
    
    return(state);
}

/*=============================================================================================
** @Name      : u8 DEVGPIOGetHWVersion(void)
** @Input     : void
** @Output    : 按位表示硬件版本号
** @Function  : 获取硬件版本号
** @The notes : 共3路
**===========================================================================================*/
u8 DEVGPIOGetHWVersion(void)
{
    u8 i = 0;
    u8 state = 0;

    for(i = ePIn_HWVER0; i <= ePIn_HWVER2; i++)
    {
        if(1 == DEVGPIOGetInPortState((e_INPUTPORT)i))
        {
            BitSet(state, (i - ePIn_HWVER0));
        }
    }

    return(state);
}

/*=============================================================================================
** @Name      : u8 DEVGPIOGetSwhBackState(void)
** @Input     : void
** @Output    : 按位表示回检信号状态
** @Function  : 获取高低边开关回检信号状态
** @The notes : 共3路(2路低边1路高边K9)
**===========================================================================================*/
u8 DEVGPIOGetSwhBackState(void)
{
    u8 state = 0;

	if(1 == DEVGPIOGetInPortState(ePIn_LK1_Sta))
	{
		BitSet(state, 0);
	}

	if(1 == DEVGPIOGetInPortState(ePIn_LK3_Sta))
	{
		BitSet(state, 1);
	}

	if(1 == DEVGPIOGetInPortState(ePIn_K9_Sta))
	{
		BitSet(state, 2);
	}

    return(state);
}

/*=============================================================================================
** @Name      : u8 DEVGPIOGetOutputDryState(void)
** @Input     : void
** @Output    : 按位表示输出干接点当前状态[0:断开 1:闭合]
** @Function  : 获取输出干接点状态
** @The notes : 共3路
**===========================================================================================*/
u8 DEVGPIOGetOutputDryState(void)
{
    u8 i = 0;
    u8 state = 0;

    for(i = ePOut_STADRY0; i <= ePOut_DHGDRY2; i++)
    {
        if(1 == DEVGPIOGetOutPortState((e_OutputPort)i))
        {
            BitSet(state, (i - ePOut_STADRY0));
        }
    }

    return(state);
}

/*=============================================================================================
** @Name      : u16 DEVGPIOGetCtrlHSwitchState(void)
** @Input     : void
** @Output    : 按位表示高边开关当前控制状态[0:断开 1:闭合]
** @Function  : 获取控制接触器高边开关状态
** @The notes : 共9路
**===========================================================================================*/
u16 DEVGPIOGetCtrlHSwitchState(void)
{
    u8 i = 0;
    u16 state = 0;

    for(i = ePOut_DRVK0; i <= ePOut_DRVK8; i++)
    {
        if(1 == DEVGPIOGetOutPortState((e_OutputPort)i))
        {
            BitSet(state, (i - ePOut_DRVK0));
        }
    }

    return(state);
}

/*=============================================================================================
** @Name      : u16 DEVGPIOGetCtrlLSwitchState(void)
** @Input     : void
** @Output    : 按位表示低边开关当前控制状态[0:断开 1:闭合]
** @Function  : 获取控制接触器低边开关状态
** @The notes : 共2路
**===========================================================================================*/
u16 DEVGPIOGetCtrlLSwitchState(void)
{
    u8 i = 0;
    u8 state = 0;

    for(i = ePOut_LDRVK0; i <= ePOut_LDRVK2; i++)
    {
        if(1 == DEVGPIOGetOutPortState((e_OutputPort)i))
        {
            BitSet(state, (i - ePOut_LDRVK0));
        }
    }

    return(state);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u8 GPIOMulPinDirInit(u8 dir)
** @Input     : dir:初始化方向 0输入 1输出
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 输入输出复用IO管脚方向初始化
** @The notes : 复用管脚PD8~PD15
**===========================================================================================*/
static u8 GPIOMulPinDirInit(u8 dir)
{
    u8 result = TRUE;

    /*GPIO输入端口初始化*/
    if(0 == dir)
    {
        result = BSPGPIOInit(ePOR_E, ePIN_14, ePORT_MUX_AS_GPIO, 0);    /*输入D0*/
        result &= BSPGPIOInit(ePOR_E, ePIN_3, ePORT_MUX_AS_GPIO, 0);    /*输入D1*/
        result &= BSPGPIOInit(ePOR_E, ePIN_12, ePORT_MUX_AS_GPIO, 0);   /*输入D2*/
        result &= BSPGPIOInit(ePOR_D, ePIN_17, ePORT_MUX_AS_GPIO, 0);   /*输入D3*/
        result &= BSPGPIOInit(ePOR_D, ePIN_16, ePORT_MUX_AS_GPIO, 0);   /*输入D4*/
        result &= BSPGPIOInit(ePOR_D, ePIN_15, ePORT_MUX_AS_GPIO, 0);   /*输入D5*/
        result &= BSPGPIOInit(ePOR_B, ePIN_5, ePORT_MUX_AS_GPIO, 0);    /*输入D6*/
        result &= BSPGPIOInit(ePOR_D, ePIN_12, ePORT_MUX_AS_GPIO,0);    /*输入D7*/
        sDevMulPortInitDirect = 1;/*初始化为输入*/
    }
    /*GPIO输出端口初始化*/
    else
    {
        result = BSPGPIOInit(ePOR_E, ePIN_14, ePORT_MUX_AS_GPIO, 1);    /*输出D0*/
        result &= BSPGPIOInit(ePOR_E, ePIN_3, ePORT_MUX_AS_GPIO, 1);    /*输出D1*/
        result &= BSPGPIOInit(ePOR_E, ePIN_12, ePORT_MUX_AS_GPIO, 1);   /*输出D2*/
        result &= BSPGPIOInit(ePOR_D, ePIN_17, ePORT_MUX_AS_GPIO, 1);   /*输出D3*/
        result &= BSPGPIOInit(ePOR_D, ePIN_16, ePORT_MUX_AS_GPIO, 1);   /*输出D4*/
        result &= BSPGPIOInit(ePOR_D, ePIN_15, ePORT_MUX_AS_GPIO, 1);   /*输出D5*/
        result &= BSPGPIOInit(ePOR_B, ePIN_5, ePORT_MUX_AS_GPIO, 1);    /*输出D6*/
        result &= BSPGPIOInit(ePOR_D, ePIN_12, ePORT_MUX_AS_GPIO, 1);   /*输出D7*/
        sDevMulPortInitDirect = 2;/*初始化为输出*/
    }

    return(result);
}

/*=============================================================================================
** @Name      : static u8 GPIOMulPinSetAllOutputState(u8 state)
** @Input     : state:按位表示各复用IO输出状态 0低电平 1高电平
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 设置输入输出复用IO管脚输出状态
** @The notes : 复用管脚PD8~PD15
**===========================================================================================*/
static u8 GPIOMulPinSetAllOutputState(u8 state)
{
    u8 result = TRUE;

    //D0输出
    if(1 == BitGet(state, 0))
    {
        result = BSPGPIOSetPortState(ePOR_E, ePIN_14, (e_IOState)1);
    }
    else
    {
        result = BSPGPIOSetPortState(ePOR_E, ePIN_14, (e_IOState)0);
    }

    //D1输出
    if(1 == BitGet(state, 1))
    {
        result &= BSPGPIOSetPortState(ePOR_E, ePIN_3, (e_IOState)1);
    }
    else
    {
        result &= BSPGPIOSetPortState(ePOR_E, ePIN_3, (e_IOState)0);
    }

    //D2输出
    if(1 == BitGet(state, 2))
    {
        result &= BSPGPIOSetPortState(ePOR_E, ePIN_12, (e_IOState)1);
    }
    else
    {
        result &= BSPGPIOSetPortState(ePOR_E, ePIN_12, (e_IOState)0);
    }

    //D3输出
    if(1 == BitGet(state, 3))
    {
        result &= BSPGPIOSetPortState(ePOR_D, ePIN_17, (e_IOState)1);
    }
    else
    {
        result &= BSPGPIOSetPortState(ePOR_D, ePIN_17, (e_IOState)0);
    }

    //D4输出
    if(1 == BitGet(state, 4))
    {
        result &= BSPGPIOSetPortState(ePOR_D, ePIN_16, (e_IOState)1);
    }
    else
    {
        result &= BSPGPIOSetPortState(ePOR_D, ePIN_16, (e_IOState)0);
    }

    //D5输出
    if(1 == BitGet(state, 5))
    {
        result &= BSPGPIOSetPortState(ePOR_D, ePIN_15, (e_IOState)1);
    }
    else
    {
        result &= BSPGPIOSetPortState(ePOR_D, ePIN_15, (e_IOState)0);
    }

    //D6输出
    if(1 == BitGet(state, 6))
    {
        result &= BSPGPIOSetPortState(ePOR_B, ePIN_5, (e_IOState)1);
    }
    else
    {
        result &= BSPGPIOSetPortState(ePOR_B, ePIN_5, (e_IOState)0);
    }

    //D7输出
    if(1 == BitGet(state, 7))
    {
        result &= BSPGPIOSetPortState(ePOR_D, ePIN_12, (e_IOState)1);
    }
    else
    {
        result &= BSPGPIOSetPortState(ePOR_D, ePIN_12, (e_IOState)0);
    }

    return(result);
}

/*=============================================================================================
** @Name      : static u8 GPIOMulPinGetAllPortState(void)
** @Input     : void
** @Output    : 按位表示各复用IO管脚当前端口状态 0低电平 1高电平
** @Function  : 获取输入输出复用IO管脚当前端口状态
** @The notes : 复用管脚PD8~PD15
**===========================================================================================*/
static u8 GPIOMulPinGetAllPortState(void)
{
    u8 state = 0;

    //D0状态
    if(1 == sDevMulPortInitDirect)//初始化为输入
    {
        if(1 == BSPGPIOGetInPortState(ePOR_E, ePIN_14))
        {
            BitSet(state, 0);
        }
    }
    else
    {
        if(1 == BSPGPIOGetOutPortState(ePOR_E, ePIN_14))
        {
            BitSet(state, 0);
        }
    }

    //D1状态
    if(1 == sDevMulPortInitDirect)//初始化为输入
    {
        if(1 == BSPGPIOGetInPortState(ePOR_E, ePIN_3))
        {
            BitSet(state, 1);
        }
    }
    else
    {
        if(1 == BSPGPIOGetOutPortState(ePOR_E, ePIN_3))
        {
            BitSet(state, 1);
        }
    }

    //D2状态
    if(1 == sDevMulPortInitDirect)//初始化为输入
    {
        if(1 == BSPGPIOGetInPortState(ePOR_E, ePIN_12))
        {
            BitSet(state, 2);
        }
    }
    else
    {
        if(1 == BSPGPIOGetOutPortState(ePOR_E, ePIN_12))
        {
            BitSet(state, 2);
        }
    }

    //D3状态
    if(1 == sDevMulPortInitDirect)//初始化为输入
    {
        if(1 == BSPGPIOGetInPortState(ePOR_D, ePIN_17))
        {
            BitSet(state, 3);
        }
    }
    else
    {
        if(1 == BSPGPIOGetOutPortState(ePOR_D, ePIN_17))
        {
            BitSet(state, 3);
        }
    }

    //D4状态
    if(1 == sDevMulPortInitDirect)//初始化为输入
    {
        if(1 == BSPGPIOGetInPortState(ePOR_D, ePIN_16))
        {
            BitSet(state, 4);
        }
    }
    else
    {
        if(1 == BSPGPIOGetOutPortState(ePOR_D, ePIN_16))
        {
            BitSet(state, 4);
        }
    }

    //D5状态
    if(1 == sDevMulPortInitDirect)//初始化为输入
    {
        if(1 == BSPGPIOGetInPortState(ePOR_D, ePIN_15))
        {
            BitSet(state, 5);
        }
    }
    else
    {
        if(1 == BSPGPIOGetOutPortState(ePOR_D, ePIN_15))
        {
            BitSet(state, 5);
        }
    }

    //D6状态
    if(1 == sDevMulPortInitDirect)//初始化为输入
    {
        if(1 == BSPGPIOGetInPortState(ePOR_B, ePIN_5))
        {
            BitSet(state, 6);
        }
    }
    else
    {
        if(1 == BSPGPIOGetOutPortState(ePOR_B, ePIN_5))
        {
            BitSet(state, 6);
        }
    }

    //D7状态
    if(1 == sDevMulPortInitDirect)//初始化为输入
    {
        if(1 == BSPGPIOGetInPortState(ePOR_D, ePIN_12))
        {
            BitSet(state, 7);
        }
    }
    else
    {
        if(1 == BSPGPIOGetOutPortState(ePOR_D, ePIN_12))
        {
            BitSet(state, 7);
        }
    }

    return(state);
}

/*=============================================================================================
** @Name      : static void GPIOMulPortWriteAllSwhState(void)
** @Input     : void
** @Output    : void
** @Function  : 写入所有输入输出复用IO口接触器输出状态
** @The notes : 包括译码器设置,使能HS1-8控制
**===========================================================================================*/
static void GPIOMulPortWriteAllSwhState(void)
{
    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOHIG);      /*管脚复用功能译码器片选高阻态(低脉冲有效)*/

    (void)GPIOMulPinDirInit(1);                             /*复用管脚初始化为输出*/
    (void)GPIOMulPinSetAllOutputState(sDevMulPortSwhCtrlState);/*复用管脚设置接触器控制状态*/

    //编码:A0=1,A1=0,A2=0:/CS_HS=0选通有效
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_13, eIOHIG);     /*设置管脚复用功能译码器编码A0*/
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_12, eIOLOW);     /*设置管脚复用功能译码器编码A1*/
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_11, eIOLOW);     /*设置管脚复用功能译码器编码A2*/

    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOLOW);      /*管脚复用功能译码器片选拉低脉冲(低脉冲有效)*/
    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOHIG);      /*管脚复用功能译码器片选恢复高阻态(低脉冲有效)*/
}

/*=============================================================================================
** @Name      : static void GPIOMulPortWriteAllDrvEnState(void)
** @Input     : void
** @Output    : void
** @Function  : 写入所有输入输出复用IO口回检诊断使能和低边开关输出状态
** @The notes : 包括译码器设置,使能HS1-4电流回检/LS1-2控制/HS9控制/HS电流通道片选
**===========================================================================================*/
static void GPIOMulPortWriteAllDrvEnState(void)
{
    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOHIG);      /*管脚复用功能译码器片选高阻态(低脉冲有效)*/

    (void)GPIOMulPinDirInit(1);                             /*复用管脚初始化为输出*/
    (void)GPIOMulPinSetAllOutputState(sDevMulPortSwhEnCtrlState);//复用管脚设置接触器控制状态*/

    //编码:A0=0,A1=1,A2=0:/CS_HS_LS=0选通有效
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_13, eIOLOW);     /*设置管脚复用功能译码器编码A0*/
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_12, eIOHIG);     /*设置管脚复用功能译码器编码A1*/
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_11, eIOLOW);     /*设置管脚复用功能译码器编码A2*/

    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOLOW);      /*管脚复用功能译码器片选拉低脉冲(低脉冲有效)*/
    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOHIG);      /*管脚复用功能译码器片选恢复高阻态(低脉冲有效)*/
}

/*=============================================================================================
** @Name      : static void GPIOMulPortWriteAllDryState(void)
** @Input     : void
** @Output    : void
** @Function  : 写入所有输入输出复用IO口干接点输出状态和绝缘检测低边开关输出状态
** @The notes : 包括译码器设置,使能干接点1-3/PE控制/菊花链1-2/低压第2路控制
**===========================================================================================*/
static void GPIOMulPortWriteAllDryState(void)
{
    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOHIG);      /*管脚复用功能译码器片选高阻态(低脉冲有效)*/

    (void)GPIOMulPinDirInit(1);                             /*复用管脚初始化为输出*/
    (void)GPIOMulPinSetAllOutputState(sDevMulPortDryCtrlState);/*复用管脚设置接触器控制状态*/

    //编码:A0=0,A1=0,A2=1:/CSDO_ISO=0选通有效
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_13, eIOLOW);     /*设置管脚复用功能译码器编码A0*/
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_12, eIOLOW);     /*设置管脚复用功能译码器编码A1*/
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_11, eIOHIG);     /*设置管脚复用功能译码器编码A2*/

    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOLOW);      /*管脚复用功能译码器片选拉低脉冲(低脉冲有效)*/
    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOHIG);      /*管脚复用功能译码器片选恢复高阻态(低脉冲有效)*/
}

/*=============================================================================================
** @Name      : static u8 GPIOMulPortReadAllDryState(void)
** @Input     : void
** @Output    : 按位表示各复用IO管脚输入干接点[0,5]状态 0开 1闭
** @Function  : 读取所有输入输出复用IO口干接点输入状态
** @The notes : 包括译码器设置,使能霍尔回检/LS1-2回检/HS9回检状态
**===========================================================================================*/
static u8 GPIOMulPortReadAllDryState(void)
{
    u8 state = 0;

    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOHIG);      /*管脚复用功能译码器片选高阻态(低脉冲有效)*/

    (void)GPIOMulPinDirInit(0);                             /*复用管脚初始化为输入*/

    //编码:A0=1,A1=1,A2=0:CSDI1=0选通有效
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_13, eIOHIG);     /*设置管脚复用功能译码器编码A0*/
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_12, eIOHIG);     /*设置管脚复用功能译码器编码A1*/
    (void)BSPGPIOSetPortState(ePOR_C, ePIN_11, eIOLOW);     /*设置管脚复用功能译码器编码A2*/

    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOLOW);      /*管脚复用功能译码器片选拉低脉冲(低脉冲有效)*/

    state = GPIOMulPinGetAllPortState();                    /*获取复用管脚当前所有管脚状态*/

    (void)BSPGPIOSetPortState(ePOR_D, ePIN_8, eIOHIG);      /*管脚复用功能译码器片选恢复高阻态(低脉冲有效)*/

    return(state);
}
