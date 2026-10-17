#ifndef EKF_ADAPTER_H
#define EKF_ADAPTER_H

#include "BSPTypeDef.h"

/* =====================================================================
 * ekf_adapter.h — EKF SOC 接入适配层
 * 职责: 实例化 EKF + 定点/浮点单位换算 + 影子结构体记录
 * 原则: 只观察、只记录, 绝不写任何控制/保护/均衡/上报变量
 * ===================================================================== */

/* EKF 影子记录编译开关: 1=开启(默认), 0=整体关闭 */
#define EKF_SHADOW_EN   1

/* ======================================================================
 * EKF upload data for EMS (CAN index 77/78/84)
 * index 77: current + min/avg/max cell voltage (4 x s16 = 8 bytes)
 * index 78: temp + soc/existSoc/refSoc (4 x s16 = 8 bytes)
 * index 84: capacity (2 x u32 = 8 bytes)
 * ===================================================================== */
typedef enum {
    eEkfInfo_Curr = 0,     /* current 0.1A (discharge positive) */
    eEkfInfo_MinV = 1,     /* min cell voltage 1mV */
    eEkfInfo_AvgV = 2,     /* avg cell voltage 1mV */
    eEkfInfo_MaxV = 3,     /* max cell voltage 1mV */
    eEkfInfo_Len
} e_EkfInfoIndex;

typedef enum {
    eEkfInfo78_Temp = 0,     /* temp 0.1C (avg) */
    eEkfInfo78_Soc  = 1,     /* EKF SOC permille */
    eEkfInfo78_ExistSoc = 2, /* existing algorithm SOC permille */
    eEkfInfo78_RefSoc = 3,   /* reference SOC permille */
    eEkfInfo78_Len
} e_EkfInfo78Index;

typedef enum {
    eEkfInfo84_NowCap = 0, /* current capacity 1mAh */
    eEkfInfo84_RemCap = 1, /* remain capacity 1mAh */
    eEkfInfo84_Len
} e_EkfInfo84Index;

#define EKF_CAN_INDEX77     77
#define EKF_CAN_INDEX78     78
#define EKF_CAN_INDEX84     84
#define EKF_CAN_INFO77_LEN  eEkfInfo_Len
#define EKF_CAN_INFO78_LEN  eEkfInfo78_Len
#define EKF_CAN_INFO84_LEN  eEkfInfo84_Len

extern s16 gCANEkfInfo_77[EKF_CAN_INFO77_LEN];
extern s16 gCANEkfInfo_78[EKF_CAN_INFO78_LEN];
extern u32 gCANEkfInfo_84[EKF_CAN_INFO84_LEN];

#if (EKF_SHADOW_EN == 1)

/* EKF 影子结构体: 仅用于对比记录 */
typedef struct {
    u16 soc;       /* EKF 估计 SOC (千分之一, 0~1000) */
    u16 refSoc;    /* 参考真值 SOC (千分之一) — 预留, 满充满放循环接入后填充 */
    u16 existSoc;  /* 现有算法 SOC (千分之一, GetGRealSocMilliAPI 快照) */
    u32 tick;      /* 时间戳 (500ms 计数, 用于三线对齐) */
} t_EkfShadow;

extern t_EkfShadow gEkfShadow;

/* EKF 适配层任务入口: 挂在 SocSoeLibAllTask 的 case 2/12 (500ms) */
void EkfAdapterTask(void);

#endif /* EKF_SHADOW_EN */

#endif /* EKF_ADAPTER_H */
