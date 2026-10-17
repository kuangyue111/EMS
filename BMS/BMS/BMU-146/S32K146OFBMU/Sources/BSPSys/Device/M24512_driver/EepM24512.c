/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : EepM24512.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 外部EEPROM驱动模块
** @Instructions : 采样模拟IIC通讯,高位先发
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPSimI2C.h"
#include "BSPIRQ.h"
#include "DEVGPIO.h"
#include "BSPUserAPI.h"
#include "EepM24512.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sExtEepromReady = 0;          //外部EEPROM就绪状态 1就绪

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void EXTEep24M512Init(void)
** @Input     : void
** @Output    : void
** @Function  : 外部EEPROM-M24512初始化
** @The notes :
**===========================================================================================*/
void EXTEep24M512Init(void)
{
	u32 flag = 0;

	/*使能24LC512写保护*/
	/*高电平禁止写操作*/
	DEVGPIOSetPortState(ePOut_EepWPr, ePSTATE_ON);

	/*通讯初始化(模拟IIC初始化)*/
	//BSPSimIICInit(Chann_EEP);

	/*读取用户校验标志是否正常*/
	if(TRUE == DEVEXTEEPReadLong(Eep_Ver_Addr, &flag, 1))
	{
		/*校验标志正确*/
		if(Eep_Ver_Flag == flag)
		{
			/*外部EEPROM有效*/
			sExtEepromReady = 1;
		}
		/*校验标志错误*/
		else
		{
			/*重写标志*/
			flag = Eep_Ver_Flag;
			DEVEXTEEPWriteLong(Eep_Ver_Addr, &flag, 1);

			/*外部EEPROM无效*/
			sExtEepromReady = 0;
		}
	}
	else
	{
		/*外部EEPROM无效*/
		sExtEepromReady = 0;
	}
}

/*=============================================================================================
** @Name      : void DEVEXTEEPCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 外部EEPROM检测任务
** @The notes : 4s
**===========================================================================================*/
void DEVEXTEEPCheckTask(void)
{
	static u8 sTime = 0;

    //外部EEPROM未就绪
    if((0 == sExtEepromReady) && (BSPGetOSMillSecTimeAPI() >= 10000))
    {
    	//只重复50次
    	if(sTime < 50)
    	{
			//重新初始化
			EXTEep24M512Init();
    		sTime++;
    	}
    }
    else
    {
    	sTime = 0;
    }
}

/*=============================================================================================
** @Name      : u8 DEVEXTEEPWriteByte(u16 addr, u8 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要写入的数据  length:要写入数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 向外部EEPROM指定地址写入length个字节
** @The notes : 尽量不要跨页写(一页128Byte)
**===========================================================================================*/
u8 DEVEXTEEPWriteByte(u16 addr, u8 *buffer, u16 length)
{
	/*输入参数无效*/
	if((0 == length) || (((u32)addr + length) > Eep_Max_Addr))
	{
		return(FALSE);
	}

	/*低电平启用写操作*/
	DEVGPIOSetPortState(ePOut_EepWPr, ePSTATE_OFF);

	//BSPSimIICWriteData(Chann_EEP, M24512_Devic_Addr, 2, addr, length , buffer);

	/*禁止中断*/
    BSP_DISABLE_IRQ();

	/*使能写命令写入地址和数据(高位先发)*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
	SimIICWait(5);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
	SimIICWait(5);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr));
	SimIICWait(5);

	/*连续写入指定长度数据*/
	while(length > 0)
	{
		/*写入数据*/
		BSPSimIICSendByte(Chann_EEP, *buffer++);
		SimIICWait(10);
		addr++;
		length--;

		/*连续数据跨页写要重新写地址*/
		if((length > 0) && ((addr % Eep_Page_Size) == 0))
		{
			/*等待上一页写入完成*/
			BSPSimIICStop(Chann_EEP);
			SimIICWait(40000);

			/*重新写本页起始地址*/
			BSPSimIICStart(Chann_EEP);
			BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
			SimIICWait(10);
			BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
			SimIICWait(10);
			BSPSimIICSendByte(Chann_EEP, (u8)(addr));
			SimIICWait(10);
		}
	}
	BSPSimIICStop(Chann_EEP);

	/*使能中断*/
    BSP_ENABLE_IRQ();

    /*等待写入完成*/
	SimIICWait(10);

    /*高电平禁止写操作*/
	DEVGPIOSetPortState(ePOut_EepWPr, ePSTATE_ON);

	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTEEPWriteWord(u16 addr, u16 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要写入的数据  length:要写入数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 向外部EEPROM指定地址写入length个单字
** @The notes : 低字节在前(小端模式),尽量不要跨页写(一页128Byte)
**===========================================================================================*/
u8 DEVEXTEEPWriteWord(u16 addr, u16 *buffer, u16 length)
{
	u8 data = 0;
	u16 size = 0;

	/*输入参数无效*/
	if((0 == length) || (((u32)addr + (length * 2)) > Eep_Max_Addr))
	{
		return(FALSE);
	}

	/*低电平启用写操作*/
	DEVGPIOSetPortState(ePOut_EepWPr, ePSTATE_OFF);

	/*禁止中断*/
    BSP_DISABLE_IRQ();

	/*使能写命令写入地址和数据(高位先发)*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr));
	SimIICWait(10);

	/*U16长度转为U8长度*/
	size = length * 2;

	/*连续写入指定长度数据*/
	while(size != 0)
	{
		/*先写低位,小端模式(低位在前)*/
		if(0 == (size % 2))
		{
			data = *buffer & 0xff;
		}
		else
		{
			data = (*buffer >> 8) & 0xff;
			buffer++;
		}
		BSPSimIICSendByte(Chann_EEP, data);
		SimIICWait(10);
		addr++;
		size--;

		/*连续数据跨页写要重新写地址*/
		if((length > 0) && ((addr % Eep_Page_Size) == 0))
		{
			/*等待上一页写入完成*/
			BSPSimIICStop(Chann_EEP);
			SimIICWait(40000);

			/*重新写本页起始地址*/
			BSPSimIICStart(Chann_EEP);
			BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
			SimIICWait(10);
			BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
			SimIICWait(10);
			BSPSimIICSendByte(Chann_EEP, (u8)(addr));
			SimIICWait(10);
		}
	}
	BSPSimIICStop(Chann_EEP);

	/*使能中断*/
    BSP_ENABLE_IRQ();
    //等待写入完成
    SimIICWait(10);

    /*高电平禁止写操作*/
	DEVGPIOSetPortState(ePOut_EepWPr, ePSTATE_ON);

	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTEEPWriteLong(u16 addr, u32 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要写入的数据  length:要写入数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 向外部EEPROM指定地址写入length个双字
** @The notes : 低字节在前(小端模式),尽量不要跨页写(一页128Byte)
**===========================================================================================*/
u8 DEVEXTEEPWriteLong(u16 addr, u32 *buffer, u16 length)
{
	u8 data = 0;
	u16 size = 0;

	/*输入参数无效*/
	if((0 == length) || (((u32)addr + (length * 4)) > Eep_Max_Addr))
	{
		return(FALSE);
	}

	/*低电平启用写操作*/
	DEVGPIOSetPortState(ePOut_EepWPr, ePSTATE_OFF);

	/*禁止中断*/
	BSP_DISABLE_IRQ();

	/*使能写命令写入地址和数据(高位先发)*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr));
	SimIICWait(10);

	/*U32长度转为U8长度*/
	size = length * 4;

	/*连续写入指定长度数据*/
	while(size != 0)
	{
		/*先写低位,小端模式(低位在前)*/
		if(0 == (size % 4))
		{
			data = *buffer & 0xff;
		}
		else if(3 == (size % 4))
		{
			data = (*buffer >> 8) & 0xff;
		}
		else if(2 == (size % 4))
		{
			data = (*buffer >> 16) & 0xff;
		}
		else
		{
			data = (*buffer >> 24) & 0xff;
			buffer++;
		}
		BSPSimIICSendByte(Chann_EEP, data);
		SimIICWait(10);
		addr++;
		size--;

		/*连续数据跨页写要重新写地址*/
		if((length > 0) && ((addr % Eep_Page_Size) == 0))
		{
			/*等待上一页写入完成*/
			BSPSimIICStop(Chann_EEP);
			SimIICWait(400000);

			/*重新写本页起始地址*/
			BSPSimIICStart(Chann_EEP);
			BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
			SimIICWait(10);
			BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
			SimIICWait(10);
			BSPSimIICSendByte(Chann_EEP, (u8)(addr));
			SimIICWait(10);
		}
	}
	BSPSimIICStop(Chann_EEP);

	/*使能中断*/
    BSP_ENABLE_IRQ();
	//等待写入完成
    SimIICWait(10);

    /*高电平禁止写操作*/
	DEVGPIOSetPortState(ePOut_EepWPr, ePSTATE_ON);

	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTEEPReadByte(u16 addr, u8 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要读取的数据  length:读取数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 从外部EEPROM指定地址读出length个字节
** @The notes :
**===========================================================================================*/
u8 DEVEXTEEPReadByte(u16 addr, u8 *buffer, u16 length)
{
	/*输入参数无效*/
	if((0 == length) || (((u32)addr + length) > Eep_Max_Addr))
	{
		return(FALSE);
	}

	//BSPSimIICReadData(Chann_EEP, M24512_Devic_Addr, 2, addr, length , buffer);

	/*禁止中断*/
    BSP_DISABLE_IRQ();

	/*使能写命令写入地址(高位先发)*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr));
	SimIICWait(10);

	/*使能读命令读取数据*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Read);
	SimIICWait(10);
	while(--length)
	{
		*buffer++ = BSPSimIICRecvByte(ACK, Chann_EEP);
	}
	*buffer = BSPSimIICRecvByte(NO_ACK, Chann_EEP);
	BSPSimIICStop(Chann_EEP);

	/*使能中断*/
    BSP_ENABLE_IRQ();

	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTEEPReadWord(u16 addr, u16 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要读取的数据  length:读取数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 从外部EEPROM指定地址读出length个单字
** @The notes : 低字节在前(小端模式)
**===========================================================================================*/
u8 DEVEXTEEPReadWord(u16 addr, u16 *buffer, u16 length)
{
	u8 data = 0;
	u16 size = 0;

	/*输入参数无效*/
	if((0 == length) || (((u32)addr + (length * 2)) > Eep_Max_Addr))
	{
		return(FALSE);
	}

	/*禁止中断*/
    BSP_DISABLE_IRQ();

	/*使能写命令写入地址(高位先发)*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr));
	SimIICWait(10);

	/*使能读命令读取数据*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Read);
	SimIICWait(10);

	size = length * 2;

	while(--size)
	{
		/*先读低位,小端模式(低位在前)*/
		if(1 == (size % 2))
		{
			data = BSPSimIICRecvByte(ACK, Chann_EEP);
		}
		else
		{
			*buffer = ((u16)BSPSimIICRecvByte(ACK, Chann_EEP) << 8) | data;
			buffer++;
		}
	}
	*buffer = ((u16)BSPSimIICRecvByte(NO_ACK, Chann_EEP) << 8) | data;
	BSPSimIICStop(Chann_EEP);

	/*使能中断*/
    BSP_ENABLE_IRQ();

	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTEEPReadLong(u16 addr, u32 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要读取的数据  length:读取数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 从外部EEPROM指定地址读出length个双字
** @The notes : 低字节在前(小端模式)
**===========================================================================================*/
u8 DEVEXTEEPReadLong(u16 addr, u32 *buffer, u16 length)
{
	u8 data[3] = {0};
	u16 size = 0;

	/*输入参数无效*/
	if((0 == length) || (((u32)addr + (length * 4)) > Eep_Max_Addr))
	{
		return(FALSE);
	}

	/*禁止中断*/
    BSP_DISABLE_IRQ();

	/*使能写命令写入地址(高位先发)*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr >> 8));
	SimIICWait(10);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr));
	SimIICWait(10);

	/*使能读命令读取数据*/
	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Read);
	SimIICWait(10);

	size = length * 4;

	while(--size)
	{
		/*先读低位,小端模式(低位在前)*/
		if(3 == (size % 4))
		{
			data[0] = BSPSimIICRecvByte(ACK, Chann_EEP);
		}
		/*再读次低位*/
		else if(2 == (size % 4))
		{
			data[1] = BSPSimIICRecvByte(ACK, Chann_EEP);
		}
		/*再读次高位*/
		else if(1 == (size % 4))
		{
			data[2] = BSPSimIICRecvByte(ACK, Chann_EEP);
		}
		else
		{
			*buffer = ((u32)BSPSimIICRecvByte(ACK, Chann_EEP) << 24) | ((u32)data[2] << 16) | ((u32)data[1] << 8) | data[0];
			buffer++;
		}
	}
	*buffer = ((u32)BSPSimIICRecvByte(NO_ACK, Chann_EEP) << 24) | ((u32)data[2] << 16) | ((u32)data[1] << 8) | data[0];
	BSPSimIICStop(Chann_EEP);

	/*使能中断*/
    BSP_ENABLE_IRQ();

	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTEEPGetReadyFlag(void)
** @Input     : void
** @Output    : 0:未就绪 1:就绪
** @Function  : 获取外部EEPROm就绪状态标志
** @The notes : 与外部EEPROM芯片通讯状态
**===========================================================================================*/
u8 DEVEXTEEPGetReadyFlag(void)
{
    return(sExtEepromReady);
}

/*=============================================================================================
** @Name      : void Eep24LC512_ClearNumPage(u8 pageNum)
** @Input     : pageNum:第几页
** @Output    : void
** @Function  : 指定清除哪一页的数据
** @The notes : pageNum必须是128的整数倍
**===========================================================================================*/
void Eep24LC512_ClearNumPage(u8 pageNum)
{
	u16 addr = 0;
	u8 j = 0;

	if(0 == pageNum)
	{
		return;
	}
	addr = pageNum * Eep_Page_Size;

	BSPSimIICStart(Chann_EEP);
	BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
	SimIICWait(1);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr>>8));
	SimIICWait(1);
	BSPSimIICSendByte(Chann_EEP, (u8)(addr));
	SimIICWait(1);
	for(j = 0; j < 128; j++)
	{
		BSPSimIICSendByte(Chann_EEP, 0x00);
		SimIICWait(1);
	}
	BSPSimIICStop(Chann_EEP);
}

/*=============================================================================================
** @Name      : void Eep2424LC512_Clear(void)
** @Input     : void
** @Output    : void
** @Function  : 清除24LC512里所有数据
** @The notes :
**===========================================================================================*/
void Eep24LC512_Clear(void)
{
	u16 temp = (Eep_Max_Addr + 1) / Eep_Page_Size;
	u16 addr= 0;
	u16 i = 0;
	u16 j =0;

	for(i = 0; i < temp; i++)
	{
		BSPSimIICStart(Chann_EEP);
		BSPSimIICSendByte(Chann_EEP, LC512_Cmd_Write);
		SimIICWait(1);
		BSPSimIICSendByte(Chann_EEP, (u8)(addr>>8));
		SimIICWait(1);
		BSPSimIICSendByte(Chann_EEP, (u8)(addr));
		SimIICWait(1);
		for(j = 0; j < 128; j++)
		{
			BSPSimIICSendByte(Chann_EEP, 0x00);
			SimIICWait(1);
		}
		BSPSimIICStop(Chann_EEP);

		//等待写入完成
		SimIICWait(5);
		addr += Eep_Page_Size;
	}
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
