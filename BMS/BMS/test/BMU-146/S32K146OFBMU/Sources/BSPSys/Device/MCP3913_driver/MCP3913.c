/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : MCP3913.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : MCP3913通讯驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPSPI.h"
//#include "Insulation.h"
#include "MCP3913.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
//extern u8 Mcp3913SampleCurr;                   /*分流器采样标志*/
/*控制命令配置*/
MCPControlTypedef MCP3913_Control = {0};

//AD偏移量(零飘)配置//{-30, -38, -30, -30, -33, -35};//{1300, 0, 0, 0, 0, 2950};//{395, 395, 395, 395, 395, 395};
const s16 cMCP3913OffsetAD[MCP_CHANNUM] = {-70, -100, -100, -100, -100, -100};

/*通过SPI读取到的SPI数据*/
static u8 sMcp3913SpiData[MCP_CHANNUM*2] = {0};

/*采样断线标志*/
static u8 Mcp3913SampleLosFlag = 0;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void Mcp3913SoftReset_Set(void)
** @Input     : void
** @Output    : void
** @Function  : 设置MCP3913软件复位
** @The notes : 带片选
**===========================================================================================*/
static void Mcp3913SoftReset_Set(void);

/*=============================================================================================
** @Name      : static void Mcp3913SoftReset_Clr(void)
** @Input     : void
** @Output    : void
** @Function  : 清除MCP3913软件复位
** @The notes : 带片选
**===========================================================================================*/
static void Mcp3913SoftReset_Clr(void);

/*=============================================================================================
** @Name      : static u8 Mcp3913SPIWriteCtrlByte(u8 cmd, u8 RW)
** @Input     : cmd: 控制命令 RW:读写方式:0只写 1只读
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 通过SPI向MCP3913写入控制命令
** @The notes : 不带片选
**===========================================================================================*/
static u8 Mcp3913SPIWriteCtrlByte(u8 cmd, u8 RW);

/*=============================================================================================
** @Name      : static u8 Mcp3913SPIWriteData(u32 data)
** @Input     : data:写入的数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 通过SPI向MCP3913写入数据
** @The notes : 不带片选,发送24位数据
**===========================================================================================*/
static u8 Mcp3913SPIWriteData(u32 data);

/*=============================================================================================
** @Name      : static u8 Mcp3913SPIReadData(u8 *data, u8 num)
** @Input     : *data:读取到的数据  num:读取数据的长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 通过SPI从MCP3913读取数据
** @The notes : 不带片选
**===========================================================================================*/
static u8 Mcp3913SPIReadData(u8 *data, u8 num);

/*=============================================================================================
** @Name      : static s16 CalcMcp3913OneChanAD(u8 channel, u8 *data)
** @Input     : channel:通道号 data:读取到该通道的SPI数据[至少2个长度]
** @Output    : MCP3913通道AD值
** @Function  : 计算MCP3913指定通道AD值
** @The notes : 2个SPI数据与通道AD偏移量整合成一个AD值
**===========================================================================================*/
static s16 CalcMcp3913OneChanAD(u8 channel, u8 *data);

/*=============================================================================================
** @Name      : static void Mcp3913SoftDelay(u32 time)
** @Input     : time:软件延时时间
** @Output    : void
** @Function  : 软件延时函数
** @The notes : 少用
**===========================================================================================*/
static void Mcp3913SoftDelay(u32 time);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void Mcp3913Init(void)
** @Input     : void
** @Output    : void
** @Function  : MCP3913采样芯片初始化
** @The notes :
**===========================================================================================*/
void Mcp3913Init(void)
{
    MCP3913_RESET_CLR;      /*ADC复位*/
    Mcp3913SoftDelay(600);  /*复位延时*/
    MCP3913_RESET_SET;
    
    //MCP3913_Control.Config0 = 0xc0380000 | (MCP_OSR256 << 13) | (MCP_BOOST2 << 18);//0000 0000 1111 1000 0010 0000 0000 0000	
    MCP3913_Control.Config0 = 0x00380000 | (MCP_OSR256 << 13) | (MCP_BOOST2 << 18);//0000 0000 1111 1000 0010 0000 0000 0000		
    MCP3913_Control.Gain = 0x00000000;
    MCP3913_Control.Status = 0x00b80000;        //00000000 10111000 00000000 00000000				
    MCP3913_Control.Config1 = 0x00000040;       //CLKEXT = 1,禁止外部晶振，改用MCU提供时钟	
    //MCP3913_Control.Config1 = 0x00000000;
    //MCP3913_Control.Phase1 = 0x00000064;
	
	//MCP3913_Control.GAINCAL_Ch0 = 0x00000000;
	//MCP3913_Control.OFFCAL_Ch0 = 0x00000000;
    
    Mcp3913SoftReset_Set();
    Mcp3913SoftDelay(10);
    
    MCP3913_CS_LOW;
    Mcp3913SPIWriteCtrlByte(MCP_GAIN, MCP_WRITE);
    Mcp3913SoftDelay(10);
    Mcp3913SPIWriteData(MCP3913_Control.Gain);
    MCP3913_CS_HIGH;
    Mcp3913SoftDelay(10);
    
    MCP3913_CS_LOW;
    Mcp3913SPIWriteCtrlByte(MCP_STATUSCOM, MCP_WRITE);
    Mcp3913SoftDelay(10);
    Mcp3913SPIWriteData(MCP3913_Control.Status);
    MCP3913_CS_HIGH;
    Mcp3913SoftDelay(10);
    
    MCP3913_CS_LOW;
    Mcp3913SPIWriteCtrlByte(MCP_CONFIG0, MCP_WRITE);
    Mcp3913SoftDelay(10);
    Mcp3913SPIWriteData(MCP3913_Control.Config0);
    MCP3913_CS_HIGH;
    Mcp3913SoftDelay(10);
    
    MCP3913_CS_LOW;
    Mcp3913SPIWriteCtrlByte(MCP_CONFIG1, MCP_WRITE);
    Mcp3913SoftDelay(10);
    Mcp3913SPIWriteData(MCP3913_Control.Config1);
    MCP3913_CS_HIGH;
    Mcp3913SoftDelay(10);
    
    #if USE_PHASE_DELAY == 1
        #if    MCP_MCLK == 4
            MCP3913_Control.Phase1 = 0x00000000;
        #elif MCP_MCLK == 6
            MCP3913_Control.Phase1 = 0x0000008e;        //-114,114的二进制为01110010，-114补码为10001110
        #elif MCP_MCLK == 8
            MCP3913_Control.Phase1 = 0x000000bd;        //-67,67的二进制为01000011，-67补码为10111101
        #elif MCP_MCLK == 12
            MCP3913_Control.Phase1 = 0x0000001B;
        #elif MCP_MCLK == 16
            //MCP3913_Control.Phase1 = 0x00043081;
            //MCP3913_Control.Phase0 = 0x00000091;
            MCP3913_Control.Phase1 = 0x00091091;
            MCP3913_Control.Phase0 = 0x00000091;
        #endif
        
    MCP3913_CS_LOW;
    Mcp3913SPIWriteCtrlByte(MCP_PHASE1, MCP_WRITE);
    Mcp3913SoftDelay(10);
    Mcp3913SPIWriteData(MCP3913_Control.Phase1);
    MCP3913_CS_HIGH;
    Mcp3913SoftDelay(10);
    
    MCP3913_CS_LOW;
    Mcp3913SPIWriteCtrlByte(MCP_PHASE0, MCP_WRITE);
    Mcp3913SoftDelay(10);
    Mcp3913SPIWriteData(MCP3913_Control.Phase0);
    MCP3913_CS_HIGH;
    Mcp3913SoftDelay(10);
    
//    MCP3913_CS_LOW;
//    Mcp3913SPIWriteCtrlByte(MCP_OFFCAL_CH0, MCP_WRITE);
//    Mcp3913SoftDelay(10);
//    Mcp3913SPIWriteData(MCP3913_Control.OFFCAL_Ch0);
//    MCP3913_CS_HIGH;
//    Mcp3913SoftDelay(10);
//    
//    MCP3913_CS_LOW;
//    Mcp3913SPIWriteCtrlByte(MCP_GAINCAL_CH0, MCP_WRITE);
//    Mcp3913SoftDelay(10);
//    Mcp3913SPIWriteData(MCP3913_Control.GAINCAL_Ch0);
//    MCP3913_CS_HIGH;
//    Mcp3913SoftDelay(10);
    #endif
    
    Mcp3913SoftReset_Clr();
}

/*=============================================================================================
** @Name      : void Mcp3913SampLosSetFlag(u8 flag)
** @Input     : flag: 0 或 1
** @Output    : void
** @Function  : 采样断线标志设置
** @The notes :
**===========================================================================================*/
void Mcp3913SampLosSetFlag(u8 flag)
{
	Mcp3913SampleLosFlag = flag;
}

/*=============================================================================================
** @Name      : u8 GetMcp3913SampLosFlag(void)
** @Input     : void
** @Output    : 采样断线标志
** @Function  : 获取采样断线标志
** @The notes :
**===========================================================================================*/
u8 GetMcp3913SampLosFlag(void)
{
	return(Mcp3913SampleLosFlag);
}

/*=============================================================================================
** @Name      : s16 GetMcp3913SampleAD(u8 Channel)
** @Input     : Channel:采样通道号[0,6]
** @Output    : 读取到的AD值
** @Function  : 获得MCP3913指定通道采样AD值
** @The notes :  MCU读取AD值一次性最多只能读取4个通道,一次读取一个(SPI通讯限制)
**===========================================================================================*/
s16 GetMcp3913SampleAD(u8 Channel)
{
    s16 adValue = 0;
    
    MCP3913_CS_LOW;
    
    /*发送读命令失败*/
    if(FALSE == Mcp3913SPIWriteCtrlByte(Channel, MCP_READ))
    {
        MCP3913_CS_HIGH;
        return(0xffff);
    }
    
    /*发送读命令成功则开始读取全部通道(6)数据*/
    //Mcp3913SoftDelay(50);
    Mcp3913SPIReadData(sMcp3913SpiData, MCP_CHANNUM*2);
    
    MCP3913_CS_HIGH;
    
    /*仅前2个数据有效为对应通道数据*/
    adValue = CalcMcp3913OneChanAD(Channel, sMcp3913SpiData);
    
    return(adValue);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void Mcp3913SoftReset_Set(void)
** @Input     : void
** @Output    : void
** @Function  : 设置MCP3913软件复位
** @The notes : 带片选
**===========================================================================================*/
static void Mcp3913SoftReset_Set(void)
{
    MCP3913_CS_LOW;
    MCP3913_Control.Config1 |= 0x003f0000;
    Mcp3913SPIWriteCtrlByte(MCP_CONFIG1, MCP_WRITE);
    Mcp3913SPIWriteData(MCP3913_Control.Config1);
    MCP3913_CS_HIGH;
}

/*=============================================================================================
** @Name      : static void Mcp3913SoftReset_Clr(void)
** @Input     : void
** @Output    : void
** @Function  : 清除MCP3913软件复位
** @The notes : 带片选
**===========================================================================================*/
static void Mcp3913SoftReset_Clr(void)
{
    MCP3913_CS_LOW;
    MCP3913_Control.Config1 &= 0x00c0ffff;
    Mcp3913SPIWriteCtrlByte(MCP_CONFIG1, MCP_WRITE);
    Mcp3913SPIWriteData(MCP3913_Control.Config1);
    MCP3913_CS_HIGH;
}

/*=============================================================================================
** @Name      : static u8 Mcp3913SPIWriteCtrlByte(u8 cmd, u8 RW)
** @Input     : cmd: 控制命令 RW:读写方式:0只写 1只读
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 通过SPI向MCP3913写入控制命令
** @The notes : 不带片选
**===========================================================================================*/
static u8 Mcp3913SPIWriteCtrlByte(u8 cmd, u8 RW)
{
    u8 temp = 0;
    
    /*整合命令*/
    temp = ((u8)MCP_DEVICE_ADDRESS << 6) | ((cmd & 0x1f) << 1) | (RW & 0x01);
    
    /*发送失败再发送一次*/
    if(FALSE == BSPSPISendData(eSPI1, temp))
    {
        /*短暂延时*/
    	Mcp3913SoftDelay(10);
        (void)BSPSPISendData(eSPI1, temp);
    }    
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : static u8 Mcp3913SPIWriteData(u32 data)
** @Input     : data:写入的数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 通过SPI向MCP3913写入数据
** @The notes : 不带片选,发送24位数据
**===========================================================================================*/
static u8 Mcp3913SPIWriteData(u32 data)
{
    u8 i = 0;
    u8 temp[3] = {0};
    
    temp[0] = (u8)(data >> 16);
    temp[1] = (u8)(data >> 8);
    temp[2] = (u8)(data);
    
    /*发送24位数据*/
    for(i = 0; i < 3; i++)
    {
        /*发送失败再发送一次*/
        if(FALSE == BSPSPISendData(eSPI1, temp[i]))
        {
            /*短暂延时*/
        	Mcp3913SoftDelay(10);
            (void)BSPSPISendData(eSPI1, temp[i]);
        }
        
        /*短暂延时*/
        Mcp3913SoftDelay(10);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : static u8 Mcp3913SPIReadData(u8 *data, u8 num)
** @Input     : *data:读取到的数据  num:读取数据的长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 通过SPI从MCP3913读取数据
** @The notes : 不带片选
**===========================================================================================*/
static u8 Mcp3913SPIReadData(u8 *data, u8 num)
{
    u8 i = 0;
    
    if((NULL == data) || (0 == num))
    {
		Mcp3913SampLosSetFlag(1);/*设置采样断线标志*/
        return(FALSE);
    }
    
    /*读取指定长度数据*/
	Mcp3913SampLosSetFlag(0);/*清除采样断线标志*/
    for(i = 0; i < num; i++)
    {
        /*发送空字节用于接收数据*/
        BSPSPISendData(eSPI1, 0xff);
        data[i] = BSPSPIGetRcvData(eSPI1);
        
        /*短暂延时*/
        Mcp3913SoftDelay(10);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : static s16 CalcMcp3913OneChanAD(u8 channel, u8 *data)
** @Input     : channel:通道号 data:读取到该通道的SPI数据[至少2个长度]
** @Output    : MCP3913通道AD值
** @Function  : 计算MCP3913指定通道AD值
** @The notes : 2个SPI数据与通道AD偏移量整合成一个AD值
**===========================================================================================*/
static s16 CalcMcp3913OneChanAD(u8 channel, u8 *data)
{
    s16 adValue = 0;
    
    if((channel < MCP_CHANNUM) && (NULL != data))
    {
    	/*
		if(1 == Mcp3913SampleCurr)
		{
			Mcp3913SampleCurr = 0;
			adValue = (s16)((s32)(((s16)data[0] << 8) | data[1]) + cMCP3913OffsetAD[channel]);
		}
		else
		{
			adValue = (s16)((s32)(((s16)data[0] << 8) | data[1]) * 13 / 10 + cMCP3913OffsetAD[channel]);
		}
		*/
    	adValue = (s16)((s32)(((s16)data[0] << 8) | data[1]) + cMCP3913OffsetAD[channel]);
    }
    
    return(adValue);
}

/*=============================================================================================
** @Name      : static void Mcp3913SoftDelay(u32 time)
** @Input     : time:软件延时时间
** @Output    : void
** @Function  : 软件延时函数
** @The notes : 少用
**===========================================================================================*/
static void Mcp3913SoftDelay(u32 time)
{
	if(0 == time)
	{
		return;
	}
	while(time--);
}
