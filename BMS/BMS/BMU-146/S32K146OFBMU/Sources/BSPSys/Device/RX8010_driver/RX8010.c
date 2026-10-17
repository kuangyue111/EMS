/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : RX8010.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 外部RTC时钟驱动模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "stdbool.h"
#include "BSPIRQ.h"
#include "BSPI2C.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "BSPSimI2C.h"
#include "RX8010.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sExtRTCReady = 0;     /*外部RTC就绪状态 1就绪*/
t_SysClock RTCClock = {0};      /*RTC时钟*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=================================================================================================
** @Name      : u8 DEVRTCRX8010Init(void)
** @Input     : void
** @Output    : 初始化结果:TRUE:成功 FALSE:失败
** @Function  : 外部RTC芯片RX8010初始化
** @The notes : 用户调用
**===============================================================================================*/
u8 DEVRTCRX8010Init(void)
{
	u8 i = 0;
    u8 tmp[7] = {0};  // 此处需要连续写的最大数量为7（不含USER RAM）

    /*通讯初始化(模拟IIC初始化)*/
	//BSPSimIICInit(Chann_RTC);

    if(!BSPSimIICReadData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_FLAG_REG, 1 , tmp))
    {
    	return(FALSE);
    }

    // 只有VLF位(bit1)为1时才需要初始化，否则直接退出
    if((tmp[0] & 0x02) > 0)
    {
		tmp[0] = 0x58;    // RX8010_ADDR_RSV17必须初始化为0x58
		//tmp[0] = 0xD8;  // RX8010_ADDR_RSV17必须初始化为0x58
		if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_RSV17, 1 , tmp))
		{
			return(FALSE);
		}

		tmp[0] = 0x00;  // RX8010_ADDR_RSV30必须初始化为0x00
		if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_RSV30, 1 , tmp))
		{
			return(FALSE);
		}

		tmp[0] = 0x18;  // RX8010_ADDR_RSV31必须初始化为0x18
		//tmp[0] = 0x08;  // RX8010_ADDR_RSV31必须初始化为0x18
		if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_RSV31, 1 , tmp))
		{
			return(FALSE);
		}

		tmp[0] = 0x00;  // RX8010_ADDR_IRQ_CTRL中关闭IRQ输出以降低功耗
		if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_IRQ_CTRL, 1 , tmp))
		{
			return(FALSE);
		}

		tmp[0] = 0x04;  // RX8010_ADDR_EXT_REG中，关闭FOUT/固定周期定时等功能
		if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_EXT_REG, 1 , tmp))
		{
			return(FALSE);
		}

		tmp[0] = 0x00;  // RX8010_ADDR_EXT_REG中，关闭FOUT/固定周期定时等功能
		if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_FLAG_REG, 1 , tmp))
		{
			return(FALSE);
		}

		tmp[0] = 0x00;  // RX8010_ADDR_CTRL_REG中，禁止TIME UPDATE中断/闹钟中断/固定周期定时中断
		if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_CTRL_REG, 1 , tmp))
		{
			return(FALSE);
		}

	    // 初始化时钟
	    tmp[0] = 0x02;  // RX8010_ADDR_SECOND，MCU上电时初始化为0，通电后以当前时间初始化，额外增加2秒任务周期补偿，并防止秒针停止误判
	    tmp[1] = 0x0;  	// RX8010_ADDR_MINUTE，MCU上电时初始化为0，通电后以当前时间初始化
	    tmp[2] = 0x0;  	// RX8010_ADDR_HOUR，MCU上电时初始化为0，通电后以当前时间初始化
	    tmp[3] = 0x40;  // RX8010_ADDR_WEEK，2000年1月1日为周六，即0x40
	    tmp[4] = 0x01;  // RX8010_ADDR_DATE
	    tmp[5] = 0x01;  // RX8010_ADDR_MONTH
	    tmp[6] = 0x20;  // RX8010_ADDR_YEAR
	    for(i = 0; i < 7; i++)
	    {
	        if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, (RX8010_ADDR_SECOND + i), 1 , &tmp[i]))
	        {
	        	return(FALSE);
	        }
	    }

		// VLF作为关键标记位，必须最后清除
		tmp[0] = 0x00;  // RX8010_ADDR_FLAG_REG，取消TIME UPDATE输出/闹钟输出/固定周期定时中断/VLF标志
		if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, RX8010_ADDR_FLAG_REG, 1 , tmp))
		{
			return(FALSE);
		}
    }

    /*读取当前RTC时间*/
	for(i = 0; i < 7; i++)
	{
		tmp[i] = 0;
		if(!BSPSimIICReadData(Chann_RTC, RTC_DEV_ID, 1, (RX8010_ADDR_SECOND + i), 1 , &tmp[i]))
		{
			BSPSimIICStop(Chann_RTC);
			return(FALSE);
		}
	}

    /*当前RTC时间错误(年/月/日取值非法)*/
    if((tmp[6] <= 0) || (tmp[6] >= YEAR_VAL_MAX)
    	|| (tmp[5] <= 0) || (tmp[5] >= MONTH_VAL_MAX)
		|| (tmp[4] <= 0) || (tmp[4] >= DATE_VAL_MAX))
    {
        /*重写RTC时间*/
	    tmp[0] = 0x02;  // RX8010_ADDR_SECOND，MCU上电时初始化为0，通电后以当前时间初始化，额外增加2秒任务周期补偿，并防止秒针停止误判
	    tmp[1] = 0x0;  	// RX8010_ADDR_MINUTE，MCU上电时初始化为0，通电后以当前时间初始化
	    tmp[2] = 0x0;  	// RX8010_ADDR_HOUR，MCU上电时初始化为0，通电后以当前时间初始化
	    tmp[3] = 0x40;  // RX8010_ADDR_WEEK，2000年1月1日为周六，即0x40
	    tmp[4] = 0x01;  // RX8010_ADDR_DATE
	    tmp[5] = 0x01;  // RX8010_ADDR_MONTH
	    tmp[6] = 0x20;  // RX8010_ADDR_YEAR
	    for(i = 0; i < 7; i++)
	    {
	        if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, (RX8010_ADDR_SECOND + i), 1 , &tmp[i]))
	        {
	        	return(FALSE);
	        }
	    }
		return(FALSE);
	}

    /*RTC准备就绪*/
    sExtRTCReady = 1;

    return(TRUE);
}

/*=============================================================================================
** @Name      : void DEVEXTRTCCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 外部RTC检测任务
** @The notes : 4s
**===========================================================================================*/
void DEVEXTRTCCheckTask(void)
{
	static u8 sTime = 0;

    //外部RTC未就绪
    if((0 == sExtRTCReady) && (BSPGetOSMillSecTimeAPI() >= 10000))
    {
    	//只重复50次
    	if(sTime < 50)
    	{
			//重新初始化
    		DEVRTCRX8010Init();
    		sTime++;
    	}
    }
    else
    {
    	sTime = 0;
    }
}

/*=================================================================================================
** @Name      : void DEVRTCResetRX8010API(void)
** @Input     : void
** @Output    : void
** @Function  : 外部RTC芯片RX8010复位
** @The notes : RX8010总线锁死后复位总线
**===============================================================================================*/
void DEVRTCResetRX8010API(void)
{
	BSPSimIICStart(Chann_RTC);
	/*发送虚拟数据*/
	BSPSimIICSendByte(Chann_RTC, 0xFF);
    BSPSimIICStart(Chann_RTC);
    BSPSimIICStop(Chann_RTC);
}

/*=================================================================================================
** @Name      : u8 DEVEXTRTCGetNowClockAPI(t_SysClock *rtcClock)
** @Input     : *rtcClock:读取的RTC时钟信息
** @Output    : 读结果:TRUE:成功 FALSE:失败
** @Function  : 从外部RTC芯片RX8010获取当前时钟
** @The notes : 当读取的数据非法时,不更新*rtcClock的数据
**===============================================================================================*/
u8 DEVEXTRTCGetNowClockAPI(t_SysClock *rtcClock)
{
	u8 i = 0;
	u8 Buffer[7] = {0};

	if(1 != sExtRTCReady)
	{
		return(FALSE);
	}

	BSP_DISABLE_IRQ();
	for(i = 0; i < 7; i++)
	{
		if(!BSPSimIICReadData(Chann_RTC, RTC_DEV_ID, 1, (RX8010_ADDR_SECOND + i), 1 , &Buffer[i]))
		{
			BSPSimIICStop(Chann_RTC);
			return(FALSE);
		}
	}
	BSP_ENABLE_IRQ();

	rtcClock->year  = ((Buffer[6] >> 4) * 10) + (Buffer[6] % 16) + BASI_YEAR_VALUE;
	rtcClock->month = ((Buffer[5] >> 4) * 10) + (Buffer[5] % 16);
	rtcClock->day   = ((Buffer[4] >> 4) * 10) + (Buffer[4] % 16);
	rtcClock->hour  = ((Buffer[2] >> 4) * 10) + (Buffer[2] % 16);
	rtcClock->minute = (((Buffer[1] >> 4) * 10) + (Buffer[1] % 16));
	rtcClock->second = (((Buffer[0] >> 4) * 10) + (Buffer[0] % 16));
	rtcClock->msecond = 0;

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 DEVEXTRTCSetNowClockAPI(t_SysClock *aimClock)
** @Input     : *aimClock:设置的目标时钟
** @Output    : 设置结果:TRUE:成功 FALSE:失败
** @Function  : 设置外部RTC芯片RX8010的当前时钟
** @The notes :
**===============================================================================================*/
u8 DEVEXTRTCSetNowClockAPI(t_SysClock *aimClock)
{
	u8 i = 0;
    u8 temp1 = 0;
    u8 temp2 = 0;
	u8 gTimeRef[7] = {0};

	if(1 != sExtRTCReady)
	{
		return(FALSE);
	}

	gTimeRef[0] = aimClock->second;
	gTimeRef[1] = aimClock->minute;
	gTimeRef[2] = aimClock->hour;
	gTimeRef[4] = aimClock->day;
	gTimeRef[5] = aimClock->month;
	gTimeRef[6] = aimClock->year - BASI_YEAR_VALUE;

    /*实时时钟时钟寄存器按8421BCD码存放*/
    for(i = 0; i < 7; i++)
    {
        temp1 = (*(gTimeRef + i)) % 10;
        temp2 = (*(gTimeRef + i)) / 10;

        *(gTimeRef + i) = (temp2 << 4) + temp1;
    }

    BSP_DISABLE_IRQ();

    for(i = 0; i < 7; i++)
    {
        if(!BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, (RX8010_ADDR_SECOND + i), 1 , &gTimeRef[i]))
        {
            //如有必要再此增加等待超时处理
        	SimIICWait(50);
        	(void)BSPSimIICWriteData(Chann_RTC, RTC_DEV_ID, 1, (RX8010_ADDR_SECOND + i), 1 , &gTimeRef[i]);
        }
    }

    BSP_ENABLE_IRQ();

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTRTCGetReadyFlagAPI(void)
** @Input     : void
** @Output    : 0:未就绪 1:就绪
** @Function  : 获取外部RTC就绪状态标志
** @The notes : 与外部RTC芯片通讯状态
**===========================================================================================*/
u8 DEVEXTRTCGetReadyFlagAPI(void)
{
    return(sExtRTCReady);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
