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
 * @file sbc_fs65_communication.h
 * @brief This file contains functions for SPI communication.
 *
 * @author nxf44615
 * @version 1.0
 * @date 13-Nov-2018
 * @copyright Copyright 2016 - 2018 NXP
 */

#ifndef SBC_FS65_COMMUNICATION_H__
#define SBC_FS65_COMMUNICATION_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "sbc_fs65_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FS65_COMM_FRAME_SIZE_BYTES 0x02U  /**< Length of the communication frame (bytes) */

/*******************************************************************************
 * API
 ******************************************************************************/
/** @addtogroup API
 * @{ */
/**
 * @brief Performs a read from a single FS65 register.
 *
 * Performs a single read register based on provided address.
 * The response is returned in @ref fs65_rx_data_t structure.
 * @param [in]  address     Register address.
 * @param [out] rxData      Structure holding the response from SBC.
 * @return @ref fs65_status_t Status return code. */
fs65_status_t FS65_ReadRegister(uint8_t address, fs65_rx_data_t* rxData);

/**
 * @brief Sends write command to the FS65.
 *
 * @param [in] address      Register address.
 * @param [in] writeData    Register write value.
 * @param [out] rxData      Diagnostic data returned by the SBC.
 * @return @ref fs65_status_t Status return code. */
fs65_status_t FS65_WriteRegister(uint8_t address, uint8_t writeData,
        fs65_rx_data_t* rxData);

/**
 * Writes set of configuration registers values.
 *
 * If registers is NULL or numOfItems is 0, function just returns OK status code.
 *
 * @param [in] registers Pointer to array of register configuration values.
 * @param [in] numOfItems Number of items.
 * @return @ref fs65_status_t Status return code.
 */
fs65_status_t FS65_WriteRegisters(fs65_reg_config_value_t* registers, uint8_t numOfItems);

/** @} */
#endif /* SBC_FS65_COMMUNICATION_H__ */
