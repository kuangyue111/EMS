/**
  ******************************************************************************
  * @file    BF8915A.c
  * @version V1.1.4
  * @date    2021-12-20
  * @brief   This file provides the BF8915A functions(该文件提供BF8915A功能).
 */
#include "BSPTypeDef.h"
#include "BSPSPI.h"
#include "BF8915A.h"

//=============================================================================================
//函数名称	: static void BF8915ACmdWrite(void)
//输入参数	: void
//输出参数	: void
//静态变量	: void
//功		能	: SPI写入6813控制命令或者PEC
//注		意	:
//=============================================================================================
static void BF8915ACmdWrite(u8* commanddata);

/*!
    \brief      SPI1_ReadWriteByte(SPI1读写一个字节)
    \param[in]  SPI_TxData     :SPI需要发送的数据
    \retval     SPI_ReceiveData:SPI接收到的数据
*/
u8 SPI1_ReadWriteByte(u8 TxData) //TxData 可以是8位或16位的，在启用SPI之前就确定好数据帧格式
{
	/*
    u8 retry = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)//检查是否设置了指定的SPI标志。传输缓冲区空标志 //0:发送缓冲非空  等待发送缓冲器变空
    {
			retry++;
			if(retry>200)return 0;
    }
    SPI_I2S_SendData(SPI1, TxData);//通过SPI1发送数据。
    retry=0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//等待接收数据完成,检查指定的SPI标志位设置与否:接收缓存非空标志位
    {
			retry++;
			if(retry>200)return 0;
    }
    return SPI_I2S_ReceiveData(SPI1); //返回最近接收的数据，SPI_DR寄存器里面的
 */
	static u8 errNum = 0;
	u8 rcvdata = 0;

	 //SPI发送信息
   (void)BSPSPISendData(eSPI2, TxData);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return 0xFF;
		}
    }
    rcvdata = BSPSPIGetRcvData(eSPI2);
    errNum = 0;

    return(rcvdata);
}

/*!
    \brief      //SPI2读写字节，此为STM32的SPI代码作为参考，客户需根据自己的主控MCU自行修改
    \param[in]  SPI_TxData     :SPI2需要发送的数据
    \retval     SPI_ReceiveData:SPI2接收到的数据
*/
u8 SPI2_ReadWriteByte(u8 TxData)
{
	/*
	u8 retry=0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)//检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}
	SPI_I2S_SendData(SPI2, TxData);//通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}
	return SPI_I2S_ReceiveData(SPI2);//返回通过SPIx最近接收的数据
	 */
	static u8 errNum = 0;
	u8 rcvdata = 0;

	 //SPI发送信息
   (void)BSPSPISendData(eSPI2, TxData);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return 0xFF;
		}
    }
    rcvdata = BSPSPIGetRcvData(eSPI2);
    errNum = 0;

    return(rcvdata);
}

BOOLEAN	isospi_reverse = FALSE;//改变该变量可以实现菊花链SPI正反向通信。
                                //FALSE:相对的正向通信，本SDK中的SPI1为相对正向通信，SPI1菊花链连接的第一颗IC看作是IC0;
                                //TURE: 相对的反向通信，本SDK中的SPI2为相对反向通信，但也是SPI1菊花链连接的第一颗IC看作是IC0。
const u16 Bf8915crc15Table[256] =
{
	0x0000, 0xc599, 0xceab, 0x0b32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,	0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa,
	0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,	0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d,
	0x5b2e,	0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,	0x77e6, 0xb27f, 0xb94d, 0x7cd4,
	0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,	0x2544, 0x02be, 0xc727, 0xcc15, 0x098c, 0xda71, 0x1fe8, 0x14da, 0xd143,
	0xf3c5, 0x365c,	0x3d6e, 0xf8f7, 0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x07c2, 0xc25b, 0xc969, 0x0cf0, 0xdf0d,	0x1a94, 0x11a6, 0xd43f,
	0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,	0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8,
	0xa8eb, 0x6d72, 0x6640,	0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba, 0x4a88, 0x8f11,
	0x057c, 0xc0e5, 0xcbd7, 0x0e4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,	0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286,
	0xa213, 0x678a, 0x6cb8, 0xa921,	0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070, 0x85e9,
	0x0f84, 0xca1d, 0xc12f, 0x04b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528, 0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e,
	0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,	0x2ac0, 0x0d3a, 0xc8a3, 0xc391, 0x0608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7,
	0x54aa, 0x9133, 0x9a01,	0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9, 0x7350,
	0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,	0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c,
	0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,	0x2fbc, 0x0846, 0xcddf, 0xc6ed, 0x0374, 0xd089, 0x1510, 0x1e22, 0xdbbb,
	0x0af8, 0xcf61, 0xc453,	0x01ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,	0x2d02,
	0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,	0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
};//precomputed CRC15 Table(预计算15位循环冗余校验值的表)

/*!
    \brief      Wake isoSPI up from idle state(将菊花链SPI从空闲状态唤醒)
    \param[in]  total_ic: number of ICs in the daisy chain(菊花链中IC(8915A)的数量)
    \retval     none
*/
void wakeup_idle(u8 total_ic)
{
	u8 i =0;

	for (i = 0; i < total_ic; i++)
	{
		if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			BF8915_CS_LOW();		//将相对正向通信的片选CS设置为低电平
			SPI1_ReadWriteByte(0xff);//Guarantees the isoSPI will be in ready mode(保证菊花链SPI将处于就绪模式)
			BF8915_CS_HIGH();		//将相对正向通信的片选CS设置为高电平
		}
		else
		{
			BF8915_CS_LOW_Reverse();//将相对反向通信的片选CS设置为低电平
			SPI2_ReadWriteByte(0xff);//Guarantees the isoSPI will be in ready mode(保证菊花链SPI将处于就绪模式)
			BF8915_CS_HIGH_Reverse();//将相对反向通信的片选CS设置为高电平
		}
	}
}

/*!
    \brief      Generic wakeup command to wake the BF8915A from sleep(将BF8915A从睡眠中唤醒的通用唤醒命令)
    \param[in]  total_ic: the number of ic(串接的IC(8915A)数量)
    \retval     none
*/
void wakeup_sleep(u8 total_ic)
{
	u8 i =0;
	for (i = 0; i < total_ic; i++)	//此循环作用为:让所有IC(8915A)芯片处于待机状态
	{
		if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			BF8915_CS_LOW();		//将相对正向通信的片选CS设置为低电平
			delay_us(300); 			// Guarantees the BF8915A will be in standby(保证BF8915A处于待机状态),该延迟应在200~300us之间
			BF8915_CS_HIGH();		//将相对正向通信的片选CS设置为高电平
		}
		else
		{
			BF8915_CS_LOW_Reverse();//将相对反向通信的片选CS设置为低电平
			delay_us(300); 			// Guarantees the BF8915A will be in standby(保证BF8915A处于待机状态),该延迟应在200~300us之间
			BF8915_CS_HIGH_Reverse();//将相对反向通信的片选CS设置为高电平
		}
	}
}

/*!
    \brief      Writes an array of bytes out of the SPI port(将字节数组写入SPI端口)
    \param[in]  len      :Number of bytes to be written on the SPI port(SPI端口上要写入的字节数)
		\param[in]  writeData:Array of bytes to be written on the SPI port(要写入SPI端口的字节数组)
    \retval     none
*/
void spi_write_array(u8 len, u8 writeData[])
{
	u8 i = 0;
	for (i = 0; i < len; i++)		//此循环将writeData[] 数组所存的数据一一发送出去
	{
		if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			SPI1_ReadWriteByte((u8)writeData[i]);
		}
		else
		{
			SPI2_ReadWriteByte((u8)writeData[i]);
		}
	}
}

/*!
    \brief      Writes and read a set number of bytes using the SPI port(使用SPI端口写入和读取设定数量的字节)
		\param[in]  tx_Data: array of data to be written on SPI port(要写入SPI端口的数据数组)
    \param[in]  tx_len : length of the tx_data arr(tx_data数组的长度)
    \param[in]  rx_data: Input: array that will store the data read by the SPI port(输入:存储SPI端口读取的数据的数组)
		\param[in]  rx_len : Option: number of bytes to be read from the SPI port(选项:从SPI端口要读取的字节数)
    \retval     none
*/
void spi_write_read(u8 tx_Data[], u8 tx_len, u8 *rx_data, u8 rx_len)
{
	u8 i = 0;
	for (i = 0; i < tx_len; i++)	//此循环将tx_Data[] 数组所存的数据一一发送出去
	{
		if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			SPI1_ReadWriteByte(tx_Data[i]);
		}
		else
		{
			SPI2_ReadWriteByte(tx_Data[i]);
		}
	}

	for (i = 0; i < rx_len; i++)//此循环将读到的数据一一存入rx_data[] 数组中
	{
		if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			rx_data[i] = (u8)SPI1_ReadWriteByte(0xFF);
		}
		else
		{
			rx_data[i] = (u8)SPI2_ReadWriteByte(0xFF);
		}
	}
}

//=============================================================================================
//函数名称	: static void BF8915ACmdWrite(void)
//输入参数	: void
//输出参数	: void
//静态变量	: void
//功		能	: SPI写入bf8915A控制命令或者PEC
//注		意	:
//=============================================================================================
static void BF8915ACmdWrite(u8* commanddata)
{
	static u8 errNum = 0;
	u8 rcvdata[4] = {0};

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
    rcvdata[0] = BSPSPIGetRcvData(eSPI2);
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
    rcvdata[1] = BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2, commanddata[2]);
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
    (void)BSPSPISendData(eSPI2, commanddata[3]);
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
}

/*!
    \brief      Generic function to write BF8915A commands. Function calculated PEC(The Packet Error Code) for tx_cmd data(编写BF8915A命令的通用函数�:猼x_cmd数据计算了检错码)
		\param[in]  tx_cmd: 2 Byte array containing the BMS command to be sent(包含要发送的BMS(电池管理系统)命令的2字节数组)
    \retval     none
*/
void BF8915A_cmd(u8 tx_cmd[2])
{
	u8 cmd[4];					//需要发送的4字节数组
	u16 cmd_pec;				//计算出发送指令的PEC(检错码)
	cmd[0]  = tx_cmd[0];		//发送指令的高8位
	cmd[1]  = tx_cmd[1];		//发送指令的低8位
	cmd_pec = Bf8915pec15_calc(2, cmd);//计算出发送指令的PEC(检错码)
	cmd[2]  = (u8)(cmd_pec >> 8);//发送指令的PEC(检错码)的高八位
	cmd[3]  = (u8)(cmd_pec);	//发送指令的PEC(检错码)的低八位

	if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
    {
	    BF8915_CS_LOW();		//将片选CS设置为低电平
	    //spi_write_array(4,cmd);//将4字节数组写入SPI端口
	    BF8915ACmdWrite(cmd);
	    BF8915_CS_HIGH();		//将片选CS设置为高电平
    }
    else
    {
	    BF8915_CS_LOW_Reverse();//将片选CS设置为低电平
	    //spi_write_array(4,cmd);//将4字节数组写入SPI端口
	    BF8915ACmdWrite(cmd);
	    BF8915_CS_HIGH_Reverse();//将片选CS设置为高电平
    }
}

/*!
    \brief      Generic function to write BF8915A commands and write payload data. Function calculated PEC for tx_cmd data(写BF8915A命令和写有效载荷数据的通用函数。tx_cmd数据的函数计算检错码)
    \param[in]  total_ic  : the number of ic(IC(8915A)的数量)
    \param[in]  tx_cmd    : 2 Byte array containing the BMS command to be sent(包含要发送的BMS(电池管理系统)命令的2字节数组)
		\param[in]  write_data: Array containing the data to be written to the BMS ICs(包含要写入BMS(电池管理系统)IC(8915A)的数据数组)
    \retval     none
*/
void BF8915A_write(u8 total_ic, u8 tx_cmd[2], u8 write_data[])
{
	const u8 BYTES_IN_REG = 6;//寄存器组中的6个8位寄存器
	const u8 CMD_LEN = 4+(8*total_ic);//4:cmd[0]~cmd[3];(8*total_ic):寄存器组中的6个8位寄存器 + 2个8位PEC(检错码)
	u8 *cmd;				//需要发送的cmd数组
	u16 data_pec;			//计算配置寄存器数据的PEC(检错码)
	u16 cmd_pec;			//计算出发送指令的PEC(检错码)
	u8 cmd_index;			//用于写指令中 寄存器组中的数据
	u8 current_ic = total_ic;// current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 current_byte = 0;	//当前需写入的寄存器组 的字节数
	cmd = (u8 *)malloc(CMD_LEN*sizeof(u8));//计算cmd数组发送数据所需的数组长度
	cmd[0]  = tx_cmd[0];	//发送指令的高8位
	cmd[1]  = tx_cmd[1];	//发送指令的低8位
	cmd_pec = Bf8915pec15_calc(2, cmd);//计算出发送指令的PEC(检错码)
	cmd[2]  = (u8)(cmd_pec >> 8);//发送指令的PEC(检错码)的高八位
	cmd[3]  = (u8)(cmd_pec);//发送指令的PEC(检错码)的低八位
	cmd_index = 4;			//从4开始写入寄存器组中的数据

	for (current_ic = total_ic; current_ic > 0; current_ic--)       // executes for each BF8915A, this loops starts with the last IC on the stack(对每个BF8915A执行，这个循环从堆栈上的最后一个8915A开始)
	{	                                                              //The first configuration written is received by the last IC in the daisy chain(菊花链中的最后一个IC(8915A)接收写入的第一个配置)

		for (current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)//该循环将寄存器组中的6个8位寄存器的数据一一写入数组cmd[]中
		{
			cmd[cmd_index] = write_data[((current_ic-1)*BYTES_IN_REG)+current_byte];
			cmd_index = cmd_index + 1;
		}
		data_pec = (u16)Bf8915pec15_calc(BYTES_IN_REG, &write_data[(current_ic-1)*BYTES_IN_REG]);// calculating the PEC for each configuration register data(计算每个配置寄存器数据的PEC(检错码))
		cmd[cmd_index] = (u8)(data_pec >> 8);	//配置寄存器数据的PEC(检错码)的高八位
		cmd[cmd_index + 1] = (u8)data_pec;		//配置寄存器数据的PEC(检错码)的低八位
		cmd_index = cmd_index + 2;				//由于数组每个寄存器组写入后，每个IC的尾端需加2字节的PEC(检错码)，因此cmd_index需每次都自增2
	}
	if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
	{
		BF8915_CS_LOW();		//将片选CS设置为低电平
		spi_write_array(CMD_LEN, cmd);//将字节数组写入SPI端口
		BF8915_CS_HIGH();		//将片选CS设置为高电平
	}
	else
	{
		BF8915_CS_LOW_Reverse();//将片选CS设置为低电平
		spi_write_array(CMD_LEN, cmd);//将字节数组写入SPI端口
		BF8915_CS_HIGH_Reverse();//将片选CS设置为高电平
	}
	free(cmd);//清空cmd数组
}

/*!
    \brief      Generic function to write BF8915A commands and read data(写BF8915A命令和读数据的通用函数). Function calculated PEC for tx_cmd data(发送指令数据的函数计算PEC(检错码))
                Issues a command onto the daisy chain and reads back 6*total_ic data in the rx_data array(向菊花链发出命令，回读rx_data数组中的6倍IC(8915A)数据)
    \param[in]  total_ic : the number of ic(IC(8915A)的数量)
    \param[in]  tx_cmd   : 2 Byte array containing the BMS command to be sent(包含要发送的BMS(电池管理系统)命令的2字节数组)
		\param[in]  rx_data  : Array that the read back data will be stored(存储回读数据的数组)
    \retval     pec_error: 0: Data read back has matching PEC;1: Data read back has incorrect PEC(0:数据读回具有匹配的检错码 1:数据读回有不正确的检错码)
*/
u8 BF8915A_read(u8 total_ic, u8 tx_cmd[2], u8 *rx_data)
{
	const u8 BYTES_IN_REG = 8;	//寄存器组中的6个8位寄存器 + 2个8位PEC(检错码)
	u8 cmd[4];					//需要发送的cmd数组
	u8 rxDataTemp[256];			//存储SPI端口读取的数据 的数组
	u8 pec_error = 0;			//检查检错码
	u16 cmd_pec;				//计算出发送指令的PEC(检错码)
	u16 data_pec;				//计算配置寄存器数据的PEC(检错码)
	u16 received_pec;			//存入接收到的PEC(检错码)
	u8 current_ic = 0;			//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 current_byte = 0;		//当前需读入的寄存器组
	cmd[0]  = tx_cmd[0];		//发送指令的高8位
	cmd[1]  = tx_cmd[1];		//发送指令的低8位
	cmd_pec = Bf8915pec15_calc(2, cmd);//计算出发送指令的PEC(检错码)
	cmd[2]  = (u8)(cmd_pec >> 8);//发送指令的PEC(检错码)的高八位
	cmd[3]  = (u8)(cmd_pec);	//发送指令的PEC(检错码)的低八位
	
	if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
	{
		BF8915_CS_LOW();		//将片选CS设置为低电平
		spi_write_read(cmd, 4, rxDataTemp, (total_ic*BYTES_IN_REG));//Read the configuration data of all ICs on the daisy chain into rx_data[] array
		BF8915_CS_HIGH();		//将片选CS设置为高电平                           //(将菊花链上所有IC(8915A)的配置数据读入rx_data[]数组)
	}
	else
	{
		BF8915_CS_LOW_Reverse();//将片选CS设置为低电平
		spi_write_read(cmd, 4, rxDataTemp, (total_ic*BYTES_IN_REG));//Read the configuration data of all ICs on the daisy chain into rx_data[] array
		BF8915_CS_HIGH_Reverse();//将片选CS设置为高电平                           //(将菊花链上所有IC(8915A)的配置数据读入rx_data[]数组)
	}

	for (current_ic = 0; current_ic < total_ic; current_ic++) //executes for each BF8915A in the daisy chain and packs the data(对菊花链中的每个BF8915A执行并打包数据)
																	                          //into the r_comm array as well as check the received data for any bit errors(并检查接收到的数据是否有任何位错误)
	{
		for (current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)//该循环将读回的寄存器组(6个8位寄存器)数据一一写入数组rx_data[]中
		{
			rx_data[(current_ic*BYTES_IN_REG)+current_byte] = rxDataTemp[(current_ic*BYTES_IN_REG)+current_byte];
		}
		received_pec = (rx_data[(current_ic*BYTES_IN_REG)+6]<<8) + rx_data[(current_ic*BYTES_IN_REG)+7];//将rx_data[]中接收到的PEC(检错码)按高低共16位存入received_pec中
		data_pec = Bf8915pec15_calc(6, &rx_data[current_ic*BYTES_IN_REG]);//计算接收回来的寄存器组(6个8位寄存器)中的PEC(检错码)
		if (received_pec != data_pec)//检查检错码是否匹配
		{
			pec_error = 1;//检错码不正确
		}
	}
	return(pec_error);
}

/*!
    \brief      Calculates and returns the CRC15(计算并返回15位循环冗余校验值)
		\param[in]  len    : Number of bytes that will be used to calculate a PEC(将用于计算PEC(检错码)的字节数)
		\param[in]  pecData: Array of data that will be used to calculate a PEC(将用于计算PEC(检错码)的数据数组)
    \retval     The calculated pec15 as an unsigned int(计算出无符号整数的15位检错码)
*/
u16 Bf8915pec15_calc(u8 len, u8 *pecData)
{
	u16 remainder,addr;
	u8 i = 0;
	remainder = 16;		//initialize the PEC(初始化检错码)
	for (i = 0; i<len; i++) // loops for each byte in data array(数据数组中每个字节的循环)
	{
		addr = ((remainder>>7)^pecData[i])&0xff;//calculate PEC table address(计算检错码的表地址)
		remainder = (remainder<<8)^Bf8915crc15Table[addr];
	}
	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2(15位循环冗余校验值的最低有效位为0，因此余数必须乘以2)
}

/*!
    \brief      Helper function to set GPIO bits(设置GPIO位的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAReg: Register group A(寄存器组A)
    \param[in]  gpio_pd_en: 0: GPIO pin pull-down enable; 1: GPIO pin pull-down disable (default)(0:GPIO引脚下拉使能；1:GPIO引脚下拉禁用(默认))
    \retval     none
*/
void BF8915A_set_cfga_gpio(u8 nIC, Reg_TypeDef *configAReg, u8 gpio_pd_en)
{
	configAReg[nIC].tx_data[0] = gpio_pd_en;//设置CFGAR0,0:GPIO引脚下拉使能；1:GPIO引脚下拉禁用
}

/*!
    \brief      Helper function to set GPIO IO_MODE(设置GPIO的IO口模式的辅助函数)
		\param[in]  nIC           : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAReg    : Register group A(寄存器组A)
		\param[in]  gpio_analog_en: 0: analog input;1: Ordinary GPIO(0:模拟输入；1:普通GPIO)
    \retval     none
*/
void BF8915A_set_cfga_gpio_mode(u8 nIC, Reg_TypeDef *configAReg, u8 gpio_analog_en)
{
	configAReg[nIC].tx_data[1] = gpio_analog_en;//设置CFGAR1,0:模拟输入；1:普通GPIO
}

/*!
    \brief      保留位，永远置0
    \retval     none
*/
void BF8915A_set_cfga_rsvd(u8 nIC, Reg_TypeDef *configAReg, BOOLEAN rsvd2, BOOLEAN rsvd1)
{
	configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]&(~(0x01<<3));//保留位，永远置0
	configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]&(~(0x01<<2));//保留位，永远置0
} 

/*!
    \brief      Helper function to set osr_sel(设置ADC过采样率选择的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAReg: Register group A(寄存器组A)
		\param[in]  osr_sel   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择)
    \retval     none
*/ 
void BF8915A_set_cfga_osr_sel(u8 nIC, Reg_TypeDef *configAReg, BOOLEAN osr_sel)
{
	configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]&(~(0x01<<1));//写0,过采样率选择64，128，256，512(四种过采样率通过指令的OSR选择)
}

/*!
    \brief      Helper function to set refon(设置基准电压配置的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAReg: Register group A(寄存器组A)
		\param[in]  refon     : 1: VREF1 remains powered on until WDT overflowsVREF1
						                0: VREF1 is turned off after ADC conversion
                          (1:VREF1(基准电压)保持上电直到WDT溢出
                            0:ADC转换完之后VREF1关闭 (默认))
    \retval     none
*/
void BF8915A_set_cfga_refon(u8 nIC, Reg_TypeDef *configAReg, BOOLEAN refon)
{
    if (refon)//设置CFGAR2的第1位
		configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]|(0x01);//写1,VREF1(基准电压)保持上电直到WDT溢出
    else 
		configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]&(~(0x01));//写0,ADC转换完之后VREF1关闭
}

/*!
    \brief      Helper Function to set uv value in CFG register(在配置寄存器组的寄存器中设置欠压阈值的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAReg: Register group A(寄存器组A)
		\param[in]  uv        : Low 8 bits of undervoltage threshold(低8位欠压阈值)
    \retval     none
*/
void BF8915A_set_cfga_uv(u8 nIC, Reg_TypeDef *configAReg, u16 uv)
{
	configAReg[nIC].tx_data[3] = 0x00FF & uv;						//设置CFGAR3,欠压阈值低8位放入
	configAReg[nIC].tx_data[4] = configAReg[nIC].tx_data[4]&0xF0;	//将CFGAR4低4位清空
	configAReg[nIC].tx_data[4] = configAReg[nIC].tx_data[4]|((0x0F00 & uv)>>8);//设置CFGAR4低4位，欠压阈值高4位放入
}

/*!
    \brief      helper function to set OV value in CFG register(在配置寄存器组的寄存器中设置过压阈值的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAReg: Register group A(寄存器组A)
		\param[in]  ov        : Overvoltage threshold lower 4 bits(过压阈值低4位)
    \retval     none
*/
void BF8915A_set_cfga_ov(u8 nIC, Reg_TypeDef *configAReg, u16 ov)
{
	configAReg[nIC].tx_data[5] = 0x00FF & (ov>>4);					//设置CFGAR5,过压阈值高8位放入
	configAReg[nIC].tx_data[4] = configAReg[nIC].tx_data[4]&0x0F;	//将CFGAR4高4位清空
	configAReg[nIC].tx_data[4] = configAReg[nIC].tx_data[4]|((0x000F & ov)<<4);//设置CFGAR4高4位，过压阈值低4位放入
																			 //保存CFGAR4低4位，即欠压阈值高4位
}
/*!
    \brief      Helper function to set CFGRA variable(设置配置寄存器组A变量的辅助函数)
		\param[in]  nIC           : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAReg    : Register group A(寄存器组A)
		\param[in]  gpio_pd_en    : GPIO pin pull-down enable; 1: GPIO pin pull-down disable (default)(0:GPIO引脚下拉使能；1:GPIO引脚下拉禁用(默认))
		\param[in]  gpio_analog_en: 0: analog input 1: Ordinary GPIO(0:模拟输入；1:普通GPIO)
		\param[in]  rsvd2         : 保留位，永远置0
		\param[in]  rsvd1         : 保留位，永远置0
		\param[in]  osr_sel       : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择)
		\param[in]  refon         : 1: VREF1 remains powered on until WDT overflowsVREF1
						                    0: VREF1 is turned off after ADC conversion
                              (1:VREF1(基准电压)保持上电直到WDT溢出
                                0:ADC转换完之后VREF1关闭 (默认))
		\param[in]  uv_th         : Low 8 bits of undervoltage threshold(低8位欠压阈值)
		\param[in]  ov_th         : Overvoltage threshold lower 4 bits(过压阈值低4位)
    \retval     none
*/
void BF8915A_set_cfgra(u8 nIC,
						 Reg_TypeDef *configAReg,
						 u8 gpio_pd_en,			//写0:GPIO脚下拉使能；1:GPIO脚下拉关闭(默认)读0 :GPIO脚的值是0 ；1:GPIO脚的值是1
						 u8 gpio_analog_en,		//GPIO0~7模式选择 0:模拟输入 1:普通GPIO
						 BOOLEAN rsvd2,			//保留位，永远置0
						 BOOLEAN rsvd1,			//保留位，永远置0
						 BOOLEAN osr_sel,		//0:过采样率选择64，128，256，512(四种过采样率通过指令的OSR选择)
											    //这个寄存器位要和指令的OSR一起配置选择ADC过采样率
						 BOOLEAN refon,  		//VREF1(基准电压)配置  1 :VREF1保持上电直到WDT溢出  0 :ADC转换完之后VREF1关闭 (默认)
						 u16 uv_th,				//Under voltage Comparison Voltage(欠压阈值)，根据实际情况自行设定
										 	 	 //Under voltage threshold ADC Code(欠电压阈值模数转换器代码): uv_th = 3000 * 65535U / 5000U. LSB = 0.0001 ---(3V)
						 u16 ov_th 				//Over  voltage Comparison Voltage(过压阈值)，根据实际情况自行设定
										 	 	 //Over  voltage threshold ADC Code(过电压阈值模数转换器代码): ov_th = 4100 * 65535U / 5000U. LSB = 0.0001 ---(4.1V)
									 )
{
	BF8915A_set_cfga_gpio(nIC, configAReg, gpio_pd_en);			//设置CFGAR0
	BF8915A_set_cfga_gpio_mode(nIC, configAReg,gpio_analog_en);	//设置CFGAR1
	BF8915A_set_cfga_rsvd(nIC, configAReg,rsvd2,rsvd1);			//保留位，永远置0
	BF8915A_set_cfga_osr_sel(nIC, configAReg,osr_sel);			//设置CFGAR2的第2位
	BF8915A_set_cfga_refon(nIC, configAReg,refon);				//设置CFGAR2的第1位
	BF8915A_set_cfga_uv(nIC, configAReg,uv_th);					//设置CFGAR3和CFGAR4低4位
	BF8915A_set_cfga_ov(nIC, configAReg,ov_th);					//设置CFGAR5和CFGAR4高4位
}

/*!
    \brief      Helper function to control discharge(控制放电的辅助功能)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configBReg: Register group B(寄存器组B)
		\param[in]  dcc       :	1: Turn on  the switch of Cell x(打开x号电池的开关)
							              0: Turn off the switch of Cell x(关闭x号电池的开关)
    \retval     none
*/
void BF8915A_set_cfgb_dcc(u8 nIC, Reg_TypeDef *configBReg, u16 dcc)
{
	configBReg[nIC].tx_data[0] = (u8)dcc;			//设置CFGBR0,放电电池开关x=1~8
	configBReg[nIC].tx_data[1] = (u8)(dcc >> 0x8U);	//设置CFGBR1,放电电池开关x=9~16
}

/*!
    \brief      Balanced duty cycle selection(均衡占空比选择)
		\param[in]  nIC        : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configBReg : Register group B(寄存器组B)
		\param[in]  bl_duty_sel: Active balancing duty cycle selection(均衡占空比选择)
    \retval     none
*/
//bl_duty_sel(4位): 均衡占空比选择,占空比=(BL_DUTY_SEL+1)*10%，配置范围10%~100%，当大于9时是100%。
void BF8915A_set_cfgb_bl_duty_sel(u8 nIC, Reg_TypeDef *configBReg, u8 bl_duty_sel)
{
	configBReg[nIC].tx_data[2] = configBReg[nIC].tx_data[2] & 0x0F;//将CFGBR2的高4位清空
	configBReg[nIC].tx_data[2] = configBReg[nIC].tx_data[2] | ((0x0F & bl_duty_sel)<<4);//设置CFGBR2高4位,均衡占空比选择
                                                                                      //保存CFGBR2低4位
}

/*!
    \brief      Helper function to set dcto(设置放电定时溢出值配置的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configBReg: Register group B(寄存器组B)
		\param[in]  dcto      : Discharge timing overflow value configuration(放电定时溢出值配置，详细说明如DCTO表所示)
    \retval     none
*/
void BF8915A_set_cfgb_dcto(u8 nIC, Reg_TypeDef *configBReg, u8 dcto)
{
	configBReg[nIC].tx_data[2] = configBReg[nIC].tx_data[2] & 0xF0;//将CFGBR2的低4位清空
	configBReg[nIC].tx_data[2] = configBReg[nIC].tx_data[2] | (0x0F & dcto);//设置CFGBR2低4位,放电定时溢出值配置
                                                                          //保存CFGBR2高4位
}

/*!
    \brief      Helper function to set vuv_bl(设置均衡欠压阈值的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configBReg: Register group B(寄存器组B)
		\param[in]  vuv_bl    : Lower 8 bits of equalization undervoltage threshold(均衡欠压阈值的低8位)
    \retval     none
*/
void BF8915A_set_cfgb_vuv_bl(u8 nIC, Reg_TypeDef *configBReg, u16 vuv_bl)
{
	configBReg[nIC].tx_data[3] = vuv_bl & 0xFF;						//设置CFGBR3,均衡欠压阈值低8位放入
	configBReg[nIC].tx_data[4] = configBReg[nIC].tx_data[4] & 0xF0;	//将CFGBR4的低4位清空
	configBReg[nIC].tx_data[4] = configBReg[nIC].tx_data[4] | ((vuv_bl & 0x0F00U) >> 0x8U);//设置CFGBR4低4位,均衡欠压阈值高4位放入
                                                                                         //保存CFGBR4高4位
}

/*!
    \brief      Helper function to set bl_dtmen(设置启用放电监测的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configBReg: Register group B(寄存器组B)
		\param[in]  dtmen     : Enable discharge monitor(启用放电监测) 1: Enable  discharge monitor(启用放电监测)
														                                         0: Disable discharge monitor(禁用放电监测)
    \retval     none
*/
void BF8915A_set_cfgb_bl_dtmen(u8 nIC, Reg_TypeDef *configBReg, BOOLEAN dtmen)
{
	if (dtmen)//设置CFGAR4的第5位
		configBReg[nIC].tx_data[4] = configBReg[nIC].tx_data[4] | (0x01<<4);//写1,使能放电监视器
	else
		configBReg[nIC].tx_data[4] = configBReg[nIC].tx_data[4] & (~(0x01<<4));//写0,不使能放电监视器
}

/*!
    \brief      Helper function to set CFGRB variable(设置配置寄存器组B变量的辅助函数)
		\param[in]  nIC        : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configBReg : Register group B(寄存器组B)
		\param[in]  dcc        :	1: Turn on  the switch of Cell x(打开x电池的开关)
							                0: Turn off the switch of Cell x(关闭x电池的开关)
		\param[in]  bl_duty_sel: Active balancing duty cycle selection(均衡占空比选择)
		\param[in]  dcto       : Discharge timing overflow value configuration(放电定时溢出值配置)
		\param[in]  vuv_bl     : Lower 8 bits of equalization undervoltage threshold(均衡欠压阈值的低8位)
		\param[in]  dtmen      : Enable discharge monitor(使能放电监测器)	1: Enable  discharge monitor(启用放电监测)
														                                            0: Disable discharge monitor(禁用放电监测)
    \retval     none
*/
void BF8915A_set_cfgrb(u8 nIC,
						 Reg_TypeDef *configBReg,
						 u16 dcc,			//放电电池开关1:打开Cell x 的开关  0:关闭Cell x 的开关 (默认)
						 u8 bl_duty_sel,	//均衡占空比选择 占空比=(BL_DUTY_SEL+1)*10%，配置范围10%~100%，当大于9时是100%
						 u8 dcto,			//放电定时溢出值配置
						 u16 vuv_bl,		//均衡欠压阈值
						 BOOLEAN dtmen		//在自动均衡下，如果DTMEN=1，当欠压(小于VUV_BL)关闭放电开关，如果DTMEN=0，不进行欠压比较
						 )
{
	BF8915A_set_cfgb_dcc(nIC, configBReg,dcc);					//控制放电的辅助功能
	BF8915A_set_cfgb_bl_duty_sel(nIC, configBReg,bl_duty_sel);	//均衡占空比选择
	BF8915A_set_cfgb_dcto(nIC, configBReg,dcto);				//设置放电定时溢出值配置的辅助函数
	BF8915A_set_cfgb_vuv_bl(nIC, configBReg,vuv_bl);			//设置均衡欠压阈值的辅助函数
	BF8915A_set_cfgb_bl_dtmen(nIC, configBReg,dtmen);			//设置启用放电监测的辅助函数
}

/*!
    \brief      Helper function to set Undervoltage threshold in monitoring mode(监测模式下，设置欠压阈值的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configCReg: Register group C(寄存器组C)
		\param[in]  vuv_m     : Undervoltage threshold in monitoring mode(监测模式下的欠压阈值)
    \retval     none
*/
void BF8915A_set_cfgc_vuv_m(u8 nIC, Reg_TypeDef *configCReg, u16 vuv_m)
{
	configCReg[nIC].tx_data[0] = 0x00FF & vuv_m;					//设置CFGCR0,监测模式下，欠压阈值低8位放入
	configCReg[nIC].tx_data[1] = configCReg[nIC].tx_data[1]&0xF0;	//将CFGCR1的低4位清空
	configCReg[nIC].tx_data[1] = configCReg[nIC].tx_data[1]|((0x0F00 & vuv_m)>>8);//设置CFGCR1低4位,监测模式下，欠压阈值高4位放入
																			//保存CFGCR1高4位
}

/*!
    \brief      Helper function to set Overvoltage  threshold in monitoring mode(监测模式下，设置过压阈值的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configCReg: Register group C(寄存器组C)
		\param[in]  vov_m     : Overvoltage threshold in monitoring mode(监测模式下的过压阈值)
    \retval     none
*/
void BF8915A_set_cfgc_vov_m(u8 nIC, Reg_TypeDef *configCReg, u16 vov_m)
{
	configCReg[nIC].tx_data[2] = 0x00FF & (vov_m>>4);				//设置CFGCR2,监测模式下，过压阈值高8位放入
	configCReg[nIC].tx_data[1] = configCReg[nIC].tx_data[1]&0x0F;	//将CFGCR1的高4位清空
	configCReg[nIC].tx_data[1] = configCReg[nIC].tx_data[1]|((0x000F & vov_m)<<4);//设置CFGCR1高4位,监测模式下，过压阈值低4位放入
																			//保存CFGCR1低4位
}

/*!
    \brief      Helper function to set Internal over-temperature threshold in monitoring mode(监测模式下，设置内部过温阈值的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configCReg: Register group C(寄存器组C)
		\param[in]  itov_m    : In monitoring mode, internal over-temperature threshold(监测模式下，内部过温阈值)
    \retval     none
*/
void BF8915A_set_cfgc_itov_m(u8 nIC, Reg_TypeDef *configCReg, u16 itov_m)
{
	configCReg[nIC].tx_data[3] = 0x00FF & itov_m;					//设置CFGCR3,监测模式下，内部过温阈值低8位放入
	configCReg[nIC].tx_data[4] = configCReg[nIC].tx_data[4]&0xF0;	//将CFGCR4的低4位清空
	configCReg[nIC].tx_data[4] = configCReg[nIC].tx_data[4]|((0x0F00 & itov_m)>>8);//设置CFGCR4低4位,监测模式下，内部过温阈值高4位放入
																				 //保存CFGCR4高4位
}

/*!
    \brief      Helper function to set external over-temperature threshold in monitoring mode(监测模式下，设置外部过温阈值的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configCReg: Register group C(寄存器组C)
		\param[in]  extov_m   : In monitoring mode, external over-temperature threshold(监测模式下，外部过温阈值)
    \retval     none
*/
void BF8915A_set_cfgc_extov_m(u8 nIC, Reg_TypeDef *configCReg, u16 extov_m)
{
	configCReg[nIC].tx_data[5] = 0x00FF & (extov_m>>4);					//设置CFGCR5,监测模式下，外部过温阈值高8位放入
	configCReg[nIC].tx_data[4] = configCReg[nIC].tx_data[4]&0x0F;		//将CFGCR4的高4位清空
	configCReg[nIC].tx_data[4] = configCReg[nIC].tx_data[4]|((0x000F & extov_m)<<4);//设置CFGCR4高4位,监测模式下，外部过温阈值低4位放入
																				  //保存CFGCR4低4位
}

/*!
    \brief      Helper function to set CFGRC variable(设置配置寄存器组C变量的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configCReg: Register group C(寄存器组C)
		\param[in]  vuv_m     : Undervoltage threshold in monitoring mode(监测模式下的欠压阈值)
		\param[in]  vov_m     : Overvoltage  threshold in monitoring mode(监测模式下的过压阈值)
		\param[in]  itov_m    : Internal over-temperature threshold in monitoring mode(监测模式下，内部过温阈值)
		\param[in]  extov_m   : external over-temperature threshold in monitoring mode(监测模式下，外部过温阈值)
    \retval     none
*/
void BF8915A_set_cfgrc(u8 nIC, Reg_TypeDef *configCReg, u16 vuv_m, u16 vov_m, u16 itov_m, u16 extov_m)
{
	BF8915A_set_cfgc_vuv_m(nIC, configCReg,vuv_m);		//监测模式下，设置欠压阈值的辅助函数
	BF8915A_set_cfgc_vov_m(nIC, configCReg,vov_m);		//监测模式下，设置过压阈值的辅助函数
	BF8915A_set_cfgc_itov_m(nIC, configCReg,itov_m);	//监测模式下，设置内部过温阈值的辅助函数
	BF8915A_set_cfgc_extov_m(nIC, configCReg,extov_m);	//监测模式下，设置外部过温阈值的辅助函数
}

/*!
    \brief      Helper function to set csel_m(设置监测模式下，电池电压监测比较选择的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg: Register group D(寄存器组D)
		\param[in]  csel_m    : In monitoring mode, battery voltage monitoring comparison selection cell1~cell16(在监测模式下，电池电压监测比较选择电池1 ~电池16)
    \retval     none
*/
void BF8915A_set_cfgd_csel_m(u8 nIC, Reg_TypeDef *configDReg, u16 csel_m)
{
	configDReg[nIC].tx_data[0] = 0x00FF & csel_m;			//设置CFGDR0，即电池电压监测比较选择cell1~cell8
	configDReg[nIC].tx_data[1] = (csel_m & 0xFF00) >> 0x8U;	//设置CFGDR1，即电池电压监测比较选择cell9~cell16
}

/*!
    \brief      Helper function to set gsel_m(设置监测模式下，GPIO0~7比较选择的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg: Register group D(寄存器组D)
		\param[in]  gsel_m    : In monitoring mode, GPIO0~7 is compared and selected(监测模式下，GPIO0~7比较选择)
    \retval     none
*/
void BF8915A_set_cfgd_gsel_m(u8 nIC, Reg_TypeDef *configDReg, u8 gsel_m)
{
	configDReg[nIC].tx_data[2] = gsel_m;//设置CFGDR2
}

/*!
    \brief      Helper function to set isospi_wake_t(设置菊花链唤醒时间的辅助函数)
		\param[in]  nIC          : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg   : Register group D(寄存器组D)
		\param[in]  isospi_wake_t: isospi wake-up time, 0: 10us; 1:20us(isospi(菊花链SPI)唤醒时间，0:10us；1:20us)
    \retval     none
*/
void BF8915A_set_cfgd_isospi_wake_t(u8 nIC, Reg_TypeDef *configDReg, BOOLEAN isospi_wake_t)
{
  if (isospi_wake_t)//设置CFGDR3的第8位
		configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] | (0x01<<7);//写1
   else 
		configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] & (~(0x01<<7));//写0
}

/*!
    \brief      Helper function to set adc_rst_sel(设置测量第1个通道ADC复位选择的辅助函数)
		\param[in]  nIC        : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg : Register group D(寄存器组D)
		\param[in]  adc_rst_sel: 0: All measurement channels reset ADC(所有测量通道都复位ADC)
								             1: Only the first channel resets ADC, then 2~n channels do not reset ADC(只有第1个通道复位ADC，之后2~n通道不复位ADC)
    \retval     none
*/
void BF8915A_set_cfgd_adc_rst_sel(u8 nIC, Reg_TypeDef *configDReg, BOOLEAN adc_rst_sel)
{
  if (adc_rst_sel)//设置CFGDR3的第7位
		configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] | (0x01<<6);//写1
  else 
		configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] & (~(0x01<<6));//写0
}

/*!
    \brief      Helper function to set time_sel_m(设置监测模式下，定时监测时间配置的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg: Register group D(寄存器组D)
		\param[in]  time_sel_m: In monitoring mode, timing monitoring time configuration(监测模式下，定时监测时间配置)
    \retval     none
*/
//time_sel_m(5位):监测模式下，定时监测时间配置，定时时间=(TIME_SEL_M+1)*1s(2~64s)，最小配置值是1(2s)
//这个时间配置和均衡周期配置共用
void BF8915A_set_cfgd_time_sel_m(u8 nIC, Reg_TypeDef *configDReg, u8 time_sel_m)
{
  
	configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] & 0xC0U;//将CFGDR3低6位清空
	configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] | (time_sel_m & 0x3F);//设置CFGDR3低6位
                                                                                //保存CFGDR3高2位
}

/*!
    \brief      Helper function to set adc_init_t2(设置ADC测量第2~n个通道ADC初始化时间的辅助函数)
		\param[in]  nIC        : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg : Register group D(寄存器组D)
		\param[in]  adc_init_t2: ADC measurement 2~n channel ADC initialization time(ADC测量第2~n个通道ADC初始化时间)
    \retval     none
*/
//ADC测量第2~n个通道ADC初始化时间，(ADC_INIT_T2+1)*20us(20~320us)
void BF8915A_set_cfgd_adc_init_t2(u8 nIC, Reg_TypeDef *configDReg, u8 adc_init_t2)
{
  
	configDReg[nIC].tx_data[4] = configDReg[nIC].tx_data[4] & 0x0FU;//将CFGDR4高4位清空
	configDReg[nIC].tx_data[4] = configDReg[nIC].tx_data[4] | ((adc_init_t2 & 0x0F) << 0x4U);//设置CFGDR4高4位
                                                                                           //保存CFGDR4低4位
}

/*!
    \brief      Helper function to set adc_init_t1(设置ADC测量第1个通道ADC初始化时间的辅助函数)
		\param[in]  nIC        : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg : Register group D(寄存器组D)
		\param[in]  adc_init_t1: ADC measurement 1 channel ADC initialization time(ADC测量第1个通道ADC初始化时间)
    \retval     none
*/
//ADC测量第1个通道ADC初始化时间(ADC_INIT_T1+1)*100us(100~1600us)
void BF8915A_set_cfgd_adc_init_t1(u8 nIC, Reg_TypeDef *configDReg, u8 adc_init_t1)
{
	configDReg[nIC].tx_data[4] = configDReg[nIC].tx_data[4] & 0xF0U;//将CFGDR4低4位清空
	configDReg[nIC].tx_data[4] = configDReg[nIC].tx_data[4] | (adc_init_t1 & 0x0F);//设置CFGDR4低4位
                                                                                 //保存CFGDR4高4位
}

/*!
    \brief      Helper function to set trim2_en(设置修调2使能的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg: Register group D(寄存器组D)
		\param[in]  trim2_en  : Modification 2 enable, 1: enable, 0: disable(修调2使能，1:使能，0:不使能)
    \retval     none
*/
void BF8915A_set_cfgd_trim2_en(u8 nIC, Reg_TypeDef *configDReg, BOOLEAN trim2_en)
{
  if (trim2_en)//设置CFGDR5的第8位
		configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] | (0x01<<7);//写1
   else 
		configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] & (~(0x01<<7));//写0
}

/*!
    \brief      Helper function to set trim1_en(设置修调1使能的辅助函数)
		\param[in]  nIC       : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg: Register group D(寄存器组D)
		\param[in]  trim1_en  : Modification 1 enable, 1: enable, 0: disable(修调1使能，1:使能，0:不使能)
    \retval     none
*/
void BF8915A_set_cfgd_trim1_en(u8 nIC, Reg_TypeDef *configDReg, BOOLEAN trim1_en)
{
  if (trim1_en)//设置CFGDR5的第7位
		configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] | (0x01<<6);//写1
   else 
		configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] & (~(0x01<<6));//写0
}

/*!
    \brief      Helper function to set trim1_en(设置监测模式下发生异常，上报的间隔时间的辅助函数)
		\param[in]  nIC        : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg : Register group D(寄存器组D)
		\param[in]  err_t_sel_m: When an exception occurs in the monitoring mode, the reporting interval(监测模式下发生异常，上报的间隔时间)
    \retval     none
*/
//err_t_sel_m(2位):监测模式下发生异常，上报的间隔时间 00:200ms，01:400ms，10:600ms，11:800ms
void BF8915A_set_cfgd_err_t_sel_m(u8 nIC, Reg_TypeDef *configDReg, u8 err_t_sel_m)
{
  
	configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] & 0xCFU;//将CFGDR5的第6~5位清空
	configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] | ((err_t_sel_m & 0x03) << 0x4U);//设置CFGDR5第6~5位
                                                                                           //保存CFGDR5第8~7位
}

/*!
    \brief      Helper function to set CFGRD variable(设置配置寄存器组D变量的辅助函数)
		\param[in]  nIC 	    : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg: A two dimensional array of the configuration data that will be written(将要写入配置数据的二维数组)
		\param[in]  vd33t_on_m: A two dimensional array of the configuration data that will be written(将要写入配置数据的二维数组)
    \retval     none   	                  					  
*/
//vd33t_on_m:监测模式下，VREF1(基准电压)和VD33T的启动时间配置(5+VD33T_ON_M*10)ms，范围是5~155ms
void BF8915A_set_cfgd_vd33t_on_m(u8 nIC, Reg_TypeDef *configDReg, u8 vd33t_on_m)
{
	configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] & 0xF0U;//将CFGDR5的低4位清空
	configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] | (vd33t_on_m & 0x0F);//保存CFGDR5高4位，设置CFGDR5低4位
}

/*!
    \brief      Helper function to set CFGRD variable(设置配置寄存器组D变量的辅助函数)
		\param[in]  nIC          : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDReg   : Register group D(寄存器组D)
		\param[in]  csel_m       : In monitoring mode, battery voltage monitoring comparison selection cell1~cell16(在监测模式下，电池电压监测比较选择电池1~电池16)
		\param[in]  gsel_m       : In monitoring mode, GPIO0~7 comparison selection(监测模式下，GPIO0~7比较选择)
		\param[in]  isospi_wake_t: isospi wake up time(菊花链唤醒时间)
		\param[in]  adc_rst_sel  : 0: All measurement channels reset ADC(所有测量通道都复位ADC)
								                1: Only the first channel resets ADC, then 2~n channels do not reset ADC(只有第1个通道复位ADC，之后2~n通道不复位ADC)
		\param[in]  time_sel_m   : In monitoring mode, timing monitoring time configuration, timing time=(TIME_SEL_M+1)*1s(2~64s), the minimum configuration value is 1(2s);This time configuration is shared with the balance period configuration
                              (监测模式下，定时监测时间配置，定时时间=(TIME_SEL_M+1)*1s(2~64s)，最小配置值是1(2s)；该时间配置与均衡时期的配置共享)
		\param[in]  adc_init_t2  : ADC measures the ADC initialization time of the second to n channels, (ADC_INIT_T2+1)*20us (20~320us)(ADC测量第2~n个通道ADC初始化时间，(ADC_INIT_T2+1)*20us(20~320us))
		\param[in]  adc_init_t1  : ADC measurement of the first channel ADC initialization time (ADC_INIT_T1+1)*100us (100~1600us)(ADC测量第1个通道ADC初始化时间(ADC_INIT_T1+1)*100us(100~1600us))
		\param[in]  trim2_en:1   : enable, 0: disable(修调2使能，1:使能，0:不使能)
		\param[in]  trim1_en:1   : enable, 0: disable(修调1使能，1:使能，0:不使能)
		\param[in]  err_t_sel_m  : 00: 200ms, 01: 400ms, 10: 600ms, 11: 800ms(监测模式下发生异常，上报的间隔时间  00:200ms，01:400ms，10:600ms，11:800ms)
		\param[in]  vd33t_on_m   : The startup time configuration of VREF1 and VD33T (5+VD33T_ON_M*10)ms, the range is 5~155ms(监测模式下，VREF1和VD33T的启动时间配置(5+VD33T_ON_M*10)ms，范围是5~155ms)
    \retval     none   	                  					  
*/
void BF8915A_set_cfgrd(u8 nIC,
					 Reg_TypeDef *configDReg, 
					 u16 csel_m,		//监测模式下，电池电压监测比较选择cell1~cell16
                           	   	   	    //1:相应的电池就会在监测模式下进行过欠压比较，置标志位
                           	   	   	    //0:相应的电池就会在监测模式下不进行过欠压比较，不置标志位
					 u8 gsel_m ,		//监测模式下，GPIO0~7比较选择
                           	   	   	    //1:相应的电池就会在监测模式下进行过温(小于外部温度阈值)比较，置标志位
                           	   	   	    //0:相应的电池就会在监测模式下不进行过温(小于外部温度阈值)比较，不置标志位
					 BOOLEAN isospi_wake_t,	//菊花链唤醒时间，0: 10us；1: 20us
					 BOOLEAN adc_rst_sel,	//测量第1个通道ADC复位选择，0:所有测量通道都复位ADC，1:只有第1个通道复位ADC，之后2~n通道不复位ADC
					 u8 time_sel_m,			//监测模式下，定时监测时间配置，定时时间=(TIME_SEL_M+1)*1s(2~64s)，最小配置值是1(2s)
                              	  	  	  	//这个时间配置和均衡周期配置共用
					 u8 adc_init_t2,		//ADC测量第2~n个通道ADC初始化时间，(ADC_INIT_T2+1)*20us(20~320us)
					 u8 adc_init_t1,		//ADC测量第1个通道ADC初始化时间(ADC_INIT_T1+1)*100us(100~1600us)
					 BOOLEAN trim2_en,		//修调2使能，1:使能，0:不使能
					 BOOLEAN trim1_en,		//修调1使能，1:使能，0:不使能
					 u8 err_t_sel_m,		//监测模式下发生异常，上报的间隔时间
                               	   	   	    //00:200ms，01:400ms，10:600ms，11:800ms
					 u8 vd33t_on_m			//监测模式下，VREF1和VD33T的启动时间配置(5+VD33T_ON_M*10)ms，范围是5~155ms
					 )
{
	BF8915A_set_cfgd_csel_m(nIC, configDReg,csel_m);				//设置监测模式下，电池电压监测比较选择的辅助函数
	BF8915A_set_cfgd_gsel_m(nIC, configDReg,gsel_m);				//设置监测模式下，GPIO0~7比较选择的辅助函数
	BF8915A_set_cfgd_isospi_wake_t(nIC, configDReg,isospi_wake_t);	//设置菊花链唤醒时间的辅助函数
	BF8915A_set_cfgd_adc_rst_sel(nIC, configDReg,adc_rst_sel);		//设置测量第1个通道ADC复位选择的辅助函数
	BF8915A_set_cfgd_time_sel_m(nIC, configDReg,time_sel_m);		//设置监测模式下，定时监测时间配置的辅助函数
	BF8915A_set_cfgd_adc_init_t2(nIC, configDReg,adc_init_t2);		//设置ADC测量第2~n个通道ADC初始化时间的辅助函数
	BF8915A_set_cfgd_adc_init_t1(nIC, configDReg,adc_init_t1);		//设置ADC测量第1个通道ADC初始化时间的辅助函数
	BF8915A_set_cfgd_trim2_en(nIC, configDReg,trim2_en);			//设置修调2使能的辅助函数
	BF8915A_set_cfgd_trim1_en(nIC, configDReg,trim1_en);			//设置修调1使能的辅助函数
	BF8915A_set_cfgd_err_t_sel_m(nIC, configDReg,err_t_sel_m);		//设置监测模式下发生异常，上报的间隔时间的辅助函数
	BF8915A_set_cfgd_vd33t_on_m(nIC, configDReg,vd33t_on_m);		//设置配置寄存器组D变量的辅助函数
}

/*!
    \brief      Write the BF8915A CFGRA(写BF8915A的配置寄存器组A)
		\param[in]  total_ic 	 : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAData: A two dimensional array of the configuration data that will be written(将要写入配置数据的二维数组)
    \retval     none   	                  					  
*/
void BF8915A_wrcfga(u8 total_ic, ConfigA_TypeDef configAData[])
{
	u8 cmd[2] = {0x00 , 0x01};			//发送写BF8915A的配置寄存器组A指令的高低共16位
	Reg_TypeDef configAArray[TOTAL_IC];	//定义一个结构体为Reg_TypeDef的数据结构A数组,用于存储指令数据
	u8 write_buffer[TOTAL_IC * 0x6U];	//寄存器组中有6个8位寄存器
	u8 write_count = 0;					//数组计数器，最大为TOTAL_IC * 0x6U
	u8 c_ic = 0;						//当前正在写入数组的8915A芯片序号
	u8 current_ic = 0;					//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 dataTemp = 0;					//计数器，每6次一循环，用于计数每个寄存器组当前循环到的寄存器
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//配置每个8915A的配置寄存器组A
	{
		BF8915A_set_cfgra(current_ic, 
						configAArray,
						configAData[current_ic].gpio_pd_en,		//写0:GPIO脚下拉使能；1:GPIO脚下拉关闭(默认)读0 :GPIO脚的值是0 ；1:GPIO脚的值是1
						configAData[current_ic].gpio_analog_en,	//GPIO0~7模式选择 0:模拟输入 1:普通GPIO
						configAData[current_ic].rsvd2,			//保留位，永远置0
						configAData[current_ic].rsvd1,			//保留位，永远置0
						configAData[current_ic].osr_sel,		//0:过采样率选择64，128，256，512(四种过采样率通过指令的OSR选择)
															  	//这个寄存器位要和指令的OSR一起配置选择ADC过采样率
						configAData[current_ic].refon,			//VREF1(基准电压)配置  1 :VREF1保持上电直到WDT溢出  0 :ADC转换完之后VREF1关闭 (默认)
						configAData[current_ic].uv_th,			//Under voltage Comparison Voltage(欠压阈值)，根据实际情况自行设定
																//Under voltage threshold ADC Code(欠电压阈值模数转换器代码): uv_th = 3000 * 65535U / 5000U. LSB = 0.0001 ---(3V)
						configAData[current_ic].ov_th 			//Over  voltage Comparison Voltage(过压阈值)，根据实际情况自行设定
																//Over  voltage threshold ADC Code(过电压阈值模数转换器代码): ov_th = 4100 * 65535U / 5000U. LSB = 0.0001 ---(4.1V)
		);
	}
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为给每个8915A芯片发送配置寄存器组A指令
	{
		if (isospi_reverse == TRUE)		//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			c_ic = current_ic;			//选择正序将配置寄存器组A指令放入数组
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//选择倒序将配置寄存器组A指令放入数组
		}

		for (dataTemp = 0; dataTemp<6; dataTemp++)//若isospi一直为FALSE，则此循环将配置寄存器组A中配置好的数组数据一一写入发送数组中，并且是通过倒序发送给每个8915A芯片
		{
			write_buffer[write_count] = configAArray[c_ic].tx_data[dataTemp];
			write_count++;
		}
	}
	BF8915A_write(total_ic, cmd, write_buffer);//写BF8915A命令和写有效载荷数据的通用函数
}

/*!
    \brief      Write the BF8915A CFGRB(写BF8915A的配置寄存器组B)
		\param[in]  total_ic 	 : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configBData: A two dimensional array of the configuration data that will be written(将要写入配置数据的二维数组)
    \retval     none   	                  					  
*/
void BF8915A_wrcfgb(u8 total_ic, ConfigB_TypeDef configBData[])
{
	u8 cmd[2] = {0x00 , 0x02};			//发送写BF8915A的配置寄存器组B指令的高低共16位
	Reg_TypeDef configBArray[TOTAL_IC];	//定义一个结构体为Reg_TypeDef的数据结构B数组,用于存储指令数据
	u8 write_buffer[TOTAL_IC * 0x6U];	//寄存器组中的6个8位寄存器
	u8 write_count = 0;					//数组计数器，最大为TOTAL_IC * 0x6U
	u8 c_ic = 0;						//当前正在写入数组的8915A芯片序号
	u8 current_ic = 0;					//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 dataTemp = 0;					//计数器，每6次一循环，用于计数每个寄存器组当前循环到的寄存器
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//配置每个8915A的配置寄存器组B
	{
		BF8915A_set_cfgrb(current_ic, 
						configBArray,
						configBData[current_ic].dcc,		//放电电池开关1:打开Cell x 的开关  0:关闭Cell x 的开关 (默认)
						configBData[current_ic].bl_duty_sel,//主动均衡占空比选择  占空比=(BL_DUTY_SEL+1)*10%，配置范围10%~100%，当大于9时是100%。
						configBData[current_ic].dcto,		//放电定时溢出值配置
						configBData[current_ic].vuv_bl_th,	//均衡欠压阈值
						configBData[current_ic].dtmen		//在自动均衡下，如果DTMEN=1，当欠压(小于VUV_BL)关闭放电开关，如果DTMEN=0，不进行欠压比较
						);
	}	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为给每个8915A芯片发送配置寄存器组B指令
	{
		if (isospi_reverse == TRUE)							//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			c_ic = current_ic;								//选择正序将配置寄存器组B指令放入数组
		}
		else
		{
			c_ic = total_ic - current_ic - 1;				//选择倒序将配置寄存器组B指令放入数组
		}

		for (dataTemp = 0; dataTemp<6; dataTemp++)			//若isospi一直为FALSE，则此循环将配置寄存器组B中配置好的数组数据一一写入发送数组中，并且是通过倒序发送给每个8915A芯片
		{
			write_buffer[write_count] = configBArray[c_ic].tx_data[dataTemp];
			write_count++;
		}
	}
	BF8915A_write(total_ic, cmd, write_buffer);//写BF8915A命令和写有效载荷数据的通用函数
}

/*!
    \brief      Write the BF8915A CFGRC(写BF8915A的配置寄存器组C)
		\param[in]  total_ic 	 : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configCData: A two dimensional array of the configuration data that will be written(将要写入配置数据的二维数组)
    \retval     none   	                  					  
*/
void BF8915A_wrcfgc(u8 total_ic, ConfigC_TypeDef configCData[])
{
	u8 cmd[2] = {0x00 , 0x03};			//发送写BF8915A的配置寄存器组C指令的高低共16位
	Reg_TypeDef configCArray[TOTAL_IC];	//定义一个结构体为Reg_TypeDef的数据结构C数组,用于存储指令数据
	u8 write_buffer[TOTAL_IC * 0x6U];	//寄存器组中的6个8位寄存器
	u8 write_count = 0;					//数组计数器，最大为TOTAL_IC * 0x6U
	u8 c_ic = 0;						//当前正在写入数组的8915A芯片序号
	u8 current_ic = 0;					//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 data = 0;						//计数器，每6次一循环，用于计数每个寄存器组当前循环到的寄存器

	for (current_ic = 0; current_ic<total_ic; current_ic++)//配置每个8915A的配置寄存器组C
	{
		BF8915A_set_cfgrc(current_ic, 
							configCArray,
							configCData[current_ic].vuv_m_th,//监测模式下，欠压阈值
							configCData[current_ic].vov_m_th,//监测模式下，过压阈值
							configCData[current_ic].itov_m_th,//监测模式下，内部过温阈值
							configCData[current_ic].extov_m_th//监测模式下，外部过温阈值
							);
	}

	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为给每个8915A芯片发送配置寄存器组C指令
	{
		if (isospi_reverse == TRUE)		//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			c_ic = current_ic;			//选择正序将配置寄存器组C指令放入数组
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//选择倒序将配置寄存器组C指令放入数组
		}

		for (data = 0; data<6; data++)	//若isospi一直为FALSE，则此循环将配置寄存器组C中配置好的数组数据一一写入发送数组中，并且是通过倒序发送给每个8915A芯片
		{
			write_buffer[write_count] = configCArray[c_ic].tx_data[data];
			write_count++;
		}
	}
	BF8915A_write(total_ic, cmd, write_buffer);//写BF8915A命令和写有效载荷数据的通用函数
}

/*!
    \brief      Write the BF8915A CFGRD(写BF8915A的配置寄存器组D)
		\param[in]  total_ic 	 : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDData: A two dimensional array of the configuration data that will be written(将要写入配置数据的二维数组)
    \retval     none   	                  					  
*/
void BF8915A_wrcfgd(u8 total_ic, ConfigD_TypeDef configDData[])
{
	u8 cmd[2] = {0x00 , 0x04};			//发送写BF8915A的配置寄存器组D指令的高低共16位
	Reg_TypeDef configDArray[TOTAL_IC];	//定义一个结构体为Reg_TypeDef的数据结构D数组,用于存储指令数据
	u8 write_buffer[TOTAL_IC * 0x6U];	//寄存器组中的6个8位寄存器
	u8 write_count = 0;					//数组计数器，最大为TOTAL_IC * 0x6U
	u8 c_ic = 0;						//当前正在写入数组的8915A芯片序号
	u8 current_ic = 0;					//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 data = 0;						//计数器，每6次一循环，用于计数每个寄存器组当前循环到的寄存器
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//配置每个8915A的配置寄存器组D
	{
		BF8915A_set_cfgrd(current_ic, 
							configDArray,
							configDData[current_ic].csel_m, 		//监测模式下，电池电压监测比较选择 1:相应的电池就会在监测模式下进行过欠压比较，置标志位
																  	//0:相应的电池就会在监测模式下不进行过欠压比较，不置标志位
							configDData[current_ic].gsel_m,			//监测模式下，GPIO0~7比较选择
							configDData[current_ic].isospi_wake_t,	//isospi(菊花链SPI)唤醒时间，0:10us		1:20us
							configDData[current_ic].adc_rst_sel,	//测量第1个通道ADC复位选择，0:所有测量通道都复位ADC，1:只有第1个通道复位ADC，之后2~n通道不复位ADC
							configDData[current_ic].time_sel_m,		//监测模式下，定时监测时间配置，定时时间=(TIME_SEL_M+1)*1s(2~64s)，最小配置值是1(2s)
							configDData[current_ic].adc_init_t2,	//ADC测量第2~n个通道ADC初始化时间，(ADC_INIT_T2+1)*20us(20~320us)
							configDData[current_ic].adc_init_t1,	//ADC测量第1个通道ADC初始化时间(ADC_INIT_T1+1)*100us(100~1600us)
							configDData[current_ic].trim2_en,		//修调2使能，1:使能，0:不使能
							configDData[current_ic].trim1_en,		//修调1使能，1:使能，0:不使能
							configDData[current_ic].err_t_sel_m,	//监测模式下发生异常，上报的间隔时间  00:200ms，01:400ms，10:600ms，11:800ms
							configDData[current_ic].vd33t_on_m		//监测模式下，VREF1和VD33T的启动时间配置(5+VD33T_ON_M*10)ms，范围是5~155ms
							);
	}
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为给每个8915A芯片发送配置寄存器组D指令
	{
		if (isospi_reverse == TRUE)		//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			c_ic = current_ic;			//选择正序将配置寄存器组D指令放入数组
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//选择倒序将电配置寄存器组D指令放入数组
		}

		for (data = 0; data<6; data++)	//若isospi一直为FALSE，则此循环将配置寄存器组D中配置好的数组数据一一写入发送数组中，并且是通过倒序发送给每个8915A芯片
		{
			write_buffer[write_count] = configDArray[c_ic].tx_data[data];//倒序写入指令
			write_count++;
		}
	}
	BF8915A_write(total_ic, cmd, write_buffer);//写BF8915A命令和写有效载荷数据的通用函数
}

/*!
    \brief      Reads the BF8915A CFGRA register(读BF8915A的配置寄存器组A)
		\param[in]  total_ic 	 : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configAData: A two dimensional array that the function stores the read configuration data(该函数存储读取的是配置数据的二维数组)
    \retval     pec_error
*/
u8 BF8915A_rdcfga(u8 total_ic, ConfigA_TypeDef configAData[])
{
	u8 cmd[2]= {0x00U, 0x05U};			//发送读BF8915A的配置寄存器组A指令的高低共16位
	Reg_TypeDef configAArray[TOTAL_IC];	//定义一个结构体为Reg_TypeDef的数据结构A数组,用于存储指令数据
	u8 read_buffer[256];				//存储SPI端口读取的数据 的数组
	u8 pec_error = 0;					//检查检错码
	u16 data_pec;						//收到的每个配置寄存器组A中的PEC(检错码)
	u16 calc_pec;						//计算收到的每个配置寄存器组A中指令数据的PEC(检错码)
	u8 c_ic = 0;						//当前正在读入数组的8915A芯片序号
	u8 current_ic = 0;					//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 byte = 0;						//计数器，每8次一循环，用于计数每个寄存器组A当前循环到的寄存器以及它的PEC(检错码)

	pec_error = BF8915A_read(total_ic, cmd, read_buffer);//执行写BF8915A命令和读数据，并回传检查错误码是否正确

	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为读出每个8915A芯片配置寄存器组A中的指令，并检查是否有错误
	{
		if (isospi_reverse == FALSE)	//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			c_ic = current_ic;			//选择正序将配置寄存器组A中的指令放入数组
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//选择倒序将配置寄存器组A中的指令放入数组
		}

		for (byte=0; byte<8; byte++)	//若isospi一直为FALSE，则此循环将收到的每个配置寄存器中的数据和它的2字节PEC(检错码)写入接收数组中，接收的是倒序的8915A芯片
		{
			configAArray[c_ic].rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		calc_pec = Bf8915pec15_calc(6,&read_buffer[8*current_ic]);//计算接收回来的寄存器组(6个8位寄存器)中的PEC(检错码)
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);//将read_buffer[]中接收到的PEC(检错码)按高低共16位存入data_pec中
		if (calc_pec != data_pec )//检查检错码是否匹配
		{
			configAArray[c_ic].rx_pec_match = 1;//检错码不正确
		}
		else configAArray[c_ic].rx_pec_match = 0;//检错码正确
	}
	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为存入每个8915A芯片配置寄存器组A中的指令
	{
		configAData[current_ic].gpio_pd_en = configAArray[current_ic].rx_data[0];		//CFGAR0的8位，GPIO下拉使能，每一位对应一个IO口
		configAData[current_ic].gpio_analog_en = configAArray[current_ic].rx_data[1];	//CFGAR1的8位，GPIO0~7模式选择
		configAData[current_ic].rsvd2 = (BOOLEAN)((configAArray[current_ic].rx_data[2] & 0x08U) == 0x08U);//保留位，永远置0
		configAData[current_ic].rsvd1 = (BOOLEAN)((configAArray[current_ic].rx_data[2] & 0x04U) == 0x04U);//保留位，永远置0
		configAData[current_ic].osr_sel = (BOOLEAN)((configAArray[current_ic].rx_data[2] & 0x02U) == 0x02U);//CFGAR2的第2位,ADC过采样率选择
		configAData[current_ic].refon = (BOOLEAN)((configAArray[current_ic].rx_data[2] & 0x01U) == 0x01U);//CFGAR2的第1位,VREF1(基准电压)配置
		configAData[current_ic].uv_th = (u16)(configAArray[current_ic].rx_data[3]) + (((u16)((configAArray[current_ic].rx_data[4])&0x0FU)) << 0x8U);
		                                                                                    //CFGAR3的8位和CFGAR4的低4位,将CFGAR1的低4位放入12~9位，欠压阈值
		configAData[current_ic].ov_th = (((u16)configAArray[current_ic].rx_data[5]) << 0x4U)  + (((configAArray[current_ic].rx_data[4])&0xF0U) >> 0x4U);
		                                                                                    //CFGAR5的8位和CFGAR4的高4位,将CFGAR4的高4位放入12~9位，过压阈值
		configAData[current_ic].rx_pec_match = configAArray[current_ic].rx_pec_match;//每个配置寄存器组A中的检错码是否匹配
	}
	return(pec_error);//回传检查检错码的值
}

/*!
    \brief      Reads the BF8915A CFGRB register(读BF8915A的配置寄存器组B)
		\param[in]  total_ic 	 : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configBData: A two dimensional array that the function stores the read configuration data(该函数存储读取的是配置数据的二维数组)
    \retval     pec_error   	                  					  
*/
u8 BF8915A_rdcfgb(u8 total_ic, ConfigB_TypeDef configBData[])
{
	u8 cmd[2]= {0x00 , 0x06};			//发送读BF8915A的配置寄存器组B指令的高低共16位
	Reg_TypeDef configBArray[TOTAL_IC];	//定义一个结构体为Reg_TypeDef的数据结构B数组,用于存储指令数据
	u8 read_buffer[256];				//存储SPI端口读取的数据 的数组
	u8 pec_error = 0;					//检查检错码
	u16 data_pec;						//收到的每个配置寄存器组B中的PEC(检错码)
	u16 calc_pec;						//计算收到的每个配置寄存器组B中指令数据的PEC(检错码)
	u8 c_ic = 0;						//当前正在读入数组的8915A芯片序号
	u8 current_ic = 0;					//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 byte=0; 							//计数器，每8次一循环，用于计数每个寄存器组B中当前循环到的寄存器以及它的PEC(检错码)

	pec_error = BF8915A_read(total_ic, cmd, read_buffer);//执行写BF8915A命令和读数据，并回传检查错误码是否正确

	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为读出每个8915A芯片配置寄存器组B中的指令，并检查是否有错误
	{
		if (isospi_reverse == FALSE)	//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			c_ic = current_ic;			//选择正序将配置寄存器组B中的指令放入数组
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//选择倒序将配置寄存器组B中的指令放入数组
		}

		for (byte=0; byte<8; byte++)	//若isospi一直为FALSE，则此循环将收到的每个配置寄存器中的数据和它的2字节PEC(检错码)写入接收数组中，接收的是倒序的8915A芯片
		{
			configBArray[c_ic].rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		calc_pec = Bf8915pec15_calc(6,&read_buffer[8*current_ic]);//计算接收回来的寄存器组(6个8位寄存器)中的PEC(检错码)
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);//将read_buffer[]中接收到的PEC(检错码)按高低共16位存入data_pec中
		if (calc_pec != data_pec )		//检查检错码是否匹配
		{
			configBArray[c_ic].rx_pec_match = 1;//检错码不正确
		}
		else configBArray[c_ic].rx_pec_match = 0;//检错码正确
	}
	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为存入每个8915A芯片配置寄存器组B中的指令
	{
		configBData[current_ic].dcc = (((u16)configBArray[current_ic].rx_data[1]) << 0x8U) + configBArray[current_ic].rx_data[0];
		                                                                                          	//CFGBR0的8位和CFGBR1的8位,将CFGBR1的8位放入高8位，放电电池开关
		configBData[current_ic].bl_duty_sel = (configBArray[current_ic].rx_data[2] & 0xF0U) >> 0x4U;//CFGBR2的高4位,主动均衡占空比选择
		configBData[current_ic].dcto = configBArray[current_ic].rx_data[2] & 0x0FU;					//CFGBR2的低4位,放电定时溢出值配置
		configBData[current_ic].vuv_bl_th = (u16)configBArray[current_ic].rx_data[3] + (((u16)(configBArray[current_ic].rx_data[4] & 0x0F)) << 0x8U);
		                                                                                         	 //CFGBR3的8位和CFGBR4的低4位,将CFGBR1的低4位放入12~9位，均衡欠压阈值
		configBData[current_ic].dtmen = (BOOLEAN)((configBArray[current_ic].rx_data[4] & 0x10U) == 0x10U);//CFGBR4的第5位,使能放电监测器
		configBData[current_ic].rx_pec_match = configBArray[current_ic].rx_pec_match;				//每个配置寄存器组B中的检错码是否匹配
	}
	return(pec_error);//回传检查检错码的值
}

/*!
    \brief      Reads the BF8915A CFGRC register(读BF8915A的配置寄存器组C)
		\param[in]  total_ic 	 : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configCData: A two dimensional array that the function stores the read configuration data(该函数存储读取的是配置数据的二维数组)
    \retval     none   	                  					  
*/
u8 BF8915A_rdcfgc(u8 total_ic, ConfigC_TypeDef configCData[])
{
	u8 cmd[2]= {0x00 , 0x07};				//发送读BF8915A的配置寄存器组C指令的高低共16位
	Reg_TypeDef configCArray[TOTAL_IC];		//定义一个结构体为Reg_TypeDef的数据结构C数组,用于存储指令数据
	u8 read_buffer[256];					//存储SPI端口读取的数据 的数组
	u8 pec_error = 0;						//检查检错码
	u16 data_pec;							//收到的每个配置寄存器组C中的PEC(检错码)
	u16 calc_pec;							//计算收到的每个配置寄存器组C中指令数据的PEC(检错码)
	u8 c_ic = 0;							//当前正在读入数组的8915A芯片序号
	u8 current_ic = 0;						//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 byte=0;								//计数器，每8次一循环，用于计数每个寄存器组B中当前循环到的寄存器以及它的PEC(检错码)

	pec_error = BF8915A_read(total_ic, cmd, read_buffer);//执行写BF8915A命令和读数据，并回传检查错误码是否正确

	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为读出每个8915A芯片配置寄存器组C中的指令，并检查是否有错误
	{
		if (isospi_reverse == FALSE)		//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			c_ic = current_ic;				//选择正序将配置寄存器组C中的指令放入数组
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//选择倒序将配置寄存器组C中的指令放入数组
		}

		for (byte=0; byte<8; byte++)//若isospi一直为FALSE，则此循环将收到的每个配置寄存器中的数据和它的2字节PEC(检错码)写入接收数组中，接收的是倒序的8915A芯片
		{
			configCArray[c_ic].rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		calc_pec = Bf8915pec15_calc(6,&read_buffer[8*current_ic]);//计算接收回来的寄存器组(6个8位寄存器)中的PEC(检错码)
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);//将read_buffer[]中接收到的PEC(检错码)按高低共16位存入data_pec中
		if (calc_pec != data_pec )//检查检错码是否匹配
		{
			configCArray[c_ic].rx_pec_match = 1;//检错码不正确
		}
		else configCArray[c_ic].rx_pec_match = 0;//检错码正确
	}
	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为存入每个8915A芯片配置寄存器组C中的指令
	{
		configCData[current_ic].vuv_m_th = configCArray[current_ic].rx_data[0] + (((u16)(configCArray[current_ic].rx_data[1] & 0x0F)) << 0x8U);
		                                                                                          //CFGCR0的8位和CFGCR1的低4位,将CFGCR1的低4位放入12~9位，监测模式下，欠压阈值
		configCData[current_ic].vov_m_th = (((u16)configCArray[current_ic].rx_data[2]) << 0x4U) + ((configCArray[current_ic].rx_data[1] & 0xF0) >> 0x04U);
		                                                                    //CFGCR1的高4位和CFGCR2的8位,将CFGCR1高4位放入低4位，将CFGCR2的8位放入12~5位，监测模式下，过压阈值
		configCData[current_ic].itov_m_th  = configCArray[current_ic].rx_data[3] + (((u16)(configCArray[current_ic].rx_data[4] & 0x0F)) << 0x8U);
		                                                                                          //CFGBC3的8位和CFGCR1的低4位,将CFGCR4的低4位放入12~9位，监测模式下，内部过温阈值
		configCData[current_ic].extov_m_th = (((u16)configCArray[current_ic].rx_data[5]) << 0x4U) + ((configCArray[current_ic].rx_data[4] & 0xF0) >> 0x04U);
		                                                                    //CFGCR4的高4位和CFGCR5的8位,将CFGCR4高4位放入低4位，将CFGCR5的8位放入12~5位，监测模式下，外部过温阈值
		configCData[current_ic].rx_pec_match = configCArray[current_ic].rx_pec_match;//每个配置寄存器组C中的检错码是否匹配
	}
	return(pec_error);//回传检查检错码的值
}

/*!
    \brief      Reads the BF8915A CFGRD register(读BF8915A的配置寄存器组D)
		\param[in]  total_ic 	 : Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  configDData: A two dimensional array that the function stores the read configuration data(该函数存储读取的是配置数据的二维数组)
    \retval     none   	                  					  
*/
u8 BF8915A_rdcfgd(u8 total_ic, ConfigD_TypeDef configDData[])
{
	u8 cmd[2]= {0x00 , 0x08};			//发送读BF8915A的配置寄存器组D指令的高低共16位
	Reg_TypeDef configDArray[TOTAL_IC];	//定义一个结构体为Reg_TypeDef的数据结构D数组,用于存储指令数据
	u8 read_buffer[256];				//存储SPI端口读取的数据 的数组
	u8 pec_error = 0;					//检查检错码
	u16 data_pec;						//收到的每个配置寄存器组D中的PEC(检错码)
	u16 calc_pec;						//计算收到的每个配置寄存器组D中指令数据的PEC(检错码)
	u8 c_ic = 0;						//当前正在读入数组的8915A芯片序号
	u8 current_ic = 0;					//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 byte=0; 							//计数器，每8次一循环，用于计数每个寄存器组B中当前循环到的寄存器以及它的PEC(检错码)

	pec_error = BF8915A_read(total_ic, cmd, read_buffer);//执行写BF8915A命令和读数据，并回传检查错误码是否正确

	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为读出每个8915A芯片配置寄存器组D中的指令，并检查是否有错误
	{
		if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
		{
			c_ic = current_ic;//选择正序将配置寄存器组D中的指令放入数组
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//选择倒序将配置寄存器组D中的指令数组
		}

		for (byte=0; byte<8; byte++)//若isospi一直为FALSE，则此循环将收到的每个配置寄存器中的数据和它的2字节PEC(检错码)写入接收数组中，接收的是倒序的8915A芯片
		{
			configDArray[c_ic].rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		calc_pec = Bf8915pec15_calc(6,&read_buffer[8*current_ic]);//计算接收回来的寄存器组(6个8位寄存器)中的PEC(检错码)
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);//将read_buffer[]中接收到的PEC(检错码)按高低共16位存入data_pec中
		if (calc_pec != data_pec )//检查检错码是否匹配
		{
			configDArray[c_ic].rx_pec_match = 1U;//检错码不正确
			pec_error = 1U;
		}
		else configDArray[c_ic].rx_pec_match = 0;//检错码正确
	}
	for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为存入每个8915A芯片配置寄存器组D中的指令
	{
		configDData[current_ic].csel_m = (((u16)configDArray[current_ic].rx_data[1]) << 0x8U) + configDArray[current_ic].rx_data[0];
		                                                                                            //CFGDR0的8位和CFGDR1的8位,将CFGDR1的8位放入高8位，读出监测模式下，电池电压监测比较选择
		configDData[current_ic].gsel_m = configDArray[current_ic].rx_data[2];						//CFGDR2的8位,读出监测模式下，GPIO0~7比较选择
		configDData[current_ic].isospi_wake_t = (BOOLEAN)((configDArray[current_ic].rx_data[3] & 0x80) == 0x80);//CFGDR3的第8位,菊花链SPI的唤醒时间
		configDData[current_ic].adc_rst_sel = (BOOLEAN)((configDArray[current_ic].rx_data[3] & 0x40) == 0x40);	//CFGDR3的第7位,测量第1个通道ADC复位选择
		configDData[current_ic].time_sel_m = configDArray[current_ic].rx_data[3] & 0x3F;			//CFGDR3的6~1位,监测模式下，定时监测时间配置
		configDData[current_ic].adc_init_t2 = (configDArray[current_ic].rx_data[4] & 0xF0) >> 0x4U;	//CFGDR4的高4位,ADC测量第2~n个通道ADC初始化时间
		configDData[current_ic].adc_init_t1 = configDArray[current_ic].rx_data[4] & 0x0F;			//CFGDR4的低4位,ADC测量第1个通道ADC初始化时间
		configDData[current_ic].trim2_en = (BOOLEAN)((configDArray[current_ic].rx_data[5] & 0x80) == 0x80U);//CFGDR5的第8位,修调2使能
		configDData[current_ic].trim1_en = (BOOLEAN)((configDArray[current_ic].rx_data[5] & 0x40) == 0x40U);//CFGDR5的第7位,修调1使能
		configDData[current_ic].err_t_sel_m = (configDArray[current_ic].rx_data[5] & 0x30) >> 0x4U; //CFGDR5的第6~5位,监测模式下发生异常，上报的间隔时间
		configDData[current_ic].vd33t_on_m = configDArray[current_ic].rx_data[5] & 0x0F;			//CFGDR5的低4位,监测模式下，VREF1和VD33T的启动时间配置
		configDData[current_ic].rx_pec_match = configDArray[current_ic].rx_pec_match;				//每个配置寄存器组D中的检错码是否匹配
	}
	return(pec_error);//回传检查检错码的值
}

/*!
    \brief      ADCV CMD(电池电压ADC转换指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择:0、1、2、3)
		\param[in]  DISCP : DISCP_DISABLE、DISCP_ENABLE(放电允许位:放电允许位写0和1)
		\param[in]  CSEL  : CSEL_CH_ALL、CSEL_CH_1and9、CSEL_CH_2and10、CSEL_CH_3and11、CSEL_CH_4and12
												             CSEL_CH_5and13、CSEL_CH_6and14、CSEL_CH_7and15、CSEL_CH_8and16
   (ADC转换的电池选择:所有电池(0)、电池1和9(1)、电池2和10(2)、电池3和11(3)、电池4和12(4)
												             电池5和13(5)、电池6和14(6)、电池7和15(7)、电池8和16(8))
    \retval     none   	                  					  
*/
void BF8915A_adcv(u8 CLKSEL, u8 OSR, u8 DISCP, u8 CSEL)
{
	u8 cmd[2] = {0U};			//初始化数组cmd[]
	u8 clksel_bits = 0x0U;		//设置CLKSEL的2个位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;			//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADCV << 0x1U);	//发送指令的高8位,#define CMD_ADCV	0x1U
	clksel_bits = (CLKSEL & 0x01) << 7;			//设置CLKSEL[0]
	cmd[1] = clksel_bits | (OSR<<5) | (DISCP<<4) | CSEL;//发送指令的低8位,详情请参考规格书中 操作指令配置 ADCV 的指令描述
	BF8915A_cmd(cmd);							//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      CVST CMD(电池电压自测试命令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_2566、OSR_512(ADC过采样频率选择:0、1、2、3)
		\param[in]  SFTEST: SELFTEST_6666		SELFTEST_9999(自测试模式选择:自测试1 自测试2)
    \retval     none   	                  				
*/  
void BF8915A_cvst(u8 CLKSEL, u8 OSR, u8 SFTEST)
{
	u8 cmd[2] = {0U};			//初始化数组cmd[]
	u8 clksel_bits = 0x0U;		//设置CLKSEL的2个位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;			//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_CVST << 0x1U);	//发送指令的高8位,#define CMD_CVST	0x03U
	clksel_bits = (CLKSEL & 0x01) << 7;			//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (SFTEST<<4);//发送指令的低8位,详情请参考规格书中 操作指令配置 CVST 的指令描述
	BF8915A_cmd(cmd);							//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      ADOL CMD(电池重叠单元测量指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择:0、1、2、3)
		\param[in]  DISCP : DISCP_DISABLE、DISCP_ENABLE	(放电允许位:放电允许位写0和1)
    \retval     none   		                  				  
*/
void BF8915A_adol(u8 CLKSEL, u8 OSR, u8 DISCP)
{
	u8 cmd[2] = {0U};			//初始化数组cmd[]
	u8 clksel_bits = 0x0U;		//设置CLKSEL的2个位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;			//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADOL << 0x1U);	//发送指令的高8位,#define CMD_ADOL	0x04U
	clksel_bits = (CLKSEL & 0x01) << 7;			//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (DISCP<<4) | 0x01U;//发送指令的低8位,详情请参考规格书中 操作指令配置 ADOL 的指令描述
	BF8915A_cmd(cmd);							//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      ADGP CMD(GPIO ADC转换指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择)
		\param[in]  GSEL 	: GSEL_CH_ALL、	 GSEL_CH_GPIO0、GSEL_CH_GPIO1、GSEL_CH_GPIO2、GSEL_CH_GPIO3
												GSEL_CH_VD33T、GSEL_CH_GPIO4、GSEL_CH_GPIO5、GSEL_CH_GPIO6、GSEL_CH_GPIO7
  (ADC转换的GPIO选择 :所有GPIO0~GPIO7,VD33T、GPIO0、GPIO1、GPIO2、GPIO3
											                 VD33T、GPIO4、GPIO5、GPIO6、GPIO7)
    \retval     none   	                  				  
*/
void BF8915A_adgp(u8 CLKSEL, u8 OSR, u8 GSEL)
{
	u8 cmd[2] = {0U};		//初始化数组cmd[]
	u8 clksel_bits = 0x0U;	//设置CLKSEL的2个位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;			//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADGP << 0x1U);	//发送指令的高8位,#define CMD_ADGP	0x05U
	clksel_bits = (CLKSEL & 0x01) << 7;			//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | GSEL;	//发送指令的低8位,详情请参考规格书中 操作指令配置 ADGP 的指令描述
	BF8915A_cmd(cmd);							//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      GPST CMD(GPIO输入自测试指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择:0、1、2、3)
		\param[in]  SFTEST: SELFTEST_6666		SELFTEST_9999(自测试模式选择:自测试1 自测试2)
    \retval     none   	                  				
*/  
void BF8915A_gpst(u8 CLKSEL, u8 OSR, u8 SFTEST )
{
	u8 cmd[2] = {0U};				//初始化数组cmd[]
	u8 clksel_bits = 0x0U;			//设置CLKSEL的2个位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;				//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_GPST << 0x1U);		//发送指令的高8位,#define CMD_GPST	0x07U
	clksel_bits = (CLKSEL & 0x01) << 7;				//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (SFTEST<<4);	//发送指令的低8位,详情请参考规格书中 操作指令配置 GPST 的指令描述
	BF8915A_cmd(cmd);								//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      ADSTAT CMD(测量内部设备参数指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择:0、1、2、3)
		\param[in]  STSEL : STATA、STATB、STATC、STATD(ADC转换的内部参数选择)
    \retval     none   	                  				
*/                                  
void BF8915A_adstat(u8 CLKSEL, u8 OSR, u8 STSEL)
{
	u8 cmd[2] = {0U};				//初始化数组cmd[]
	u8 clksel_bits = 0x0U;			//设置CLKSEL的2个位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;			//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADSTAT << 0x1U);//发送指令的高8位,#define CMD_ADSTAT	0x08U
	clksel_bits = (CLKSEL & 0x01) << 7;			//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | STSEL;	//发送指令的低8位,详情请参考规格书中 操作指令配置 ADSTAT 的指令描述
	BF8915A_cmd(cmd);							//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      STATST CMD(自测试内部设备参数指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择:0、1、2、3)
		\param[in]  SFTEST: SELFTEST_6666		SELFTEST_9999(自测试模式选择:自测试1 自测试2)
    \retval     none   	                  				
*/  
void BF8915A_statst(u8 CLKSEL, u8 OSR, u8 SFTEST)
{
	u8 cmd[2] = {0U};				//初始化数组cmd[]
	u8 clksel_bits = 0x0U;			//设置CLKSEL 位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;				//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_STATST << 0x1U);	//发送指令的高8位,#define CMD_STATST	0x09U
	clksel_bits = (CLKSEL & 0x01) << 7;				//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (SFTEST<<4);	//发送指令的低8位,详情请参考规格书中 操作指令配置 CMD_STATST 的指令描述
	BF8915A_cmd(cmd);								//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      ADCVGP CMD(测量电池电压和GPIO输入指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择:0、1、2、3)
		\param[in]  DISCP : DISCP_DISABLE、DISCP_ENABLE(放电允许位:放电允许位写0和1)
    \retval     none   	                  				
*/  
void BF8915A_adcvgp(u8 CLKSEL, u8 OSR, u8 DISCP)
{
	u8 cmd[2] = {0U};						//初始化数组cmd[]
	u8 clksel_bits = 0x0U;					//设置CLKSEL的2个位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;		//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADCVGP << 0x1U);	//发送指令的高8位,#define CMD_ADCVGP	0x0aU
	clksel_bits = (CLKSEL & 0x01) << 7;				//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (DISCP<<4);	//发送指令的低8位,详情请参考规格书中 操作指令配置 CMD_ADCVGP 的指令描述
	BF8915A_cmd(cmd);								//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      MNT CMD(监测指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择:0、1、2、3)
		\param[in]  MNMOD : MNMOD_DISABLE、MNMOD_1、MNMOD_2、MNMOD_3(监测模式选择:不进入监测模式，指令无效(00)、模式1(01)、模式2(02)、模式3(03))
		\param[in]  STRMN : STRMN_ENABLE、STRMN_DISABLE(监测模式使能:开启监测模式(1)、关闭监测模式(2))
		\param[in]  BLEN  : BLEN_ON、BLEN_OFF	(均衡开关总控制信号2:关闭(0)、开启(1))
    \retval     none                     				
*/                                       				
void BF8915A_mnt(u8 CLKSEL, u8 OSR, u8 MNMOD, u8 STRMN, u8 BLEN)
{
	u8 cmd[2] = {0U};				//初始化数组cmd[]
	u8 clksel_bits = 0x0U;			//设置CLKSEL的2个 位的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;			//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_MNT << 0x1U);	//发送指令的高8位,#define CMD_MNT	0x0cU
	clksel_bits = (CLKSEL & 0x01) << 7;			//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (MNMOD<<3) | (STRMN << 2) | BLEN ;//发送指令的低8位,详情请参考规格书中 操作指令配置 CMD_MNT 的指令描述
	BF8915A_cmd(cmd);							//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      STRBL CMD(被动均衡指令)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   :	OSR_64 、OSR_128、OSR_256、OSR_512(ADC过采样频率选择)
		\param[in]  OESEL :	OESEL_ODD、OESEL_EVEN	(均衡放电电池奇偶选择:奇数节(0)、偶数节(1))
		\param[in]  BLEN  : BLEN_ON、BLEN_OFF(均衡开关总控制信号 2:开启(1)、关闭(0))
    \retval     none                     
*/                      
void BF8915A_strbl(u8 CLKSEL, u8 OSR, u8 OESEL,u8 BLEN)
{
	u8 cmd[2] = {0U};				//初始化数组cmd[]
	u8 clksel_bits = 0x0U;			//设置CLKSEL的2个位 的变量
	clksel_bits = (CLKSEL & 0x02) >> 1;			//设置CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_STRBL << 0x1U);	//发送指令的高8位,#define CMD_STRBL	0x0dU
	clksel_bits = (CLKSEL & 0x01) << 7;			//设置CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (OESEL<<4) | BLEN;//发送指令的低8位,详情请参考规格书中 操作指令配置 CMD_STRBL 的指令描述
	BF8915A_cmd(cmd);							//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      Reads the raw cell voltage register data(读取原始电池电压寄存器数据)
		\param[in]  reg     : Determines which  register is read back(确定回读哪个电池电压寄存器)
		\param[in]  total_ic: the Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  cellData: Array of the unparsed codes(未解析代码的数组)
    \retval     none
*/
void BF8915A_rdcv_reg(u8 reg, u8 total_ic, u8 *cellData)
{
	const u8 REG_LEN = 8; 	//number of 6 bytes in each ICs register + 2 bytes for the PEC(每个集成电路寄存器中的6字节数加PEC(检错码)的2个字节)
	u8 cmd[4];				//初始化数组cmd[]
	u16 cmd_pec;			//计算出发送指令的PEC(检错码)
	cmd[0] = 0x00;			//发送指令的高8位
	cmd[1] = reg + 0x08U;	//发送指令的低8位，详情在8915A.h文件中 电池电压寄存器对应的命令
	cmd_pec = Bf8915pec15_calc(2, cmd);	//计算出发送指令的PEC(检错码)
	cmd[2] = (u8)(cmd_pec >> 8);		//发送指令的PEC(检错码)的高八位
	cmd[3] = (u8)(cmd_pec);				//发送指令的PEC(检错码)的低八位

	if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
	{
		BF8915_CS_LOW();//将片选CS设置为低电平
		spi_write_read(cmd,4,cellData,(REG_LEN*total_ic));//使用SPI端口写入和读取设定数量的字节
		BF8915_CS_HIGH();//将片选CS设置为高电平
	}
	else
	{
		BF8915_CS_LOW_Reverse();//将片选CS设置为低电平
		spi_write_read(cmd,4,cellData,(REG_LEN*total_ic));//使用SPI端口写入和读取设定数量的字节
		BF8915_CS_HIGH_Reverse();//将片选CS设置为高电平
	}
}

/*!
    \brief      reads BF8915A GPIO registers(读取BF8915A的GPIO寄存器)
                The function reads a single GPIO voltage register and stores the read data in the *data point as a byte array. 
                This function is rarely used outside of the BF8915A_rdgv() command.
               (该函数读取单个GPIO电压寄存器，并将读取的数据作为字节数组存储在*data数据中。此功能很少在BF8915A_rdgv()命令之外使用。)
		\param[in]  reg     : Determines which  register is read back(确定回读哪个电池电压寄存器)
		\param[in]  total_ic: the Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  gpioData: Array of the unparsed codes(未解析代码的数组)
    \retval     none
*/
void BF8915A_rdgv_reg(u8 reg, u8 total_ic, u8 *gpioData)
{
	const u8 REG_LEN = 8; // number of 6 bytes in the register + 2 bytes for the PEC(寄存器中的6字节数加上PEC(检错码)的2个字节)
	u8 cmd[4];			//初始化数组cmd[]
	u16 cmd_pec;		//计算出发送指令的PEC(检错码)
	cmd[0] = 0x00;		//发送指令的高8位
	cmd[1] = reg + 0x0EU;				//发送指令的低8位，详情在8915A.h文件中 GPIOG电压
	cmd_pec = Bf8915pec15_calc(2, cmd);	//计算出发送指令的PEC(检错码)
	cmd[2] = (u8)(cmd_pec >> 8);		//发送指令的PEC(检错码)的高八位
	cmd[3] = (u8)(cmd_pec);				//发送指令的PEC(检错码)的低八位
	
	if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
	{
		BF8915_CS_LOW();//将片选CS设置为低电平
		spi_write_read(cmd,4,gpioData,(REG_LEN*total_ic));//使用SPI端口写入和读取设定数量的字节
		BF8915_CS_HIGH();//将片选CS设置为高电平
	}
	else
	{
		BF8915_CS_LOW_Reverse();//将片选CS设置为低电平
		spi_write_read(cmd,4,gpioData,(REG_LEN*total_ic));//使用SPI端口写入和读取设定数量的字节
		BF8915_CS_HIGH_Reverse();//将片选CS设置为高电平
	}
}

/*!
    \brief      Reads BF8915A stat registers(读取BF8915A状态寄存器)
		\param[in]  reg     : Determines which register is read back(确定回读哪个寄存器)
		\param[in]  total_ic: the Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  statData: Array of the unparsed codes(未解析代码的数组)
    \retval     none
*/
void BF8915A_rdstat_reg(u8 reg, u8 total_ic, u8 *statData)
{
	const u8 REG_LEN = 8;	//number of 6 bytes in the register + 2 bytes for the PEC(寄存器中的6字节数加上PEC(检错码)的2个字节)
	u8 cmd[4];				//初始化数组cmd[]
	u16 cmd_pec;			//计算出发送指令的PEC(检错码)
	cmd[0] = 0x00;			//发送指令的高8位
	cmd[1] = reg + 0x11;	//发送指令的低8位，详情在8915A.h文件中 状态寄存器RDSTAT
	cmd_pec = Bf8915pec15_calc(2, cmd);	//计算出发送指令的PEC(检错码)
	cmd[2] = (u8)(cmd_pec >> 8);		//发送指令的PEC(检错码)的高八位
	cmd[3] = (u8)(cmd_pec);				//发送指令的PEC(检错码)的低八位

	if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
	{
		BF8915_CS_LOW();//将片选CS设置为低电平
		spi_write_read(cmd,4,statData,(REG_LEN*total_ic));//使用SPI端口写入和读取设定数量的字节
		BF8915_CS_HIGH();//将片选CS设置为高电平
	}
	else
	{
		BF8915_CS_LOW_Reverse();//将片选CS设置为低电平
		spi_write_read(cmd,4,statData,(REG_LEN*total_ic));//使用SPI端口写入和读取设定数量的字节
		BF8915_CS_HIGH_Reverse();//将片选CS设置为高电平
	}
}

/*!
    \brief      Helper function that parses voltage measurement registers(解析电压测量寄存器的辅助函数)
		\param[in]  current_ic: Number of ics in current cycle(当前循环的IC(8915A)数)
		\param[in]  cell_reg  : Controls which cell voltage register is read back(控制回读哪个单体电压寄存器)
		\param[in]  cell_data : Number of bytes of data received(接收到数据的字节数，6个数据+2个PEC(检错码))
		\param[in]  cell_codes: cell voltage value(电池电压值)
		\param[in]  ic_pec    : If a PEC error was detected during most recent read cmd(如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误)
    \retval     pec_error
*/
u8 parse_cells(u8 current_ic, u8 cell_reg, u8 cell_data[], u16 *cell_codes, u8 *ic_pec)
{
	const u8 BYT_IN_REG = 6;	//寄存器组中的6个8位寄存器
	u8 CELL_IN_REG = 3;			//每个电压寄存器组中的 3个电压值
	u8 pec_error = 0;			//检查检错码
	u16 parsed_cell;			//单个电压值
	u16 received_pec;			//收到的每个寄存器组中的PEC(检错码)
	u16 data_pec;				//计算收到的每个寄存器组中指令数据的PEC(检错码)
	u8 data_counter = current_ic*NUM_RX_BYT; //data counter(数据计数器)
	u8 current_cell = 0;		//每个电压寄存器组当前循环到的单体电池电压
	
	for (current_cell = 0; current_cell < CELL_IN_REG; current_cell++)  // This loop parses the read back data into cell voltages, it loops once for each of the 3 cell voltage codes in the register
	{																																		//(这个循环将读回的数据解析成电压，对寄存器中的每3个电压码循环一次)
		parsed_cell = (u16)cell_data[data_counter] + ((u16)cell_data[data_counter + 1] << 8);//Each cell codes is received as two bytes and is combined to create the parsed cell voltage code
																		   //(每个电压代码被接收为两个字节，并且被组合以创建解析的单体电压代码)
		cell_codes[current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;//将解析出的单个电压码依次存放在数组cell_codes[]中
		data_counter = data_counter + 2;//Because cell voltage codes are two bytes the data counter must increment by two for each parsed cell code
																//(因为单体电压码是两个字节，所以对于每个解析的电池码，数据计数器必须增加2)
		if(cell_reg == CVFR)//#define CVFR	0x06U
		{
			current_cell = 0x3U;//存放电池电压的结构体数组有16个元素，此处代码为了防止数组解析CVFR(电池电压寄存器组F)时，数组溢出问题
		}
	}
	if(cell_reg == CVFR)//若寄存器为CVFR，则只需循环一次即可跳出循环
	{
		data_counter = data_counter + 4;
	}
	received_pec = (cell_data[data_counter] << 8) | cell_data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th after the 6 cell voltage data bytes
																																						 //(接收到的current_ic(当前IC(8915A)数量)的PEC(检错码)作为6个单体电压数据字节之后的第7个和第8个字节发送)
	data_pec = Bf8915pec15_calc(BYT_IN_REG, &cell_data[(current_ic) * NUM_RX_BYT]);//计算接收回来的寄存器组(6个8位寄存器)中的PEC(检错码)

	if (received_pec != data_pec)//检查检错码是否匹配
	{
		pec_error = 1;     //The pec_error variable is simply set 1 if any PEC errors(如果有任何pec错误，pec_error变量简单地设置为1)
		ic_pec[cell_reg-1]=1;//将cellVoltage[c_ic].pec_match[0]中的值置1
	}
	else
	{
		ic_pec[cell_reg-1]=0;//检错码正确，将cellVoltage[c_ic].pec_match[0]中的值置0
	}
	data_counter=data_counter+2;//Because the transmitted PEC code is 2 bytes long the data_counter must be incremented by 2 bytes to point to the next ICs state voltage data
														  //(因为传输的PEC(检错码)代码是2字节长，所以数据计数器必须增加2字节才能指向下一个8915A状态电压数据)
	return(pec_error);//回传检查检错码的值
}

/*!
    \brief      Reads and parses the BF8915A cell voltage registers(读取并解析BF8915A电池电压寄存器)
		\param[in]  reg                : Controls which cell voltage register is read back(控制回读哪个电池电压寄存器)
		\param[in]  total_ic           : the Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  CellVoltage_TypeDef: CellVoltage Struct(电池电压结构)
    \retval     the time of pec_error(pec_error的数)
*/
u8 BF8915A_rdcv(u8 reg, u8 total_ic, CellVoltage_TypeDef *cellVoltage)
{
	u8 pec_error = 0;		//检查检错码
	u8 *cell_data;			//初始化一个8位的指针cell_data
	u8 c_ic = 0;			//当前正在读入数组的8915A芯片序号
	u8 cell_reg = 1;		//电池电压寄存器计数器
	u8 current_ic = 0;		//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	cell_data = (u8 *) malloc((NUM_RX_BYT*total_ic)*sizeof(u8));//#define NUM_RX_BYT 0x8U(接收到数据的字节数，6个字节数据+2个PEC(检错码)字节)

	if (reg == 0)//如果选择读取所有电池电压
	{
		for (cell_reg = 1; cell_reg < NUM_CV_REG + 1; cell_reg++) //#define NUM_CV_REG 6U,对每个BF8915A电池电压寄存器执行一次
		{
			BF8915A_rdcv_reg(cell_reg, total_ic,cell_data );//读取原始电池电压寄存器数据
			for (current_ic = 0; current_ic < total_ic; current_ic++)//该循环作用为读取并解析每个BF8915A所选择的电池电压寄存器
			{
				if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
				{
					c_ic = current_ic;		//选择正序将电池电压放入数组
				}
				else
				{
					c_ic = total_ic - current_ic - 1;//选择倒序将电池电压放入数组
				}
				pec_error = pec_error + parse_cells(current_ic,cell_reg, cell_data,
												&cellVoltage[c_ic].cellVoltage[0],
												&cellVoltage[c_ic].pec_match[0]);//解析电压测量寄存器的辅助函数,并回传检查检错码的值
			}
		}
	}
	else//如果不选择读取所有电池电压
	{
		BF8915A_rdcv_reg(reg, total_ic,cell_data);//读取原始电池电压寄存器数据

		for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为读取并解析每个BF8915A所选择的电池电压寄存器
		{
			if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
			{
				c_ic = current_ic;		//选择正序将电池电压放入数组
			}
			else
			{
				c_ic = total_ic - current_ic - 1;//选择倒序将电池电压放入数组
			}
			pec_error = pec_error + parse_cells(current_ic,reg, cell_data,
                                          &cellVoltage[c_ic].cellVoltage[0],
                                          &cellVoltage[c_ic].pec_match[0]);//解析电压测量寄存器的辅助函数,并回传检查检错码的值
		}
	}
	free(cell_data);	//清空指针
	return(pec_error);	//回传检查检错码的值
}

/*!
    \brief      Reads and parses the BF8915A GPIO registers(读取并解析BF8915A GPIO寄存器)
		\param[in]  reg        : Determines which GPIO voltage register is read back(确定回读哪个GPIO电压寄存器)
		\param[in]  total_ic   : the Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  gpioVoltage: A two dimensional array of the gpio voltage codes(GPIO电压的二维数组)
    \retval     pec_error
*/
u8 BF8915A_rdgv(u8 reg, u8 total_ic, GPIOReg_TypeDef *gpioVoltage)
{
	u8 *dataPtr;		//初始化一个8位的指针dataPtr
	u8 pec_error = 0;	//检查检错码
	u8 c_ic =0;			//当前正在读入数组的8915A芯片序号
	u8 gpio_reg = 1;	//GPIO寄存器计数器
	u8 current_ic = 0;	//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	dataPtr = (u8 *) malloc((NUM_RX_BYT*total_ic)*sizeof(u8));//计算指针dataPtr需要接收的长度
																		//#define NUM_RX_BYT 0x8U(接收到数据的字节数，6个字节数据+2个PEC(检错码)字节)
	if (reg == 0)//如果选择读取所有GPIO电压寄存器
	{
		for (gpio_reg = 1; gpio_reg < NUM_GOIO_REG + 1; gpio_reg++) //#define NUM_STAT_REG	3U,对每个BF8915A的GPIO电压寄存器执行一次
		{
			BF8915A_rdgv_reg(gpio_reg, total_ic,dataPtr);//Reads the raw gpio register data into the data[] array(将原始GPIO寄存器数据读入data[]数组)
			for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为读取并解析每个BF8915A所选择的GPIO电压寄存器
			{
				if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
				{
					c_ic = current_ic;//选择正序将GPIO电压放入数组
				}
				else
				{
					c_ic = total_ic - current_ic - 1;//选择倒序将GPIO电压放入数组
				}
				pec_error = parse_cells(current_ic,gpio_reg, dataPtr,
                                &gpioVoltage[c_ic].gpioVoltage[0],
                                &gpioVoltage[c_ic].pec_match[0]);//解析电压测量寄存器的辅助函数,并回传检查检错码的值
			}
		}
	}
	else//如果不选择读取所有GPIO电压寄存器
	{
		BF8915A_rdgv_reg(reg, total_ic, dataPtr);//Reads the raw gpio register data into the data[] array(将原始GPIO寄存器数据读入data[]数组)

		for (current_ic = 0; current_ic<total_ic; current_ic++)//该循环作用为读取并解析每个BF8915A所选择的GPIO电压寄存器
		{
			if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
			{
				c_ic = current_ic;		//选择正序将GPIO电压放入数组
			}
			else
			{
				c_ic = total_ic - current_ic - 1;//选择倒序将GPIO电压放入数组
			}
			pec_error = parse_cells(current_ic,reg, dataPtr,
                              &gpioVoltage[c_ic].gpioVoltage[0],
                              &gpioVoltage[c_ic].pec_match[0]);//解析电压测量寄存器的辅助函数,并回传检查检错码的值
		}

	}
	free(dataPtr);		//清空指针
	return (pec_error);	//回传检查检错码的值
}

/*!
    \brief      Reads and parses the BF8915A stat registers(读取和解析BF8915A状态寄存器)
		\param[in]  reg             : Determines which Stat register is read back(确定回读哪个状态寄存器)
		\param[in]  total_ic        : the Number of ICs in the system(系统中IC(8915A)的数量)
		\param[in]  StateReg_TypeDef: A two dimensional array of the state codes(状态寄存器的二维数组)
    \retval     pec_error
*/
u8 BF8915A_rdstat(u8 reg, u8 total_ic, StateReg_TypeDef *stateReg)
{
	const u8 BYT_IN_REG = 6;		//寄存器组中的6个8位寄存器
	const u8 GPIO_IN_REG = 3;		//每个寄存器组中的 3个电压值

	u8 *dataPtr;					//初始化一个8位的指针dataPtr
	u8 data_counter = 0;			//data counter(数据计数器)
	u8 pec_error = 0;				//检查检错码
	u16 parsed_stat;				//单个状态值
	u16 received_pec;				//收到的每个寄存器组中的PEC(检错码)
	u16 data_pec;					//计算收到的每个寄存器组中指令数据的PEC(检错码)
	u8 c_ic = 0;					//当前正在读入数组的8915A芯片序号
	u8 stat_reg = 1;				//当前正在读取的状态寄存器组
	u8 current_ic = 0;				//current_ic is used as the IC counter(当前IC(8915A)用作IC(8915A)计数器)
	u8 current_gpio = 0;			//当前正在读入的电压值
	u8 stateD_byte = 0;				//状态寄存器的字节计数器
	u8 stateD_bit = 0;				//状态寄存器的位计数器
	dataPtr = (u8 *) malloc((NUM_RX_BYT*total_ic)*sizeof(u8));//计算指针dataPtr需要接收的长度

	if (reg == 0)//如果选择读取所有状态寄存器
	{
		for (stat_reg = 1; stat_reg < NUM_STAT_REG + 0x1U; stat_reg++)//#define NUM_STAT_REG	4U,对每个BF8915A的状态电压寄存器执行一次
		{
			data_counter = 0;//data counter(数据计数器)
			BF8915A_rdstat_reg(stat_reg, total_ic,dataPtr);//Reads the raw status register data into the dataPtr[] array(将原始状态寄存器数据读入dataPtr[]数组)

			for (current_ic = 0 ; current_ic < total_ic; current_ic++)//该循环作用为读取并解析每个BF8915A所选择的状态电压寄存器
			{
				if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
				{
					c_ic = current_ic;//选择正序将状态寄存器放入数组
				}
				else
				{
					c_ic = total_ic - current_ic - 1;//选择倒序将状态寄存器放入数组
				}
				if ((stat_reg > 0) && (stat_reg < 4))//如果读取的寄存器为STATA、STATB、STATC
				{
					for (current_gpio = 0; current_gpio < GPIO_IN_REG; current_gpio++) // This loop parses the read back data into state voltages, it loops once for each of the 3 state voltage codes in the register
					{                                                                  //(这个循环将读回的数据解析为状态电压，它对寄存器中的3个状态电压代码中的每一个循环一次)
						parsed_stat = dataPtr[data_counter] + (dataPtr[data_counter+1]<<8);//Each state codes is received as two bytes and is combined to create the parsed state voltage code
						                                                                   //(每个状态码被接收为两个字节，并且被组合来创建解析的状态电压代码)
						stateReg[c_ic].stat_codes[current_gpio + (stat_reg - 1) * GPIO_IN_REG] = parsed_stat;//将解析出的单个电压码依次存放在数组stateReg[c_ic].stat_codes[]中
						data_counter=data_counter+2;//Because cell voltage codes are two bytes the data counter must increment by two for each parsed cell code
																	    	//(因为单体电压码是两个字节，所以对于每个解析的电池码，数据计数器必须增加2)
						if((reg == STATC) &&(current_gpio == 0x1U))	//此处判断未了防止读STATC时,导致StateReg_TypeDef结构体中stat_codes[]数组溢出问题
						{
							current_gpio = GPIO_IN_REG;//寄存器为STATC且已经循环到第二次，跳过一次循环
							data_counter = data_counter+2;//Because cell voltage codes are two bytes the data counter must increment by two for each parsed cell code
																		        //(因为单体电压码是两个字节，所以对于每个解析的电池码，数据计数器必须增加2)
						}
					}
				}
				else if (stat_reg == 4)//如果读取的寄存器为STATD
				{
					stateReg[c_ic].uvOvFlags[0] = dataPtr[data_counter++];//STDR0的8位，Cell1~Cell4的欠/过压标志
					stateReg[c_ic].uvOvFlags[1] = dataPtr[data_counter++];//STDR1的8位，Cell5~Cell8的欠/过压标志
					stateReg[c_ic].uvOvFlags[2] = dataPtr[data_counter++];//STDR2的8位，Cell9~Cell12的欠/过压标志
					stateReg[c_ic].uvOvFlags[3] = dataPtr[data_counter++];//STDR3的8位，Cell12~Cell16的欠/过压标志
					stateReg[c_ic].cfg_err = ((dataPtr[data_counter] & 0x40) >> 0x6U);	//STDR4的第7位，配置字校验出错标志(包括奇偶异或校验和ECC校验)
					stateReg[c_ic].pec_err = ((dataPtr[data_counter] & 0x20) >> 0x5U);	//STDR4的第6位，指令数据的PEC校验出错标志
					stateReg[c_ic].extov = ((dataPtr[data_counter] & 0x10) >> 0x4U);	//STDR4的第5位，监测模式下，外部温度(gpio)过温标志
					stateReg[c_ic].itov = ((dataPtr[data_counter] & 0x08) >> 0x3U);		//STDR4的第4位，监测模式下，内部温度(vtemp)过温标志
					stateReg[c_ic].htov2 = ((dataPtr[data_counter] & 0x04) >> 0x2U);	//STDR4的第3位，硬件温度(95℃)过温标志
					stateReg[c_ic].htov1 = ((dataPtr[data_counter] & 0x02) >> 0x1U);	//STDR4的第2位，硬件温度(125℃)过温标志
					stateReg[c_ic].mux_fail = (dataPtr[data_counter++] & 0x01);			//STDR4的第1位，开关译码测试结果
					stateReg[c_ic].mode_state = ((dataPtr[data_counter] & 0xC0U) >> 0x6U);//STDR5的第8~7位，模式状态寄存器
					stateReg[c_ic].rst_state = (dataPtr[data_counter++] & 0x3FU);		//STDR5的第6~1位，复位标志寄存器
				
					stateReg[c_ic].cellOvFlag = 0x00U;	//电池欠压标志位，每个位用来标志每个电池的欠压标志位
					stateReg[c_ic].cellUvFlag = 0x00U;	//电池过压标志位，每个位用来标志每个电池的过压标志位
					
					for(stateD_byte = 0;stateD_byte < 4;stateD_byte ++)//状态寄存器的字节循环
					{
						for(stateD_bit = 0;stateD_bit < 4;stateD_bit ++)//状态寄存器的位循环
						{
							if((stateReg[c_ic].uvOvFlags[stateD_byte] & (0x01 << (stateD_bit * 0x2U))) == (0x01 << stateD_bit * 0x2U))//判断该Cellx的欠压标志位是否为1
								stateReg[c_ic].cellUvFlag |= 0x0001 << ((stateD_byte * 0x4U) + stateD_bit);//该Cellx的欠压标志位置1
							else
								stateReg[c_ic].cellUvFlag &= ~(0x0001 << ((stateD_byte * 0x4U) + stateD_bit));//该Cellx的欠压标志位置0
							
							if((stateReg[c_ic].uvOvFlags[stateD_byte] & (0x01 << (stateD_bit * 0x2U + 0x1U))) == (0x01 << (stateD_bit * 0x2U + 0x1U)))//判断该Cellx的过压标志位是否为1
								stateReg[c_ic].cellOvFlag |= 0x0001 << ((stateD_byte * 0x4U) + stateD_bit);//该Cellx的过压标志位置1
							else
								stateReg[c_ic].cellOvFlag &= ~(0x0001 << ((stateD_byte * 0x4U) + stateD_bit));//该Cellx的过压标志位置0
						}
					}					
				}
				received_pec = (dataPtr[data_counter]<<8)+ dataPtr[data_counter+1];//接收到的当前IC(8915A)的PEC(检错码)作为6个状态电压数据字节之后的第7个和第8个字节发送
				data_pec = Bf8915pec15_calc(BYT_IN_REG, &dataPtr[current_ic*NUM_RX_BYT]);//计算接收回来的寄存器组(6个8位寄存器)中的PEC(检错码)

				if (received_pec != data_pec)//检查检错码是否匹配
				{
					pec_error = 1; 			//如果有任何PEC(检错码)错误，pec_error变量简单地设置为1
					stateReg[c_ic].pec_match[stat_reg-1]=1;//stateReg[c_ic].pec_match[stat_reg-1]中的检错码值置1
				}
				else
				{
					stateReg[c_ic].pec_match[stat_reg-1]=0;//stateReg[c_ic].pec_match[stat_reg-1]中的检错码值置0
				}
				data_counter = data_counter + 2;//Because the transmitted PEC code is 2 bytes long the data_counter must be incremented by 2 bytes to point to the next ICs state voltage data
																		    //(因为传输的PEC(检错码)代码是2字节长，所以数据计数器必须增加2字节才能指向下一个集成电路状态电压数据)
			}
		}
	}
	else
	{
		BF8915A_rdstat_reg(reg, total_ic, dataPtr);//Reads the raw status register data into the dataPtr[] array(将原始状态寄存器数据读入dataPtr[]数组)
		for (current_ic = 0 ; current_ic < total_ic; current_ic++) //该循环作用为读取并解析每个BF8915A所选择的状态电压寄存器
		{
			if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
			{
				c_ic = current_ic;		//选择正序将电池电压放入数组
			}
			else
			{
				c_ic = total_ic - current_ic - 1;//选择倒序将电池电压放入数组
			}
			if ((reg > 0) && (reg <4))	//如果读取的寄存器为STATA、STATB、STATC
			{
				for (current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++) //This loop parses the read back data into state voltages, it loops once for each of the 3 state voltage codes in the register
				{																		                              //(这个循环将读回的数据解析为状态电压，它对寄存器中的3个状态电压码中的每一个循环一次)
          
					parsed_stat = dataPtr[data_counter] + (dataPtr[data_counter+1]<<8);//Each state codes is received as two bytes and is combined to create the parsed state voltage code
																					                                   //(每个状态码被接收为两个字节，并且被组合来创建解析的状态电压代码)
					stateReg[c_ic].stat_codes[current_gpio + (reg - 1) * GPIO_IN_REG] = parsed_stat;
					data_counter = data_counter+2;//Because state voltage codes are two bytes the data counter must increment by two for each parsed state voltage code
																			  //(因为状态电压码是两个字节，所以对于每个解析的状态电压码，数据计数器必须增加2)
					if((reg == STATC) &&(current_gpio == 0x1U))	//此处判断未了防止读STATC时,导致StateReg_TypeDef结构体中stat_codes[]数组溢出问题
					{
						current_gpio = GPIO_IN_REG;		//若寄存器为STATC且已经循环到第二次，则跳过一次循环
						data_counter = data_counter+2;	//Because cell voltage codes are two bytes the data counter must increment by two for each parsed cell code
																		      //(因为单体电压码是两个字节，所以对于每个解析的电池码，数据计数器必须增加2)
					}
				}
			}
			else if (reg == 4)//如果读取的寄存器为STATD
			{
				stateReg[c_ic].uvOvFlags[0] = dataPtr[data_counter++];//STDR0的8位，Cell1~Cell4的欠/过压标志
				stateReg[c_ic].uvOvFlags[1] = dataPtr[data_counter++];//STDR1的8位，Cell5~Cell8的欠/过压标志
				stateReg[c_ic].uvOvFlags[2] = dataPtr[data_counter++];//STDR2的8位，Cell9~Cell12的欠/过压标志
				stateReg[c_ic].uvOvFlags[3] = dataPtr[data_counter++];//STDR3的8位，Cell12~Cell16的欠/过压标志
				stateReg[c_ic].cfg_err = ((dataPtr[data_counter] & 0x40) >> 0x6U);	//STDR4的第7位，配置字校验出错标志(包括奇偶异或校验和ECC校验)
				stateReg[c_ic].pec_err = ((dataPtr[data_counter] & 0x20) >> 0x5U);	//STDR4的第6位，指令数据的PEC校验出错标志
				stateReg[c_ic].extov = ((dataPtr[data_counter] & 0x10) >> 0x4U);	//STDR4的第5位，监测模式下，外部温度(gpio)过温标志
				stateReg[c_ic].itov = ((dataPtr[data_counter] & 0x08) >> 0x3U);		//STDR4的第4位，监测模式下，内部温度(vtemp)过温标志
				stateReg[c_ic].htov2 = ((dataPtr[data_counter] & 0x04) >> 0x2U);	//STDR4的第3位，硬件温度(95℃)过温标志
				stateReg[c_ic].htov1 = ((dataPtr[data_counter] & 0x02) >> 0x1U);	//STDR4的第2位，硬件温度(125℃)过温标志
				stateReg[c_ic].mux_fail = (dataPtr[data_counter++] & 0x01);			//STDR4的第1位，开关译码测试结果
				stateReg[c_ic].mode_state = ((dataPtr[data_counter] & 0xC0U) >> 0x6U);//STDR5的第8~7位，模式状态寄存器
				stateReg[c_ic].rst_state = (dataPtr[data_counter++] & 0x3FU);		//STDR5的第6~1位，复位标志寄存器
				for(stateD_byte = 0;stateD_byte < 4;stateD_byte ++)//状态寄存器的字节循环
				{
					for(stateD_bit = 0;stateD_bit < 4;stateD_bit ++)//状态寄存器的位循环
					{
						if((stateReg[c_ic].uvOvFlags[stateD_byte] & (0x01 << (stateD_bit * 0x2U))) == (0x01 << stateD_bit * 0x2U))//判断该Cellx的欠压标志位是否为1
							stateReg[c_ic].cellUvFlag |= 0x0001 << ((stateD_byte * 0x4U) + stateD_bit);//该Cellx的欠压标志位置1
						else
							stateReg[c_ic].cellUvFlag &= ~(0x0001 << ((stateD_byte * 0x4U) + stateD_bit));//该Cellx的欠压标志位置0

						if((stateReg[c_ic].uvOvFlags[stateD_byte] & (0x01 << (stateD_bit * 0x2U + 0x1U))) == (0x01 << (stateD_bit * 0x2U + 0x1U)))//判断该Cellx的过压标志位是否为1
							stateReg[c_ic].cellOvFlag |= 0x0001 << ((stateD_byte * 0x4U) + stateD_bit);//该Cellx的过压标志位置1
						else
							stateReg[c_ic].cellOvFlag &= ~(0x0001 << ((stateD_byte * 0x4U) + stateD_bit));//该Cellx的过压标志位置0
					}
				}
			}
			received_pec = (dataPtr[data_counter]<<8)+ dataPtr[data_counter+1];//The received PEC for the current_ic is transmitted as the 7th and 8th after the 6 gpio voltage data bytes
																		                                     //(接收到的集成电路计数器的检错码作为6个GPIO电压数据字节之后的第7个和第8个字节发送)
			data_pec = Bf8915pec15_calc(BYT_IN_REG, &dataPtr[current_ic*NUM_RX_BYT]);//计算接收回来的寄存器组(6个8位寄存器)中的PEC(检错码)
			if (received_pec != data_pec)//检查检错码是否匹配
			{
				pec_error = 1;//如果有任何PEC(检错码)错误，pec_error变量简单地设置为1
				stateReg[c_ic].pec_match[reg-1]=1;//stateReg[c_ic].pec_match[stat_reg-1]中的检错码值置1
			}
			data_counter = data_counter + 2;//Because the transmitted PEC code is 2 bytes long the data_counter must be incremented by 2 bytes to point to the next ICs state voltage data
																		  //(因为传输的PEC(检错码)代码是2字节长，所以数据计数器必须增加2字节才能指向下一个集成电路状态电压数据)
		}
	}
	free(dataPtr);//清空指针
	return (pec_error);//回传检查检错码的值
}

/*!
    \brief      Clear Cell Register(清除电池电压寄存器)
		\param[in]  none
    \retval     none
*/
void BF8915A_clrcell(void)
{
	u8 cmd[2]= {0x00 , 0x81};
	BF8915A_cmd(cmd);//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      Clear GPIO Register(清除GPIO寄存器)
		\param[in]  none
    \retval     none
*/
void BF8915A_clrgp(void)
{
	u8 cmd[2]= {0x00 , 0x82};
	BF8915A_cmd(cmd);//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      Clear STATE Register(清除状态寄存器)
		\param[in]  none
    \retval     none
*/
void BF8915A_clrstat(void)
{
	u8 cmd[2]= {0x00 , 0x83};
	BF8915A_cmd(cmd);//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}

/*!
    \brief      PLADC CMD(轮询ADC转换状态指令)
		\param[in]  none
    \retval     none
*/
u8 BF8915A_pladc(void)
{
	u8 cmd[4];			//初始化数组cmd[]
	u8 adc_state = 0xFF;//初始化adc_state
	u16 cmd_pec;		//计算出发送指令的PEC(检错码)

	cmd[0] = 0x00;		//发送指令的高8位
	cmd[1] = 0x85;		//发送指令的低8位
	cmd_pec = Bf8915pec15_calc(2, cmd);//计算出发送指令的PEC(检错码)
	cmd[2] = (u8)(cmd_pec >> 8);//发送指令的PEC(检错码)的高八位
	cmd[3] = (u8)(cmd_pec);//发送指令的PEC(检错码)的低八位
	
	if (isospi_reverse == FALSE)//判断选择的菊花链通信方向是相对正向还是相对反向
    {
		BF8915_CS_LOW();		//将片选CS设置为低电平
		spi_write_array(4,cmd);	//将字节数组写入SPI端口
		adc_state = SPI1_ReadWriteByte(0xFF);//返回SPI接收到的数据
		if(adc_state == 0xff)
		{
			BF8915_CS_HIGH();	//将片选CS设置为高电平
		}
    }
    else
    {
		BF8915_CS_LOW_Reverse();//将片选CS设置为低电平
		spi_write_array(4,cmd);	//将字节数组写入SPI端口
		adc_state = SPI2_ReadWriteByte(0xFF);//返回SPI接收到的数据
	    if(adc_state == 0xff)
	    {
	    	BF8915_CS_HIGH_Reverse();//将片选CS设置为高电平
	    }
    }
	return(adc_state);//回传SPI接收到的数据
}

/*!
    \brief      soft reset(软件复位)
		\param[in]  none
    \retval     none
*/
void BF8915A_softrst(void)
{
	u8 cmd[2]= {0x00 , 0x55};
	BF8915A_cmd(cmd);//编写BF8915A命令的通用函数。函数为cmd数据计算了检错码
}
