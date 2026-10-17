/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANHandle.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : CAN通信包规约处理接口
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "PublicPara.h"
#include "MbInterface.h"
#include "ModbusUser.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sMbRcvMsgCount[eSCINUM] = {0};/*接收信息计数*/

extern u16 gGBmuGenPara_102[];          /*主控通用参数值*/
extern u16 gSysHigParaRO_101[];         /*系统硬件参数设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void MbCommuResetInit(u8 sciNum)
** @Input     : sciNum:SCI端口号
** @Output    : void
** @Function  : modbus通讯复位初始化
** @The notes :
**===============================================================================================*/
static void MbCommuResetInit(u8 sciNum);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void ModbusUserInit(void)
** @Input     : void
** @Output    : void
** @Function  : modbus用户模块初始化
** @The notes :
**===============================================================================================*/
void ModbusUserInit(void)
{
    u8 addr0 = 0;
    u8 addr1 = 0;
    u8 addr2 = 0;
    u16 bps0 = 0;
    u16 bps1 = 0;
    u16 bps2 = 0;
    
    //远程端口站地址
    if((1 <= gSysHigParaRO_101[eSHWPara101_RemoteAddr]) && (gSysHigParaRO_101[eSHWPara101_RemoteAddr] <= 250))
    {
        addr0 = (u8)gSysHigParaRO_101[eSHWPara101_RemoteAddr];
    }
    else
    {
        addr0 = gGBmuGenPara_102[eBmuGenPara102_BmuDev];
        gSysHigParaRO_101[eSHWPara101_RemoteAddr] = addr0;
    }
    
    //就地端口站地址
    if((1 <= gSysHigParaRO_101[eSHWPara101_LocalAddr]) && (gSysHigParaRO_101[eSHWPara101_LocalAddr] <= 250))
    {
        addr1 = (u8)gSysHigParaRO_101[eSHWPara101_LocalAddr];
    }
    else
    {
        addr1 = 70;
        gSysHigParaRO_101[eSHWPara101_LocalAddr] = addr1;
    }
    
    //监控端口站地址
    if((1 <= gSysHigParaRO_101[eSHWPara101_MonitAddr]) && (gSysHigParaRO_101[eSHWPara101_MonitAddr] <= 250))
    {
        addr2 = (u8)gSysHigParaRO_101[eSHWPara101_MonitAddr];
    }
    else
    {
        addr2 = 100;
        gSysHigParaRO_101[eSHWPara101_MonitAddr] = addr2;
    }
    
    //远程端口波特率
    if((9600 == gSysHigParaRO_101[eSHWPara101_RemoteRate]) || (19200  == gSysHigParaRO_101[eSHWPara101_RemoteRate]) || (38400  == gSysHigParaRO_101[eSHWPara101_RemoteRate]))
    {
        bps0 = gSysHigParaRO_101[eSHWPara101_RemoteRate];
    }
    else
    {
        bps0 = SCI0_BPS;
        gSysHigParaRO_101[eSHWPara101_RemoteRate] = bps0;
    }
    
    //就地端口波特率
    if((9600 == gSysHigParaRO_101[eSHWPara101_LocalRate]) || (19200  == gSysHigParaRO_101[eSHWPara101_LocalRate]) || (38400  == gSysHigParaRO_101[eSHWPara101_LocalRate]))
    {
        bps1 = gSysHigParaRO_101[eSHWPara101_LocalRate];
    }
    else
    {
        bps1 = SCI1_BPS;
        gSysHigParaRO_101[eSHWPara101_LocalRate] = bps1;
    }
    
    //监控端口波特率
    if((9600 == gSysHigParaRO_101[eSHWPara101_MonitRate]) || (19200  == gSysHigParaRO_101[eSHWPara101_MonitRate]) || (38400  == gSysHigParaRO_101[eSHWPara101_MonitRate]))
    {
        bps2 = gSysHigParaRO_101[eSHWPara101_MonitRate];
    }
    else
    {
        bps2 = SCI2_BPS;
        gSysHigParaRO_101[eSHWPara101_MonitRate] = bps2;
    }
    
    #if(MODBUS_LIB_EN == 1)
    //远程端口Modbus初始化和串口复位
    ModbusStationAllInit(eSCI0, bps0, eModbus_Server, addr0);
    (void)DEVSCIResetInit(eSCI0);
    
    //就地端口Modbus初始化和串口复位
    ModbusStationAllInit(eSCI1, bps1, eModbus_Server, addr1);
    (void)DEVSCIResetInit(eSCI1);
    
    //监控端口Modbus初始化和串口复位
    #if((0 == PRJ_PARA_NUM_INFO))//测试工装配置为服务端
    ModbusStationAllInit(eSCI2, bps2, eModbus_Server, addr2);
    #else//正常程序为客户端
    ModbusStationAllInit(eSCI2, bps2, eModbus_Client, 0xff);
    #endif
    (void)DEVSCIResetInit(eSCI2);
    #endif
}

/*=================================================================================================
** @Name      : void MbRcvMsgCountAdd(u8 sciNum)
** @Input     : sciNum:SCI端口号
** @Output    : void
** @Function  : 接收到RS485信息计数自加
** @The notes :
**===============================================================================================*/
void MbRcvMsgCountAdd(u8 sciNum)
{
    if(sciNum >= eSCINUM)
    {
        return;
    }
    
    sMbRcvMsgCount[sciNum]++;
    
    if(sMbRcvMsgCount[sciNum] > 200)    //防止越界
    {
        sMbRcvMsgCount[sciNum] = 200;
    }
}

/*=================================================================================================
** @Name      : void MbCheckRcvOffResetTask(void)
** @Input     : void
** @Output    : void
** @Function  : 检测RS485通讯中断复位任务
** @The notes : 2s
**===============================================================================================*/
void MbCheckRcvOffResetTask(void)
{
    u8 i = 0;
    static u8 sRcvFlag = 0;
    static u8 sOffTime[eSCINUM] = {0};
    static u8 sResetTime[eSCINUM] = {0};
    
    //未开始接收信息不检测
    for(i = 0; i < eSCINUM; i++)
    {
        //接收到信息则记录接收标志
        if(sMbRcvMsgCount[i] > 0)
        {
            BitSet(sRcvFlag, i);
        }
        
        //已记录接收到信息
        if(BitGet(sRcvFlag, i) > 0)
        {
            //未接收到信息
            if(0 == sMbRcvMsgCount[i])
            {
                if(sOffTime[i] >= 15)           //超时30s
                {
                    if(sResetTime[i] < 3)       //未复位3次
                    {
                        MbCommuResetInit(i);    //通讯口初始化复位
                        sResetTime[i]++;        //复位次数计数
                    }
                    sOffTime[i] = 0;            //清除通讯中断计时
                }
                else
                {
                    sOffTime[i]++;              //通讯中断计时
                }
            }
            else                                //接收到信息
            {
                sOffTime[i] = 0;                //清除通讯中断计时
                sResetTime[i] = 0;              //清除复位次数
                sMbRcvMsgCount[i] = 0;          //清除信息计数
            }
        }
        //未接收过信息
        else
        {
            sOffTime[i] = 0;                    //清除通讯中断计时
            sResetTime[i] = 0;                  //清除复位次数
        }
    }
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void MbCommuResetInit(u8 sciNum)
** @Input     : sciNum:SCI端口号
** @Output    : void
** @Function  : modbus通讯复位初始化
** @The notes :
**===============================================================================================*/
static void MbCommuResetInit(u8 sciNum)
{
    u8 addr = 0;
    u16 bps = 0;
    
    //远程端口
    if(eSCI0 == sciNum)
    {
        //远程口站地址
        addr = (u8)gSysHigParaRO_101[eSHWPara101_RemoteAddr];
        
        //远程口波特率
        bps = gSysHigParaRO_101[eSHWPara101_RemoteRate];
    }
    //就地口
    else if(eSCI1 == sciNum)
    {
        //就地口站地址
        addr = (u8)gSysHigParaRO_101[eSHWPara101_LocalAddr];
        
        //就地口波特率
        bps = gSysHigParaRO_101[eSHWPara101_LocalRate];
    }
    //监控口
    else if(eSCI1 == sciNum)
    {
        //监控口站地址
        addr = (u8)gSysHigParaRO_101[eSHWPara101_MonitAddr];

        //监控口波特率
        bps = gSysHigParaRO_101[eSHWPara101_MonitRate];
    }
    else
    {
        return;
    }
    
    #if(MODBUS_LIB_EN == 1)
    ModbusStationAllInit(sciNum, bps, eModbus_Server, addr);
    (void)DEVSCIResetInit((e_SCINUM)sciNum);
    #endif
}
