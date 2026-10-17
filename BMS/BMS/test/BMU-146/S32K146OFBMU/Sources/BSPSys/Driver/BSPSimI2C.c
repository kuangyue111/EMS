/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPSimI2c.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 模拟IIC驱动模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPIRQ.h"
#include "BSPGPIO.h"
#include "DEVGPIO.h"
#include "BSPSimI2c.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
u8 gIICBusy = 0;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void GetByteCRC(u8 data, u8 InitVal)
** @Input     : Data:需要计算CRC的数据, InitVal:初始值
** @Output    : 读取的字节
** @Function  : 计算单字节数据的CRC
** @The notes :
**===========================================================================================*/
static u8 GetByteCRC(u8 Data, u8 InitVal);

/*=============================================================================================
** @Name      : static u8 GetBlockCRC(u8* Data, u8 Length, u8 InitVal)
** @Input     : Data:需要计算CRC的数据， Length:数据串长度, InitVal:初始值
** @Output    : Crc计算出的CRC
** @Function  : 计算块数据的CRC
** @The notes :
**===========================================================================================*/
/*static u8 GetBlockCRC(u8* Data, u8 Length, u8 InitVal);*/

/*=============================================================================================
** @Name      : static void IIC_SCL_DIR(u8 Direct, e_SimI2cChann Channel)
** @Input     : Direct:初始化SCL端口方向， Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static void IIC_SCL_DIR(u8 Direct, e_SimI2cChann Channel);

/*=============================================================================================
** @Name      : static void IIC_SDA_DIR(u8 Direct, e_SimI2cChann Channel)
** @Input     : Direct:初始化SDA端口方向， Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static void IIC_SDA_DIR(u8 Direct, e_SimI2cChann Channel);

/*=============================================================================================
** @Name      : static void IIC_SCL_DAT(u8 Level, e_SimI2cChann Channel)
** @Input     : Level:初始化SCL端口电平值， Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static void IIC_SCL_DAT(u8 Level, e_SimI2cChann Channel);

/*=============================================================================================
** @Name      : static void IIC_SDA_DAT(u8 Level, e_SimI2cChann Channel)
** @Input     : Level:初始化SDA端口电平值， Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static void IIC_SDA_DAT(u8 Level, e_SimI2cChann Channel);

/*=============================================================================================
** @Name      : static u8 IIC_SDA_STA(e_SimI2cChann Channel)
** @Input     :  Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static u8 IIC_SDA_STA(e_SimI2cChann Channel);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPSimIICInit(e_SimI2cChann Channel)
** @Input     : Channel选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC通信初始化
** @The notes :
**===========================================================================================*/
void BSPSimIICInit(e_SimI2cChann Channel)
{
//    gIICBusy = 0;
//    IIC_SDA_DIR(OUTPUT, Channel);  	            /*设置数据信号线为输出*/
//    IIC_SCL_DAT((e_IOState)HIGH, Channel);      /*设置输出前，要保持高电平*/
//    IIC_SCL_DIR(OUTPUT, Channel);  	            /*设置时钟信号线为输出*/
}

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel:IIC通道号[1:RTC 2:EEPROM], DevAddr设备地址, RegAddr寄存器地址, *Data需要写入的数据, Length写入数据的长度
** @Output    : IIC写数据成功标志0不成功1成功
** @Function  : 模拟IIC写一块带CRC校验的数据
** @The notes :
**===========================================================================================*/
u8 BSPSimIICWriteBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
{
    u8 i = 0;
    u8 CrcCal = 0;

    if(Channel > eSimIIC_NUM)
    {
        return(FALSE);
	}

    /*共用IIC不能按位表示*/
    //if(0 == (gIICBusy & (1 << Channel)))
    if(0 == gIICBusy)
    {
        BSPSimIICStart(Channel);

        /*设备地址左移一位,末位补0表示写*/
        (void)BSPSimIICSendByte(Channel, (DevAddr << 1));
        SimIICWait(10);

        (void)BSPSimIICSendByte(Channel, RegAddr);
        SimIICWait(10);

        //*发送写第一个数据,前端无应答则失败*/
        if(NO_ACK == BSPSimIICSendByte(Channel, *Data))
	    {
            BSPSimIICStop(Channel);
            return(FALSE);
        }

        CrcCal = GetByteCRC((DevAddr << 1), 0);
        CrcCal = GetByteCRC(RegAddr, CrcCal);
        CrcCal = GetByteCRC(Data[0], CrcCal);

        /*写入校验码,前端无应答则校验失败*/
        if(NO_ACK == BSPSimIICSendByte(Channel, CrcCal))
	    {
            BSPSimIICStop(Channel);
            return(FALSE);
        }

        /*发送写剩余数据*/
        for(i = 1; i < Length; i++)
        {
            /*前端无应答则失败*/
            if(NO_ACK == BSPSimIICSendByte(Channel, *(Data + i)))
			{
                BSPSimIICStop(Channel);
                return(FALSE);
			}

            CrcCal = GetByteCRC(*(Data + i), 0);

            /*写入校验码,前端无应答则校验失败*/
            if(NO_ACK == BSPSimIICSendByte(Channel, CrcCal))
            {
                BSPSimIICStop(Channel);
                return(FALSE);
			}

            SimIICWait(10);
        }

        BSPSimIICStop(Channel);
        return(TRUE);
	}

	return(FALSE);
}

/*=============================================================================================
** @Name      : u8 BSPSimIICReadBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel:IIC通道号[1:RTC 2:EEPROM], DevAddr是设备地址, RegAddr是寄存器地址, *data是读出的数据, Length是读的数据长度
** @Output    : IIC读数据成功标志0不成功1成功
** @Function  : 模拟IIC读一块带CRC校验的数据
** @The notes :
**===========================================================================================*/
u8 BSPSimIICReadBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
{
    u8 i = 0;
    u8 CrcCal = 0;
    u8 Buffer[IIC_DATA_LEN*2] = {0};

    if((Channel > eSimIIC_NUM) || (Length > IIC_DATA_LEN))
    {
        return(FALSE);
	}

    /*共用IIC不能按位表示*/
    //if(0 == (gIICBusy & (1 << Channel)))
    if(0 == gIICBusy)
    {
        BSPSimIICStart(Channel);

        /*设备地址左移一位,末位补0表示写*/
        (void)BSPSimIICSendByte(Channel, (DevAddr << 1));
        SimIICWait(10);
        (void)BSPSimIICSendByte(Channel, RegAddr);
        SimIICWait(10);

        BSPSimIICStart(Channel);

        /*设备地址左移一位,末位补1表示读*/
        (void)BSPSimIICSendByte(Channel, ((DevAddr << 1) | 1));

        /*除了最后一个数据不应答外其余都要应答*/
        for (i = 0; i < Length * 2 - 1; i++)
        {
            *(Buffer + i) = BSPSimIICRecvByte(ACK, Channel);
        }

        *(Buffer +  Length * 2 - 1) = BSPSimIICRecvByte(NO_ACK, Channel);

        BSPSimIICStop(Channel);

        CrcCal = GetByteCRC(((DevAddr << 1) | 1), 0);
        CrcCal = GetByteCRC(Buffer[0], CrcCal);

        if(CrcCal != *(Buffer + 1))
        {
            return(FALSE);
		}

        for(i = 1; i < Length; i++)
        {
            CrcCal = GetByteCRC(*(Buffer + i * 2), 0);
            if(CrcCal != *(Buffer + 2 * i + 1))
            {
                return(FALSE);
			}
		}

        for(i = 0; i < Length; i++)
        {
            *(Data + i) = *(Buffer + 2 * i);
		}

        return(TRUE);
    }

    return(FALSE);
}

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8* Data, u8 Length)
** @Input     : Channel是IIC通道号[1:RTC 2:EEPROM], DevAddr设备地址, RegAddr寄存器地址, *Data需要写入的数据, Length写入数据的长度
** @Output    : IIC写数据成功标志0不成功1成功
** @Function  : 模拟IIC写无CRC校验的数据
** @The notes :
**===========================================================================================*/
u8 BSPSimIICWriteBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8* Data, u8 Length)
{
    u8 i = 0;

    if(Channel > eSimIIC_NUM)
    {
        return(FALSE);
	}

    /*共用IIC不能按位表示*/
    //if(0 == (gIICBusy & (1 << Channel)))
    if(0 == gIICBusy)
    {
        BSPSimIICStart(Channel);

        /*设备地址左移一位,末位补0表示写*/
        if(NO_ACK == BSPSimIICSendByte(Channel, (DevAddr << 1)))
        {
            BSPSimIICStop(Channel);
            return(FALSE);
		}
        SimIICWait(10);

        /*前端无应答则失败*/
        if(NO_ACK == BSPSimIICSendByte(Channel, RegAddr))
        {
            BSPSimIICStop(Channel);
            return(FALSE);
		}

        SimIICWait(10);

        /*发送写全部数据*/
        for(i = 0; i < Length; i++)
        {
            /*前端无应答则失败*/
            if(NO_ACK == BSPSimIICSendByte(Channel, *(Data + i)))
			{
                BSPSimIICStop(Channel);
                return(FALSE);
			}

            SimIICWait(10);
        }

        BSPSimIICStop(Channel);
        return(TRUE);
	}

	return(FALSE);
}

/*=============================================================================================
** @Name      : u8 BSPSimIICReadBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel是IIC通道号[1:RTC 2:EEPROM], DevAddr是设备地址, RegAddr是寄存器地址, *data是读出的数据 dataLen是读的数据长度
** @Output    : IIC读数据成功标志0不成功1成功
** @Function  : 模拟IIC读一块无CRC校验的数据
** @The notes :
**===========================================================================================*/
u8 BSPSimIICReadBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
{
    u8 i = 0;

    if((Channel > eSimIIC_NUM) || (Length > IIC_DATA_LEN))
    {
        return(FALSE);
	}

    /*共用IIC不能按位表示*/
    //if(0 == (gIICBusy & (1 << Channel)))
    if(0 == gIICBusy)
    {
        BSPSimIICStart(Channel);

        /*设备地址左移一位,末位补0表示写*/
        if(NO_ACK == BSPSimIICSendByte(Channel, (DevAddr << 1)))
        {
            BSPSimIICStop(Channel);
            return(FALSE);
		}
        SimIICWait(10);

        /*前端无应答则失败*/
        if(NO_ACK == BSPSimIICSendByte(Channel, RegAddr))
        {
            BSPSimIICStop(Channel);
            return(FALSE);
		}
        SimIICWait(10);

        BSPSimIICStart(Channel);
        SimIICWait(10);

        /*设备地址左移一位,末位补1表示读*/
        if(NO_ACK == BSPSimIICSendByte(Channel, ((DevAddr << 1) | 1)))
        {
            BSPSimIICStop(Channel);
            return(FALSE);
		}

        /*除了最后一个数据不应答外其余都要应答*/
        for (i = 0; i < Length - 1; i++)
        {
            *(Data + i) = BSPSimIICRecvByte(ACK, Channel);
        }

        *(Data + Length - 1) = BSPSimIICRecvByte(NO_ACK, Channel);

        BSPSimIICStop(Channel);

        return(TRUE);
    }

    return(FALSE);
}

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data)
** @Input     : Chan:选择模拟IIC通道[1:RTC 2:EEPROM] devID是设备地址 regLen是寄存器长度 regAddr是寄存器地址 dataLen是写的数据长度  *data是写的数据
** @Output    : 写数据结果:FALSE失败 TRUE:成功
** @Function  : 使用模拟IIC向指定寄存器和地址写入数据
** @The notes : RTC、EEPROM模拟IIC写数据
**===========================================================================================*/
u8 BSPSimIICWriteData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data)
{
    u8 i = 0;

    /*共用IIC不能按位表示*/
    //if(0 == (gIICBusy & (1 << chan)))
    if(0 == gIICBusy)
    {
        BSPSimIICStart(chan);
        BSPSimIICSendByte(chan, devID << 1);  //设备ID左移1位，末位加0：写操作
        SimIICWait(30);

        if(regLen == 2)
        {
            BSPSimIICSendByte(chan, (u8)(regAddr >> 8));
            SimIICWait(30);
            BSPSimIICSendByte(chan, (u8)(regAddr & 0x00ff));
        }
        else
        {
            BSPSimIICSendByte(chan, (u8)(regAddr & 0x00ff));
        }

        SimIICWait(30);

        for(i = 0; i < dataLen; i++)
        {
            BSPSimIICSendByte(chan, data[i]);
            SimIICWait(30);
        }

        BSPSimIICStop(chan);

        return(TRUE);
    }

    return(FALSE);
}

/*=============================================================================================
** @Name      : u8 BSPSimIICReadData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data)
** @Input     : Chan:选择模拟IIC通道[1:RTC 2:EEPROM] devID是设备地址 regLen是寄存器长度 regAddr是寄存器地址 dataLen是读的数据长度  *data是读出的数据
** @Output    : 读数据结果:FALSE失败 TRUE:成功
** @Function  : 模拟IIC向指定寄存器和地址读取数据
** @The notes : RTC、EEPROM模拟IIC读数据
**===========================================================================================*/
u8 BSPSimIICReadData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data)
{
    u8 i = 0;

    /*共用IIC不能按位表示*/
    //if(0 == (gIICBusy & (1 << chan)))
    if(0 == gIICBusy)
    {
        BSPSimIICStart(chan);
        BSPSimIICSendByte(chan, devID << 1);  //设备ID左移1位，末位加0：写操作
        if(regLen == 2)
        {
            BSPSimIICSendByte(chan, (u8)(regAddr >> 8));
            SimIICWait(20);
            BSPSimIICSendByte(chan, (u8)(regAddr & 0x00ff));
        }
        else
        {
            BSPSimIICSendByte(chan, (u8)(regAddr & 0x00ff));
        }

        BSPSimIICStart(chan);
        BSPSimIICSendByte(chan, (devID << 1) | 1);

        for (i = 0; i < dataLen - 1; i++)    //除了最后一个数据不应答外其余都要应答
        {
            *(data + i) = BSPSimIICRecvByte(ACK, chan);
            SimIICWait(30);
        }
        *(data + dataLen - 1) = BSPSimIICRecvByte(NO_ACK, chan);

        BSPSimIICStop(chan);

        return(TRUE);
    }

    return(FALSE);
}

/*=============================================================================================
** @Name      : void BSPSimIICStart(u8 Channel)
** @Input     : Channel选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC启动
** @The notes :
**===========================================================================================*/
void BSPSimIICStart(u8 Channel)
{
    if(Channel >= eSimIIC_NUM)
    {
        return;
    }

    gIICBusy |= 1 << Channel;

    IIC_SDA_DIR(OUTPUT, Channel);               /*设置数据信号线为输出*/
    IIC_SCL_DIR(OUTPUT, Channel);               /*设置时钟信号线为输出*/

    IIC_SDA_DAT((e_IOState)HIGH, Channel);      /*发送起始条件的数据信号*/
    IIC_SCL_DAT((e_IOState)HIGH, Channel);

    SimIICWait(30);
    IIC_SDA_DAT((e_IOState)LOW, Channel); 	    /*发送起始信号*/
    SimIICWait(30);                              /*起始条件建立时间大于4.7us,延时,时钟线保持高，数据线从高到低一次跳变，IIC开始通信*/

    IIC_SCL_DAT((e_IOState)LOW, Channel);       /*钳住I2C总线，准备发送或接收数据*/
}

/*=============================================================================================
** @Name      : void BSPSimIICStop(u8 Channel)
** @Input     : Channel选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC停止
** @The notes :
**===========================================================================================*/
void BSPSimIICStop(u8 Channel)
{
    if(Channel >= eSimIIC_NUM)
    {
        return;
    }

    IIC_SDA_DIR(OUTPUT, Channel);               /*设置数据信号线为输出*/
    IIC_SCL_DIR(OUTPUT, Channel);               /*设置时钟信号线为输出*/

    IIC_SDA_DAT((e_IOState)LOW, Channel);       /*发送结束条件的数据信号*/
    IIC_SCL_DAT((e_IOState)HIGH, Channel);

    SimIICWait(30);                             /*起始条件建立时间大于4.7us,延时,时钟线保持高，数据线线从低到高一次跳变，IIC停止通信*/
    IIC_SDA_DAT((e_IOState)HIGH, Channel);      /*发送I2C总线结束信号*/

    gIICBusy &= ~(1 << Channel);
}

/*=============================================================================================
** @Name      : u8 BSPSimIICSendByte(u8 Channel, u8 Data)
** @Input     : Channel:选择模拟IIC通道[1:RTC 2:EEPROM] Data:发送字节
** @Output    : Ack从器件应答值
** @Function  : 模拟IIC向设备发送一个字节(高位先发)
** @The notes : 注意在中断函数里处理的函数不能再次打开全局中断
**===========================================================================================*/
u8 BSPSimIICSendByte(u8 Channel, u8 Data)
{
    u8 Count = 8;
    u8 Ack = 0;

    //在此禁止中断
    //BSP_DISABLE_IRQ();

    IIC_SCL_DIR(OUTPUT, Channel);
    IIC_SDA_DIR(OUTPUT, Channel);
    IIC_SCL_DAT((e_IOState)LOW, Channel);       /*上升沿发送数据过去*/

	/*发送8bit位*/
    while(Count--)
    {
    	/*高位先发*/
        if(Data & 0x80)
        {
            IIC_SDA_DAT((e_IOState)HIGH, Channel);
        }
        else
        {
            IIC_SDA_DAT((e_IOState)LOW, Channel);
        }

        /*上升沿发送数据过去*/
        IIC_SCL_DAT((e_IOState)HIGH, Channel);
        /*放此处可增加延时时间*/
        Data <<= 1;
        SimIICWait(30);
        IIC_SCL_DAT((e_IOState)LOW, Channel);
        SimIICWait(30);
    }

    /*模拟主器件释放SDA以便从器件发送ACK信号*/
    IIC_SDA_DAT((e_IOState)HIGH, Channel);
    IIC_SDA_DIR(INPUT, Channel);
    SimIICWait(30);
    IIC_SCL_DAT((e_IOState)HIGH, Channel);
    SimIICWait(30);
    IIC_SCL_DAT((e_IOState)LOW, Channel);
    //SimIICWait(30);
    Ack = IIC_SDA_STA(Channel);
    SimIICWait(30);

    /*使能中断*/
    //BSP_ENABLE_IRQ();

    return(Ack);
}

/*=============================================================================================
** @Name      : u8 BSPSimIICRecvByte(u8 Ack, u8 Channel)
** @Input     : Ack:是否向从机应答，Channel:选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : 读取到的字节
** @Function  : 模拟IIC向设备读取一个字节(先读高位)
** @The notes : 注意在中断函数里处理的函数不能再次打开全局中断
**===========================================================================================*/
u8 BSPSimIICRecvByte(u8 Ack, u8 Channel)
{
    u8 Count = 8;
    u8 Data = 0;

    IIC_SCL_DIR(OUTPUT, Channel);
    IIC_SDA_DIR(INPUT, Channel);

    /*禁止中断*/
    //BSP_DISABLE_IRQ();

    /*读8bit*/
    while(Count--)
    {
        /*从最高位开始读*/
        Data <<= 1;
        if(IIC_SDA_STA(Channel))
        {
            Data |= 1;
        }

        /*读完1bit设置时钟输入下一bit*/
        IIC_SCL_DAT((e_IOState)HIGH, Channel);
        SimIICWait(30);
        IIC_SCL_DAT((e_IOState)LOW, Channel);
        SimIICWait(30);
    }

    IIC_SDA_DIR(OUTPUT, Channel);

    if(0 == Ack)
    {
        IIC_SDA_DAT((e_IOState)LOW, Channel);
    }
    else
    {
        IIC_SDA_DAT((e_IOState)HIGH, Channel);
    }

    SimIICWait(30);
    IIC_SCL_DAT((e_IOState)HIGH, Channel);
    SimIICWait(30);
    IIC_SCL_DAT((e_IOState)LOW, Channel);
    SimIICWait(30);

    /*使能中断*/
    //BSP_ENABLE_IRQ();

    return(Data);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void GetByteCRC(u8 data, u8 InitVal)
** @Input     : Data:需要计算CRC的数据, InitVal:初始值
** @Output    : 读取的字节
** @Function  : 计算单字节数据的CRC
** @The notes :
**===========================================================================================*/
static u8 GetByteCRC(u8 Data, u8 InitVal)
{
    u8 i = 0;
    u8 Crc = 0;

    Crc = InitVal;
    Crc ^= Data;

    for(i = 0; i < 8; i++)
    {
        if(Crc & 0x80)
        {
            Crc <<= 1;
            Crc ^= 0x07;
        }
        else
        {
            Crc <<= 1;
        }
    }

    return Crc;
}

/*=============================================================================================
** @Name      : static u8 GetBlockCRC(u8* Data, u8 Length, u8 InitVal)
** @Input     : Data:需要计算CRC的数据， Length:数据串长度, InitVal:初始值
** @Output    : Crc计算出的CRC
** @Function  : 计算块数据的CRC
** @The notes :
**===========================================================================================*/
/*static u8 GetBlockCRC(u8* Data, u8 Length, u8 InitVal)
{
    u8 i, j = 0;
    u8 Crc = 0;

    Crc = InitVal;

    for(j = 0; j < Length; j++)
    {
        Crc ^= *(Data + j);

        for(i = 0; i < 8; i++)
        {
            if(Crc & 0x80)
            {
                Crc <<= 1;
                Crc ^= 0x07;
            }
            else
            {
                Crc <<= 1;
            }
        }
    }

    return Crc;
}
*/

/*=============================================================================================
** @Name      : static void IIC_SCL_DIR(u8 Direct, e_SimI2cChann Channel)
** @Input     : Direct:初始化SCL端口方向， Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static void IIC_SCL_DIR(u8 Direct, e_SimI2cChann Channel)
{
	if(1 == Channel)
	{
		(void)BSPGPIOInit(ePOR_A, ePIN_3, ePORT_MUX_AS_GPIO, Direct);
	}
	else if(2 == Channel)
	{
		(void)BSPGPIOInit(ePOR_A, ePIN_3, ePORT_MUX_AS_GPIO, Direct);
	}
}

/*=============================================================================================
** @Name      : static void IIC_SDA_DIR(u8 Direct, e_SimI2cChann Channel)
** @Input     : Direct:初始化SDA端口方向， Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static void IIC_SDA_DIR(u8 Direct, e_SimI2cChann Channel)
{
	if(1 == Channel)
	{
		(void)BSPGPIOInit(ePOR_A, ePIN_2, ePORT_MUX_AS_GPIO, Direct);
	}
	else if(2 == Channel)
	{
		(void)BSPGPIOInit(ePOR_A, ePIN_2, ePORT_MUX_AS_GPIO, Direct);
	}
}

/*=============================================================================================
** @Name      : static void IIC_SCL_DAT(u8 Level, e_SimI2cChann Channel)
** @Input     : Level:初始化SCL端口电平值， Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static void IIC_SCL_DAT(u8 Level, e_SimI2cChann Channel)
{
	if(1 == Channel)
	{
		(void)BSPGPIOSetPortState(ePOR_A, ePIN_3, Level);
	}
	else if(2 == Channel)
	{
		(void)BSPGPIOSetPortState(ePOR_A, ePIN_3, Level);
	}
}

/*=============================================================================================
** @Name      : static void IIC_SDA_DAT(u8 Level, e_SimI2cChann Channel)
** @Input     : Level:初始化SDA端口电平值， Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static void IIC_SDA_DAT(u8 Level, e_SimI2cChann Channel)
{
	if(1 == Channel)
	{
		(void)BSPGPIOSetPortState(ePOR_A, ePIN_2, Level);
	}
	else if(2 == Channel)
	{
		(void)BSPGPIOSetPortState(ePOR_A, ePIN_2, Level);
	}
}

/*=============================================================================================
** @Name      : static u8 IIC_SDA_STA(e_SimI2cChann Channel)
** @Input     :  Channel:从机通道
** @Output    :
** @Function  :
** @The notes :
**===========================================================================================*/
static u8 IIC_SDA_STA(e_SimI2cChann Channel)
{
	u8 state = 0;

	if(1 == Channel)
	{
		state = BSPGPIOGetInPortState(ePOR_A, ePIN_2);
	}
	else
	{
		state = BSPGPIOGetInPortState(ePOR_A, ePIN_2);
	}

	return(state);
}
