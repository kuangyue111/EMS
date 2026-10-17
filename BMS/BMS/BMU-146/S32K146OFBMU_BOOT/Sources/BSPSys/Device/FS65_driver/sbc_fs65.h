/*
 * Copyright 2016 - 2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file sbc_fs65.h
 * @brief FS65/FS45 driver interface.
 *
 * @author nxf44615
 * @version 1.0
 * @date 13-Nov-2018
 * @copyright Copyright 2016 - 2018 NXP
 */

#ifndef SBC_FS65_H__
#define SBC_FS65_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "sbc_fs65_common.h"
#include "sbc_fs65_map.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** @brief Watchdog seed default value. */
#define FS65_WD_SEED_DEFAULT 0xB2U

/** @addtogroup EnumsDefs
 * @{ */

/** @brief Voltage outputs. Can be used with function @ref FS65_SetRegulatorState(). */
typedef enum
{
    /** @brief VCAN control. */
    fs65VCan = FS65_R_M_VCAN_EN_SHIFT,
    /** @brief VAUX control (switch off not recommended if VAUX is safety critical). */
    fs65Aux = FS65_R_M_VAUX_EN_SHIFT,
    /** @brief VCCA control (switch off not recommended if VCCA is safety critical). */
    fs65Vcca = FS65_R_M_VCCA_EN_SHIFT,
    /** @brief VCORE control (switch off not recommended if VCORE is safety critical). */
    fs65Vcore = FS65_R_M_VCORE_EN_SHIFT,
    /** @brief VKAM control. */
    fs65Vkam = 0xFF
} fs65_reg_mode_t;

/** @brief AMUX channel selection. Can be used with function @ref FS65_SwitchAMUXchannel(). */
typedef enum
{
    fs65AmuxVref = FS65_RW_M_AMUX_VREF,                 /**< @brief V_REF. */
    fs65AmuxVsnsWide = FS65_RW_M_AMUX_VSNS_W,           /**< @brief V_SNS wide range.  */
    fs65AmuxIO_0Wide = FS65_RW_M_AMUX_IO_0_W,           /**< @brief IO_0 wide range. */
    fs65AmuxIO_5Wide = FS65_RW_M_AMUX_IO_5_W,           /**< @brief IO_5 wide range. */
    fs65AmuxVsnsTight = FS65_RW_M_AMUX_VSNS_T,          /**< @brief V_SNS tight range. */
    fs65AmuxIO_0Tight = FS65_RW_M_AMUX_IO_0_T,          /**< @brief IO_0 tight range. */
    fs65AmuxIO_5TightDivVkam = FS65_RW_M_AMUX_IO_5_T,   /**< @brief IO_5 tight range/VKAM. */
    fs65AmuxDieTempSensor = FS65_RW_M_AMUX_TEMP_SENSOR, /**< @brief Die Temperature Sensor. */
} fs65_amux_selection_t;

/** @brief CAN mode. Can be used with function @ref FS65_CAN_SetMode(). */
typedef enum
{
    fs65CanModeSleepNoWakeup = FS65_RW_M_CAN_MODE_SLN_WU,   /**< @brief Sleep/no wake-up capability. */
    fs65CanModeListenOnly = FS65_RW_M_CAN_MODE_LISTEN_ONLY, /**< @brief Listen only. */
    fs65CanModeSleepWakeup = FS65_RW_M_CAN_MODE_SL_WU,      /**< @brief Sleep/wake-up capability. */
    fs65CanModeNormal = FS65_RW_M_CAN_MODE_NORMAL,          /**< @brief Normal operation mode. */
} fs65_can_mode_t;

/** @brief LIN mode. Can be used with function @ref FS65_LIN_SetMode(). */
typedef enum
{
    fs65LinModeSleepNoWakeup = FS65_RW_M_LIN_MODE_SLN_WU,   /**< @brief Sleep/no wake-up capability. */
    fs65LinModeListenOnly = FS65_RW_M_LIN_MODE_LISTEN_ONLY, /**< @brief Listen only. */
    fs65LinModeSleepWakeup = FS65_RW_M_LIN_MODE_SL_WU,      /**< @brief Sleep/wake-up capability. */
    fs65LinModeNormal = FS65_RW_M_LIN_MODE_NORMAL,          /**< @brief Normal operation mode. */
} fs65_lin_mode_t;

/** @brief LDT operating function. Can be used with function @ref FS65_LDT_SetTimerOperation(). */
typedef enum
{
    /** @brief In normal mode count and generate flag or INT when counter reaches the after run value. */
    fs65LdtFunc1 = FS65_RW_M_F2_F0_FUNCTION1,
    /** @brief In normal mode count until after run value is reached, then enters in LPOFF. */
    fs65LdtFunc2 = FS65_RW_M_F2_F0_FUNCTION2,
    /** @brief In normal mode count until after run value is reached, then enters in LPOFF. Once in LPOFF, count until
     *         wake-up value is reached and wake-up. */
    fs65LdtFunc3 = FS65_RW_M_F2_F0_FUNCTION3,
    /** @brief In LPOFF, count until wake-up value is reached and wake-up. */
    fs65LdtFunc4 = FS65_RW_M_F2_F0_FUNCTION4,
    /** @brief In LPOFF, count and do not wake-up. Counter value is stored in wake-up register. */
    fs65LdtFunc5 = FS65_RW_M_F2_F0_FUNCTION5,
} fs65_ldt_function_t;

/** @brief LDT mode. Can be used with function @ref FS65_LDT_SetTimerMode(). */
typedef enum
{
    fs65LdtModeCalibration = FS65_RW_M_MODE_CALIBRATION, /**< @brief Calibration mode (488 us resolution). */
    fs65LdtModeNormal = FS65_RW_M_MODE_NORMAL,           /**< @brief Normal mode (1 s resolution). */
} fs65_ldt_mode_t;

/** @brief Wake-up register source. Can be used with function @ref FS65_LDT_SetWakeUpRegSrc(). */
typedef enum
{
    /** @brief Read programmed wake-up register. */
    fs65LdtWakeupProg = FS65_RW_M_REG_SE_PROGRAMMED_REG,
    /** @brief Read real time counter into wake-up register (after counter is stopped with LDT_ENABLE bit). */
    fs65LdtWakeupRTC = FS65_RW_M_REG_SE_RTC_REG,
} fs65_ldt_wu_scr_t;

/** @brief Fail-safe output selection for its low level request.
 *         Can be used with function @ref FS65_RequestFSxLow(). */
typedef enum
{
    /** @brief Request FS0B assertion. */
    fs65ReqFS0B = FS65_W_FS_FS0B_REQ_FS0B_REQ,
    /** @brief Request FS1B assertion with tDELAY controlled by the backup delay (open S1). */
    fs65ReqFS1BDelay = FS65_W_FS_FS1B_DLY_REQ_FS1B_REQ,
    /** @brief Request FS1B assertion with immediate assertion, no delay. */
    fs65ReqFS1B = FS65_W_FS_FS1B_REQ_FS1B_REQ,
} fs65_fsx_req_type_t;
/** @} */

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/** @defgroup API Driver API
 * @{ */

/**
 * @brief This function runs full initialization of the SBC device.
 *
 * This function writes configuration values to selected SBC registers
 * (i.e. content of the userConfig structure),
 * checks results of built-in self-test diagnostics and optionally
 * runs additional procedures to ensure correct functionality of
 * safety critical features.
 * The first watchdog refresh is part of the initialization procedure,
 * the SBC should be in NORMAL mode upon the function exit.
 *
 * @param userConfig        Configuration structure of the SW driver.
 *
 * @return @ref fs65_status_t Status return code.
 *
 * @remark Note that main register set is initialized only after power on reset.
 *         It retains its values after transition to low-power mode and
 *         back to normal.
 *         It is recommended to read-out wake-up sources and diagnostic statuses
 *         after initialization to clear all flags.
 */
fs65_status_t FS65_Init(fs65_user_config_t* userConfig);

/**
 * @brief Changes seed of LFSR used for watchdog.
 *
 * The watchdog seed can be changed just during the INIT_FS phase (for challenger WD)
 * or during the OPEN watchdog window (for simple WD). Timing is up to the application!
 *
 * @param [in] wdSeed   Watchdog LFSR seed.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_WD_ChangeSeed(uint8_t wdSeed);

/**
 * @brief Switches a desired channel to the AMUX pin.
 *
 * @param [in] channelSelection  Selected channel to be delivered to AMUX pin.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_SwitchAMUXchannel(fs65_amux_selection_t channelSelection);

/**
 * @brief Sets state (enable/disable) of the selected voltage regulator.
 *
 * @param [in] vreg     Voltage regulator enum (VCAN, VAUX, VCCA, VCORE).
 * @param [in] enable   State (enable = true / disable = false).
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_SetRegulatorState(fs65_reg_mode_t vreg, bool enable);

/**
 * @brief Reads actual Fault Error Counter value.
 * @param [out] faultErrorCounterValue  Fault Error counter value storage.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_GetFaultErrorCounterValue(uint8_t* faultErrorCounterValue);

/**
 * @brief This function gets current and previous mode of the SBC.
 *
 * @param [out] currentMode     Current mode of the SBC device.
 * @param [out] prevMode        Previous mode of the SBC device.
 *
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_GetMode(fs65_current_mode_t *currentMode, fs65_prev_mode_t *prevMode);

/**
 * @brief This function checks if VAUX is safety critical and optionally
 *        runs related diagnostics.
 *
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_CheckVAUX(void);

#ifdef FS65_FEATURE_FS1B
/**
 * @brief This function checks if FS1B has expected low level during
 *        initialization and runs related diagnostics.
 *
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_CheckFS1B(void);

#endif

/**
 * @brief This function releases selected fail-safe output as a part of
 *        error recovery procedure.
 *
 * @param [in] fsOutput Selection of fail-safe output.
 * @return @ref fs65_status_t Status return code.
 *
 * @remark When a fault is removed and the fault error counter changes back to
 *         level '0', a right word must be filled in the RELEASE_FSxB register.
 *         The value depends on the current WD_LFSR. LSB, MSB must be swapped,
 *         and a negative operation per bit must be applied.
 *         The RELEASE_FSxB write command must be done after the WD_LFSR read
 *         command within the same WD period.
 */
fs65_status_t FS65_ReleaseFSx(fs65_fsxb_release_t fsOutput);

/**
 * @brief Checks if LBIST and ABIST1 diagnostics passed.
 *
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_CheckLbistAbistOk(void);

/**
 * @brief This function switches mode of the SBC to the LPOFF, optionally with
 *        automatic wake-up 1ms after transition.
 *
 * @param autoWU            Automatic wake-up 1 ms after transition.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_SetLowPowerMode(bool autoWU);

/**
 * @brief This function requests an interrupt (pulse on the INT pin).
 *
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_RequestInterrupt(void);

#ifdef FS65_FEATURE_CAN
/**
 * @brief This function changes CAN mode and the automatic transition of the CAN
 *        transceiver to the low-power mode on specific events.
 *
 * @param [in] mode         CAN mode.
 * @param [in] autoDis      Automatic transition to the LPOFF on
 *                          (CAN OT/TXD dominant/RXD recessive).
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_CAN_SetMode(fs65_can_mode_t mode, bool autoDis);
#endif

#ifdef FS65_FEATURE_LIN
/**
 * @brief This function changes LIN mode and the automatic transition of the LIN
 *        transceiver to the low-power mode on specific events.
 *
 * @param [in] mode     LIN mode.
 * @param [in] autoDis  Automatic transition to the LPOFF on
 *                      (LIN OT/TXDL dominant/RXDL recessive).
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_LIN_SetMode(fs65_lin_mode_t mode, bool autoDis);
#endif

#ifdef FS65_FEATURE_LDT
/**
 * @brief This function sets operating function of the LDT.
 *
 * @param [in] op   Operating function of the LDT.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_LDT_SetTimerOperation(fs65_ldt_function_t op);

/**
 * @brief This function sets mode of the LDT (normal/calibration).
 *
 * @param [in] mode     Mode of the LDT.
 * @return @ref fs65_status_t Status return code.
 *
 * @remark Resolution is 1s for normal mode and 488us for calibration mode.
 */
fs65_status_t FS65_LDT_SetTimerMode(fs65_ldt_mode_t mode);

/**
 * @brief This function sets counter to read real-time counter or programmed
 *        value into wake-up register.
 *
 * @param [in] source   Source for the wake-up register.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_LDT_SetWakeUpRegSrc(fs65_ldt_wu_scr_t source);

/**
 * @brief This function starts or stops the LDT counter.
 *
 * @param [in] run   Use true for start or false for stop the LDT counter.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_LDT_RunCounter(bool run);

/**
 * @brief This function sets new after-run value for the LDT.
 *
 * @param [in] value             After-run value.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_LDT_SetAfterRunValue(uint16_t value);

/**
 * @brief This function sets new wake-up value for the LDT.
 *
 * @param [in] value     Wake-up value (1 - 16777215).
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_LDT_SetWakeUpValue(uint32_t value);
#endif

/**
 * @brief This function changes duration of watchdog window.
 *
 * @param [in] windowDuration   Watchdog window duration. Use any FS65_W_FS_WD_WINDOW_*
 *                              macro from @ref sbc_fs65_map.h.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_WD_ChangeWindow(uint8_t windowDuration);

/**
 * @brief This function refreshes watchdog of the SBC device.
 *
 * @return @ref fs65_status_t Status return code.
 *
 * @remark MCU internally computes monitoring result based on a received
 *         challenge token (LFSR state) and sends it back as an answer.
 */
fs65_status_t FS65_WD_Refresh(void);

/**
 * @brief This function requests a low pulse on the RSTB (MCU reset).
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_RequestReset(void);

/**
 * @brief This function requests a low level on the selected fail-safe output.
 *
 * @param [in] fsxSelection  Selection of fail-safe output.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_RequestFSxLow(fs65_fsx_req_type_t fsxSelection);

/**
 * @brief This function sets level of the IO_4 when configured as an output.
 *
 * @param [in] level        Level of IO_4 when configured as output. True for high
 *                          level, false for low level.
 * @return @ref fs65_status_t Status return code.
 *
 * @remark Note that this function automatically enables IO_4 to be used as output.
 */
fs65_status_t FS65_SetOUT4(bool level);

/** @} */

#endif /* SBC_FS65_H__ */
