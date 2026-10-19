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
 * @file sbc_fs65.c
 * @brief Driver functions for the FS65/FS45 SBC.
 *
 * @author nxf44615
 * @version 1.0
 * @date 13-Nov-2018
 * @copyright Copyright 2016 - 2018 NXP
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "wait.h"
#include "sbc_fs65.h"
#include "sbc_fs65_communication.h"
#include "sbc_fs65_assert.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Shift of the register write value if secured bits are used. */
#define FS65_SECURE_WRITE_SHIFT 4U

/**
 * @brief Returns true if value VAL is in the range defined by MIN and MAX values
 *        (range includes the border values).
 * @param val Comparison value.
 * @param min Minimal value of the range.
 * @param max Maximal value of the range.
 * @return True if value is the range. False otherwise. */
#define FS65_IS_IN_RANGE(val, min, max)   (((val) >= (min)) && ((val) <= (max)))

/*******************************************************************************
 * Local Functions Prototypes
 ******************************************************************************/
/**
 * @brief Reads challenge token (next generated LFSR state) from the SBC.
 *
 * @param [out] wdSeed Challenge token.
 * @return Status result of the function.
 */
static fs65_status_t FS65_WD_ReadChallengeToken(uint8_t* wdSeed);

/**
 * @brief Compute Challenger Watchdog answer.
 *
 * @param token Actual LFSR value.
 * @return Computed watchdog answer.
 */
static uint8_t FS65_WD_ComputeAnswer(uint8_t token);

/*******************************************************************************
 * Local Variables
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/


/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_WD_ReadChallengeToken
 * Description : Reads challenge token (next generated LFSR state) from the SBC.
 *
 *END**************************************************************************/
static fs65_status_t FS65_WD_ReadChallengeToken(uint8_t* wdSeed)
{
    fs65_status_t status = fs65StatusOk;    /* Status variable. */
    fs65_rx_data_t response;                /* Response to the command. */

    FS_ASSERT(wdSeed != NULL);

    status = FS65_ReadRegister(FS65_FS_WD_LFSR_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Store the watchdog seed from the SBC device. */
    *wdSeed = response.readData;

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_WD_ComputeAnswer
 * Description : Compute Challenger Watchdog answer.
 *
 *END**************************************************************************/
static uint8_t FS65_WD_ComputeAnswer(uint8_t token)
{
    uint16_t mr = token; /* Monitoring result. */

    /* Compute challenger watchdog answer. */
    mr *= 4U;
    mr += 6U;
    mr -= 4U;
    mr = ~mr;
    mr /= 4U;

    return (uint8_t)mr;
}

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_Init
 * Description : This function runs full initialization of the SBC device.
 *
 *END**************************************************************************/
fs65_status_t FS65_Init(fs65_user_config_t* userConfig)
{
    fs65_status_t status = fs65StatusOk;    /* Status variable. */
    fs65_rx_data_t response;                /* Response to the command. */
    fs65_current_mode_t sbcMode;            /* Actual SBC mode. */
    fs65_prev_mode_t prevMode;              /* Previous SBC mode. */
    uint8_t initIntValue;

    FS_ASSERT(userConfig != NULL);

    status = FS65_CheckLbistAbistOk();
    if (status != fs65StatusOk)
    {
        return status;
    }

#ifdef FS65_FEATURE_FS1B
    /* Check if FS1B is at low (if implemented). */
    status = FS65_CheckFS1B();
    if (status != fs65StatusOk)
    {
        return status;
    }
#endif

    /* Determine if it is necessary to re-initialize main register set. */
    status = FS65_GetMode(&sbcMode, &prevMode);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* MSM does not have to be initialized in case of transition
     * LPOFF -> NORMAL. */
    if ((sbcMode == fs65ModeInit) && (prevMode != fs65PrevModeLPOFF))
    {
        /* Initialize main registers. */
        status = FS65_WriteRegisters(userConfig->initMainRegs, userConfig->initMainRegsCount);
        if (status != fs65StatusOk)
        {
            return fs65StatusError;
        }
    }

    /* FSSM can be initialized only in INIT_FS. */
    if (sbcMode == fs65ModeInit)
    {
        /* Initialize FSSM (fail-safe registers). */
        status = FS65_WriteRegisters(userConfig->initFailSafeRegs, userConfig->initFailSafeRegsCount);
        if (status != fs65StatusOk)
        {
            return fs65StatusError;
        }

        /* INIT_MAIN -> NORMAL_MODE. */
        /* If INIT_INT config value is not defined, just read and write the register value. */
        if (userConfig->initIntReg == NULL)
        {
            status = FS65_ReadRegister(FS65_M_INIT_INT_ADDR, &response);
            if (status != fs65StatusOk)
            {
                return status;
            }
            initIntValue = response.readData;
        }
        else
        {
            initIntValue = *(userConfig->initIntReg);
        }
        status |= FS65_WriteRegister(FS65_M_INIT_INT_ADDR, initIntValue, NULL);
        if (status != fs65StatusOk)
        {
            return status;
        }
    }

    /* Read Diag. registers to clear all bits. */
    status = FS65_ReadRegister(FS65_M_DIAG_VPRE_ADDR, &response);
    status |= FS65_ReadRegister(FS65_M_DIAG_VCORE_ADDR, &response);
    status |= FS65_ReadRegister(FS65_M_DIAG_VCCA_ADDR, &response);
    status |= FS65_ReadRegister(FS65_M_DIAG_VAUX_ADDR, &response);
    status |= FS65_ReadRegister(FS65_M_DIAG_VSUP_VCAN_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Initialize the rest of registers. */
    status = FS65_WriteRegisters(userConfig->nonInitRegs, userConfig->nonInitRegsCount);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* INIT_FS -> NORMAL_WD */
    status = FS65_WD_Refresh();
    if (status != fs65StatusOk)
    {
        return status;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_WD_Refresh
 * Description : Performs the watchdog refresh.
 *
 *END**************************************************************************/
fs65_status_t FS65_WD_Refresh(void)
{
    fs65_status_t status = fs65StatusOk;    /* Status variable. */
    uint8_t lfsr;                           /* Line feed shift register for challenger WD. */
    uint8_t answer;                         /* Calculated monitoring result. */

    /* Synchronize with LFSR generator on the device. */
    status = FS65_WD_ReadChallengeToken(&lfsr);
    if (status != fs65StatusOk)
    {
        return status;
    }

    answer = FS65_WD_ComputeAnswer(lfsr);

    status = FS65_WriteRegister(FS65_M_WD_ANSWER_ADDR,
            (uint8_t)(answer << FS65_W_M_WD_ANSWER_SHIFT), NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_SwitchAMUXchannel
 * Description : Switches a desired channel to the AMUX pin.
 *
 *END**************************************************************************/
fs65_status_t FS65_SwitchAMUXchannel(fs65_amux_selection_t channelSelection)
{
    /* Check of AMUX channel selection range. */
    if (!FS65_IS_IN_RANGE(channelSelection, fs65AmuxVref, fs65AmuxDieTempSensor))
    {
        return fs65StatusError;
    }
    return FS65_WriteRegister(FS65_M_IO_OUT_AMUX_ADDR, (uint16_t)channelSelection, NULL);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_SetRegulatorState
 * Description : Sets state (enable/disable) of the selected voltage regulator.
 *
 *END**************************************************************************/
fs65_status_t FS65_SetRegulatorState(fs65_reg_mode_t vreg, bool enable)
{
    fs65_status_t status;   /* Status variable. */
    uint8_t writeValue;     /* Register write value. */
    uint8_t address;        /* Register address. */
    uint8_t readMask;       /* Register read mask. */
    uint8_t writeMask;      /* Register write mask. */
    uint8_t writeShift;     /* Register write shift. */
    fs65_rx_data_t response;                            /* Received data. */
    uint8_t secureWriteShift = FS65_SECURE_WRITE_SHIFT; /* Shift for reg. write data. */
    bool readActualValue = true;                        /* If true, register actual value is read before write operation. */
    uint8_t enableValue = enable ? 1U : 0U;             /* Enable/disable regulator output. */

    switch (vreg)
    {
        case fs65VCan:
            address = FS65_M_REG_MODE_ADDR;
            readMask = FS65_R_M_VCAN_EN_MASK;
            writeMask = FS65_W_M_VCAN_EN_MASK;
            writeShift = FS65_W_M_VCAN_EN_SHIFT;
            break;

        case fs65Aux:
            address = FS65_M_REG_MODE_ADDR;
            readMask = FS65_R_M_VAUX_EN_MASK;
            writeMask = FS65_W_M_VAUX_EN_MASK;
            writeShift = FS65_W_M_VAUX_EN_SHIFT;
            break;

        case fs65Vcca:
            address = FS65_M_REG_MODE_ADDR;
            readMask = FS65_R_M_VCCA_EN_MASK;
            writeMask = FS65_W_M_VCCA_EN_MASK;
            writeShift = FS65_W_M_VCCA_EN_SHIFT;
            break;

        case fs65Vcore:
            address = FS65_M_REG_MODE_ADDR;
            readMask = FS65_R_M_VCORE_EN_MASK;
            writeMask = FS65_W_M_VCORE_EN_MASK;
            writeShift = FS65_W_M_VCORE_EN_SHIFT;
            break;

        case fs65Vkam:
            address = FS65_M_MODE_ADDR;
            readMask = writeMask = FS65_RW_M_VKAM_EN_MASK;
            writeShift = FS65_RW_M_VKAM_EN_SHIFT;
            /* Value is not shifted for M_MODE register. */
            secureWriteShift = 0;
            /* Vkam bit is the only non-volatile value that can be set by M_MODE register.
             * It has no sense to read content of this register before write. */
            readActualValue = false;
            break;

        default:
            return fs65StatusError;
    }

    if (readActualValue)
    {
        /* Read actual register value to preserve value of other bits. */
        status = FS65_ReadRegister(address, &response);
        if (status != fs65StatusOk)
        {
            return status;
        }
        /* Compose write value. */
        writeValue = FS65_BO_SETVAL((uint8_t)(response.readData << secureWriteShift),
                (uint8_t)(enableValue << writeShift), writeMask);
    }
    else
    {
        /* Write just the enable/disable bit (M_MODE register only). */
        writeValue = (uint8_t)(enableValue << writeShift);
    }

    status = FS65_WriteRegister(address, writeValue, NULL);

    /* Check if the value was accepted. */
    status |= FS65_ReadRegister(address, &response);
    if (status != fs65StatusOk ||
            (((response.readData & readMask) << secureWriteShift) != (writeValue & writeMask)))
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_GetFaultErrorCounterValue
 * Description : Reads actual Fault Error Counter value.
 *
 *END**************************************************************************/
fs65_status_t FS65_GetFaultErrorCounterValue(uint8_t* faultErrorCounterValue)
{
    fs65_status_t status;           /* Status value. */
    fs65_rx_data_t response;        /* Register received data. */

    FS_ASSERT(faultErrorCounterValue != NULL);

    status = FS65_ReadRegister(FS65_M_DIAG_SF_ERR_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    *faultErrorCounterValue = FS65_BO_GET_REG_VALUE(response.readData, FS65_R_M_FLT_ERR_MASK,
            FS65_R_M_FLT_ERR_SHIFT);

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_GetMode
 * Description : This function gets current and previous mode of the SBC.
 *
 *END**************************************************************************/
fs65_status_t FS65_GetMode(fs65_current_mode_t *currentMode, fs65_prev_mode_t *prevMode)
{
    fs65_status_t status = fs65StatusOk;   /* Status variable. */
    fs65_rx_data_t response;               /* Received data. */

    FS_ASSERT(currentMode != NULL);
    FS_ASSERT(prevMode != NULL);

    status = FS65_ReadRegister(FS65_M_MODE_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Parses response to get previous mode of the SBC. */
    switch (response.readData & (FS65_R_M_LPOFF_MASK | FS65_R_M_DFS_MASK))
    {
        case FS65_R_M_LPOFF_RESUME_LPOFF:
            *prevMode = fs65PrevModeLPOFF;
            break;
        case FS65_R_M_DFS_RESUME_DFS:
            *prevMode = fs65PrevModeDFS;
            break;
        default:
            *prevMode = fs65PrevModePOR;
            break;
    }

    /* Parses response to get current mode of the SBC. */
    switch (response.readData & (FS65_R_M_NORMAL_MASK | FS65_R_M_INIT_MASK))
    {
        case FS65_R_M_NORMAL_NORMAL:
            *currentMode = fs65ModeNormal;
            break;
        case FS65_R_M_INIT_INIT:
            *currentMode = fs65ModeInit;
            break;
        default:
            *currentMode = fs65ModeUnknown;
            break;
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_CheckVAUX
 * Description : This function checks if VAUX is safety critical and optionally
 *               runs related diagnostics.
 *
 *END**************************************************************************/
fs65_status_t FS65_CheckVAUX(void)
{
    fs65_status_t status = fs65StatusOk;   /* Status variable. */
    fs65_rx_data_t response;               /* Response to the command. */

    /* Check ABIST2_VAUX if necessary. */
    status = FS65_ReadRegister(FS65_FS_INIT_VAUX_OVUV_IMPACT_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    if (((response.readData & FS65_R_FS_VAUX_FS_OV_MASK) != FS65_R_FS_VAUX_FS_OV_NO_EFFECT)
            || ((response.readData & FS65_R_FS_VAUX_FS_UV_MASK) != FS65_R_FS_VAUX_FS_OV_NO_EFFECT))
    {
        /* Run ABIST2_VAUX. */
        status = FS65_WriteRegister(FS65_FS_BIST_ADDR, FS65_W_FS_ABIST2_VAUX_ABIST_VAUX, NULL);
        if (status != fs65StatusOk)
        {
            return status;
        }

        /* Wait for 200 us (see AN5238 document for details). */
        MCU_WaitUs(200);

        /* Check BIST for ABIST2_VAUX result. */
        status = FS65_ReadRegister(FS65_FS_BIST_ADDR, &response);
        if (status != fs65StatusOk)
        {
            return status;
        }

        if ((response.readData & FS65_R_FS_ABIST2_VAUX_OK_MASK) != FS65_R_FS_ABIST2_VAUX_OK_PASS)
        {
            return fs65StatusError;
        }
    }

    return status;
}

#ifdef FS65_FEATURE_FS1B
/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_CheckFS1B
 * Description : This function checks if FS1B has expected low level during
 *               initialization and runs related diagnostics.
 *
 *END**************************************************************************/
fs65_status_t FS65_CheckFS1B(void)
{
    fs65_status_t status = fs65StatusOk; /* Status variable. */
    fs65_rx_data_t response;             /* Response to the command. */

    status = FS65_ReadRegister(FS65_M_DEVICE_ID_FS_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check if FS1B is implemented. */
    if ((response.readData & FS65_R_M_FS1_MASK) == FS65_R_M_FS1_ENABLE)
    {
        status = FS65_ReadRegister(FS65_FS_RELEASE_FSXB_ADDR, &response);
        if (status != fs65StatusOk)
        {
            return status;
        }

        /* Check if FS1B is at high. */
        if ((response.readData & FS65_R_FS_FS1B_SNS_MASK) == FS65_R_FS_FS1B_SNS_HIGH)
        {
            status = FS65_ReadRegister(FS65_M_DIAG_SF_IOS_ADDR, &response);
            if (status != fs65StatusOk)
            {
                return status;
            }

            /* Check DIAG_SF_IOS for short-circuit to high. */
            if (((response.readData & FS65_R_M_FS1B_DIAG_MASK) == FS65_R_M_FS1B_DIAG_SC_LOW) ||
                ((response.readData & FS65_R_M_FS1B_DIAG_MASK) == FS65_R_M_FS1B_DIAG_SC_HIGH))
            {
                return fs65StatusError;
            }
        }

        /* Run ABIST2_FS1B. */
        status = FS65_WriteRegister(FS65_FS_BIST_ADDR, FS65_W_FS_ABIST2_FS1B_ABIST_FS1B, NULL);
        if (status != fs65StatusOk)
        {
            return status;
        }

        /* Wait 200 us (see AN5238 document for details). */
        MCU_WaitUs(200);

        /* Check BIST for ABIST2_FS1B result. */
        status = FS65_ReadRegister(FS65_FS_BIST_ADDR, &response);
        if (status != fs65StatusOk)
        {
            return status;
        }

        if ((response.readData & FS65_R_FS_ABIST2_FS1B_OK_MASK) !=
                FS65_R_FS_ABIST2_FS1B_OK_PASS)
        {
            return fs65StatusError;
        }
    }

    return status;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_ReleaseFSx
 * Description : This function releases selected fail-safe output as a part of
 *               error recovery procedure.
 *
 *END**************************************************************************/
fs65_status_t FS65_ReleaseFSx(fs65_fsxb_release_t fsOutput)
{
    fs65_status_t status = fs65StatusOk;    /* Status variable. */
    uint8_t writeData;                      /* Data to be written. */
    uint8_t wdSeed;                         /* Watchdog seed value. */

    FS_ASSERT((fsOutput == fs65ReleaseFs0b) || (fsOutput == fs65ReleaseFs1b) || \
            (fsOutput == fs65ReleaseFs0bFs1b));

#ifdef FS65_FEATURE_FS1B
    status = FS65_CheckFS1B();
    if (status != fs65StatusOk)
    {
        return status;
    }
#endif

    status = FS65_CheckVAUX();
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Get actual LFSR state. */
    status = FS65_WD_ReadChallengeToken(&wdSeed);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Prepare release word. */
    switch (fsOutput)
    {
        case fs65ReleaseFs0b:
            writeData = (uint8_t)fs65ReleaseFs0b;
            writeData |= (wdSeed & (1U << 4U) ? 0x00U : 0x01U);
            writeData |= (wdSeed & (1U << 3U) ? 0x00U : 0x02U);
            writeData |= (wdSeed & (1U << 2U) ? 0x00U : 0x04U);
            writeData |= (wdSeed & (1U << 1U) ? 0x00U : 0x08U);
            writeData |= (wdSeed & (1U << 0U) ? 0x00U : 0x10U);
            break;
#ifdef FS65_FEATURE_FS1B
        case fs65ReleaseFs1b:
            writeData = (uint8_t)fs65ReleaseFs1b;
            writeData |= (wdSeed & (1U << 7U) ? 0x00U : 0x01U);
            writeData |= (wdSeed & (1U << 6U) ? 0x00U : 0x02U);
            writeData |= (wdSeed & (1U << 5U) ? 0x00U : 0x04U);
            writeData |= (wdSeed & (1U << 4U) ? 0x00U : 0x08U);
            writeData |= (wdSeed & (1U << 3U) ? 0x00U : 0x10U);
            break;
        case fs65ReleaseFs0bFs1b:
            writeData = (uint8_t)fs65ReleaseFs0bFs1b;
            writeData |= (wdSeed & (1U << 7U) ? 0x00U : 0x01U);
            writeData |= (wdSeed & (1U << 6U) ? 0x00U : 0x02U);
            writeData |= (wdSeed & (1U << 2U) ? 0x00U : 0x04U);
            writeData |= (wdSeed & (1U << 1U) ? 0x00U : 0x08U);
            writeData |= (wdSeed & (1U << 0U) ? 0x00U : 0x10U);
            break;
#endif
        default:
            return fs65StatusError;
    }

    return FS65_WriteRegister(FS65_FS_RELEASE_FSXB_ADDR, writeData, NULL);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_CheckLbistAbistOk
 * Description : Checks if the ABIST1 and LBIST diagnostics passed. If not,
 *               the function returns error code.
 *
 *END**************************************************************************/
fs65_status_t FS65_CheckLbistAbistOk(void)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */

    /* Check BIST for LBIST & ABIST1 completion. */
    status = FS65_ReadRegister(FS65_FS_BIST_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    if ((response.readData & FS65_R_FS_ABIST1_OK_MASK) != FS65_R_FS_ABIST1_OK_PASS)
    {
        return fs65StatusError;
    }

    if ((response.readData & FS65_R_FS_LBIST_OK_MASK) != FS65_R_FS_LBIST_OK_PASS)
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_SetLowPowerMode
 * Description : This function switches mode of the SBC to the LPOFF,
 *               optionally with automatic wake-up 1ms after transition.
 *
 *END**************************************************************************/
fs65_status_t FS65_SetLowPowerMode(bool autoWU)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t  writeData;         /* Data to be updated. */

    /* Read VKAM control state to preserve it. */
    status = FS65_ReadRegister(FS65_M_MODE_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Set previously read VKAM value to write data. */
    writeData = (response.readData & FS65_RW_M_VKAM_EN_MASK) | FS65_W_M_GO_LPOFF_LPOFF;

    /* Set LPOFF mode. */
    writeData |= autoWU ? FS65_W_M_LPOFF_AUTO_WU_LPOFF : FS65_W_M_GO_LPOFF_LPOFF;

    return FS65_WriteRegister(FS65_M_MODE_ADDR, writeData, NULL);

    /* Write check is not performed because of entering LPOFF mode. */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_RequestInterrupt
 * Description : This function requests an interrupt (pulse on the INT pin).
 *
 *END**************************************************************************/
fs65_status_t FS65_RequestInterrupt(void)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Register write data. */

    /* Prepare content. */
    status = FS65_ReadRegister(FS65_M_MODE_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    /* Save VKAM value and set INT_REQ bit. */
    writeData = (response.readData & FS65_RW_M_VKAM_EN_MASK) | FS65_W_M_INT_REQ_INT_REQ;

    return FS65_WriteRegister(FS65_M_MODE_ADDR, writeData, NULL);

    /* Write check is not performed because written value cannot be read out. */
}

#ifdef FS65_FEATURE_CAN
/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_CAN_SetMode
 * Description : This function changes CAN mode and the automatic transition
 *               of the CAN transceiver to the low-power mode on specific events.
 *
 *END**************************************************************************/
fs65_status_t FS65_CAN_SetMode(fs65_can_mode_t mode, bool autoDis)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Read current register value. */
    status = FS65_ReadRegister(FS65_M_CAN_LIN_MODE_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Update register. */
    writeData = (response.readData & (FS65_RW_M_LIN_AUTO_DIS_MASK | FS65_RW_M_LIN_MODE_MASK)) |
            (uint8_t)mode | (autoDis ? FS65_RW_M_CAN_AUTO_DIS_RESET : FS65_RW_M_CAN_AUTO_DIS_NO);

    status = FS65_WriteRegister(FS65_M_CAN_LIN_MODE_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check written data. */
    status = FS65_ReadRegister(FS65_M_CAN_LIN_MODE_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if ((response.readData & (FS65_RW_M_CAN_AUTO_DIS_MASK | FS65_RW_M_CAN_MODE_MASK)) !=
            ((uint8_t)mode | (autoDis ? FS65_RW_M_CAN_AUTO_DIS_RESET : FS65_RW_M_CAN_AUTO_DIS_NO)))
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}
#endif

#ifdef FS65_FEATURE_LIN
/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_LIN_SetMode
 * Description : This function changes LIN mode and the automatic transition
 *               of the LIN transceiver to the low-power mode on specific events.
 *
 *END**************************************************************************/
fs65_status_t FS65_LIN_SetMode(fs65_lin_mode_t mode, bool autoDis)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Read current register value. */
    status = FS65_ReadRegister(FS65_M_CAN_LIN_MODE_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Update register. */
    writeData = (response.readData & (FS65_RW_M_CAN_AUTO_DIS_MASK | FS65_RW_M_CAN_MODE_MASK)) |
            (uint8_t)mode | (autoDis ? FS65_RW_M_LIN_AUTO_DIS_RESET : FS65_RW_M_LIN_AUTO_DIS_NO);

    status = FS65_WriteRegister(FS65_M_CAN_LIN_MODE_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check written data. */
    status = FS65_ReadRegister(FS65_M_CAN_LIN_MODE_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if ((response.readData & (FS65_RW_M_LIN_AUTO_DIS_MASK | FS65_RW_M_LIN_MODE_MASK)) !=
            ((uint8_t)mode | (autoDis ? FS65_RW_M_LIN_AUTO_DIS_RESET : FS65_RW_M_LIN_AUTO_DIS_NO)))
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}
#endif

/*******************************************************************************
 * Code - public functions - LDT (long duration timer)
 ******************************************************************************/

#ifdef FS65_FEATURE_LDT
/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_LDT_SetTmrOp
 * Description : This function sets operating function of the LDT.
 *
 *END**************************************************************************/
fs65_status_t FS65_LDT_SetTimerOperation(fs65_ldt_function_t op)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Read current register value. */
    status = FS65_ReadRegister(FS65_M_LONG_DURATION_TIMER_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Update register. */
    writeData = (response.readData & (FS65_RW_M_LDT_ENABLE_MASK | FS65_RW_M_MODE_MASK |
            FS65_RW_M_REG_SE_MASK)) | (uint8_t)op;

    status = FS65_WriteRegister(FS65_M_LONG_DURATION_TIMER_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check written data. */
    status = FS65_ReadRegister(FS65_M_LONG_DURATION_TIMER_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if ((response.readData & FS65_RW_M_F2_F0_MASK) != (uint8_t)op)
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_LDT_SetTmrMode
 * Description : This function sets mode of the LDT (normal/calibration).
 *
 *END**************************************************************************/
fs65_status_t FS65_LDT_SetTimerMode(fs65_ldt_mode_t mode)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Read current register value. */
    status = FS65_ReadRegister(FS65_M_LONG_DURATION_TIMER_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Update register. */
    writeData = (response.readData & (FS65_RW_M_LDT_ENABLE_MASK | FS65_RW_M_REG_SE_MASK |
            FS65_RW_M_F2_F0_MASK)) | (uint8_t)mode;

    status = FS65_WriteRegister(FS65_M_LONG_DURATION_TIMER_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check written data. */
    status = FS65_ReadRegister(FS65_M_LONG_DURATION_TIMER_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if ((response.readData & FS65_RW_M_MODE_MASK) != (uint8_t)mode)
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_LDT_SetWakeUpRegSrc
 * Description : This function sets counter to read real-time counter or
 *               programmed value into wake-up register.
 *
 *END**************************************************************************/
fs65_status_t FS65_LDT_SetWakeUpRegSrc(fs65_ldt_wu_scr_t source)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Read current register value. */
    status = FS65_ReadRegister(FS65_M_LONG_DURATION_TIMER_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Update register. */
    writeData = (response.readData & (FS65_RW_M_LDT_ENABLE_MASK | FS65_RW_M_MODE_MASK |
            FS65_RW_M_F2_F0_MASK)) | (uint8_t)source;

    status = FS65_WriteRegister(FS65_M_LONG_DURATION_TIMER_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check written data. */
    status = FS65_ReadRegister(FS65_M_LONG_DURATION_TIMER_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if ((response.readData & FS65_RW_M_REG_SE_MASK) != (uint8_t)source)
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_LDT_RunCounter
 * Description : This function starts or stops the LDT counter.
 *
 *END**************************************************************************/
fs65_status_t FS65_LDT_RunCounter(bool run)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Read current register value. */
    status = FS65_ReadRegister(FS65_M_LONG_DURATION_TIMER_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Update register. */
    writeData = (response.readData & (FS65_RW_M_MODE_MASK | FS65_RW_M_REG_SE_MASK |
            FS65_RW_M_F2_F0_MASK)) | (run ? FS65_RW_M_LDT_ENABLE_START : FS65_RW_M_LDT_ENABLE_STOP);

    status = FS65_WriteRegister(FS65_M_LONG_DURATION_TIMER_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check written data. */
    status = FS65_ReadRegister(FS65_M_LONG_DURATION_TIMER_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if ((response.readData & FS65_RW_M_LDT_ENABLE_MASK) !=
            (run ? FS65_RW_M_LDT_ENABLE_START : FS65_RW_M_LDT_ENABLE_STOP))
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_LDT_SetAfterRunValue
 * Description : This function sets new after-run value for the LDT.
 *
 *END**************************************************************************/
fs65_status_t FS65_LDT_SetAfterRunValue(uint16_t value)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Set high byte. */
    writeData = (uint8_t)((value >> 8) & 0xFFU);

    status = FS65_WriteRegister(FS65_M_LDT_AFTER_RUN_1_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    status = FS65_ReadRegister(FS65_M_LDT_AFTER_RUN_1_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if (response.readData != writeData)
    {
        return fs65StatusError;
    }

    /* Set low byte. */
    writeData = (uint8_t)(value & 0xFFU);

    status = FS65_WriteRegister(FS65_M_LDT_AFTER_RUN_2_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    status = FS65_ReadRegister(FS65_M_LDT_AFTER_RUN_2_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if (response.readData != writeData)
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_LDT_SetWakeUpValue
 * Description : This function sets new wake-up value for the LDT.
 *
 *END**************************************************************************/
fs65_status_t FS65_LDT_SetWakeUpValue(uint32_t value)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Set high byte. */
    writeData = (uint8_t)((value >> 16) & 0xFFU);

    status = FS65_WriteRegister(FS65_M_LDT_WAKE_UP_1_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    status = FS65_ReadRegister(FS65_M_LDT_WAKE_UP_1_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if (response.readData != writeData)
    {
        return fs65StatusError;
    }

    /* Set middle byte. */
    writeData = (uint8_t)((value >> 8) & 0xFFU);

    status = FS65_WriteRegister(FS65_M_LDT_WAKE_UP_2_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    status = FS65_ReadRegister(FS65_M_LDT_WAKE_UP_2_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if (response.readData != writeData)
    {
        return fs65StatusError;
    }

    /* Set low byte. */
    writeData = (uint8_t)(value & 0xFFU);

    status = FS65_WriteRegister(FS65_M_LDT_WAKE_UP_3_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    status = FS65_ReadRegister(FS65_M_LDT_WAKE_UP_3_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }
    if (response.readData != writeData)
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}
#endif

/*******************************************************************************
 * Code - public functions - safety related
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_WD_ChangeWindow
 * Description : This function changes duration of watchdog window.
 *
 *END**************************************************************************/
fs65_status_t FS65_WD_ChangeWindow(uint8_t windowDuration)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the read command. */

    FS_ASSERT((windowDuration & (~((uint8_t)FS65_W_FS_WD_WINDOW_MASK))) == 0x00);

    /* Write the WD Window register. */
    status = FS65_WriteRegister(FS65_FS_WD_WINDOW_ADDR, windowDuration, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check the written data. */
    status = FS65_ReadRegister(FS65_FS_WD_WINDOW_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    if ((response.readData & FS65_R_FS_WD_WINDOW_MASK) != (windowDuration >> FS65_W_FS_WD_WINDOW_SHIFT))
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_WD_ChangeSeed
 * Description : This function changes seed of LFSR used for watchdog.
 *
 *END**************************************************************************/
fs65_status_t FS65_WD_ChangeSeed(uint8_t wdSeed)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the read command. */

    FS_ASSERT(wdSeed != 0U); /* This value is not allowed. */

    /* Write the WD seed. */
    status =  FS65_WriteRegister(FS65_FS_WD_LFSR_ADDR, \
            (uint8_t)(wdSeed << FS65_RW_FS_WD_LFSR_SHIFT), NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check the written WD seed. */
    status = FS65_ReadRegister(FS65_FS_WD_LFSR_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    if ((response.readData & FS65_RW_FS_WD_LFSR_MASK) !=
            (uint8_t)(wdSeed << FS65_RW_FS_WD_LFSR_SHIFT))
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_RequestReset
 * Description : This function requests a low pulse on the RSTB (MCU reset).
 *
 *END**************************************************************************/
fs65_status_t FS65_RequestReset(void)
{
    return FS65_WriteRegister(FS65_FS_SF_OUTPUT_REQUEST_ADDR, \
            FS65_W_FS_RSTB_REQ_RSTB_REQ, NULL);

    /* Write check is not performed because written value cannot be read out. */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_RequestFSxLow
 * Description : This function requests a low level on the selected fail-safe output.
 *
 *END**************************************************************************/
fs65_status_t FS65_RequestFSxLow(fs65_fsx_req_type_t fsxSelection)
{
    return FS65_WriteRegister(FS65_FS_SF_OUTPUT_REQUEST_ADDR, (uint8_t)fsxSelection, NULL);

    /* Write check is not performed because written value cannot be read out. */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_SetOUT4
 * Description : This function sets level of the IO_4 when configured
 *               as an output.
 *
 *END**************************************************************************/
fs65_status_t FS65_SetOUT4(bool level)
{
    fs65_status_t status;       /* Status variable. */
    fs65_rx_data_t response;    /* Response to the command. */
    uint8_t writeData;          /* Data to be written. */

    /* Read current register data. */
    status = FS65_ReadRegister(FS65_M_IO_OUT_AMUX_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Prepare content. */
    writeData = (response.readData & FS65_RW_M_AMUX_MASK) |
            (((uint8_t) level) << FS65_RW_M_IO_OUT_4_SHIFT) | FS65_RW_M_IO_OUT_4_EN_ENABLED;

    /* Update register. */
    status = FS65_WriteRegister(FS65_M_IO_OUT_AMUX_ADDR, writeData, NULL);
    if (status != fs65StatusOk)
    {
        return status;
    }

    /* Check written data. */
    status = FS65_ReadRegister(FS65_M_IO_OUT_AMUX_ADDR, &response);
    if (status != fs65StatusOk)
    {
        return status;
    }

    if ((response.readData & (FS65_RW_M_IO_OUT_4_MASK | FS65_RW_M_IO_OUT_4_EN_MASK)) !=
            ((((uint8_t) level) << FS65_RW_M_IO_OUT_4_SHIFT) | FS65_RW_M_IO_OUT_4_EN_ENABLED))
    {
        return fs65StatusError;
    }

    return fs65StatusOk;
}
