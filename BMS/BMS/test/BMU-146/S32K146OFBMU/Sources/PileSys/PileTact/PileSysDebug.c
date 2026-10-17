/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileSysDebug.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控调试模式策略模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "PublicInfo.h"
#include "PileInfo.h"
#include "PileGroupRcv.h"
#include "PileWorkState.h"
#include "PileTactInterface.h"
#include "PileSysDebug.h"

#if(1 == PILE_MANAG_EN)
#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 gPileSysDebugStep = 0;	    /*堆控系统调试步骤:0:未调试模式 [1,9]:调试电池组步骤*/

extern u16 gPDebugResult_82[];			/*调试结果状态信息数组*/
extern u16 gPDebugCtrl_88[];			/*系统调试命令信息数组*/

extern u16 gSysGenParaRO_100[];  		/*系统通用参数设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileDebugModeGroupCtrlDeal(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 堆控调试模式下电池组调试控制处理
//注意事项	: 工作模式检测任务调用
//=============================================================================================
void PileDebugModeGroupCtrlDeal(void)
{
    u8 i = 0;
    u8 result = 0;
    static u16 sTimeOut = 0;
    
    if(eMODE_DEBUG != GetPileWorkModeAPI())
    {
        sTimeOut = 0;
    	gPileSysDebugStep = 0;
    	return;
    }

    switch(gPileSysDebugStep)
    {
        //调试参数复位
        case 0:
            sTimeOut = 0;
            break;
            
        //开始执行调试(清除原有调试结果)
        case 1:
        	//非电池组调试
            if((gPDebugResult_82[eDbgRes82_DbgType] > eDBGCMD_BMUEEP)
                || (gPDebugResult_82[eDbgRes82_BMUid] > gSysGenParaRO_100[eSGenPara100_AllGNum]))
            {
            	//调试结果 0:未调试
            	gPileSysDebugStep = 0;
                gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_UNEXE;
            }
        	//电池组调试
            else
            {
            	//清除电池组调试结果
            	gPileSysDebugStep = 2;
            	ClrPileAllGroupDebugResultAPI();
            }
            break;
            
		//发送调试命令
		case 2:
			/*将原设定调试参数重新赋值,防止调试过程上层改变参数*/
			for(i = 0; i < eDbgCmd88_Affirm; i++)
			{
				gPDebugCtrl_88[i] = gPDebugResult_82[i];
			}

			//所有电池组调试
			if(0 == gPDebugResult_82[eDbgRes82_BMUid])
			{
				//发送电池组广播控制信息
				PileBroadAllGroupDebugCmdHook(0, PINDEX88_LEN, gPDebugCtrl_88);
			}
			//单组电池组调试
			else
			{
				//发送电池组单组控制信息
				PileSetGroupSysDebugCtrlCmdHook((gPDebugResult_82[eDbgRes82_BMUid] - 1), 0, PINDEX88_LEN, gPDebugCtrl_88);
			}

			//未判断结果
			if(0 == sTimeOut)
			{
				//发送一次命令后返回再清一次原结果信息
				sTimeOut = 1;
				gPileSysDebugStep = 1;
			}
			//已判断结果
			else
			{
				//再进行结果判断
				gPileSysDebugStep = 3;
			}
			break;

        //检测电池组是否调试成功
        case 3:
        	result = TacCheckPileGDebugResultHook();

        	//电池组调试成功
            if(eDBGRES_SUCCE == result)
            {
                sTimeOut = 0;
    			gPileSysDebugStep = 0;
    			gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;
            }
            else
            {
                sTimeOut++;
                if(sTimeOut >= 30)
                {
                	gPileSysDebugStep = 0;
        			gPDebugResult_82[eDbgRes82_DbgResult] = result;
                }
                else
                {
                	gPileSysDebugStep = 2;
                }
            }
            break;
            
        default:
        	gPileSysDebugStep = 0;
            break;
    }
}

//=============================================================================================
//函数名称	: void TacSetPileDebugAffirmCmdAPI(u8 cmd)
//输入参数	: cmd:调试确认命令 1:功能调试确认 2:写EEPROM调试确认
//输出参数	: void
//函数功能	: 设置系统调试确认命令
//注意事项	:
//=============================================================================================
void TacSetPileDebugAffirmCmdAPI(u8 cmd)
{
	/*功能调试确认*/
    if(1 == cmd)
    {
        if(eMODE_DEBUG != GetPileWorkModeAPI())                             //非调试模式不得调试
        {
        	gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_NOMOD;          //调试结果 2:未进入调试模式
        }
        else if(eDBGCMD_SMUOUT == gPDebugCtrl_88[eDbgCmd88_DbgType])        //堆控内部输出IO调试命令
        {
            //if(TRUE == TacDebugPileOutPortDeal())                         //设置堆控内部输出IO调试处理成功
            {
                gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_SMUSWH == gPDebugCtrl_88[eDbgCmd88_DbgType])        //堆控接触器调试命令
        {
            //if(TRUE == TacDebugPileSwitchDeal())                          //设置堆控接触器调试处理成功
            {
                gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_SMUDRY == gPDebugCtrl_88[eDbgCmd88_DbgType])        //堆控输出干接点调试命令
        {
            //if(TRUE == TacDebugPileutDryDeal())                           //设置堆控输出干接点调试处理成功
            {
                gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_SMUEEP == gPDebugCtrl_88[eDbgCmd88_DbgType])        //堆控配置参数读写调试命令
        {
            //if(TRUE == TacDebugPileReadEepDeal())                         //设置写堆控 EEP参数调试信息成功
            {
                gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(gPDebugCtrl_88[eDbgCmd88_DbgType] < eDBGCMD_FIN)            //主控(从控)调试命令
        {
            gPileSysDebugStep = 1;                                          //开始电池组调试
			gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_UNEXE;          //调试结果 0:未调试
        }
        else                                                                //调试类型不正确
        {
            gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;          //调试结果 3:设置参数错误
        }
    }
    /*EEP写调试确认*/
    else if(2 == cmd)
    {
        if(eMODE_DEBUG != GetPileWorkModeAPI())                             //非调试模式不得调试
        {
            gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_NOMOD;          //调试结果 2:未进入调试模式
        }
        else if(eDBGCMD_SMUEEP == gPDebugCtrl_88[eDbgCmd88_DbgType])        //堆控配置参数读写调试命令
        {
            //if(TRUE == TacDebugPileWriteEepDeal())                        //堆控EEP参数调试处理成功
            {
                gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if((eDBGCMD_BSUEEP == gPDebugCtrl_88[eDbgCmd88_DbgType])       //从控配置参数读写调试命令
        	|| (eDBGCMD_BMUEEP == gPDebugCtrl_88[eDbgCmd88_DbgType]))       //主控配置参数读写调试命令
        {
            gPileSysDebugStep = 1;                                          //开始电池组调试
			gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_UNEXE;          //调试结果 0:未调试
        }
        else                                                                //调试类型不正确
        {
            gPDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;          //调试结果 3:设置参数错误
        }
    }
    else                                                                    //非调试确认命令
    {
        return;
    }

    /*调试成功时事件记录*/
    if(eDBGRES_SUCCE == gPDebugResult_82[eDbgRes82_DbgResult])
    {
    	//TacPileDebugSuccessDealHook(gPDebugCtrl_88[eDbgCmd88_DbgType], gPDebugCtrl_88[eDbgCmd88_BSUid], gPDebugCtrl_88[eDbgCmd88_OutNum]);
    }
}

//=============================================================================================
//函数名称	: u8 GetPileSysDebugStepAPI(void)
//输入参数	: void
//输出参数	: 当前调试步骤:0:未执行(已执行) 其它:正在执行
//函数功能	: 获取堆控当前调试步骤
//注意事项	: 
//=============================================================================================
u8 GetPileSysDebugStepAPI(void)
{
    return(gPileSysDebugStep);
}

//=============================================================================================
//函数名称	: void DebugCtrlStackOutputIO(void)
//输入参数	: void
//输出参数	: TRUE:调试成功 FALSE:调试失败
//函数功能	: 调试模式控制电池堆输出IO/LED
//注意事项	: 
//=============================================================================================
u8 DebugCtrlStackOutputIO(void)
{
    u8 result = 0;
    /*
    u8 i = 0;
    u8 ioID = 0;
    u8 state = 0;
    if((gDebugInfo_75[eDebug75_BMUid] <= eOUTPNUM)              //输出IO号正确
        && (gDebugInfo_75[eDebug75_BalState] <= eIOSTATE))      //控制状态正确
    {
        state = gDebugInfo_75[eDebug75_BalState];
        
        if(0 == gDebugInfo_75[eDebug75_BMUid])                  //调试所有输出IO
        {
            for(i = 0; i < eOUTPNUM; i++)                       //控制所有输出LED
            {
                (void)BSPIOCtrlOutputPort((e_OUTPNUM)i, (e_IOSTATE)state);
            }
        }
        //调试单个输出IO
        else
        {
            ioID = gDebugInfo_75[eDebug75_BMUid] - 1;
            (void)BSPIOCtrlOutputPort((e_OUTPNUM)ioID, (e_IOSTATE)state);
        }
        result = 1;
    }
    //IO号/状态设置错误
    else
    {
        gDebugEepInfo_66[eEepInfo66_Result] = 3;                //调试结果 3:设置参数错误
        result = 0;
    }
    */
    return(result);
}

//=============================================================================================
//函数名称	: void DebugReadStackEepData(void)
//输入参数	: void
//输出参数	: TRUE:读取成功 FALSE:读取失败
//函数功能	: 调试模式读电池堆EEP参数
//注意事项	: 
//=============================================================================================
u8 DebugReadStackEepData(void)
{
    u8 result = 0;
    /*
    u8 data[4] = {0};
    u16 eepData[2] = {0};
    
    if(gDebugEepCfg_76[eEepCfg76_EepAddr] < 4096)           //读参数EEP地址正确
    {
        if(0 == gDebugEepCfg_76[eEepCfg76_DataSort])        //读EEP8位参数
        {
            ReadCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 1);
            
            gDebugEepInfo_66[eEepInfo66_RODataLW] = data[0];//得到低16位数据
            gDebugEepInfo_66[eEepInfo66_RODataHW] = 0;      //高16位数据为0
            result = TRUE;
        }
        else if(1 == gDebugEepCfg_76[eEepCfg76_DataSort])    //读EEP16位参数
        {
            ReadCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 2);
            
            eepData[0] = ((u16)data[0] << 8) | data[1];     //EEP大端模式(高位在前)
            gDebugEepInfo_66[eEepInfo66_RODataLW] = eepData[0]; //得到低16位数据
            gDebugEepInfo_66[eEepInfo66_RODataHW] = 0;      //高16位数据为0
            result = TRUE;
        }
        else if(2 == gDebugEepCfg_76[eEepCfg76_DataSort])   //读EEP32位参数
        {
            ReadCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 4);
            
            eepData[0] = ((u16)data[2] << 8) | data[3];     //EEP大端模式(高位在前)
            eepData[1] = ((u16)data[0] << 8) | data[1];
            gDebugEepInfo_66[eEepInfo66_RODataLW] = eepData[0];//得到低16位数据
            gDebugEepInfo_66[eEepInfo66_RODataHW] = eepData[1];//得到高16位数据
            result = TRUE;
        }
        else                                                //读数据类型错误
        {
            gDebugEepInfo_66[eEepInfo66_Result] = 3;        //调试结果 3:设置参数错误
            result = FALSE;
        }
    }
    else                                                    //读参数EEP地址错误
    {
        gDebugEepInfo_66[eEepInfo66_Result] = 3;            //调试结果 3:设置参数错误
        result = FALSE;
    }
    */
    return(result);
}

//=============================================================================================
//函数名称	: void DebugWriteStackEepData(void)
//输入参数	: void
//输出参数	: TRUE:写入成功 FALSE:写入失败
//函数功能	: 调试模式写电池堆EEP参数
//注意事项	: 
//=============================================================================================
u8 DebugWriteStackEepData(void)
{
    u8 result = 0;
    /*
    u8 data[4] = {0};
    
    if(gDebugEepCfg_76[eEepCfg76_EepAddr] < 4096)           //写参数EEP地址正确
    {
        if(0 == gDebugEepCfg_76[eEepCfg76_DataSort])        //写EEP8位数据
        {
            //写EEP参数
            data[0] = gDebugEepCfg_76[eEepCfg76_RWDataLW] & 0xff;
            
            WriteCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 1);
            
            //读EEP参数
            ReadCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 1);
            
            gDebugEepInfo_66[eEepInfo66_RODataHW] = 0;
            gDebugEepInfo_66[eEepInfo66_RODataLW] = data[0];
            
            result = TRUE;
        }
        else if(1 == gDebugEepCfg_76[eEepCfg76_DataSort])    //写EEP16位数据
        {
            //写EEP参数:EEP大端模式(高位在前)
            data[0] = (gDebugEepCfg_76[eEepCfg76_RWDataLW] >> 8) & 0xff;
            data[1] = gDebugEepCfg_76[eEepCfg76_RWDataLW] & 0xff;
            
            WriteCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 2);
            
            //读EEP参数
            ReadCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 2);
            
            gDebugEepInfo_66[eEepInfo66_RODataHW] = 0;
            gDebugEepInfo_66[eEepInfo66_RODataLW] = ((u16)data[0] << 8) | data[1];
            
            result = TRUE;
        }
        else if(2 == gDebugEepCfg_76[eEepCfg76_DataSort])    //写EEP32位数据
        {
            //写EEP参数:EEP大端模式(高位在前)
            data[0] = (gDebugEepCfg_76[eEepCfg76_RWDataHW] >> 8) & 0xff;
            data[1] = gDebugEepCfg_76[eEepCfg76_RWDataHW] & 0xff;
            data[2] = (gDebugEepCfg_76[eEepCfg76_RWDataLW] >> 8) & 0xff;
            data[3] = gDebugEepCfg_76[eEepCfg76_RWDataLW] & 0xff;
            
            WriteCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 4);
            
            //读EEP参数
            ReadCfgEeprom(gDebugEepCfg_76[eEepCfg76_EepAddr], data, 4);
            
            gDebugEepInfo_66[eEepInfo66_RODataHW] = ((u16)data[0] << 8) | data[1];
            gDebugEepInfo_66[eEepInfo66_RODataLW] = ((u16)data[2] << 8) | data[3];
            
            result = TRUE;
        }
        else                                                //写数据类型错误
        {
            gDebugEepInfo_66[eEepInfo66_Result] = 3;        //调试结果 3:设置参数错误
            result = FALSE;
        }
    }
    else                                                    //写参数EEP地址错误
    {
        gDebugEepInfo_66[eEepInfo66_Result] = 3;            //调试结果 3:设置参数错误
        result = FALSE;
    }
    */
    return(result);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
#endif
