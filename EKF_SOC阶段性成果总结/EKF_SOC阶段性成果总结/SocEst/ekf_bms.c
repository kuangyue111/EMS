#include "ekf_bms.h"
#include <math.h>

/* 算法初始化 */
void EKF_Init(ekf_instance_t *ekf, float init_soc, float q_n, float t_s) {
    if (ekf == 0) return;

    /* 1. 初始化状态向量 x = [SOC, U1, U2, b]^T (b 初始为 0, 无先验偏置) */
    ekf->x[0] = init_soc;
    ekf->x[1] = 0.0f;
    ekf->x[2] = 0.0f;
    ekf->x[3] = 0.0f;

    /* 2. 初始化协方差矩阵 P
     * SOC/U1/U2 不确定度较小; b 初始不确定度放大到 1e-2 (等效 ±100mV),
     * 允许偏置状态在初期快速收敛 */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ekf->P[i][j] = 0.0f;
        }
    }
    ekf->P[0][0] = 1.0e-4f;
    ekf->P[1][1] = 1.0e-4f;
    ekf->P[2][2] = 1.0e-4f;
    ekf->P[3][3] = 1.0e-2f;

    /* 3. 初始化过程噪声协方差矩阵 Q
     * Q_b = 1e-9: 偏置状态时间常数为分钟~小时级(慢), 只吸收系统性模型误差,
     * 不会跟踪快速变化(避免吞掉真实的 SOC 信息) */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ekf->Q[i][j] = 0.0f;
        }
    }
    ekf->Q[0][0] = 1.0e-6f;  /* SOC 噪声，设得很小 */
    ekf->Q[1][1] = 1.0e-5f;  /* U1 噪声 */
    ekf->Q[2][2] = 1.0e-5f;  /* U2 噪声 */
    ekf->Q[3][3] = 1.0e-9f;  /* b 噪声(慢) */

    /* 4. 初始化测量噪声协方差 R（取决于电压传感器的精度） */
    ekf->R = 0.01f; /* 对应约 10mV 噪声扰动 */

    /* 5. 电池基本参数与偏置配置 */
    ekf->Qn = q_n * 3600.0f; /* Ah 转换为 As */
    ekf->Ts = t_s;
    ekf->eta = 1.0f;         /* 默认效率为 1.0 */
    ekf->bias_limit = 0.15f; /* b 限幅 ±150mV(工程保护) */
}

/* 核心迭代更新 */
void EKF_Update(ekf_instance_t *ekf, const ekf_input_t *input, ekf_output_t *output) {
    if (ekf == 0 || input == 0 || output == 0) return;

    /* 局部变量定义 */
    float r0 = 0.0f, r1 = 0.0f, c1 = 0.0f, r2 = 0.0f, c2 = 0.0f;
    float phi1 = 0.0f, phi2 = 0.0f, g1 = 0.0f, g2 = 0.0f;

    /* 提取当前状态 */
    float soc_old = ekf->x[0];
    float u1_old  = ekf->x[1];
    float u2_old  = ekf->x[2];
    float b_old   = ekf->x[3];

    /*-------------------------------------------------------------------------
     * 步骤 A: 查表获取当前状态下的电池物理参数 (R0, R1, C1, R2, C2)
     *------------------------------------------------------------------------*/
    EKF_GetBatteryParams(soc_old, input->temp, input->I, &r0, &r1, &c1, &r2, &c2);

    /* 计算离散化状态转移参数 */
    phi1 = expf(-ekf->Ts / (r1 * c1));
    phi2 = expf(-ekf->Ts / (r2 * c2));
    g1 = r1 * (1.0f - phi1);
    g2 = r2 * (1.0f - phi2);

    /*-------------------------------------------------------------------------
     * 步骤 B: 时间更新 (Time Update / Prediction)
     *------------------------------------------------------------------------*/
    /* 1. 状态预测 x_minus (b 的转移矩阵为 1, 即 b_m = b_old) */
    float soc_minus = soc_old - (ekf->eta * ekf->Ts / ekf->Qn) * input->I;
    float u1_minus  = phi1 * u1_old + g1 * input->I;
    float u2_minus  = phi2 * u2_old + g2 * input->I;
    float b_minus   = b_old;

    /* 限制 SOC 物理边界在 0 ~ 1 之间 */
    if (soc_minus > 1.0f) soc_minus = 1.0f;
    if (soc_minus < 0.0f) soc_minus = 0.0f;

    /* 2. 协方差预测 P_minus = A * P * A^T + Q
     * A = diag(1, phi1, phi2, 1), 手写展开:
     * P_minus[i][j] = A[i]*P[i][j]*A[j] + Q[i][j] */
    float P_minus[4][4];
    P_minus[0][0] = ekf->P[0][0] + ekf->Q[0][0];
    P_minus[0][1] = ekf->P[0][1] * phi1;
    P_minus[0][2] = ekf->P[0][2] * phi2;
    P_minus[0][3] = ekf->P[0][3];

    P_minus[1][0] = phi1 * ekf->P[1][0];
    P_minus[1][1] = phi1 * ekf->P[1][1] * phi1 + ekf->Q[1][1];
    P_minus[1][2] = phi1 * ekf->P[1][2] * phi2;
    P_minus[1][3] = phi1 * ekf->P[1][3];

    P_minus[2][0] = phi2 * ekf->P[2][0];
    P_minus[2][1] = phi2 * ekf->P[2][1] * phi1;
    P_minus[2][2] = phi2 * ekf->P[2][2] * phi2 + ekf->Q[2][2];
    P_minus[2][3] = phi2 * ekf->P[2][3];

    P_minus[3][0] = ekf->P[3][0];
    P_minus[3][1] = ekf->P[3][1] * phi1;
    P_minus[3][2] = ekf->P[3][2] * phi2;
    P_minus[3][3] = ekf->P[3][3] + ekf->Q[3][3];

    /*-------------------------------------------------------------------------
     * 步骤 C: 状态更新与纠偏 (Measurement Update)
     *------------------------------------------------------------------------*/
    /* 1. 预测端电压 Ut_estimated (观测方程加入偏置项 +b) */
    float ocv = EKF_GetOCV(soc_minus, input->temp, input->I);
    float ut_est = ocv + b_minus - u1_minus - u2_minus - input->I * r0;

    /* 2. 计算端电压误差 (残差) */
    float error = input->Ut_measured - ut_est;

    /* 3. 构造雅可比观测矩阵 H = [d_ocv, -1, -1, +1] (?Ut/?b = +1) */
    float d_ocv = EKF_GetOCVSlope(soc_minus, input->temp, input->I);
    float H[4] = { d_ocv, -1.0f, -1.0f, 1.0f };

    /* 4. 计算卡尔曼增益 K = P_minus * H^T * (H * P_minus * H^T + R)^-1 */
    float M[4];
    for (int i = 0; i < 4; i++) {
        M[i] = P_minus[i][0] * H[0] + P_minus[i][1] * H[1]
             + P_minus[i][2] * H[2] + P_minus[i][3] * H[3];
    }

    /* 分母 S = H * P_minus * H^T + R (纯标量) */
    float S = H[0] * M[0] + H[1] * M[1] + H[2] * M[2] + H[3] * M[3] + ekf->R;

    /* 安全保护，防止分母除以零 */
    if (fabsf(S) < 1.0e-6f) {
        S = 1.0e-6f;
    }

    /* 卡尔曼增益向量 K (4x1, 标量除法) */
    float K[4];
    K[0] = M[0] / S;
    K[1] = M[1] / S;
    K[2] = M[2] / S;
    K[3] = M[3] / S;

    /* 5. 最优状态更新 x = x_minus + K * error */
    ekf->x[0] = soc_minus + K[0] * error;
    ekf->x[1] = u1_minus  + K[1] * error;
    ekf->x[2] = u2_minus  + K[2] * error;
    ekf->x[3] = b_minus   + K[3] * error;

    /* SOC 边界限制 */
    if (ekf->x[0] > 1.0f) ekf->x[0] = 1.0f;
    if (ekf->x[0] < 0.0f) ekf->x[0] = 0.0f;

    /* 偏置限幅(工程保护, 防止 b 发散吞掉 SOC 可观测性) */
    if (ekf->x[3] >  ekf->bias_limit) ekf->x[3] =  ekf->bias_limit;
    if (ekf->x[3] < -ekf->bias_limit) ekf->x[3] = -ekf->bias_limit;

    /* 6. 最优协方差更新 P = (I - K * H) * P_minus */
    float W[4][4];
    for (int i = 0; i < 4; i++) {
        W[i][0] = (i == 0 ? 1.0f : 0.0f) - K[i] * H[0];
        W[i][1] = (i == 1 ? 1.0f : 0.0f) - K[i] * H[1];
        W[i][2] = (i == 2 ? 1.0f : 0.0f) - K[i] * H[2];
        W[i][3] = (i == 3 ? 1.0f : 0.0f) - K[i] * H[3];
    }

    float P_new[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            P_new[i][j] = W[i][0] * P_minus[0][j] + W[i][1] * P_minus[1][j]
                        + W[i][2] * P_minus[2][j] + W[i][3] * P_minus[3][j];
        }
    }

    /* 保持协方差矩阵的对称性（防止单精度浮点累计误差导致发散） */
    for (int i = 0; i < 4; i++) {
        for (int j = i; j < 4; j++) {
            float val = (P_new[i][j] + P_new[j][i]) * 0.5f;
            ekf->P[i][j] = val;
            ekf->P[j][i] = val;
        }
    }

    /* 7. 输出给外部接口的数据 */
    output->SOC = ekf->x[0];
    output->U1 = ekf->x[1];
    output->U2 = ekf->x[2];
    output->bias = ekf->x[3];
    output->Ut_estimated = ut_est;
    output->error_Ut = error;
}
