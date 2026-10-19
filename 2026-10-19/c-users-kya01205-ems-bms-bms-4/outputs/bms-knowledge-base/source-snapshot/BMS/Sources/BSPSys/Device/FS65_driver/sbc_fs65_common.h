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

/** @file sbc_fs65_common.h
 * @brief Driver common structures, enums, macros and configuration values.
 *
 * This header file also contains prototypes of functions that have to be
 * implemented by the user application (MCU_ prefix).
 *
 * @author nxf44615
 * @version 1.0
 * @date 13-Nov-2018
 * @copyright Copyright 2016 - 2018 NXP
 */

#ifndef SBC_FS65_COMMON_H__
#define SBC_FS65_COMMON_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @defgroup Features Defines for SBC features
 *
 * @note If some feature is not supported by the SBC, it can be disabled by
 *       the particular define renaming/deletion.
 * @{ */

/** @brief FS1B functionality. */
#define FS65_FEATURE_FS1B

/** @brief CAN functionality */
#define FS65_FEATURE_CAN

/** @brief LIN functionality */
#define FS65_FEATURE_LIN

/** @brief LDT functionality */
#define FS65_FEATURE_LDT

/** @}*/

/** @defgroup EnumsDefs Enums definition
 * @{ */
/** @brief FSxb pin release options. */
typedef enum
{
    fs65ReleaseFs0b = 0x60,     /**< @brief Release FS0b pin only. */
    fs65ReleaseFs1b = 0xC0,     /**< @brief Release FS1b pin only. */
    fs65ReleaseFs0bFs1b = 0xA0  /**< @brief Release both FS0b and FS1b pins. */
} fs65_fsxb_release_t;

/** @name Enums related to functions used to handle SBC mode. */
/** @{*/

/** @brief Previous SBC state. */
typedef enum
{
    fs65PrevModePOR,     /**< @brief Previous SBC mode was POR (power on reset). */
    fs65PrevModeDFS,     /**< @brief Resume from deep fail-safe mode. */
    fs65PrevModeLPOFF    /**< @brief Resume from LPOFF mode. */
} fs65_prev_mode_t;

/** @brief Actual SBC state. */
typedef enum
{
    fs65ModeUnknown,    /**< @brief Current SBC mode is unknown. */
    fs65ModeInit,       /**< @brief Current SBC mode is INIT. */
    fs65ModeNormal      /**< @brief Current SBC mode is NORMAL. */
} fs65_current_mode_t;
/** @}*/

/** @brief Status return codes. */
typedef enum
{
    fs65StatusOk    = 0U,   /**< @brief No error. */
    fs65StatusError = 1U    /**< @brief Error. */
} fs65_status_t;

/** @brief Command type. */
typedef enum
{
    fs65RegRead,    /**< @brief Register Read */
    fs65RegWrite    /**< @brief Register Write */
} fs65_command_t;

/** @brief Parity type. */
typedef enum
{
    fs65ParityOdd,  /**< @brief Number of 1s is odd. */
    fs65ParityEven, /**< @brief Number of 1s is even. */
} fs65_parity_t;
/** @} */

/** @defgroup StructDefs Struct definitions
 * @{ */
/** @brief Structure representing transmit data frame. */
typedef struct
{
    /** @brief Main/Fail Safe register selection. */
    bool isFailSafe;

    /** @brief true if the SPI command is secured. */
    bool isSecured;

    /** @brief Register address. */
    uint8_t registerAddress;

    /** @brief Command type (R/W). */
    fs65_command_t commandType;

    /** @brief Data to be written to the register.
     *
     * If commandType is "read", this value will be ignored.
     */
    uint8_t writeData;

} fs65_tx_data_t;

/** @brief Structure representing received data frame. */
typedef struct
{
    /** @brief A device status is returned into this byte after a successful transfer. */
    uint8_t deviceStatus;

    /** @brief Extended diagnostics. Sent by Main or Fail-safe if secured register is accessed.
     *
     * Diagnostics content is saved in 4 highest bits. */
    uint8_t deviceStatusEx;

    /** @brief Content of a read register. */
    uint8_t readData;

} fs65_rx_data_t;

/**
 * @brief Structure representing configuration value of one register.
 */
typedef struct
{
    uint8_t address;    /**< @brief Register address */

    uint8_t value;      /**< @brief Value of the register. */

    uint8_t readMask;   /**< @brief Mask used for register read value check. */

    bool isSecured;     /**< @brief True if register uses secure bits. */

} fs65_reg_config_value_t;

/**
 * @brief Structure for FS65 user configuration.
 *
 * This structure is used as a parameter for @ref FS65_Init() function.
 *
 * @note If any of the struct member is set to NULL (pointer values),
 *       such a member will be ignored and no write operation will be
 *       performed for this group of registers during the INIT phase.
 */
typedef struct
{
    /**
     * @brief INIT main registers.
     *
     * Array of register configuration values for INIT main registers.
     * Following registers can be configured:
     *  - INIT_VREG
     *  - INIT_WU1
     *  - INIT_WU2
     *  - INIT_INH_INT
     *
     * @note INIT_INT register should not be present in this array as its writing causes
     *       transition from INIT MAIN to NORMAL MODE. INIT_INT register value can be set
     *       by initIntReg value (part of this structure).
     */
    fs65_reg_config_value_t* initMainRegs;
    uint8_t initMainRegsCount;                  /**< @brief Number of INIT main registers. */

    /**
     * @brief INIT_FS registers.
     *
     * Array of register configuration values for INIT FS registers.
     * Following registers can be configured:
     *  - INIT_FS1B_TIMING
     *  - INIT_SUPERVISOR
     *  - INIT_FAULT
     *  - INIT_FSSM
     *  - INIT_SF_IMPACT
     *  - INIT_WD_CNT
     *  - INIT_VCORE_OVUV_IMPACT
     *  - INIT_VCCA_OVUV_IMPACT
     *  - INIT_VAUX_OVUV_IMPACT
     */
    fs65_reg_config_value_t* initFailSafeRegs;
    uint8_t initFailSafeRegsCount;              /**< @brief Number of INIT_FS registers. */

    /**
     * @brief Non-init registers.
     *
     * Configuration of the rest of the registers.
     */
    fs65_reg_config_value_t* nonInitRegs;
    uint8_t nonInitRegsCount;                   /**< @brief Number of non-init registers. */

    /**
     * @brief Pointer to INIT_INT register value.
     *
     * If NULL, actual register value is read and written back.
     */
    uint8_t* initIntReg;
} fs65_user_config_t;
/** @} */

/** @name Bitwise operations used by this driver. */
/** @{ */
/** @brief This macro updates value of bits specified by the mask. It is assumed that
 *  value is already shifted. Write value is masked also. */
#define FS65_BO_SETVAL(data, val, mask)   (((data) & ~(mask)) | ((val) & (mask)))

/** @brief This macro updates value of bits specified by the mask. Additionally range
 *  check on the value is performed. It is assumed that value is not shifted. */
#define FS65_BO_SETVAL_EXT(data, value, mask, shift) \
    (((data) & ~(mask << shift)) | (((value) & (mask)) << (shift)))

/** @brief This macro returns value specified by the mask. */
#define FS65_BO_GETVAL(data, mask, shift) ((data) & (mask) << (shift))

/**
 * @brief Macro for getting value from register.
 *
 * @param value Value read from register.
 * @param mask Bit selection.
 * @param shift Bit shift.
 * @returns Masked and r-shifted value. */
#define FS65_BO_GET_REG_VALUE(value, mask, shift) (((value) & (mask)) >> (shift))
/** @} */

/**
 * @brief Returns true if register is fail-safe
 *
 * @param address   Register address.
 * @return true if register is fail-safe.
 */
#define FS65_IS_REG_FAILSAFE(address) ((address) & 0x20)

/*******************************************************************************
 * Prototypes of extern functions
 ******************************************************************************/
/**
 * @defgroup Extern MCU specific functions
 * @attention Functions in this group must be implemented by the user.
 * @{ */

/**
 * @brief This function transfers single frame through blocking SPI communication
 *         in both directions. MCU specific.
 *
 * This function must be implemented if SPI communication is used.
 * @warning This function must be implemented as blocking as there is not synchronization
 * mechanism implemented in the driver.
 * @param [in]  txFrame             Frame to be send.
 * @param [out] rxFrame             Received frame.
 * @return @ref fs65_status_t Status return code. */
extern fs65_status_t MCU_SPI_TransferData(uint8_t* txFrame, uint8_t* rxFrame);

/**
 * @brief This function waits for specified amount of microseconds.
 *
 * @param [in] delay Delay in microseconds.
 */
extern void MCU_WaitUs(uint16_t delay);

/** @} */

#endif /* SBC_FS65_COMMON_H__ */
