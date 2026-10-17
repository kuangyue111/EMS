#include "ekf_adapter.h"

/* EKF 上送 EMS 数据数组 (CAN 索引号 77/78/84, 查询式) */
s16 gCANEkfInfo_77[EKF_CAN_INFO77_LEN];
s16 gCANEkfInfo_78[EKF_CAN_INFO78_LEN];
u32 gCANEkfInfo_84[EKF_CAN_INFO84_LEN];
#if (EKF_SHADOW_EN == 1)

#include "ekf_bms.h"        /* EKF 核心: EKF_Init / EKF_Update / ekf_instance_t */
#include "CurrSample.h"     /* GetGRealCurr10mAAPI / e_CSampChan */
#include "GroupInfo.h"      /* GetGCellAvgVoltAPI / GetGCellAvgTempAPI */
#include "CapInfoCalc.h"    /* GetGRealSocMilliAPI / GetGroupTotalCapAPI / GetGroupStandCapAPI */
#include "EnerInterface.h"  /* EnerGetGroupBattReadyHook */

/* =====================================================================
 * 单位换算常量 (现有 API 定点数 -> EKF float 物理量)
 * ===================================================================== */
#define EKF_TS            (0.5f)     /* 采样周期 s, 挂在 500ms 槽位 */
#define EKF_CURR_SCALE    (0.01f)    /* 电流 10mA -> A */
#define EKF_VOLT_SCALE    (0.001f)   /* 电压 1mV  -> V */
#define EKF_TEMP_SCALE    (0.1f)     /* 温度 0.1℃ -> ℃ */
#define EKF_SOC_SCALE     (0.001f)   /* SOC 千分之一 -> 0~1 */
#define EKF_CAP_SCALE     (0.001f)   /* 容量 mAh -> Ah */

/* =====================================================================
 * 全局影子记录
 * ===================================================================== */
t_EkfShadow gEkfShadow;

/* =====================================================================
 * EKF 实例 (静态, 只被 EkfAdapterTask 访问)
 * ===================================================================== */
static ekf_instance_t sEkf;
static ekf_input_t    sEkfIn;
static ekf_output_t   sEkfOut;
static u8             sEkfInited = 0;

/* =====================================================================
 * EkfAdapterTask
 * 挂在 SocSoeLibAllTask 的 case 2/12, 500ms 周期
 * ===================================================================== */
void EkfAdapterTask(void)
{
    u16 volt = 0;   /* EKF input cell voltage (1mV) */
    s16 temp = 0;   /* EKF input cell temperature (0.1C) */

    /* init once when battery ready */
    if ((0 == sEkfInited) && (1 == EnerGetGroupBattReadyHook()))
    {
        float initSoc = (float)GetGRealSocMilliAPI() * EKF_SOC_SCALE;   /* 安时积分 SOC 作为初始值 */
        float qnAh = (float)GetGroupTotalCapAPI() * EKF_CAP_SCALE;      /* 实际总容量(满充满放学习值) */
        if (qnAh <= 0.0f)
        {
            qnAh = (float)GetGroupStandCapAPI() * EKF_CAP_SCALE;        /* topCap 异常(为0)时回退标称容量, 防 Qn=0 除零 */
        }

        EKF_Init(&sEkf, initSoc, qnAh, EKF_TS);
        sEkfInited = 1;
    }

    volt = GetGCellAvgVoltAPI();
    temp = GetGCellAvgTempAPI();

    /* refresh CAN upload (current + voltage statistics + capacity, always uploaded):
       index 77 = curr/min/avg/max cell voltage (s16 x4)
       index 84 = nowCap/remCap (u32 x2) */
    gCANEkfInfo_77[eEkfInfo_Curr] = GetGSampOutCurrAPI();
    gCANEkfInfo_77[eEkfInfo_MinV] = (s16)GetGCellMinVoltAPI();
    gCANEkfInfo_77[eEkfInfo_AvgV] = (s16)GetGCellAvgVoltAPI();
    gCANEkfInfo_77[eEkfInfo_MaxV] = (s16)GetGCellMaxVoltAPI();
    gCANEkfInfo_84[eEkfInfo84_NowCap] = GetGroupNowCapAPI();
    gCANEkfInfo_84[eEkfInfo84_RemCap] = GetGroupRemainCapAPI();

    if (0 == sEkfInited)
    {
        return;   /* battery not ready yet */
    }

    /* sample -> float physical unit */
    sEkfIn.I           = (float)GetGRealCurr10mAAPI(eCAFE_Shunt) * EKF_CURR_SCALE;
    sEkfIn.Ut_measured = (float)volt * EKF_VOLT_SCALE;
    sEkfIn.temp        = (float)temp * EKF_TEMP_SCALE;

    /* EKF iteration */
    EKF_Update(&sEkf, &sEkfIn, &sEkfOut);

    /* shadow record */
    gEkfShadow.existSoc = GetGRealSocMilliAPI();
    if (sEkfOut.SOC > 1.0f) sEkfOut.SOC = 1.0f;
    if (sEkfOut.SOC < 0.0f) sEkfOut.SOC = 0.0f;
    gEkfShadow.soc = (u16)(sEkfOut.SOC * 1000.0f + 0.5f);
    gEkfShadow.refSoc = 0;
    gEkfShadow.tick++;

    /* refresh CAN upload (temp + SOC comparison, after EKF update):
       index 78 = temp/soc/existSoc/refSoc (s16 x4) */
    gCANEkfInfo_78[eEkfInfo78_Temp]     = temp;
    gCANEkfInfo_78[eEkfInfo78_Soc]      = (s16)gEkfShadow.soc;
    gCANEkfInfo_78[eEkfInfo78_ExistSoc] = (s16)gEkfShadow.existSoc;
    gCANEkfInfo_78[eEkfInfo78_RefSoc]   = (s16)gEkfShadow.refSoc;
}

#endif /* EKF_SHADOW_EN */
