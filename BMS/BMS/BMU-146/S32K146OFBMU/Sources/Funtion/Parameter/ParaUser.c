/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaUser.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块用户接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "GroupPara.h"
#include "ParaWrite.h"
#include "PileInfo.h"
#include "EventUser.h"
#include "EnerInterface.h"
#include "CANUser.h"
#include "ParaUser.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u8 gCANSerialNum_2[];			                    /*设备条码序列号参数设置*/
extern u16 gPGroupState_57[]; 						        /*电池组状态信息数组*/
extern u16 gSysGenParaRO_100[];			                    /*系统通用参数设置*/
extern u16 gSysHigParaRO_101[];			                    /*系统硬件参数设置*/
extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];

extern u16 gSysGenParaRW_105[];			                    /*系统通用参数设置*/
extern u16 gSysHigParaRW_106[];			                    /*系统硬件参数设置*/
extern u16 gGBmuGenParaWR_107[];							/*主控通用参数值*/
extern u16 gGBmuHigLevParaWR_108[];							/*主控高级参数值*/
extern u8 gGHardParaWR_109[];								/*主控硬件参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ParaGroupSelfAddrCfgAPI(u16 addr, u8 port)
** @Input     : addr:设置目标设备地址[50,65] port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 设备序列号参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 ParaGroupSelfAddrCfgAPI(u16 addr, u8 port)
{
	/*写入成功*/
	if(TRUE == ParaWriteStoreGroupGenParaInfo(eBmuGenPara102_BmuDev, &addr, 1))
	{
		/*产生事件*/
		EventRcdDisplayUserAPI(eEVN_RCDSORT_CFG, 2, eBmuGenPara102_BmuDev, gGBmuGenPara_102[eBmuGenPara102_BmuDev], addr, port);

		gGBmuGenPara_102[eBmuGenPara102_BmuDev] = addr;

		return(TRUE);
	}

	return(FALSE);
}

/*=================================================================================================
** @Name      : u8 RcvDeviceSerialParaCfgAPI(u8 index, u8 port)
** @Input     : index:参数字段索引号 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 设备序列号参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 RcvDeviceSerialParaCfgAPI(u8 index, u8 port)
{
    u8 para = 0;

    /*索引正确*/
    if(index < CANINDEX2_LEN)
    {
    	para = gCANSerialNum_2[index];

    	/*写入成功*/
		if(TRUE == ParaWriteStoreDevSerial(index + 1, &para, 1))
		{
			/*记录标志*/
			if(20 == index)
			{
				para = 0x5a;
				(void)ParaWriteStoreDevSerial(0, &para, 1);
			}

            return(TRUE);
		}
    	/*写入失败*/
		else
		{
			/*清除显示值*/
			gCANSerialNum_2[index] = 0xff;
		}

    	/*事件记录*/
     	;
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 RcvSystemGenParaCfgAPI(u8 index, u8 port)
** @Input     : index:参数字段索引号 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 系统通用参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 RcvSystemGenParaCfgAPI(u8 index, u8 port)
{
    u16 para = 0;

    /*索引正确*/
    if(index < SINDEX105_LEN)
    {
    	para = gSysGenParaRW_105[index];

        /*写入成功*/
    	if(TRUE == ParaWriteStoreSystemGenParaInfo(index, &para, 1))
        {
    		/*显示区索引*/
            if(index < SINDEX100_LEN)
            {
            	/*产生事件*/
            	EventRcdDisplayUserAPI(eEVN_RCDSORT_CFG, 0, index, gSysGenParaRO_100[index], para, port);

            	gSysGenParaRO_100[index] = para;
            }
            /*特殊值*/
            else
            {
                /*电池组激活状态值*/
            	if(eSGenPara105_GroupEn == index)
            	{
            		#if(1 == PILE_MANAG_EN)
                	gPGroupState_57[ePGroupState57_EnG] = gSysGenParaRW_105[eSGenPara105_GroupEn];
                	#endif
            	}
            }

            return(TRUE);
        }
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 RcvSystemHWParaCfgAPI(u8 index, u8 port)
** @Input     : index:参数字段索引号 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 系统硬件参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 RcvSystemHWParaCfgAPI(u8 index, u8 port)
{
    u16 para = 0;

    /*索引正确*/
    if(index < SINDEX106_LEN)
    {
    	para = gSysHigParaRW_106[index];

        /*写入成功*/
    	if(TRUE == ParaWriteStoreSystemHWParaInfo(index, &para, 1))
        {
    		/*显示区索引*/
            if(index < SINDEX101_LEN)
            {
            	/*产生事件*/
            	EventRcdDisplayUserAPI(eEVN_RCDSORT_CFG, 1, index, gSysHigParaRO_101[index], para, port);

            	gSysHigParaRO_101[index] = para;
            }
            /*特殊值*/
            else
            {
            	;
            }

            return(TRUE);
        }
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 RcvGroupGenParaCfgAPI(u8 index, u8 port)
** @Input     : index:参数字段索引号 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 电池组主控通用参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 RcvGroupGenParaCfgAPI(u8 index, u8 port)
{
    u16 para = 0;

    /*索引正确*/
    if(index < GINDEX107_LEN)
    {
    	para = gGBmuGenParaWR_107[index];

        /*写入成功*/
    	if(TRUE == ParaWriteStoreGroupGenParaInfo(index, &para, 1))
        {
    		/*显示区索引*/
            if(index < GINDEX102_LEN)
            {
            	/*产生事件*/
            	EventRcdDisplayUserAPI(eEVN_RCDSORT_CFG, 2, index, gGBmuGenPara_102[index], para, port);

            	gGBmuGenPara_102[index] = para;

            	/*修改SOC要特殊处理*/
            	if(eBmuGenPara102_NowSoc == index)
            	{
                	/*更新显示SOC和内部当前容量*/
            		CorrGDisplaySocByUser(para * 10);
            		CorrGNowCapBySocAPI(para);
            	}
            	/*修改SOE要特殊处理*/
            	else if(eBmuGenPara102_NowSoe == index)
            	{
                	/*更新显示SOC和内部当前容量*/
            		CorrGDisplaySoeByUser(para * 10);
            		CorrGNowEnerBySoeAPI(para);
            	}
            	/*修改总容量要特殊处理*/
            	else if(eBmuGenPara102_ReaAllCap == index)
            	{
                	/*更新内部总容量*/
            		CorrGroupTotalCapAPI(para * 100);
            	}
            	/*修改总电能要特殊处理*/
            	else if(eBmuGenPara102_ReaAllEner == index)
            	{
                	/*更新内部总电能*/
            		CorrGroupTotalEnerAPI(para * 100);

            		/*配置整数为标称能量*/
            		if(0 == (para % 10))
            		{
                    	/*更新内部标称能量*/
            			CorrGroupStandEnerAPI(para * 100);
            		}
            	}
            	/*修改额定容量要特殊处理*/
            	else if(eBmuGenPara102_RateCap == index)
            	{
            		/*配置整数为标称容量*/
					if(0 == (para % 10))
					{
						/*更新内部标称容量*/
						CorrGroupStandCapAPI(para * 100);
					}
            	}
            }
            /*特殊值*/
            else
            {
            	;
            }

            return(TRUE);
        }
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 RcvGroupHigParaCfgAPI(u8 index, u8 port)
** @Input     : index:参数字段索引号 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 电池组主控高级参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 RcvGroupHigParaCfgAPI(u8 index, u8 port)
{
    u16 para = 0;

    /*索引正确*/
    if(index < GINDEX108_LEN)
    {
    	para = gGBmuHigLevParaWR_108[index];

        /*写入成功*/
    	if(TRUE == ParaWriteStoreGroupHigParaInfo(index, &para, 1))
        {
    		/*显示区索引*/
            if(index < GINDEX103_LEN)
            {
            	/*产生事件*/
            	EventRcdDisplayUserAPI(eEVN_RCDSORT_CFG, 3, index, gGBmuHigLevPara_103[index], para, port);

            	gGBmuHigLevPara_103[index] = para;
            }
            /*特殊值*/
            else
            {
            	;
            }

            return(TRUE);
        }
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 RcvGroupHWParaCfgAPI(u8 index, u8 port)
** @Input     : index:参数字段索引号 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 电池组主控硬件参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 RcvGroupHWParaCfgAPI(u8 index, u8 port)
{
    u8 para = 0;

    /*索引正确*/
    if(index < GINDEX109_LEN)
    {
    	para = gGHardParaWR_109[index];

        /*写入成功*/
    	if(TRUE == ParaWriteStoreGroupHWParaInfo(index, &para, 1))
        {
    		/*显示区索引*/
            if(index < GINDEX104_LEN)
            {
            	/*产生事件*/
            	EventRcdDisplayUserAPI(eEVN_RCDSORT_CFG, 4, index, gGHardPara_104[index], para, port);

            	gGHardPara_104[index] = para;
            }
            /*特殊值*/
            else
            {
            	;
            }

            return(TRUE);
        }
    }

    return(FALSE);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

