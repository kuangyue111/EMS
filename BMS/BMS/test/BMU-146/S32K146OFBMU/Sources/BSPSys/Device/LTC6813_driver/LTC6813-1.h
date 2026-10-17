/*
 Linear Technology DC2100A Demonstration Board.
 API Header File for LTC6813-2 Multicell Battery Monitors.
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
 $Revision: 693 $
 $Date: 2014-09-08 13:52:04 -0400 (Mon, 08 Sep 2014) $

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

/*! @file
    @ingroup LTC6813-2
    API Header File for LTC6813-2 Multicell Battery Monitors.
*/

#ifndef _LTC6813_H
#define _LTC6813_H

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Includes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "BSPTypeDef.h"
//#include "DC2100A.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define LTC6813_PUP_DISENABLE       0           // PUP Not Permitted
#define LTC6813_PUP_ENABLE          1           // PUP Permitted

//! @name LTC6813-2 Driver Properties
//! @{
//!
#define LTC6813_MAX_BOARDS                  25                  //!< The maximum number of addresses available to the LTC6813-2
#define LTC6813_BROADCAST                   LTC6813_MAX_BOARDS  //!< Code for application code to indicate an LTC6813 command is to be broadcast to all boards
#define LTC6813_NUM_CELLV_ADC               18                  //!< Number of cell voltage ADC measurements
#define LTC6813_NUM_GPIO                    9                   //!< Number of GPIO pins available on LTC6813
#define LTC6813_NUM_AUX_ADC                 6                   //!< Number of auxiliary ADC measurements
#define LTC6813_ADC_CLEAR                   0xFFFF              //!< ADC Value returned when results are cleared, but not retaken.
//! @}

//! @name Cell Selections
//! @{
//! Cell selection for ADC Conversion from datasheet table 35.
typedef enum {
    LTC6813_CH_ALL = 0,                 //!< All Cells
    LTC6813_CH_CELL_1_AND_7 = 1,        //!< Cell 1 and Cell 7 and 13
    LTC6813_CH_CELL_2_AND_8 = 2,        //!< Cell 2 and Cell 8 and 14
    LTC6813_CH_CELL_3_AND_9 = 3,        //!< Cell 3 and Cell 9 and 15
    LTC6813_CH_CELL_4_AND_10 = 4,       //!< Cell 4 and Cell 10 and 16
    LTC6813_CH_CELL_5_AND_11 = 5,       //!< Cell 5 and Cell 11 and 17
    LTC6813_CH_CELL_6_AND_12 = 6,       //!< Cell 6 and Cell 12 and 18
    LTC6813_CH_CELL_NUM
} LTC6813_CH_CELL_TYPE;
//! @}

//! @name GPIO Selections
//! @{
//! GPIO for ADC Conversion from datasheet table 35.
typedef enum {
    LTC6813_CHG_GPIO_ALL = 0,       //!< GPIO 1-5, 2nd Ref, GPIO 6-9
    LTC6813_CHG_GPIO1 = 1,          //!< GPIO 1
    LTC6813_CHG_GPIO2 = 2,          //!< GPIO 2
    LTC6813_CHG_GPIO3 = 3,          //!< GPIO 3
    LTC6813_CHG_GPIO4 = 4,          //!< GPIO 4
    LTC6813_CHG_GPIO5 = 5,          //!< GPIO 5
    LTC6813_CHG_GPIO_2ND_REF = 6,   //!< 2nd Reference
    LTC6813_CHG_GPIO_NUM
} LTC6813_CHG_GPIO_TYPE;
//! @}

//! @name Status Group Selections
//! @{
//! Status Group Selections from datasheet table 35.
typedef enum {
    LTC6813_CHST_ALL = 0,           //!< SOC, ITMP, VA, VD
    LTC6813_CHST_SOC = 1,           //!< SOC
    LTC6813_CHST_ITMP = 2,          //!< ITMP
    LTC6813_CHST_VA = 3,            //!< VA
    LTC6813_CHST_VD = 4,            //!< VD
    LTC6813_CHST_NUM
} LTC6813_CHST_TYPE;
//! @}

//! @name Conversion Modes
//! @{
//! Conversion Modes Available in the LTC6813 from datasheet table 35.
typedef enum {
    LTC6813_CONVERSION_27KHZ_MODE = 0,  //!< 27kHz conversion mode
    LTC6813_CONVERSION_14KHZ_MODE = 1,  //!< 14kHz conversion mode
    LTC6813_CONVERSION_7KHZ_MODE = 2,   //!< 7kHz conversion mode
    LTC6813_CONVERSION_3KHZ_MODE = 3,   //!< 3kHz conversion mode
    LTC6813_CONVERSION_2KHZ_MODE = 4,   //!< 2kHz conversion mode
    LTC6813_CONVERSION_26HZ_MODE = 5,   //!< 26Hz conversion mode
    LTC6813_CONVERSION_NUM_MODES
} LTC6813_CONVERSION_MODE_T;
//! @}

//! @name Conversion Delays
//! @{
//! Reference wakeup time from datasheet Electrical Characteristics page 7.
//! Time for ADC results for one channel to be ready, from Table 6 (t1C).
//! Time for ADC results for all channels to be ready, from Table 5 (t6C).
#define LTC6813_TREFUP                       4400       //!< in us, max reference wakeup time.
#define LTC6813_CONVERSION_27KHZ_DELAY       201        //!< in us, delay between sampling and reading ADC in 27kHz conversion mode
#define LTC6813_CONVERSION_14KHZ_DELAY       230        //!< in us, delay between sampling and reading ADC in 14kHz conversion mode
#define LTC6813_CONVERSION_7KHZ_DELAY        405        //!< in us, delay between sampling and reading ADC in 7kHz conversion mode
#define LTC6813_CONVERSION_3KHZ_DELAY        501        //!< in us, delay between sampling and reading ADC in 3kHz conversion mode
#define LTC6813_CONVERSION_2KHZ_DELAY        754        //!< in us, delay between sampling and reading ADC in 2kHz conversion mode
#define LTC6813_CONVERSION_26HZ_DELAY        34         //!< in ms, delay between sampling and reading ADC in 26Hz conversion mode
#define LTC6813_CONVERSIONS_ALL_27KHZ_DELAY  11130      //!< in us, delay between sampling and reading ADC in 27kHz conversion mode
#define LTC6813_CONVERSIONS_ALL_14KHZ_DELAY  1288       //!< in us, delay between sampling and reading ADC in 14kHz conversion mode
#define LTC6813_CONVERSIONS_ALL_7KHZ_DELAY   2335       //!< in us, delay between sampling and reading ADC in 7kHz conversion mode
#define LTC6813_CONVERSIONS_ALL_3KHZ_DELAY   3033       //!< in us, delay between sampling and reading ADC in 3kHz conversion mode
#define LTC6813_CONVERSIONS_ALL_2KHZ_DELAY   4430       //!< in us, delay between sampling and reading ADC in 2kHz conversion mode
#define LTC6813_CONVERSIONS_ALL_26HZ_DELAY   202        //!< in ms, delay between sampling and reading ADC in 26Hz conversion mode
//! @}

//! @name Voltage Resolution
//! @{
//! Voltage resolutions from Table 46 (t6C)
#define LTC6813_VOLTAGE_RESOLUTION           100        //!< uV per bit (CxV), cell voltage resolution.
#define LTC6813_UVOV_RESOLUTION              (16*100)   //!< uV per bit (VUV and VOV), under-voltage and over-voltage resolution.
#define LTC6813_SOC_RESOLUTION               (20*100)   //!< uV per bit (SOC), sum-of-cells resolution.
#define LTC6813_V_SUPPLY_VOLTAGE             11000      //!< mV per bit, Minimum supply voltage from datasheet page 7.
//! @}

//! @name Communication Protocol Constants
//! @{
//! Communication Protocol Message Sizes, not including PEC, as specified by datasheet Table 26.
//! Baud Rate from from datasheet page 8
#define LTC6813_COMMAND_SIZE                 2              //!< bytes per command
#define LTC6813_REGISTER_GROUP_SIZE          6              //!< bytes per register group
#define LTC6813_PEC_SIZE                     2  			//!< 15 bit PEC, requires u16 data type
#define LTC6813_ADC_SIZE                     2  			//!< 16 bit ADC results
#define LTC6813_BAUD_RATE                    1000           //!< in kHz, Max input SPI Frequency (1/tCLK from datasheet)
#define LTC6813_COMMAND_TIME         (1LL * (LTC6813_COMMAND_SIZE + LTC6813_PEC_SIZE) * \
                                      BITS_PER_BYTE * US_PER_MS / LTC6813_BAUD_RATE)  //!< minimum time necessary to send a command in us
#define LTC6813_REGISTER_GROUP_TIME  (1LL * (LTC6813_COMMAND_SIZE + LTC6813_REGISTER_GROUP_SIZE + 2 * LTC6813_PEC_SIZE) * \
                                      BITS_PER_BYTE * US_PER_MS / LTC6813_BAUD_RATE)  //!< minimum time necessary to perform a register group read or write in us
//! @}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Prototypes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//! Initializes the LTC6813-2 code module.
//! @return void
void LTC6813_Init(void);

//! Gets the LTC6813 revision.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_Revision_Get(u8 board_num,              //!< The logical address for the PCB containing this LTC6813-2 IC.
                             u8* revision               //!< Pointer where to return revision.
                             );

//! Sets the LTC6813 GPIO Pull Downs.
//! Write: 0 -> GPIOx Pin Pull-Down ON; 1-> GPIOx Pin Pull-Down OFF
//! @return void
void LTC6813_GPIO_Set(u8 board_num,                     //!< The logical address for the PCB containing this LTC6813-2 IC.
					  u8 gpio_bitmap                    //!< bitmap for GPIO pull-downs, bit0 = GPIO 0
                      );

//! Gets the LTC6813 ADC Reference status, where 1 = ON and 0 = OFF.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_Refon_Get(u8 board_num,                 //!< The logical address for the PCB containing this LTC6813-2 IC.
						 u8* refon                      //!< location where to return the ADC Reference status.
                          );


//! Turns the LTC6813 ADC Reference on and off.
//! @return void
void LTC6813_Refon_Set(u8 board_num,                    //!< The logical address for the PCB containing this LTC6813-2 IC.
                       BOOLEAN refon                    //!< TRUE to turn on the reference, FALSE to turn it off.
                       );

//! Sets the LTC6813 discharger pin levels and timeout values.
//! @return void
void LTC6813_Dischargers_Set(u8 board_num,                  //!< The logical address for the PCB containing this LTC6813-2 IC.
							 u16 discharge_bitmap,          //!< bitmap for discharger, bit0 = cell 0.
							 u16 timeout_value              //!< timeout value for dischargers.
                             );

//! Sets the LTC6813 under-voltage and over-voltage thresholds in LTC6813_UVOV_RESOLUTION units.
//! @return void
void LTC6813_UVOV_Thresholds_Set(u8 board_num,              //!< The logical address for the PCB containing this LTC6813-2 IC.
								 u16 vuv_value,             //!< Under-voltage value.
								 u16 vov_value              //!< Over-voltage value.
                                 );

//! Gets the LTC6813 under-voltage and over-voltage thresholds in LTC6813_UVOV_RESOLUTION units.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_UVOV_Thresholds_Get(u8 board_num,           //!< The logical address for the PCB containing this LTC6813-2 IC.
                                   u16* vuv_value,          //!< Pointer where to return under-voltage value.
                                   u16* vov_value           //!< Pointer where to return over-voltage value.
                                   );

//! Gets the LTC6813 flags indicating under-voltage and over-voltage conditions are present.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_UVOV_Flags_Get(u8 board_num,                //!< The logical address for the PCB containing this LTC6813-2 IC.
							   u16* vuv_flags,              //!< Pointer where to return under-voltage flags in a bitmap with bit 0 = cell 0.
							   u16* vov_flags               //!< Pointer where to return over-voltage flags in a bitmap with bit 0 = cell 0.
                               );

//! Clears the LTC6813 Cell Voltage ADC registers.  This is useful to detect if the conversion was started properly when the results are read.
//! @return void
void LTC6813_Cell_ADC_Clear(u8 board_num                   //!< The logical address for the PCB containing this LTC6813-2 IC.
                            );

//! Starts the LTC6813 Cell Voltage ADC conversion at the specified conversion mode.
//! Note function always permits discharge.
//! @return void
void LTC6813_Cell_ADC_Start(u8 board_num,                       //!< The logical address for the PCB containing this LTC6813-2 IC.
                            LTC6813_CONVERSION_MODE_T mode,     //!< The mode to use for ADC conversion.
                            LTC6813_CH_CELL_TYPE cell_select,   //!< The cells to convert.
                            BOOLEAN discharge_permitted         //!< True if discharge is to be permitted during this cell voltage conversion.
                            );
                            
void LTC6813_Cell_ADC_ADOW(u8 board_num,                        //!< The logical address for the PCB containing this LTC6813-2 IC.
                            LTC6813_CONVERSION_MODE_T mode,     //!< The mode to use for ADC conversion.
                            LTC6813_CH_CELL_TYPE cell_select,   //!< The cells to convert.
                            BOOLEAN discharge_permitted,        //!< True if discharge is to be permitted during this cell voltage conversion.
                            u8 pup
                            );

//! Reads the LTC6813 Cell Voltage ADC conversion results.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_Cell_ADC_Read(                      //!< The logical address for the PCB containing this LTC6813-2 IC.
                              LTC6813_CH_CELL_TYPE cell_select,     //!< The cells that were converted.
                              u16 adc_value_ptr[][18],         //!< Pointer where up to LTC6813_NUM_CELLV_ADC cell voltages will be returned.
                              u8 IC_num,
							  u8 reg_Num
							 );

//! Reads the LTC6813 Cell Voltage ADC conversion results.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_Cell_ADC_ReadAB(                      //!< The logical address for the PCB containing this LTC6813-2 IC.
                              LTC6813_CH_CELL_TYPE cell_select,     //!< The cells that were converted.
                              u16 adc_value_ptr[][18],         //!< Pointer where up to LTC6813_NUM_CELLV_ADC cell voltages will be returned.
                              u8 IC_num
							 );

//! Reads the LTC6813 Cell Voltage ADC conversion results.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_Cell_ADC_ReadCD(                      //!< The logical address for the PCB containing this LTC6813-2 IC.
                              LTC6813_CH_CELL_TYPE cell_select,     //!< The cells that were converted.
                              u16 adc_value_ptr[][18],         //!< Pointer where up to LTC6813_NUM_CELLV_ADC cell voltages will be returned.
                              u8 IC_num
							 );

//! Reads the LTC6813 Cell Voltage ADC conversion results.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_Cell_ADC_ReadEF(                      //!< The logical address for the PCB containing this LTC6813-2 IC.
                              LTC6813_CH_CELL_TYPE cell_select,//!< The cells that were converted.
                              u16 adc_value_ptr[][18],         //!< Pointer where up to LTC6813_NUM_CELLV_ADC cell voltages will be returned.
                              u8 IC_num
							 );

//! Clears the LTC6813 GPIO ADC registers.  This is useful to detect if the conversion was started properly when the results are read.
//! @return void
void LTC6813_GPIO_ADC_Clear(u8 board_num                       //!< The logical address for the PCB containing this LTC6813-2 IC.
                           );

//! Starts the specified LTC6813 GPIO ADC conversion at the specified conversion mode.
//! @return void
void LTC6813_GPIO_ADC_Start(u8 board_num,                           //!< The logical address for the PCB containing this LTC6813-2 IC.
                            LTC6813_CONVERSION_MODE_T mode,         //!< The mode to use for ADC conversion.
                            LTC6813_CHG_GPIO_TYPE gpio_select       //!< The gpio to convert from LTC6813_CHG GPIO Selections.
                            );

//! Reads the specified LTC6813 GPIO ADC conversion results.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_GPIO_ADC_Read(u8 board_num,                          //!< The logical address for the PCB containing this LTC6813-2 IC.
                              LTC6813_CHG_GPIO_TYPE gpio_select,     //!< The gpio that was converted.
                              u16 adc_value_ptr[][12],                //!< Pointer where up to LTC6813_NUM_GPIO GPIO ADC results will be returned.
                              u8 Ic_num  
							);

//! Reads the specified LTC6813 GPIO ADC conversion results.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_GPIO_ADC_ReadAB(u8 board_num,                        //!< The logical address for the PCB containing this LTC6813-2 IC.
                              LTC6813_CHG_GPIO_TYPE gpio_select,     //!< The gpio that was converted.
                              u16 adc_value_ptr[][12],                //!< Pointer where up to LTC6813_NUM_GPIO GPIO ADC results will be returned.
                              u8 Ic_num
							);

//! Reads the specified LTC6813 GPIO ADC conversion results.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_GPIO_ADC_ReadCD(u8 board_num,                        //!< The logical address for the PCB containing this LTC6813-2 IC.
                              LTC6813_CHG_GPIO_TYPE gpio_select,     //!< The gpio that was converted.
                              u16 adc_value_ptr[][12],                //!< Pointer where up to LTC6813_NUM_GPIO GPIO ADC results will be returned.
                              u8 Ic_num
							);

//! Writes a string of bytes to the LTC6813 I2C port implemented on its GPIO pins.
//! @return void
void LTC6813_I2C_Write(u8 board_num,                    //!< The logical address for the PCB containing this LTC6813-2 IC.
                       BOOLEAN start,                   //!< TRUE if this write would be started by an I2C Start Condition.
                       BOOLEAN stop,                    //!< TRUE if this write would be ended with an I2C Stop Condition.
					   u8* data_ptr,                    //!< Pointer to the data to write to the I2C Bus.
					   u16 num_bytes,                   //!< The number of bytes to write to the I2C Bus.
					   u16 baud_khz                     //!< The baud rate at which the I2C Bus should be clocked.
                       );

//! Writes one byte, and then reads a string of bytes to the LTC6813 I2C port implemented on its GPIO pins.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_I2C_Read(u8 board_num,                  //!< The logical address for the PCB containing this LTC6813-2 IC.
                      BOOLEAN start,                    //!< TRUE if this read would be started by an I2C Start Condition.
                      BOOLEAN stop,                     //!< TRUE if this read would be ended with an I2C Stop Condition.
					  u8* data_ptr,                     //!< Pointer to a byte to first write to the I2C Bus, and where to store the data read from the I2C Bus.
					  u16 num_bytes,                    //!< The number of bytes to read to the I2C Bus.
					  u16 baud_khz                      //!< The baud rate at which the I2C Bus should be clocked.
                      );

//! Writes a string of bytes to the LTC6813 SPI port implemented on its GPIO pins.
//! @return void
void LTC6813_SPI_Write(u8 board_num,                    //!< The logical address for the PCB containing this LTC6813-2 IC.
                      BOOLEAN start,                    //!< TRUE if the CS should be raised at the start of SPI communication.
                      BOOLEAN stop,                     //!< TRUE if the CS should be lowered at the end of SPI communication.
					  u8* data_ptr,                     //!< Pointer to the data to write to the SPI Bus.
					  u16 num_bytes,                    //!< The number of bytes to write to the SPI Bus.
					  u16 baud_khz                      //!< The baud rate at which the SPI Bus should be clocked.
                      );

//! Writes one byte, and then reads a string of bytes to the LTC6813 SPI port implemented on its GPIO pins.
//! @return TRUE if the LTC6813 communication was successful.
BOOLEAN LTC6813_SPI_Read(u8 board_num,                  //!< The logical address for the PCB containing this LTC6813-2 IC.
                      BOOLEAN start,                    //!< TRUE if the CS should be raised at the start of SPI communication.
                      BOOLEAN stop,                     //!< TRUE if the CS should be lowered at the end of SPI communication.
					  u8* data_ptr,                     //!< Pointer to a byte to first write to the SPI Bus, and where to store the data read from the SPI Bus.
					  u16 num_bytes,                    //!< The number of bytes to write to the SPI Bus.
					  u16 baud_khz                      //!< The baud rate at which the SPI Bus should be clocked.
                      );

//! Calculates the LTC6813 CRC over a string of bytes as per datasheet figure 22.
//! @return the calculated CRC
u16 LTC6813_PEC_Calc(u8 *data,           //!< Pointer to the data to over which to calculate the CRC.
                     int length          //!< The number of bytes over which to calculate the CRC.
                     );
								
//! Calculates the LTC6813 CRC over a string of bytes as per datasheet figure 22.
u16 LTC6813_DaisyPEC_Calc(u8 Ic_num, u8 data[][6], int length);
								
void ltc6813_register_group_write(u8* register_group);

// Writes a register group to the LTC6813
void ltc6813_registerB_group_write(u8* register_group);
															
//=============================================================================================
//函数名称	: void delay(u32 times)
//输入参数	: times:延时 
//输出参数	: void
//静态变量	: void
//功		能	: 延时函数
//注		意	:
//=============================================================================================
void delay(u32 times);

//=============================================================================================
//函数名称	: void LTC6813_DaisyWriteCfgA(u8 total_ic, u8 config[][6])
//输入参数	: total_ic:菊花链上从件个数   config[][6]:需发送的数据
//输出参数	: 应温度下标值
//函数功能	: LTC6813写寄存器A函数
//注意事项	:
//=============================================================================================
void LTC6813_DaisyWriteCfgA(u8 total_ic, u8 config[][6]);

//=============================================================================================
//函数名称	: void LTC6813_DaisyWriteCfgB(u8 total_ic, u8 config[][6])
//输入参数	: total_ic:菊花链上从件个数   config[][6]:需发送的数据
//输出参数	: 应温度下标值
//函数功能	: LTC6813写寄存器B函数
//注意事项	:
//=============================================================================================
void LTC6813_DaisyWriteCfgB(u8 total_ic, u8 config[][6]);

void ltcSpi_port_Select(u8 port);

#endif
