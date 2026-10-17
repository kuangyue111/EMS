/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANTxCycle.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : CAN通信包报文周期上传操作处理接口
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "CANHandle.h"
#include "CANTabOper.h"
#include "CANInterface.h"
#include "CANUser.h"
#include "CANTxCycle.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u32 gCANCountTime;                                           //CAN通信包总计时(1ms)

extern t_CANTxCycTab gCanTxCycTable[];                              //CAN通信周期发送处理表

extern u16 gCANDevInfo_1[];                                         //设备信息
extern u8 gCANBaseDef_3[];                                          //通信包基本配置信息
extern u8 gCANTxVerDef_10[];                                        //通信包校时帧发送配置信息
extern u8 gCANTxBreDef_11[];                                        //通信包心跳帧发送配置信息
extern u8 gCANTxMulDef_12[];                                        //通信包长帧帧发送配置信息
extern u8 gCANTxSinDef_13[];                                        //通信包单帧帧发送配置信息
extern u8 gCANTxCelDef_14[];                                        //通信包单体帧发送配置信息
extern u8 gCANTxParDef_15[];                                        //通信包参数帧发送配置信息
extern u8 gCANParCfgStat_20[];                                      //通信包参数批量配置状态信息
extern u16 gCANRcdNum_30[];                                         //通信包事件记录数目状态信息

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CANTxCycleTabInit(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信包周期发送表初始化
** @The notes : 通信包初始化调用
**===============================================================================================*/
void CANTxCycleTabInit(void)
{
	u8 i = 0;
	u8 id = 0;
	u8 tabLen = 0;
	u8 mIndex = 0;
	u8 sIndex = 0;
	u8 sIndexNum = 0;
	u16 txTime = 0;
	u8 errcode = 0;
    const t_CANIndexTab *pIndexTable;

    /*未定义发送表*/
    if(gCanTxCycTable == NULL)
    {
        return;
    }

    /*通信包长度(发送长帧/单帧/单体/参数帧报文)*/
    tabLen = gCANBaseDef_3[eCANRBas3_TxMulNum] + gCANBaseDef_3[eCANRBas3_TxSinNum] + gCANBaseDef_3[eCANRBas3_TxCelNum] + gCANBaseDef_3[eCANRBas3_TxParNum];

    /*周期发送单帧信息配置信息*/
    for(i = 0; i < gCANBaseDef_3[eCANRBas3_TxSinNum]; i++)
    {
        /*获得用户配置的发送配置信息*/
    	mIndex = gCANTxSinDef_13[1 + (7 * i)];
    	sIndex = gCANTxSinDef_13[2 + (7 * i)];
    	sIndexNum = gCANTxSinDef_13[3 + (7 * i)];
    	txTime = ((u16)gCANTxSinDef_13[5 + (7 * i)] << 8) | gCANTxSinDef_13[4 + (7 * i)];

        /*查找主索引表,获得该主索引的数据地址*/
    	pIndexTable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], mIndex, sIndex, sIndexNum, &errcode);

    	/*索引信息、数据类型和周期必须配置*/
        if((pIndexTable != NULL) && (pIndexTable->pObject != NULL) && (pIndexTable->dataType > 0) &&  (txTime >= CAN_TX_UPM_SPA_T))
        {
        	/*获得该主索引的数据地址*/
            gCanTxCycTable[id].pdata = pIndexTable->pObject;

            /*配置主动发送信息和启动时间*/
            gCanTxCycTable[id].dataType = pIndexTable->dataType;
            gCanTxCycTable[id].funcode = eCANFUN_UPS;
            gCanTxCycTable[id].index = 1 + (7 * i);
            gCanTxCycTable[id].nextTime = 1 + CAN_TX_UPM_SPA_T * i;
            id++;
        }
    }

    /*周期发送单体帧信息配置信息*/
    for(i = 0; i < gCANBaseDef_3[eCANRBas3_TxCelNum]; i++)
    {
        /*获得用户配置的发送配置信息*/
    	mIndex = gCANTxCelDef_14[1 + (8 * i)];
    	sIndex = gCANTxCelDef_14[2 + (8 * i)];
    	sIndexNum = gCANTxCelDef_14[3 + (8 * i)];
    	txTime = ((u16)gCANTxCelDef_14[6 + (8 * i)] << 8) | gCANTxCelDef_14[5 + (8 * i)];

        /*查找主索引表,获得该主索引的数据地址*/
    	pIndexTable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], mIndex, sIndex, sIndexNum, &errcode);

    	/*索引信息、数据类型和周期必须配置*/
        if((pIndexTable != NULL) && (pIndexTable->pObject != NULL) && (pIndexTable->dataType > 0) &&  (txTime >= CAN_TX_UPM_SPA_T))
        {
        	/*获得该主索引的数据地址*/
            gCanTxCycTable[id].pdata = pIndexTable->pObject;

            /*配置主动发送信息和启动时间*/
            gCanTxCycTable[id].dataType = pIndexTable->dataType;
            gCanTxCycTable[id].funcode = eCANFUN_UPC;
            gCanTxCycTable[id].index = 1 + (8 * i);
            gCanTxCycTable[id].nextTime = 3 + CAN_TX_UPM_SPA_T * i;
            id++;
        }
    }

    /*周期发送长帧信息配置信息*/
    for(i = 0; i < gCANBaseDef_3[eCANRBas3_TxMulNum]; i++)
    {
        /*获得用户配置的发送配置信息*/
    	mIndex = gCANTxMulDef_12[1 + (7 * i)];
    	sIndex = gCANTxMulDef_12[2 + (7 * i)];
    	sIndexNum = gCANTxMulDef_12[3 + (7 * i)];
    	txTime = ((u16)gCANTxMulDef_12[5 + (7 * i)] << 8) | gCANTxMulDef_12[4 + (7 * i)];

        /*查找主索引表,获得该主索引的数据地址*/
    	pIndexTable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], mIndex, sIndex, sIndexNum, &errcode);

    	/*索引信息、数据类型和周期必须配置*/
        if((pIndexTable != NULL) && (pIndexTable->pObject != NULL) && (pIndexTable->dataType > 0) &&  (txTime >= CAN_TX_UPM_SPA_T))
        {
        	/*获得该主索引的数据地址*/
            gCanTxCycTable[id].pdata = pIndexTable->pObject;

            /*配置主动发送信息和启动时间*/
            gCanTxCycTable[id].dataType = pIndexTable->dataType;
            gCanTxCycTable[id].funcode = eCANFUN_UPM;
            gCanTxCycTable[id].index = 1 + (7 * i);
            gCanTxCycTable[id].nextTime = 5 + CAN_TX_UPM_SPA_T * i;
            id++;
        }
    }

    /*周期发送参数帧信息配置信息*/
    for(i = 0; i < gCANBaseDef_3[eCANRBas3_TxParNum]; i++)
    {
        /*获得用户配置的发送配置信息*/
    	mIndex = gCANTxParDef_15[1 + (8 * i)];
    	sIndex = gCANTxParDef_15[2 + (8 * i)];
    	sIndexNum = gCANTxParDef_15[3 + (8 * i)];
    	txTime = ((u16)gCANTxParDef_15[6 + (8 * i)] << 8) | gCANTxParDef_15[5 + (8 * i)];

        /*查找主索引表,获得该主索引的数据地址*/
    	pIndexTable = CanScanIndexTabInfo(gCANDevInfo_1[eCANDev1_Addr], mIndex, sIndex, sIndexNum, &errcode);

    	/*索引信息、数据类型和周期必须配置*/
        if((pIndexTable != NULL) && (pIndexTable->pObject != NULL) && (pIndexTable->dataType > 0) &&  (txTime >= CAN_TX_UPM_SPA_T))
        {
        	/*获得该主索引的数据地址*/
            gCanTxCycTable[id].pdata = pIndexTable->pObject;

            /*配置主动发送信息和启动时间*/
            gCanTxCycTable[id].dataType = pIndexTable->dataType;
            gCanTxCycTable[id].funcode = eCANFUN_UPP;
            gCanTxCycTable[id].index = 1 + (8 * i);
            gCanTxCycTable[id].nextTime = 7 + CAN_TX_UPM_SPA_T * i;
            id++;
        }
    }

    /*剩余发送表清零*/
    for(i = id; i < tabLen; i++)
    {
        /*发送表清零*/
        gCanTxCycTable[id].pdata = NULL;
        gCanTxCycTable[id].dataType = 0;
        gCanTxCycTable[id].funcode = eCANFUN_VFT;
        gCanTxCycTable[id].index = 0;
        gCanTxCycTable[id].nextTime = 0;
    }
}

/*=================================================================================================
** @Name      : void CANTxCycleTabHandle(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信包周期发送表处理任务
** @The notes : 通信包总任务10ms调用
**===============================================================================================*/
void CANTxCycleTabHandle(void)
{
    u8 i = 0;
	u8 mIndex = 0;
	u8 sIndex = 0;
	u8 sIndexNum = 0;
	u16 txTime = 0;
	u8 canNum = 0;
	u8 destAddr = 0;
	u8 mapIndex = 0;
	u8 tabLen = 0;

    /*未定义发送表*/
    if(gCanTxCycTable == NULL)
    {
        return;
    }

    /*通信包长度(发送长帧/单帧/单体/参数帧报文)*/
    tabLen = gCANBaseDef_3[eCANRBas3_TxMulNum] + gCANBaseDef_3[eCANRBas3_TxSinNum] + gCANBaseDef_3[eCANRBas3_TxCelNum] + gCANBaseDef_3[eCANRBas3_TxParNum];

    /*逐个检测每个通信包*/
    for(i = 0; i < tabLen; i++)
    {
    	/*到达发送周期*/
        if(gCANCountTime >= gCanTxCycTable[i].nextTime)
        {
        	/*通信发送信息有效*/
            if(gCanTxCycTable[i].pdata != NULL)
            {
            	/*发送长帧*/
            	if(eCANFUN_UPM == gCanTxCycTable[i].funcode)
            	{
            		/*使能发送多帧*/
                    if(1 == gCANTxMulDef_12[0])
                    {
                	    /*每组7个信息*/
                        mIndex = gCANTxMulDef_12[gCanTxCycTable[i].index];
                        sIndex = gCANTxMulDef_12[gCanTxCycTable[i].index + 1];
                        sIndexNum = gCANTxMulDef_12[gCanTxCycTable[i].index + 2];
                        txTime = ((u16)gCANTxMulDef_12[gCanTxCycTable[i].index + 4] << 8) | gCANTxMulDef_12[gCanTxCycTable[i].index + 3];
                        canNum = gCANTxMulDef_12[gCanTxCycTable[i].index + 5];
                        destAddr = gCANTxMulDef_12[gCanTxCycTable[i].index + 6];

                        /*用户允许发送*/
                        if(TRUE == CANTxCycUpUserEnState_Hook(eCANFUN_UPM, mIndex, canNum))
                        {
                            /*生成主动发送信息*/
            	            (void)CANReqIdleTxRcvTab(eCANFUN_UPM, 0, gCanTxCycTable[i].pdata, gCanTxCycTable[i].dataType, mIndex, sIndex, sIndexNum, 0, destAddr, canNum);
                        }
                   }
                }
                /*发送单帧*/
                else if(eCANFUN_UPS == gCanTxCycTable[i].funcode)
                {
                    /*使能发送单帧*/
                    if(1 == gCANTxSinDef_13[0])
                    {
                        /*每组7个信息*/
                        mIndex = gCANTxSinDef_13[gCanTxCycTable[i].index];
                        sIndex = gCANTxSinDef_13[gCanTxCycTable[i].index + 1];
                        sIndexNum = gCANTxSinDef_13[gCanTxCycTable[i].index + 2];
                        txTime = ((u16)gCANTxSinDef_13[gCanTxCycTable[i].index + 4] << 8) | gCANTxSinDef_13[gCanTxCycTable[i].index + 3];
                        canNum = gCANTxSinDef_13[gCanTxCycTable[i].index + 5];
                        destAddr = gCANTxSinDef_13[gCanTxCycTable[i].index + 6];

                        /*生成主动发送信息*/
            	        (void)CANReqIdleTxRcvTab(eCANFUN_UPS, 0, gCanTxCycTable[i].pdata, gCanTxCycTable[i].dataType, mIndex, sIndex, sIndexNum, 0, destAddr, canNum);
                    }
                }
                /*发送单体帧*/
                else if(eCANFUN_UPC == gCanTxCycTable[i].funcode)
                {
                    /*使能发送单体帧*/
                    if(1 == gCANTxCelDef_14[0])
                    {
                        /*每组8个信息*/
                        mIndex = gCANTxCelDef_14[gCanTxCycTable[i].index];
                        sIndex = gCANTxCelDef_14[gCanTxCycTable[i].index + 1];
                        sIndexNum = gCANTxCelDef_14[gCanTxCycTable[i].index + 2];
                        mapIndex = gCANTxCelDef_14[gCanTxCycTable[i].index + 3];
                        txTime = ((u16)gCANTxCelDef_14[gCanTxCycTable[i].index + 5] << 8) | gCANTxCelDef_14[gCanTxCycTable[i].index + 4];
                        canNum = gCANTxCelDef_14[gCanTxCycTable[i].index + 6];
                        destAddr = gCANTxCelDef_14[gCanTxCycTable[i].index + 7];

                        /*用户允许发送*/
						if(TRUE == CANTxCycUpUserEnState_Hook(eCANFUN_UPC, mIndex, canNum))
						{
							/*生成主动发送信息*/
            	            (void)CANReqIdleTxRcvTab(eCANFUN_UPC, 0, gCanTxCycTable[i].pdata, gCanTxCycTable[i].dataType, mIndex, sIndex, sIndexNum, mapIndex, destAddr, canNum);
						}
                    }
                }
                /*发送参数帧*/
                else if(eCANFUN_UPP == gCanTxCycTable[i].funcode)
                {
            		/*使能发送参数帧*/
                    if(1 == gCANTxParDef_15[0])
                    {
                        /*每组8个信息*/
                        mIndex = gCANTxParDef_15[gCanTxCycTable[i].index];
                        sIndex = gCANTxParDef_15[gCanTxCycTable[i].index + 1];
                        sIndexNum = gCANTxParDef_15[gCanTxCycTable[i].index + 2];
                        mapIndex = gCANTxParDef_15[gCanTxCycTable[i].index + 3];
                        txTime = ((u16)gCANTxParDef_15[gCanTxCycTable[i].index + 5] << 8) | gCANTxParDef_15[gCanTxCycTable[i].index + 4];
                        canNum = gCANTxParDef_15[gCanTxCycTable[i].index + 6];
                        destAddr = gCANTxParDef_15[gCanTxCycTable[i].index + 7];

                        /*生成主动发送信息*/
            	        (void)CANReqIdleTxRcvTab(eCANFUN_UPP, 0, gCanTxCycTable[i].pdata, gCanTxCycTable[i].dataType, mIndex, sIndex, sIndexNum, mapIndex, destAddr, canNum);
                    }
            	}

                /*更新下一次发送信息的时间*/
                gCanTxCycTable[i].nextTime =  gCANCountTime + txTime;

                /*超过最大时间时间*/
                if(gCanTxCycTable[i].nextTime >= CAN_MAX_COUNT_TIME)
                {
                    /*回头计时*/
                    gCanTxCycTable[i].nextTime = gCanTxCycTable[i].nextTime - CAN_MAX_COUNT_TIME;
                }
            }
        }
    }
}

/*=================================================================================================
** @Name      : void CANLibCycInfoTxHandle(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信包内部周期信息发送处理任务
** @The notes : 通信包总任务10ms调用
**===============================================================================================*/
void CANLibCycInfoTxHandle(void)
{
    u8 i = 0;
	u8 canNum = 0;
	u16 txTime = 0;
	static u32 sCfgTime = 0;
	//static u32 sSoeTime = 0;
	static u32 sVerTime[CAN_MAX_CAN_NUM] = {0};
	static u32 sBreTime[CAN_MAX_CAN_NUM] = {0};

    /*心跳帧发送*/
    if(1 == gCANTxBreDef_11[0])
    {
    	/*逐个检测要发送的每组CAN通道*/
    	for(i = 0; i < gCANBaseDef_3[eCANRBas3_TxBreNum]; i++)
    	{
            /*达到发送周期*/
            if(gCANCountTime >= sBreTime[i])
            {
        	    /*每组3个信息*/
                canNum = gCANTxBreDef_11[1 + i * 3];
                txTime = ((u16)gCANTxBreDef_11[3 + i * 3] << 8) | gCANTxBreDef_11[2 + i * 3];

            	/*发送心跳报文*/
            	(void)CANBuildSendBreMsg_API(canNum);

            	/*更新下一次发送信息的时间*/
            	sBreTime[i] = gCANCountTime + txTime;

                /*超过最大时间时间*/
                if(sBreTime[i] >= CAN_MAX_COUNT_TIME)
                {
                    /*回头计时*/
                	sBreTime[i] = sBreTime[i] - CAN_MAX_COUNT_TIME;
                }
            }
    	}
    }

    /*校时帧发送*/
    if(2 == gCANTxVerDef_10[0])
    {
    	/*逐个检测要发送的每组CAN通道*/
    	for(i = 0; i < gCANBaseDef_3[eCANRBas3_TxVerNum]; i++)
    	{
            /*达到发送周期*/
            if(gCANCountTime >= sVerTime[i])
            {
        	    /*每组3个信息*/
                canNum = gCANTxVerDef_10[1 + i * 3];
                txTime = ((u16)gCANTxVerDef_10[3 + i * 3] << 8) | gCANTxVerDef_10[2 + i * 3];

            	/*发送校时报文*/
            	(void)CANBuildSendVerMsg_API(canNum);

            	/*更新下一次发送信息的时间*/
            	sVerTime[i] = gCANCountTime + txTime;

                /*超过最大时间时间*/
                if(sVerTime[i] >= CAN_MAX_COUNT_TIME)
                {
                    /*回头计时*/
                	sVerTime[i] = sVerTime[i] - CAN_MAX_COUNT_TIME;
                }
            }
    	}
    }

    /*参数批量配置状态信息发送(500ms周期)*/
    /*进入参数配置模式*/
    if(gCANParCfgStat_20[0] > 0)
    {
        /*达到发送周期*/
        if(gCANCountTime >= sCfgTime)
        {
    	    /*发送主动发送长帧报文*/
        	(void)CANBuildSendUpmMsg_API(0xff, 0, 20, 0, 2, CAN_UPDATE_PORT);

        	/*更新下一次发送信息的时间*/
        	sCfgTime = gCANCountTime + 500;

            /*超过最大时间时间*/
            if(sCfgTime >= CAN_MAX_COUNT_TIME)
            {
                /*回头计时*/
            	sCfgTime = sCfgTime - CAN_MAX_COUNT_TIME;
            }
        }
    }

    /*事件记录数目状态信息发送(500ms周期)*/
    /*有记录数目则主动发送(改为需要查询,因为数目需要查询才更新显示值)*/
//    if((gCANRcdNum_30[0] > 0) || (gCANRcdNum_30[1] > 0) || (gCANRcdNum_30[2] > 0) || (gCANRcdNum_30[3] > 0))
//    {
//        /*达到发送周期*/
//        if(gCANCountTime >= sSoeTime)
//        {
//    	    /*发送主动发送长帧报文*/
//        	(void)CANBuildSendUpmMsg_API(0xff, 0, 30, 0, 4, CAN_UPDATE_PORT);
//
//        	/*更新下一次发送信息的时间*/
//        	sSoeTime = gCANCountTime + 500;
//
//            /*超过最大时间时间*/
//            if(sSoeTime >= CAN_MAX_COUNT_TIME)
//            {
//                /*回头计时*/
//            	sSoeTime = sSoeTime - CAN_MAX_COUNT_TIME;
//            }
//        }
//    }
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

