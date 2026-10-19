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
#include "BSPUserAPI.h"
#include "CANTabOper.h"
#include "CANInterface.h"
#include "CANTxCycle.h"
#include "CANUser.h"
#include "CANHandle.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
u32 gCANCountTime = 0;                                              //CAN通信包总计时(1ms)

extern t_CANDevTab gCanDevTable[];                                  //CAN通信设备表
extern t_CANTxRcvTab gCanTxRcvTable[];                              //CAN通信收发处理表

extern u16 gCANVerTime_0[];                                         //校时信息
extern u16 gCANDevInfo_1[];                                         //设备信息
extern u8 gCANBaseDef_3[];                                          //通信包基本配置信息
extern u8 gCANRoutDef_4[];                                          //路由信息
extern u8 gCANTxVerDef_10[];                                        //校时配置表
extern u8 gCANTxBreDef_11[];                                        //心跳配置表
extern u8 gCANTxMulDef_12[];                                        //主动更新长帧配置表
extern u8 gCANTxSinDef_13[];                                        //主动更新单帧配置表
extern u8 gCANTxCelDef_14[];                                        //主动更新单体配置表
extern u8 gCANTxParDef_15[];                                        //主动更新参数配置表

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void CANTxRcvTabWaitHandle(u8 id)
** @Input     : id:通信表序号
** @Output    : void
** @Function  : 通信收发表等待状态处理函数
** @The notes : 通讯表状态为等待1ms时定时调用
**===============================================================================================*/
static void CANTxRcvTabWaitHandle(u8 id);

/*=================================================================================================
** @Name      : static u8 CANRcvOtheRoutMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 接收其它设备报文路由处理
** @The notes : 转发非自身设备报文和广播报文
**===============================================================================================*/
static u8 CANRcvOtheRoutMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvVerTimeMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : *canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 校时帧接收处理
** @The notes : 更新校时数据和调用用户HOOK
**===============================================================================================*/
static u8 CANRcvVerTimeMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvBreathMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 心跳帧接收处理
** @The notes : 调用用户HOOK
**===============================================================================================*/
static u8 CANRcvBreathMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvReqUpdateMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 请求修改帧接收处理
** @The notes : 更新修改域数据和调用用户HOOK
**===============================================================================================*/
static u8 CANRcvReqUpdateMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvAnsUpdateMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 修改回复帧接收处理
** @The notes : 更新修改域数据和调用用户HOOK
**===============================================================================================*/
static u8 CANRcvAnsUpdateMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvConUpdateMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 修改确认帧接收处理
** @The notes : 调用用户HOOK
**===============================================================================================*/
static u8 CANRcvConUpdateMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvReqReadMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 请求查询帧接收处理
** @The notes : 调用用户HOOK更新数据域和回复
**===============================================================================================*/
static u8 CANRcvReqReadMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvAnsReadMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 查询回复帧接收处理
** @The notes : 更新查询得到的数据和确认查询
**===============================================================================================*/
static u8 CANRcvAnsReadMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvConReadMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 查询确认帧接收处理
** @The notes : 确认是否正确接收和发送下一帧查询回复
**===============================================================================================*/
static u8 CANRcvConReadMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Name      : static u8 CANRcvCycUpMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 周期主动更新帧接收处理
** @The notes : 更新接收得到的数据到索引表
**===============================================================================================*/
static u8 CANRcvCycUpMsgDeal(t_CANFram canFram, u8 canNum);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CANDevTabEntryInit_API(u8 devAddr, u8 mIndexNum const s_CANIndexTab  *pIndexTab, u8 devId)
** @Input     : devAdd:设备地址 mIndexNum:主索引数目 pIndexTab:设备主动引表 devId:创建设备表序号
** @Output    : void
** @Function  : CAN通信创建设备表初始化
** @The notes : 用户配置CAN通信设备表信息
**===============================================================================================*/
void CANDevTabEntryInit_API(u8 devAddr, u8 mIndexNum, const t_CANIndexTab *pIndexTab, u8 devId)
{
    /*创建设备表*/
    if(devId < gCANBaseDef_3[eCANRBas3_DevNum])
    {
        gCanDevTable[devId].devAdrr = devAddr;
        gCanDevTable[devId].mIndexNum = mIndexNum;
        gCanDevTable[devId].pIndexTable = pIndexTab;
    }
}

/*=================================================================================================
** @Name      : u8 CANLibRcvMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : CAN通信报文接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
u8 CANLibRcvMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
    u8 i = 0;
    u8 result = 0;
    u_CANId canID = {0};
    t_CANFram canFram = {0};

    /*需要屏蔽该报文不处理*/
    if(TRUE == CANRcvShieldDeal_Hook(ID, canNum))
    {
        return(eCANERR_NOSUCH_DEV);
    }

    /*解析CAN帧*/
    canID.Word = ID;
    canFram.ID = ID;
    canFram.DLC = DLC;
    for(i = 0; i < DLC; i++)
    {
        canFram.Data[i] = pdata[i];
    }

    /*非自身报文*/
    if(gCANDevInfo_1[eCANDev1_Addr] != canID.Bits.daddr)
    {
        /*非广播报文*/
        if((0xff == canID.Bits.daddr) && ((canID.Bits.saddr < 70) || (canID.Bits.saddr >= 110)))
        {
             //return(eCANERR_NOSUCH_DEV);
        }
        //查找路由表,进行转发处理
        (void)CANRcvOtheRoutMsgDeal(canFram, canNum);

        /*非广播报文*/
        if(0xff != canID.Bits.daddr)
        {
             return(eCANERR_NOSUCH_DEV);
        }
    }

    /*根据CAN ID功能码处理报文*/
    switch(canID.Bits.funcode)
    {
        /*校时信息报文*/
        case eCANFUN_VFT:
            result = CANRcvVerTimeMsgDeal(canFram, canNum);
            break;

        /*心跳信息报文*/
        case eCANFUN_BRE:
            result = CANRcvBreathMsgDeal(canFram, canNum);
            break;

        /*请求修改命令报文*/
        case eCANFUN_RUP:
            result = CANRcvReqUpdateMsgDeal(canFram, canNum);
            break;

        /*请求修改回复报文*/
        case eCANFUN_AUP:
            result = CANRcvAnsUpdateMsgDeal(canFram, canNum);
            break;

        /*请求修改确认报文*/
        case eCANFUN_CUP:
            result = CANRcvConUpdateMsgDeal(canFram, canNum);
            break;

        /*请求查询命令报文*/
        case eCANFUN_REQ:
            result = CANRcvReqReadMsgDeal(canFram, canNum);
            break;

        /*请求查询回复报文*/
        case eCANFUN_ARQ:
            result = CANRcvAnsReadMsgDeal(canFram, canNum);
            break;

        /*查询回复确认报文*/
        case eCANFUN_CRQ:
            result = CANRcvConReadMsgDeal(canFram, canNum);
            break;

        /*主动更新(长帧)报文*/
        case eCANFUN_UPM:
        /*主动发送(单帧)报文*/
        case eCANFUN_UPS:
        /*主动发送(单体)报文*/
        case eCANFUN_UPC:
        /*主动发送(参数)报文*/
        case eCANFUN_UPP:
            result = CANRcvCycUpMsgDeal(canFram, canNum);
            break;

        /*在线升级报文
        case eCANFUN_LOR:
        case eCANFUN_LOA:
        case eCANFUN_LOW:
            result = eCANERR_KEEP_FUN;
            break;*/

        default:
            result = eCANERR_KEEP_FUN;
            break;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void CANLibHandleTask_API(void)
** @Input     : void
** @Output    : void
** @Function  : CAN所有处理任务
** @The notes : 用户必须5ms周期调用
**===============================================================================================*/
void CANLibHandleTask_API(void)
{
    static u8 sTimer = 0;

    /*10ms任务*/
    switch(sTimer)
    {
        /*用户配置周期发送信息*/
        case 0:
            CANTxCycleTabHandle();
            sTimer = 1;
            break;

        /*通信包自身周期发送信息(触发类)*/
        case 1:
            CANLibCycInfoTxHandle();
            sTimer = 0;
            break;

        default:
            sTimer = 0;
            break;
    }
}

/*=================================================================================================
** @Name      : void CANLibTxRcvHandleTask_API(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信报文收发处理任务(收发通信表处理)
** @The notes : 用户必须1ms周期调用
**===============================================================================================*/
void CANLibTxRcvHandleTask_API(void)
{
    u8 i = 0;

    /*未创建通信收发表*/
    if(gCanTxRcvTable == NULL)
    {
        return;
    }

    /*通信包总计时*/
    gCANCountTime++;
    if(gCANCountTime >= CAN_MAX_COUNT_TIME)
    {
        gCANCountTime = gCANCountTime - CAN_MAX_COUNT_TIME;
    }

    /*检测各个通信收发表状态*/
    for(i = 0; i < gCANBaseDef_3[eCANRBas3_TRTabNum]; i++)
    {
        /*准备就绪,发送信息*/
        if(eCANTAB_READYOK == gCanTxRcvTable[i].ready)
        {
            /*发送通信包信息*/
            (void)CANSendTxRcvTabMsgPack(i);

            /*等待下一周期*/
            gCanTxRcvTable[i].ready = eCANTAB_WAIT;
        }
        /*等待处理,计时*/
        else if(eCANTAB_WAIT == gCanTxRcvTable[i].ready)
        {
            /*发送周期计时等待*/
            CANTxRcvTabWaitHandle(i);
        }
    }
}

/*=================================================================================================
** @Name      : u8 CANBuildSendVerMsg_API(u8 canNum)
** @Input     : canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送校时报文帧
** @The notes : 用户自行发送校时帧
**===============================================================================================*/
u8 CANBuildSendVerMsg_API(u8 canNum)
{
    /*取校时数组时间发送*/
    u32 ID = 0;
    u8 data[8] = {0};
    u8 errcode = eCANERR_SUCCESSFUL;
    const t_CANIndexTab *pindextable;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*查找自身设备校时索引0是否存在*/
    pindextable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], 0, 0, 4, &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*主索引0和广播外网报文*/
    ID = CANBuildMsgID_API(eCANFUN_VFT, 0, 0xff, 0);

    /*从校时索引0取出时间并发送*/
    CANReadCanIndexData(pindextable, 0, 4, data);
    CANSendMsgUser_Hook(ID, data, 8, canNum);

    return(errcode);
}

/*=================================================================================================
** @Name      : u8 CANBuildSendBreMsg_API(u8 canNum)
** @Input     : canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送心跳报文帧
** @The notes : 用户自行发送心跳帧
**===============================================================================================*/
u8 CANBuildSendBreMsg_API(u8 canNum)
{
    /*取校时数组时间发送*/
    u32 ID = 0;
    u8 data[8] = {0};
    static u8 sTime[CAN_MAX_CAN_NUM] = {0};

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*主索引1和广播内网报文*/
    ID = CANBuildMsgID_API(eCANFUN_BRE, 1, 0xff, 1);

    /*构建心跳帧数据域和发送*/
    data[0] = sTime[canNum];
    data[1] = (gCANDevInfo_1[eCANDev1_HW] >> 8) & 0xff;
    data[2] = gCANDevInfo_1[eCANDev1_ID] & 0xff;
    data[3] = (gCANDevInfo_1[eCANDev1_ID] >> 8) & 0xff;
    data[4] = gCANDevInfo_1[eCANDev1_SWL] & 0xff;
    data[5] = (gCANDevInfo_1[eCANDev1_SWL] >> 8) & 0xff;
    data[6] = gCANDevInfo_1[eCANDev1_SWH] & 0xff;
    data[7] = (gCANDevInfo_1[eCANDev1_SWH] >> 8) & 0xff;

    CANSendMsgUser_Hook(ID, data, 8, canNum);

    sTime[canNum]++;

    return(eCANERR_SUCCESSFUL);
}

/*=================================================================================================
** @Name      : u8 CANBuildSendRupMsg_API(u8 destAddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum)
** @Input     : destAddr:目标设备地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送请求修改报文帧
** @The notes : 用户自行发送请求修改报文帧
**===============================================================================================*/
u8 CANBuildSendRupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(destAddr, mIndex, sIndex, sIndexNum, &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*申请空闲收发表进行发送(需要接收回复处理)*/
    return(CANReqIdleTxRcvTab(eCANFUN_RUP, attrib, pindextable->pObject, pindextable->dataType, mIndex, sIndex, sIndexNum, conFlag, destAddr, canNum));
}

/*=================================================================================================
** @Name      : u8 CANBuildSendAupMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送修改回复报文帧
** @The notes : 用户自行发送修改回复报文帧
**===============================================================================================*/
u8 CANBuildSendAupMsg_API(t_CANFram *canFram, u8 canNum)
{
    /*取已写入的主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;
    u_CANId canID = {0};
    u32 ID = 0;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*取接收到的ID*/
    canID.Word = canFram->ID;

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram->Data[1], (canFram->Data[0] & 0x0f), &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*重新组合ID(接收报文源地址变目标地址,下行变上行)*/
    ID = CANBuildMsgID_API(eCANFUN_AUP, canID.Bits.mindex, canID.Bits.saddr, (canID.Bits.attrib^1));

    /*重新读取当前实际值*/
    CANReadCanIndexData(pindextable, canFram->Data[1], (canFram->Data[0] & 0x0f), &canFram->Data[2]);

    /*发送回复报文*/
    CANSendMsgUser_Hook(ID, canFram->Data, (((canFram->Data[0] & 0x0f) * (pindextable->dataType)) + 2), canNum);

    return(errcode);
}

/*=================================================================================================
** @Name      : u8 CANBuildSendCupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标设备地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送修改确认报文帧
** @The notes : 用户自行发送修改确认报文帧
**===============================================================================================*/
u8 CANBuildSendCupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;
    u_CANCmdByte canCmd = {0};
    u8 data[3] = {0};
    u32 ID = 0;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(destAddr, mIndex, sIndex, sIndexNum, &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*组合修改确认ID*/
    ID = CANBuildMsgID_API(eCANFUN_CUP, mIndex, destAddr, attrib);
    canCmd.Bits.CON = 1;

    /*构建心跳帧数据域和发送*/
    data[0] = canCmd.Byte;
    data[1] = sIndex;
    data[2] = sIndexNum;

    /*发送确认报文*/
    CANSendMsgUser_Hook(ID, data, 3, canNum);

    return(errcode);
}

/*=================================================================================================
** @Name      : u8 CANBuildSendReqMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送请求查询报文帧
** @The notes : 用户自行发送请求查询报文帧
**===============================================================================================*/
u8 CANBuildSendReqMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(destAddr, mIndex, sIndex, sIndexNum, &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*申请空闲收发表进行发送(需要接收回复处理)*/
    return(CANReqIdleTxRcvTab(eCANFUN_REQ, attrib, pindextable->pObject, pindextable->dataType, mIndex, sIndex, sIndexNum, conFlag, destAddr, canNum));
}

/*=================================================================================================
** @Name      : u8 CANBuildSendArqMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送查询回复报文帧
** @The notes : 用户自行发送查询回复报文帧
**===============================================================================================*/
u8 CANBuildSendArqMsg_API(t_CANFram *canFram, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;
    u_CANCmdByte canCmd = {0};
    u_CANId canID = {0};

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*取接收到的ID和命令字*/
    canID.Word = canFram->ID;
    canCmd.Byte = canFram->Data[0];

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram->Data[1], canFram->Data[2], &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*申请空闲收发表进行发送(多帧处理)*/
    return(CANReqIdleTxRcvTab(eCANFUN_ARQ, (canID.Bits.attrib^1), pindextable->pObject, pindextable->dataType, canID.Bits.mindex, canFram->Data[1], canFram->Data[2], canCmd.Bits.CON, canID.Bits.saddr, canNum));
}

/*=================================================================================================
** @Name      : u8 CANBuildSendCrqMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送查询确认报文帧
** @The notes : 用户自行发送查询确认报文帧
**===============================================================================================*/
u8 CANBuildSendCrqMsg_API(t_CANFram *canFram, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;
    u_CANId canID = {0};
    u32 ID = 0;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*取接收到的ID*/
    canID.Word = canFram->ID;

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram->Data[1], canFram->Data[2], &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*重新组合ID(接收报文源地址变目标地址)*/
    ID = CANBuildMsgID_API(eCANFUN_CRQ, canID.Bits.mindex, canID.Bits.saddr, (canID.Bits.attrib^1));

    /*重新读取当前实际值*/
    CANReadCanIndexData(pindextable, canFram->Data[1], (canFram->Data[0] & 0x0f), &canFram->Data[2]);

    /*发送回复报文*/
    CANSendMsgUser_Hook(ID, canFram->Data, (((canFram->Data[0] & 0x0f) * (pindextable->dataType)) + 2), canNum);

    return(errcode);
}

/*=================================================================================================
** @Name      : u8 CANBuildSendUpmMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目  canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新长帧报文帧
** @The notes : 用户自行发送主动更新长帧报文帧
**===============================================================================================*/
u8 CANBuildSendUpmMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*已禁止周期发送*/
    if(0 == gCANTxMulDef_12[0])
    {
        return(eCANERR_CYCUP_DIS);
    }

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], mIndex, sIndex, sIndexNum, &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*申请空闲收发表进行发送(多帧处理)*/
    return(CANReqIdleTxRcvTab(eCANFUN_UPM, attrib, pindextable->pObject, pindextable->dataType, mIndex, sIndex, sIndexNum, 0, destAddr, canNum));
}

/*=================================================================================================
** @Name      : u8 CANBuildSendUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新单帧报文帧
** @The notes : 用户自行发送主动更新单帧报文帧
**===============================================================================================*/
u8 CANBuildSendUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*已禁止周期发送*/
    if(0 == gCANTxSinDef_13[0])
    {
        return(eCANERR_CYCUP_DIS);
    }

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], mIndex, sIndex, sIndexNum, &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*申请空闲收发表进行发送(单帧处理)*/
    return(CANReqIdleTxRcvTab(eCANFUN_UPS, attrib, pindextable->pObject, pindextable->dataType, mIndex, sIndex, sIndexNum, 0, destAddr, canNum));
}

/*=================================================================================================
** @Name      : u8 CANBuildSendUpcMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 mapIndex:映射起始主索引号 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新单体报文帧
** @The notes : 用户自行发送主动更新单体报文帧
**===============================================================================================*/
u8 CANBuildSendUpcMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*已禁止周期发送*/
    if(0 == gCANTxCelDef_14[0])
    {
        return(eCANERR_CYCUP_DIS);
    }

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], mIndex, sIndex, sIndexNum, &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*申请空闲收发表进行发送(单体帧处理)*/
    return(CANReqIdleTxRcvTab(eCANFUN_UPC, attrib, pindextable->pObject, pindextable->dataType, mIndex, sIndex, sIndexNum, mapIndex, destAddr, canNum));
}

/*=================================================================================================
** @Name      : u8 CANBuildSendUppMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 mapIndex:映射起始主索引号 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新参数报文帧
** @The notes : 用户自行发送主动更新参数报文帧
**===============================================================================================*/
u8 CANBuildSendUppMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum)
{
    /*取相应设备主索引数组数值发送*/
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*已禁止周期发送*/
    if(0 == gCANTxParDef_15[0])
    {
        return(eCANERR_CYCUP_DIS);
    }

    /*查找目标设备主索引是否存在且判断子索引是否越界*/
    pindextable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], mIndex, sIndex, sIndexNum, &errcode);
    if(pindextable == NULL)
    {
        return(errcode);
    }

    /*申请空闲收发表进行发送(参数帧处理)*/
    return(CANReqIdleTxRcvTab(eCANFUN_UPP, attrib, pindextable->pObject, pindextable->dataType, mIndex, sIndex, sIndexNum, mapIndex, destAddr, canNum));
}

/*=================================================================================================
** @Name      : u8 CANBuildSendErrMsg_API(u8 errReas, t_CANFram *canFram, u8 canNum)
** @Input     : errReas:错误原因 *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送错误应答报文帧
** @The notes : 请求查询和请求修改命令错误时应答使用
**===============================================================================================*/
u8 CANBuildSendErrMsg_API(u8 errReas, t_CANFram *canFram, u8 canNum)
{
    /*将收到的报文修改成错误应答功能码发送*/
    u_CANId canID = {0};
    u32 ID = 0;

    /*CAN通道最大支持4路*/
    if(canNum >= CAN_MAX_CAN_NUM)
    {
        return(eCANERR_NOSUCH_CAN);
    }

    /*取接收到的ID*/
    canID.Word = canFram->ID;

    /*重新组合ID(接收报文源地址变目标地址)*/
    ID = CANBuildMsgID_API(eCANFUN_ERR, canID.Bits.mindex, canID.Bits.saddr, (canID.Bits.attrib^1));

    /*记录错误码*/
    canFram->Data[0] = (canFram->Data[0] & 0x0f) | ((errReas & 0x0f) << 4);

    /*发送回复报文*/
    CANSendMsgUser_Hook(ID, canFram->Data, canFram->DLC, canNum);

    /*索引错误*/
    if(eCANERRREAS_INDEXERR == errReas)
    {
        return(eCANERR_NOSUCH_MID);
    }
    /*CRC错误*/
    else if(eCANERRREAS_CRCERR == errReas)
    {
        return(eCANERR_MSGCRC_ERR);
    }
    /*权限非法*/
    else if(eCANERRREAS_AUTHERR == errReas)
    {
        return(eCANERR_CANPW_DIS);
    }
    /*状态非法*/
    /*参数非法*/
    else
    {
        return(eCANERR_RUPW_FAIL);
    }
}

/*=================================================================================================
** @Name      : u32 CANBuildMsgID_API(e_CANFunCode funCode, u8 mIndex, u8 destAddr, u8 attrib)
** @Input     : funCode:功能码 mIndex:主索引号 destAddr:目标地址 attrib:帧属性
** @Output    : 32位CAN通信帧ID值
** @Function  : 组建32位CAN通信帧ID值
** @The notes :
**===============================================================================================*/
u32 CANBuildMsgID_API(e_CANFunCode funCode, u8 mIndex, u8 destAddr, u8 attrib)
{
    /*功能码4bit|帧属性1bit|主索引8bit|目标地址8bit|源地址8bit*/
    u_CANId ID;

    ID.Word = 0;
    ID.Bits.rsv = 0;
    ID.Bits.funcode = (e_CANFunCode)(funCode & 0x0F);
    ID.Bits.attrib = attrib & 0x01;
    ID.Bits.mindex = mIndex;
    ID.Bits.daddr = destAddr;
    ID.Bits.saddr = gCANDevInfo_1[eCANDev1_Addr];

    return(ID.Word);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void CANTxRcvTabWaitHandle(u8 id)
** @Input     : id:通信表序号
** @Output    : void
** @Function  : 通信收发表等待状态处理函数
** @The notes : 通讯表状态为等待1ms时定时调用
**===============================================================================================*/
static void CANTxRcvTabWaitHandle(u8 id)
{
    /*通信收发表序号越界*/
    if(id >= gCANBaseDef_3[eCANRBas3_TRTabNum])
    {
        return;
    }

    switch(gCanTxRcvTable[id].funcode)
    {
        /*主动更新长帧间隔控制*/
        case eCANFUN_UPM:
            /*多帧帧间间隔时间检测*/
            CANMulFramTxTimerCheck(id, CAN_TX_UPM_SPA_T);
            break;

        /*主动更新单体帧间隔控制*/
        case eCANFUN_UPC:
            /*单体帧帧间间隔时间检测*/
            CANMulFramTxTimerCheck(id, CAN_TX_UPC_SPA_T);
            break;

        /*主动更新参数帧间隔控制*/
        case eCANFUN_UPP:
            /*参数帧帧间间隔时间检测*/
            CANMulFramTxTimerCheck(id, CAN_TX_UPP_SPA_T);
            break;

        /*回复查询多帧间隔控制*/
        case eCANFUN_ARQ:
            /*不需确认的请求查询则定时回复查询多帧间隔控制*/
            if(gCanTxRcvTable[id].cmd.Bits.CON == 0)
            {
                /*多帧帧间间隔时间检测*/
                CANMulFramTxTimerCheck(id, CAN_TX_ARQ_SPA_T);
            }
            /*需确认的请求查询则等待查询确认命令超时检测*/
            else
            {
                /*等待重新确认超时检测*/
                CANWaitAnsFramTimerCheck(id, CAN_TX_CRQ_OVR_T, CAN_TX_RET_OVR_T);
            }
            break;

        /*请求修改重发间隔控制*/
        case eCANFUN_RUP:
            /*等待修改回复超时检测*/
            CANWaitAnsFramTimerCheck(id, CAN_TX_RUP_OVR_T, CAN_TX_RET_OVR_T);
            break;

        /*请求查询重发间隔控制*/
        case eCANFUN_REQ:
            /*等待查询回复超时检测*/
            CANWaitAnsFramTimerCheck(id, CAN_TX_REQ_OVR_T, CAN_TX_RET_OVR_T);
            break;

        /*其它功能码报文直接清除收发通信表*/
        default:
            CANClrTxRcvTabInfo(id);
            break;
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvOtheRoutMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 接收其它设备报文路由处理
** @The notes : 转发非自身设备报文和广播报文
**===============================================================================================*/
static u8 CANRcvOtheRoutMsgDeal(t_CANFram canFram, u8 canNum)
{
    u8 i = 0;
    u8 j = 0;
    u8 temp = 0;

    /*未使能路由转发或无路由表*/
    if((0 == gCANRoutDef_4[eCANRout4_En]) || (0 == gCANRoutDef_4[eCANRout4_Num]))
    {
        return(eCANERR_NOSUCH_DEV);
    }

    /*计算第1组路由表索引*/
    temp = eCANRout4_Num + 1;

    /*查找所有路由组*/
    for(i = 0; i < gCANRoutDef_4[eCANRout4_Num]; i++)
    {
        /*接收CAN通道号匹配需要路由的通道号,则行路由转发处理*/
        if(canNum == gCANRoutDef_4[temp])
        {
            /*根据该CAN通道号的路由模式进行处理*/
            for(j = 0; j < gCANRoutDef_4[temp + 1]; j++)
            {
               /*将报文转发到需要转发的口*/
                CANSendMsgUser_Hook(canFram.ID, canFram.Data, canFram.DLC, (gCANRoutDef_4[temp + 2 + j]));
            }
        }

        /*计算下一组路由表索引*/
        temp = gCANRoutDef_4[temp + 1] + temp + 2;
    }

    return(eCANERR_NOSUCH_DEV);
}

/*=================================================================================================
** @Name      : static u8 CANRcvVerTimeMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 校时帧接收处理
** @The notes : 更新校时数据和调用用户HOOK
**===============================================================================================*/
static u8 CANRcvVerTimeMsgDeal(t_CANFram canFram, u8 canNum)
{
    u_CANId canID = {0};

    /*解析帧ID*/
    canID.Word = canFram.ID;

    /*非校时报文(只能通过校时功能码修改)*/
    if((eCANFUN_VFT != canID.Bits.funcode) || (0 != canID.Bits.mindex))
    {
        return(eCANERR_NOSUCH_MID);
    }

    /*数据域有效*/
    if(canFram.DLC >= 8)
    {
        /*解析接收到的时间*/
        gCANVerTime_0[eCANTim0_Y] = ((u16)canFram.Data[1] << 8) | (u16)canFram.Data[0];
        gCANVerTime_0[eCANTim0_MD] = ((u16)canFram.Data[3] << 8) | (u16)canFram.Data[2];
        gCANVerTime_0[eCANTim0_HM] = ((u16)canFram.Data[5] << 8) | (u16)canFram.Data[4];
        gCANVerTime_0[eCANTim0_MS] = ((u16)canFram.Data[7] << 8) | (u16)canFram.Data[6];

        /*校时报文用户处理*/
        CANRcvVerUserDeal_Hook(canID.Bits.saddr, canNum);

        return(eCANERR_SUCCESSFUL);
    }
    else
    {
        return(eCANERR_RCVMSG_ERR);
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvBreathMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 心跳帧接收处理
** @The notes : 调用用户HOOK
**===============================================================================================*/
static u8 CANRcvBreathMsgDeal(t_CANFram canFram, u8 canNum)
{
    u_CANId canID = {0};

    /*解析帧ID*/
    canID.Word = canFram.ID;

    /*非心跳报文*/
    if((eCANFUN_BRE != canID.Bits.funcode) || (1 != canID.Bits.mindex))
    {
        return(eCANERR_NOSUCH_MID);
    }

    /*数据域有效*/
    if(canFram.DLC >= 8)
    {
        /*心跳报文用户处理*/
        CANRcvBreUserDeal_Hook(canID.Bits.saddr, canNum);

        return(eCANERR_SUCCESSFUL);
    }
    else
    {
        return(eCANERR_RCVMSG_ERR);
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvReqUpdateMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 请求修改帧接收处理
** @The notes : 更新修改域数据和调用用户HOOK
**===============================================================================================*/
static u8 CANRcvReqUpdateMsgDeal(t_CANFram canFram, u8 canNum)
{
    u_CANId canID = {0};
    u_CANCmdByte canCmd = {0};
    const t_CANIndexTab *pindextable;
    u8 wrResult = eCANERRREAS_NORMAL;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*解析帧ID*/
    canID.Word = canFram.ID;

    /*非请求修改报文*/
    if(eCANFUN_RUP != canID.Bits.funcode)
    {
        return(eCANERR_KEEP_FUN);
    }

    canCmd.Byte = canFram.Data[0];

    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram.Data[1], canCmd.Bits.NUM, &errcode);

    /*主索引表信息存在*/
    if(pindextable != NULL)
    {
        /*CAN报文长度必须与索引长度匹配
        if(canFram.DLC != (2 + (canCmd.Bits.NUM * pindextable->dataType)))*/
        /*CAN报文长度不能小于索引长度*/
        if(canFram.DLC < (2 + (canCmd.Bits.NUM * pindextable->dataType)))
        {
            /*子索引越界*/
            return(eCANERR_SIDLEN_OVE);
        }

        /*帧长度为8但数据长度不满6时需要校验CRC校验码*/
        if((canFram.DLC >= 8) && ((canCmd.Bits.NUM * pindextable->dataType) < 6))
        {
            /*接收的CRC与实际计算CRC不一致*/
            //if(canFram.Data[7] != CANComputeRequestCRC8((canFram.ID & 0xffff), &canFram.Data[0], 7))
            {
                /*发送错误应答报文(CRC错误)*/
                //(void)CANBuildSendErrMsg_API(eCANERRREAS_CRCERR, &canFram, canNum);

                /*CRC错误*/
                //return(eCANERR_MSGCRC_ERR);
            }
        }

        /*写子索引数据,更新通信索引表*/
        (void)CANWriteCanIndexData(pindextable, canFram.Data[1], canCmd.Bits.NUM, &canFram.Data[2], canNum);

        /*不需要修改确认*/
        if(0 == canCmd.Bits.CON)
        {
            /*最后一帧执行处理函数*/
            if(1 == canCmd.Bits.FIN)
            {
                /*请求修改后处理函数*/
                wrResult = CANRcvRupUserDeal_Hook(canID.Bits.saddr, canID.Bits.mindex, canFram.Data[1], canCmd.Bits.NUM, canNum);
            }
        }

        /*请求修改正常*/
        if(eCANERRREAS_NORMAL == wrResult)
        {
            /*生成修改回复信息*/
            return(CANBuildSendAupMsg_API(&canFram, canNum));
        }
        else
        {
            /*发送错误应答报文(权限或参数错误)*/
            return(CANBuildSendErrMsg_API(wrResult, &canFram, canNum));
        }
    }
    /*主索引表信息不存在*/
    else
    {
        /*发送错误应答报文(索引错误)*/
        (void)CANBuildSendErrMsg_API(eCANERRREAS_INDEXERR, &canFram, canNum);

        return(errcode);
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvAnsUpdateMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 修改回复帧接收处理
** @The notes : 对比数据是否修改成功和确认修改
**===============================================================================================*/
static u8 CANRcvAnsUpdateMsgDeal(t_CANFram canFram, u8 canNum)
{
    u8 i = {0};
    u8 matIndex = 0;
    u8 data[6] = {0};
    u_CANId canID = {0};
    u_CANCmdByte canCmd = {0};
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*解析帧ID*/
    canID.Word = canFram.ID;

    /*非修改回复报文*/
    if(eCANFUN_AUP != canID.Bits.funcode)
    {
        return(eCANERR_KEEP_FUN);
    }

    canCmd.Byte = canFram.Data[0];

    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram.Data[1], canCmd.Bits.NUM, &errcode);

    /*主索引表信息存在*/
    if(pindextable != NULL)
    {
        /*CAN报文长度必须与索引长度匹配
        if(canFram.DLC != (2 + (canCmd.Bits.NUM * pindextable->dataType)))*/
        /*CAN报文长度不能小于索引长度*/
        if(canFram.DLC < (2 + (canCmd.Bits.NUM * pindextable->dataType)))
        {
            /*子索引越界*/
            return(eCANERR_SIDLEN_OVE);
        }

        /*查找CAN收发通信表是否正在处理该报文(发送请求修改命令)*/
        matIndex = CANFindTxRcvTabMatching(canID.Bits.saddr, eCANFUN_RUP, canID.Bits.mindex, canFram.Data[1]);
        if(matIndex == 0xff)
        {
            /*无该通信收发表*/
            return(eCANERR_NOSUCH_TAB);
        }

        /*读取发送修改的数据与接收到的修改回复数据进行比较*/
        (void)CANReadTxRcvTabData(matIndex, canCmd.Bits.NUM, data);

        for(i = 0; i < (canCmd.Bits.NUM * pindextable->dataType); i++)
        {
            if(data[i] != canFram.Data[2 + i])
            {
                break;
            }
        }

        /*修改成功*/
        if(i >= (canCmd.Bits.NUM * pindextable->dataType))
        {
            /*正确接收，清零计时器*/
            CANClrTxRcvTabTimer(matIndex);

            /*有下一帧*/
            if(gCanTxRcvTable[matIndex].remframe > 0)
            {
                /*发送信息*/
                CANSendTxRcvTabMsgPack(matIndex);

                /*置准备就绪标识*/
                gCanTxRcvTable[matIndex].ready = eCANTAB_WAIT;
            }
             /*没有下一帧*/
            else
            {
                /*需要修改确认*/
                if(gCanTxRcvTable[matIndex].cmd.Bits.CON == 1)
                {
                    /*发送修改确认命令*/
                    (void)CANBuildSendCupMsg_API(gCanTxRcvTable[matIndex].daddr, gCanTxRcvTable[matIndex].attrib, gCanTxRcvTable[matIndex].mIndex, gCanTxRcvTable[matIndex].beginIndex, (gCanTxRcvTable[matIndex].nextsIndex - gCanTxRcvTable[matIndex].beginIndex), canNum);
                }

                /*清除该收发表*/
                CANClrTxRcvTabInfo(matIndex);
            }

            errcode = eCANERR_SUCCESSFUL;
        }
        else
        {
            errcode = eCANERR_RUPW_FAIL;
        }

        return(errcode);
    }
    /*主索引表信息不存在*/
    else
    {
        return(errcode);
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvConUpdateMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 修改确认帧接收处理
** @The notes : 调用用户HOOK
**===============================================================================================*/
static u8 CANRcvConUpdateMsgDeal(t_CANFram canFram, u8 canNum)
{
    u_CANId canID = {0};
    u_CANCmdByte canCmd = {0};
    const t_CANIndexTab *pindextable;
    u8 wrResult = eCANERRREAS_NORMAL;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*解析帧ID*/
    canID.Word = canFram.ID;

    /*非修改确认报文*/
    if(eCANFUN_CUP != canID.Bits.funcode)
    {
        return(eCANERR_KEEP_FUN);
    }

    canCmd.Byte = canFram.Data[0];

    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram.Data[1], canFram.Data[2], &errcode);

    /*主索引表信息存在*/
    if(pindextable != NULL)
    {
        /*CAN报文长度必须与索引长度匹配
        if(canFram.DLC != 3)*/
        /*CAN报文长度不能小于索引长度*/
        if(canFram.DLC < 3)
        {
            /*子索引越界*/
            return(eCANERR_SIDLEN_OVE);
        }

        /*修改确认*/
        if(canCmd.Bits.CON == 1)
        {
            /*请求修改后处理函数*/
            wrResult = CANRcvRupUserDeal_Hook(canID.Bits.saddr, canID.Bits.mindex, canFram.Data[1], canFram.Data[2], canNum);

            /*请求修改正常*/
            if(eCANERRREAS_NORMAL == wrResult)
            {
                errcode = eCANERR_SUCCESSFUL;
            }
            else
            {
                /*发送错误应答报文(权限或参数错误)*/
                errcode = CANBuildSendErrMsg_API(wrResult, &canFram, canNum);
            }
        }
        else
        {
            errcode = eCANERR_RCVMSG_ERR;
        }

        return(errcode);
    }
    /*主索引表信息不存在*/
    else
    {
        /*发送错误应答报文(索引错误)*/
        (void)CANBuildSendErrMsg_API(eCANERRREAS_INDEXERR, &canFram, canNum);

        return(errcode);
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvReqReadMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 请求查询帧接收处理
** @The notes : 调用用户HOOK更新数据域和回复
**===============================================================================================*/
static u8 CANRcvReqReadMsgDeal(t_CANFram canFram, u8 canNum)
{
    u_CANId canID = {0};
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*解析帧ID*/
    canID.Word = canFram.ID;

    /*非请求查询报文*/
    if(eCANFUN_REQ != canID.Bits.funcode)
    {
        return(eCANERR_KEEP_FUN);
    }

    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram.Data[1], canFram.Data[2], &errcode);

    /*主索引表信息存在*/
    if(pindextable != NULL)
    {
        /*CAN报文长度必须与索引长度匹配
        if(canFram.DLC != 3)*/
        /*CAN报文长度不能小于索引长度*/
        if(canFram.DLC < 3)
        {
            //子索引越界
            return(eCANERR_SIDLEN_OVE);
        }

        /*查询长度为0表示查询剩余长度*/
        if(0 == canFram.Data[2])
        {
            //修改查询长度为剩余长度
            canFram.Data[2] = pindextable->sIndexNum - canFram.Data[1];
        }

        /*请求查询回复前处理函数*/
        CANRcvReqUserDeal_Hook(canID.Bits.saddr, canID.Bits.mindex, canFram.Data[1], canFram.Data[2], canNum);

        /*生成查询回复信息*/
        return(CANBuildSendArqMsg_API(&canFram, canNum));
    }
    /*主索引表信息不存在*/
    else
    {
        /*发送错误应答报文(索引错误)*/
        (void)CANBuildSendErrMsg_API(eCANERRREAS_INDEXERR, &canFram, canNum);

        return(errcode);
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvAnsReadMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 查询回复帧接收处理
** @The notes : 更新查询得到的数据和确认查询
**===============================================================================================*/
static u8 CANRcvAnsReadMsgDeal(t_CANFram canFram, u8 canNum)
{
    u8 matIndex = 0;
    u_CANId canID = {0};
    u_CANCmdByte canCmd = {0};
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*解析帧ID*/
    canID.Word = canFram.ID;

    //非查询回复报文
    if(eCANFUN_ARQ != canID.Bits.funcode)
    {
        return(eCANERR_KEEP_FUN);
    }

    canCmd.Byte = canFram.Data[0];

    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram.Data[1], canCmd.Bits.NUM, &errcode);

    /*主索引表信息存在*/
    if(pindextable != NULL)
    {
        /*CAN报文长度必须与索引长度匹配
        if(canFram.DLC != (2 + (canCmd.Bits.NUM * pindextable->dataType)))*/
        /*CAN报文长度不能小于索引长度*/
        if(canFram.DLC < (2 + (canCmd.Bits.NUM * pindextable->dataType)))
        {
            /*子索引越界*/
            return(eCANERR_SIDLEN_OVE);
        }

        /*查找CAN收发通信表是否正在处理该报文(发送请求查询命令)*/
        matIndex = CANFindTxRcvTabMatching(canID.Bits.saddr, eCANFUN_REQ, canID.Bits.mindex, canFram.Data[1]);
        if(matIndex == 0xff)
        {
            /*无该通信收发表*/
            return(eCANERR_NOSUCH_TAB);
        }

        /*更新收发通信表中等待回复的索引信息*/
        gCanTxRcvTable[matIndex].sIndex = canFram.Data[1];
        gCanTxRcvTable[matIndex].nextsIndex = canFram.Data[1] + canCmd.Bits.NUM;

        /*正确接收,清零计时器*/
        CANClrTxRcvTabTimer(matIndex);

        /*将接收到的数据写入主索引表中*/
        errcode = CANWriteCanIndexData(pindextable, canFram.Data[1], canCmd.Bits.NUM, &canFram.Data[2], canNum);

        /*写入主索引更新成功*/
        if(eCANERR_SUCCESSFUL == errcode)
        {
            /*如果该信息需要确认,则发送查询确认信息*/
            if(canCmd.Bits.CON == 1)
            {
                /*如果是查询回复最后一帧则清空该收发通信表*/
                if(canCmd.Bits.FIN == 1)
                {
                    CANClrTxRcvTabInfo(matIndex);
                }

                /*生成发送查询确认报文*/
                return(CANBuildSendCrqMsg_API(&canFram, canNum));
            }

            errcode = eCANERR_SUCCESSFUL;
        }
        /*查询回复写入主索引失败*/
        else
        {
            errcode = eCANERR_REQW_FAIL;
        }

        return(errcode);
    }
    /*主索引表信息不存在*/
    else
    {
        return(errcode);
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvConReadMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 查询确认帧接收处理
** @The notes : 确认是否正确接收和发送下一帧查询回复
**===============================================================================================*/
static u8 CANRcvConReadMsgDeal(t_CANFram canFram, u8 canNum)
{
    u8 i = {0};
    u8 matIndex = 0;
    u8 data[6] = {0};
    u_CANId canID = {0};
    u_CANCmdByte canCmd = {0};
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*解析帧ID*/
    canID.Word = canFram.ID;

    /*非查询确认报文*/
    if(eCANFUN_CRQ != canID.Bits.funcode)
    {
        return(eCANERR_KEEP_FUN);
    }

    canCmd.Byte = canFram.Data[0];

    pindextable = CanScanIndexTabInfo(canID.Bits.daddr, canID.Bits.mindex, canFram.Data[1], canCmd.Bits.NUM, &errcode);

    /*主索引表信息存在*/
    if(pindextable != NULL)
    {
        /*CAN报文长度必须与索引长度匹配
        if(canFram.DLC != (2 + (canCmd.Bits.NUM * pindextable->dataType)))*/
        /*CAN报文长度不能小于索引长度*/
        if(canFram.DLC < (2 + (canCmd.Bits.NUM * pindextable->dataType)))
        {
            /*子索引越界*/
            return(eCANERR_SIDLEN_OVE);
        }

        /*查找CAN收发通信表是否正在处理该报文(发送请求查询命令)*/
        matIndex = CANFindTxRcvTabMatching(canID.Bits.saddr, eCANFUN_ARQ, canID.Bits.mindex, canFram.Data[1]);
        if(matIndex == 0xff)
        {
            /*无该通信收发表*/
            return(eCANERR_NOSUCH_TAB);
        }

        /*读取回复的数据与接收到确认的数据进行比较*/
        (void)CANReadTxRcvTabData(matIndex, canCmd.Bits.NUM, data);

        for(i = 0; i < (canCmd.Bits.NUM * pindextable->dataType); i++)
        {
            if(data[i] != canFram.Data[2 + i])
            {
                break;
            }
        }

        /*确认查询回复接收成功*/
        if(i >= (canCmd.Bits.NUM * pindextable->dataType))
        {
            /*正确接收,清零计时器*/
            CANClrTxRcvTabTimer(matIndex);

            /*有下一帧*/
            if(gCanTxRcvTable[matIndex].remframe > 0)
            {
                /*发送信息*/
                CANSendTxRcvTabMsgPack(matIndex);

                /*置准备就绪标识*/
                gCanTxRcvTable[matIndex].ready = eCANTAB_WAIT;
            }
            /*发送完全部查询回复*/
            else
            {
                /*确认最后一帧查询回复*/
                if(canCmd.Bits.FIN == 1)
                {
                    /*清除该收发表*/
                    CANClrTxRcvTabInfo(matIndex);
                }
            }

            errcode = eCANERR_SUCCESSFUL;
        }
        /*未正确接收写入查询回复*/
        else
        {
            errcode = eCANERR_REQW_FAIL;
        }

        return(errcode);
    }
    /*主索引表信息不存在*/
    else
    {
        return(errcode);
    }
}

/*=================================================================================================
** @Name      : static u8 CANRcvCycUpMsgDeal(t_CANFram canFram, u8 canNum)
** @Input     : canFram:CAN通信帧 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : 周期主动更新帧接收处理
** @The notes : 更新接收得到的数据到索引表
**===============================================================================================*/
static u8 CANRcvCycUpMsgDeal(t_CANFram canFram, u8 canNum)
{
    u8 *pData = 0;
    u8 mIndex = 0;
    u8 sIndex = 0;
    u8 sIndexNum = 0;
    u_CANId canID = {0};
    u_CANCmdByte canCmd = {0};
    t_CANMapTrans canMapInfo = {0};
    const t_CANIndexTab *pindextable;
    u8 errcode = eCANERR_SUCCESSFUL;

    /*解析帧ID*/
    canID.Word = canFram.ID;

    //周期主动更新长帧报文
    if(eCANFUN_UPM == canID.Bits.funcode)
    {
        canCmd.Byte = canFram.Data[0];

        //获取索引信息
        pData = &canFram.Data[2];
        mIndex = canID.Bits.mindex;
        sIndex = canFram.Data[1];
        sIndexNum = canCmd.Bits.NUM;
    }
    //周期主动更新单帧报文
    else if(eCANFUN_UPS == canID.Bits.funcode)
    {
        //获取索引信息
        pData = &canFram.Data[0];
        mIndex = canID.Bits.mindex;
        sIndex = 0;
        sIndexNum = 1;
    }
    //周期主动更新单体或参数报文
    else if((eCANFUN_UPC == canID.Bits.funcode) || (eCANFUN_UPP == canID.Bits.funcode))
    {
        //获取索引信息
        pData = &canFram.Data[0];

        /*通过映射求实际索引信息*/
        canMapInfo = CANIndexMapTransUser_Hook(canID.Bits.funcode, canID.Bits.mindex, canFram.DLC);
        mIndex = canMapInfo.realMIndex;
        sIndex = canMapInfo.realSIndex;
        sIndexNum = canMapInfo.realSIndexNum;
    }
    //非周期主动更新帧报文
    else
    {
        return(eCANERR_KEEP_FUN);
    }

    pindextable = CanScanIndexTabInfo(canID.Bits.saddr, mIndex, sIndex, sIndexNum, &errcode);

    /*主索引表信息存在*/
    if(pindextable != NULL)
    {
        //周期主动更新长帧报文
        if(eCANFUN_UPM == canID.Bits.funcode)
        {
            /*CAN报文长度必须与索引长度匹配
            if(canFram.DLC != (2 + (canCmd.Bits.NUM * pindextable->dataType)))*/
            /*CAN报文长度不能小于索引长度*/
            if(canFram.DLC < (2 + (sIndexNum * pindextable->dataType)))
            {
                /*子索引越界*/
                return(eCANERR_SIDLEN_OVE);
            }
        }
        /*周期主动发送单帧信息*/
        else
        {
            /*求子索引数目*/
            sIndexNum = canFram.DLC / pindextable->dataType;

            /*不够一个子索引长度*/
            if(0 == sIndexNum)
            {
                /*报文错误*/
                return(eCANERR_RCVMSG_ERR);
            }
            /*写入超过子索引长度*/
            else if((sIndex + sIndexNum) > pindextable->sIndexNum)
            {
                /*子索引越界*/
                return(eCANERR_SIDLEN_OVE);
            }
        }

        /*将接收到的数据写入主索引表中*/
        errcode = CANWriteCanIndexData(pindextable, sIndex, sIndexNum, pData, canNum);

        /*写入主索引更新成功*/
        if(eCANERR_SUCCESSFUL == errcode)
        {
            /*接收周期更新报文后处理函数*/
            CANRcvCycUpUserDeal_Hook (canFram.ID, canFram.Data, canFram.DLC, canNum);

            errcode = eCANERR_SUCCESSFUL;
        }
        /*主动更新写入主索引失败*/
        else
        {
            errcode = eCANERR_CYCW_FAIL;
        }

        return(errcode);
    }
    /*主索引表信息不存在*/
    else
    {
        return(errcode);
    }
}
