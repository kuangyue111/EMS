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

/** @file sbc_fs65_communication.c
 * @brief Implementation of communication logic for NXP SBC FS65/FS45.
 *
 * @author nxf44615
 * @version 1.0
 * @date 13-Nov-2018
 * @copyright Copyright 2016 - 2018 NXP
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "sbc_fs65_communication.h"
#include "sbc_fs65_map.h"
#include "sbc_fs65_assert.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Data frame (SPI). */
#define FS65_REG_ADDR_MASK  0x3FU   /* Register address mask (not shifted) */
#define FS65_REG_ADDR_SHIFT 0x01U   /* SPI register address shift */

#define FS65_SPI_BIT_7_0    0U
#define FS65_SPI_BIT_15_8   1U
#define FS65_SPI_RW_BIT_MASK 0x80U
#define FS65_SPI_W_BIT_VALUE 0x80U
#define FS65_SPI_R_BIT_VALUE 0

#define FS65_SPI_DEVICE_STATUS_EX_MASK 0xF0U
#define FS65_SPI_FS_REGISTER_DATA_MASK 0x0FU

#define FS65_FAIL_SAFE_REGS_COUNT 14U

/*******************************************************************************
 * Constants
 ******************************************************************************/
/**
 * @brief List of secured register addresses.
 */
static const uint8_t m_securedRegisters[] = {
    FS65_M_MODE_ADDR,
    FS65_M_REG_MODE_ADDR,
    FS65_FS_INIT_FS1B_TIMING_ADDR,
    FS65_FS_BIST_ADDR,
    FS65_FS_INIT_SUPERVISOR_ADDR,
    FS65_FS_INIT_FAULT_ADDR,
    FS65_FS_INIT_FSSM_ADDR,
    FS65_FS_INIT_SF_IMPACT_ADDR,
    FS65_FS_WD_WINDOW_ADDR,
    FS65_FS_SF_OUTPUT_REQUEST_ADDR,
    FS65_FS_INIT_WD_CNT_ADDR,
    FS65_FS_INIT_VCORE_OVUV_IMPACT_ADDR,
    FS65_FS_INIT_VCCA_OVUV_IMPACT_ADDR,
    FS65_FS_INIT_VAUX_OVUV_IMPACT_ADDR
};

/*******************************************************************************
 * Local Functions Prototypes
 ******************************************************************************/
/**
 * @brief Returns true if register SPI transfer is secured by security bits.
 *
 * @param address Register address.
 * @return true if register SPI transfer is secured.
 */
static inline bool FS65_IsRegisterSecured(uint8_t address);

/**
 * @brief This function calculates parity value of passed data array.
 *        Returns 1 if the parity is even, 0 otherwise.
 *
 * @param data Computes parity of input data.
 * @return Parity.
 */
static fs65_parity_t FS65_ComputeParity(uint16_t data);

/**
 * @brief Performs SPI transfer of the txData. Received frame is saved into
 *        rxData structure or ignored if rxData is NULL.
 *
 * @param txData Data to be transmitter (MOSI).
 * @param rxData Data received (MISO).
 * @return @ref fs65_status_t Status return code.
 */
static fs65_status_t FS65_TransferData(fs65_tx_data_t* txData, fs65_rx_data_t* rxData);

/**
 * @brief Creates a raw frame for SPI transfer.
 *
 * @param txData tx data.
 * @param txFrame Raw frame prepared for SPI transmission (MOSI).
 */
static void FS65_CreateSendFrame(fs65_tx_data_t* txData, uint8_t* txFrame);

/**
 * @brief Computes security bits from 4 MSbits and write it to 4 LSbits.
 *
 * @param [in, out] data
 */
static void FS65_ComputeSecurityBits(uint8_t* data);

/*******************************************************************************
 * Local Variables
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_IsRegisterSecured
 * Description : Returns true if register SPI transfer is secured by security bits.
 *
 *END**************************************************************************/
static inline bool FS65_IsRegisterSecured(uint8_t address)
{
    uint8_t i; /* Counter. */

    for (i = 0; i < FS65_FAIL_SAFE_REGS_COUNT; i++)
    {
        if (address == m_securedRegisters[i])
        {
            return true;
        }
    }
    return false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_TransferData
 * Description : Performs SPI transfer of the txData. Received frame is saved into
 *               rxData structure or ignored if rxData is NULL.
 *
 *END**************************************************************************/
static fs65_status_t FS65_TransferData(fs65_tx_data_t* txData, fs65_rx_data_t* rxData)
{
    fs65_status_t status;   /* Status variable. */
    uint8_t txFrame[FS65_COMM_FRAME_SIZE_BYTES] = {0};  /* SPI Tx frame. */
    uint8_t rxFrame[FS65_COMM_FRAME_SIZE_BYTES] = {0};  /* SPI Rx frame. */

    FS_ASSERT(txData != NULL);

    FS65_CreateSendFrame(txData, txFrame);
    status = MCU_SPI_TransferData(txFrame, rxFrame);
    if (status != fs65StatusOk)
    {
        return status;
    }

    if (rxData != NULL)
    {
        if (txData->isSecured)
        {
            rxData->deviceStatusEx = rxFrame[FS65_SPI_BIT_7_0] & FS65_SPI_DEVICE_STATUS_EX_MASK;
            rxData->readData = rxFrame[FS65_SPI_BIT_7_0] & FS65_SPI_FS_REGISTER_DATA_MASK;
        }
        else
        {
            rxData->deviceStatusEx = 0;
            rxData->readData = rxFrame[FS65_SPI_BIT_7_0];
        }
        rxData->deviceStatus = rxFrame[FS65_SPI_BIT_15_8];
    }
    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_CreateSendFrame
 * Description : Creates a raw frame for SPI transfer.
 *
 *END**************************************************************************/
static void FS65_CreateSendFrame(fs65_tx_data_t* txData, uint8_t* txFrame)
{
    fs65_parity_t parity; /* Tx frame parity result. */

    FS_ASSERT(txData != NULL);
    FS_ASSERT(txFrame != NULL);

    txFrame[FS65_SPI_BIT_7_0] = 0;
    txFrame[FS65_SPI_BIT_15_8] = 0;

    /* Sets address of the register. */
    txFrame[FS65_SPI_BIT_15_8] = FS65_BO_SETVAL_EXT(txFrame[FS65_SPI_BIT_15_8],
            txData->registerAddress, FS65_REG_ADDR_MASK, FS65_REG_ADDR_SHIFT);

    /* check if register is r or w */
    switch (txData->commandType)
    {
      case fs65RegRead:
          /* Creates read command. */
          txFrame[FS65_SPI_BIT_15_8] = FS65_BO_SETVAL(txFrame[FS65_SPI_BIT_15_8],
                  FS65_SPI_R_BIT_VALUE, FS65_SPI_RW_BIT_MASK);
          /* No data to be sent. */
          txFrame[FS65_SPI_BIT_7_0] = 0x00;
          break;

      case fs65RegWrite:
          /* Creates write command. */
          txFrame[FS65_SPI_BIT_15_8] |= FS65_BO_SETVAL(txFrame[FS65_SPI_BIT_15_8],
                  FS65_SPI_W_BIT_VALUE, FS65_SPI_RW_BIT_MASK);
          /* Sets data. */
          txFrame[FS65_SPI_BIT_7_0] = txData->writeData;
          if (txData->isSecured)
          {
              FS65_ComputeSecurityBits(&(txFrame[FS65_SPI_BIT_7_0]));
          }
          break;
    }

    /* Sets parity bit (just for the write command). */
    if (txData->commandType == fs65RegWrite)
    {
        parity = FS65_ComputeParity((txFrame[FS65_SPI_BIT_15_8] << 8) + txFrame[FS65_SPI_BIT_7_0]);
        if (parity == fs65ParityEven)
        {
            txFrame[FS65_SPI_BIT_15_8] = FS65_BO_SETVAL(txFrame[FS65_SPI_BIT_15_8], 1U, 1U);
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_ComputeSecurityBits
 * Description : Computes security bits from 4 MSbits and write it to 4 LSbits.
 *
 *END**************************************************************************/
static void FS65_ComputeSecurityBits(uint8_t* data)
{
    /* Set 4 LSbits to 0. */
    *data &= 0xF0;

    *data |= (*data & (1U << 6U) ? 1U : 0U); /* Secure 0 = Bit6 */
    *data |= (*data & (1U << 7U) ? 2U : 0U); /* Secure 1 = Bit7 */
    *data |= (*data & (1U << 4U) ? 0U : 4U); /* Secure 2 = NOT(Bit4) */
    *data |= (*data & (1U << 5U) ? 0U : 8U); /* Secure 3 = NOT(Bit5) */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_ComputeParity
 * Description : This function calculates parity value of passed data array.
 *               Returns 1 if the parity is even, 0 otherwise.
 *
 *END**************************************************************************/
static fs65_parity_t FS65_ComputeParity(uint16_t data)
{
    uint16_t x = data; /* Data from which the parity will be calculated. */

    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;

    return ((~x) & 1U) ? fs65ParityEven : fs65ParityOdd;
}

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_ReadRegister
 * Description : Performs a single read register based on provided address.
 *               The response is returned in @ref fs65_rx_frame_t structure.
 *
 *END**************************************************************************/
fs65_status_t FS65_ReadRegister(uint8_t address, fs65_rx_data_t* rxData)
{
    fs65_tx_data_t txData = {
        .registerAddress = address,
        .commandType = fs65RegRead
    };

    FS_ASSERT(rxData != NULL);

    /* Selects register set (main x fail-safe). */
    txData.isFailSafe = FS65_IS_REG_FAILSAFE(address);

    return FS65_TransferData(&txData, rxData);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_WriteRegister
 * Description : Sends write command to the FS65.
 *
 *END**************************************************************************/
fs65_status_t FS65_WriteRegister(uint8_t address, uint8_t writeData, fs65_rx_data_t* rxData)
{
    fs65_tx_data_t txData = {
        .registerAddress = address,
        .writeData = writeData,
        .commandType = fs65RegWrite
    };

    txData.isFailSafe = FS65_IS_REG_FAILSAFE(address);
    txData.isSecured = FS65_IsRegisterSecured(address);

    return FS65_TransferData(&txData, rxData);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_WriteRegisters
 * Description : Writes set of configuration registers values.
 *
 *END**************************************************************************/
fs65_status_t FS65_WriteRegisters(fs65_reg_config_value_t* registers, uint8_t numOfItems)
{
    fs65_status_t status;           /* Status variable. */
    fs65_rx_data_t response;        /* Response to the command. */
    uint8_t i;                      /* Counter. */
    fs65_reg_config_value_t* reg;   /* Pointer to actually selected register configuration. */
    uint8_t writeValue;             /* Register write value. */
    uint8_t readValue;              /* Register read value. */

    if (registers == NULL || numOfItems == 0)
    {
        return fs65StatusOk;
    }

    for (i = 0; i < numOfItems; i++)
    {
        reg = registers + i;
        /* Secured registers have the write value shifted and 4 lowest bits
         * are used for the Secured bits. */
        writeValue = reg->isSecured ? (uint8_t)(reg->value << 4) : reg->value;
        status = FS65_WriteRegister(reg->address, writeValue, NULL);

        /* In case of write to the fail-safe register, add mandatory delay between
         * consequent transfers (3.5 us according to the specification). */
        if (FS65_IS_REG_FAILSAFE(reg->address))
        {
            MCU_WaitUs(4);
        }

        /* Check written register value. */
        status |= FS65_ReadRegister(reg->address, &response);
        /* Secured registers have just 4 writable bits. */
        readValue = reg->isSecured ? (response.readData & 0x0F) : response.readData;

        /* Read mask should be used for read value as some registers
         * contains status bits, that are not writable (e.g. INIT_VREG - BAT_FAIL bit) */
        if (status != fs65StatusOk || (readValue & reg->readMask) != reg->value)
        {
            return fs65StatusError;
        }
    }
    return fs65StatusOk;
}
