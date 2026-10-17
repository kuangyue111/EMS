#ifndef EKF_BMS_H
#define EKF_BMS_H

#ifdef __cplusplus
extern "C" {
#endif

/* 1. 输入接口结构体(与 3 状态版完全一致) */
typedef struct {
    float I;            /* 当前回路电流 (单位: A, 放电为正, 充电为负) */
    float Ut_measured;  /* 传感器实测的单体端电压 (单位: V) */
    float temp;         /* 电池当前温度 (单位: ℃) */
} ekf_input_t;

/* 2. 输出接口结构体(新增 bias 字段, 其余不变) */
typedef struct {
    float SOC;          /* 估算出的最优 SOC (范围: 0.0 ~ 1.0) */
    float U1;           /* 第一级 RC 极化电压估计值 (V) */
    float U2;           /* 第二级 RC 极化电压估计值 (V) */
    float bias;         /* 偏置状态 b (V): OCV/极化等效偏置的在线估计(诊断用) */
    float Ut_estimated; /* 算法内部预测的端电压 (V) */
    float error_Ut;     /* 实测电压与预测电压的残差 (V) */
} ekf_output_t;

/* 3. EKF 核心状态结构体(4 状态: x = [SOC, U1, U2, b]^T)
 * b 为慢时间常数的等效电压偏置, 在线吸收 OCV/极化的系统性模型误差
 * (如 LFP 充放电极化欠估 ~40mV、单体选择偏差等), 见
 *  docs/EKF充电精度分析_储能柜实测0923.md 第 10 节) */
typedef struct {
    /* 状态向量 x = [SOC, U1, U2, b]^T */
    float x[4];

    /* 协方差矩阵 P (4x4) */
    float P[4][4];

    /* 噪声参数 */
    float Q[4][4];      /* 过程噪声协方差 (Q[3][3] 为偏置状态 Q_b) */
    float R;            /* 测量噪声协方差 */

    /* 电池基本静态配置 */
    float Qn;           /* 电池额定容量 (单位: As, 安培秒 = Ah * 3600) */
    float Ts;           /* 采样周期 (单位: s, 通常为 1.0s) */
    float eta;          /* 库伦效率 */

    /* 偏置状态配置 */
    float bias_limit;   /* b 的限幅 (V), 工程保护, 默认 0.15 */
} ekf_instance_t;

/* 4. EKF 核心 API 接口声明(签名与 3 状态版完全一致, 调用方零修改) */

/**
 * @brief 初始化 EKF 滤波器参数
 * @param ekf 滤波器实例指针
 * @param init_soc 初始 SOC (0.0 ~ 1.0)
 * @param q_n 电池额定容量 (Ah)
 * @param t_s 采样周期 (s)
 */
void EKF_Init(ekf_instance_t *ekf, float init_soc, float q_n, float t_s);

/**
 * @brief EKF 核心迭代步骤（在定时器中断中每秒调用一次）
 * @param ekf 滤波器实例指针
 * @param input 传感器输入数据
 * @param output 算法输出结果
 */
void EKF_Update(ekf_instance_t *ekf, const ekf_input_t *input, ekf_output_t *output);


/* 5. 外部回调接口(不变) */
extern float EKF_GetOCV(float soc, float temp, float current);
extern float EKF_GetOCVSlope(float soc, float temp, float current);
extern void EKF_GetBatteryParams(float soc, float temp, float current, float *r0, float *r1, float *c1, float *r2, float *c2);

#ifdef __cplusplus
}
#endif

#endif /* EKF_BMS_H */
