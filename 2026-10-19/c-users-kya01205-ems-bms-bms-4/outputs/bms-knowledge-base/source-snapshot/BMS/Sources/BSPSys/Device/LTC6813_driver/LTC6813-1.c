/*
 Linear Technology DC2100A Demonstration Board.
 Driver File for LTC6813-2 Multicell Battery Monitors.
 All datasheet references in this file refer to Linear Technology Corp. document 681312fa.pdf.

 @verbatim
 The LTC6813 is a 3rd generation multicell battery stack
 monitor that measures up to 12 series connected battery
 cells with a total measurement error of less than 1.2mV. The
 cell measurement range of 0V to 5V makes the LTC6813
 suitable for most battery chemistries. All 12 cell voltages
 can be captured in 290us, and lower data acquisition rates
 can be selected for high noise reduction.
 Multiple LTC6813 devices can be connected in series,
 permitting simultaneous cell monitoring of long, high voltage
 battery strings. Each LTC6813 has an isoSPI interface
 for high speed, RF-immune, local area communications.
 Using the LTC6813-2, multiple devices are connected in
 a daisy-chain with one host processor connection for all
 devices. Using the LTC6813-2, multiple devices are connected
 in parallel to the host processor, with each device
 individually addressed.
 Additional features include passive balancing for each cell,
 an onboard 5V regulator, and 5 general purpose I/O lines.
 In sleep mode, current consumption is reduced to 4uA.
 The LTC6813 can be powered directly from the battery,
 or from an isolated supply.
 @endverbatim

 http://www.linear.com/product/LTC6813

 http://www.linear.com/product/LTC6813#demoboards

 REVISION HISTORY
 $Revision: 699 $
 $Date: 2014-09-09 16:02:09 -0400 (Tue, 09 Sep 2014) $

 Copyright (c) 2013, Linear Technology Corp.(LTC)
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of Linear Technology Corp.

*/

//! @defgroup LTC6813-2 LTC6813-2 Multicell Battery Monitors.

/*! @file
    @ingroup LTC6813-2
    Driver File for LTC6813-2 Multicell Battery Monitors.
*/

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Includes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "BSPTypeDef.h"
#include "LTC6813-1.h"
#include "LTC6813-1_Config.h"
#include "LTC6813-1_Registers.h"
#include "DEVGPIO.h"
#include "stdlib.h"
#include "BSPSPI.h"
//#include <string.h>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Timing parameters for state transitions defined by datasheet Figure 1.
// Electrical Characteristics from datasheet pages 7 and 8 contain worst case values.
#define LTC6813_TWAKE                                           300     // us, max value
#define LTC6813_TSLEEP                                          1800    // ms, min value
#define LTC6813_TREADY                                          10      // us, max value
#define LTC6813_TIDLE                                           4300    // us, min value
#define LTC6813_TDWELL                                          1       // us, min value of 240ns in datasheet is outside resolution of uP timer.

#define LTC6813_NUM_CELLV_ADC_PER_REGISTER_GROUP                (LTC6813_REGISTER_GROUP_SIZE / LTC6813_ADC_SIZE) // Number of cell voltage ADC measurements per register group
#define LTC6813_NUM_REGISTER_GROUP_READS_FOR_ALL_CELLV          (LTC6813_NUM_CELLV_ADC / LTC6813_NUM_CELLV_ADC_PER_REGISTER_GROUP) // Number of register groups that must be read to get all cell voltage ADC measurements
#define LTC6813_NUM_REGISTER_GROUP_READS_FOR_TWO_CELLV          (2) // Number of register groups that must be read to get a pair of cell voltage ADC measurements
#define LTC6813_NUM_AUX_ADC_PER_REGISTER_GROUP                  (LTC6813_REGISTER_GROUP_SIZE / LTC6813_ADC_SIZE)

#define LTC6813_NUM_COMM_BYTES_PER_REGISTER_GROUP               3       // Number of I2C/SPI bytes contained in the COMM register group

// Relationship between input SPI clocks and I2C/SPI output clocks, as per page 32 of datasheet.
#define LTC6813_SPI_CLOCK_CYCLES_PER_STCOMM_BYTE                24
#define LTC6813_SPI_BYTES_PER_STCOMM_BYTE                       (LTC6813_SPI_CLOCK_CYCLES_PER_STCOMM_BYTE/BITS_PER_BYTE)

// input clocks, when used to drive the I2C/SPI output clocks, as per table 19 of datasheet
#define LTC6813_BAUD_RATE_DIVISOR                               2       // tCLK/(SCL Clock Frequency) in table 19 of datasheet

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Table used perform PEC calculation defined by datasheet figure 22.
const u16 ltc6813_pec_seed_value = 16;
const u16 ltc6813_pec_table[256] =
{   0x0000, 0xc599, 0xceab, 0x0b32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,
    0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
    0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
    0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
    0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
    0x2544, 0x02be, 0xc727, 0xcc15, 0x098c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
    0x3d6e, 0xf8f7, 0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x07c2, 0xc25b, 0xc969, 0x0cf0, 0xdf0d,
    0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
    0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
    0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
    0x4a88, 0x8f11, 0x057c, 0xc0e5, 0xcbd7, 0xe4e,  0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
    0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
    0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
    0x85e9, 0x0f84, 0xca1d, 0xc12f, 0x04b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
    0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
    0x2ac0, 0x0d3a, 0xc8a3, 0xc391, 0x0608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
    0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
    0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
    0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
    0x2fbc, 0x0846, 0xcddf, 0xc6ed, 0x0374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0x0af8, 0xcf61, 0xc453,
    0x01ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
    0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
    0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
};

u8 ltc6813_gpio_pulldown;        // Since the GPIOx values in the CFGR register group read differently than what was written, the only way
                                            // to read/modify/write other values in this register are to have local storage for the written values.

u8 ltc6813_adcopt;                        // When starting an ADC conversion at a sample frequency, the adcopt must be correctly set.  Local storage allows driver
                                            // to quickly detect whether the adcopt bit needs to be changed in the CFG register without reading a register group unnecessarily.

u32 ltc6813_wakeup_timestamp;    // in LTC6813_CONFIG_TIMER_RESOLUTION, timestamp for the last time that the LTC6813 system received a wakeup signal.

//函数临时变量,但长度太大改为全局变量
u8 cv_r[(LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE)*LTC6813_MAX_BOARDS];    // storage for the Cell Voltage Register Groups + PEC

u8 ltcSpi_Port = 0;//0：A口 1:B口

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Prototypes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void ltc6813_wakeup(void);
void ltc6813_get_adcopt_and_md(u8 conversion_mode, u8* adcopt_ptr, u8* md_ptr);
void ltc6813_adc_opt_set(u8 board_num, u8 adcopt);
void ltc6813_command_code_send(u16 command_code, BOOLEAN reg_group_command);

BOOLEAN ltc6813_register_group_read(u8* register_group);
void ltc6813_cfgr_modify(u8 board_num, u8* register_mask_ptr, u8* register_value_ptr);
void ltc6813_clock_out(u16 bytes_to_send, u16 baud_khz);
u16 ltc6813_pec_lookup(char data, u16 remainder);

//=============================================================================================
//函数名称	: static void LTC6813CSLOW(void)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: 选通6813
//注		意	: 低电平选通有效
//=============================================================================================
static void LTC6813CSLOW(void);

//=============================================================================================
//函数名称	: static void LTC6813CSHIGH(void)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: 禁止6813
//注		意	: 高电平禁用
//=============================================================================================
static void LTC6813CSHIGH(void);

//=============================================================================================
//函数名称	: static void LTC6813CMD_PEC_WRITE(void)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: SPI写入6813控制命令或者PEC
//注		意	:
//=============================================================================================
static void LTC6813CMD_PEC_WRITE(u8* commanddata);

//=============================================================================================
//函数名称	: static void LTC6813REGWRITE(void)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: SPI写入6813控制命令或者PEC
//注		意	:
//=============================================================================================
static void LTC6813REGWRITE(u8* commanddata);

//=============================================================================================
//函数名称	: static void LTC6813DaisyREGWRITE(u8 Ic_num, u8 commanddata[][6])
//输入参数	: Ic_num:第N个6813      commanddata[][6]:需要发送的数据
//输出参数	: void
//静态变量	: void
//功		能	: SPI写入6813控制命令或者PEC(针对菊花链)
//注		意	:
//=============================================================================================
static void LTC6813DaisyREGWRITE(u8 Ic_num, u8 commanddata[][6]);

//=============================================================================================
//函数名称	: static BOOLEAN LTC6813REGGROUPREAD(u8* adcdata, u8 Ic_num)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: 读取一组ADC电压寄存器，包括3个电压数据和1个PEC数据
//注		意	:
//=============================================================================================
static BOOLEAN LTC6813REGGROUPREAD(u8* adcdata, u8 Ic_num);
	
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Initializes the LTC6813-2 code module.
void LTC6813_Init(void)
{
    // init I/O pulldowns to be all off
	// ltc6813_gpio_pulldown = ((1 << LTC6813_NUM_GPIO) - 1);

    // init ADCOPT to nonsensical value so that it will always be set the first time.
	// ltc6813_adcopt = 0xFF;

    // Initialize wakeup timestamp to ensure that SLEEP is assumed for first communication with LTC6813
	// ltc6813_wakeup_timestamp = LTC6813_CONFIG_TIMER - (LTC6813_TSLEEP * LTC6813_CONFIG_TIMER_RESOLUTION / MS_PER_S);

    return;
}

// Read/Modify/Write the Config register such that the GPIOs are at the values specified in the gpio_value_bitmap.
void LTC6813_GPIO_Set(u8 board_num, u8 gpio_bitmap)
{
    struct {
    	u8 mask[LTC6813_REGISTER_GROUP_SIZE];
    	u8 value[LTC6813_REGISTER_GROUP_SIZE];
    } register_modify;

    ltc6813_gpio_pulldown = gpio_bitmap;    // Remember this written value, as all other read/modify/write operations on the
                                            // CFGR register will need to write this value instead of the read value.

    // Start with clear mask and value, and then set up gpio mask/value
    // memset(&register_modify.mask[1], 0, sizeof(register_modify)-1);
    register_modify.mask[0] = LTC6813_CFGR0_GPIOx_MASK;
    register_modify.value[0] = LTC6813_CFGR0_GPIOx(ltc6813_gpio_pulldown);

    ltc6813_cfgr_modify(board_num, register_modify.mask, register_modify.value);

    return;
}

// Read/Modify/Write the Config to turn the Reference on and off.
void LTC6813_Refon_Set(u8 board_num, BOOLEAN refon)
{
    struct {
    	u8 mask[LTC6813_REGISTER_GROUP_SIZE];
    	u8 value[LTC6813_REGISTER_GROUP_SIZE];
    } register_modify;

    // Start with clear mask and value, and then set up gpio mask/value
    // memset(&register_modify.mask[1], 0, sizeof(register_modify)-1);
    register_modify.mask[0] = LTC6813_CFGR0_GPIOx_MASK;
    register_modify.value[0] = LTC6813_CFGR0_GPIOx(ltc6813_gpio_pulldown);

    // Create Mask to set Ref On
    register_modify.mask[0] |= LTC6813_CFGR0_REFON_MASK;

    // Set Value for Ref
    register_modify.value[0] |= LTC6813_CFGR0_REFON(refon);

    ltc6813_cfgr_modify(board_num, register_modify.mask, register_modify.value);

    return;
}

// Read/Modify/Write the Config register such that the undervoltage and overvoltage values are those specified.
void LTC6813_UVOV_Thresholds_Set(u8 board_num, u16 vuv_value, u16 vov_value)
{
    struct {
    	u8 mask[LTC6813_REGISTER_GROUP_SIZE];
    	u8 value[LTC6813_REGISTER_GROUP_SIZE];
    } register_modify;

    // Start with clear mask and value, and then set up gpio mask/value
    // memset(&register_modify.mask[1], 0, sizeof(register_modify)-1);
    register_modify.mask[0] = LTC6813_CFGR0_GPIOx_MASK;
    register_modify.value[0] = LTC6813_CFGR0_GPIOx(ltc6813_gpio_pulldown);

    // Create Mask to set VUV and VOV
    *((u16*)(register_modify.mask+1)) |= LTC6813_CFGR1_VUV_MASK;
    *((u16*)(register_modify.mask+2)) |= LTC6813_CFGR2_VOV_MASK;

    // Set Value for VUV and VOV
    *((u16*)(register_modify.value+1)) |= LTC6813_CFGR1_VUV(vuv_value);
    *((u16*)(register_modify.value+2)) |= LTC6813_CFGR2_VOV(vov_value);

    ltc6813_cfgr_modify(board_num, register_modify.mask, register_modify.value);

    return;
}

// Read/Modify/Write the Config register such that the discharger and discharge timeout values are those specified.
void LTC6813_Dischargers_Set(u8 board_num, u16 discharge_bitmap, u16 timeout_value)
{
    struct {
    	u8 mask[LTC6813_REGISTER_GROUP_SIZE];
    	u8 value[LTC6813_REGISTER_GROUP_SIZE];
    } register_modify;

    // Start with clear mask and value, and then set up gpio mask/value
    // memset(&register_modify.mask[1], 0, sizeof(register_modify)-1);
    register_modify.mask[0] = LTC6813_CFGR0_GPIOx_MASK;
    register_modify.value[0] = LTC6813_CFGR0_GPIOx(ltc6813_gpio_pulldown);

    // Create Mask to set discharger and discharge timeout values
    *((u16*)(register_modify.mask+4)) |= LTC6813_CFGR4_DCCx_MASK;
    register_modify.mask[5] |= LTC6813_CFGR5_DCTO_MASK;

    // Set Value for discharger and discharge timeout values
    *((u16*)(register_modify.value+4)) |= LTC6813_CFGR4_DCCx(discharge_bitmap);
    register_modify.value[5] |= LTC6813_CFGR5_DCTO(timeout_value);

    ltc6813_cfgr_modify(board_num, register_modify.mask, register_modify.value);

    return;
}

// Gets the LTC6813 under-voltage and over-voltage thresholds in LTC6813_UVOV_RESOLUTION units.
BOOLEAN LTC6813_UVOV_Thresholds_Get(u8 board_num, u16* vuv_value, u16* vov_value)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
    u8 cfgr[LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE];    // storage for the Configuration Register Group B + PEC

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    command_code = LTC6813_COMMAND_CODE_RDCFG(address);

    // Send the command code
    ltc6813_command_code_send(command_code, TRUE);

    // Read the Status Register
    if(ltc6813_register_group_read(cfgr) == TRUE)
    {
        *vuv_value = *((u16*)(cfgr+1)) & LTC6813_CFGR1_VUV_MASK;
        *vov_value = (*((u16*)(cfgr+2)) & LTC6813_CFGR2_VOV_MASK) >> 4;
    }
    else
    {
    	// LTC6813_CONFIG_ERROR_CRC(board_num, command_code, cfgr, sizeof(cfgr));
        success = FALSE;
    }

    return success;
}

// Gets the LTC6813 flags indicating under-voltage and over-voltage conditions are present.
BOOLEAN LTC6813_UVOV_Flags_Get(u8 board_num, u16* vuv_flags, u16* vov_flags)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
    u8 stbr[LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE];    // storage for the Status Register Group B + PEC
    u8 byte_num;
    u8 bit_num;
    u8 bit_mask_in;
    u16 bit_mask_out;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    command_code = LTC6813_COMMAND_CODE_RDSTATB(address);

    // Send the command code
    ltc6813_command_code_send(command_code, TRUE);
    
    success = LTC6813REGGROUPREAD(stbr, board_num);
    
    if(success == TRUE)
    {
        bit_mask_out = 0x0001;
        *vuv_flags = 0;
        *vov_flags = 0;

        // Interpret data in register group according to datasheet Table 44
        for (byte_num = 2; byte_num < 5; byte_num++)
        {
            bit_mask_in = 0x01;

            for (bit_num = 0; bit_num < 8; bit_num += 2)
            {
                if(stbr[byte_num] & bit_mask_in)
                {
                    *vuv_flags |= bit_mask_out;
                }
                bit_mask_in <<= 1;

                if(stbr[byte_num] & bit_mask_in)
                {
                    *vov_flags |= bit_mask_out;
                }
                bit_mask_in <<= 1;

                bit_mask_out <<= 1;
            }
        }
        
    }
    else
    {
        return FALSE; 
    }
 /*
    // Read the Status Register
    if(ltc6813_register_group_read(stbr) == TRUE)
    {
        // Initialize so that uvov flags will be separated into uv and ov by loop below
        bit_mask_out = 0x0001;
        *vuv_flags = 0;
        *vov_flags = 0;

        // Interpret data in register group according to datasheet Table 44
        for (byte_num = 2; byte_num < 5; byte_num++)
        {
            bit_mask_in = 0x01;

            for (bit_num = 0; bit_num < 8; bit_num += 2)
            {
                if(stbr[byte_num] & bit_mask_in) *vuv_flags |= bit_mask_out;
                bit_mask_in <<= 1;

                if(stbr[byte_num] & bit_mask_in) *vov_flags |= bit_mask_out;
                bit_mask_in <<= 1;

                bit_mask_out <<= 1;
            }
        }
    }
    else
    {
     //   LTC6813_CONFIG_ERROR_CRC(board_num, command_code, stbr, sizeof(stbr));
        success = FALSE;
    }
       */
    return TRUE;
}

// Gets the LTC6813 revision.
BOOLEAN LTC6813_Revision_Get(u8 board_num, u8* revision)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
    u8 stbr[LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE];    // storage for the Status Register Group B + PEC

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    command_code = LTC6813_COMMAND_CODE_RDSTATB(address);

    // Send the command code
    ltc6813_command_code_send(command_code, TRUE);

    // Read the Status Register
    if(ltc6813_register_group_read(stbr) == TRUE)
    {
        *revision = stbr[5] >> 4;
    }
    else
    {
    //    LTC6813_CONFIG_ERROR_CRC(board_num, command_code, stbr, sizeof(stbr));
        success = FALSE;
    }

    return success;
}

// Gets the LTC6813 ADC Reference status, where 1 = ON and 0 = OFF.
BOOLEAN LTC6813_Refon_Get(u8 board_num, u8* refon)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
    u8 cfgr[LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE];    // storage for the Configuration Register Group B + PEC

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    command_code = LTC6813_COMMAND_CODE_RDCFG(address);

    // Send the command code
    ltc6813_command_code_send(command_code, TRUE);

    // Read the Status Register
    if(ltc6813_register_group_read(cfgr) == TRUE)
    {
        if (cfgr[0] & LTC6813_CFGR0_REFON_MASK)
        {
            *refon = TRUE;
        }
        else
        {
            *refon = FALSE;
        }
    }
    else
    {
    	// LTC6813_CONFIG_ERROR_CRC(board_num, command_code, cfgr, sizeof(cfgr));
        success = FALSE;
    }

    return success;
}


// Clears the LTC6813 Cell Voltage ADC registers.  This is useful to detect if the conversion was started properly when the results are read.
void LTC6813_Cell_ADC_Clear(u8 board_num)
{
	u8 address;
    u16 command_code;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    // Build the command code
    command_code = LTC6813_COMMAND_CODE_CLRCELL(address);

    // Send the command code
    ltc6813_command_code_send(command_code, FALSE);

    return;
}

// Starts the LTC6813 Cell Voltage ADC conversion at the specified conversion mode.
void LTC6813_Cell_ADC_Start(u8 board_num, LTC6813_CONVERSION_MODE_T mode, LTC6813_CH_CELL_TYPE cell_select, BOOLEAN discharge_permitted)
{
	u8 address;
    u16 command_code;
    u8 adcopt;
    u8 md;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Get adcopt and md values to achieve the desired sample rate.
    ltc6813_get_adcopt_and_md((u8)mode, &adcopt, &md);

    // Set adcopt in the cfg register, if necessary
    if( ltc6813_adcopt != adcopt)
    {
        ltc6813_adc_opt_set(board_num, adcopt);
    }
    else
    {
        // Wakeup 6813 in case it has entered SLEEP or IDLE.
        ltc6813_wakeup();
    }

    // Build the command code to start ADC conversion
    command_code = LTC6813_COMMAND_CODE_ADCV(address, md, (discharge_permitted ? LTC6813_DCP_DISCHARGE_PERMITTED : LTC6813_DCP_DISCHARGE_NOT_PERMITTED), cell_select);

    // Send the command code
    ltc6813_command_code_send(command_code, FALSE);

    return;
}

// 启动导线开路检测和轮训状态
void LTC6813_Cell_ADC_ADOW(u8 board_num, LTC6813_CONVERSION_MODE_T mode, LTC6813_CH_CELL_TYPE cell_select, BOOLEAN discharge_permitted, u8 pup)
{
	u8 address;
    u16 command_code;
    u8 adcopt;
    u8 md;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Get adcopt and md values to achieve the desired sample rate.
    ltc6813_get_adcopt_and_md((u8)mode, &adcopt, &md);

    // Set adcopt in the cfg register, if necessary
    if( ltc6813_adcopt != adcopt)
    {
        ltc6813_adc_opt_set(board_num, adcopt);
    }
    else
    {
        // Wakeup 6813 in case it has entered SLEEP or IDLE.
        ltc6813_wakeup();
    }
   
    // Build the command code to start ADC conversion
    command_code = LTC6813_COMMAND_CODE_ADOW(address, md, pup, (discharge_permitted ? LTC6813_DCP_DISCHARGE_PERMITTED : LTC6813_DCP_DISCHARGE_NOT_PERMITTED), cell_select);

    // Send the command code
    ltc6813_command_code_send(command_code, FALSE);

    return;
}

//=============================================================================================
//函数名称	: BOOLEAN LTC6813_Cell_ADC_Read(int_8 board_num, LTC6813_CH_CELL_TYPE cell_select, u16 adc_value_ptr[][18], u8 IC_num)
//输入参数	: LTC6813_CH_CELL_TYPE cell_select,需要采样的节号； u16* adc_value_ptr 输出电压数组 reg_Num:寄存器
//输出参数	: TURE：采样正常，FALSE：采样CRC校验不通过
//静态变量	: void
//功		能	: 电压采样任务
//注		意	:
//=============================================================================================
BOOLEAN LTC6813_Cell_ADC_Read(LTC6813_CH_CELL_TYPE cell_select, u16 adc_value_ptr[][18], u8 IC_num, u8 reg_Num)
{
    u8 success = TRUE;
    u8 address;
    u16 command_code;
	u16 parsed_cell;
    u8 reg_count;
    u8 i = 0;
    //u8 cv_r[(LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE)*IC_num];    // storage for the Cell Voltage Register Groups + PEC
    u8 data_counter = 0; //data counter
	u8 current_ic = 0;   //ic counter

    if(cell_select == LTC6813_CH_ALL)
    {
		for(reg_count = reg_Num; reg_count < (reg_Num + 2); reg_count++)
		{
			data_counter = 0;
			// Get the board address from the board number
			address = LTC6813_CONFIG_GET_BOARD_ADDRESS(0);

			// Wakeup 6813 in case it has entered SLEEP or IDLE.
			ltc6813_wakeup();

			if(reg_Num == 4)
			{
				command_code = LTC6813_COMMAND_CODE_RDCVA(address) + (reg_count * 0x0002 - 3);
			}
			else
			{
				command_code = LTC6813_COMMAND_CODE_RDCVA(address) + (reg_count * 0x0002);
			}

			// Send the command code
			ltc6813_command_code_send(command_code, TRUE);

			success = LTC6813REGGROUPREAD(cv_r, IC_num);

			if(success == TRUE)
			{
				for(current_ic = 0 ; current_ic < IC_num; current_ic++) 				        // executes for every LTC6813 in the daisy chain
				{																 	  			// current_ic is used as the IC counter
					for(i = 0; i < 3; i++)   												    // This loop parses the read back data into cell voltages, it
					{														   		  			// loops once for each of the 3 cell voltage codes in the register
						parsed_cell = (u16)cv_r[data_counter] + ((u16)cv_r[data_counter+1]<<8); // Each cell code is received as two bytes and is combined to
																								// create the parsed cell voltage code
						adc_value_ptr[current_ic][i + (reg_count * 3)] = 0x0000FFFF & parsed_cell;
						data_counter = data_counter + 2;     									// Because cell voltage codes are two bytes the data counter
																								// must increment by two for each parsed cell code
					}
					data_counter = data_counter + 2;//两个PEC
				}
			}
			else
			{
				return FALSE;
			}
		}
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//=============================================================================================
//函数名称	: BOOLEAN LTC6813_Cell_ADC_ReadAB(int_8 board_num, LTC6813_CH_CELL_TYPE cell_select, u16 adc_value_ptr[][18], u8 IC_num)
//输入参数	: LTC6813_CH_CELL_TYPE cell_select,需要采样的节号； u16* adc_value_ptr 输出电压数组 
//输出参数	: TURE：采样正常，FALSE：采样CRC校验不通过
//静态变量	: void
//功		能	: 电压采样任务
//注		意	:
//=============================================================================================
BOOLEAN LTC6813_Cell_ADC_ReadAB(LTC6813_CH_CELL_TYPE cell_select, u16 adc_value_ptr[][18], u8 IC_num)
{
    u8 success = TRUE;
    u8 address;
    u16 command_code;
	u16 parsed_cell;
    u8 reg_count;
    u8 i = 0;
    //u8 cv_r[(LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE)*IC_num];    // storage for the Cell Voltage Register Groups + PEC
    u8 data_counter = 0; //data counter
	u8 current_ic = 0;   //ic counter
	
    if(cell_select == LTC6813_CH_ALL)
    {
		for(reg_count = 0; reg_count < 2; reg_count++)
		{
			data_counter = 0;
			// Get the board address from the board number
			address = LTC6813_CONFIG_GET_BOARD_ADDRESS(0);

			// Wakeup 6813 in case it has entered SLEEP or IDLE.
			ltc6813_wakeup();

			command_code = LTC6813_COMMAND_CODE_RDCVA(address) + (reg_count * 0x0002);
			
			// Send the command code
			ltc6813_command_code_send(command_code, TRUE);
			
			success = LTC6813REGGROUPREAD(cv_r, IC_num);
			
			if(success == TRUE)
			{
				for(current_ic = 0 ; current_ic < IC_num; current_ic++) 				        // executes for every LTC6813 in the daisy chain
				{																 	  			// current_ic is used as the IC counter
					for(i = 0; i < 3; i++)   												    // This loop parses the read back data into cell voltages, it 
					{														   		  			// loops once for each of the 3 cell voltage codes in the register 					
						parsed_cell = (u16)cv_r[data_counter] + ((u16)cv_r[data_counter+1]<<8); // Each cell code is received as two bytes and is combined to
																								// create the parsed cell voltage code																								
						adc_value_ptr[current_ic][i + (reg_count * 3)] = 0x0000FFFF & parsed_cell;						
						data_counter = data_counter + 2;     									// Because cell voltage codes are two bytes the data counter
																								// must increment by two for each parsed cell code
					}
					data_counter = data_counter + 2;//两个PEC
				}		
			}
			else
			{
				return FALSE;
			}					
		}
        return TRUE;
    }
    else
    {
        return FALSE; 
    }

}

//=============================================================================================
//函数名称	: BOOLEAN LTC6813_Cell_ADC_ReadCD(int_8 board_num, LTC6813_CH_CELL_TYPE cell_select, u16 adc_value_ptr[][18], u8 IC_num)
//输入参数	: LTC6813_CH_CELL_TYPE cell_select,需要采样的节号； u16* adc_value_ptr 输出电压数组
//输出参数	: TURE：采样正常，FALSE：采样CRC校验不通过
//静态变量	: void
//功		能	: 电压采样任务
//注		意	:
//=============================================================================================
BOOLEAN LTC6813_Cell_ADC_ReadCD(LTC6813_CH_CELL_TYPE cell_select, u16 adc_value_ptr[][18], u8 IC_num)
{
    u8 success = TRUE;
    u8 address;
    u16 command_code;
	u16 parsed_cell;
    u8 reg_count;
    u8 i = 0;
    //u8 cv_r[(LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE)*IC_num];    // storage for the Cell Voltage Register Groups + PEC
    u8 data_counter = 0; //data counter
	u8 current_ic = 0;   //ic counter

    if(cell_select == LTC6813_CH_ALL)
    {
		for(reg_count = 2; reg_count < 4; reg_count++)
		{
			data_counter = 0;
			// Get the board address from the board number
			address = LTC6813_CONFIG_GET_BOARD_ADDRESS(0);

			// Wakeup 6813 in case it has entered SLEEP or IDLE.
			ltc6813_wakeup();

			command_code = LTC6813_COMMAND_CODE_RDCVA(address) + (reg_count * 0x0002);

			// Send the command code
			ltc6813_command_code_send(command_code, TRUE);

			success = LTC6813REGGROUPREAD(cv_r, IC_num);

			if(success == TRUE)
			{
				for(current_ic = 0 ; current_ic < IC_num; current_ic++) 				        // executes for every LTC6813 in the daisy chain
				{																 	  			// current_ic is used as the IC counter
					for(i = 0; i < 3; i++)   												    // This loop parses the read back data into cell voltages, it
					{														   		  			// loops once for each of the 3 cell voltage codes in the register
						parsed_cell = (u16)cv_r[data_counter] + ((u16)cv_r[data_counter+1]<<8); // Each cell code is received as two bytes and is combined to
																								// create the parsed cell voltage code
						adc_value_ptr[current_ic][i + (reg_count * 3)] = 0x0000FFFF & parsed_cell;
						data_counter = data_counter + 2;     									// Because cell voltage codes are two bytes the data counter
																								// must increment by two for each parsed cell code
					}
					data_counter = data_counter + 2;//两个PEC
				}
			}
			else
			{
				return FALSE;
			}
		}
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

//=============================================================================================
//函数名称	: BOOLEAN LTC6813_Cell_ADC_ReadEF(int_8 board_num, LTC6813_CH_CELL_TYPE cell_select, u16 adc_value_ptr[][18], u8 IC_num)
//输入参数	: LTC6813_CH_CELL_TYPE cell_select,需要采样的节号； u16* adc_value_ptr 输出电压数组 
//输出参数	: TURE：采样正常，FALSE：采样CRC校验不通过
//静态变量	: void
//功	能	: 电压采样任务
//注	意	:  
//=============================================================================================
// Reads the LTC6813 Cell Voltage ADC conversion results.
BOOLEAN LTC6813_Cell_ADC_ReadEF(LTC6813_CH_CELL_TYPE cell_select, u16 adc_value_ptr[][18], u8 IC_num)
{
    u8 success = TRUE;
    u8 address;
    u16 command_code;
	u16 parsed_cell;
    u8 reg_count;
    u8 i = 0;
    //u8 cv_r[(LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE)*IC_num];    // storage for the Cell Voltage Register Groups + PEC
    //u8 cv_r[50];    // storage for the Cell Voltage Register Groups + PEC
    u8 data_counter = 0; //data counter
	u8 current_ic = 0;   //ic counter
	
    if(cell_select == LTC6813_CH_ALL)
    {
		for(reg_count = 4; reg_count < 6; reg_count++)
		{
			data_counter = 0;
			// Get the board address from the board number
			address = LTC6813_CONFIG_GET_BOARD_ADDRESS(0);

			// Wakeup 6813 in case it has entered SLEEP or IDLE.
			ltc6813_wakeup();

			command_code = LTC6813_COMMAND_CODE_RDCVA(address) + (reg_count * 0x0002 - 3);
			
			// Send the command code
			ltc6813_command_code_send(command_code, TRUE);
			
			success = LTC6813REGGROUPREAD(cv_r, IC_num);
			
			if(success == TRUE)
			{
				for(current_ic = 0 ; current_ic < IC_num; current_ic++) 				        // executes for every LTC6813 in the daisy chain
				{																 	  			// current_ic is used as the IC counter
					for(i = 0; i < 3; i++)   												    // This loop parses the read back data into cell voltages, it 
					{														   		  			// loops once for each of the 3 cell voltage codes in the register 					
						parsed_cell = cv_r[data_counter] + (cv_r[data_counter+1] << 8);         // Each cell code is received as two bytes and is combined to
																								// create the parsed cell voltage code																								
						adc_value_ptr[current_ic][i + (reg_count * 3)] = 0x0000FFFF & parsed_cell;
						data_counter = data_counter + 2;     									// Because cell voltage codes are two bytes the data counter
																								// must increment by two for each parsed cell code
					}
					data_counter = data_counter + 2;
				}		
			}
			else
			{
				return FALSE;
			}					
		}
        return TRUE;
    }
    else
    {
        return FALSE; 
    }

}

// Clears the LTC6813 GPIO ADC registers.  This is useful to detect if the conversion was started properly when the results are read.
void LTC6813_GPIO_ADC_Clear(u8 board_num)
{
	u8 address;
    u16 command_code;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    // Build the command code
    command_code = LTC6813_COMMAND_CODE_CLRAUX(address);

    // Send the command code
    ltc6813_command_code_send(command_code, FALSE);

    return;
}

// Starts the specified LTC6813 GPIO ADC conversion at the specified conversion mode.
void LTC6813_GPIO_ADC_Start(u8 board_num, LTC6813_CONVERSION_MODE_T mode, LTC6813_CHG_GPIO_TYPE gpio_select)
{
	u8 address;
    u16 command_code;
    u8 adcopt;
    u8 md;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Get adcopt and md values to achieve the desired sample rate.
    ltc6813_get_adcopt_and_md((u8)mode, &adcopt, &md);

    // Set adcopt in the cfg register, if necessary
    if( ltc6813_adcopt != adcopt)
    {
        ltc6813_adc_opt_set(board_num, adcopt);
    }
    else
    {
        // Wakeup 6813 in case it has entered SLEEP or IDLE.
        ltc6813_wakeup();
    }

    // Build the command code
    command_code = LTC6813_COMMAND_CODE_ADAX(address, md, gpio_select);

    // Send the command code
    ltc6813_command_code_send(command_code, FALSE);

    return;
}

// Reads the specified LTC6813 GPIO ADC conversion results.
BOOLEAN LTC6813_GPIO_ADC_Read(u8 board_num, LTC6813_CHG_GPIO_TYPE gpio_select, u16 adc_value_ptr[][12], u8 Ic_num)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
	u16 parsed_cell;
	u8 reg_count;
	u8 i = 0;
    //u8 cv_r[(LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE)*Ic_num];    // storage for the Auxiliary Register Group + PEC
    u8 data_counter = 0; //data counter
	u8 current_ic = 0;   //ic counter

    // Determine how many gpio ADC results were requested and where that data is located in register group
    if(LTC6813_CHG_GPIO_ALL == gpio_select)
    {		
		for(reg_count = 0; reg_count < 4; reg_count++)
		{
			data_counter = 0;
			// Get the board address from the board number
			address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

			// Wakeup 6813 in case it has entered SLEEP or IDLE.
			ltc6813_wakeup();			
			// Build the command code
			if(reg_count < 2)
			{
				command_code = LTC6813_COMMAND_CODE_RDAUXA(address) + reg_count * 2;/*LTC6811*/
			}
			else
			{
				command_code = LTC6813_COMMAND_CODE_RDAUXA(address) + reg_count * 2 - 3;/*LTC6813*/
			}

			// Send the command code
			ltc6813_command_code_send(command_code, TRUE);
			
			success = LTC6813REGGROUPREAD(cv_r, Ic_num);

			if(success == TRUE)
			{
				for(current_ic = 0 ; current_ic < Ic_num; current_ic++) 				        // executes for every LTC6813 in the daisy chain
				{																 	  			// current_ic is used as the IC counter
					for(i = 0; i < 3; i++)   												    // This loop parses the read back data into cell voltages, it 
					{														   		  			// loops once for each of the 3 cell voltage codes in the register 					
						parsed_cell = cv_r[data_counter] + (cv_r[data_counter+1]<<8);           //Each cell code is received as two bytes and is combined to
																								// create the parsed cell voltage code																								
						adc_value_ptr[current_ic][i + (reg_count * 3)] = 0x0000FFFF & parsed_cell;
						data_counter = data_counter + 2;     									//Because cell voltage codes are two bytes the data counter
																								//must increment by two for each parsed cell code
					}
					data_counter = data_counter + 2;
				}
			}
			else
			{
				return FALSE; 
			}
		}	
	}	
    return success;
}

// Reads the specified LTC6813 GPIO ADC conversion results.
BOOLEAN LTC6813_GPIO_ADC_ReadAB(u8 board_num, LTC6813_CHG_GPIO_TYPE gpio_select, u16 adc_value_ptr[][12], u8 Ic_num)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
	u16 parsed_cell;
	u8 reg_count;
	u8 i = 0;
    //u8 cv_r[(LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE)*Ic_num];    // storage for the Auxiliary Register Group + PEC
    u8 data_counter = 0; //data counter
	u8 current_ic = 0;   //ic counter

    // Determine how many gpio ADC results were requested and where that data is located in register group
    if(LTC6813_CHG_GPIO_ALL == gpio_select)
    {
		for(reg_count = 0; reg_count < 2; reg_count++)
		{
			data_counter = 0;
			// Get the board address from the board number
			address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

			// Wakeup 6813 in case it has entered SLEEP or IDLE.
			ltc6813_wakeup();
			// Build the command code
			command_code = LTC6813_COMMAND_CODE_RDAUXA(address) + reg_count * 2;/*LTC6811*/

			// Send the command code
			ltc6813_command_code_send(command_code, TRUE);

			success = LTC6813REGGROUPREAD(cv_r, Ic_num);

			if(success == TRUE)
			{
				for(current_ic = 0 ; current_ic < Ic_num; current_ic++) 				        // executes for every LTC6813 in the daisy chain
				{																 	  			// current_ic is used as the IC counter
					for(i = 0; i < 3; i++)   												    // This loop parses the read back data into cell voltages, it
					{														   		  			// loops once for each of the 3 cell voltage codes in the register
						parsed_cell = cv_r[data_counter] + (cv_r[data_counter+1]<<8);           //Each cell code is received as two bytes and is combined to
																								// create the parsed cell voltage code
						adc_value_ptr[current_ic][i + (reg_count * 3)] = 0x0000FFFF & parsed_cell;
						data_counter = data_counter + 2;     									//Because cell voltage codes are two bytes the data counter
																								//must increment by two for each parsed cell code
					}
					data_counter = data_counter + 2;
				}
			}
			else
			{
				return FALSE;
			}
		}
	}
    return success;
}

// Reads the specified LTC6813 GPIO ADC conversion results.
BOOLEAN LTC6813_GPIO_ADC_ReadCD(u8 board_num, LTC6813_CHG_GPIO_TYPE gpio_select, u16 adc_value_ptr[][12], u8 Ic_num)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
	u16 parsed_cell;
	u8 reg_count;
	u8 i = 0;
    //u8 cv_r[(LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE)*Ic_num];    // storage for the Auxiliary Register Group + PEC
    u8 data_counter = 0; //data counter
	u8 current_ic = 0;   //ic counter

    // Determine how many gpio ADC results were requested and where that data is located in register group
    if(LTC6813_CHG_GPIO_ALL == gpio_select)
    {
		for(reg_count = 2; reg_count < 4; reg_count++)
		{
			data_counter = 0;
			// Get the board address from the board number
			address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

			// Wakeup 6813 in case it has entered SLEEP or IDLE.
			ltc6813_wakeup();
			// Build the command code
			command_code = LTC6813_COMMAND_CODE_RDAUXA(address) + reg_count * 2 - 3;

			// Send the command code
			ltc6813_command_code_send(command_code, TRUE);

			success = LTC6813REGGROUPREAD(cv_r, Ic_num);

			if(success == TRUE)
			{
				for(current_ic = 0 ; current_ic < Ic_num; current_ic++) 				        // executes for every LTC6813 in the daisy chain
				{																 	  			// current_ic is used as the IC counter
					for(i = 0; i < 3; i++)   												    // This loop parses the read back data into cell voltages, it
					{														   		  			// loops once for each of the 3 cell voltage codes in the register
						parsed_cell = cv_r[data_counter] + (cv_r[data_counter+1]<<8);           //Each cell code is received as two bytes and is combined to
																								// create the parsed cell voltage code
						adc_value_ptr[current_ic][i + (reg_count * 3)] = 0x0000FFFF & parsed_cell;
						data_counter = data_counter + 2;     									//Because cell voltage codes are two bytes the data counter
																								//must increment by two for each parsed cell code
					}
					data_counter = data_counter + 2;
				}
			}
			else
			{
				return FALSE;
			}
		}
	}
    return success;
}

// Writes a string of bytes to the LTC6813 I2C port implemented on its GPIO pins.
void LTC6813_I2C_Write(u8 board_num, BOOLEAN start, BOOLEAN stop, u8* data_ptr, u16 num_bytes, u16 baud_khz)
{
    u8 address;
    u16 command_code;
    u8 comm[LTC6813_REGISTER_GROUP_SIZE];
    u8* comm_ptr;
    u8 bytes_to_send;
    u8 byte_num;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    while (num_bytes)
    {
        // Send the WRCOMM command code
        command_code = LTC6813_COMMAND_CODE_WRCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        // Build COMM Register Group
        bytes_to_send = 0;
        comm_ptr = comm;

        // First I2C byte might be a start or a stop, and will always transmit something (else we wouldn't be in this loop)
        *comm_ptr++ = ((start ? LTC6813_ICOM_I2C_WRITE_START : LTC6813_ICOM_I2C_WRITE_BLANK) << 4) + UPPER_NIBBLE(*data_ptr);
        *comm_ptr++ = (LOWER_NIBBLE(*data_ptr) << 4) + ((num_bytes == 1) && stop ? LTC6813_FCOM_WRITE_I2C_NACK_STOP : LTC6813_FCOM_WRITE_I2C_NACK);

        start = FALSE;
        data_ptr++;
        num_bytes--;
        bytes_to_send++;

        // Second and Third I2C byte might be a stop or no transmit.  Note that 1st byte is special case, and handled above this loop
        for(byte_num = 1; byte_num < LTC6813_NUM_COMM_BYTES_PER_REGISTER_GROUP; byte_num++)
        {
            *comm_ptr++ = ((num_bytes ? LTC6813_ICOM_I2C_WRITE_BLANK : LTC6813_ICOM_I2C_WRITE_NO_TRANSMIT) << 4) + UPPER_NIBBLE(*data_ptr);
            *comm_ptr++ = (LOWER_NIBBLE(*data_ptr) << 4) + ((num_bytes == 1) && stop ? LTC6813_FCOM_WRITE_I2C_NACK_STOP : LTC6813_FCOM_WRITE_I2C_NACK);
            if(num_bytes)
            {
                data_ptr++;
                num_bytes--;
                bytes_to_send++;
            }
        }

        // Write COMM Register Group
        ltc6813_register_group_write(comm);

        // Send the STRCOMM Command
        command_code = LTC6813_COMMAND_CODE_STCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        //Send the number clocks to send the COMM Register Group data to the I2C
        ltc6813_clock_out(bytes_to_send, baud_khz);
    }
}

// Writes one byte, and then reads a string of bytes to the LTC6813 I2C port implemented on its GPIO pins.
BOOLEAN LTC6813_I2C_Read(u8 board_num, BOOLEAN start, BOOLEAN stop, u8* data_ptr, u16 num_bytes, u16 baud_khz)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
    u8 comm[LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE];    // storage for the Comm Register Group + PEC
    u8* comm_ptr;
    u8 bytes_to_send;
    u8 byte_num;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    while (num_bytes)
    {
        // Send the WRCOMM command code
        command_code = LTC6813_COMMAND_CODE_WRCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        // Build COMM Register Group
        bytes_to_send = 0;
        comm_ptr = comm;

        // First I2C byte might be a start or a stop, and will always transmit something (else we wouldn't be in this loop)
        if(start)
        {
            *comm_ptr++ = (LTC6813_ICOM_I2C_WRITE_START << 4) + UPPER_NIBBLE(*data_ptr);
            *comm_ptr++ = (LOWER_NIBBLE(*data_ptr) << 4) + LTC6813_FCOM_WRITE_I2C_NACK;
        }
        else
        {
            *comm_ptr++ = ((LTC6813_ICOM_I2C_WRITE_BLANK) << 4) + UPPER_NIBBLE(LTC6813_COMM_READ_DUMMY);
            *comm_ptr++ = (LOWER_NIBBLE(LTC6813_COMM_READ_DUMMY) << 4) + ((num_bytes == 1) && stop ? LTC6813_FCOM_WRITE_I2C_NACK_STOP : LTC6813_FCOM_WRITE_I2C_ACK);
        }
        num_bytes--;
        bytes_to_send++;

        // Second and Third I2C byte might be a stop or no transmit.  Note that 1st byte is special case, and handled above this loop
        for(byte_num = 1; byte_num < LTC6813_NUM_COMM_BYTES_PER_REGISTER_GROUP; byte_num++)
        {
            *comm_ptr++ = ((num_bytes ? LTC6813_ICOM_I2C_WRITE_BLANK : LTC6813_ICOM_I2C_WRITE_NO_TRANSMIT) << 4) + UPPER_NIBBLE(LTC6813_COMM_READ_DUMMY);
            *comm_ptr++ = (LOWER_NIBBLE(LTC6813_COMM_READ_DUMMY) << 4) + ((num_bytes == 1) && stop ? LTC6813_FCOM_WRITE_I2C_NACK_STOP : LTC6813_FCOM_WRITE_I2C_ACK);
            if(num_bytes)
            {
                num_bytes--;
                bytes_to_send++;
            }
        }

        // Write COMM Register Group
        ltc6813_register_group_write(comm);

        // Send the STRCOMM Command
        command_code = LTC6813_COMMAND_CODE_STCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        //Send the number clocks to send the COMM Register Group data to the I2C
        ltc6813_clock_out(bytes_to_send, baud_khz);

        // Send the RDCOMM command code
        command_code = LTC6813_COMMAND_CODE_RDCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        // Read COMM register group and verify pec
        if(ltc6813_register_group_read(comm) == TRUE)
        {
            // Interpret data in register group according to datasheet Table 45
            comm_ptr = comm;

            while(bytes_to_send--)
            {
                *data_ptr = (((*comm_ptr) << 4) & 0xF0) + ((*(comm_ptr + 1)) >> 4);
                if(start)  // ignore the first byte, which is a readback of the i2c address sent after the start bit
                {
                    start = FALSE;
                }
                else
                {
                    data_ptr++;
                }
                comm_ptr += 2;
            }
        }
        else
        {
        	// LTC6813_CONFIG_ERROR_CRC(board_num, command_code, comm, sizeof(comm));
            num_bytes = 0;  // Bail out of command after first bad read.
            success = FALSE;
        }
    }

    return success;
}

// Writes a string of bytes to the LTC6813 SPI port implemented on its GPIO pins.
void LTC6813_SPI_Write(u8 board_num, BOOLEAN start, BOOLEAN stop, u8* data_ptr, u16 num_bytes, u16 baud_khz)
{
	u8 address;
    u16 command_code;
    u8 comm[LTC6813_REGISTER_GROUP_SIZE];
    u8* comm_ptr;
    u8 bytes_to_send;
    u8 byte_num;

    (void)start;
    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    while (num_bytes)
    {
        // Send the WRCOMM command code
        command_code = LTC6813_COMMAND_CODE_WRCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        // Build COMM Register Group
        bytes_to_send = 0;
        comm_ptr = comm;

        for(byte_num = 0; byte_num < LTC6813_NUM_COMM_BYTES_PER_REGISTER_GROUP; byte_num++)
        {
            *comm_ptr++ = ((num_bytes ? LTC6813_ICOM_SPI_WRITE_CSB_LOW : LTC6813_ICOM_SPI_WRITE_NO_TRANSMIT) << 4) + UPPER_NIBBLE(*data_ptr);
            *comm_ptr++ = (LOWER_NIBBLE(*data_ptr) << 4) + ((num_bytes <= 1) && stop ? LTC6813_FCOM_SPI_WRITE_CSB_HIGH : LTC6813_FCOM_SPI_WRITE_CSB_LOW);
            if(num_bytes)
            {
                data_ptr++;
                num_bytes--;
                bytes_to_send++;
            }
        }

        // Send COMM Register Group
        ltc6813_register_group_write(comm);

        // Send the STRCOMM Command
        command_code = LTC6813_COMMAND_CODE_STCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        //Send the number clocks to send the COMM Register Group data to the SPI
        ltc6813_clock_out(bytes_to_send, baud_khz);
    }

}

// Writes one byte, and then reads a string of bytes to the LTC6813 SPI port implemented on its GPIO pins.
BOOLEAN LTC6813_SPI_Read(u8 board_num, BOOLEAN start, BOOLEAN stop, u8* data_ptr, u16 num_bytes, u16 baud_khz)
{
    BOOLEAN success = TRUE;
    u8 address;
    u16 command_code;
    u8 comm[LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE];    // storage for the Comm Register Group + PEC
    u8* comm_ptr;
    u8 bytes_to_send;
    u8 byte_num;

    // Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_num);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    while (num_bytes)
    {
        // Send the WRCOMM command code
        command_code = LTC6813_COMMAND_CODE_WRCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        // Build COMM Register Group
        bytes_to_send = 0;
        comm_ptr = comm;

        // First SPI byte might be a start or a stop, and will always be something (else we wouldn't be in this loop)
        if(start)
        {
            *comm_ptr++ = (LTC6813_ICOM_SPI_WRITE_CSB_LOW << 4) + UPPER_NIBBLE(*data_ptr);
            *comm_ptr++ = (LOWER_NIBBLE(*data_ptr) << 4) + LTC6813_FCOM_SPI_WRITE_CSB_LOW;
        }
        else
        {
            *comm_ptr++ = (LTC6813_ICOM_SPI_WRITE_CSB_LOW << 4) + UPPER_NIBBLE(LTC6813_COMM_READ_DUMMY);
            *comm_ptr++ = (LOWER_NIBBLE(LTC6813_COMM_READ_DUMMY) << 4) + ((num_bytes <= 1) && stop ? LTC6813_FCOM_SPI_WRITE_CSB_HIGH : LTC6813_FCOM_SPI_WRITE_CSB_LOW);
        }
        num_bytes--;
        bytes_to_send++;

        for(byte_num = 1; byte_num < LTC6813_NUM_COMM_BYTES_PER_REGISTER_GROUP; byte_num++)
        {
            *comm_ptr++ = (LTC6813_ICOM_SPI_WRITE_CSB_LOW << 4) + UPPER_NIBBLE(LTC6813_COMM_READ_DUMMY);
            *comm_ptr++ = (LOWER_NIBBLE(LTC6813_COMM_READ_DUMMY) << 4) + ((num_bytes <= 1) && stop ? LTC6813_FCOM_SPI_WRITE_CSB_HIGH : LTC6813_FCOM_SPI_WRITE_CSB_LOW);
            if(num_bytes)
            {
                num_bytes--;
                bytes_to_send++;
            }
        }

        // Write COMM Register Group
        ltc6813_register_group_write(comm);

        // Send the STRCOMM Command
        command_code = LTC6813_COMMAND_CODE_STCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        //Send the number clocks to send the COMM Register Group data to the I2C
        ltc6813_clock_out(bytes_to_send, baud_khz);

        // Send the RDCOMM command code
        command_code = LTC6813_COMMAND_CODE_RDCOMM(address);
        ltc6813_command_code_send(command_code, TRUE);

        // Read COMM register group and verify pec
        if(ltc6813_register_group_read(comm) == TRUE)
        {
            // Interpret data in register group according to datasheet Table 45
            comm_ptr = comm;

            while(bytes_to_send--)
            {
                *data_ptr = (((*comm_ptr) << 4) & 0xF0) + ((*(comm_ptr + 1)) >> 4);
                if(start)  // ignore the first byte, which is a readback of the i2c address sent after the start bit
                {
                    start = FALSE;
                }
                else
                {
                    data_ptr++;
                }
                comm_ptr += 2;
            }
        }
        else
        {
            //LTC6813_CONFIG_ERROR_CRC(board_num, command_code, comm, sizeof(comm));
            num_bytes = 0;  // Bail out of command after first bad read.
            success = FALSE;
        }
    }

    return success;
}

//! Calculates the LTC6813 CRC over a string of bytes as per datasheet figure 22.
u16 LTC6813_PEC_Calc(u8 *data, int length)
{
    u16 remainder;
    u8 i = 0;

    remainder = ltc6813_pec_seed_value;

    for (i = 0; i < length; i++)
    {
        remainder = ltc6813_pec_lookup(data[i], remainder);
    }

    return (remainder * 2); //The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}

//! Calculates the LTC6813 CRC over a string of bytes as per datasheet figure 22.
u16 LTC6813_DaisyPEC_Calc(u8 Ic_num, u8 data[][6], int length)
{
    u16 remainder;
    u8 i = 0;

    remainder = ltc6813_pec_seed_value;

    for (i = 0; i < length; i++)
    {
        remainder = ltc6813_pec_lookup(data[Ic_num][i], remainder);
    }

    return (remainder * 2); //The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// Wakes up LTC6813 by lowering and raising CS as per datasheet Figure 21.
void ltc6813_wakeup(void)
{
    // Check timestamp to determine if a short or long delay is required after wakeup signal.
   // wakeup_timestamp_new = LTC6813_CONFIG_TIMER;
      //需要添加拉低选通
    LTC6813CSLOW();
    //需要添加延时1ms
    delay(200);
    //LTC6813_CONFIG_DELAY_US(LTC6813_TDWELL);
    //需要添加拉高选通
    LTC6813CSHIGH();
/*
    if((wakeup_timestamp_new - ltc6813_wakeup_timestamp) < (LTC6813_TSLEEP * LTC6813_CONFIG_TIMER_RESOLUTION / MS_PER_S))
    {
        // If wakeup signal sent less than LTC6813_TSLEEP time ago, then a short delay of LTC6813_TREADY is required.
        LTC6813_CONFIG_DELAY_US(LTC6813_TREADY);
    }
    else
    {
        // If wakeup signal sent more than LTC6813_TSLEEP time ago, then a long delay of LTC6813_TWAKE is required.
        LTC6813_CONFIG_DELAY_US(LTC6813_TWAKE);
    }

    ltc6813_wakeup_timestamp = wakeup_timestamp_new;
*/
    //需要添加延时TWAKE 300us
    delay(100);
    // LTC6813_CONFIG_DELAY_US(LTC6813_TWAKE);
    return;
}

// Returns adcopt and md values to achieve the desired sample rate.
void ltc6813_get_adcopt_and_md(u8 conversion_mode, u8* adcopt_ptr, u8* md_ptr)
{
    // Select adcopt for this sample rate.
    switch(conversion_mode)
    {
        case LTC6813_CONVERSION_27KHZ_MODE:
            *adcopt_ptr = LTC6813_ADCOPT_0;
            *md_ptr = LTC6813_MD_MODE_FAST;
            break;
        case LTC6813_CONVERSION_14KHZ_MODE:
            *adcopt_ptr = LTC6813_ADCOPT_1;
            *md_ptr = LTC6813_MD_MODE_FAST;
            break;
        case LTC6813_CONVERSION_7KHZ_MODE:
            *adcopt_ptr = LTC6813_ADCOPT_0;
            *md_ptr = LTC6813_MD_MODE_NORMAL;
            break;
        case LTC6813_CONVERSION_3KHZ_MODE:
            *adcopt_ptr = LTC6813_ADCOPT_1;
            *md_ptr = LTC6813_MD_MODE_NORMAL;
            break;
        case LTC6813_CONVERSION_2KHZ_MODE:
            *adcopt_ptr = LTC6813_ADCOPT_1;
            *md_ptr = LTC6813_MD_MODE_FILTERED;
            break;
        case LTC6813_CONVERSION_26HZ_MODE:
        default:
            *adcopt_ptr = LTC6813_ADCOPT_0;
            *md_ptr = LTC6813_MD_MODE_FILTERED;
             break;
    }

    return;
}

// Read/Modify/Write the Cfg Register to set the ADC option bit.
void ltc6813_adc_opt_set(u8 board_num, u8 adcopt)
{
    struct
    {
    	u8 mask[LTC6813_REGISTER_GROUP_SIZE];
    	u8 value[LTC6813_REGISTER_GROUP_SIZE];
    } register_modify;

    // Start with clear mask and value, and then set up gpio mask/value
    // memset(&register_modify.mask[1], 0, sizeof(register_modify) - 1);
    register_modify.mask[0] = LTC6813_CFGR0_GPIOx_MASK;
    register_modify.value[0] = LTC6813_CFGR0_GPIOx(ltc6813_gpio_pulldown);

    // Create Mask to set ADC Option
    register_modify.mask[0] |= LTC6813_CFGR0_ADCOPT_MASK;

    // Set Value for ADC Option
    register_modify.value[0] |= LTC6813_CFGR0_ADCOPT(adcopt);

    ltc6813_cfgr_modify(board_num, register_modify.mask, register_modify.value);

    // Remember last value set for ADC Option
    ltc6813_adcopt = adcopt;

    return;
}

// Writes a command to the LTC6813
void ltc6813_command_code_send(u16 command_code, BOOLEAN reg_group_command)
{
    u8 writebyte[LTC6813_COMMAND_SIZE];
    u8 pecdata[LTC6813_PEC_SIZE];
    u16 pec;
     //需要添加拉低选通
    // Pull CS low to start write
    LTC6813CSLOW();

    // Build the Command Code and Send
    writebyte[0] = UPPER_BYTE(command_code);
    writebyte[1] = LOWER_BYTE(command_code);
    //需要添加SPI发送接口
    LTC6813CMD_PEC_WRITE(writebyte);
    // LTC6813_CONFIG_SPI_BUFFER_SEND_START(writebyte, LTC6813_COMMAND_SIZE);

    // Calculate PEC and Send
    pec = LTC6813_PEC_Calc(writebyte, LTC6813_COMMAND_SIZE);
    pecdata[0] = UPPER_BYTE(pec);
    pecdata[1] = LOWER_BYTE(pec);
    //需要添加SPI发送接口
    LTC6813CMD_PEC_WRITE(pecdata);
    // LTC6813_CONFIG_SPI_BUFFER_SEND_START(&writebyte[LTC6813_COMMAND_SIZE], LTC6813_PEC_SIZE);

    // If no register group to follow, release CS to end write
    if (reg_group_command == FALSE)
    {
    	//需要添加等待SPI发送完成判断
    	//while(LTC6813_CONFIG_SPI_BUFFER_DONE() == FALSE);  // Wait for SPI transmission to be done before releasing CS
    	//需要添加拉高选通
    	LTC6813CSHIGH();             // End the communication
    }

    return;
}

u8 watchtemp[LTC6813_PEC_SIZE] = {0};
// Writes a register group to the LTC6813
void ltc6813_register_group_write(u8* register_group)
{
    u8 writebyte[LTC6813_PEC_SIZE];
    u16 pec;
    u8 address = 0;
    u16 command_code = 0;
        
	// Get the board address from the board number
    address = LTC6813_CONFIG_GET_BOARD_ADDRESS(0);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    // Build the command code
    command_code = LTC6813_COMMAND_CODE_WRCFG(address);

    // Send the command code
    ltc6813_command_code_send(command_code, TRUE);
    
  
    // Send register group bytes
    //需要添加SPI发送接口
    LTC6813REGWRITE(register_group);
    //LTC6813_CONFIG_SPI_BUFFER_SEND_START(register_group, LTC6813_REGISTER_GROUP_SIZE);

    // Calculate PEC and Send
    pec = LTC6813_PEC_Calc(register_group, LTC6813_REGISTER_GROUP_SIZE);
    writebyte[0] = UPPER_BYTE(pec);
    writebyte[1] = LOWER_BYTE(pec);
    watchtemp[0] = writebyte[0];
    watchtemp[1] = writebyte[1];
    //需要添加SPI发送接口              
    LTC6813CMD_PEC_WRITE(writebyte);
    //LTC6813_CONFIG_SPI_BUFFER_SEND_START(writebyte, LTC6813_PEC_SIZE);
     //需要添加等待SPI发送完成判断
    //while(LTC6813_CONFIG_SPI_BUFFER_DONE() == FALSE);  // Wait for SPI transmission to be done before releasing CS
    //需要添加拉高选通
    LTC6813CSHIGH();             // End the communication
 
    return;
}
// Writes a register group to the LTC6813
void ltc6813_registerB_group_write(u8* register_group)
{
    u8 writebyte[LTC6813_PEC_SIZE];
    u16 pec;
    //u8 address = 0;
    u16 command_code = 0;

	// Get the board address from the board number
    //address = LTC6813_CONFIG_GET_BOARD_ADDRESS(0);

    // Wakeup 6813 in case it has entered SLEEP or IDLE.
    ltc6813_wakeup();

    // Build the command code
    //command_code = LTC6813_COMMAND_CODE_WRCFG(address);
    command_code = 0x24;

    // Send the command code
    ltc6813_command_code_send(command_code, TRUE);


    // Send register group bytes
    //需要添加SPI发送接口
    LTC6813REGWRITE(register_group);
    //LTC6813_CONFIG_SPI_BUFFER_SEND_START(register_group, LTC6813_REGISTER_GROUP_SIZE);

    // Calculate PEC and Send
    pec = LTC6813_PEC_Calc(register_group, LTC6813_REGISTER_GROUP_SIZE);
    writebyte[0] = UPPER_BYTE(pec);
    writebyte[1] = LOWER_BYTE(pec);
    watchtemp[0] = writebyte[0];
    watchtemp[1] = writebyte[1];
    //需要添加SPI发送接口
    LTC6813CMD_PEC_WRITE(writebyte);
    //LTC6813_CONFIG_SPI_BUFFER_SEND_START(writebyte, LTC6813_PEC_SIZE);
     //需要添加等待SPI发送完成判断
    //while(LTC6813_CONFIG_SPI_BUFFER_DONE() == FALSE);  // Wait for SPI transmission to be done before releasing CS
    //需要添加拉高选通
    LTC6813CSHIGH();             // End the communication

    return;
}

// Reads a register group from the LTC6813 and verifies the PEC
BOOLEAN ltc6813_register_group_read(u8* register_group)
{
	u8 byte_num;
    u16 pec_calc;

    // Start to receive the register group bytes from the SPI.
    //需要添加SPI接收接口
   // LTC6813_CONFIG_SPI_BUFFER_RECEIVE_START(register_group, (LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE));

    // Initialize PEC calculation
    pec_calc =  ltc6813_pec_seed_value;

    // Read register group bytes one at a time, so that the PEC can be calculated as they're being received.
    for (byte_num = 0; byte_num < LTC6813_REGISTER_GROUP_SIZE; byte_num++)
    {
		//需要添加SPI接收完成判断
		//while(LTC6813_CONFIG_SPI_BUFFER_RECEIVE_BYTES_AVAILABLE(&register_group[byte_num]) == 0);  // Wait for a byte to be received
        pec_calc =  ltc6813_pec_lookup(register_group[byte_num], pec_calc);         // Calculate PEC for this byte
    }

    // Complete PEC calculation
    pec_calc <<=  1;      //The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2

    // Wait for PEC bytes to be received
    //需要添加SPI接收完成判断
    //while(LTC6813_CONFIG_SPI_BUFFER_RECEIVE_BYTES_AVAILABLE(&register_group[LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE - 1]) == 0);

    // End the communication
    //需要添加拉高选通
    LTC6813CSHIGH();

    // Verify PEC and return result
    if((UPPER_BYTE(pec_calc) != register_group[LTC6813_REGISTER_GROUP_SIZE]) ||
       (LOWER_BYTE(pec_calc) != register_group[LTC6813_REGISTER_GROUP_SIZE + 1]))
    {
        return FALSE;
    }
    return TRUE;
}


// Clocks a number of bytes out of the i2c/spi at a set output baud rate, and then returns the baud rate to that for the LTC6813
void ltc6813_clock_out(u16 bytes_to_send, u16 baud_khz)
{
    /*int_8 dummy;
    int_8 byte_num;
  
    while(LTC6813_CONFIG_SPI_BUFFER_DONE() == FALSE);  // Wait for SPI transmission to be done before changing the baud rate

    // Set baud rate to input value for LTC6813, that results in the desired output baud rate.
    LTC6813_CONFIG_SPI_SET_BAUD( MIN(baud_khz * LTC6813_BAUD_RATE_DIVISOR, LTC6813_BAUD_RATE) );

    dummy = 0xFF;   // Keep transmit lines high as you provide clock for STCOMM command.

    //Send the number of clocks needed to send bytes (3 * 8 * number of bytes) in COMM register group
    for (byte_num = 0; byte_num < 3 * bytes_to_send; byte_num++)
    {
        LTC6813_CONFIG_SPI_BUFFER_SEND_START(&dummy, sizeof(dummy));
    }

    while(LTC6813_CONFIG_SPI_BUFFER_DONE() == FALSE);  // Wait for SPI transmission to be done before releasing CS

    // End the communication
    //需要添加拉高选通
    LTC6813CSHIGH();

    // Return baud rate to max allowed by the LTC6813
    LTC6813_CONFIG_SPI_SET_BAUD(LTC6813_BAUD_RATE);
   */

    (void)bytes_to_send;
    (void)baud_khz;
    
    return;
}

// Reads, Modifies, and Writes bits in the Configuration Register Group.
// If a broadcast is desired, then this code actually implemented as some of the bits in the Configuration Register Group are unique to each board.
void ltc6813_cfgr_modify(u8 board_num, u8* register_mask_ptr, u8* register_value_ptr)
{
	u8 address;
    u16 command_code;
    u8 cfgr[LTC6813_REGISTER_GROUP_SIZE + LTC6813_PEC_SIZE];    // storage for the Configuration Register Group + PEC
    u8 byte_num;
    u8 board_loop_num;
    u8 board_limit_num;

    // Since broadcast reads can't be performed in the 6813-2, this must be handled with a loop.
    if(board_num == LTC6813_BROADCAST)
    {
        board_loop_num = 0;
        board_limit_num = 1;
    }
    else
    {
        board_loop_num = board_num;
        board_limit_num = board_num;
    }

    do
    {
        // Get the board address from the board number
        address = LTC6813_CONFIG_GET_BOARD_ADDRESS(board_loop_num);

        // Wakeup 6813 in case it has entered SLEEP or IDLE.
        ltc6813_wakeup();

        // Send the RDCFG command code.
        command_code = LTC6813_COMMAND_CODE_RDCFG(address);
        ltc6813_command_code_send(command_code, TRUE);

        // Read the Configuration Register Group
        // If PEC is correct for read data, modify masked bits and write back
        if(ltc6813_register_group_read(cfgr) == TRUE)
        {

            // Modify the Configuration Register Group, with only the masked bits modified
            for (byte_num = 0; byte_num < LTC6813_REGISTER_GROUP_SIZE; byte_num++)
            {
                cfgr[byte_num] &= ~(*(register_mask_ptr + byte_num));
                cfgr[byte_num] |= (*(register_value_ptr + byte_num));
            }

            // Send the WRCFG command code
            command_code = LTC6813_COMMAND_CODE_WRCFG(address);
            ltc6813_command_code_send(command_code, TRUE);

            // Send Configuration Register Group Write
            ltc6813_register_group_write(cfgr);
        }
        else
        {
			//LTC6813_CONFIG_ERROR_CRC(board_loop_num, command_code, cfgr, sizeof(cfgr));
            board_loop_num = board_limit_num = 0;  // Bail out of command after first bad read.
        }

        board_loop_num++;
    } while (board_loop_num < board_limit_num);

    return;
}

// calculates the pec for one byte, and returns the intermediate calculation
u16 ltc6813_pec_lookup(char data, u16 remainder)
{
    u8 addr;

    addr = ((remainder >> 7) ^ data) & 0xff;    //calculate PEC table address
    remainder = (remainder << 8) ^ ltc6813_pec_table[addr];             //get value from CRC15Table;

    return remainder;
}

void ltcSpi_port_Select(u8 port)
{
	ltcSpi_Port = port;
}

//=============================================================================================
//函数名称	: static void LTC6813CSLOW(void)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: 选通6813
//注		意	: 低电平选通有效
//=============================================================================================
static void LTC6813CSLOW(void)
{
	if(1 == ltcSpi_Port)//B口
	{
		(void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_ON);
		//(void)DEVGPIOSetPortState(ePOut_LTC6820EN1, ePSTATE_ON);      /*LTC6820使能(低有效)*/
		(void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_OFF);
	}
	else
	{
		(void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_OFF);
		//(void)DEVGPIOSetPortState(ePOut_LTC6820EN2, ePSTATE_ON);      /*LTC6820使能(低有效)*/
		(void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_ON);
	}
}

//=============================================================================================
//函数名称	: static void LTC6813CSHIGH(void)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: 禁止6813
//注		意	: 高电平禁用
//=============================================================================================
static void LTC6813CSHIGH(void)
{
	if(1 == ltcSpi_Port)//B口
	{
		(void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_ON);
		//(void)DEVGPIOSetPortState(ePOut_LTC6820EN2, ePSTATE_ON);      /*LTC6820使能(低有效)*/
	}
	else
	{
		(void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_ON);
		//(void)DEVGPIOSetPortState(ePOut_LTC6820EN1, ePSTATE_ON);      /*LTC6820使能(低有效)*/
	}
}

//=============================================================================================
//函数名称	: static void LTC6813CMD_PEC_WRITE(void)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: SPI写入6813控制命令或者PEC
//注		意	:
//=============================================================================================
static void LTC6813CMD_PEC_WRITE(u8* commanddata)
{
	static u8 errNum = 0;
     
	 //SPI发送信息
    (void)BSPSPISendData(eSPI2, commanddata[0]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		
		if(errNum > 10)
		{
			return;
		}
    }
    (void)BSPSPIGetRcvData(eSPI2);
    
	errNum = 0;
	
    (void)BSPSPISendData(eSPI2, commanddata[1]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		
		if(errNum > 10)
		{
			return;
		}
    }
    (void)BSPSPIGetRcvData(eSPI2);
}

//=============================================================================================
//函数名称	: static void LTC6813REGWRITE(void)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: SPI写入6813控制命令或者PEC
//注		意	:
//=============================================================================================
static void LTC6813REGWRITE(u8* commanddata)
{
	static u8 errNum = 0;
	u8 rcvdata[6] = {0};
     
	 //SPI发送信息
   (void)BSPSPISendData(eSPI2,commanddata[0]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[0] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2,commanddata[1]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[1] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2,commanddata[2]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[2] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;    
    (void)BSPSPISendData(eSPI2,commanddata[3]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[3] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2,commanddata[4]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[4] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2,commanddata[5]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[5] = BSPSPIGetRcvData(eSPI2);
}

//=============================================================================================
//函数名称	: static void LTC6813DaisyREGWRITE(u8 Ic_num, u8 commanddata[][6])
//输入参数	: Ic_reg:寄存器个数      commanddata[][6]:需要发送的数据
//输出参数	: void
//静态变量	: void
//功		能	: SPI写入6813控制命令或者PEC(针对菊花链)
//注		意	:
//=============================================================================================
static void LTC6813DaisyREGWRITE(u8 Ic_reg, u8 commanddata[][6])
{
	static u8 errNum = 0;
	u8 rcvdata[6] = {0};
     
	 //SPI发送信息
    (void)BSPSPISendData(eSPI2,commanddata[Ic_reg][0]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[0] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2,commanddata[Ic_reg][1]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[1] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2,commanddata[Ic_reg][2]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[2] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;    
    (void)BSPSPISendData(eSPI2,commanddata[Ic_reg][3]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[3] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2,commanddata[Ic_reg][4]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[4] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2,commanddata[Ic_reg][5]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    rcvdata[5] = BSPSPIGetRcvData(eSPI2);
}

//=============================================================================================
//函数名称	: static BOOLEAN LTC6813REGGROUPREAD(u8* adcdata, u8 Ic_num)
//输入参数	: void  
//输出参数	: void
//静态变量	: void
//功		能	: 读取一组ADC电压寄存器，包括3个电压数据和1个PEC数据
//注		意	:
//=============================================================================================
static BOOLEAN LTC6813REGGROUPREAD(u8* adcdata, u8 Ic_num)
{
	static u8 errNum = 0;
	//u8 temp = 0;
	u8 i = 0;
	u16 pec_calc = 16;
 
	LTC6813CSLOW();    
//	errNum = 0;
//    //SPI发送信息
//    (void)BSPSPISendData(eSPI2, 0xff);//第一个数据多读取一次，否则数据出错(待解决)
//    //delay(delay1);
//    while(BSPSPIGetWorkState(eSPI2))
//    {
//		errNum++;
//		if(errNum > 10)
//		{
//			errNum = 0;
//			return FALSE;
//		}
//    }
//    adcdata[0] = BSPSPIGetRcvData(eSPI2);

	for(i = 0; i < Ic_num * 8; i++)
	{
		//delay(100);
		//SPI发送信息
		(void)BSPSPISendData(eSPI2,0xff);
		//delay(delay2);
		while(BSPSPIGetWorkState(eSPI2))
		{
			errNum++;
			if(errNum > 10)
			{
				return FALSE;
			}		
		}
		adcdata[i] = BSPSPIGetRcvData(eSPI2);
		delay(10);
	}
	   
    for(i = 0; i < 6; i++)//只校验第一个从件的数据，菊花链中的第一个从件出现异常时，在其之后也出现异常
    {
        pec_calc =  ltc6813_pec_lookup(adcdata[i], pec_calc);         // Calculate PEC for this byte 
    }
	
	// Complete PEC calculation
    pec_calc <<=  1;      //The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2

	// End the communication
    LTC6813CSHIGH();

    // Verify PEC and return result
    if((UPPER_BYTE(pec_calc) != adcdata[6]) 
      ||(LOWER_BYTE(pec_calc) != adcdata[7]))
    {
        //return FALSE;
    }
    return TRUE;
}

//=============================================================================================
//函数名称	: void LTC6813_DaisyWriteCfgA(u8 total_ic, u8 config[][6])
//输入参数	: total_ic:菊花链上从件个数   config[][6]:需发送的数据
//输出参数	: 应温度下标值
//函数功能	: LTC6813写寄存器A函数
//注意事项	:
//=============================================================================================
void LTC6813_DaisyWriteCfgA(u8 total_ic, u8 config[][6])
{
	s8 current_ic = 0;
    u8 writebyte[LTC6813_PEC_SIZE];
    u16 pec;
    u8 address = 0;
    u16 command_code = 0;

 	LTC6813CSLOW();
	// Get the board address from the board number
	address = LTC6813_CONFIG_GET_BOARD_ADDRESS(0);

	// Wakeup 6804 in case it has entered SLEEP or IDLE.
	ltc6813_wakeup();

	// Build the command code
	command_code = LTC6813_COMMAND_CODE_WRCFG(address);

	// Send the command code
	ltc6813_command_code_send(command_code, TRUE);

	for(current_ic = (total_ic - 1); current_ic >= 0; current_ic--) 	        // executes for each LTC6804 in daisy chain, this loops starts with
    {																			// the last IC on the stack. The first configuration written is
																				// received by the last IC in the daisy chain
		// Send register group bytes
		//需要添加SPI发送接口
		LTC6813DaisyREGWRITE(current_ic, config);

		// Calculate PEC and Send
		pec = LTC6813_DaisyPEC_Calc(current_ic, config, LTC6813_REGISTER_GROUP_SIZE);
		writebyte[0] = UPPER_BYTE(pec);
		writebyte[1] = LOWER_BYTE(pec);
		watchtemp[0] = writebyte[0];
		watchtemp[1] = writebyte[1];
		//需要添加SPI发送接口
		LTC6813CMD_PEC_WRITE(writebyte);
		//LTC6804_CONFIG_SPI_BUFFER_SEND_START(writebyte, LTC6804_PEC_SIZE);
		 //需要添加等待SPI发送完成判断
		//while(LTC6804_CONFIG_SPI_BUFFER_DONE() == FALSE);  // Wait for SPI transmission to be done before releasing CS
		//需要添加拉高选通
		//LTC6804CSHIGH();             // End the communication
	}
	LTC6813CSHIGH();                   // End the communication
}

//=============================================================================================
//函数名称	: void LTC6813_DaisyWriteCfgB(u8 total_ic, u8 config[][6])
//输入参数	: total_ic:菊花链上从件个数   config[][6]:需发送的数据
//输出参数	: 应温度下标值
//函数功能	: LTC6813写寄存器B函数
//注意事项	:
//=============================================================================================
void LTC6813_DaisyWriteCfgB(u8 total_ic, u8 config[][6])
{
	s8 current_ic = 0;
    u8 writebyte[LTC6813_PEC_SIZE];
    u16 pec;
    //u8 address = 0;
    u16 command_code = 0;

 	LTC6813CSLOW();
	// Get the board address from the board number
	//address = LTC6813_CONFIG_GET_BOARD_ADDRESS(0);

	// Wakeup 6804 in case it has entered SLEEP or IDLE.
	ltc6813_wakeup();

	// Build the command code
	command_code = 0x24;

	// Send the command code
	ltc6813_command_code_send(command_code, TRUE);

	for(current_ic = (total_ic - 1); current_ic >= 0; current_ic--) 	        // executes for each LTC6804 in daisy chain, this loops starts with
    {																			// the last IC on the stack. The first configuration written is
																				// received by the last IC in the daisy chain
		// Send register group bytes
		//需要添加SPI发送接口
		LTC6813DaisyREGWRITE(current_ic, config);

		// Calculate PEC and Send
		pec = LTC6813_DaisyPEC_Calc(current_ic, config, LTC6813_REGISTER_GROUP_SIZE);
		writebyte[0] = UPPER_BYTE(pec);
		writebyte[1] = LOWER_BYTE(pec);
		watchtemp[0] = writebyte[0];
		watchtemp[1] = writebyte[1];
		//需要添加SPI发送接口
		LTC6813CMD_PEC_WRITE(writebyte);
		//LTC6804_CONFIG_SPI_BUFFER_SEND_START(writebyte, LTC6804_PEC_SIZE);
		 //需要添加等待SPI发送完成判断
		//while(LTC6804_CONFIG_SPI_BUFFER_DONE() == FALSE);  // Wait for SPI transmission to be done before releasing CS
		//需要添加拉高选通
		//LTC6804CSHIGH();             // End the communication
	}
	LTC6813CSHIGH();                   // End the communication
}

//=============================================================================================
//函数名称	: void delay(u32 times)
//输入参数	: times:延时 
//输出参数	: void
//静态变量	: void
//功	能	: 延时函数
//注	意	:  
//=============================================================================================
void delay(u32 times)
{   
    if(times == 0) 
    {
      return;  
    }
    
    while(times--)
    {
        ;
    }
}
