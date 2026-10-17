/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Fs6503_PowerManag.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电源管理芯片驱动模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "pins_driver.h"
#include "osif.h"
#include "status.h"
#include "BSPLPIT.h"
#include "DEVGPIO.h"
#include "wait.h"
#include "spi1_pal.h"
#include "sbc_fs65.h"
#include "sbc_fs65_map.h"
#include "sbc_fs65_common.h"
#include "sbc_fs65_communication.h"
#include "Fs6503_PowerManag.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
/* Configuration of the FS65 SBC. */
fs65_user_config_t userConfig = {0};

/* Flag indicating WD refresh is required. */
volatile bool isWdRefreshRequired = false;

/* If true, RESET pin was released. */
volatile bool isResetReleased = false;

/* If true, low priority communication is enabled.
 * Will be set to true right after WD refresh.
 * Communication should be finished before the "closed"
 * WD window ends. */
volatile bool isCommEnabled = false;

/*******************************************************************************
 * FS65 configuration structures
 * See description for the fs65_user_config_t structure for more information.
 ******************************************************************************/
static fs65_reg_config_value_t initMainRegs[] =
{
    {
        FS65_M_INIT_VREG_ADDR,
        FS65_RW_M_VAUX_TRK_EN_NO_TRACKING | FS65_RW_M_TAUX_LIM_OFF_50_MS |
        FS65_RW_M_VCAN_OV_MON_OFF | FS65_RW_M_IPFF_DIS_ENABLED | FS65_RW_M_TCCA_LIM_OFF_50_MS |
        FS65_RW_M_ICCA_LIM_ICCA_LIM_OUT,
        FS65_RW_M_VAUX_TRK_EN_MASK | FS65_RW_M_TAUX_LIM_OFF_MASK | FS65_RW_M_VCAN_OV_MON_MASK |
        FS65_RW_M_IPFF_DIS_MASK | FS65_RW_M_TCCA_LIM_OFF_MASK | FS65_RW_M_ICCA_LIM_MASK,
        false
    },
    {
        FS65_M_INIT_WU1_ADDR,
        //FS65_RW_M_WU_IO4_NO_WAKEUP | FS65_RW_M_WU_IO3_NO_WAKEUP | FS65_RW_M_WU_IO2_NO_WAKEUP |
        FS65_RW_M_WU_IO4_RISING_EDGE | FS65_RW_M_WU_IO3_NO_WAKEUP | FS65_RW_M_WU_IO2_NO_WAKEUP |
        FS65_RW_M_WU_IO0_RISING_EDGE,
        0xFFU,
        false
    },
    {
        FS65_M_INIT_WU2_ADDR,
        FS65_RW_M_LIN_SR_20KBITS | FS65_RW_M_LIN_J2602_DIS_COMPLIANT | FS65_RW_M_CAN_WU_TO_120US |
        FS65_RW_M_CAN_DIS_CFG_RX_ONLY | FS65_RW_M_WU_IO5_NO_WAKEUP,
        0xFFU,
        false
    },
    {
        FS65_M_INIT_INH_INT_ADDR,
        FS65_RW_M_INT_INH_0_NOT_MASKED | FS65_RW_M_INT_INH_2_MASKED | FS65_RW_M_INT_INH_3_MASKED |
        FS65_RW_M_INT_INH_4_NOT_MASKED | FS65_RW_M_INT_INH_5_NOT_MASKED,
        FS65_RW_M_INT_INH_0_MASK | FS65_RW_M_INT_INH_2_MASK | FS65_RW_M_INT_INH_3_MASK |
        FS65_RW_M_INT_INH_4_MASK | FS65_RW_M_INT_INH_5_MASK,
        false
    }
};

static fs65_reg_config_value_t initFailSafeRegs[] =
{
    {
        FS65_FS_INIT_FS1B_TIMING_ADDR,
        FS65_R_FS_FS1B_TIME_106_848MS,
        FS65_R_FS_FS1B_TIME_MASK,
		true
    },
    {
        FS65_FS_INIT_SUPERVISOR_ADDR,
        FS65_R_FS_FS1B_TIME_RANGE_X1 | FS65_R_FS_VAUX_5D_NORMAL | FS65_R_FS_VCCA_5D_NORMAL |
        FS65_R_FS_VCORE_5D_NORMAL,
        0x0FU,
		true
    },
    {
        FS65_FS_INIT_FAULT_ADDR,
        FS65_R_FS_FLT_ERR_IMP_RSTB | FS65_R_FS_FS1B_CAN_IMPACT_RX_ONLY | FS65_R_FS_FLT_ERR_FS_INT3_FIN6,
        0x0FU,
		true
    },
    {
        FS65_FS_INIT_FSSM_ADDR,
        FS65_R_FS_RSTB_DURATION_10MS | FS65_R_FS_PS_HIGH | FS65_R_FS_IO_23_FS_NOT_SAFETY |
        FS65_R_FS_IO_45_FS_NOT_SAFETY,
        0x0FU,
		true
    },
    {
        FS65_FS_INIT_SF_IMPACT_ADDR,
        FS65_R_FS_WD_IMPACT_RSTB | FS65_R_FS_DIS_8S_ENABLED | FS65_R_FS_TDLY_TDUR_DELAY,
        0x0FU,
		true
    },
    {
        FS65_FS_WD_WINDOW_ADDR,
		FS65_R_FS_WD_WINDOW_128MS,
        0x0FU,
		true
    },
    {
        FS65_FS_WD_LFSR_ADDR,
        FS65_WD_SEED_DEFAULT,
        0xFFU,
        false
    },
    {
        FS65_FS_INIT_WD_CNT_ADDR,
        FS65_R_FS_WD_CNT_RFR_6 | FS65_R_FS_WD_CNT_ERR_6,
        0x0FU,
		true
    },
    {
        FS65_FS_INIT_VCORE_OVUV_IMPACT_ADDR,
        FS65_R_FS_VCORE_FS_UV_FS0B | FS65_R_FS_VCORE_FS_OV_RSTB_FS0B,
        0x0FU,
		true
    },
    {
        FS65_FS_INIT_VCCA_OVUV_IMPACT_ADDR,
        FS65_R_FS_VCCA_FS_UV_FS0B | FS65_R_FS_VCCA_FS_OV_RSTB_FS0B,
        0x0FU,
		true
    },
    {
        FS65_FS_INIT_VAUX_OVUV_IMPACT_ADDR,
        FS65_R_FS_VAUX_FS_UV_FS0B | FS65_R_FS_VAUX_FS_OV_RSTB_FS0B,
        0x0FU,
		true
    }
};

static fs65_reg_config_value_t nonInitRegs[] =
{
    {
        FS65_M_MODE_ADDR,
        FS65_RW_M_VKAM_EN_DISABLED,
        FS65_RW_M_VKAM_EN_MASK,
        true
    },
    {
        FS65_M_REG_MODE_ADDR,
        FS65_R_M_VCAN_EN_ENABLED | FS65_R_M_VAUX_EN_ENABLED | FS65_R_M_VCCA_EN_ENABLED |
        FS65_R_M_VCORE_EN_ENABLED,
        FS65_R_M_VCAN_EN_MASK | FS65_R_M_VAUX_EN_MASK | FS65_R_M_VCCA_EN_MASK |
        FS65_R_M_VCORE_EN_MASK,
        true
    },
    {
        FS65_M_IO_OUT_AMUX_ADDR,
        FS65_RW_M_AMUX_VREF | FS65_RW_M_IO_OUT_4_LOW | FS65_RW_M_IO_OUT_4_EN_Z,
        FS65_RW_M_AMUX_MASK | FS65_RW_M_IO_OUT_4_MASK | FS65_RW_M_IO_OUT_4_EN_MASK,
        false
    },
    {
        FS65_M_CAN_LIN_MODE_ADDR,
        FS65_RW_M_LIN_AUTO_DIS_RESET | FS65_RW_M_LIN_MODE_NORMAL | FS65_RW_M_CAN_AUTO_DIS_RESET |
        FS65_RW_M_CAN_MODE_NORMAL,
        FS65_RW_M_LIN_AUTO_DIS_MASK | FS65_RW_M_LIN_MODE_MASK | FS65_RW_M_CAN_AUTO_DIS_MASK |
        FS65_RW_M_CAN_MODE_MASK,
        false
    }
};

/*用户请求下电标志:0无请求 1:请求下电*/
static u8 sUserPwrOffFlag = 0;
static u8 sIRQAddTimer[2] = {0};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void loadUserConfig(fs65_user_config_t* config)
** @Input     : config:FS6503配置参数结构体
** @Output    : void
** @Function  : 加载FS6503配置参数
** @The notes :
**===========================================================================================*/
static void loadUserConfig(fs65_user_config_t* config);

/*=============================================================================================
** @Name      : static void FS6503CSLOW(void)
** @Input     : void
** @Output    : void
** @Function  : 选通FS6503
** @The notes :
**===========================================================================================*/
static void FS6503CSLOW(void);

/*=============================================================================================
** @Name      : static void FS6503CSHIGH(void)
** @Input     : void
** @Output    : void
** @Function  : 禁止FS6503
** @The notes :
**===========================================================================================*/
static void FS6503CSHIGH(void);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : fs65_status_t Fs6503Init(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化FS6503电源芯片
** @The notes : 该函数在初始化完MCU之后调用
**===========================================================================================*/
fs65_status_t Fs6503Init(void)
{
    fs65_status_t sbcStatus = fs65StatusOk; /* SBC return status code. */

    loadUserConfig(&userConfig);/*加载FS6503配置参数*/

	sbcStatus = FS65_Init(&userConfig);

	/* Start timers for WD refresh and diagnostics */
	#ifdef FS65_WD_ENABLED
		WD_TIMER_START();
	#endif
	DIAG_TIMER_START();

	sbcStatus = FS65_SetRegulatorState(fs65Vkam, true);
	sbcStatus |= FS65_CAN_SetMode(fs65CanModeNormal, true);

    return sbcStatus;
}

/*=============================================================================================
** @Name      : fs65_status_t FS65DiagnosticTask(void)
** @Input     : void
** @Output    : void
** @Function  : FS6503诊断任务(上拉故障输出管脚)
** @The notes : 在关闭FS6503软件喂狗之后调用
**===========================================================================================*/
fs65_status_t FS65DiagnosticTask(void)
{
	fs65_status_t sbcStatus = STATUS_SUCCESS;       /* Return status code. */
	fs65_rx_data_t rxData;
	uint8_t errorCounter = 0xFF;

	/*用户需要下电关机则停止中断(停止上拉保持低电平)*/
	if(1 == sUserPwrOffFlag)
	{
	    return sbcStatus;
	}

    /* Diagnostic communication is enabled just once after the WD is refreshed
     * (during the WD CLOSED window). */
    if (isCommEnabled && !isWdRefreshRequired)
    {
        isCommEnabled = false;

        sbcStatus = FS65_GetFaultErrorCounterValue(&errorCounter);
        sbcStatus |= FS65_ReadRegister(FS65_FS_RELEASE_FSXB_ADDR, &rxData);
        if (sbcStatus != fs65StatusOk)
        {
            return sbcStatus;
        }

        /* FS0b pin asserted, start release procedure. */
        if (errorCounter == 0 && (rxData.readData & FS65_R_FS_FS0B_SNS_MASK) == FS65_R_FS_FS0B_SNS_LOW)
        {
            //PRINTF("FSxb pins release started... ");

            /* Close S1 switch between VPRE and VPU_FS if FS1B backup delay was engaged (FS1B_DLY_DRV bit = 1). */
            sbcStatus = FS65_WriteRegister(FS65_FS_SF_OUTPUT_REQUEST_ADDR, 0, NULL);

            /* Try to release the FS0b pin. */
            sbcStatus |= FS65_ReleaseFSx(fs65ReleaseFs0bFs1b);

            sbcStatus |= FS65_ReadRegister(FS65_FS_RELEASE_FSXB_ADDR, &rxData);
            if (sbcStatus != fs65StatusOk)
            {
                return sbcStatus;
            }

            if ((rxData.readData & FS65_R_FS_FS0B_SNS_MASK) == FS65_R_FS_FS0B_SNS_LOW)
            {
            	//控制FS0B失败
            }
            else
            {
            	//控制FS0B成功
            }
        }
    }
    return sbcStatus;
}

/*=============================================================================================
** @Name      : void FS65WdRefreshTask(void)
** @Input     : void
** @Output    : void
** @Function  : FS6503周期喂狗任务
** @The notes : 中断调用
**===========================================================================================*/
void FS65WdRefreshTask(void)
{
	fs65_status_t sbcStatus = fs65StatusOk; /* SBC return status code. */

//	if (isWdRefreshRequired)
//	{
		//无用户请求下电才正常喂狗(用户请求下电则停止喂狗即可下电)
		if(0 == sUserPwrOffFlag)
		{
			sbcStatus = FS65_WD_Refresh();
			if (sbcStatus == fs65StatusOk)
			{
				isWdRefreshRequired = false;
			}
		}
//	}

		//判断RTOS是否死机(18s周期)
		if(sIRQAddTimer[0] >= 180)
		{
			//RTOS计时出错为死机(不够10s)
			if(sIRQAddTimer[1] <= 20)
			{
				//复位程序
				/*FreeRTOS中断优先级分成两组：一组受 FreeRTOS管理，在临界区里会被屏蔽，另一组不受RTOS影响，永远都是使能的。*/
				BSP_DISABLE_IRQ();

				/* SystemSoftwareReset */
				SystemSoftwareReset();/*软复位直接进入boot*/
			}

			//重新计时
			sIRQAddTimer[0] = 0;
			sIRQAddTimer[1] = 0;
		}
		//未到达18s则继续计时
		else
		{
			sIRQAddTimer[0]++;
		}
}

/*=============================================================================================
** @Name      : void FS65SetUserPwrOffFlagAPI(u8 flag)
** @Input     : flag:用户下电请求标志:0上电请求 1:下电请求
** @Output    : void
** @Function  : 设置FS6503用户下电请求标志
** @The notes : 请求下电时进入软件休眠模式
**===========================================================================================*/
void FS65SetUserPwrOffFlagAPI(u8 flag)
{
	//请求下电(只执行一次)
	if((1 == flag) && (0 == sUserPwrOffFlag))
	{
		//进入软件休眠模式
		(void)FS65_SetLowPowerMode(false);
	}

	sUserPwrOffFlag = flag;
}

/*=============================================================================================
** @Name      : fs65_status_t MCU_SPI_TransferData(uint8_t* txFrame, uint8_t* rxFrame)
** @Input     : txFrame:SPI发送数据      rxFrame：SPI接收数据
** @Output    : void
** @Function  : Implementation of driver function for SPI communication.
** @The notes : 高位先发
**===========================================================================================*/
fs65_status_t MCU_SPI_TransferData(uint8_t* txFrame, uint8_t* rxFrame)
{
    status_t status = STATUS_SUCCESS;
    uint8_t tx[2];
    uint8_t rx[2];
    u16 senddata = 0;
    u16 getdata = 0;

    tx[0] = txFrame[0];/*缓冲区*/
    tx[1] = txFrame[1];

    DEV_ASSERT(txFrame != NULL);

    FS6503CSLOW();/*选通FS6503*/

    senddata = (u16)tx[1] << 8 | tx[0];

    BSPSPISendData(eSPI0, senddata);

    getdata = BSPSPIGetRcvData(eSPI0);

    rx[0] = getdata & 0xff;
    rx[1] = (getdata >> 8) & 0xff;

    FS6503CSHIGH();/*禁止FS6503*/

    if (rxFrame != NULL)
    {
        rxFrame[0] = rx[0];
        rxFrame[1] = rx[1];
    }

    if (status != STATUS_SUCCESS)
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*=============================================================================================
** @Name      : void MCU_WaitUs(uint16_t delay)
** @Input     : delay:软件延时时间
** @Output    : void
** @Function  : Implementation of driver function for blocking wait.
** @The notes :
**===========================================================================================*/
void MCU_WaitUs(uint16_t delay)
{
    S32_WaitUs(delay);
}

/*=============================================================================================
** @Name      : status_t sbcTestLdt(void)
** @Input     : void
** @Output    : void
** @Function  : Test of the LDT timer. Uses LDT Function 3(sleep and wake-up).
** @The notes :
**===========================================================================================*/
status_t sbcTestLdt(void)
{
    fs65_status_t status;

    status = FS65_LDT_RunCounter(false);
    status |= FS65_LDT_SetTimerOperation(fs65LdtFunc3);
    status |= FS65_LDT_SetTimerMode(fs65LdtModeNormal);
    status |= FS65_LDT_SetAfterRunValue(SBC_LDT_AFTER_RUN_VALUE);
    status |= FS65_LDT_SetWakeUpValue(SBC_LDT_WAKE_UP_VALUE);

    status |= FS65_LDT_RunCounter(true);

    if (status != fs65StatusOk)
    {
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/*=============================================================================================
** @Name      : void OSDiagTimerAddTask(void)
** @Input     : void
** @Output    : void
** @Function  : RTOS诊断计时任务
** @The notes : 500ms定时调用
**===========================================================================================*/
void OSDiagTimerAddTask(void)
{
	sIRQAddTimer[1]++;
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void loadUserConfig(fs65_user_config_t* config)
** @Input     : config:FS6503配置参数结构体
** @Output    : void
** @Function  : 加载FS6503配置参数
** @The notes :
**===========================================================================================*/
static void loadUserConfig(fs65_user_config_t* config)
{
    config->initMainRegs = initMainRegs;
    config->initMainRegsCount = 4;

    config->initFailSafeRegs = initFailSafeRegs;
    config->initFailSafeRegsCount = 11;

    config->nonInitRegs = nonInitRegs;
    config->nonInitRegsCount = 3;

    config->initIntReg = 0;
}

/*=============================================================================================
** @Name      : static void FS6503CSLOW(void)
** @Input     : void
** @Output    : void
** @Function  : 选通FS6503
** @The notes :
**===========================================================================================*/
static void FS6503CSLOW(void)
{
	DEVGPIOSetPortState(ePOut_SBCFS65_CS, ePSTATE_OFF);/*片选拉低*/
}

/*=============================================================================================
** @Name      : static void FS6503CSHIGH(void)
** @Input     : void
** @Output    : void
** @Function  : 禁止FS6503
** @The notes :
**===========================================================================================*/
static void FS6503CSHIGH(void)
{
	 DEVGPIOSetPortState(ePOut_SBCFS65_CS, ePSTATE_ON);/*片选拉高*/
}
