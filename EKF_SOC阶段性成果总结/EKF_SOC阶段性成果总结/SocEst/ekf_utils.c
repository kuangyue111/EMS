#include "ekf_bms.h"
#include <math.h>
#include <stddef.h>
#include "ekf_params.h"
#include "ekf_ocv_spline.h"


/* =====================================================================
 * 电池参数(SOC/温度坐标轴、充放电阻容参数表、OCV 多项式系数)
 * 由 python/generate_ekf_params.py 从辨识结果 CSV 自动生成, 见 ekf_params.h
 * ===================================================================== */

/* =====================================================================
 * 2. 辅助算法：双线性插值核心引擎 (Bilinear Interpolation)
 * ===================================================================== */

/**
 * @brief 在一维升序数组中快速检索区间索引 (二分法检索，确定插值上下界) 
 */
static int FindIntervalIndex(const float *axis, int len, float val)
{
    if (val <= axis[0]) return 0;
    if (val >= axis[len - 1]) return len - 2;

    int low = 0;
    int high = len - 1;
    while ((high - low) > 1) {
        int mid = (low + high) / 2;
        if (axis[mid] > val) {
            high = mid;
        } else {
            low = mid;
        }
    }
    return low;
}

/**
 * @brief 2D 双线性插值计算
 */
static float BilinearInterpolation(const float *x_axis, int x_len,
                                   const float *y_axis, int y_len,
                                   const float *z_table, float x, float y)
{
    /* 输入物理限制保护 (Clamping) */
    if (x < x_axis[0]) x = x_axis[0];
    if (x > x_axis[x_len - 1]) x = x_axis[x_len - 1];
    if (y < y_axis[0]) y = y_axis[0];
    if (y > y_axis[y_len - 1]) y = y_axis[y_len - 1];

    /* 检索相邻区间的索引 */
    int i = FindIntervalIndex(x_axis, x_len, x);
    int j = FindIntervalIndex(y_axis, y_len, y);

    float x0 = x_axis[i];
    float x1 = x_axis[i + 1];
    float y0 = y_axis[j];
    float y1 = y_axis[j + 1];

    /* 取出相邻四个顶点的参数值 (Z 表数据一维化寻址) */
    float z00 = z_table[i * y_len + j];
    float z01 = z_table[i * y_len + (j + 1)];
    float z10 = z_table[(i + 1) * y_len + j];
    float z11 = z_table[(i + 1) * y_len + (j + 1)];

    /* 计算插值权重 */
    float t = (x - x0) / (x1 - x0);
    float u = (y - y0) / (y1 - y0);

    /* 双线性插值核心公式 */
    float z = (1.0f - t) * (1.0f - u) * z00 +
              t * (1.0f - u) * z10 +
              (1.0f - t) * u * z01 +
              t * u * z11;

    return z;
}

/* =====================================================================
 * 3. 接口实现
 * ===================================================================== */

/* =====================================================================
 * 双三次样条 OCV 求值/求导 (系数来自 ekf_ocv_spline.h, 自动生成)
 * ===================================================================== */

/**
 * @brief PCHIP 样条求值: SOC 方向 PCHIP(保单调三次), 温度方向线性
 * @param coeff 系数表, 布局 coeff[(temp_idx*OCV_SOC_INT + seg_idx)*4 + k], k=0..3 → c0..c3
 * @param soc   SOC (0~1, 已 clamp)
 * @param temp  温度 (℃, 已 clamp 到 [10,45])
 */
static float OCVSplineEval(const float *coeff, float soc, float temp)
{
    int i = FindIntervalIndex(s_ocv_soc_axis, OCV_SOC_N, soc);
    int j = FindIntervalIndex(s_ocv_temp_axis, OCV_TEMP_N, temp);

    float d = soc - s_ocv_soc_axis[i];   /* SOC 局部坐标 */
    float v = (temp - s_ocv_temp_axis[j]) / (s_ocv_temp_axis[j + 1] - s_ocv_temp_axis[j]);

    const float *a0 = &coeff[( j       * OCV_SOC_INT + i) * 4];
    const float *a1 = &coeff[((j + 1) * OCV_SOC_INT + i) * 4];

    /* PCHIP: f = c0 + c1*d + c2*d^2 + c3*d^3 (霍纳) */
    float f0 = a0[0] + d * (a0[1] + d * (a0[2] + d * a0[3]));
    float f1 = a1[0] + d * (a1[1] + d * (a1[2] + d * a1[3]));

    return f0 * (1.0f - v) + f1 * v;     /* 温度方向线性 */
}

/**
 * @brief PCHIP 样条对 SOC 求导 dOCV/dSOC
 * @note  df/dSOC = c1 + 2*c2*d + 3*c3*d^2, 温度方向线性
 */
static float OCVSplineSlope(const float *coeff, float soc, float temp)
{
    int i = FindIntervalIndex(s_ocv_soc_axis, OCV_SOC_N, soc);
    int j = FindIntervalIndex(s_ocv_temp_axis, OCV_TEMP_N, temp);

    float d = soc - s_ocv_soc_axis[i];
    float v = (temp - s_ocv_temp_axis[j]) / (s_ocv_temp_axis[j + 1] - s_ocv_temp_axis[j]);

    const float *a0 = &coeff[( j       * OCV_SOC_INT + i) * 4];
    const float *a1 = &coeff[((j + 1) * OCV_SOC_INT + i) * 4];

    float g0 = a0[1] + d * (2.0f * a0[2] + d * 3.0f * a0[3]);
    float g1 = a1[1] + d * (2.0f * a1[2] + d * 3.0f * a1[3]);

    return g0 * (1.0f - v) + g1 * v;
}

/**
 * @brief 根据当前 SOC、温度、电流方向获取 OCV (双三次样条, 充放电分开)
 * @param current 电流 (A, 放电为正/充电为负), 用于选择充/放电 OCV 表
 */
float EKF_GetOCV(float soc, float temp, float current)
{
    /* 边界限值保护 */
    if (soc < 0.0f) soc = 0.0f;
    if (soc > 1.0f) soc = 1.0f;
    if (temp < s_ocv_temp_axis[0]) temp = s_ocv_temp_axis[0];
    if (temp > s_ocv_temp_axis[OCV_TEMP_N - 1]) temp = s_ocv_temp_axis[OCV_TEMP_N - 1];

    /* 充电(I<0)用充电表, 放电(I>=0)用放电表 */
    const float *coeff = (current < 0.0f) ? s_ocv_coeff_chg : s_ocv_coeff_dis;

    return OCVSplineEval(coeff, soc, temp);
}

/**
 * @brief 计算 OCV 在当前 SOC 下的斜率 dOCV/dSOC (雅可比矩阵 H[0] 元素)
 * @note  对双三次样条在 SOC 方向解析求导, 并 clamp 下限 0.005 防止 LFP 平台区增益失效
 * @param current 电流 (A, 放电为正/充电为负), 用于选择充/放电 OCV 表
 */
float EKF_GetOCVSlope(float soc, float temp, float current)
{
    /* 边界限值保护 */
    if (soc < 0.0f) soc = 0.0f;
    if (soc > 1.0f) soc = 1.0f;
    if (temp < s_ocv_temp_axis[0]) temp = s_ocv_temp_axis[0];
    if (temp > s_ocv_temp_axis[OCV_TEMP_N - 1]) temp = s_ocv_temp_axis[OCV_TEMP_N - 1];

    const float *coeff = (current < 0.0f) ? s_ocv_coeff_chg : s_ocv_coeff_dis;

    float slope = OCVSplineSlope(coeff, soc, temp);

    /* 限制斜率下限, 防止 LFP 极度平坦区斜率接近 0 导致卡尔曼滤波增益完全失效 */
    if (slope < 0.005f) slope = 0.005f;
    return slope;
}

/**
 * @brief 2D 查表获取二阶 RC 模型的所有阻容参数
 * @param current 当前电流 (A, 放电为正 / 充电为负), 用于选择充/放电参数表
 */
void EKF_GetBatteryParams(float soc, float temp, float current,
                          float *r0, float *r1, float *c1, float *r2, float *c2)
{
    /* 空指针安全防御保护 */
    if ((r0 == NULL) || (r1 == NULL) || (c1 == NULL) || (r2 == NULL) || (c2 == NULL)) {
        return; 
    }

    /* 根据电流方向选择参数表: 放电(I>=0)用放电表, 充电(I<0)用充电表 */
    const float *r0_tab = (current < 0.0f) ? s_R0_table_chg : s_R0_table;
    const float *r1_tab = (current < 0.0f) ? s_R1_table_chg : s_R1_table;
    const float *c1_tab = (current < 0.0f) ? s_C1_table_chg : s_C1_table;
    const float *r2_tab = (current < 0.0f) ? s_R2_table_chg : s_R2_table;
    const float *c2_tab = (current < 0.0f) ? s_C2_table_chg : s_C2_table;

    /* 对每个阻容参数执行双线性插值 */
    *r0 = BilinearInterpolation(s_soc_axis, SOC_DIM, s_temp_axis, TEMP_DIM, r0_tab, soc, temp);
    *r1 = BilinearInterpolation(s_soc_axis, SOC_DIM, s_temp_axis, TEMP_DIM, r1_tab, soc, temp);
    *c1 = BilinearInterpolation(s_soc_axis, SOC_DIM, s_temp_axis, TEMP_DIM, c1_tab, soc, temp);
    *r2 = BilinearInterpolation(s_soc_axis, SOC_DIM, s_temp_axis, TEMP_DIM, r2_tab, soc, temp);
    *c2 = BilinearInterpolation(s_soc_axis, SOC_DIM, s_temp_axis, TEMP_DIM, c2_tab, soc, temp);
}
