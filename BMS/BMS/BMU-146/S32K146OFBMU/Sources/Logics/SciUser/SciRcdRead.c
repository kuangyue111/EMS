/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : SciInfoPoint.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 串口信息模块指针接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "CANUser.h"
#include "Authority.h"
#include "RecordInterface.h"
#include "RcdDisplayUser.h"
#include "SciCtrlHandle.h"
#include "SciInfoPoint.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sSciReadSofState = 0;                            /*读故障记录状态[0未就绪 1可读 2已读] (只读)*/
static u16 sSciReadSoeState = 0;                            /*读事件记录状态[0未就绪 1可读 2已读] (只读)*/
static u16 sSciReadSodState = 0;                            /*读数据记录状态[0未就绪 1可读 2已读] (只读)*/

extern u16 gCANRcdNum_30[];                                 /*历史记录条数*/
extern u16 gCANSofInfo_31[];                                /*故障记录详细信息*/
extern u16 gCANSoeInfo_32[];                                /*事件记录详细信息*/
extern u16 gCANSodNum_35[];                                 /*运行数据存储总状态信息*/
extern u16 gCANSodDay_36[];                                 /*运行数据存储日状态信息*/
extern u16 gCANSodInfo_37[];                                /*运行数据存储详细信息*/
extern u16 gCANSoxOper_39[];                                /*历史记录读写操作命令*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16* SciGetPtrReadSofStateRO(void)
** @Input     : void
** @Output    : 读故障记录读状态寄存器地址指针
** @Function  : 获取读故障记录读状态寄存器地址指针
** @The notes : 只读,串口查询记录状态
**===============================================================================================*/
u16* SciGetPtrReadSofStateRO(void)
{
    u16* temp = (void*)0;

    //未就绪时有故障记录则可读(已读时必须经过确认下一条转为就绪或未就绪)
    if((0 == sSciReadSofState) && (gCANRcdNum_30[0] > 0))
    {
    	sSciReadSofState = 1;
    }

    temp = &sSciReadSofState;

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSofAllNumRO(void)
** @Input     : void
** @Output    : 故障记录总数目寄存器地址指针
** @Function  : 获取读故障记录总数目寄存器地址指针
** @The notes : 只读
**===============================================================================================*/
u16* SciGetPtrSofAllNumRO(void)
{
    u16* temp = (void*)0;

    //更新显示值
    ErrEvntRcdUpdateStateAPI();

    temp = &gCANRcdNum_30[0];

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSofInfoRO(u8 index)
** @Input     : void
** @Output    : 故障记录详细信息寄存器地址指针
** @Function  : 获取读故障记录详细信息寄存器地址指针
** @The notes : 只读
**===============================================================================================*/
u16* SciGetPtrSofInfoRO(u8 index)
{
    u16* temp = (void*)0;

    if(index < CANINDEX31_LEN)
    {
        //请求最后一个事件信息则认为事件已读
        if(((CANINDEX31_LEN - 1) == index) && (1 == sSciReadSofState))
        {
        	sSciReadSofState = 2;
        }

        temp = &gCANSofInfo_31[index];
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrRdSofAffirmWR(u8 fncCode, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据[请求写有效]
** @Output    : 读故障记录切换确认寄存器地址指针
** @Function  : 获取读故障记录切换确认寄存器地址指针
** @The notes : 可读写
**===============================================================================================*/
u16* SciGetPtrRdSofAffirmWR(u8 fncCode, u8 sciNum, u16 data)
{
    u16* temp = (void*)0;

	//写确认命令则切换下一条事件
	 if(((6 == fncCode) || (16 == fncCode)) && (90 == data))
	{
		/*已获得串口运维权限*/
		if(eAuthL_Operater <= GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
		{
			 //读取一条新记录
			if(TRUE == ErrRcdUpdateReadInfoAPI())
			{
				sSciReadSofState = 1;
			}
			else
			{
				sSciReadSofState = 0;
			}

			temp = &gCANSoxOper_39[2];
		}
		/*无权限*/
		else
		{
			temp = SciGetPtrDefaultValueWR();
		}
	}
	//写清除记录命令
	else if(((6 == fncCode) || (16 == fncCode)) && ((data <= 30) || (data == 0xff)))
	{
		/*已获得串口高级权限*/
		if(eAuthL_Service <= GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
		{
			sSciReadSofState = 0;

			gCANSoxOper_39[2] = data;
			ErrRcdClrDateInfoAPI();

			temp = &gCANSoxOper_39[2];
		}
		/*无权限*/
		else
		{
			temp = SciGetPtrDefaultValueWR();
		}
	}
	//写错误命令
	else if((6 == fncCode) || (16 == fncCode))
	{
		temp = SciGetPtrDefaultValueWR();
	}
	//读命令
	else
	{
		temp = &gCANSoxOper_39[2];
	}

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrReadSoeStateRO(void)
** @Input     : void
** @Output    : 读事件记录读状态寄存器地址指针
** @Function  : 获取读事件记录读状态寄存器地址指针
** @The notes : 只读,串口查询记录状态
**===============================================================================================*/
u16* SciGetPtrReadSoeStateRO(void)
{
    u16* temp = (void*)0;

    //未就绪时有事件记录则可读(已读时必须经过确认下一条转为就绪或未就绪)
    if((0 == sSciReadSoeState) && (gCANRcdNum_30[1] > 0))
    {
    	sSciReadSoeState = 1;
    }

    temp = &sSciReadSoeState;

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSoeAllNumRO(void)
** @Input     : void
** @Output    : 事件记录总数目寄存器地址指针
** @Function  : 获取读事件记录总数目寄存器地址指针
** @The notes : 只读
**===============================================================================================*/
u16* SciGetPtrSoeAllNumRO(void)
{
    u16* temp = (void*)0;

    //更新显示值
    ErrEvntRcdUpdateStateAPI();

    temp = &gCANRcdNum_30[1];

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSoeInfoRO(u8 index)
** @Input     : void
** @Output    : 事件记录详细信息寄存器地址指针
** @Function  : 获取读事件记录详细信息寄存器地址指针
** @The notes : 只读
**===============================================================================================*/
u16* SciGetPtrSoeInfoRO(u8 index)
{
    u16* temp = (void*)0;

    if(index < CANINDEX32_LEN)
    {
        //请求最后一个事件信息则认为事件已读
        if(((CANINDEX32_LEN - 1) == index) && (1 == sSciReadSoeState))
        {
        	sSciReadSoeState = 2;
        }

        temp = &gCANSoeInfo_32[index];
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrRdSoeAffirmWR(u8 fncCode, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据[请求写有效]
** @Output    : 读事件记录切换确认寄存器地址指针
** @Function  : 获取读事件记录切换确认寄存器地址指针
** @The notes : 可读写
**===============================================================================================*/
u16* SciGetPtrRdSoeAffirmWR(u8 fncCode, u8 sciNum, u16 data)
{
    u16* temp = (void*)0;

	//写确认命令则切换下一条事件
	 if(((6 == fncCode) || (16 == fncCode)) && (90 == data))
	{
		/*已获得串口运维权限*/
		if(eAuthL_Operater <= GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
		{
			//读取一条新记录
			if(TRUE == EventRcdUpdateReadInfoAPI())
			{
				sSciReadSoeState = 1;
			}
			else
			{
				sSciReadSoeState = 0;
			}

			 temp = &gCANSoxOper_39[3];
		}
		/*无权限*/
		else
		{
			temp = SciGetPtrDefaultValueWR();
		}
	}
	//写清除记录命令
	else if(((6 == fncCode) || (16 == fncCode)) && ((data <= 30) || (data == 0xff)))
	{
		/*已获得串口运维权限*/
		if(eAuthL_Service <= GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
		{
			sSciReadSoeState = 0;

			gCANSoxOper_39[3] = data;
			EventRcdClrDateInfoAPI();

			 temp = &gCANSoxOper_39[3];
		}
		/*无权限*/
		else
		{
			temp = SciGetPtrDefaultValueWR();
		}
	}
	//写错误命令值
	else if((6 == fncCode) || (16 == fncCode))
	{
		temp = SciGetPtrDefaultValueWR();
	}
	//读命令
	else
	{
		temp = &gCANSoxOper_39[2];
	}

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrReadSoeStateRO(void)
** @Input     : void
** @Output    : 读数据记录读状态寄存器地址指针
** @Function  : 获取读数据记录读状态寄存器地址指针
** @The notes : 只读,串口查询记录状态
**===============================================================================================*/
u16* SciGetPtrReadSodStateRO(void)
{
    u16* temp = (void*)0;

    //未就绪时有数据记录则可读(已读时必须经过确认下一条转为就绪或未就绪)
    if((0 == sSciReadSoeState) && (gCANSodDay_36[1] > 0))
    {
    	sSciReadSodState = 1;
    }

    temp = &sSciReadSodState;

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrReadSodAllNumRO(u8 index)
** @Input     : index:状态信息索引[0,2]
** @Output    : 运行数据记录总数目寄存器地址指针
** @Function  : 获取运行数据记录总数目寄存器地址指针
** @The notes : 只读,串口查询记录天数和记录数目
**===============================================================================================*/
u16* SciGetPtrReadSodAllNumRO(u8 index)
{
    u16* temp = (void*)0;

    //运行数据记录天数和记录数目
    if(index < CANINDEX35_LEN)
    {
        //更新显示值
    	DataRcdUpdateRcdStateAPI();

        temp = &gCANSodNum_35[index];
    }
    else
    {
    	temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrReadSodDayNumRO(u8 index)
** @Input     : index:每天记录信息索引[0,31]
** @Output    : 每日运行数据记录数目寄存器地址指针
** @Function  : 获取每日运行数据记录数目寄存器地址指针
** @The notes : 只读,串口查询记录当前读取日期和每天记录数目
**===============================================================================================*/
u16* SciGetPtrReadSodDayNumRO(u8 index)
{
    u16* temp = (void*)0;

    //运行数据记录每天记录数目
    if(index < CANINDEX36_LEN)
    {
        //更新显示值
    	DataRcdUpdateRcdStateAPI();

        temp = &gCANSodDay_36[index];
    }
    else
    {
    	temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrReadSodInfoRO(u8 index)
** @Input     : index:记录详细信息索引[0,55]
** @Output    : 运行数据记录详细信息寄存器地址指针
** @Function  : 获取运行数据记录详细信息寄存器地址指针
** @The notes : 只读,串口查询一条记录详细信息
**===============================================================================================*/
u16* SciGetPtrReadSodInfoRO(u8 index)
{
    u16* temp = (void*)0;

    //运行数据记录每天记录数目
    if(index < CANINDEX37_LEN)
    {
        //请求最后一个数据信息则认为数据已读
        if(((CANINDEX37_LEN - 1) == index) && (1 == sSciReadSodState))
        {
        	sSciReadSodState = 2;
        }

        temp = &gCANSodInfo_37[index];
    }
    else
    {
    	temp = SciGetPtrDefaultValueRO();
    }

    return(temp);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrRdSodAffirmWR(u8 fncCode, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据[请求写有效]
** @Output    : 读数据记录切换确认寄存器地址指针
** @Function  : 获取读数据记录切换确认寄存器地址指针
** @The notes : 可读写
**===============================================================================================*/
u16* SciGetPtrRdSodAffirmWR(u8 fncCode, u8 sciNum, u16 data)
{
    u16* temp = (void*)0;

	//写确认命令则切换下一条数据记录
	if(((6 == fncCode) || (16 == fncCode)) && (90 == data))
	{
		/*已获得串口运维权限*/
		if(eAuthL_Operater <= GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
		{
			/*读一条记录到显示区*/
			if(TRUE == DataRcdUpdateDisplayAPI())
			{
				sSciReadSofState = 1;
			}
			else
			{
				sSciReadSofState = 0;
			}

			temp = &gCANSoxOper_39[0];
		}
		/*无权限*/
		else
		{
			temp = SciGetPtrDefaultValueWR();
		}
	}
	//写切换读记录日期命令
	else if(((6 == fncCode) || (16 == fncCode)) && (data <= 30))
	{
		/*已获得串口运维权限*/
		if(eAuthL_Operater <= GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
		{
			sSciReadSofState = 0;

			gCANSoxOper_39[0] = data;
			DataRcdSetReadDateAPI();

			temp = &gCANSoxOper_39[0];
		}
		/*无权限*/
		else
		{
			temp = SciGetPtrDefaultValueWR();
		}
	}
	//写清除指定日期记录命令
	else if(((6 == fncCode) || (16 == fncCode)) && (data >= 100) && (data <= 130))
	{
		/*已获得串口高级权限*/
		if(eAuthL_Service <= GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
		{
			sSciReadSofState = 0;

			gCANSoxOper_39[1] = data - 100;
			DataRcdClrDateInfoAPI();

			//temp = &gCANSoxOper_39[1];
			temp = SciGetPtrDefaultValueWR();
		}
		/*无权限*/
		else
		{
			temp = SciGetPtrDefaultValueWR();
		}
	}
	//写清除全部记录命令
	else if(((6 == fncCode) || (16 == fncCode)) && (data == 0xff))
	{
		/*已获得串口高级权限*/
		if(eAuthL_Service <= GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
		{
			sSciReadSofState = 0;

			gCANSoxOper_39[1] = data;
			DataRcdClrDateInfoAPI();

			 temp = &gCANSoxOper_39[1];
		}
		/*无权限*/
		else
		{
			temp = SciGetPtrDefaultValueWR();
		}
	}
	//写错误命令
	else if((6 == fncCode) || (16 == fncCode))
	{
		temp = SciGetPtrDefaultValueWR();
	}
	//读命令
	else
	{
		temp = &gCANSoxOper_39[0];
	}

    return(temp);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

