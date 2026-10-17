/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : GroupDebug.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池调试模式处理模块接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "ParaInterface.h"
#include "AuxDevMsg.h"
#include "TacUser.h"
#include "TacInterface.h"
#include "GroupProtect.h"
#include "GroupTactic.h"
#include "GroupDebug.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gGDebugResult_82[];		                        /*调试结果状态信息数组*/
extern u16 gGDebugCtrl_88[];			                    /*系统调试命令信息数组*/

extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];                                 /*主控硬件参数配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 TacCalcDataSetBitNum(u32 value)
** @Input     : value:需要检测的数据
** @Output    : 数据置1位数
** @Function  : 计算数据置1位数
** @The notes :
**===============================================================================================*/
static u8 TacCalcDataSetBitNum(u32 value);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void TacSetGroupDebugAffirmCmdAPI(u8 cmd)
** @Input     : cmd:调试确认命令 1:功能调试确认 2:EEPROM读调试确认 3:EEPROM写调试确认
** @Output    : void
** @Function  : 电池组调试确认命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void TacSetGroupDebugAffirmCmdAPI(u8 cmd)
{
	/*功能调试确认*/
    if(1 == cmd)
    {
        if(eMODE_DEBUG != GetGWorkModeAPI())                                //非调试模式不得调试
        {
        	gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_NOMOD;          //调试结果 2:未进入调试模式
        }
        else if(eDBGCMD_BSUBAL == gGDebugCtrl_88[eDbgCmd88_DbgType])        //从控均衡调试命令
        {
            if(TRUE == TacDebugSlaveBalanDeal())                            //设置从控均衡调试处理成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_BSUSWH == gGDebugCtrl_88[eDbgCmd88_DbgType])        //从控接触器调试命令
        {
            if(TRUE == TacDebugSlaveSwitchDeal())                           //设置从控接触器调试处理成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_BSUDRY == gGDebugCtrl_88[eDbgCmd88_DbgType])        //从控输出干接点调试命令
        {
            if(TRUE == TacDebugSlaveOutDryDeal())                           //设置从控输出干接点调试处理成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_BSUEEP == gGDebugCtrl_88[eDbgCmd88_DbgType])        //从控EEPROM参数读调试命令
        {
        	if(TRUE == TacDebugSlaveReadEepDeal())                          //设置读BMU EEP参数调试信息成功
			{
        		gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
			}
        }
        else if(eDBGCMD_BMUOUT == gGDebugCtrl_88[eDbgCmd88_DbgType])        //主控内部输出IO调试命令
        {
            if(TRUE == TacDebugGroupOutPortDeal())                          //设置主控内部输出IO调试处理成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_BMUSWH == gGDebugCtrl_88[eDbgCmd88_DbgType])        //主控接触器调试命令
        {
            if(TRUE == TacDebugGroupSwitchDeal())                           //设置主控接触器调试处理成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_BMUDRY == gGDebugCtrl_88[eDbgCmd88_DbgType])        //主控输出干接点调试命令
        {
            if(TRUE == TacDebugGroupOutDryDeal())                           //设置主控输出干接点调试处理成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_BMUEEP == gGDebugCtrl_88[eDbgCmd88_DbgType])        //主控配置参数读写调试命令
        {
            if(TRUE == TacDebugGroupReadEepDeal())                          //设置写主控 EEP参数调试信息成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_BMUAUX == gGDebugCtrl_88[eDbgCmd88_DbgType])        //主控辅助设备调试命令
        {
            if(TRUE == TacDebugGroupAuxDevDeal())                           //设置主控辅助设备调试处理成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else                                                                //调试类型不正确
        {
            gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;          //调试结果 3:设置参数错误
        }
    }
    /*EEP写调试确认*/
    else if(2 == cmd)
    {
        if(eMODE_DEBUG != GetGWorkModeAPI())                                //非调试模式不得调试
        {
            gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_NOMOD;          //调试结果 2:未进入调试模式
        }
        else if(eDBGCMD_BSUEEP == gGDebugCtrl_88[eDbgCmd88_DbgType])        //从控EEPROM参数写调试命令(不允许写从控EEPROM参数)
        {
            gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;          //调试结果 3:设置参数错误
        }
        else if(eDBGCMD_BMUEEP == gGDebugCtrl_88[eDbgCmd88_DbgType])        //主控配置参数读写调试命令
        {
            if(TRUE == TacDebugGroupWriteEepDeal())                         //主控EEP参数调试处理成功
            {
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else                                                                //调试类型不正确
        {
            gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;          //调试结果 3:设置参数错误
        }
    }
    else                                                                    //非调试确认命令
    {
        return;
    }

    /*调试成功时事件记录*/
    if(eDBGRES_SUCCE == gGDebugResult_82[eDbgRes82_DbgResult])
    {
    	TacGroupDebugSuccessDealHook(gGDebugCtrl_88[eDbgCmd88_DbgType], gGDebugCtrl_88[eDbgCmd88_BSUid], gGDebugCtrl_88[eDbgCmd88_OutNum]);
    }
}

/*=================================================================================================
** @Name      : u8 TacDebugSlaveBalanDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制从控均衡状态
** @The notes : 接收从控均衡调试确认命令时调用
**===============================================================================================*/
u8 TacDebugSlaveBalanDeal(void)
{
    u16 num = 0;
    u32 cell = 0;
    u8 result = 0;
    u8 slave = 0;
    u8 slaVNum = 0;

    slave = gGDebugCtrl_88[eDbgCmd88_BSUid];
    slaVNum = (u8)gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum];

    //从控通讯CAN断线故障
    /*if(FALSE == TacGetGAllSlaveReadyHook())
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_CANOF;              //调试结果 4:通讯CAN断线
        result = FALSE;
    }
    //禁用均衡
    else */if(0 == gGDebugCtrl_88[eDbgCmd88_BalType])
    {
    	TacGroupDebugSlaveBalanHook(0, 0, 0, 0);                            //禁止所有BMU所有均衡
        result = TRUE;
    }
    //主动均衡
    else if((1 == gGDebugCtrl_88[eDbgCmd88_BalType])
        && (slave <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))        //从控序号正确
    {
        if(0 == gGDebugCtrl_88[eDbgCmd88_BalStat])                          //关闭均衡
        {
        	TacGroupDebugSlaveBalanHook(slave, 0, 1, 0);                    //关闭指定BMU均衡
            result = TRUE;
        }
        else if(gGDebugCtrl_88[eDbgCmd88_BalStat] <= 2)                     //充电或放电均衡
        {
            cell = ((u32)gGDebugCtrl_88[eDbgCmd88_BalCellH] << 16) | gGDebugCtrl_88[eDbgCmd88_BalCellL];
            num = TacCalcDataSetBitNum(cell);                               //计算均衡电池节数

            if((0 == (cell & (0xffffffff << slaVNum)))
                && (1 == num))                                              //主动均衡只能1节电池充放电
            {
            	TacGroupDebugSlaveBalanHook(slave, cell, 1, gGDebugCtrl_88[eDbgCmd88_BalStat]);
                result = TRUE;
            }
            else                                                            //均衡电池错误
            {
               gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;       //调试结果 3:设置参数错误
               result = FALSE;
            }
        }
        //均衡状态错误
        else
        {
           gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;           //调试结果 3:设置参数错误
           result = FALSE;
        }
    }
    //被动均衡
    else if((2 == gGDebugCtrl_88[eDbgCmd88_BalType])
        && (slave <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))        //从控序号正确
    {
        if(0 == gGDebugCtrl_88[eDbgCmd88_BalStat])                          //关闭均衡
        {
        	TacGroupDebugSlaveBalanHook(slave, 0, 2, 0);
            result = TRUE;
        }
        else if(2 == gGDebugCtrl_88[eDbgCmd88_BalStat])                     //放电均衡
        {
            cell = ((u32)gGDebugCtrl_88[eDbgCmd88_BalCellH] << 16) | gGDebugCtrl_88[eDbgCmd88_BalCellL];

            if(0 == (cell & (0xffffffff << slaVNum)))
            {
            	TacGroupDebugSlaveBalanHook(slave, cell, 2, gGDebugCtrl_88[eDbgCmd88_BalStat]);
                result = TRUE;
            }
            //均衡电池错误
            else
            {
               gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;       //调试结果 3:设置参数错误
               result = FALSE;
            }
        }
        //均衡状态错误
        else
        {
           gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;            //调试结果 3:设置参数错误
           result = FALSE;
        }
    }
    //均衡方式或从控序号错误
    else
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = FALSE;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugSlaveSwitchDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制从控接触器状态
** @The notes : 接收从控接触器调试确认命令时调用
**===============================================================================================*/
u8 TacDebugSlaveSwitchDeal(void)
{
    u8 slave = 0;
    u8 result = 0;

    slave = gGDebugCtrl_88[eDbgCmd88_BSUid];

    //从控通讯CAN断线故障
    /*if(FALSE == TacGetGAllSlaveReadyHook())
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_CANOF;              //调试结果 4:通讯CAN断线
        result = FALSE;
    }
    //控制端口和状态正确
    else */if((gGDebugCtrl_88[eDbgCmd88_OutNum] <= 2)                       //两路风扇接触器
        && (gGDebugCtrl_88[eDbgCmd88_OutStat] <= 1)                         //状态正确
        && (slave <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))        //从控序号正确
    {
        //输入从控接触器控制信息
    	TacGroupDebugSlaveSwitchHook(slave, gGDebugCtrl_88[eDbgCmd88_OutNum], gGDebugCtrl_88[eDbgCmd88_OutStat]);
    }
    //控制端口或状态错误
    else
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = FALSE;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugSlaveOutDryDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制从控输出干接点状态
** @The notes : 接收从控输出干接点调试确认命令时调用
**===============================================================================================*/
u8 TacDebugSlaveOutDryDeal(void)
{
    u8 slave = 0;
    u8 result = 0;

    slave = gGDebugCtrl_88[eDbgCmd88_BSUid];

    //从控通讯CAN断线故障
    /*if(FALSE == TacGetGAllSlaveReadyHook())
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_CANOF;              //调试结果 4:通讯CAN断线
        result = FALSE;
    }
    //控制端口和状态正确
    else */if((gGDebugCtrl_88[eDbgCmd88_OutNum] <= 2)                       //两路风扇接触器
        && (gGDebugCtrl_88[eDbgCmd88_OutStat] <= 1)                         //状态正确
        && (slave <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))        //从控序号正确
    {
        //输入从控接触器控制信息
    	TacGroupDebugSlaveOutDryHook(slave, gGDebugCtrl_88[eDbgCmd88_OutNum], gGDebugCtrl_88[eDbgCmd88_OutStat]);
    }
    //控制端口或状态错误
    else
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = FALSE;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void DebugReadBmuEepData(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式读取BMUEEPROM参数值
** @The notes : 接收BMUEEPROM参数调试确认命令时调用
**===============================================================================================*/
u8 TacDebugSlaveReadEepDeal(void)
{
    u8 slave = 0;
    u8 result = 0;

    slave = gGDebugCtrl_88[eDbgCmd88_BSUid];

    //从控通讯CAN断线故障(调试不考虑从控单体是否正常)
    /*if(FALSE == TacGetGAllSlaveReadyHook())
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_CANOF;              //调试结果 4:通讯CAN断线
        result = FALSE;
    }
    //调试正确
    else */if((slave >= 1) && (slave <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])//只能读单个从控
        && (gGDebugCtrl_88[eDbgCmd88_EepAddr] <= 4095)                      //从控参数地址
        && (gGDebugCtrl_88[eDbgCmd88_EepSort] <= 2))                        //0:8位 1:16位 2:32位
    {
        //显示从控的参数值
        gGDebugResult_82[eDbgRes82_RDataLW] = 0;                            //读取数据低16位
        gGDebugResult_82[eDbgRes82_RDataHW] = 0;                            //读取数据高16位

        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_SUCCE;               //调试结果 1:成功
    }
    //调试参数错误
    else
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = FALSE;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugGroupOutPortDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制主控内部输出端口状态
** @The notes : 接收主控内部输出端口调试确认命令时调用
**===============================================================================================*/
u8 TacDebugGroupOutPortDeal(void)
{/*
    u8 i = 0;
    u8 port = 0;
    u8 state = 0;
    u8 result = 0;

    if((gGDebugCtrl_88[eDbgCmd88_SwhNum] <= eGroupSwh_Sta)                  //输出口号正确
        && (gGDebugCtrl_88[eDbgCmd88_SwhStat] < ePSTATE_NUM))               //控制状态正确
    {
        state = gGDebugCtrl_88[eDbgCmd88_SwhStat];

        //调试所有输出端口
        if(0 == gGDebugCtrl_88[eDbgCmd88_SwhNum])
        {
        	//控制所有输出端口
            for(i = 0; i < eGroupSwh_Sta; i++)
            {
                ;
            }
        }
        //调试单个输出端口
        else
        {
        	//控制指定输出端口
        	port = gGDebugCtrl_88[eDbgCmd88_SwhNum] - 1;
            ;
        }
        result = TRUE;
    }
    //输出端口号/状态设置错误
    else
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = FALSE;
    }

    return(result);*/
    return(FALSE);
}

/*=================================================================================================
** @Name      : void TacDebugGroupSwitchDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制主控接触器状态
** @The notes : 接收主控接触器调试确认命令时调用
**===============================================================================================*/
u8 TacDebugGroupSwitchDeal(void)
{
    u8 i = 0;
    u8 swhID = 0;
    u8 state = 0;
    u8 result = 0;

    if((gGDebugCtrl_88[eDbgCmd88_SwhNum] <= eGroupSwh_Sta)                  //接触器号正确
        && (gGDebugCtrl_88[eDbgCmd88_SwhStat] < ePSTATE_NUM))               //控制状态正确
    {
        state = gGDebugCtrl_88[eDbgCmd88_SwhStat];

        //调试所有接触器
        if(0 == gGDebugCtrl_88[eDbgCmd88_SwhNum])
        {
        	//控制所有接触器
            for(i = 0; i < eGroupSwh_Sta; i++)
            {
                TacCtrlGroupSwhStateAPI((e_GroupSwhSort)i, (e_GPIOState)state);
            }
        }
        //调试单个接触器
        else
        {
        	//控制指定接触器
            swhID = gGDebugCtrl_88[eDbgCmd88_SwhNum] - 1;
            TacCtrlGroupSwhStateAPI((e_GroupSwhSort)swhID, (e_GPIOState)state);
        }
        result = 1;
    }
    //接触器号/状态设置错误
    else
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = 0;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugGroupOutDryDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制主控输出干接点状态
** @The notes : 接收主控输出干接点调试确认命令时调用
**===============================================================================================*/
u8 TacDebugGroupOutDryDeal(void)
{
    u8 i = 0;
    u8 dry = 0;
    u8 state = 0;
    u8 result = 0;

    if((gGDebugCtrl_88[eDbgCmd88_SwhNum] <= 3)                              //接触器号正确
        && (gGDebugCtrl_88[eDbgCmd88_SwhStat] < ePSTATE_NUM))               //控制状态正确
    {
        state = gGDebugCtrl_88[eDbgCmd88_SwhStat];

        //调试所有干接点
        if(0 == gGDebugCtrl_88[eDbgCmd88_SwhNum])
        {
        	//控制所有接触器
            for(i = 0; i < 3; i++)
            {
                TacCtrlGroupSwhStateAPI((e_GroupSwhSort)(eGroupSwh_Sta + i), (e_GPIOState)state);
            }
        }
        //调试单个接触器
        else
        {
        	//控制指定接触器
        	dry = gGDebugCtrl_88[eDbgCmd88_SwhNum] - 1;
            TacCtrlGroupSwhStateAPI((e_GroupSwhSort)(eGroupSwh_Sta + dry), (e_GPIOState)state);
        }
        result = 1;
    }
    //接触器号/状态设置错误
    else
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = 0;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugGroupReadEepDeal(void)
** @Input     : void
** @Output    : TRUE:读取成功 FALSE:读取失败
** @Function  : 调试模式读主控EEP参数值
** @The notes : 接收读主控EEP参数调试确认命令时调用
**===============================================================================================*/
u8 TacDebugGroupReadEepDeal(void)
{
    u8 result = 0;
    u8 data[4] = {0};
    u16 eepData[2] = {0};

    if(gGDebugCtrl_88[eDbgCmd88_EepAddr] < 4096)                            //读参数EEP地址正确
    {
        if(0 == gGDebugCtrl_88[eDbgCmd88_EepSort])                          //读EEP8位参数
        {
        	ParaReadStoreDataByte(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 1);

            gGDebugResult_82[eDbgRes82_RDataLW] = data[0];                  //得到低16位数据
            gGDebugResult_82[eDbgRes82_RDataHW] = 0;                        //高16位数据为0
            result = TRUE;
        }
        else if(1 == gGDebugCtrl_88[eDbgCmd88_EepSort])                     //读EEP16位参数
        {
        	ParaReadStoreDataByte(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 2);

            eepData[0] = ((u16)data[1] << 8) | data[0];                      //EEP小端模式(低位在前)
            gGDebugResult_82[eDbgRes82_RDataLW] = eepData[0];                //得到低16位数据
            gGDebugResult_82[eDbgRes82_RDataHW] = 0;                         //高16位数据为0
            result = TRUE;
        }
        else if(2 == gGDebugCtrl_88[eDbgCmd88_EepSort])                     //读EEP32位参数
        {
        	ParaReadStoreDataByte(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 4);

            eepData[0] = ((u16)data[1] << 8) | data[0];                     //EEP小端模式(低位在前)
            eepData[1] = ((u16)data[3] << 8) | data[2];
            gGDebugResult_82[eDbgRes82_RDataLW] = eepData[0];               //得到低16位数据
            gGDebugResult_82[eDbgRes82_RDataHW] = eepData[1];               //得到高16位数据
            result = TRUE;
        }
        else                                                                //读数据类型错误
        {
            gGDebugResult_82[eDbgRes82_RDataLW] = 0;
            gGDebugResult_82[eDbgRes82_RDataHW] = 0;
            gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;          //调试结果 3:设置参数错误
            result = FALSE;
        }
    }
    else                                                                    //读参数EEP地址错误
    {
        gGDebugResult_82[eDbgRes82_RDataLW] = 0;
        gGDebugResult_82[eDbgRes82_RDataHW] = 0;
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
        result = FALSE;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugGroupWriteEepDeal(void)
** @Input     : void
** @Output    : TRUE:写入成功 FALSE:写入失败
** @Function  : 调试模式写主控EEP参数值
** @The notes : 接收写主控EEP参数调试确认命令时调用
**===============================================================================================*/
u8 TacDebugGroupWriteEepDeal(void)
{
    u8 result = 0;
    u8 data[4] = {0};

    if(gGDebugCtrl_88[eDbgCmd88_EepAddr] < 4096)               //写参数EEP地址正确
    {
        if(0 == gGDebugCtrl_88[eDbgCmd88_EepSort])             //写EEP8位数据
        {
            //写EEP参数
            data[0] = gGDebugCtrl_88[eDbgCmd88_WDataLW] & 0xff;

            ParaWriteParaUserHandleHook(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 1);

            //读EEP参数
            ParaReadStoreDataByte(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 1);

            gGDebugResult_82[eDbgRes82_RDataLW] = data[0];
            gGDebugResult_82[eDbgRes82_RDataHW] = 0;

            result = TRUE;
        }
        else if(1 == gGDebugCtrl_88[eDbgCmd88_EepSort])        //写EEP16位数据
        {
            //写EEP参数:EEP小端模式(低位在前)
            data[0] = gGDebugCtrl_88[eDbgCmd88_WDataLW] & 0xff;
            data[1] = (gGDebugCtrl_88[eDbgCmd88_WDataLW] >> 8) & 0xff;

            ParaWriteParaUserHandleHook(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 2);

            //读EEP参数:EEP小端模式(低位在前)
            ParaReadStoreDataByte(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 2);

            gGDebugResult_82[eDbgRes82_RDataLW] = ((u16)data[1] << 8) | data[0];
            gGDebugResult_82[eDbgRes82_RDataHW] = 0;

            result = TRUE;
        }
        else if(2 == gGDebugCtrl_88[eDbgCmd88_EepSort])        //写EEP32位数据
        {
            //写銭EP参数:EEP小端模式(低位在前)
            data[0] = gGDebugCtrl_88[eDbgCmd88_WDataLW] & 0xff;
            data[1] = (gGDebugCtrl_88[eDbgCmd88_WDataLW] >> 8) & 0xff;
            data[2] = gGDebugCtrl_88[eDbgCmd88_WDataHW] & 0xff;
            data[3] = (gGDebugCtrl_88[eDbgCmd88_WDataHW] >> 8) & 0xff;

            ParaWriteParaUserHandleHook(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 4);

            //读EEP参数:EEP小端模式(低位在前)
            ParaReadStoreDataByte(gGDebugCtrl_88[eDbgCmd88_EepAddr], data, 4);

            gGDebugResult_82[eDbgRes82_RDataLW] = ((u16)data[1] << 8) | data[0];
            gGDebugResult_82[eDbgRes82_RDataHW] = ((u16)data[3] << 8) | data[2];

            result = TRUE;
        }
        else                                                    //写数据类型错误
        {
            gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;           //调试结果 3:设置参数错误
            result = FALSE;
        }
    }
    else                                                        //写参数EEP地址错误
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = FALSE;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugGroupAuxDevDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:同时失败
** @Function  : 调试模式调试主控辅助设备状态
** @The notes : 接收读主控EEP外设调试确认命令时调用
**===============================================================================================*/
u8 TacDebugGroupAuxDevDeal(void)
{
    u8 i = 0;
    u8 result = 0;

    if((gGDebugCtrl_88[eDbgCmd88_DevSort] <= 10)                            //外设类型正确
        && (gGDebugCtrl_88[eDbgCmd88_DevState] <= 100))                     //外设状态(参数)正确
    {
        result = 1;

        //调试电池箱调速风扇
        if(0 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//关闭全部风扇
        	if(0 == gGDebugCtrl_88[eDbgCmd88_DevState])
        	{
				//控制所有风扇调速为0
				TacSetPwmFanUserCmdAPI(0xff, 0);
        	}
        	//按调速开启风扇
        	else
        	{
				//控制选择的风扇调速为目标转速
				for(i = 0; i < BMU_PWM_FAN_NUM; i++)
				{
					//低16通道或高16通道选择调速
					if(((i < 16) && (1 == BitGet(gGDebugCtrl_88[eDbgCmd88_DevFanL], i)))
						|| ((i >= 16) && (1 == BitGet(gGDebugCtrl_88[eDbgCmd88_DevFanH],  (i - 16)))))
					{
						//设置转速
						TacSetPwmFanUserCmdAPI(i, gGDebugCtrl_88[eDbgCmd88_DevState]);
					}
					//不选择调速
					else
					{
						//清除转速
						TacSetPwmFanUserCmdAPI(i, 0);
					}
				}
        	}
        }
        //调试电池柜百叶风机
        else if(1 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//控制状态正确
        	if(gGDebugCtrl_88[eDbgCmd88_DevState] <= 1)
        	{
				//控制百叶启停
        		TacSetAuxShutUserCmdAPI(gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试空调电源
        else if(2 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//控制状态正确
        	if(gGDebugCtrl_88[eDbgCmd88_DevState] <= 1)
        	{
				//控制空调强制开关机
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForPwr, gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试空调送风
        else if(3 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//控制状态正确
        	if(gGDebugCtrl_88[eDbgCmd88_DevState] <= 1)
        	{
				//控制空调强制送风
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForBlow, gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试空调待机
        else if(4 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//控制状态正确
        	if(gGDebugCtrl_88[eDbgCmd88_DevState] <= 1)
        	{
				//控制空调强制待机
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForIdle, gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试空调制冷
        else if(5 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//强制控制
        	if(((s16)gGDebugCtrl_88[eDbgCmd88_DevState] >= 0) && ((s16)gGDebugCtrl_88[eDbgCmd88_DevState] <= 1))
        	{
				//控制空调强制制冷
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForCold, gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//参数控制
            else if(((s16)gGDebugCtrl_88[eDbgCmd88_DevState] >= 16) && ((s16)gGDebugCtrl_88[eDbgCmd88_DevState] <= 40))
        	{
				//控制空调参数制冷
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOnT, gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试空调制热
        else if(6 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//强制控制
        	if(((s16)gGDebugCtrl_88[eDbgCmd88_DevState] >= 0) && ((s16)gGDebugCtrl_88[eDbgCmd88_DevState] <= 1))
        	{
				//控制空调强制制热
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForHeat, gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//参数控制
        	else if(((s16)gGDebugCtrl_88[eDbgCmd88_DevState] >= 2) && ((s16)gGDebugCtrl_88[eDbgCmd88_DevState] <= 30))
        	{
				//控制空调参数制热
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_HeatOnT, gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试空调除湿
        else if(7 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//关闭除湿控制
        	if(0 == gGDebugCtrl_88[eDbgCmd88_DevState])
        	{
				//控制空调除湿参数最大
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOnH, 99);
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOffH, 95);
        	}
        	//开启除湿控制
        	else if(1 == gGDebugCtrl_88[eDbgCmd88_DevState])
        	{
				//控制空调除湿参数最小
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOnH, 50);
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOffH, 45);
        	}
        	//参数控制
        	else if((gGDebugCtrl_88[eDbgCmd88_DevState] >= 50) && (gGDebugCtrl_88[eDbgCmd88_DevState] <= 99))
        	{
				//控制空调参数除湿(开启)
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOnH, gGDebugCtrl_88[eDbgCmd88_DevState]);
        		SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOffH, (gGDebugCtrl_88[eDbgCmd88_DevState] - 5));
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试电池柜照明
        else if(8 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//控制状态正确
        	if(gGDebugCtrl_88[eDbgCmd88_DevState] <= 1)
        	{
        		/*照明开*/
        		if(1 == gGDebugCtrl_88[eDbgCmd88_DevState])
        		{
        			/*输入控制命令*/
        			TacSetAuxRackLightCmdAPI(1);
        		}
        		else
        		{
        			/*输入控制命令*/
        			TacSetAuxRackLightCmdAPI(2);
        		}
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试电池柜声光报警
        else if(9 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//控制状态正确
        	if(gGDebugCtrl_88[eDbgCmd88_DevState] <= 1)
        	{
        		/*声光报警器开*/
        		if(1 == gGDebugCtrl_88[eDbgCmd88_DevState])
        		{
        			/*输入控制命令*/
        			TacSetAuxZHSAForceCmdAPI(1);
        		}
        		else
        		{
        			/*输入控制命令*/
        			TacSetAuxZHSAForceCmdAPI(2);
        		}
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
        //调试电池柜气溶胶消防触发
        else if(10 == gGDebugCtrl_88[eDbgCmd88_DevSort])
        {
        	//控制状态正确
        	if(gGDebugCtrl_88[eDbgCmd88_DevState] <= 1)
        	{
        		/*输入控制命令*/
        		TacSetAuxFireTrigCmdAPI(gGDebugCtrl_88[eDbgCmd88_DevState]);
        	}
        	//控制状态错误
        	else
        	{
                gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;              //调试结果 3:设置参数错误
                result = 0;
        	}
        }
    }
    //外设类型/设状态(参数)设置错误
    else
    {
        gGDebugResult_82[eDbgRes82_DbgResult] = eDBGRES_ERRPA;                      //调试结果 3:设置参数错误
        result = 0;
    }

    return(result);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 TacCalcDataSetBitNum(u32 value)
** @Input     : value:需要检测的数据
** @Output    : 数据置1位数
** @Function  : 计算数据置1位数
** @The notes :
**===============================================================================================*/
static u8 TacCalcDataSetBitNum(u32 value)
{
    u8 num = 0;

    while(0 != value)
    {
        if(value & 0x01)
        {
            num++;
        }

        value >>= 1;
    }

    return(num);
}

