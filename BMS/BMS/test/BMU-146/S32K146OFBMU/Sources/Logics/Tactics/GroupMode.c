/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : GroupMode.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池工作模式信息模块接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "ParaInterface.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "TacInterface.h"
#include "TacUser.h"
#include "GroupMode.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sHisNorMode = 0;                                 /*原始正常模式 0自动模式 1手动模式*/
static u32 sRcvUnorModeTime = 0;                            /*接收进入非正常模式时间 1ms*/

extern u16 gGSysStateInfo_50[];                             /*系统状态信息数组*/
extern u16 gGSysCmdCtrl_86[];                               /*系统状态信息数组*/

extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];                                 /*主控硬件参数配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupModeInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组模式信息初始化
** @The notes : 读EEPROM写入值
**===============================================================================================*/
void GroupModeInfoInit(void)
{
	u16 data[2] = {0};

	/*读存储值设置自动模式还是手动模式*/
	if(TRUE == ParaReadStoreSystemWorkMode(data, 2))
	{
        gGSysStateInfo_50[eSysStateInfo50_SysMod] = data[1];
	}
	/*读设置值失败*/
	else
	{
		/*重写默认值*/
		data[0] = PARA_CFG_FLAG_U16;
		data[1] = gGSysStateInfo_50[eSysStateInfo50_SysMod];
		ParaWriteStoreSystemWorkMode(1, &data[1], 1);
		ParaWriteStoreSystemWorkMode(0, &data[0], 1);
	}

	sHisNorMode = gGSysStateInfo_50[eSysStateInfo50_SysMod];
}

/*=================================================================================================
** @Name      : void GroupWorkModeTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组工作模式检测任务
** @The notes : 1s周期调用
**===============================================================================================*/
void GroupWorkModeTask(void)
{
    static u8 sTimer = 0;

    /*处于紧急模式或调试模式*/
	if((eMODE_EMERG == GetGWorkModeAPI())
		|| (eMODE_DEBUG == GetGWorkModeAPI()))
	{
		/*特殊模式保持10min后自动退出*/
		if(ABS(BSPGetOSMillSecTimeAPI(), sRcvUnorModeTime) >= MODE_KEEP_UNORMODE_T)
		{
			//切换到原始正常模式命令
			gGSysCmdCtrl_86[eSysCmdCtrl86_Mode] = sHisNorMode;
			RcvGroupWorkModeCmdAPI(eCMDPORT_INTE);
		}
	}

    /*自动模式或紧急模式*/
    if((eMODE_AUTO == GetGWorkModeAPI())
        || (eMODE_EMERG == GetGWorkModeAPI()))
    {
        //系统处于空闲且适合启动
        if((eWORK_IDLE == GetGWorkStateAPI())
            && (1 == TacGetGroupFitStartStateHook()))
        {
            //达到启动等待延时或上电首次启动
            if((sTimer >= (gGBmuHigLevPara_103[eBmuHigLevPara103_SelfWaitT] / 10))
                || (BSPGetOSMillSecTimeAPI() <= 10000))
            {
                //设置正常启动命令
                gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTART;
                RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
            }
            //未达到启动延时
            else
            {
                sTimer++;
            }
        }
        //系统正在启动未闭合接触器时不适合启动则停机否则由故障保护启动失败
        else if((eWORK_START == GetGWorkStateAPI())
            && (0 == TacGetGroupFitStartStateHook())
            && (1 == TacGetGroupRunSwhAllOffStateAPI()))
        {
            //立即停止系统
            gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTOP;
            RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
            sTimer = 0;
        }
        //系统非空闲或充放电保护
        else
        {
            //启动延时清零
            sTimer = 0;
        }
    }
    else
    {
        //启动延时清零
        sTimer = 0;
    }
}

/*=================================================================================================
** @Name      : void TacSetGroupWorkModeAPI(e_WorkMode mode)
** @Input     : mode:电池组工作模式[0,5]
** @Output    : void
** @Function  : 设置电池组工作模式
** @The notes : 非正常模式只能退回正常模式,非正常模式之间不能跳转
**===============================================================================================*/
void TacSetGroupWorkModeAPI(e_WorkMode mode)
{
	u16 data[2] = {0};

	if(mode >= eMODE_STATE)
    {
        return;
    }

    /*正处于调试模式或紧急模式*/
    if((eMODE_DEBUG == gGSysStateInfo_50[eSysStateInfo50_SysMod])
    	|| (eMODE_EMERG == gGSysStateInfo_50[eSysStateInfo50_SysMod]))
    {
        /*退出调试模式或紧急模式到原来模式*/
    	if(eMODE_AUTO == mode)
    	{
    		/*模式切换用户处理*/
    		TacGroupWorkModeChangDealHook((u8)gGSysStateInfo_50[eSysStateInfo50_SysMod], (u8)sHisNorMode);

    		/*只能由特殊模式切换至原来正常模式*/
    		gGSysStateInfo_50[eSysStateInfo50_SysMod] = sHisNorMode;
    	}
        /*退出调试模式或紧急模式到手动模式*/
    	else if(eMODE_HAND == mode)
    	{
    		/*原来非手动模式*/
    		if(eMODE_HAND != sHisNorMode)
    		{
    			/*记录新模式*/
    			data[1] = mode;
    			data[0] = PARA_CFG_FLAG_U16;
    			ParaWriteStoreSystemWorkMode(1, &data[1], 1);
    			ParaWriteStoreSystemWorkMode(0, &data[0], 1);
    		}

    		/*模式切换用户处理*/
    		TacGroupWorkModeChangDealHook((u8)gGSysStateInfo_50[eSysStateInfo50_SysMod], (u8)mode);

    		/*只能由特殊模式切换至正常模式*/
    		gGSysStateInfo_50[eSysStateInfo50_SysMod] = mode;
    	}
    }
    /*正处于手动模式或自动模式*/
    else
    {
        /*模式切换*/
    	if( mode != gGSysStateInfo_50[eSysStateInfo50_SysMod])
    	{
    		/*手动/自动切换*/
        	if((eMODE_HAND == mode) || (eMODE_AUTO == mode))
        	{
        		/*记录新模式*/
    			data[1] = mode;
    			data[0] = PARA_CFG_FLAG_U16;
    			ParaWriteStoreSystemWorkMode(1, &data[1], 1);
    			ParaWriteStoreSystemWorkMode(0, &data[0], 1);
        	}

        	/*记录原始模式和模式切换时间*/
        	sHisNorMode = gGSysStateInfo_50[eSysStateInfo50_SysMod];

    		/*模式切换用户处理*/
    		TacGroupWorkModeChangDealHook((u8)gGSysStateInfo_50[eSysStateInfo50_SysMod], (u8)mode);

        	/*切换当前模式接收模式切换时间*/
        	gGSysStateInfo_50[eSysStateInfo50_SysMod] = mode;
    	}
    }

    /*记录接收到切换调试模式的时间*/
	sRcvUnorModeTime = BSPGetOSMillSecTimeAPI();
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
