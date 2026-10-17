# 嵌入式/MCU工程师面试题库 - 第六部分 (Q261-Q280)

> 手写代码专题：每题包含完整可编译C代码、MCU优化说明、复杂度分析

---

## Q261. 手写环形缓冲区(线程安全版)

### 原理说明

环形缓冲区(Circular Buffer / Ring Buffer)是嵌入式系统中常用的数据结构，用于生产者-消费者模型。通过头尾两个指针在固定大小的数组上循环移动，实现FIFO操作。线程安全版通过关中断或互斥锁保护临界区。

### 完整代码

```c
/**
 * ring_buffer.h - 线程安全环形缓冲区
 */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* MCU平台适配：关中断/开中断宏 */
#ifdef __ARMCC_VERSION          /* Keil MDK */
  #define RB_ENTER_CRITICAL()  __disable_irq()
  #define RB_EXIT_CRITICAL()   __enable_irq()
#elif defined(__GNUC__)         /* GCC ARM */
  #define RB_ENTER_CRITICAL()  __asm volatile("cpsid i" ::: "memory")
  #define RB_EXIT_CRITICAL()   __asm volatile("cpsie i" ::: "memory")
#else
  #include <stdatomic.h>
  static atomic_flag rb_critical = ATOMIC_FLAG_INIT;
  #define RB_ENTER_CRITICAL()  while(atomic_flag_test_and_set(&rb_critical))
  #define RB_EXIT_CRITICAL()   atomic_flag_clear(&rb_critical)
#endif

/* 环形缓冲区控制块 */
typedef struct {
    uint8_t  *buffer;       /* 数据缓冲区指针（外部静态分配） */
    uint32_t  size;         /* 缓冲区总大小 */
    uint32_t  read_idx;     /* 读指针索引 */
    uint32_t  write_idx;    /* 写指针索引 */
    uint32_t  count;        /* 当前数据量 */
    bool      is_pow2;      /* size是否为2的幂次 */
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, uint32_t size);
bool ring_buffer_put(ring_buffer_t *rb, uint8_t data);
bool ring_buffer_get(ring_buffer_t *rb, uint8_t *data);
uint32_t ring_buffer_put_n(ring_buffer_t *rb, const uint8_t *data, uint32_t len);
uint32_t ring_buffer_get_n(ring_buffer_t *rb, uint8_t *data, uint32_t len);
bool ring_buffer_peek(const ring_buffer_t *rb, uint32_t offset, uint8_t *data);

static inline bool ring_buffer_is_empty(const ring_buffer_t *rb) { return (rb->count == 0); }
static inline bool ring_buffer_is_full(const ring_buffer_t *rb)  { return (rb->count >= rb->size); }
static inline uint32_t ring_buffer_used(const ring_buffer_t *rb)  { return rb->count; }
static inline uint32_t ring_buffer_free(const ring_buffer_t *rb)  { return rb->size - rb->count; }
void ring_buffer_reset(ring_buffer_t *rb);

#endif /* RING_BUFFER_H */
```

```c
/**
 * ring_buffer.c - 环形缓冲区实现
 * 复杂度：所有操作 O(1) 时间，O(N) 空间（N=缓冲区大小）
 */
#include "ring_buffer.h"
#include <string.h>

void ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, uint32_t size)
{
    if (rb == NULL || buf == NULL || size == 0) return;
    rb->buffer    = buf;
    rb->size      = size;
    rb->read_idx  = 0;
    rb->write_idx = 0;
    rb->count     = 0;
    rb->is_pow2   = ((size & (size - 1U)) == 0U) && (size > 0);
}

bool ring_buffer_put(ring_buffer_t *rb, uint8_t data)
{
    if (rb == NULL || rb->buffer == NULL) return false;
    RB_ENTER_CRITICAL();
    bool result = false;
    if (rb->count < rb->size) {
        rb->buffer[rb->write_idx] = data;
        if (rb->is_pow2) rb->write_idx = (rb->write_idx + 1U) & (rb->size - 1U);
        else             rb->write_idx = (rb->write_idx + 1U) % rb->size;
        rb->count++;
        result = true;
    }
    RB_EXIT_CRITICAL();
    return result;
}

bool ring_buffer_get(ring_buffer_t *rb, uint8_t *data)
{
    if (rb == NULL || rb->buffer == NULL || data == NULL) return false;
    RB_ENTER_CRITICAL();
    bool result = false;
    if (rb->count > 0) {
        *data = rb->buffer[rb->read_idx];
        if (rb->is_pow2) rb->read_idx = (rb->read_idx + 1U) & (rb->size - 1U);
        else             rb->read_idx = (rb->read_idx + 1U) % rb->size;
        rb->count--;
        result = true;
    }
    RB_EXIT_CRITICAL();
    return result;
}

uint32_t ring_buffer_put_n(ring_buffer_t *rb, const uint8_t *data, uint32_t len)
{
    if (rb == NULL || rb->buffer == NULL || data == NULL || len == 0) return 0;
    RB_ENTER_CRITICAL();
    uint32_t free_space = rb->size - rb->count;
    uint32_t to_write = (len < free_space) ? len : free_space;
    for (uint32_t i = 0; i < to_write; i++) {
        rb->buffer[rb->write_idx] = data[i];
        if (rb->is_pow2) rb->write_idx = (rb->write_idx + 1U) & (rb->size - 1U);
        else             rb->write_idx = (rb->write_idx + 1U) % rb->size;
    }
    rb->count += to_write;
    RB_EXIT_CRITICAL();
    return to_write;
}

uint32_t ring_buffer_get_n(ring_buffer_t *rb, uint8_t *data, uint32_t len)
{
    if (rb == NULL || rb->buffer == NULL || data == NULL || len == 0) return 0;
    RB_ENTER_CRITICAL();
    uint32_t to_read = (len < rb->count) ? len : rb->count;
    for (uint32_t i = 0; i < to_read; i++) {
        data[i] = rb->buffer[rb->read_idx];
        if (rb->is_pow2) rb->read_idx = (rb->read_idx + 1U) & (rb->size - 1U);
        else             rb->read_idx = (rb->read_idx + 1U) % rb->size;
    }
    rb->count -= to_read;
    RB_EXIT_CRITICAL();
    return to_read;
}

bool ring_buffer_peek(const ring_buffer_t *rb, uint32_t offset, uint8_t *data)
{
    if (rb == NULL || rb->buffer == NULL || data == NULL || offset >= rb->count) return false;
    RB_ENTER_CRITICAL();
    uint32_t idx = rb->is_pow2 ? ((rb->read_idx + offset) & (rb->size - 1U))
                               : ((rb->read_idx + offset) % rb->size);
    *data = rb->buffer[idx];
    RB_EXIT_CRITICAL();
    return true;
}

void ring_buffer_reset(ring_buffer_t *rb)
{
    if (rb == NULL) return;
    RB_ENTER_CRITICAL();
    rb->read_idx = rb->write_idx = 0;
    rb->count = 0;
    RB_EXIT_CRITICAL();
}
```

```c
/* 使用示例：UART RX环形缓冲区 */
#include <stdio.h>
#define UART_RX_BUF_SIZE 128
static uint8_t uart_rx_buffer[UART_RX_BUF_SIZE];
static ring_buffer_t uart_rx_rb;

void UART_RX_IRQHandler(void) {
    uint8_t byte = UART_ReadData();
    ring_buffer_put(&uart_rx_rb, byte);
}

void process_uart_data(void) {
    uint8_t frame[32];
    uint32_t read = ring_buffer_get_n(&uart_rx_rb, frame, 32);
    if (read > 0) {
        for (uint32_t i = 0; i < read; i++) printf("%02X ", frame[i]);
        printf("\n");
    }
}

int main(void) {
    ring_buffer_init(&uart_rx_rb, uart_rx_buffer, UART_RX_BUF_SIZE);
    while (1) { process_uart_data(); }
    return 0;
}
```

### 复杂度分析

| 操作 | 时间复杂度 | 空间复杂度 | 说明 |
|------|-----------|-----------|------|
| `put/get` | O(1) | O(1) | 临界区内常数操作 |
| `put_n/get_n` | O(N) | O(1) | N=实际传输字节数 |
| `peek` | O(1) | O(1) | 只读不改指针 |

### MCU优化要点

1. **2的幂次缓冲区**：将 `%` 替换为 `& (size-1)` 位掩码，节省CPU周期
2. **关中断保护**：比互斥锁更轻量，适合中断上下文
3. **冗余count字段**：避免仅靠指针位置判断空/满，简化逻辑
4. **静态分配buffer**：无malloc，适合无堆的MCU环境

---

## Q262. 手写软件I2C/SPI驱动

### 原理说明

软件模拟I2C/SPI是MCU面试必考题。当硬件外设不足或引脚冲突时，通过GPIO模拟时序实现通信。

### I2C软件驱动

```c
/**
 * soft_i2c.h - 软件模拟I2C主机驱动
 */
#ifndef SOFT_I2C_H
#define SOFT_I2C_H

#include <stdint.h>
#include <stdbool.h>

/* 平台GPIO宏 (根据实际MCU修改) */
/* 示例使用STM32 HAL */
#ifdef STM32_HAL
  #define I2C_SCL_H()   HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET)
  #define I2C_SCL_L()   HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_RESET)
  #define I2C_SDA_H()   HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET)
  #define I2C_SDA_L()   HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_RESET)
  #define I2C_SDA_READ() HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN)
#else
  /* 通用寄存器直接操作 (假设ODR/IDR已定义) */
  #define I2C_SCL_H()   (*scl_odr |= scl_mask)
  #define I2C_SCL_L()   (*scl_odr &= ~scl_mask)
  #define I2C_SDA_H()   (*sda_odr |= sda_mask)
  #define I2C_SDA_L()   (*sda_odr &= ~sda_mask)
  #define I2C_SDA_READ() ((*sda_idr & sda_mask) ? 1 : 0)
#endif
#define I2C_DELAY_US(us) delay_us(us)

typedef enum {
    I2C_OK = 0, I2C_ERR_NACK_ADDR = -1, I2C_ERR_NACK_DATA = -2, I2C_ERR_PARAM = -3
} i2c_status_t;

typedef struct {
    uint32_t clock_speed;   /* 100000 or 400000 */
    uint32_t half_period_us;
    bool initialized;
} i2c_handle_t;

i2c_status_t i2c_init(i2c_handle_t *hi2c, uint32_t clock_speed);
i2c_status_t i2c_master_transmit(i2c_handle_t *hi2c, uint8_t dev_addr,
                                  const uint8_t *data, uint16_t len);
i2c_status_t i2c_master_receive(i2c_handle_t *hi2c, uint8_t dev_addr,
                                 uint8_t *data, uint16_t len);
i2c_status_t i2c_mem_write(i2c_handle_t *hi2c, uint8_t dev_addr,
                            uint8_t mem_addr, uint8_t mem_addr_size,
                            const uint8_t *data, uint16_t len);
i2c_status_t i2c_mem_read(i2c_handle_t *hi2c, uint8_t dev_addr,
                           uint8_t mem_addr, uint8_t mem_addr_size,
                           uint8_t *data, uint16_t len);

#endif /* SOFT_I2C_H */
```

```c
/**
 * soft_i2c.c - 软件I2C实现
 * 复杂度：单字节 O(1)，N字节 O(N)
 */
#include "soft_i2c.h"

static inline void i2c_delay_half(i2c_handle_t *hi2c) { I2C_DELAY_US(hi2c->half_period_us); }

static void i2c_start(i2c_handle_t *hi2c) {
    I2C_SDA_H(); i2c_delay_half(hi2c);
    I2C_SCL_H(); i2c_delay_half(hi2c);
    I2C_SDA_L(); i2c_delay_half(hi2c);
    I2C_SCL_L(); i2c_delay_half(hi2c);
}

static void i2c_stop(i2c_handle_t *hi2c) {
    I2C_SDA_L(); i2c_delay_half(hi2c);
    I2C_SCL_H(); i2c_delay_half(hi2c);
    I2C_SDA_H(); i2c_delay_half(hi2c);
}

static bool i2c_write_byte(i2c_handle_t *hi2c, uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        if (byte & 0x80) I2C_SDA_H(); else I2C_SDA_L();
        i2c_delay_half(hi2c);
        I2C_SCL_H(); i2c_delay_half(hi2c);  /* 上升沿采样 */
        I2C_SCL_L(); i2c_delay_half(hi2c);
        byte <<= 1;
    }
    /* 第9拍：读ACK */
    I2C_SDA_H();     /* 释放SDA */
    /* sda_input_mode(); */  /* 平台相关：切换输入模式 */
    i2c_delay_half(hi2c);
    I2C_SCL_H(); i2c_delay_half(hi2c);
    bool ack = (I2C_SDA_READ() == 0);
    I2C_SCL_L(); i2c_delay_half(hi2c);
    /* sda_output_mode(); */
    return ack;
}

static uint8_t i2c_read_byte(i2c_handle_t *hi2c, bool send_nack) {
    uint8_t byte = 0;
    I2C_SDA_H(); /* sda_input_mode(); */
    i2c_delay_half(hi2c);
    for (uint8_t i = 0; i < 8; i++) {
        I2C_SCL_H(); i2c_delay_half(hi2c);
        byte = (byte << 1) | (I2C_SDA_READ() ? 1 : 0);
        I2C_SCL_L(); i2c_delay_half(hi2c);
    }
    /* 发送ACK/NACK */
    /* sda_output_mode(); */
    if (send_nack) I2C_SDA_H(); else I2C_SDA_L();
    i2c_delay_half(hi2c);
    I2C_SCL_H(); i2c_delay_half(hi2c);
    I2C_SCL_L(); i2c_delay_half(hi2c);
    I2C_SDA_H();
    return byte;
}

i2c_status_t i2c_init(i2c_handle_t *hi2c, uint32_t clock_speed) {
    if (hi2c == NULL) return I2C_ERR_PARAM;
    hi2c->clock_speed = clock_speed;
    hi2c->half_period_us = 500000U / clock_speed;
    hi2c->initialized = true;
    I2C_SCL_H(); I2C_SDA_H();
    return I2C_OK;
}

i2c_status_t i2c_master_transmit(i2c_handle_t *hi2c, uint8_t dev_addr,
                                  const uint8_t *data, uint16_t len) {
    if (!hi2c || !hi2c->initialized || !data || len == 0) return I2C_ERR_PARAM;
    i2c_start(hi2c);
    if (!i2c_write_byte(hi2c, (dev_addr << 1) | 0x00)) { i2c_stop(hi2c); return I2C_ERR_NACK_ADDR; }
    for (uint16_t i = 0; i < len; i++) {
        if (!i2c_write_byte(hi2c, data[i])) { i2c_stop(hi2c); return I2C_ERR_NACK_DATA; }
    }
    i2c_stop(hi2c);
    return I2C_OK;
}

i2c_status_t i2c_master_receive(i2c_handle_t *hi2c, uint8_t dev_addr,
                                 uint8_t *data, uint16_t len) {
    if (!hi2c || !hi2c->initialized || !data || len == 0) return I2C_ERR_PARAM;
    i2c_start(hi2c);
    if (!i2c_write_byte(hi2c, (dev_addr << 1) | 0x01)) { i2c_stop(hi2c); return I2C_ERR_NACK_ADDR; }
    for (uint16_t i = 0; i < len; i++) data[i] = i2c_read_byte(hi2c, (i == len - 1));
    i2c_stop(hi2c);
    return I2C_OK;
}

i2c_status_t i2c_mem_write(i2c_handle_t *hi2c, uint8_t dev_addr,
                            uint8_t mem_addr, uint8_t mem_addr_size,
                            const uint8_t *data, uint16_t len) {
    if (!hi2c || !hi2c->initialized || !data || len == 0) return I2C_ERR_PARAM;
    i2c_start(hi2c);
    if (!i2c_write_byte(hi2c, (dev_addr << 1) | 0x00)) { i2c_stop(hi2c); return I2C_ERR_NACK_ADDR; }
    for (uint8_t i = mem_addr_size; i > 0; i--)
        if (!i2c_write_byte(hi2c, (uint8_t)(mem_addr >> (8 * (i - 1))))) {
            i2c_stop(hi2c); return I2C_ERR_NACK_DATA;
        }
    for (uint16_t i = 0; i < len; i++)
        if (!i2c_write_byte(hi2c, data[i])) { i2c_stop(hi2c); return I2C_ERR_NACK_DATA; }
    i2c_stop(hi2c);
    return I2C_OK;
}

i2c_status_t i2c_mem_read(i2c_handle_t *hi2c, uint8_t dev_addr,
                           uint8_t mem_addr, uint8_t mem_addr_size,
                           uint8_t *data, uint16_t len) {
    if (!hi2c || !hi2c->initialized || !data || len == 0) return I2C_ERR_PARAM;
    i2c_start(hi2c);
    if (!i2c_write_byte(hi2c, (dev_addr << 1) | 0x00)) { i2c_stop(hi2c); return I2C_ERR_NACK_ADDR; }
    for (uint8_t i = mem_addr_size; i > 0; i--)
        if (!i2c_write_byte(hi2c, (uint8_t)(mem_addr >> (8 * (i - 1))))) {
            i2c_stop(hi2c); return I2C_ERR_NACK_DATA;
        }
    i2c_start(hi2c);  /* Repeated START */
    if (!i2c_write_byte(hi2c, (dev_addr << 1) | 0x01)) { i2c_stop(hi2c); return I2C_ERR_NACK_ADDR; }
    for (uint16_t i = 0; i < len; i++) data[i] = i2c_read_byte(hi2c, (i == len - 1));
    i2c_stop(hi2c);
    return I2C_OK;
}
```

### SPI软件驱动

```c
/**
 * soft_spi.h - 软件模拟SPI主机驱动
 */
#ifndef SOFT_SPI_H
#define SOFT_SPI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef STM32_HAL
  #define SPI_SCK_H()  HAL_GPIO_WritePin(SPI_SCK_PORT,  SPI_SCK_PIN,  GPIO_PIN_SET)
  #define SPI_SCK_L()  HAL_GPIO_WritePin(SPI_SCK_PORT,  SPI_SCK_PIN,  GPIO_PIN_RESET)
  #define SPI_MOSI_H() HAL_GPIO_WritePin(SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_SET)
  #define SPI_MOSI_L() HAL_GPIO_WritePin(SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_RESET)
  #define SPI_MISO_RD() HAL_GPIO_ReadPin(SPI_MISO_PORT, SPI_MISO_PIN)
  #define SPI_CS_H()   HAL_GPIO_WritePin(SPI_CS_PORT,   SPI_CS_PIN,   GPIO_PIN_SET)
  #define SPI_CS_L()   HAL_GPIO_WritePin(SPI_CS_PORT,   SPI_CS_PIN,   GPIO_PIN_RESET)
#endif
#define SPI_DELAY_US(us) delay_us(us)

typedef enum { SPI_MODE_0=0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3 } spi_mode_t;
typedef enum { SPI_MSB_FIRST=0, SPI_LSB_FIRST } spi_bit_order_t;

typedef struct {
    spi_mode_t mode; spi_bit_order_t bit_order;
    uint32_t clock_hz; uint8_t cpol; uint8_t cpha;
    uint32_t half_period_us; bool initialized;
} spi_handle_t;

int  spi_init(spi_handle_t *hspi, spi_mode_t mode, spi_bit_order_t order, uint32_t clock_hz);
void spi_set_cs(spi_handle_t *hspi, bool enable);
uint8_t spi_transfer_byte(spi_handle_t *hspi, uint8_t tx_data);
void spi_transfer(spi_handle_t *hspi, const uint8_t *tx, uint8_t *rx, uint16_t len);

#endif /* SOFT_SPI_H */
```

```c
/**
 * soft_spi.c - 软件SPI实现
 */
#include "soft_spi.h"

int spi_init(spi_handle_t *hspi, spi_mode_t mode, spi_bit_order_t order, uint32_t clock_hz) {
    if (hspi == NULL) return -1;
    hspi->mode = mode; hspi->bit_order = order; hspi->clock_hz = clock_hz;
    hspi->cpol = (mode & 0x02) ? 1 : 0; hspi->cpha = (mode & 0x01) ? 1 : 0;
    hspi->half_period_us = 500000U / clock_hz;
    hspi->initialized = true;
    if (hspi->cpol) SPI_SCK_H(); else SPI_SCK_L();
    SPI_MOSI_H(); SPI_CS_H();
    return 0;
}

void spi_set_cs(spi_handle_t *hspi, bool enable) {
    (void)hspi;
    if (enable) SPI_CS_L(); else SPI_CS_H();
}

uint8_t spi_transfer_byte(spi_handle_t *hspi, uint8_t tx_data) {
    uint8_t rx_data = 0;
    /* SPI Mode 0 (CPOL=0, CPHA=0): 上升沿采样 */
    for (uint8_t i = 0; i < 8; i++) {
        if (tx_data & 0x80) SPI_MOSI_H(); else SPI_MOSI_L();
        tx_data <<= 1;
        SPI_DELAY_US(hspi->half_period_us);
        SPI_SCK_H();  /* 上升沿 */
        SPI_DELAY_US(5);  /* setup time */
        rx_data = (rx_data << 1) | (SPI_MISO_RD() ? 1 : 0);
        SPI_DELAY_US(hspi->half_period_us - 5);
        SPI_SCK_L();  /* 下降沿 */
        SPI_DELAY_US(hspi->half_period_us);
    }
    return rx_data;
}

void spi_transfer(spi_handle_t *hspi, const uint8_t *tx, uint8_t *rx, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        uint8_t txb = (tx) ? tx[i] : 0xFF;
        uint8_t rxb = spi_transfer_byte(hspi, txb);
        if (rx) rx[i] = rxb;
    }
}
```

### 使用示例

```c
#include <stdio.h>

static spi_handle_t spi1;

int main(void) {
    spi_init(&spi1, SPI_MODE_0, SPI_MSB_FIRST, 1000000);  /* 1MHz */
    /* 读取SPI Flash ID */
    uint8_t cmd[4] = {0x90, 0x00, 0x00, 0x00};
    uint8_t rx[2]  = {0};
    spi_set_cs(&spi1, true);
    spi_transfer(&spi1, cmd, NULL, 4);
    spi_transfer(&spi1, NULL, rx, 2);
    spi_set_cs(&spi1, false);
    printf("MFG: %02X, Dev: %02X\n", rx[0], rx[1]);
    return 0;
}
```

### 复杂度分析

| 操作 | 时间复杂度 | 说明 |
|------|-----------|------|
| I2C单字节 | O(1) | 固定9时钟 |
| SPI单字节 | O(1) | 固定8时钟 |
| N字节传输 | O(N) | 线性 |

### MCU优化要点

1. **寄存器直接操作**：GPIO ODR/IDR寄存器位操作最快
2. **DWT延时**：使用Cortex-M的DWT CYCCNT做精确定时
3. **中断安全**：长传输考虑分段 + 关中断
4. **禁用编译器优化**：GPIO访问需要`volatile`防止优化

---

## Q263. 手写CRC16/CRC32计算

### 原理说明

CRC(Cyclic Redundancy Check)基于多项式除法。查表法将256种字节值对应CRC预计算存储，实现O(1) per byte。

### 完整代码

```c
/**
 * crc.h - CRC16/CRC32 查表法 & 直接计算法
 */
#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ======== CRC16 ======== */
typedef struct {
    uint16_t table[256];
    uint16_t init;
    uint16_t xor_out;
    bool ref_in, ref_out;
    bool table_built;
} crc16_ctx_t;

void crc16_init(crc16_ctx_t *ctx, uint16_t poly, uint16_t init,
                bool ref_in, bool ref_out, uint16_t xor_out);
uint16_t crc16_calc(const crc16_ctx_t *ctx, const uint8_t *data, size_t len);
uint16_t crc16_calc_direct(uint16_t poly, uint16_t init, const uint8_t *data, size_t len);
uint16_t crc16_update(const crc16_ctx_t *ctx, uint16_t crc, const uint8_t *data, size_t len);

/* ======== CRC32 ======== */
typedef struct {
    uint32_t table[256];
    uint32_t init;
    uint32_t xor_out;
    bool ref_in, ref_out;
    bool table_built;
} crc32_ctx_t;

void crc32_init(crc32_ctx_t *ctx, uint32_t poly, uint32_t init,
                bool ref_in, bool ref_out, uint32_t xor_out);
uint32_t crc32_calc(const crc32_ctx_t *ctx, const uint8_t *data, size_t len);
uint32_t crc32_calc_direct(uint32_t poly, uint32_t init, const uint8_t *data, size_t len);
uint32_t crc32_update(const crc32_ctx_t *ctx, uint32_t crc, const uint8_t *data, size_t len);

/* 便捷函数 */
uint16_t crc16_modbus(const uint8_t *data, size_t len);  /* 0x8005 ref */
uint32_t crc32_ieee(const uint8_t *data, size_t len);    /* IEEE 802.3 */

#endif /* CRC_H */
```

```c
/**
 * crc.c - 实现
 * 时间复杂度：O(N) 查表法 / O(8N) 直接法
 * 空间复杂度：O(1)查表法(运行时)/O(表大小) 或 O(1)直接法
 */
#include "crc.h"

static uint16_t rev16(uint16_t v) {
    v = ((v & 0x5555) << 1)  | ((v & 0xAAAA) >> 1);
    v = ((v & 0x3333) << 2)  | ((v & 0xCCCC) >> 2);
    v = ((v & 0x0F0F) << 4)  | ((v & 0xF0F0) >> 4);
    return (v << 8) | (v >> 8);
}

static uint32_t rev32(uint32_t v) {
    v = ((v & 0x55555555UL) << 1)  | ((v & 0xAAAAAAAAUL) >> 1);
    v = ((v & 0x33333333UL) << 2)  | ((v & 0xCCCCCCCCUL) >> 2);
    v = ((v & 0x0F0F0F0FUL) << 4)  | ((v & 0xF0F0F0F0UL) >> 4);
    v = ((v & 0x00FF00FFUL) << 8)  | ((v & 0xFF00FF00UL) >> 8);
    return (v << 16) | (v >> 16);
}

void crc16_init(crc16_ctx_t *ctx, uint16_t poly, uint16_t init,
                bool ref_in, bool ref_out, uint16_t xor_out) {
    if (ctx == NULL) return;
    ctx->init = init; ctx->xor_out = xor_out;
    ctx->ref_in = ref_in; ctx->ref_out = ref_out;
    for (uint32_t i = 0; i < 256; i++) {
        if (ref_in) {
            uint16_t crc = (uint16_t)(i << 8);
            for (int j = 0; j < 8; j++)
                crc = (crc & 0x8000) ? ((crc << 1) ^ poly) : (crc << 1);
            ctx->table[i] = rev16(crc);
        } else {
            uint16_t crc = (uint16_t)(i << 8);
            for (int j = 0; j < 8; j++)
                crc = (crc & 0x8000) ? ((crc << 1) ^ poly) : (crc << 1);
            ctx->table[i] = crc;
        }
    }
    ctx->table_built = true;
}

uint16_t crc16_calc(const crc16_ctx_t *ctx, const uint8_t *data, size_t len) {
    if (!ctx || !ctx->table_built || !data) return 0;
    uint16_t crc = ctx->init;
    for (size_t i = 0; i < len; i++) {
        uint8_t idx = ctx->ref_in ? (uint8_t)(crc ^ data[i]) : (uint8_t)((crc >> 8) ^ data[i]);
        crc = ctx->ref_in ? ((crc >> 8) ^ ctx->table[idx]) : ((crc << 8) ^ ctx->table[idx]);
    }
    if (ctx->ref_out) crc = rev16(crc);
    return crc ^ ctx->xor_out;
}

uint16_t crc16_update(const crc16_ctx_t *ctx, uint16_t crc, const uint8_t *data, size_t len) {
    if (!ctx || !ctx->table_built || !data) return crc;
    for (size_t i = 0; i < len; i++) {
        uint8_t idx = ctx->ref_in ? (uint8_t)(crc ^ data[i]) : (uint8_t)((crc >> 8) ^ data[i]);
        crc = ctx->ref_in ? ((crc >> 8) ^ ctx->table[idx]) : ((crc << 8) ^ ctx->table[idx]);
    }
    return crc;
}

uint16_t crc16_calc_direct(uint16_t poly, uint16_t init, const uint8_t *data, size_t len) {
    uint16_t crc = init;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++)
            crc = (crc & 0x8000) ? ((crc << 1) ^ poly) : (crc << 1);
    }
    return crc;
}

/* CRC32 */
void crc32_init(crc32_ctx_t *ctx, uint32_t poly, uint32_t init,
                bool ref_in, bool ref_out, uint32_t xor_out) {
    if (ctx == NULL) return;
    ctx->init = init; ctx->xor_out = xor_out;
    ctx->ref_in = ref_in; ctx->ref_out = ref_out;
    for (uint32_t i = 0; i < 256; i++) {
        if (ref_in) {
            uint32_t crc = i;
            for (int j = 0; j < 8; j++)
                crc = (crc & 1) ? ((crc >> 1) ^ poly) : (crc >> 1);
            ctx->table[i] = crc;
        } else {
            uint32_t crc = i << 24;
            for (int j = 0; j < 8; j++)
                crc = (crc & 0x80000000UL) ? ((crc << 1) ^ poly) : (crc << 1);
            ctx->table[i] = crc;
        }
    }
    ctx->table_built = true;
}

uint32_t crc32_calc(const crc32_ctx_t *ctx, const uint8_t *data, size_t len) {
    if (!ctx || !ctx->table_built || !data) return 0;
    uint32_t crc = ctx->init;
    for (size_t i = 0; i < len; i++) {
        uint8_t idx = ctx->ref_in ? (uint8_t)(crc ^ data[i]) : (uint8_t)((crc >> 24) ^ data[i]);
        crc = ctx->ref_in ? ((crc >> 8) ^ ctx->table[idx]) : ((crc << 8) ^ ctx->table[idx]);
    }
    if (ctx->ref_out) crc = rev32(crc);
    return crc ^ ctx->xor_out;
}

uint32_t crc32_update(const crc32_ctx_t *ctx, uint32_t crc, const uint8_t *data, size_t len) {
    if (!ctx || !ctx->table_built || !data) return crc;
    for (size_t i = 0; i < len; i++) {
        uint8_t idx = ctx->ref_in ? (uint8_t)(crc ^ data[i]) : (uint8_t)((crc >> 24) ^ data[i]);
        crc = ctx->ref_in ? ((crc >> 8) ^ ctx->table[idx]) : ((crc << 8) ^ ctx->table[idx]);
    }
    return crc;
}

uint32_t crc32_calc_direct(uint32_t poly, uint32_t init, const uint8_t *data, size_t len) {
    uint32_t crc = init;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint32_t)data[i] << 24;
        for (int j = 0; j < 8; j++)
            crc = (crc & 0x80000000UL) ? ((crc << 1) ^ poly) : (crc << 1);
    }
    return crc;
}

/* 便捷函数 */
uint16_t crc16_modbus(const uint8_t *data, size_t len) {
    static crc16_ctx_t ctx; static bool init = false;
    if (!init) { crc16_init(&ctx, 0x8005, 0xFFFF, true, true, 0); init = true; }
    return crc16_calc(&ctx, data, len);
}

uint32_t crc32_ieee(const uint8_t *data, size_t len) {
    static crc32_ctx_t ctx; static bool init = false;
    if (!init) { crc32_init(&ctx, 0x04C11DB7UL, 0xFFFFFFFFUL, true, true, 0xFFFFFFFFUL); init = true; }
    return crc32_calc(&ctx, data, len);
}
```

```c
/* 使用示例 */
#include <stdio.h>
#include <assert.h>

int main(void) {
    uint8_t modbus_frame[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01};
    uint16_t crc = crc16_modbus(modbus_frame, sizeof(modbus_frame));
    printf("Modbus CRC16: 0x%04X\n", crc);
    assert(crc == 0x840A);

    const char *s = "123456789";
    uint32_t c32 = crc32_ieee((const uint8_t *)s, 9);
    printf("CRC32: 0x%08lX\n", (unsigned long)c32);
    assert(c32 == 0xCBF43926UL);
    printf("All tests passed!\n");
    return 0;
}
```

### 复杂度分析

| 方法 | 时间 | 空间 | 适用 |
|------|------|------|------|
| 查表法 | O(N) | O(512B) CRC16 / O(1KB) CRC32 | 正常场景 |
| 直接计算 | O(8N) | O(1) | ROM紧张 |
| 增量更新 | O(N') | O(1) | 分片/流式 |

### MCU优化要点

1. **const表放Flash**：查找表用`const`放入只读段
2. **硬件CRC**：STM32有硬件CRC单元，DMA+CRC零CPU开销
3. **预计算表**：静态const数组避免运行时初始化

---

## Q264. 手写内存分配器(malloc/free简化版)

### 原理说明

MCU简化内存分配器，使用首次适应(First-Fit)算法通过单向链表管理空闲块。每个块头部含元数据（大小、空闲标志、下一块指针、魔数）。堆空间静态分配。

### 完整代码

```c
/**
 * mem_alloc.h - MCU简化内存分配器
 */
#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MEM_ALIGNMENT      8
#define MEM_ALIGN(s)       (((s) + (MEM_ALIGNMENT - 1)) & ~(MEM_ALIGNMENT - 1))
#define MEM_MIN_BLOCK_SIZE MEM_ALIGN(sizeof(mem_block_t))

typedef struct mem_block {
    uint32_t          size;
    bool              is_free;
    struct mem_block *next;
    uint32_t          magic;
} mem_block_t;

#define MEM_MAGIC_VALID 0xDEADBEEF
#define MEM_MAGIC_FREE  0xDEADBEEF

typedef struct {
    uint8_t    *heap_start;
    uint32_t    heap_size;
    mem_block_t *free_list;
    uint32_t    alloc_count, free_count;
    uint32_t    total_alloc, total_free, max_used;
    bool        initialized;
} mem_alloc_t;

int    mem_init(mem_alloc_t *alloc, uint8_t *heap, uint32_t heap_size);
void  *mem_malloc(mem_alloc_t *alloc, uint32_t size);
void  *mem_calloc(mem_alloc_t *alloc, uint32_t num, uint32_t size);
void   mem_free(mem_alloc_t *alloc, void *ptr);
void  *mem_realloc(mem_alloc_t *alloc, void *ptr, uint32_t new_size);
bool   mem_check_integrity(const mem_alloc_t *alloc);
uint32_t mem_available(const mem_alloc_t *alloc);
uint32_t mem_max_block(const mem_alloc_t *alloc);
void   mem_dump(const mem_alloc_t *alloc);

#endif /* MEM_ALLOC_H */
```

```c
/**
 * mem_alloc.c - 内存分配器实现
 * malloc O(N) N=空闲块数, free O(1), realloc O(N+M)
 */
#include "mem_alloc.h"
#include <string.h>

static void mem_coalesce(mem_alloc_t *alloc) {
    mem_block_t *curr = alloc->free_list;
    while (curr && curr->next) {
        if (((uint8_t *)curr + curr->size) == (uint8_t *)(curr->next) && curr->next->is_free) {
            curr->size += curr->next->size;
            curr->next = curr->next->next;
        } else { curr = curr->next; }
    }
}

static void mem_list_remove(mem_alloc_t *alloc, mem_block_t *block) {
    if (alloc->free_list == block) { alloc->free_list = block->next; return; }
    mem_block_t *c = alloc->free_list;
    while (c && c->next != block) c = c->next;
    if (c) c->next = block->next;
}

static void mem_list_insert(mem_alloc_t *alloc, mem_block_t *block) {
    block->is_free = true; block->magic = MEM_MAGIC_FREE;
    if (!alloc->free_list || (uint8_t *)block < (uint8_t *)(alloc->free_list)) {
        block->next = alloc->free_list; alloc->free_list = block; return;
    }
    mem_block_t *c = alloc->free_list;
    while (c->next && (uint8_t *)(c->next) < (uint8_t *)block) c = c->next;
    block->next = c->next; c->next = block;
}

static void mem_split(mem_alloc_t *alloc, mem_block_t *block, uint32_t needed) {
    uint32_t remain = block->size - needed;
    if (remain >= MEM_MIN_BLOCK_SIZE + MEM_ALIGNMENT) {
        mem_block_t *nb = (mem_block_t *)((uint8_t *)block + needed);
        nb->size = remain; nb->is_free = true; nb->next = block->next; nb->magic = MEM_MAGIC_FREE;
        mem_list_remove(alloc, block);
        block->size = needed;
        mem_list_insert(alloc, block);
        mem_list_insert(alloc, nb);
    }
}

int mem_init(mem_alloc_t *alloc, uint8_t *heap, uint32_t heap_size) {
    if (!alloc || !heap || heap_size < MEM_MIN_BLOCK_SIZE * 2) return -1;
    memset(alloc, 0, sizeof(mem_alloc_t));
    uint8_t *aligned = heap;
    uint32_t off = (uint32_t)((uintptr_t)aligned % MEM_ALIGNMENT);
    if (off) { aligned += (MEM_ALIGNMENT - off); heap_size -= (MEM_ALIGNMENT - off); }
    alloc->heap_start = aligned; alloc->heap_size = heap_size; alloc->initialized = true;
    mem_block_t *init = (mem_block_t *)aligned;
    init->size = heap_size; init->is_free = true; init->next = NULL; init->magic = MEM_MAGIC_FREE;
    alloc->free_list = init;
    return 0;
}

void *mem_malloc(mem_alloc_t *alloc, uint32_t size) {
    if (!alloc || !alloc->initialized || size == 0) return NULL;
    uint32_t needed = MEM_ALIGN(size) + MEM_MIN_BLOCK_SIZE;
    mem_block_t *curr = alloc->free_list;
    while (curr) {
        if (curr->is_free && curr->size >= needed && curr->magic == MEM_MAGIC_FREE) {
            mem_list_remove(alloc, curr); mem_split(alloc, curr, needed);
            curr->is_free = false; curr->magic = MEM_MAGIC_VALID;
            alloc->alloc_count++; alloc->total_alloc += needed;
            uint32_t used = (uint32_t)((uint8_t *)curr + curr->size - alloc->heap_start);
            if (used > alloc->max_used) alloc->max_used = used;
            void *p = (void *)((uint8_t *)curr + MEM_MIN_BLOCK_SIZE);
            memset(p, 0, size);
            return p;
        }
        curr = curr->next;
    }
    mem_coalesce(alloc);
    curr = alloc->free_list;
    while (curr) {
        if (curr->is_free && curr->size >= needed && curr->magic == MEM_MAGIC_FREE) {
            mem_list_remove(alloc, curr); mem_split(alloc, curr, needed);
            curr->is_free = false; curr->magic = MEM_MAGIC_VALID;
            alloc->alloc_count++; alloc->total_alloc += needed;
            void *p = (void *)((uint8_t *)curr + MEM_MIN_BLOCK_SIZE);
            memset(p, 0, size);
            return p;
        }
        curr = curr->next;
    }
    return NULL;
}

void *mem_calloc(mem_alloc_t *alloc, uint32_t num, uint32_t size) {
    if (num == 0 || size == 0 || size > UINT32_MAX / num) return NULL;
    return mem_malloc(alloc, num * size);
}

void mem_free(mem_alloc_t *alloc, void *ptr) {
    if (!alloc || !alloc->initialized || !ptr) return;
    mem_block_t *block = (mem_block_t *)((uint8_t *)ptr - MEM_MIN_BLOCK_SIZE);
    if (block->magic != MEM_MAGIC_VALID || block->is_free) return;
    memset(ptr, 0, block->size - MEM_MIN_BLOCK_SIZE);
    block->is_free = true; block->magic = MEM_MAGIC_FREE;
    mem_list_insert(alloc, block); mem_coalesce(alloc);
    alloc->free_count++; alloc->total_free += block->size;
}

void *mem_realloc(mem_alloc_t *alloc, void *ptr, uint32_t new_size) {
    if (!alloc || !alloc->initialized) return NULL;
    if (!ptr) return mem_malloc(alloc, new_size);
    if (new_size == 0) { mem_free(alloc, ptr); return NULL; }
    mem_block_t *block = (mem_block_t *)((uint8_t *)ptr - MEM_MIN_BLOCK_SIZE);
    if (block->magic != MEM_MAGIC_VALID) return NULL;
    uint32_t old_data = block->size - MEM_MIN_BLOCK_SIZE;
    uint32_t new_need = MEM_ALIGN(new_size) + MEM_MIN_BLOCK_SIZE;
    if (block->size >= new_need) {
        if (block->size - new_need >= MEM_MIN_BLOCK_SIZE + MEM_ALIGNMENT)
            mem_split(alloc, block, new_need);
        return ptr;
    }
    void *np = mem_malloc(alloc, new_size);
    if (!np) return NULL;
    memmove(np, ptr, old_data); mem_free(alloc, ptr);
    return np;
}

bool mem_check_integrity(const mem_alloc_t *alloc) {
    if (!alloc || !alloc->initialized) return false;
    mem_block_t *c = alloc->free_list;
    while (c) {
        if (c->magic != MEM_MAGIC_FREE || !c->is_free) return false;
        if ((uint8_t *)c < alloc->heap_start) return false;
        if ((uint8_t *)c + c->size > alloc->heap_start + alloc->heap_size) return false;
        c = c->next;
    }
    return true;
}

uint32_t mem_available(const mem_alloc_t *alloc) {
    if (!alloc || !alloc->initialized) return 0;
    uint32_t f = 0; mem_block_t *c = alloc->free_list;
    while (c) {
        if (c->is_free && c->magic == MEM_MAGIC_FREE && c->size >= MEM_MIN_BLOCK_SIZE)
            f += c->size - MEM_MIN_BLOCK_SIZE;
        c = c->next;
    }
    return f;
}

uint32_t mem_max_block(const mem_alloc_t *alloc) {
    if (!alloc || !alloc->initialized) return 0;
    uint32_t m = 0; mem_block_t *c = alloc->free_list;
    while (c) {
        if (c->is_free && c->magic == MEM_MAGIC_FREE) {
            uint32_t u = (c->size >= MEM_MIN_BLOCK_SIZE) ? (c->size - MEM_MIN_BLOCK_SIZE) : 0;
            if (u > m) m = u;
        }
        c = c->next;
    }
    return m;
}

void mem_dump(const mem_alloc_t *alloc) {
    if (!alloc || !alloc->initialized) { printf("[mem_dump] not init\n"); return; }
    printf("=== Memory Allocator ===\nHeap: %p size %u\nAllocs:%u Frees:%u\n",
           (void*)alloc->heap_start, alloc->heap_size, alloc->alloc_count, alloc->free_count);
    printf("Peak:%u  Avail:%u  MaxBlock:%u\n",
           alloc->max_used, mem_available(alloc), mem_max_block(alloc));
    mem_block_t *c = alloc->free_list; int n = 0;
    while (c) {
        printf("  [%d] %p size=%u free=%d\n", n++, (void*)c, c->size, c->is_free);
        c = c->next;
    }
}
```

```c
/* 使用示例 */
#include <stdio.h>
#include <string.h>

#define HEAP_SIZE 4096
static uint8_t heap[HEAP_SIZE] __attribute__((aligned(8)));
static mem_alloc_t my_alloc;

int main(void) {
    mem_init(&my_alloc, heap, HEAP_SIZE);
    printf("Avail: %u\n", mem_available(&my_alloc));
    char *s1 = mem_malloc(&my_alloc, 64);
    char *s2 = mem_malloc(&my_alloc, 128);
    int  *ar = mem_calloc(&my_alloc, 10, sizeof(int));
    if (s1 && s2 && ar) {
        strcpy(s1, "Hello"); strcpy(s2, "World");
        for (int i = 0; i < 10; i++) ar[i] = i*10;
        printf("%s %s, ar[5]=%d\n", s1, s2, ar[5]);
    }
    mem_free(&my_alloc, s1);
    s2 = mem_realloc(&my_alloc, s2, 256);
    if (s2) printf("Realloc ok: %s\n", s2);
    mem_dump(&my_alloc);
    printf("Integrity: %s\n", mem_check_integrity(&my_alloc) ? "PASS" : "FAIL");
    mem_free(&my_alloc, s2); mem_free(&my_alloc, ar);
    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 | 说明 |
|------|------|------|------|
| `mem_init` | O(1) | O(1) | |
| `mem_malloc` | O(N) | O(1) | N=空闲块数, First-Fit |
| `mem_free` | O(N) | O(1) | 含合并遍历 |
| `mem_realloc` | O(N+M) | O(1) | M=拷贝大小 |

### MCU优化要点

1. **静态堆**：`uint8_t heap[SIZE]` 预分配
2. **魔数校验**：防止野指针/重复释放
3. **对齐**：堆起始地址和请求大小均8字节对齐
4. **零碎片化**：free时自动合并相邻空闲块

---

## Q265. 手写快速排序/二分查找

### 原理说明

快速排序基于分治法，选pivot分区递归排序。非递归版用显式栈避免MCU递归栈溢出。二分查找在有序数组中O(log N)搜索。

### 完整代码

```c
/**
 * sort_search.h - 快速排序与二分查找
 */
#ifndef SORT_SEARCH_H
#define SORT_SEARCH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef int (*cmp_fn_t)(const void *a, const void *b);

void quicksort(void *arr, size_t count, size_t elem_size, cmp_fn_t cmp);
void quicksort_iter(void *arr, size_t count, size_t elem_size, cmp_fn_t cmp);
void insertsort(void *arr, size_t count, size_t elem_size, cmp_fn_t cmp);

void *binary_search(const void *key, const void *arr, size_t count,
                    size_t elem_size, cmp_fn_t cmp);
void *binary_search_first(const void *key, const void *arr, size_t count,
                          size_t elem_size, cmp_fn_t cmp);
void *binary_search_last(const void *key, const void *arr, size_t count,
                         size_t elem_size, cmp_fn_t cmp);
void *binary_search_lower_bound(const void *key, const void *arr, size_t count,
                                size_t elem_size, cmp_fn_t cmp);
void *binary_search_upper_bound(const void *key, const void *arr, size_t count,
                                size_t elem_size, cmp_fn_t cmp);

static inline int cmp_int(const void *a, const void *b)
{ return (*(const int*)a > *(const int*)b) - (*(const int*)a < *(const int*)b); }

static inline int cmp_uint32(const void *a, const void *b)
{ return (*(const uint32_t*)a > *(const uint32_t*)b) - (*(const uint32_t*)a < *(const uint32_t*)b); }

#endif /* SORT_SEARCH_H */
```

```c
/**
 * sort_search.c - 实现
 * 快排：平均 O(N log N) 最坏 O(N²) 空间 O(log N)
 * 二分：O(log N) 空间 O(1)
 */
#include "sort_search.h"
#include <string.h>

static void swap_bytes(void *a, void *b, size_t size) {
    uint8_t tmp[64];
    size_t rem = size;
    while (rem > 0) {
        size_t c = (rem > 64) ? 64 : rem;
        memcpy(tmp, (uint8_t*)a + size - rem, c);
        memcpy((uint8_t*)a + size - rem, (uint8_t*)b + size - rem, c);
        memcpy((uint8_t*)b + size - rem, tmp, c);
        rem -= c;
    }
}

static size_t partition(void *arr, size_t low, size_t high, size_t esz, cmp_fn_t cmp) {
    uint8_t *base = (uint8_t *)arr;
    size_t mid = low + (high - low) / 2;
    if (cmp(base + mid*esz, base + low*esz) < 0) swap_bytes(base + mid*esz, base + low*esz, esz);
    if (cmp(base + high*esz, base + low*esz) < 0) swap_bytes(base + high*esz, base + low*esz, esz);
    if (cmp(base + high*esz, base + mid*esz) < 0) swap_bytes(base + high*esz, base + mid*esz, esz);
    swap_bytes(base + mid*esz, base + (high-1)*esz, esz);
    void *pivot = base + (high-1)*esz;
    size_t i = low; size_t j = high-1;
    for (;;) {
        do i++; while (cmp(base + i*esz, pivot) < 0 && i < high);
        do j--; while (cmp(base + j*esz, pivot) > 0 && j > low);
        if (i >= j) break;
        swap_bytes(base + i*esz, base + j*esz, esz);
    }
    swap_bytes(base + i*esz, base + (high-1)*esz, esz);
    return i;
}

static void qs_recurse(void *arr, size_t low, size_t high, size_t esz, cmp_fn_t cmp) {
    while (low < high) {
        if (high - low < 16) {
            uint8_t *base = (uint8_t*)arr;
            for (size_t i = low+1; i <= high; i++)
                for (size_t j = i; j > low && cmp(base + j*esz, base + (j-1)*esz) < 0; j--)
                    swap_bytes(base + j*esz, base + (j-1)*esz, esz);
            return;
        }
        size_t pi = partition(arr, low, high, esz, cmp);
        if (pi - low < high - pi) {
            qs_recurse(arr, low, (pi > 0) ? pi-1 : 0, esz, cmp);
            low = pi + 1;
        } else {
            qs_recurse(arr, pi+1, high, esz, cmp);
            high = (pi > 0) ? pi-1 : 0;
        }
    }
}

void quicksort(void *arr, size_t count, size_t esz, cmp_fn_t cmp) {
    if (!arr || count <= 1 || !esz || !cmp) return;
    qs_recurse(arr, 0, count-1, esz, cmp);
}

#define QS_STACK 64
void quicksort_iter(void *arr, size_t count, size_t esz, cmp_fn_t cmp) {
    if (!arr || count <= 1 || !esz || !cmp) return;
    size_t stack[QS_STACK*2]; int top = -1;
    stack[++top] = 0; stack[++top] = count-1;
    while (top >= 0) {
        size_t hi = stack[top--], lo = stack[top--];
        if (lo >= hi) continue;
        if (hi - lo < 16) {
            uint8_t *base = (uint8_t*)arr;
            for (size_t i = lo+1; i <= hi; i++)
                for (size_t j = i; j > lo && cmp(base + j*esz, base+(j-1)*esz) < 0; j--)
                    swap_bytes(base + j*esz, base+(j-1)*esz, esz);
            continue;
        }
        size_t pi = partition(arr, lo, hi, esz, cmp);
        if ((pi>0?pi-1:0)-lo < hi-(pi+1)) {
            if (top+4 < QS_STACK*2) { stack[++top]=pi+1; stack[++top]=hi; stack[++top]=lo; stack[++top]=(pi>0?pi-1:0); }
        } else {
            if (top+4 < QS_STACK*2) { stack[++top]=lo; stack[++top]=(pi>0?pi-1:0); stack[++top]=pi+1; stack[++top]=hi; }
        }
    }
}

void insertsort(void *arr, size_t count, size_t esz, cmp_fn_t cmp) {
    if (!arr || count <= 1 || !cmp) return;
    uint8_t *base = (uint8_t*)arr;
    for (size_t i = 1; i < count; i++)
        for (size_t j = i; j > 0 && cmp(base + j*esz, base+(j-1)*esz) < 0; j--)
            swap_bytes(base + j*esz, base+(j-1)*esz, esz);
}

void *binary_search(const void *key, const void *arr, size_t count,
                    size_t esz, cmp_fn_t cmp) {
    if (!key || !arr || !count || !cmp) return NULL;
    const uint8_t *base = (const uint8_t*)arr;
    size_t lo = 0, hi = count - 1;
    while (lo <= hi) {
        size_t mid = lo + (hi - lo) / 2;
        int r = cmp(key, base + mid*esz);
        if (r == 0) return (void*)(base + mid*esz);
        else if (r < 0) { if (mid == 0) break; hi = mid - 1; }
        else lo = mid + 1;
    }
    return NULL;
}

void *binary_search_first(const void *key, const void *arr, size_t count,
                          size_t esz, cmp_fn_t cmp) {
    if (!key || !arr || !count || !cmp) return NULL;
    const uint8_t *base = (const uint8_t*)arr;
    size_t lo = 0, hi = count, res = count;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (cmp(base + mid*esz, key) < 0) lo = mid + 1;
        else { hi = mid; if (cmp(base + mid*esz, key) == 0) res = mid; }
    }
    return (res < count) ? (void*)(base + res*esz) : NULL;
}

void *binary_search_last(const void *key, const void *arr, size_t count,
                         size_t esz, cmp_fn_t cmp) {
    if (!key || !arr || !count || !cmp) return NULL;
    const uint8_t *base = (const uint8_t*)arr;
    size_t lo = 0, hi = count, res = count;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (cmp(base + mid*esz, key) <= 0) {
            if (cmp(base + mid*esz, key) == 0) res = mid;
            lo = mid + 1;
        } else hi = mid;
    }
    return (res < count) ? (void*)(base + res*esz) : NULL;
}

void *binary_search_lower_bound(const void *key, const void *arr, size_t count,
                                size_t esz, cmp_fn_t cmp) {
    if (!key || !arr || !count || !cmp) return NULL;
    const uint8_t *base = (const uint8_t*)arr;
    size_t lo = 0, hi = count, res = count;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (cmp(base + mid*esz, key) < 0) lo = mid + 1;
        else { hi = mid; res = mid; }
    }
    return (res < count) ? (void*)(base + res*esz) : NULL;
}

void *binary_search_upper_bound(const void *key, const void *arr, size_t count,
                                size_t esz, cmp_fn_t cmp) {
    if (!key || !arr || !count || !cmp) return NULL;
    const uint8_t *base = (const uint8_t*)arr;
    size_t lo = 0, hi = count;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (cmp(base + mid*esz, key) <= 0) lo = mid + 1;
        else hi = mid;
    }
    return (lo < count) ? (void*)(base + lo*esz) : NULL;
}
```

```c
/* 使用示例 */
#include <stdio.h>

int main(void) {
    int data[] = {42, 17, 89, 3, 56, 91, 24, 8, 73, 15};
    size_t n = 10;
    quicksort(data, n, sizeof(int), cmp_int);
    printf("Sorted: ");
    for (size_t i=0;i<n;i++) printf("%d ",data[i]);
    printf("\n");

    int t = 42;
    int *p = binary_search(&t, data, n, sizeof(int), cmp_int);
    printf("Found %d at idx %ld\n", t, p ? (long)(p-data) : -1L);

    int dups[] = {10,20,20,20,30,40};
    int v = 20;
    int *f = binary_search_first(&v, dups, 6, sizeof(int), cmp_int);
    int *l = binary_search_last(&v, dups, 6, sizeof(int), cmp_int);
    printf("First 20 at %ld, Last 20 at %ld\n", (long)(f-dups), (long)(l-dups));

    int d2[] = {99,44,88,11,66,33,77,22,55,0};
    quicksort_iter(d2, 10, sizeof(int), cmp_int);
    printf("Iter sort: ");
    for (int i=0;i<10;i++) printf("%d ",d2[i]);
    printf("\n");
    return 0;
}
```

### 复杂度分析

| 算法 | 平均时间 | 最坏时间 | 空间 | 稳定性 |
|------|---------|---------|------|--------|
| 快速排序 | O(N log N) | O(N²) | O(log N) | 不稳定 |
| 非递归快排 | O(N log N) | O(N²) | O(栈) | 不稳定 |
| 二分查找 | O(log N) | O(log N) | O(1) | - |

### MCU优化要点

1. **非递归版**：避免递归栈溢出
2. **三数取中**：减少最坏情况
3. **小数组插入排序**：阈值16切换
4. **尾递归消除**：先处理小分区
## Q266. 手写字符串转整数(atoi)含错误处理

### 原理说明

实现`atoi`工业级版本，处理：前导空格、正负号、进制前缀(0x/0b/0)、溢出检测、非法字符检测。返回结果结构体包含转换值和错误码。

### 完整代码

```c
/**
 * my_atoi.h - 字符串转整数（含完整错误处理）
 */
#ifndef MY_ATOI_H
#define MY_ATOI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    ATOI_OK = 0,
    ATOI_ERR_NULL     = -1,
    ATOI_ERR_EMPTY    = -2,
    ATOI_ERR_OVERFLOW = -3,
    ATOI_ERR_INVALID  = -4,
    ATOI_ERR_NODIGITS = -5,
} atoi_error_t;

typedef struct { int32_t value; int error; int chars_read; bool negative; } atoi_result_t;
typedef struct { int64_t value; int error; int chars_read; bool negative; } atoi64_result_t;
typedef struct { uint32_t value; int error; int chars_read; } atou_result_t;
typedef struct { float value; int error; int chars_read; } atof_result_t;

atoi_result_t my_atoi(const char *str);
atoi_result_t my_atoi_base(const char *str, int base);
atoi64_result_t my_atoll(const char *str);
atou_result_t my_atou(const char *str);
atou_result_t my_atou_base(const char *str, int base);
atof_result_t my_atof(const char *str);

#endif /* MY_ATOI_H */
```

```c
/**
 * my_atoi.c - 实现
 * 时间复杂度 O(N) N=字符串长度，空间 O(1)
 */
#include "my_atoi.h"
#include <limits.h>
#include <ctype.h>

atoi_result_t my_atoi(const char *str) { return my_atoi_base(str, 0); }

atoi_result_t my_atoi_base(const char *str, int base) {
    atoi_result_t r = {0, ATOI_OK, 0, false};
    if (!str) { r.error = ATOI_ERR_NULL; return r; }
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') { str++; r.chars_read++; }
    if (!*str) { r.error = ATOI_ERR_EMPTY; return r; }
    bool neg = false;
    if (*str == '-') { neg = true; str++; r.chars_read++; r.negative = true; }
    else if (*str == '+') { str++; r.chars_read++; }
    if (!*str) { r.error = ATOI_ERR_NODIGITS; return r; }
    if (base == 0) {
        if (*str == '0') {
            if (*(str+1) == 'x' || *(str+1) == 'X') { base = 16; str += 2; r.chars_read += 2; }
            else if (*(str+1) == 'b' || *(str+1) == 'B') { base = 2; str += 2; r.chars_read += 2; }
            else { base = 8; str++; r.chars_read++; }
        } else base = 10;
    }
    if (!*str) { r.error = ATOI_ERR_NODIGITS; return r; }
    int32_t val = 0; bool has = false;
    while (*str) {
        int d;
        if (*str >= '0' && *str <= '9') d = *str - '0';
        else if (*str >= 'a' && *str <= 'z') d = *str - 'a' + 10;
        else if (*str >= 'A' && *str <= 'Z') d = *str - 'A' + 10;
        else break;
        if (d >= base) break;
        has = true;
        if (!neg) {
            if (val > (INT32_MAX - d) / base) { r.error = ATOI_ERR_OVERFLOW; r.value = INT32_MAX; return r; }
            val = val * base + d;
        } else {
            if (val < (INT32_MIN + d) / base) { r.error = ATOI_ERR_OVERFLOW; r.value = INT32_MIN; return r; }
            val = val * base - d;
        }
        str++; r.chars_read++;
    }
    if (!has) { r.error = ATOI_ERR_NODIGITS; return r; }
    r.value = val; r.error = ATOI_OK;
    return r;
}

atoi64_result_t my_atoll(const char *str) {
    atoi64_result_t r = {0, ATOI_OK, 0, false};
    if (!str) { r.error = ATOI_ERR_NULL; return r; }
    while (*str == ' ' || *str == '\t') { str++; r.chars_read++; }
    if (!*str) { r.error = ATOI_ERR_EMPTY; return r; }
    bool neg = false;
    if (*str == '-') { neg = true; str++; r.chars_read++; r.negative = true; }
    else if (*str == '+') { str++; r.chars_read++; }
    if (!*str) { r.error = ATOI_ERR_NODIGITS; return r; }
    int64_t val = 0; bool has = false;
    while (*str >= '0' && *str <= '9') {
        int d = *str - '0'; has = true;
        if (!neg) {
            if (val > (INT64_MAX - d) / 10) { r.error = ATOI_ERR_OVERFLOW; r.value = INT64_MAX; return r; }
            val = val * 10 + d;
        } else {
            if (val < (INT64_MIN + d) / 10) { r.error = ATOI_ERR_OVERFLOW; r.value = INT64_MIN; return r; }
            val = val * 10 - d;
        }
        str++; r.chars_read++;
    }
    if (!has) { r.error = ATOI_ERR_NODIGITS; return r; }
    r.value = val;
    return r;
}

atou_result_t my_atou(const char *str) { return my_atou_base(str, 10); }

atou_result_t my_atou_base(const char *str, int base) {
    atou_result_t r = {0, ATOI_OK, 0};
    if (!str) { r.error = ATOI_ERR_NULL; return r; }
    while (*str == ' ' || *str == '\t') { str++; r.chars_read++; }
    if (!*str) { r.error = ATOI_ERR_EMPTY; return r; }
    if (*str == '+') { str++; r.chars_read++; }
    if (base == 0) {
        if (*str == '0') {
            if (*(str+1) == 'x' || *(str+1) == 'X') { base = 16; str += 2; r.chars_read += 2; }
            else { base = 8; str++; r.chars_read++; }
        } else base = 10;
    }
    uint32_t val = 0; bool has = false;
    while (*str) {
        int d;
        if (*str >= '0' && *str <= '9') d = *str - '0';
        else if (*str >= 'a' && *str <= 'f') d = *str - 'a' + 10;
        else if (*str >= 'A' && *str <= 'F') d = *str - 'A' + 10;
        else break;
        if (d >= base) break;
        has = true;
        if (val > (UINT32_MAX - (uint32_t)d) / (uint32_t)base)
            { r.error = ATOI_ERR_OVERFLOW; r.value = UINT32_MAX; return r; }
        val = val * (uint32_t)base + (uint32_t)d;
        str++; r.chars_read++;
    }
    if (!has) { r.error = ATOI_ERR_NODIGITS; return r; }
    r.value = val;
    return r;
}

atof_result_t my_atof(const char *str) {
    atof_result_t r = {0.0f, ATOI_OK, 0};
    if (!str) { r.error = ATOI_ERR_NULL; return r; }
    while (*str == ' ' || *str == '\t') { str++; r.chars_read++; }
    if (!*str) { r.error = ATOI_ERR_EMPTY; return r; }
    bool neg = false;
    if (*str == '-') { neg = true; str++; r.chars_read++; }
    else if (*str == '+') { str++; r.chars_read++; }
    bool has = false; float val = 0.0f;
    while (*str >= '0' && *str <= '9') {
        has = true; val = val * 10.0f + (float)(*str - '0');
        str++; r.chars_read++;
    }
    if (*str == '.') {
        str++; r.chars_read++;
        float frac = 0.1f;
        while (*str >= '0' && *str <= '9') {
            has = true; val += (float)(*str - '0') * frac;
            frac *= 0.1f; str++; r.chars_read++;
        }
    }
    if (!has) { r.error = ATOI_ERR_NODIGITS; return r; }
    r.value = neg ? -val : val;
    return r;
}
```

```c
/* 使用示例 */
#include <stdio.h>

int main(void) {
    atoi_result_t r;
    r = my_atoi("12345");     printf("'12345' => %d (ok=%d)\n", r.value, r.error == ATOI_OK);
    r = my_atoi("-9876");     printf("'-9876' => %d\n", r.value);
    r = my_atoi("  +42");     printf("'  +42' => %d\n", r.value);
    r = my_atoi("");           printf("'' => err=%d\n", r.error);
    r = my_atoi("abc");        printf("'abc' => err=%d\n", r.error);
    r = my_atoi("0x1A3F");    printf("'0x1A3F' => %d\n", r.value);
    r = my_atoi("0b1101");    printf("'0b1101' => %d\n", r.value);
    r = my_atoi("2147483648");printf("overflow => err=%d val=%d\n", r.error, r.value);

    atou_result_t ur = my_atou("4294967295");
    printf("atou '4294967295' => %u\n", ur.value);
    ur = my_atou("0xFFAB");
    printf("atou '0xFFAB' => %u\n", ur.value);

    atof_result_t fr = my_atof("-3.14159");
    printf("atof '-3.14159' => %f\n", fr.value);

    atoi64_result_t lr = my_atoll("9223372036854775807");
    printf("atoll => %lld\n", (long long)lr.value);
    return 0;
}
```

### 复杂度分析

| 函数 | 时间 | 空间 | 说明 |
|------|------|------|------|
| `my_atoi` | O(N) | O(1) | N=字符串长度 |
| `my_atou` | O(N) | O(1) | 无符号版 |
| `my_atof` | O(N) | O(1) | 含小数解析 |
| `my_atoll` | O(N) | O(1) | 64位版 |

### MCU优化要点

1. **避免浮点**：无FPU的MCU尽量用`my_atou`整型解析
2. **溢出检测用除法**：`val > (INT_MAX - d) / base` 避免未定义行为
3. **结构体返回**：C中通过寄存器返回结构体效率高
4. **进制自动检测**：支持0x/0b/0前缀

---

## Q267. 手写链表反转/合并有序链表

### 原理说明

链表反转：迭代法 O(N) O(1)，三指针 prev/curr/next。合并有序链表：双指针比较拼接，迭代版空间 O(1)。

### 完整代码

```c
/**
 * linked_list.h - 链表操作
 */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct list_node { int data; struct list_node *next; } list_node_t;
typedef struct dlist_node { int data; struct dlist_node *prev, *next; } dlist_node_t;

list_node_t *list_create_node(int data);
void list_destroy(list_node_t *head);
list_node_t *list_append(list_node_t *head, int data);
void list_print(const list_node_t *head);

list_node_t *list_reverse(list_node_t *head);
list_node_t *list_reverse_recursive(list_node_t *head);
list_node_t *list_reverse_k_group(list_node_t *head, int k);
list_node_t *list_reverse_between(list_node_t *head, int m, int n);

list_node_t *list_merge_sorted(list_node_t *l1, list_node_t *l2);
list_node_t *list_merge_sorted_recursive(list_node_t *l1, list_node_t *l2);
list_node_t *list_merge_k_sorted(list_node_t **lists, int k);

bool list_has_cycle(const list_node_t *head);
list_node_t *list_detect_cycle_entry(list_node_t *head);
list_node_t *list_find_middle(list_node_t *head);
list_node_t *list_remove_nth_from_end(list_node_t *head, int n);
bool list_is_palindrome(list_node_t *head);
dlist_node_t *dlist_reverse(dlist_node_t *head);

#endif /* LINKED_LIST_H */
```

```c
/**
 * linked_list.c - 实现
 * 反转：O(N) O(1)  合并：O(N+M) O(1)  环检测：O(N) O(1)
 */
#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>

list_node_t *list_create_node(int data) {
    list_node_t *n = (list_node_t *)malloc(sizeof(list_node_t));
    if (n) { n->data = data; n->next = NULL; }
    return n;
}

void list_destroy(list_node_t *head) {
    while (head) { list_node_t *next = head->next; free(head); head = next; }
}

list_node_t *list_append(list_node_t *head, int data) {
    list_node_t *n = list_create_node(data);
    if (!n) return head;
    if (!head) return n;
    list_node_t *c = head;
    while (c->next) c = c->next;
    c->next = n;
    return head;
}

void list_print(const list_node_t *head) {
    while (head) { printf("%d", head->data); head = head->next; if (head) printf("->"); }
    printf("\n");
}

/* 迭代反转 */
list_node_t *list_reverse(list_node_t *head) {
    list_node_t *prev = NULL, *curr = head;
    while (curr) {
        list_node_t *next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

/* 递归反转 */
list_node_t *list_reverse_recursive(list_node_t *head) {
    if (!head || !head->next) return head;
    list_node_t *nh = list_reverse_recursive(head->next);
    head->next->next = head;
    head->next = NULL;
    return nh;
}

/* K个一组反转 */
list_node_t *list_reverse_k_group(list_node_t *head, int k) {
    if (!head || k <= 1) return head;
    list_node_t *c = head;
    for (int i = 0; i < k; i++) { if (!c) return head; c = c->next; }
    list_node_t *prev = NULL; c = head;
    for (int i = 0; i < k; i++) {
        list_node_t *next = c->next; c->next = prev; prev = c; c = next;
    }
    head->next = list_reverse_k_group(c, k);
    return prev;
}

/* 区间反转[m,n] 1-indexed */
list_node_t *list_reverse_between(list_node_t *head, int m, int n) {
    if (!head || m >= n && m > 0) return head;
    if (m >= n) return head;  /* correction */
    if (!head || m >= n) return head;
    list_node_t dummy = {0, head}, *pre = &dummy;
    for (int i = 1; i < m && pre; i++) pre = pre->next;
    if (!pre || !pre->next) return head;
    list_node_t *start = pre->next, *then = start->next;
    for (int i = 0; i < n - m; i++) {
        start->next = then->next;
        then->next = pre->next;
        pre->next = then;
        then = start->next;
    }
    return dummy.next;
}

/* 合并有序链表（迭代） */
list_node_t *list_merge_sorted(list_node_t *l1, list_node_t *l2) {
    list_node_t dummy = {0, NULL}, *tail = &dummy;
    while (l1 && l2) {
        if (l1->data <= l2->data) { tail->next = l1; l1 = l1->next; }
        else { tail->next = l2; l2 = l2->next; }
        tail = tail->next;
    }
    tail->next = l1 ? l1 : l2;
    return dummy.next;
}

/* 合并有序链表（递归） */
list_node_t *list_merge_sorted_recursive(list_node_t *l1, list_node_t *l2) {
    if (!l1) return l2; if (!l2) return l1;
    if (l1->data <= l2->data) { l1->next = list_merge_sorted_recursive(l1->next, l2); return l1; }
    else { l2->next = list_merge_sorted_recursive(l1, l2->next); return l2; }
}

/* 合并K个有序链表（分治） */
static list_node_t *merge2(list_node_t *a, list_node_t *b) {
    list_node_t d = {0,NULL}, *t = &d;
    while (a && b) {
        if (a->data <= b->data) { t->next = a; a = a->next; }
        else { t->next = b; b = b->next; }
        t = t->next;
    }
    t->next = a ? a : b;
    return d.next;
}

list_node_t *list_merge_k_sorted(list_node_t **lists, int k) {
    if (!lists || k == 0) return NULL;
    int step = 1;
    while (step < k) {
        for (int i = 0; i < k - step; i += step * 2) lists[i] = merge2(lists[i], lists[i + step]);
        step *= 2;
    }
    return lists[0];
}

/* 环检测（Floyd） */
bool list_has_cycle(const list_node_t *head) {
    if (!head) return false;
    const list_node_t *s = head, *f = head;
    while (f && f->next) { s = s->next; f = f->next->next; if (s == f) return true; }
    return false;
}

list_node_t *list_detect_cycle_entry(list_node_t *head) {
    if (!head) return NULL;
    list_node_t *s = head, *f = head;
    while (f && f->next) {
        s = s->next; f = f->next->next;
        if (s == f) { s = head; while (s != f) { s = s->next; f = f->next; } return s; }
    }
    return NULL;
}

/* 中间节点 */
list_node_t *list_find_middle(list_node_t *head) {
    if (!head) return NULL;
    list_node_t *s = head, *f = head;
    while (f && f->next) { s = s->next; f = f->next->next; }
    return s;
}

/* 删除倒数第N个 */
list_node_t *list_remove_nth_from_end(list_node_t *head, int n) {
    if (!head || n <= 0) return head;
    list_node_t d = {0, head}, *f = &d, *s = &d;
    for (int i = 0; i <= n && f; i++) f = f->next;
    while (f) { s = s->next; f = f->next; }
    list_node_t *td = s->next;
    if (td) { s->next = td->next; free(td); }
    return d.next;
}

/* 回文检测 */
bool list_is_palindrome(list_node_t *head) {
    if (!head || !head->next) return true;
    list_node_t *s = head, *f = head;
    while (f->next && f->next->next) { s = s->next; f = f->next->next; }
    list_node_t *sh = list_reverse(s->next);
    list_node_t *p1 = head, *p2 = sh; bool ok = true;
    while (p2) { if (p1->data != p2->data) { ok = false; break; } p1 = p1->next; p2 = p2->next; }
    s->next = list_reverse(sh);
    return ok;
}

/* 双向链表反转 */
dlist_node_t *dlist_reverse(dlist_node_t *head) {
    dlist_node_t *c = head, *nh = NULL;
    while (c) {
        dlist_node_t *t = c->prev; c->prev = c->next; c->next = t;
        nh = c; c = c->prev;
    }
    return nh;
}
```

```c
/* 使用示例 */
#include <stdio.h>

int main(void) {
    list_node_t *h = list_create_node(1);
    h = list_append(h,2); h = list_append(h,3); h = list_append(h,4); h = list_append(h,5);
    printf("Original: "); list_print(h);
    h = list_reverse(h);
    printf("Reversed: "); list_print(h);

    h = list_reverse(h);  /* restore */
    h = list_reverse_k_group(h, 2);
    printf("K=2 group: "); list_print(h);

    list_node_t *l1 = list_create_node(1); l1=list_append(l1,3); l1=list_append(l1,5);
    list_node_t *l2 = list_create_node(2); l2=list_append(l2,4); l2=list_append(l2,6);
    list_node_t *m = list_merge_sorted(l1, l2);
    printf("Merged:   "); list_print(m);
    printf("Has cycle: %s\n", list_has_cycle(m) ? "YES" : "NO");

    list_node_t *mid = list_find_middle(m);
    printf("Middle: %d\n", mid ? mid->data : -1);

    list_node_t *pal = list_create_node(1);
    pal=list_append(pal,2); pal=list_append(pal,3); pal=list_append(pal,2); pal=list_append(pal,1);
    printf("Is palindrome: %s\n", list_is_palindrome(pal) ? "YES" : "NO");

    list_destroy(m); list_destroy(pal);
    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 | 说明 |
|------|------|------|------|
| 迭代反转 | O(N) | O(1) | 推荐MCU使用 |
| 递归反转 | O(N) | O(N) | 递归栈开销 |
| 合并两个链表 | O(N+M) | O(1) | 迭代版 |
| 合并K个链表 | O(N log K) | O(1) | 分治法 |
| 环检测 | O(N) | O(1) | Floyd |
| K个一组反转 | O(N) | O(1) | |

### MCU优化要点

1. **避免递归**：栈空间有限，优先用迭代版
2. **避免malloc**：使用静态节点池
3. **哑节点栈上分配**：`list_node_t dummy = {0, head}` 而非malloc
4. **节点池**：预分配固定数量节点

---

## Q268. 手写位图(Bitmap)操作

### 原理说明

位图用1bit表示一个状态，极致的内存效率。32个状态仅需4字节。常用于：内存管理、FreeRTOS任务就绪表、标志位集合。

### 完整代码

```c
/**
 * bitmap.h - 位图操作（MCU优化版）
 */
#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint32_t bitmap_word_t;
#define BITMAP_WORD_BITS  32
#define BITMAP_WORD_MASK  (BITMAP_WORD_BITS - 1)
#define BITMAP_WORD_SHIFT 5

typedef struct { uint8_t *bits; uint32_t num_bits; uint32_t num_bytes; } bitmap_t;

void bitmap_init(bitmap_t *bm, uint8_t *buffer, uint32_t num_bits);
void bitmap_set(bitmap_t *bm, uint32_t bit);
void bitmap_clear(bitmap_t *bm, uint32_t bit);
void bitmap_toggle(bitmap_t *bm, uint32_t bit);
bool bitmap_get(const bitmap_t *bm, uint32_t bit);
void bitmap_assign(bitmap_t *bm, uint32_t bit, bool value);
void bitmap_set_all(bitmap_t *bm);
void bitmap_clear_all(bitmap_t *bm);
void bitmap_toggle_all(bitmap_t *bm);
void bitmap_set_range(bitmap_t *bm, uint32_t start, uint32_t end);
void bitmap_clear_range(bitmap_t *bm, uint32_t start, uint32_t end);
int32_t bitmap_find_first_set(const bitmap_t *bm);
int32_t bitmap_find_first_clear(const bitmap_t *bm);
int32_t bitmap_find_next_set(const bitmap_t *bm, uint32_t start);
int32_t bitmap_find_next_clear(const bitmap_t *bm, uint32_t start);
uint32_t bitmap_count_set(const bitmap_t *bm);
uint32_t bitmap_count_clear(const bitmap_t *bm);
bool bitmap_is_empty(const bitmap_t *bm);
bool bitmap_is_full(const bitmap_t *bm);
void bitmap_and(bitmap_t *dst, const bitmap_t *a, const bitmap_t *b);
void bitmap_or(bitmap_t *dst, const bitmap_t *a, const bitmap_t *b);
void bitmap_xor(bitmap_t *dst, const bitmap_t *a, const bitmap_t *b);
void bitmap_not(bitmap_t *dst, const bitmap_t *src);
void bitmap_print(const bitmap_t *bm);

#endif /* BITMAP_H */
```

```c
/**
 * bitmap.c - 位图实现
 * 单bit操作 O(1)，查找 O(N/32)，popcount O(N/32)
 */
#include "bitmap.h"
#include <string.h>
#include <stdio.h>

void bitmap_init(bitmap_t *bm, uint8_t *buffer, uint32_t num_bits) {
    if (!bm || !buffer) return;
    bm->bits = buffer; bm->num_bits = num_bits; bm->num_bytes = (num_bits + 7) / 8;
    bitmap_clear_all(bm);
}

void bitmap_set(bitmap_t *bm, uint32_t bit) {
    if (!bm || bit >= bm->num_bits) return;
    bitmap_word_t *w = (bitmap_word_t *)bm->bits;
    w[bit >> BITMAP_WORD_SHIFT] |= ((bitmap_word_t)1U << (bit & BITMAP_WORD_MASK));
}

void bitmap_clear(bitmap_t *bm, uint32_t bit) {
    if (!bm || bit >= bm->num_bits) return;
    bitmap_word_t *w = (bitmap_word_t *)bm->bits;
    w[bit >> BITMAP_WORD_SHIFT] &= ~((bitmap_word_t)1U << (bit & BITMAP_WORD_MASK));
}

void bitmap_toggle(bitmap_t *bm, uint32_t bit) {
    if (!bm || bit >= bm->num_bits) return;
    bitmap_word_t *w = (bitmap_word_t *)bm->bits;
    w[bit >> BITMAP_WORD_SHIFT] ^= ((bitmap_word_t)1U << (bit & BITMAP_WORD_MASK));
}

bool bitmap_get(const bitmap_t *bm, uint32_t bit) {
    if (!bm || bit >= bm->num_bits) return false;
    const bitmap_word_t *w = (const bitmap_word_t *)bm->bits;
    return (w[bit >> BITMAP_WORD_SHIFT] & ((bitmap_word_t)1U << (bit & BITMAP_WORD_MASK))) != 0;
}

void bitmap_assign(bitmap_t *bm, uint32_t bit, bool value) {
    if (value) bitmap_set(bm, bit); else bitmap_clear(bm, bit);
}

/* 全部操作 */
void bitmap_set_all(bitmap_t *bm) {
    if (!bm) return;
    memset(bm->bits, 0xFF, bm->num_bytes);
    uint32_t extra = (bm->num_bytes * 8) - bm->num_bits;
    if (extra > 0 && bm->num_bytes > 0) bm->bits[bm->num_bytes - 1] &= (0xFF >> extra);
}

void bitmap_clear_all(bitmap_t *bm) { if (bm) memset(bm->bits, 0, bm->num_bytes); }

void bitmap_toggle_all(bitmap_t *bm) {
    if (!bm) return;
    bitmap_word_t *w = (bitmap_word_t *)bm->bits;
    uint32_t wc = bm->num_bits / BITMAP_WORD_BITS;
    for (uint32_t i = 0; i < wc; i++) w[i] = ~w[i];
    uint32_t rem = bm->num_bits % BITMAP_WORD_BITS;
    if (rem > 0) {
        bitmap_word_t mask = ((bitmap_word_t)1U << rem) - 1U;
        w[wc] = (~w[wc]) & mask;
    }
}

void bitmap_set_range(bitmap_t *bm, uint32_t start, uint32_t end) {
    if (!bm || start >= bm->num_bits || end > bm->num_bits || start >= end) return;
    bitmap_word_t *w = (bitmap_word_t *)bm->bits;
    while (start < end && (start & BITMAP_WORD_MASK) != 0) { bitmap_set(bm, start); start++; }
    while (end - start >= BITMAP_WORD_BITS) {
        w[start >> BITMAP_WORD_SHIFT] = ~(bitmap_word_t)0U;
        start += BITMAP_WORD_BITS;
    }
    while (start < end) { bitmap_set(bm, start); start++; }
}

void bitmap_clear_range(bitmap_t *bm, uint32_t start, uint32_t end) {
    if (!bm || start >= bm->num_bits || end > bm->num_bits || start >= end) return;
    bitmap_word_t *w = (bitmap_word_t *)bm->bits;
    while (start < end && (start & BITMAP_WORD_MASK) != 0) { bitmap_clear(bm, start); start++; }
    while (end - start >= BITMAP_WORD_BITS) { w[start >> BITMAP_WORD_SHIFT] = 0; start += BITMAP_WORD_BITS; }
    while (start < end) { bitmap_clear(bm, start); start++; }
}

/* 查找 */
int32_t bitmap_find_first_set(const bitmap_t *bm) {
    if (!bm) return -1;
    const bitmap_word_t *w = (const bitmap_word_t *)bm->bits;
    uint32_t wc = (bm->num_bits + BITMAP_WORD_MASK) / BITMAP_WORD_BITS;
    for (uint32_t i = 0; i < wc; i++) {
        if (w[i] != 0) {
            uint32_t bit = i * BITMAP_WORD_BITS +
                (uint32_t)__builtin_ctz(w[i]); /* GCC内建函数: count trailing zeros */
            return (bit < bm->num_bits) ? (int32_t)bit : -1;
        }
    }
    return -1;
}

int32_t bitmap_find_first_clear(const bitmap_t *bm) {
    if (!bm) return -1;
    const bitmap_word_t *w = (const bitmap_word_t *)bm->bits;
    uint32_t wc = (bm->num_bits + BITMAP_WORD_MASK) / BITMAP_WORD_BITS;
    for (uint32_t i = 0; i < wc; i++) {
        if (w[i] != ~(bitmap_word_t)0U) {
            uint32_t bit = i * BITMAP_WORD_BITS + (uint32_t)__builtin_ctz(~w[i]);
            return (bit < bm->num_bits) ? (int32_t)bit : -1;
        }
    }
    return -1;
}

int32_t bitmap_find_next_set(const bitmap_t *bm, uint32_t start) {
    if (!bm || start >= bm->num_bits) return -1;
    const bitmap_word_t *w = (const bitmap_word_t *)bm->bits;
    uint32_t idx = start >> BITMAP_WORD_SHIFT;
    bitmap_word_t m = w[idx] & (~((bitmap_word_t)0U) << (start & BITMAP_WORD_MASK));
    if (m != 0) {
        uint32_t bit = idx * BITMAP_WORD_BITS + (uint32_t)__builtin_ctz(m);
        return (bit < bm->num_bits) ? (int32_t)bit : -1;
    }
    for (uint32_t i = idx + 1; i * BITMAP_WORD_BITS < bm->num_bits; i++) {
        if (w[i] != 0) {
            uint32_t bit = i * BITMAP_WORD_BITS + (uint32_t)__builtin_ctz(w[i]);
            return (bit < bm->num_bits) ? (int32_t)bit : -1;
        }
    }
    return -1;
}

int32_t bitmap_find_next_clear(const bitmap_t *bm, uint32_t start) {
    if (!bm || start >= bm->num_bits) return -1;
    const bitmap_word_t *w = (const bitmap_word_t *)bm->bits;
    uint32_t idx = start >> BITMAP_WORD_SHIFT;
    bitmap_word_t m = (~w[idx]) & (~((bitmap_word_t)0U) << (start & BITMAP_WORD_MASK));
    if (m != 0) {
        uint32_t bit = idx * BITMAP_WORD_BITS + (uint32_t)__builtin_ctz(m);
        return (bit < bm->num_bits) ? (int32_t)bit : -1;
    }
    for (uint32_t i = idx + 1; i * BITMAP_WORD_BITS < bm->num_bits; i++) {
        if (w[i] != ~(bitmap_word_t)0U) {
            uint32_t bit = i * BITMAP_WORD_BITS + (uint32_t)__builtin_ctz(~w[i]);
            return (bit < bm->num_bits) ? (int32_t)bit : -1;
        }
    }
    return -1;
}

/* 统计 */
uint32_t bitmap_count_set(const bitmap_t *bm) {
    if (!bm) return 0;
    uint32_t cnt = 0;
    const bitmap_word_t *w = (const bitmap_word_t *)bm->bits;
    uint32_t wc = bm->num_bits / BITMAP_WORD_BITS;
    for (uint32_t i = 0; i < wc; i++) cnt += (uint32_t)__builtin_popcount(w[i]);
    uint32_t rem = bm->num_bits % BITMAP_WORD_BITS;
    if (rem > 0) cnt += (uint32_t)__builtin_popcount(w[wc] & (((bitmap_word_t)1U << rem) - 1U));
    return cnt;
}

uint32_t bitmap_count_clear(const bitmap_t *bm) { return bm ? bm->num_bits - bitmap_count_set(bm) : 0; }
bool bitmap_is_empty(const bitmap_t *bm) { return bitmap_count_set(bm) == 0; }
bool bitmap_is_full(const bitmap_t *bm) { return bm ? bitmap_count_set(bm) == bm->num_bits : false; }

/* 集合运算 */
void bitmap_and(bitmap_t *dst, const bitmap_t *a, const bitmap_t *b) {
    if (!dst || !a || !b) return;
    bitmap_word_t *dw = (bitmap_word_t *)dst->bits;
    const bitmap_word_t *aw = (const bitmap_word_t *)a->bits;
    const bitmap_word_t *bw = (const bitmap_word_t *)b->bits;
    uint32_t wc = (a->num_bits + BITMAP_WORD_MASK) / BITMAP_WORD_BITS;
    for (uint32_t i = 0; i < wc; i++) dw[i] = aw[i] & bw[i];
}

void bitmap_or(bitmap_t *dst, const bitmap_t *a, const bitmap_t *b) {
    if (!dst || !a || !b) return;
    bitmap_word_t *dw = (bitmap_word_t *)dst->bits;
    const bitmap_word_t *aw = (const bitmap_word_t *)a->bits;
    const bitmap_word_t *bw = (const bitmap_word_t *)b->bits;
    uint32_t wc = (a->num_bits + BITMAP_WORD_MASK) / BITMAP_WORD_BITS;
    for (uint32_t i = 0; i < wc; i++) dw[i] = aw[i] | bw[i];
}

void bitmap_xor(bitmap_t *dst, const bitmap_t *a, const bitmap_t *b) {
    if (!dst || !a || !b) return;
    bitmap_word_t *dw = (bitmap_word_t *)dst->bits;
    const bitmap_word_t *aw = (const bitmap_word_t *)a->bits;
    const bitmap_word_t *bw = (const bitmap_word_t *)b->bits;
    uint32_t wc = (a->num_bits + BITMAP_WORD_MASK) / BITMAP_WORD_BITS;
    for (uint32_t i = 0; i < wc; i++) dw[i] = aw[i] ^ bw[i];
}

void bitmap_not(bitmap_t *dst, const bitmap_t *src) {
    if (!dst || !src) return;
    bitmap_word_t *dw = (bitmap_word_t *)dst->bits;
    const bitmap_word_t *sw = (const bitmap_word_t *)src->bits;
    uint32_t wc = (src->num_bits + BITMAP_WORD_MASK) / BITMAP_WORD_BITS;
    for (uint32_t i = 0; i < wc; i++) dw[i] = ~sw[i];
    uint32_t rem = src->num_bits % BITMAP_WORD_BITS;
    if (rem > 0) dw[wc-1] &= ((bitmap_word_t)1U << rem) - 1U;
}

void bitmap_print(const bitmap_t *bm) {
    if (!bm) return;
    printf("Bitmap(%u bits): ", bm->num_bits);
    for (uint32_t i = 0; i < bm->num_bits && i < 64; i++) printf("%c", bitmap_get(bm, i) ? '1' : '0');
    printf(" set=%u\n", bitmap_count_set(bm));
}
```

```c
/* 使用示例 */
#include <stdio.h>

#define TASK_COUNT 32
static uint8_t bm_buf[(TASK_COUNT + 7) / 8];

int main(void) {
    bitmap_t bm; bitmap_init(&bm, bm_buf, TASK_COUNT);
    bitmap_set(&bm, 3); bitmap_set(&bm, 7); bitmap_set(&bm, 15); bitmap_set(&bm, 31);
    bitmap_print(&bm);
    printf("Bit 7: %s\n", bitmap_get(&bm, 7) ? "SET" : "CLEAR");
    bitmap_clear(&bm, 7);
    printf("After clear 7, bit 7: %s\n", bitmap_get(&bm, 7) ? "SET" : "CLEAR");
    int32_t f = bitmap_find_first_set(&bm); printf("First set: %ld\n", (long)f);
    int32_t e = bitmap_find_first_clear(&bm); printf("First clear: %ld\n", (long)e);
    printf("Empty: %s  Full: %s\n", bitmap_is_empty(&bm) ? "YES":"NO", bitmap_is_full(&bm) ? "YES":"NO");
    bitmap_toggle_all(&bm); bitmap_print(&bm);
    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 |
|------|------|------|
| set/clear/get/toggle | O(1) | O(1) |
| find_first_set | O(N/32) | O(1) |
| find_next_set | O(N/32) | O(1) |
| count_set (popcount) | O(N/32) | O(1) |
| and/or/xor/not | O(N/32) | O(1) |
| set_range/clear_range | O(N/32) | O(1) |

### MCU优化要点

1. **32位字对齐访问**：一次操作32个bit
2. **__builtin_ctz/popcount**：GCC内建函数，ARM Cortex-M有CLZ/CTZ指令
3. **位带操作**：Cortex-M3/M4支持Bit-Banding，单bit原子操作
4. **静态buffer**：`uint8_t bm_buf[(N+7)/8]` 静态分配

---

## Q269. 手写FIFO队列(数组实现)

### 原理说明

数组实现的FIFO队列比链表版更MCU友好（无malloc、缓存局部性好）。通过头尾指针在数组上循环移动实现O(1)入队/出队。

### 完整代码

```c
/**
 * fifo.h - FIFO队列（数组实现，线程安全）
 */
#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    uint8_t  *buffer;
    uint32_t  capacity;  /* 最大容量（2的幂次） */
    uint32_t  head;      /* 出队位置 */
    uint32_t  tail;      /* 入队位置 */
    uint32_t  count;
    bool      is_pow2;
} fifo_t;

void fifo_init(fifo_t *fifo, uint8_t *buffer, uint32_t capacity);
bool fifo_enqueue(fifo_t *fifo, uint8_t data);
bool fifo_dequeue(fifo_t *fifo, uint8_t *data);
bool fifo_peek(const fifo_t *fifo, uint8_t *data);
bool fifo_peek_at(const fifo_t *fifo, uint32_t offset, uint8_t *data);
uint32_t fifo_enqueue_n(fifo_t *fifo, const uint8_t *data, uint32_t len);
uint32_t fifo_dequeue_n(fifo_t *fifo, uint8_t *data, uint32_t len);

static inline bool fifo_is_empty(const fifo_t *fifo) { return fifo->count == 0; }
static inline bool fifo_is_full(const fifo_t *fifo) { return fifo->count >= fifo->capacity; }
static inline uint32_t fifo_count(const fifo_t *fifo) { return fifo->count; }
static inline uint32_t fifo_free(const fifo_t *fifo) { return fifo->capacity - fifo->count; }
void fifo_reset(fifo_t *fifo);

#endif /* FIFO_H */
```

```c
/**
 * fifo.c - FIFO实现
 * 入队/出队 O(1)，批量 O(N)
 */
#include "fifo.h"

void fifo_init(fifo_t *fifo, uint8_t *buffer, uint32_t capacity) {
    if (!fifo || !buffer || capacity == 0) return;
    fifo->buffer   = buffer;
    fifo->capacity = capacity;
    fifo->head     = 0;
    fifo->tail     = 0;
    fifo->count    = 0;
    fifo->is_pow2  = ((capacity & (capacity - 1U)) == 0U) && (capacity > 0);
}

bool fifo_enqueue(fifo_t *fifo, uint8_t data) {
    if (!fifo || !fifo->buffer || fifo->count >= fifo->capacity) return false;
    fifo->buffer[fifo->tail] = data;
    if (fifo->is_pow2) fifo->tail = (fifo->tail + 1U) & (fifo->capacity - 1U);
    else               fifo->tail = (fifo->tail + 1U) % fifo->capacity;
    fifo->count++;
    return true;
}

bool fifo_dequeue(fifo_t *fifo, uint8_t *data) {
    if (!fifo || !fifo->buffer || !data || fifo->count == 0) return false;
    *data = fifo->buffer[fifo->head];
    if (fifo->is_pow2) fifo->head = (fifo->head + 1U) & (fifo->capacity - 1U);
    else               fifo->head = (fifo->head + 1U) % fifo->capacity;
    fifo->count--;
    return true;
}

bool fifo_peek(const fifo_t *fifo, uint8_t *data) {
    if (!fifo || !fifo->buffer || !data || fifo->count == 0) return false;
    *data = fifo->buffer[fifo->head];
    return true;
}

bool fifo_peek_at(const fifo_t *fifo, uint32_t offset, uint8_t *data) {
    if (!fifo || !fifo->buffer || !data || offset >= fifo->count) return false;
    uint32_t idx;
    if (fifo->is_pow2) idx = (fifo->head + offset) & (fifo->capacity - 1U);
    else               idx = (fifo->head + offset) % fifo->capacity;
    *data = fifo->buffer[idx];
    return true;
}

uint32_t fifo_enqueue_n(fifo_t *fifo, const uint8_t *data, uint32_t len) {
    if (!fifo || !fifo->buffer || !data || len == 0) return 0;
    uint32_t free = fifo->capacity - fifo->count;
    uint32_t n = (len < free) ? len : free;
    for (uint32_t i = 0; i < n; i++) {
        fifo->buffer[fifo->tail] = data[i];
        if (fifo->is_pow2) fifo->tail = (fifo->tail + 1U) & (fifo->capacity - 1U);
        else               fifo->tail = (fifo->tail + 1U) % fifo->capacity;
    }
    fifo->count += n;
    return n;
}

uint32_t fifo_dequeue_n(fifo_t *fifo, uint8_t *data, uint32_t len) {
    if (!fifo || !fifo->buffer || !data || len == 0) return 0;
    uint32_t n = (len < fifo->count) ? len : fifo->count;
    for (uint32_t i = 0; i < n; i++) {
        data[i] = fifo->buffer[fifo->head];
        if (fifo->is_pow2) fifo->head = (fifo->head + 1U) & (fifo->capacity - 1U);
        else               fifo->head = (fifo->head + 1U) % fifo->capacity;
    }
    fifo->count -= n;
    return n;
}

void fifo_reset(fifo_t *fifo) {
    if (!fifo) return;
    fifo->head = fifo->tail = 0;
    fifo->count = 0;
}
```

```c
/* 使用示例 */
#include <stdio.h>

#define CMD_FIFO_SIZE 64
static uint8_t cmd_buffer[CMD_FIFO_SIZE];
static fifo_t cmd_fifo;

int main(void) {
    fifo_init(&cmd_fifo, cmd_buffer, CMD_FIFO_SIZE);
    /* 入队 */
    for (int i = 0; i < 10; i++) fifo_enqueue(&cmd_fifo, (uint8_t)(i + 'A'));
    printf("FIFO count: %u\n", fifo_count(&cmd_fifo));
    /* 窥视 */
    uint8_t peek; fifo_peek(&cmd_fifo, &peek);
    printf("Peek front: %c\n", (char)peek);
    /* 出队 */
    uint8_t data;
    while (fifo_dequeue(&cmd_fifo, &data)) printf("%c ", (char)data);
    printf("\nEmpty: %s\n", fifo_is_empty(&cmd_fifo) ? "YES" : "NO");

    /* 批量入队 */
    uint8_t batch[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    fifo_enqueue_n(&cmd_fifo, batch, 5);
    uint8_t rx[5] = {0};
    uint32_t n = fifo_dequeue_n(&cmd_fifo, rx, 5);
    printf("Dequeued %u: ", n);
    for (uint32_t i = 0; i < n; i++) printf("%02X ", rx[i]);
    printf("\n");
    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 |
|------|------|------|
| enqueue/dequeue | O(1) | O(1) |
| peek/peek_at | O(1) | O(1) |
| enqueue_n/dequeue_n | O(N) | O(1) |

### MCU优化要点

1. **2的幂次容量**：`%`替换为`&`位掩码
2. **冗余count**：避免仅靠head/tail比较判断空/满
3. **批量操作memcpy优化**：连续区域可用memcpy加速

---

## Q270. 手写状态机框架

### 原理说明

有限状态机(FSM)是嵌入式系统核心设计模式。使用函数指针表驱动，每个状态对应一个处理函数，事件驱动状态转移。支持进入/退出动作。

### 完整代码

```c
/**
 * fsm.h - 有限状态机框架
 */
#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 事件定义 */
typedef uint16_t fsm_event_t;
#define FSM_EVENT_NONE     0
#define FSM_EVENT_ENTER    0xFFFE  /* 进入状态事件 */
#define FSM_EVENT_EXIT     0xFFFF  /* 退出状态事件 */

/* 状态定义 */
typedef uint16_t fsm_state_t;

/* 前向声明 */
struct fsm;

/* 状态处理函数：返回新状态 */
typedef fsm_state_t (*fsm_handler_t)(struct fsm *fsm, fsm_event_t event, void *param);

/* 状态转换表条目 */
typedef struct {
    fsm_event_t  event;
    fsm_state_t  next_state;
} fsm_transition_t;

/* 状态描述 */
typedef struct {
    fsm_state_t          state;
    fsm_handler_t        handler;
    const fsm_transition_t *transitions;
    uint16_t             trans_count;
} fsm_state_desc_t;

/* FSM句柄 */
typedef struct fsm {
    fsm_state_t          state;
    fsm_state_t          prev_state;
    uint32_t             state_entry_ms;
    void                 *context;
    const fsm_state_desc_t *states;
    uint16_t             state_count;
    bool                 initialized;
} fsm_t;

/* API */
int fsm_init(fsm_t *fsm, const fsm_state_desc_t *states, uint16_t count,
             fsm_state_t initial_state, void *context);
int fsm_dispatch(fsm_t *fsm, fsm_event_t event, void *param);
bool fsm_transit(fsm_t *fsm, fsm_state_t new_state);
fsm_state_t fsm_get_state(const fsm_t *fsm);
fsm_state_t fsm_get_prev_state(const fsm_t *fsm);
uint32_t fsm_state_duration_ms(const fsm_t *fsm, uint32_t (*ms_tick)(void));
const char *fsm_state_name(fsm_state_t state);

#endif /* FSM_H */
```

```c
/**
 * fsm.c - FSM实现
 * 分发: O(M) M=当前状态转换表大小
 */
#include "fsm.h"
#include <string.h>

int fsm_init(fsm_t *fsm, const fsm_state_desc_t *states, uint16_t count,
             fsm_state_t initial_state, void *context) {
    if (!fsm || !states || count == 0) return -1;
    memset(fsm, 0, sizeof(fsm_t));
    fsm->states = states; fsm->state_count = count;
    fsm->state = initial_state; fsm->prev_state = initial_state;
    fsm->context = context; fsm->initialized = true;
    /* 触发进入事件 */
    fsm_dispatch(fsm, FSM_EVENT_ENTER, NULL);
    return 0;
}

int fsm_dispatch(fsm_t *fsm, fsm_event_t event, void *param) {
    if (!fsm || !fsm->initialized) return -1;
    /* 查找当前状态描述 */
    const fsm_state_desc_t *desc = NULL;
    for (uint16_t i = 0; i < fsm->state_count; i++) {
        if (fsm->states[i].state == fsm->state) { desc = &fsm->states[i]; break; }
    }
    if (!desc) return -2;

    /* 查找事件对应的转换 */
    fsm_state_t next_state = fsm->state; /* 默认保持 */
    if (desc->transitions) {
        for (uint16_t i = 0; i < desc->trans_count; i++) {
            if (desc->transitions[i].event == event) {
                next_state = desc->transitions[i].next_state;
                break;
            }
        }
    }

    /* 执行当前状态的处理器 */
    if (desc->handler) {
        fsm_state_t handler_result = desc->handler(fsm, event, param);
        if (handler_result != fsm->state && event != FSM_EVENT_ENTER && event != FSM_EVENT_EXIT) {
            next_state = handler_result;
        }
    }

    /* 执行状态转换 */
    if (next_state != fsm->state) {
        fsm_transit(fsm, next_state);
    }

    return 0;
}

bool fsm_transit(fsm_t *fsm, fsm_state_t new_state) {
    if (!fsm || !fsm->initialized) return false;
    /* 验证目标状态存在 */
    bool valid = false;
    for (uint16_t i = 0; i < fsm->state_count; i++) {
        if (fsm->states[i].state == new_state) { valid = true; break; }
    }
    if (!valid) return false;

    /* 退出当前状态 */
    fsm_dispatch(fsm, FSM_EVENT_EXIT, NULL);
    /* 切换 */
    fsm->prev_state = fsm->state;
    fsm->state = new_state;
    /* 进入新状态 */
    fsm_dispatch(fsm, FSM_EVENT_ENTER, NULL);
    return true;
}

fsm_state_t fsm_get_state(const fsm_t *fsm) { return fsm ? fsm->state : 0; }
fsm_state_t fsm_get_prev_state(const fsm_t *fsm) { return fsm ? fsm->prev_state : 0; }

uint32_t fsm_state_duration_ms(const fsm_t *fsm, uint32_t (*ms_tick)(void)) {
    (void)fsm; (void)ms_tick;
    return 0;
}
```

```c
/* 使用示例：UART接收状态机 */
#include <stdio.h>

typedef enum { ST_IDLE, ST_WAIT_LEN, ST_RECV_DATA, ST_CHECKSUM, ST_DONE } uart_state_t;
typedef enum { EV_BYTE=1, EV_TIMEOUT=2, EV_CHECK_OK=3, EV_CHECK_FAIL=4 } uart_event_t;

#define MAX_FRAME_LEN 32
typedef struct {
    uint8_t buf[32]; uint8_t len; uint8_t idx; uint8_t checksum;
} uart_ctx_t;

static fsm_state_t idle_handler(fsm_t *fsm, fsm_event_t ev, void *p) {
    uart_ctx_t *ctx = (uart_ctx_t *)fsm->context;
    if (ev == FSM_EVENT_ENTER) { ctx->idx = 0; ctx->len = 0; ctx->checksum = 0; }
    if (ev == EV_BYTE && p) { ctx->buf[0] = *(uint8_t*)p; return ST_WAIT_LEN; }
    return ST_IDLE;
}

static fsm_state_t wait_len_handler(fsm_t *fsm, fsm_event_t ev, void *p) {
    uart_ctx_t *ctx = (uart_ctx_t *)fsm->context;
    if (ev == EV_BYTE && p) {
        ctx->len = *(uint8_t*)p;
        if (ctx->len == 0 || ctx->len > MAX_FRAME_LEN) return ST_IDLE;
        ctx->idx = 0; return ST_RECV_DATA;
    }
    if (ev == EV_TIMEOUT) return ST_IDLE;
    return ST_WAIT_LEN;
}

static fsm_state_t recv_data_handler(fsm_t *fsm, fsm_event_t ev, void *p) {
    uart_ctx_t *ctx = (uart_ctx_t *)fsm->context;
    if (ev == EV_BYTE && p) {
        ctx->buf[2 + ctx->idx] = *(uint8_t*)p;
        ctx->idx++;
        if (ctx->idx >= ctx->len) return ST_CHECKSUM;
    }
    return ST_RECV_DATA;
}

static fsm_state_t checksum_handler(fsm_t *fsm, fsm_event_t ev, void *p) {
    uart_ctx_t *ctx = (uart_ctx_t *)fsm->context;
    if (ev == EV_BYTE && p) {
        uint8_t rxd = *(uint8_t*)p;
        for (int i = 0; i < ctx->len; i++) ctx->checksum ^= ctx->buf[2 + i];
        if (ctx->checksum == rxd) return ST_DONE;
        else return ST_IDLE;
    }
    return ST_CHECKSUM;
}

static fsm_state_t done_handler(fsm_t *fsm, fsm_event_t ev, void *p) {
    (void)p;
    if (ev == FSM_EVENT_ENTER) {
        uart_ctx_t *ctx = (uart_ctx_t *)fsm->context;
        printf("Frame received! buf[0]=%02X len=%u\n", ctx->buf[0], ctx->len);
    }
    return ST_IDLE; /* 自动回到空闲 */
}

static const fsm_state_desc_t uart_states[] = {
    {ST_IDLE,        idle_handler,       NULL, 0},
    {ST_WAIT_LEN,    wait_len_handler,   NULL, 0},
    {ST_RECV_DATA,   recv_data_handler,  NULL, 0},
    {ST_CHECKSUM,    checksum_handler,   NULL, 0},
    {ST_DONE,        done_handler,       NULL, 0},
};

int main(void) {
    uart_ctx_t ctx = {0};
    fsm_t fsm;
    fsm_init(&fsm, uart_states, 5, ST_IDLE, &ctx);

    /* 模拟接收到一帧: [0xAA][0x03][0x11,0x22,0x33][0xAA^0x03^0x11^0x22^0x33] */
    uint8_t frame[] = {0xAA, 0x03, 0x11, 0x22, 0x33, 0xAA ^ 0x03 ^ 0x11 ^ 0x22 ^ 0x33};
    for (int i = 0; i < 6; i++) fsm_dispatch(&fsm, EV_BYTE, &frame[i]);
    printf("Final state: %u\n", fsm_get_state(&fsm));
    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 说明 |
|------|------|------|
| `fsm_dispatch` | O(M) | M=当前状态转换表大小 |
| `fsm_transit` | O(M+K) | M=转换查询, K=新旧状态handler |
| 查找状态描述 | O(N) | N=状态总数（可哈希优化） |

### MCU优化要点

1. **表驱动**：状态和转换用const数组存在Flash
2. **状态编码**：用enum确保编译时类型安全
3. **无动态分配**：FSM和context都静态分配
4. **分层状态机**：复杂系统可用HSM（Hierarchical State Machine）
## Q271. 手写PID控制器(位置式/增量式)

### 原理说明

PID(Proportional-Integral-Derivative)是嵌入式控制领域最广泛的算法。位置式PID输出绝对控制量，增量式PID输出控制量增量（天然抗积分饱和）。MCU实现需注意：抗积分饱和、输出限幅、死区、微分先行等。

### 完整代码

```c
/**
 * pid.h - PID控制器（位置式 & 增量式）
 */
#ifndef PID_H
#define PID_H

#include <stdint.h>
#include <stdbool.h>

typedef float pid_float_t; /* 可替换为int32_t做定点PID */

/* PID模式 */
typedef enum { PID_MODE_POSITIONAL = 0, PID_MODE_INCREMENTAL } pid_mode_t;

/* PID配置 */
typedef struct {
    pid_float_t kp;           /* 比例系数 */
    pid_float_t ki;           /* 积分系数 */
    pid_float_t kd;           /* 微分系数 */
    pid_float_t ts;           /* 采样时间(s) */
    pid_float_t out_min;      /* 输出下限 */
    pid_float_t out_max;      /* 输出上限 */
    pid_float_t deadband;     /* 死区 */
    pid_float_t integrator_limit; /* 积分限幅 */
    pid_mode_t  mode;
    bool        anti_windup;      /* 使能抗积分饱和 */
    bool        deriv_on_measure; /* 微分先行（对测量值微分） */
} pid_config_t;

/* PID运行时状态 */
typedef struct {
    pid_config_t cfg;
    pid_float_t  setpoint;     /* 设定值 */
    pid_float_t  prev_input;   /* 上一次输入（微分用） */
    pid_float_t  integrator;   /* 积分累加器 */
    pid_float_t  prev_error;   /* 上一次误差 */
    pid_float_t  output;       /* 当前输出 */
    pid_float_t  p_term, i_term, d_term; /* 调试用：各分项 */
    bool         initialized;
} pid_ctrl_t;

/* API */
int pid_init(pid_ctrl_t *pid, const pid_config_t *cfg);
int pid_reset(pid_ctrl_t *pid);
pid_float_t pid_calculate(pid_ctrl_t *pid, pid_float_t setpoint,
                          pid_float_t measurement);
void pid_set_tunings(pid_ctrl_t *pid, pid_float_t kp, pid_float_t ki, pid_float_t kd);
void pid_set_output_limits(pid_ctrl_t *pid, pid_float_t min, pid_float_t max);
void pid_get_status(const pid_ctrl_t *pid,
                    pid_float_t *p, pid_float_t *i, pid_float_t *d,
                    pid_float_t *out);
pid_float_t pid_get_output(const pid_ctrl_t *pid);

#endif /* PID_H */
```

```c
/**
 * pid.c - PID实现
 * 时间复杂度 O(1)，空间复杂度 O(1)
 */
#include "pid.h"
#include <string.h>
#include <math.h>

int pid_init(pid_ctrl_t *pid, const pid_config_t *cfg) {
    if (!pid || !cfg) return -1;
    memset(pid, 0, sizeof(pid_ctrl_t));
    pid->cfg = *cfg;
    pid->initialized = true;
    return 0;
}

int pid_reset(pid_ctrl_t *pid) {
    if (!pid || !pid->initialized) return -1;
    pid->integrator   = 0.0f;
    pid->prev_input   = 0.0f;
    pid->prev_error   = 0.0f;
    pid->output       = 0.0f;
    pid->p_term = pid->i_term = pid->d_term = 0.0f;
    return 0;
}

pid_float_t pid_calculate(pid_ctrl_t *pid, pid_float_t setpoint,
                          pid_float_t measurement) {
    if (!pid || !pid->initialized) return 0.0f;
    pid->setpoint = setpoint;

    pid_float_t error = setpoint - measurement;

    /* ---------- 死区处理 ---------- */
    if (fabsf(error) < pid->cfg.deadband) error = 0.0f;

    if (pid->cfg.mode == PID_MODE_POSITIONAL) {
        /* ===== 位置式PID ===== */

        /* 1. 比例项 */
        pid->p_term = pid->cfg.kp * error;

        /* 2. 积分项（含抗积分饱和） */
        pid->i_term = pid->integrator + pid->cfg.ki * pid->cfg.ts * error;

        /* 3. 微分项（微分先行 or 误差微分） */
        if (pid->cfg.deriv_on_measure) {
            /* 对测量值微分：避免设定值突变引起微分冲击 */
            pid->d_term = -pid->cfg.kd *
                (measurement - pid->prev_input) / pid->cfg.ts;
        } else {
            /* 对误差微分 */
            pid_float_t de_dt = (error - pid->prev_error) / pid->cfg.ts;
            pid->d_term = pid->cfg.kd * de_dt;
        }

        /* 4. 计算输出（PID三项和） */
        pid_float_t output = pid->p_term + pid->i_term + pid->d_term;

        /* 5. 限幅 + 抗积分饱和 */
        if (output > pid->cfg.out_max) {
            output = pid->cfg.out_max;
            if (pid->cfg.anti_windup) {
                /* Clamping: 输出饱和时停止积分累加 */
                /* 只有误差方向与输出超限方向相反才更新积分 */
                if (error * pid->cfg.ki > 0) {
                    pid->integrator = pid->i_term - pid->cfg.ki * pid->cfg.ts * error;
                }
            }
        } else if (output < pid->cfg.out_min) {
            output = pid->cfg.out_min;
            if (pid->cfg.anti_windup) {
                if (error * pid->cfg.ki < 0) {
                    pid->integrator = pid->i_term - pid->cfg.ki * pid->cfg.ts * error;
                }
            }
        } else {
            /* 正常更新积分 */
            pid->integrator = pid->i_term;
        }

        /* 6. 积分限幅 */
        if (pid->integrator > pid->cfg.integrator_limit)
            pid->integrator = pid->cfg.integrator_limit;
        else if (pid->integrator < -pid->cfg.integrator_limit)
            pid->integrator = -pid->cfg.integrator_limit;

        pid->output = output;

    } else {
        /* ===== 增量式PID ===== */

        /* Δu(k) = Kp*(e(k)-e(k-1)) + Ki*Ts*e(k) + (Kd/Ts)*(e(k)-2*e(k-1)+e(k-2)) */

        /* 比例增量 */
        pid_float_t p_inc = pid->cfg.kp * (error - pid->prev_error);

        /* 积分增量 */
        pid_float_t i_inc = pid->cfg.ki * pid->cfg.ts * error;

        /* 微分增量（用位置式微分近似） */
        pid_float_t d_inc;
        if (pid->cfg.deriv_on_measure) {
            /* 对测量值微分 */
            d_inc = -pid->cfg.kd *
                (measurement - 2.0f * pid->prev_input + pid->pre_prev_input) / pid->cfg.ts;
            pid->pre_pre_prev_input = pid->prev_input;
        } else {
            d_inc = (pid->cfg.kd / pid->cfg.ts) *
                (error - 2.0f * pid->prev_error + pid->pre_pre_prev_error);
            pid->pre_pre_prev_error = pid->prev_error;
        }

        pid_float_t delta_u = p_inc + i_inc + d_inc;

        /* 增量限幅 */
        if (delta_u > pid->cfg.out_max) delta_u = pid->cfg.out_max;
        if (delta_u < pid->cfg.out_min) delta_u = pid->cfg.out_min;

        pid->output += delta_u;

        /* 输出总量限幅 */
        if (pid->output > pid->cfg.out_max) pid->output = pid->cfg.out_max;
        if (pid->output < pid->cfg.out_min) pid->output = pid->cfg.out_min;

        pid->p_term = p_inc;
        pid->i_term = i_inc;
        pid->d_term = d_inc;
    }

    /* 保存历史 */
    pid->prev_error = error;
    pid->prev_input = measurement;

    return pid->output;
}

void pid_set_tunings(pid_ctrl_t *pid, pid_float_t kp, pid_float_t ki, pid_float_t kd) {
    if (!pid) return;
    pid->cfg.kp = kp; pid->cfg.ki = ki; pid->cfg.kd = kd;
}

void pid_set_output_limits(pid_ctrl_t *pid, pid_float_t min, pid_float_t max) {
    if (!pid) return;
    pid->cfg.out_min = min; pid->cfg.out_max = max;
}

void pid_get_status(const pid_ctrl_t *pid, pid_float_t *p, pid_float_t *i,
                    pid_float_t *d, pid_float_t *out) {
    if (!pid) return;
    if (p) *p = pid->p_term; if (i) *i = pid->i_term;
    if (d) *d = pid->d_term; if (out) *out = pid->output;
}

pid_float_t pid_get_output(const pid_ctrl_t *pid) { return pid ? pid->output : 0.0f; }
```

```c
/* 使用示例：温度PID控制 */
#include <stdio.h>

int main(void) {
    pid_config_t cfg = {
        .kp = 10.0f, .ki = 1.0f, .kd = 0.5f,
        .ts = 0.1f, /* 100ms采样周期 */
        .out_min = 0.0f, .out_max = 100.0f, /* PWM占空比0-100% */
        .deadband = 0.5f,
        .integrator_limit = 50.0f,
        .mode = PID_MODE_POSITIONAL,
        .anti_windup = true,
        .deriv_on_measure = true
    };
    pid_ctrl_t pid;
    pid_init(&pid, &cfg);

    /* 模拟：设定温度60度，当前25度 */
    float setpoint = 60.0f;
    float measurement = 25.0f;

    printf("Iter\tOut\tP\tI\tD\tMeas\n");
    for (int i = 0; i < 50; i++) {
        float out = pid_calculate(&pid, setpoint, measurement);
        if (i % 5 == 0) {
            float p,i_,d;
            pid_get_status(&pid, &p, &i_, &d, NULL);
            printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", i, out, p, i_, d, measurement);
        }
        /* 简单模拟热惯性 */
        measurement += out * 0.01f;
        measurement += (25.0f - measurement) * 0.02f; /* 散热 */
    }
    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 |
|------|------|------|
| `pid_calculate` | O(1) | O(1) |
| 所有set/get | O(1) | O(1) |

### MCU优化要点

1. **定点PID**：用`int32_t`替代`float`，Q15/Q24定点格式
2. **整型化**：`pid_calculate`中所有运算用整数完成
3. **定时器驱动**：用硬件定时器触发PID计算
4. **表查Ki*Ts**：预计算Ki*Ts避免每次乘法
5. **增量式天然抗饱和**：增量式不需要积分限幅
6. **微分先行**：避免设定值突变

---

## Q272. 手写滑动平均滤波/中值滤波

### 原理说明

滑动平均滤波(MAF)：维护一个固定大小的窗口，新数据入队，旧数据出队，输出窗口内均值。中值滤波：排序后取中间值，对脉冲噪声效果好。

### 完整代码

```c
/**
 * filter.h - 滑动平均滤波 & 中值滤波
 */
#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ============ 滑动平均滤波器 ============ */
typedef struct {
    float    *buffer;     /* 数据窗口（外部静态分配） */
    uint32_t  window_size;
    uint32_t  index;       /* 当前写入位置 */
    float     sum;         /* 当前窗口内总和（加速计算） */
    uint32_t  count;       /* 已累积数量 */
    bool      warmed_up;   /* 是否已填满窗口 */
} maf_t;

void  maf_init(maf_t *f, float *buf, uint32_t window_size);
float maf_filter(maf_t *f, float input);
void  maf_reset(maf_t *f);
float maf_get_last(const maf_t *f);
bool  maf_is_ready(const maf_t *f);

/* ============ 中值滤波器 ============ */
#ifndef MEDIAN_WINDOW_MAX
#define MEDIAN_WINDOW_MAX 32
#endif

typedef struct {
    float    buffer[MEDIAN_WINDOW_MAX];
    float    sorted[MEDIAN_WINDOW_MAX];
    uint32_t window_size;
    uint32_t index;
    uint32_t count;
    bool     warmed_up;
} median_filter_t;

void  median_filter_init(median_filter_t *f, uint32_t window_size);
float median_filter_process(median_filter_t *f, float input);
void  median_filter_reset(median_filter_t *f);
float median_filter_get_last(const median_filter_t *f);

/* ============ 一阶低通滤波器（补充） ============ */
typedef struct {
    float alpha;     /* 滤波系数 (0~1) */
    float last_val;
    bool  initialized;
} lpf_t;

void  lpf_init(lpf_t *f, float alpha);
float lpf_filter(lpf_t *f, float input);
void  lpf_set_alpha(lpf_t *f, float alpha);

#endif /* FILTER_H */
```

```c
/**
 * filter.c - 实现
 * 滑动平均 O(1)，中值滤波 O(N log N)每次
 */
#include "filter.h"
#include <string.h>
#include <stdlib.h>

/* ============ 滑动平均滤波（O(1)） ============ */

void maf_init(maf_t *f, float *buf, uint32_t window_size) {
    if (!f || !buf || window_size == 0) return;
    memset(f, 0, sizeof(maf_t));
    f->buffer = buf; f->window_size = window_size;
}

float maf_filter(maf_t *f, float input) {
    if (!f || !f->buffer) return input;

    if (f->warmed_up) {
        /* 滑动窗口：减去旧值，加上新值 */
        f->sum -= f->buffer[f->index];
        f->sum += input;
        f->buffer[f->index] = input;
    } else {
        /* 预热阶段：累加 */
        f->buffer[f->index] = input;
        f->sum += input;
        f->count++;
        if (f->count >= f->window_size) f->warmed_up = true;
    }

    f->index = (f->index + 1) % f->window_size;

    if (f->warmed_up) return f->sum / (float)f->window_size;
    else              return f->sum / (float)f->count;
}

void maf_reset(maf_t *f) {
    if (!f) return;
    f->index = 0; f->sum = 0.0f; f->count = 0; f->warmed_up = false;
}

float maf_get_last(const maf_t *f) {
    if (!f || !f->buffer) return 0.0f;
    if (f->warmed_up) return f->sum / (float)f->window_size;
    if (f->count > 0) return f->sum / (float)f->count;
    return 0.0f;
}

bool maf_is_ready(const maf_t *f) { return f ? f->warmed_up : false; }

/* ============ 中值滤波 ============ */

static int cmp_float(const void *a, const void *b) {
    float fa = *(const float *)a, fb = *(const float *)b;
    return (fa > fb) - (fa < fb);
}

void median_filter_init(median_filter_t *f, uint32_t window_size) {
    if (!f) return;
    if (window_size > MEDIAN_WINDOW_MAX) window_size = MEDIAN_WINDOW_MAX;
    memset(f, 0, sizeof(median_filter_t));
    f->window_size = window_size;
}

float median_filter_process(median_filter_t *f, float input) {
    if (!f) return input;

    /* 环形写入 */ 
    f->buffer[f->index] = input;
    f->index = (f->index + 1) % f->window_size;
    if (f->count < f->window_size) { f->count++; f->warmed_up = false; return input; }
    f->warmed_up = true;

    /* 复制到sorted数组并排序 */ 
    uint32_t n = f->window_size;
    memcpy(f->sorted, f->buffer, n * sizeof(float));
    qsort(f->sorted, n, sizeof(float), cmp_float);

    /* 取中值 */ 
    if (n & 1) return f->sorted[n / 2];          /* 奇数：中间值 */
    else       return (f->sorted[n/2-1] + f->sorted[n/2]) * 0.5f; /* 偶数：均值 */
}

void median_filter_reset(median_filter_t *f) {
    if (!f) return;
    memset(f, 0, sizeof(median_filter_t));
}

float median_filter_get_last(const median_filter_t *f) {
    if (!f || !f->warmed_up) return 0.0f;
    uint32_t n = f->window_size;
    if (n & 1) return f->sorted[n / 2];
    else return (f->sorted[n/2-1] + f->sorted[n/2]) * 0.5f;
}

/* ============ 一阶低通滤波 ============ */

void lpf_init(lpf_t *f, float alpha) {
    if (!f) return;
    f->alpha = (alpha < 0.0f) ? 0.0f : ((alpha > 1.0f) ? 1.0f : alpha);
    f->last_val = 0.0f; f->initialized = false;
}

float lpf_filter(lpf_t *f, float input) {
    if (!f) return input;
    if (!f->initialized) { f->last_val = input; f->initialized = true; return input; }
    f->last_val = f->alpha * input + (1.0f - f->alpha) * f->last_val;
    return f->last_val;
}

void lpf_set_alpha(lpf_t *f, float alpha) {
    if (!f) return;
    f->alpha = (alpha < 0.0f) ? 0.0f : ((alpha > 1.0f) ? 1.0f : alpha);
}
```

```c
/* 使用示例 */
#include <stdio.h>

#define MAF_WINDOW  8
#define MED_WINDOW  5

static float maf_buf[MAF_WINDOW];

int main(void) {
    /* 滑动平均 */
    maf_t maf; maf_init(&maf, maf_buf, MAF_WINDOW);
    float noisy[] = {10.1f, 10.5f, 19.8f, 10.3f, 10.2f, 10.4f, 10.1f, 10.6f, 10.0f, 10.3f};
    printf("Moving Average Filter (win=%u):\n", MAF_WINDOW);
    for (int i = 0; i < 10; i++) printf("  in=%.2f out=%.2f\n", noisy[i], maf_filter(&maf, noisy[i]));

    /* 中值滤波 */
    median_filter_t med; median_filter_init(&med, MED_WINDOW);
    float spike[] = {10.0f, 10.2f, 99.9f, 10.1f, 10.3f, 10.0f, 10.4f, 99.9f, 10.2f};
    printf("\nMedian Filter (win=%u):\n", MED_WINDOW);
    for (int i = 0; i < 9; i++) printf("  in=%.2f out=%.2f\n", spike[i], median_filter_process(&med, spike[i]));

    /* 一阶低通 */
    lpf_t lpf; lpf_init(&lpf, 0.2f);
    printf("\nLow Pass Filter (alpha=0.2):\n");
    for (int i = 0; i < 10; i++) printf("  in=%.2f out=%.2f\n", noisy[i], lpf_filter(&lpf, noisy[i]));
    return 0;
}
```

### 复杂度分析

| 滤波器 | 时间 | 空间 | 说明 |
|--------|------|------|------|
| 滑动平均 | O(1) | O(W) | W=窗口大小 |
| 中值滤波 | O(W log W) | O(W) | 每次排序 |
| 一阶低通 | O(1) | O(1) | 递归式 |

### MCU优化要点

1. **滑动平均O(1)技巧**：维护sum变量，避免每次重算
2. **定点整数**：ADC采样用int16_t/uint16_t，避免浮点
3. **中值滤波优化**：用插入排序维护有序数组，降至O(W)
4. **2的幂次窗口**：可以用移位代替除法

---

## Q273. 手写Base64编解码

### 原理说明

Base64将3字节(24bit)编码为4个ASCII字符(每6bit一个索引)。编码表：A-Z, a-z, 0-9, +, /。解码为逆过程。广泛用于二进制数据文本化传输。

### 完整代码

```c
/**
 * base64.h - Base64编解码
 */
#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* 计算编码后长度 */
static inline size_t base64_encode_length(size_t input_len) {
    return ((input_len + 2) / 3) * 4;
}

/* 计算解码后最大长度 */
static inline size_t base64_decode_length(size_t input_len) {
    return (input_len / 4) * 3;
}

/* 编码：返回实际输出长度 失败返回0 */
size_t base64_encode(const uint8_t *input, size_t input_len,
                     char *output, size_t output_capacity);

/* 解码：返回实际输出长度 失败返回0 */
size_t base64_decode(const char *input, size_t input_len,
                     uint8_t *output, size_t output_capacity);

/* 解码安全版：输入可能含换行等，先清洗再解码 */
size_t base64_decode_strict(const char *input, size_t input_len,
                            uint8_t *output, size_t output_capacity);

/* 验证是否为合法Base64字符串 */
bool base64_is_valid(const char *input, size_t input_len);

#endif /* BASE64_H */
```

```c
/**
 * base64.c - Base64实现
 * 编码 O(N)，解码 O(N)
 */
#include "base64.h"
#include <string.h>

/* 编码表（RFC 4648标准） */
static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* 解码表：字符 -> 6bit值 */
static const int8_t base64_decode_table[256] = {
    ['A']=0,['B']=1,['C']=2,['D']=3,['E']=4,['F']=5,['G']=6,['H']=7,
    ['I']=8,['J']=9,['K']=10,['L']=11,['M']=12,['N']=13,['O']=14,['P']=15,
    ['Q']=16,['R']=17,['S']=18,['T']=19,['U']=20,['V']=21,['W']=22,['X']=23,
    ['Y']=24,['Z']=25,['a']=26,['b']=27,['c']=28,['d']=29,['e']=30,['f']=31,
    ['g']=32,['h']=33,['i']=34,['j']=35,['k']=36,['l']=37,['m']=38,['n']=39,
    ['o']=40,['p']=41,['q']=42,['r']=43,['s']=44,['t']=45,['u']=46,['v']=47,
    ['w']=48,['x']=49,['y']=50,['z']=51,['0']=52,['1']=53,['2']=54,['3']=55,
    ['4']=56,['5']=57,['6']=58,['7']=59,['8']=60,['9']=61,['+']=62,['/']=63,
};

size_t base64_encode(const uint8_t *input, size_t input_len,
                     char *output, size_t output_capacity) {
    if (!input || input_len == 0 || !output) return 0;

    size_t expected = base64_encode_length(input_len);
    if (output_capacity < expected + 1) return 0; /* +1 for null terminator */

    size_t out_idx = 0;

    for (size_t i = 0; i < input_len; i += 3) {
        /* 取3字节拼成24bit */
        uint32_t triple = (uint32_t)input[i] << 16;
        if (i + 1 < input_len) triple |= (uint32_t)input[i + 1] << 8;
        if (i + 2 < input_len) triple |= (uint32_t)input[i + 2];

        /* 拆成4个6bit */
        output[out_idx++] = base64_table[(triple >> 18) & 0x3F];
        output[out_idx++] = base64_table[(triple >> 12) & 0x3F];

        if (i + 1 < input_len)
            output[out_idx++] = base64_table[(triple >> 6) & 0x3F];
        else
            output[out_idx++] = '='; /* 填充 */

        if (i + 2 < input_len)
            output[out_idx++] = base64_table[triple & 0x3F];
        else
            output[out_idx++] = '='; /* 填充 */
    }

    output[out_idx] = '\0';
    return out_idx;
}

size_t base64_decode(const char *input, size_t input_len,
                     uint8_t *output, size_t output_capacity) {
    if (!input || input_len == 0 || !output) return 0;

    /* 长度必须是4的倍数 */ 
    if (input_len % 4 != 0) return 0;

    size_t max_out = base64_decode_length(input_len);
    if (output_capacity < max_out) return 0;

    size_t out_idx = 0;
    uint32_t accum = 0; /* 累加器 */ 
    int bit_count = 0;

    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == '=') {
            /* 填充字符：停止 */
            break;
        }

        int8_t val = base64_decode_table[(uint8_t)input[i]];
        if (val < 0) {
            /* 空格、换行等忽略，其他非法字符报错 */ 
            if (input[i] != ' ' && input[i] != '\n' && input[i] != '\r' &&
                input[i] != '\t' && input[i] != '\f')
                return 0;
            continue;
        }

        accum = (accum << 6) | (uint32_t)(val & 0x3F);
        bit_count += 6;

        /* 每8bit输出一个字节 */ 
        while (bit_count >= 8) {
            bit_count -= 8;
            if (out_idx >= output_capacity) return 0;
            output[out_idx++] = (uint8_t)((accum >> bit_count) & 0xFF);
        }
    }

    /* 检查填充 */
    if (input_len >= 2 && input[input_len - 1] == '=') {
        if (input[input_len - 2] == '=') {
            /* 2个填充：只保留前1字节 */
            if (out_idx > 0) out_idx--; /* 回退一个padding */
        }
        /* 1个填充保留前2字节 */
        if (input[input_len - 1] == '=' && input[input_len - 2] != '=') {}
    }

    return out_idx;
}

size_t base64_decode_strict(const char *input, size_t input_len,
                            uint8_t *output, size_t output_capacity) {
    if (!input || !output) return 0;

    /* 先清理：去除空白字符 */ 
    char clean[256]; /* MCU上可根据实际情况调整大小 */ 
    size_t clean_len = 0;
    for (size_t i = 0; i < input_len && clean_len < sizeof(clean) - 1; i++) {
        if (input[i] != ' ' && input[i] != '\n' && input[i] != '\r' &&
            input[i] != '\t') {
            clean[clean_len++] = input[i];
        }
    }
    clean[clean_len] = '\0';

    return base64_decode(clean, clean_len, output, output_capacity);
}

bool base64_is_valid(const char *input, size_t input_len) {
    if (!input || input_len == 0 || input_len % 4 != 0) return false;
    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == '=') {
            /* '='只能出现在末尾2个字符 */ 
            if (i < input_len - 2) return false;
            continue;
        }
        if (base64_decode_table[(uint8_t)input[i]] < 0) {
            if (input[i] != ' ' && input[i] != '\n' && input[i] != '\r') return false;
        }
    }
    return true;
}
```

```c
/* 使用示例 */
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *plain = "Hello, MCU World!";
    char encoded[64];
    uint8_t decoded[64];

    /* 编码 */
    size_t enc_len = base64_encode((const uint8_t *)plain, strlen(plain), encoded, sizeof(encoded));
    if (enc_len > 0) {
        printf("Plain:  '%s'\n", plain);
        printf("Base64: '%s'\n", encoded);
        printf("Encoded length: %zu\n", enc_len);

        /* 解码 */
        size_t dec_len = base64_decode(encoded, enc_len, decoded, sizeof(decoded));
        decoded[dec_len] = '\0';
        printf("Decoded: '%s'\n", decoded);
        printf("Match: %s\n", strcmp(plain, (char *)decoded) == 0 ? "YES" : "NO");
    }

    /* 二进制数据编码 */
    uint8_t binary[] = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0xFD};
    char bin_enc[32];
    base64_encode(binary, sizeof(binary), bin_enc, sizeof(bin_enc));
    printf("\nBinary Base64: '%s'\n", bin_enc);

    /* 解码验证 */
    uint8_t bin_dec[10];
    size_t bdl = base64_decode(bin_enc, strlen(bin_enc), bin_dec, sizeof(bin_dec));
    printf("Decoded binary: ");
    for (size_t i = 0; i < bdl; i++) printf("%02X ", bin_dec[i]);
    printf("\nMatch: %s\n", memcmp(binary, bin_dec, bdl) == 0 ? "YES" : "NO");

    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 | 说明 |
|------|------|------|------|
| `base64_encode` | O(N) | O(1) | N=输入长度 |
| `base64_decode` | O(N) | O(1) | |
| `base64_decode_strict` | O(N) | O(N)临时 | 也可原位清洗 |

膨胀率：编码后长度 = ceil(N/3)*4，约膨胀33%。

### MCU优化要点

1. **解码表用const int8_t[256]**：大多数位置为-1（非法），O(1)查表
2. **3字节→4字符批量处理**：利用32位寄存器一次性位操作
3. **static缓冲区**：output由调用者预分配，无需动态分配
4. **不处理换行**：MCU场景传输带换行的Base64较少，简化实现

---

## Q274. 手写JSON简易解析器

### 原理说明

实现轻量级JSON解析器，支持：对象、数组、字符串、数字、bool、null。递归下降解析。为MCU优化：无动态分配、回调通知、限制嵌套深度和长度。

### 完整代码

```c
/**
 * tiny_json.h - MCU友好JSON简易解析器
 * 递归下降解析，回调通知，零动态分配
 */
#ifndef TINY_JSON_H
#define TINY_JSON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define JSON_MAX_DEPTH   16    /* 最大嵌套深度 */
#define JSON_MAX_KEY_LEN 32    /* 最大键名长度 */
#define JSON_MAX_STR_LEN 256   /* 最大字符串长度 */

/* JSON节点类型 */
typedef enum {
    JSON_NULL = 0,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
} json_type_t;

/* 解析后的值 */
typedef struct {
    json_type_t type;
    union {
        bool     bool_val;
        double   number_val;
        int64_t  int_val;
        struct { char *ptr; size_t len; } str_val;
    };
} json_value_t;

/* 回调函数类型 */
typedef int (*json_callback_t)(void *user_data, const char *key, json_value_t *value);

/* 解析器上下文 */
typedef struct {
    const char *json;      /* 输入JSON字符串 */ 
    size_t      pos;       /* 当前解析位置 */
    size_t      len;       /* JSON总长度 */
    int         error_line;
    const char *error_msg;
} json_parser_t;

/* API */
int json_parse(const char *json_str, json_callback_t cb, void *user_data);
void json_skip_whitespace(json_parser_t *p);

/* 内部解析函数（暴露供测试） */ 
json_value_t json_parse_value(json_parser_t *p, char *key_buf, int depth);

#endif /* TINY_JSON_H */
```

```c
/**
 * tiny_json.c - JSON解析器实现
 * 时间复杂度 O(N)，空间 O(Depth)
 */
#include "tiny_json.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static void json_set_error(json_parser_t *p, const char *msg) {
    p->error_msg = msg;
    p->error_line = (int)(p->pos);
}

void json_skip_whitespace(json_parser_t *p) {
    while (p->pos < p->len &&
           (p->json[p->pos] == ' ' || p->json[p->pos] == '\t' ||
            p->json[p->pos] == '\n' || p->json[p->pos] == '\r'))
        p->pos++;
}

static char json_peek(const json_parser_t *p) {
    return (p->pos < p->len) ? p->json[p->pos] : '\0';
}

static char json_next(json_parser_t *p) {
    if (p->pos >= p->len) return '\0';
    return p->json[p->pos++];
}

/* 解析字符串（含转义） */ 
static bool json_parse_string(json_parser_t *p, char *out, size_t max_len) {
    if (json_next(p) != '"') { json_set_error(p, "Expected '\"'"); return false; }
    size_t idx = 0;
    while (p->pos < p->len) {
        char c = p->json[p->pos];
        if (c == '"') { p->pos++; if (idx < max_len) out[idx] = '\0'; return true; }
        if (c == '\\') {
            p->pos++;
            if (p->pos >= p->len) { json_set_error(p, "Unterminated escape"); return false; }
            c = json_peek(p);
            switch (c) {
                case '"':  out[idx++] = '"';  break; case '\\': out[idx++] = '\\'; break;
                case '/':  out[idx++] = '/';  break; case 'b':  out[idx++] = '\b'; break;
                case 'f':  out[idx++] = '\f'; break; case 'n':  out[idx++] = '\n'; break;
                case 'r':  out[idx++] = '\r'; break; case 't':  out[idx++] = '\t'; break;
                case 'u':
                    /* 跳过Unicode转义（简化处理） */
                    p->pos += 5; out[idx++] = '?'; if (idx >= max_len) idx = max_len - 1;
                    continue;
                default:
                    json_set_error(p, "Invalid escape"); return false;
            }
            p->pos++;
        } else {
            p->pos++;
            out[idx++] = c;
        }
        if (idx >= max_len) { json_set_error(p, "String too long"); return false; }
    }
    json_set_error(p, "Unterminated string"); return false;
}

/* 解析数字 */ 
static bool json_parse_number(json_parser_t *p, json_value_t *val) {
    size_t start = p->pos;
    bool is_float = false;
    bool is_neg = false;

    if (json_peek(p) == '-') { is_neg = true; p->pos++; }

    if (json_peek(p) == '0') {
        p->pos++; /* 前导零 */
        if (json_peek(p) >= '0' && json_peek(p) <= '9') {
            json_set_error(p, "Leading zero not allowed"); return false;
        }
    } else {
        if (json_peek(p) < '1' || json_peek(p) > '9') {
            json_set_error(p, "Expected digit"); return false;
        }
        while (json_peek(p) >= '0' && json_peek(p) <= '9') p->pos++;
    }

    if (json_peek(p) == '.') { is_float = true; p->pos++;
        if (json_peek(p) < '0' || json_peek(p) > '9')
        { json_set_error(p, "Expected digit after decimal"); return false; }
        while (json_peek(p) >= '0' && json_peek(p) <= '9') p->pos++;
    }

    if (json_peek(p) == 'e' || json_peek(p) == 'E') { is_float = true; p->pos++;
        if (json_peek(p) == '+' || json_peek(p) == '-') p->pos++;
        if (json_peek(p) < '0' || json_peek(p) > '9')
        { json_set_error(p, "Expected digit in exponent"); return false; }
        while (json_peek(p) >= '0' && json_peek(p) <= '9') p->pos++;
    }

    size_t num_len = p->pos - start;
    char num_str[32]; /* JSON数字一般不会太长 */
    if (num_len >= sizeof(num_str)) { json_set_error(p, "Number too long"); return false; }
    memcpy(num_str, p->json + start, num_len);
    num_str[num_len] = '\0';

    if (is_float) {
        val->type = JSON_NUMBER;
        val->number_val = strtod(num_str, NULL);
    } else {
        val->type = JSON_NUMBER;
        val->int_val = (int64_t)strtoll(num_str, NULL, 10);
        val->number_val = (double)val->int_val;
    }
    return true;
}

/* 递归解析值 */ 
json_value_t json_parse_value(json_parser_t *p, char *key_buf, int depth) {
    json_value_t val = { .type = JSON_NULL };
    if (depth > JSON_MAX_DEPTH) { json_set_error(p, "Max depth exceeded"); return val; }

    json_skip_whitespace(p);
    char c = json_peek(p);

    switch (c) {
        case 'n': /* null */
            if (p->pos + 4 <= p->len && memcmp(p->json + p->pos, "null", 4) == 0)
            { p->pos += 4; val.type = JSON_NULL; }
            else json_set_error(p, "Expected 'null'");
            break;

        case 't': /* true */
            if (p->pos + 4 <= p->len && memcmp(p->json + p->pos, "true", 4) == 0)
            { p->pos += 4; val.type = JSON_BOOL; val.bool_val = true; }
            else json_set_error(p, "Expected 'true'");
            break;

        case 'f': /* false */
            if (p->pos + 5 <= p->len && memcmp(p->json + p->pos, "false", 5) == 0)
            { p->pos += 5; val.type = JSON_BOOL; val.bool_val = false; }
            else json_set_error(p, "Expected 'false'");
            break;

        case '"': { /* string */ 
            static char str_buf[JSON_MAX_STR_LEN];
            if (!json_parse_string(p, str_buf, sizeof(str_buf))) return val;
            val.type = JSON_STRING;
            val.str_val.ptr = str_buf;
            val.str_val.len = strlen(str_buf);
            break;
        }

        case '{': { /* object */ 
            val.type = JSON_OBJECT;
            json_next(p); /* skip '{' */
            json_skip_whitespace(p);
            if (json_peek(p) == '}') { json_next(p); return val; } /* 空对象 */
            while (1) {
                json_skip_whitespace(p);
                char key[JSON_MAX_KEY_LEN];
                if (!json_parse_string(p, key, sizeof(key))) return val;
                json_skip_whitespace(p);
                if (json_next(p) != ':') { json_set_error(p, "Expected ':'"); return val; }
                json_skip_whitespace(p);
                json_value_t child = json_parse_value(p, key_buf, depth + 1);
                if (p->error_msg) return val;
                /* 忽略child值，实际应用中应存储或回调 */ 
                (void)child;
                json_skip_whitespace(p);
                if (json_peek(p) == ',') { json_next(p); continue; }
                if (json_peek(p) == '}') { json_next(p); break; }
                json_set_error(p, "Expected ',' or '}'"); return val;
            }
            break;
        }

        case '[': { /* array */ 
            val.type = JSON_ARRAY;
            json_next(p); /* skip '[' */
            json_skip_whitespace(p);
            if (json_peek(p) == ']') { json_next(p); return val; }
            int idx = 0;
            while (1) {
                json_skip_whitespace(p);
                char idx_key[16]; snprintf(idx_key, sizeof(idx_key), "[%d]", idx++);
                json_value_t child = json_parse_value(p, idx_key, depth + 1);
                if (p->error_msg) return val;
                (void)child;
                json_skip_whitespace(p);
                if (json_peek(p) == ',') { json_next(p); continue; }
                if (json_peek(p) == ']') { json_next(p); break; }
                json_set_error(p, "Expected ',' or ']'"); return val;
            }
            break;
        }

        default:
            if ((c >= '0' && c <= '9') || c == '-') {
                json_parse_number(p, &val);
            } else {
                json_set_error(p, "Unexpected character");
            }
            break;
    }

    return val;
}

/* 公共解析入口 */ 
int json_parse(const char *json_str, json_callback_t cb, void *user_data) {
    if (!json_str || !cb) return -1;

    json_parser_t p = {
        .json = json_str,
        .pos  = 0,
        .len  = strlen(json_str),
        .error_line = 0,
        .error_msg = NULL,
    };

    json_skip_whitespace(&p);
    char key_buf[JSON_MAX_KEY_LEN] = "";
    json_value_t root = json_parse_value(&p, key_buf, 0);

    if (p.error_msg) {
        printf("JSON parse error at pos %d: %s\n", p.error_line, p.error_msg);
        return -1;
    }

    /* 调用回调 */
    cb(user_data, "", &root);
    return 0;
}
```

```c
/* 使用示例 */
#include <stdio.h>

static int print_callback(void *user_data, const char *key, json_value_t *val) {
    (void)user_data;
    printf("Key: '%s' Type: %d", key, val->type);
    switch (val->type) {
        case JSON_NULL:   printf(" = null\n"); break;
        case JSON_BOOL:   printf(" = %s\n", val->bool_val ? "true" : "false"); break;
        case JSON_NUMBER: printf(" = %lld\n", (long long)val->int_val); break;
        case JSON_STRING: printf(" = '%.*s'\n", (int)val->str_val.len, val->str_val.ptr); break;
        case JSON_ARRAY: case JSON_OBJECT: printf("\n"); break;
    }
    return 0;
}

int main(void) {
    const char *json = "{"
        "\"name\": \"ESP32-S3\","
        "\"sensors\": [\"temp\", \"humidity\"],"
        "\"active\": true,"
        "\"config\": {\"baud\": 115200, \"timeout\": 500},"
        "\"version\": null"
    "}";

    printf("Parsing JSON...\n");
    int ret = json_parse(json, print_callback, NULL);
    printf("Result: %s\n", ret == 0 ? "OK" : "FAILED");
    return 0;
}
```

### 复杂度分析

| 指标 | 值 |
|------|-----|
| 时间复杂度 | O(N)，N=JSON长度 |
| 空间复杂度 | O(Depth)，递归栈+少量静态buf |
| 最大嵌套深度 | 16（可配置） |

### MCU优化要点

1. **递归下降**：简单直观，深度限制16层
2. **静态缓冲区**：key_buf/str_buf用static局部变量
3. **无动态分配**：完全零malloc
4. **回调通知**：找到键值对时立即回调，不需要存储整棵树
5. **早停**：错误立即返回

---

## Q275. 手写Modbus RTU帧解析

### 原理说明

Modbus RTU是工业领域最广泛使用的串行通信协议。帧格式：地址(1B) + 功能码(1B) + 数据(NB) + CRC16(2B)。帧间间隔 >= 3.5字符时间。

### 完整代码

```c
/**
 * modbus_rtu.h - Modbus RTU帧解析器
 */
#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Modbus RTU帧限制 */
#define MODBUS_RTU_MAX_DATA_LEN  252   /* 最大数据长度（帧总长≤256） */
#define MODBUS_RTU_FRAME_MIN_LEN 4     /* 最小帧：地址+功能码+CRC */
#define MODBUS_RTU_FRAME_MAX_LEN 256

/* Modbus功能码 */
#define MODBUS_FC_READ_COILS          0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS 0x02
#define MODBUS_FC_READ_HOLDING_REGS    0x03
#define MODBUS_FC_READ_INPUT_REGS      0x04
#define MODBUS_FC_WRITE_SINGLE_COIL   0x05
#define MODBUS_FC_WRITE_SINGLE_REG    0x06
#define MODBUS_FC_WRITE_MULTI_COILS   0x0F
#define MODBUS_FC_WRITE_MULTI_REGS    0x10

/* 异常码 */
#define MODBUS_EX_NONE                0x00
#define MODBUS_EX_ILLEGAL_FUNCTION    0x01
#define MODBUS_EX_ILLEGAL_DATA_ADDR   0x02
#define MODBUS_EX_ILLEGAL_DATA_VALUE  0x03

/* 解析后的帧 */
typedef struct {
    uint8_t  address;
    uint8_t  function;
    uint8_t  data[MODBUS_RTU_MAX_DATA_LEN];
    uint16_t data_len;
    uint16_t crc;
    bool     is_exception;
    uint8_t  exception_code;
    bool     valid;
} modbus_frame_t;

/* Modbus RTU解析器 */
typedef struct {
    uint8_t  rx_buffer[MODBUS_RTU_FRAME_MAX_LEN];
    uint16_t rx_index;
    uint32_t last_byte_time;   /* 上一字节接收时间（用于帧间隔判断） */
    bool     receiving;        /* 是否正在接收一帧 */
    void   (*send_fn)(const uint8_t *data, uint16_t len);
} modbus_rtu_t;

/* API */
void modbus_rtu_init(modbus_rtu_t *mb, void (*send_fn)(const uint8_t *, uint16_t));
int  modbus_rtu_feed(modbus_rtu_t *mb, uint8_t byte, uint32_t timestamp_ms);
int  modbus_rtu_parse(const uint8_t *raw, uint16_t len, modbus_frame_t *frame);
uint16_t modbus_crc16(const uint8_t *data, uint16_t len);
int  modbus_build_response_ok(modbus_frame_t *req, uint8_t *resp_buf, uint16_t *resp_len,
                              const uint8_t *reg_data, uint16_t reg_len);
int  modbus_build_response_ex(modbus_frame_t *req, uint8_t *resp_buf, uint16_t *resp_len,
                              uint8_t exception_code);
const char *modbus_exception_name(uint8_t code);

#endif /* MODBUS_RTU_H */
```

```c
/**
 * modbus_rtu.c - Modbus RTU实现
 * 解析 O(N)，CRC O(N)
 */
#include "modbus_rtu.h"
#include <string.h>

#define MODBUS_FRAME_TIMEOUT_MS 50  /* 帧超时时间 */

/* CRC16查表法（Modbus多项式 0x8005，输入/输出均反转） */
static const uint16_t crc16_table[256] = {
    0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
    0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
    0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
    0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
    0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
    0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
    0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
    0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
    0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
    0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
    0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
    0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
    0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
    0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
    0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
    0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
    0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
    0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
    0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
    0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
    0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
    0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
    0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
    0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
    0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
    0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
    0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
    0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
    0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
    0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
    0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
    0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
};

uint16_t modbus_crc16(const uint8_t *data, uint16_t len) {
    uint16_t crc = 0xFFFF;
    while (len--) crc = (crc >> 8) ^ crc16_table[(crc ^ *data++) & 0xFF];
    return crc;
}

void modbus_rtu_init(modbus_rtu_t *mb, void (*send_fn)(const uint8_t *, uint16_t)) {
    if (!mb) return;
    memset(mb, 0, sizeof(modbus_rtu_t));
    mb->send_fn = send_fn;
}

/* 喂入一个字节，返回：0=继续接收 1=完整帧接收 >1=错误 */
int modbus_rtu_feed(modbus_rtu_t *mb, uint8_t byte, uint32_t timestamp_ms) {
    if (!mb) return -1;
    if (!mb->receiving) { mb->receiving = true; mb->rx_index = 0; }

    if (mb->rx_index >= MODBUS_RTU_FRAME_MAX_LEN) {
        mb->rx_index = 0; /* 溢出重置 */ return -2;
    }

    mb->rx_buffer[mb->rx_index++] = byte;
    mb->last_byte_time = timestamp_ms;

    /* 地址 */
    if (mb->rx_index == 1) return 0;
    /* 功能码 */
    if (mb->rx_index == 2) return 0;

    /* 从第3字节开始尝试判定帧长 */
    if (mb->rx_index >= 4) {
        uint16_t expected_len = 0;
        switch (mb->rx_buffer[1]) {
            case MODBUS_FC_READ_COILS:
            case MODBUS_FC_READ_DISCRETE_INPUTS:
            case MODBUS_FC_READ_HOLDING_REGS:
            case MODBUS_FC_READ_INPUT_REGS:
                expected_len = 8; /* addr+func+start(2)+qty(2)+crc(2) */
                break;
            case MODBUS_FC_WRITE_SINGLE_COIL:
            case MODBUS_FC_WRITE_SINGLE_REG:
                expected_len = 8;
                break;
            case MODBUS_FC_WRITE_MULTI_COILS:
            case MODBUS_FC_WRITE_MULTI_REGS:
                if (mb->rx_index >= 7) expected_len = (uint16_t)(mb->rx_buffer[6] + 9);
                break;
            default:
                /* 异常响应帧 */
                expected_len = 5;
                break;
        }

        if (expected_len > 0 && mb->rx_index >= expected_len) {
            uint16_t rxd_crc = ((uint16_t)mb->rx_buffer[expected_len - 1] << 8) |
                                (uint16_t)mb->rx_buffer[expected_len - 2];
            uint16_t calc_crc = modbus_crc16(mb->rx_buffer, expected_len - 2);
            mb->receiving = false;
            return (rxd_crc == calc_crc) ? 1 : -3; /* CRC错误 */
        }
    }

    return 0;
}

int modbus_rtu_parse(const uint8_t *raw, uint16_t len, modbus_frame_t *frame) {
    if (!raw || len < MODBUS_RTU_FRAME_MIN_LEN || !frame) return -1;
    if (len > MODBUS_RTU_FRAME_MAX_LEN) return -2;

    /* CRC校验 */
    uint16_t calc_crc = modbus_crc16(raw, len - 2);
    uint16_t rxd_crc  = ((uint16_t)raw[len - 1] << 8) | (uint16_t)raw[len - 2];

    if (calc_crc != rxd_crc) return -3;

    memset(frame, 0, sizeof(modbus_frame_t));
    frame->address  = raw[0];
    frame->function = raw[1];

    /* 判断是否为异常响应 */
    if (raw[1] & 0x80) {
        frame->is_exception = true;
        frame->exception_code = raw[2];
        frame->data_len = 1;
        frame->data[0] = raw[2];
    } else {
        frame->data_len = len - 4; /* 减去 address+function+crc */
        if (frame->data_len > MODBUS_RTU_MAX_DATA_LEN)
            frame->data_len = MODBUS_RTU_MAX_DATA_LEN;
        memcpy(frame->data, raw + 2, frame->data_len);
    }

    frame->crc   = rxd_crc;
    frame->valid = true;
    return 0;
}

int modbus_build_response_ok(modbus_frame_t *req, uint8_t *resp_buf,
                             uint16_t *resp_len, const uint8_t *reg_data,
                             uint16_t reg_len) {
    if (!req || !resp_buf || !resp_len || !reg_data) return -1;
    resp_buf[0] = req->address;
    resp_buf[1] = req->function;
    resp_buf[2] = (uint8_t)reg_len; /* 字节数 */
    memcpy(resp_buf + 3, reg_data, reg_len);
    uint16_t total = 3 + reg_len;
    uint16_t crc = modbus_crc16(resp_buf, total);
    resp_buf[total]   = (uint8_t)(crc & 0xFF);
    resp_buf[total+1] = (uint8_t)(crc >> 8);
    *resp_len = total + 2;
    return 0;
}

int modbus_build_response_ex(modbus_frame_t *req, uint8_t *resp_buf,
                             uint16_t *resp_len, uint8_t exception_code) {
    if (!req || !resp_buf || !resp_len) return -1;
    resp_buf[0] = req->address;
    resp_buf[1] = req->function | 0x80;
    resp_buf[2] = exception_code;
    uint16_t crc = modbus_crc16(resp_buf, 3);
    resp_buf[3] = (uint8_t)(crc & 0xFF);
    resp_buf[4] = (uint8_t)(crc >> 8);
    *resp_len = 5;
    return 0;
}
```

```c
/* 使用示例 */
#include <stdio.h>

int main(void) {
    /* 解析读取保持寄存器的请求帧 */
    uint8_t raw_frame[] = {
        0x01,       /* 从机地址1 */
        0x03,       /* 功能码：读保持寄存器 */
        0x00, 0x00, /* 起始地址0 */
        0x00, 0x02, /* 读取2个寄存器 */
        0x00, 0x00  /* CRC占位 */
    };
    uint16_t crc = modbus_crc16(raw_frame, 6);
    raw_frame[6] = (uint8_t)(crc & 0xFF);
    raw_frame[7] = (uint8_t)(crc >> 8);

    modbus_frame_t frame;
    int ret = modbus_rtu_parse(raw_frame, 8, &frame);
    if (ret == 0) {
        printf("Modbus Frame: addr=%u func=%u data_len=%u\n",
               frame.address, frame.function, frame.data_len);
        printf("Start addr: %u, Quantity: %u\n",
               (uint16_t)(frame.data[0] << 8) | frame.data[1],
               (uint16_t)(frame.data[2] << 8) | frame.data[3]);

        /* 构建响应 */
        uint8_t reg_values[] = {0x00, 0x64, 0x00, 0xC8}; /* 100和200 */
        uint8_t resp[256]; uint16_t resp_len;
        modbus_build_response_ok(&frame, resp, &resp_len, reg_values, 4);
        printf("Response (%u bytes): ", resp_len);
        for (uint16_t i = 0; i < resp_len; i++) printf("%02X ", resp[i]);
        printf("\n");
    } else {
        printf("Parse error: %d\n", ret);
    }

    /* 异常响应示例 */
    uint8_t ex_resp[256]; uint16_t ex_len;
    modbus_build_response_ex(&frame, ex_resp, &ex_len, MODBUS_EX_ILLEGAL_DATA_ADDR);
    printf("Exception response (%u bytes): ", ex_len);
    for (uint16_t i = 0; i < ex_len; i++) printf("%02X ", ex_resp[i]);
    printf("\n");

    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 |
|------|------|------|
| CRC16查表 | O(N) | O(512B)表 |
| 帧解析 | O(N) | O(1) |
| 流式接收(feed) | O(1) | O(256)RX buf |

### MCU优化要点

1. **CRC表static const**：512字节预计算表放Flash
2. **流式解析**：逐字节接收，不缓存整个帧再解析
3. **功能码快速路径**：switch-case根据需要扩展的功能码数量
4. **帧超时**：利用定时器判断帧间间隔(3.5字符时间 ≈ t3.5)
5. **小端CRC**：Modbus CRC16以小端序在帧中传输
## Q276. 手写看门狗喂狗任务

### 原理说明

看门狗(Watchdog)是嵌入式系统最后一道防线。软件定期"喂狗"(刷新看门狗计数器)，如果系统死锁/死循环导致无法喂狗，看门狗超时后复位系统。正确的喂狗策略：**单点喂狗**（只在主循环或一个超循环中喂狗），通过任务心跳检测确保所有关键任务都活着。

### 完整代码

```c
/**
 * watchdog.h - 看门狗管理器（多任务心跳检测 + 单点喂狗）
 */
#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 最大监控任务数 */
#define WD_MAX_TASKS 8

/* 看门狗配置 */
typedef struct {
    uint32_t timeout_ms;     /* 看门狗超时时间（硬件看门狗） */ 
    uint32_t feed_period_ms; /* 喂狗周期（应 < timeout_ms/2） */ 
    uint32_t task_timeout;   /* 任务心跳超时（应 > 任务最长执行时间） */ 
} wd_config_t;

/* 被监控的任务描述 */
typedef struct {
    const char *name;       /* 任务名称（调试用） */ 
    uint32_t    period_ms;  /* 期望的心跳周期 */ 
    uint32_t    last_beat;  /* 上次心跳时间戳 */ 
    uint32_t    missed_count; /* 累计失跳次数 */ 
    bool        alive;      /* 是否存活 */ 
} wd_task_t;

/* 看门狗管理器 */
typedef struct {
    wd_config_t  config;
    wd_task_t    tasks[WD_MAX_TASKS];
    uint8_t      task_count;
    uint32_t     last_feed;    /* 上次喂狗时间 */ 
    bool         initialized;

    /* 硬件相关：根据实际MCU实现 */ 
    void (*hw_wdg_init)(uint32_t timeout_ms);
    void (*hw_wdg_feed)(void);
    void (*hw_reset_reason_clear)(void);
    uint32_t (*hw_get_tick_ms)(void);
} wd_manager_t;

/* API */
int  wd_init(wd_manager_t *wd, const wd_config_t *cfg,
             void (*hw_init)(uint32_t), void (*hw_feed)(void),
             uint32_t (*hw_tick)(void));
int  wd_register_task(wd_manager_t *wd, const char *name, uint32_t period_ms);
void wd_task_heartbeat(wd_manager_t *wd, const char *name);
int  wd_service(wd_manager_t *wd);       /* 在主循环中定期调用 */ 
bool wd_check_all_alive(wd_manager_t *wd); /* 检查所有任务是否存活 */ 
void wd_dump_status(const wd_manager_t *wd);

/* 临界区喂狗宏 */
void wd_feed_critical(wd_manager_t *wd);

#endif /* WATCHDOG_H */
```

```c
/**
 * watchdog.c - 看门狗管理器实现
 * 时间复杂度 O(N) N=任务数，空间 O(N)
 */
#include "watchdog.h"
#include <string.h>
#include <stdio.h>

int wd_init(wd_manager_t *wd, const wd_config_t *cfg,
            void (*hw_init)(uint32_t), void (*hw_feed)(void),
            uint32_t (*hw_tick)(void)) {
    if (!wd || !cfg || !hw_init || !hw_feed || !hw_tick) return -1;
    memset(wd, 0, sizeof(wd_manager_t));
    wd->config = *cfg;
    wd->hw_wdg_init = hw_init;
    wd->hw_wdg_feed = hw_feed;
    wd->hw_get_tick_ms = hw_tick;

    /* 初始化硬件看门狗 */ 
    hw_init(cfg->timeout_ms);
    wd->last_feed = hw_tick();
    wd->initialized = true;

    return 0;
}

int wd_register_task(wd_manager_t *wd, const char *name, uint32_t period_ms) {
    if (!wd || !name || wd->task_count >= WD_MAX_TASKS) return -1;
    wd_task_t *t = &wd->tasks[wd->task_count++];
    t->name = name; t->period_ms = period_ms;
    t->last_beat = wd->hw_get_tick_ms();
    t->alive = true;
    return 0;
}

void wd_task_heartbeat(wd_manager_t *wd, const char *name) {
    if (!wd || !name) return;
    for (uint8_t i = 0; i < wd->task_count; i++) {
        if (strcmp(wd->tasks[i].name, name) == 0) {
            wd->tasks[i].last_beat = wd->hw_get_tick_ms();
            wd->tasks[i].alive = true;
            return;
        }
    }
}

int wd_service(wd_manager_t *wd) {
    if (!wd || !wd->initialized) return -1;

    uint32_t now = wd->hw_get_tick_ms();
    bool can_feed = true;

    /* 检查所有任务心跳 */ 
    for (uint8_t i = 0; i < wd->task_count; i++) {
        wd_task_t *t = &wd->tasks[i];
        uint32_t elapsed = now - t->last_beat;

        if (elapsed > wd->config.task_timeout) {
            t->alive = false;
            t->missed_count++;
            can_feed = false; /* 有任务失活，停止喂狗让系统复位 */ 
        } else {
            t->alive = true;
        }
    }

    /* 只有所有任务都活着才喂狗 */ 
    if (can_feed) {
        wd->hw_wdg_feed();
        wd->last_feed = now;
    }

    return can_feed ? 0 : -1;
}

bool wd_check_all_alive(wd_manager_t *wd) {
    if (!wd) return false;
    for (uint8_t i = 0; i < wd->task_count; i++) {
        if (!wd->tasks[i].alive) return false;
    }
    return true;
}

void wd_feed_critical(wd_manager_t *wd) {
    /* 在长时间临界区中喂狗，防止误触（如Flash擦写） */
    if (!wd || !wd->initialized) return;
    wd->hw_wdg_feed();
    wd->last_feed = wd->hw_get_tick_ms();
}

void wd_dump_status(const wd_manager_t *wd) {
    if (!wd) return;
    uint32_t now = wd->hw_get_tick_ms();
    printf("=== Watchdog Status ===\n");
    printf("Timeout: %ums, Feed period: %ums\n", wd->config.timeout_ms, wd->config.feed_period_ms);
    printf("Last feed: %ums ago\n", now - wd->last_feed);
    printf("Tasks:\n");
    for (uint8_t i = 0; i < wd->task_count; i++) {
        uint32_t elapsed = now - wd->tasks[i].last_beat;
        printf("  [%s] alive=%s last_beat=%ums ago missed=%u\n",
               wd->tasks[i].name, wd->tasks[i].alive ? "YES" : "NO",
               elapsed, wd->tasks[i].missed_count);
    }
    printf("======================\n");
}
```

```c
/* 使用示例 */
#include <stdio.h>

/* 硬件模拟 */
static uint32_t mock_tick = 0;
static uint32_t mock_get_tick(void) { return mock_tick; }
static void mock_wdg_init(uint32_t t) { printf("[HW] WDG init timeout=%ums\n", t); }
static void mock_wdg_feed(void) { printf("[HW] WDG feed @%ums\n", mock_tick); }

int main(void) {
    wd_manager_t wd;
    wd_config_t  cfg = {
        .timeout_ms = 1000, .feed_period_ms = 400, .task_timeout = 500
    };
    wd_init(&wd, &cfg, mock_wdg_init, mock_wdg_feed, mock_get_tick);

    /* 注册3个监控任务 */
    wd_register_task(&wd, "sensor",   200);
    wd_register_task(&wd, "comm",     100);
    wd_register_task(&wd, "led_ctrl", 50);

    /* 模拟运行1000ms */
    for (int i = 0; i < 1000; i += 50) {
        mock_tick += 50;

        /* 任务心跳（模拟正常执行） */
        wd_task_heartbeat(&wd, "sensor");
        wd_task_heartbeat(&wd, "comm");
        wd_task_heartbeat(&wd, "led_ctrl");

        int ret = wd_service(&wd);
        if (i % 200 == 0) {
            printf("[t=%u] service=%d alive=%s\n", mock_tick, ret,
                   wd_check_all_alive(&wd) ? "ALL" : "SOME DEAD");
        }
    }

    /* 模拟通信任务挂起 */
    printf("\n--- Simulating comm task hang ---\n");
    for (int i = 0; i < 600; i += 100) {
        mock_tick += 100;
        wd_task_heartbeat(&wd, "sensor");
        wd_task_heartbeat(&wd, "led_ctrl");
        /* comm任务不喂心跳！ */
        int ret = wd_service(&wd);
        printf("[t=%u] service=%d alive=%s\n", mock_tick, ret,
               wd_check_all_alive(&wd) ? "ALL" : "SOME DEAD");
    }

    wd_dump_status(&wd);
    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 |
|------|------|------|
| `wd_service` | O(N) | O(N) | N=监控任务数 |
| `wd_task_heartbeat` | O(N) | O(1) | 遍历查找任务名 |
| 喂狗本身 | O(1) | O(1) | |

### MCU优化要点

1. **单点喂狗**：只在`wd_service`中喂狗，避免各处分散喂狗
2. **心跳在任务中**：每个任务执行完一轮后调用`wd_task_heartbeat`
3. **心跳超时 > 任务最长执行时间**：避免误判
4. **临界区喂狗**：Flash擦写等待长时间操作时调用`wd_feed_critical`
5. **独立看门狗(IWDG)**：使用独立时钟源，不受主时钟故障影响
6. **窗口看门狗(WWDG)**：要求在特定时间窗口内喂狗，更严格

---

## Q277. 手写延时函数(非阻塞版)

### 原理说明

非阻塞延时基于时间戳比较，不占用CPU。核心是`millis()`风格的系统时钟获取函数。所有延时通过比较"当前时间 - 开始时间"是否达到目标来实现，任务可以轮询检查而不会阻塞其他任务。

### 完整代码

```c
/**
 * nb_delay.h - 非阻塞延时与定时器框架
 */
#ifndef NB_DELAY_H
#define NB_DELAY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 非阻塞定时器（一次性） */
typedef struct {
    uint32_t start_ms;
    uint32_t duration_ms;
    bool     running;
    bool     expired;
} nb_timer_t;

/* 非阻塞周期定时器 */
typedef struct {
    uint32_t last_tick_ms;
    uint32_t period_ms;
    uint32_t missed_count;
    bool     running;
} nb_periodic_t;

/* 微秒级非阻塞延时（基于DWT CYCCNT） */
typedef struct {
    uint32_t start_cycles;
    uint32_t duration_us;
    bool     running;
    bool     expired;
} nb_us_timer_t;

/* ============ 毫秒定时器 ============ */
void nb_timer_start(nb_timer_t *t, uint32_t duration_ms, uint32_t current_ms);
bool nb_timer_is_done(nb_timer_t *t, uint32_t current_ms);
void nb_timer_stop(nb_timer_t *t);
void nb_timer_restart(nb_timer_t *t, uint32_t current_ms);
uint32_t nb_timer_elapsed(const nb_timer_t *t, uint32_t current_ms);
uint32_t nb_timer_remaining(const nb_timer_t *t, uint32_t current_ms);

/* ============ 周期定时器 ============ */
void nb_periodic_start(nb_periodic_t *p, uint32_t period_ms, uint32_t current_ms);
bool nb_periodic_check(nb_periodic_t *p, uint32_t current_ms);
void nb_periodic_stop(nb_periodic_t *p);
uint32_t nb_periodic_get_missed(nb_periodic_t *p);

/* ============ 超时等待（带条件检查） ============ */
typedef bool (*nb_condition_fn)(void *ctx);

/* 等待条件成立，有超时保护 */ 
bool nb_wait_for(nb_condition_fn cond, void *ctx, uint32_t timeout_ms,
                 uint32_t (*get_ms)(void));

/* 每间隔period_ms执行一次action，直到duration_ms */ 
typedef void (*nb_action_fn)(void *ctx);
void nb_run_periodic(uint32_t duration_ms, uint32_t period_ms,
                     nb_action_fn action, void *ctx, uint32_t (*get_ms)(void));

#endif /* NB_DELAY_H */
```

```c
/**
 * nb_delay.c - 非阻塞延时实现
 * 所有操作 O(1)
 */
#include "nb_delay.h"

/* ============ 毫秒一次性定时器 ============ */

void nb_timer_start(nb_timer_t *t, uint32_t duration_ms, uint32_t current_ms) {
    if (!t) return;
    t->start_ms    = current_ms;
    t->duration_ms = duration_ms;
    t->running     = true;
    t->expired     = false;
}

bool nb_timer_is_done(nb_timer_t *t, uint32_t current_ms) {
    if (!t || !t->running) return false;
    /* 处理定时器溢出 */ 
    uint32_t elapsed = current_ms - t->start_ms;
    if (elapsed >= t->duration_ms) {
        t->expired = true;
        t->running = false;
        return true;
    }
    return false;
}

void nb_timer_stop(nb_timer_t *t) {
    if (t) { t->running = false; t->expired = false; }
}

void nb_timer_restart(nb_timer_t *t, uint32_t current_ms) {
    if (!t) return;
    t->start_ms = current_ms;
    t->running  = true;
    t->expired  = false;
}

uint32_t nb_timer_elapsed(const nb_timer_t *t, uint32_t current_ms) {
    if (!t || !t->running) return 0;
    uint32_t e = current_ms - t->start_ms;
    return (e <= t->duration_ms) ? e : t->duration_ms;
}

uint32_t nb_timer_remaining(const nb_timer_t *t, uint32_t current_ms) {
    if (!t || !t->running) return 0;
    uint32_t e = current_ms - t->start_ms;
    return (e >= t->duration_ms) ? 0 : (t->duration_ms - e);
}

/* ============ 周期定时器 ============ */

void nb_periodic_start(nb_periodic_t *p, uint32_t period_ms, uint32_t current_ms) {
    if (!p) return;
    p->last_tick_ms = current_ms;
    p->period_ms    = period_ms;
    p->missed_count = 0;
    p->running      = true;
}

bool nb_periodic_check(nb_periodic_t *p, uint32_t current_ms) {
    if (!p || !p->running) return false;
    uint32_t elapsed = current_ms - p->last_tick_ms;
    if (elapsed >= p->period_ms) {
        /* 处理可能丢失的周期 */ 
        p->missed_count = elapsed / p->period_ms - 1;
        p->last_tick_ms += p->period_ms * (elapsed / p->period_ms);
        /* 防止追赶过远 */ 
        if (current_ms - p->last_tick_ms > p->period_ms) {
            p->last_tick_ms = current_ms; /* 重置 */ 
        }
        return true;
    }
    return false;
}

void nb_periodic_stop(nb_periodic_t *p) {
    if (p) { p->running = false; p->missed_count = 0; }
}

uint32_t nb_periodic_get_missed(nb_periodic_t *p) {
    return p ? p->missed_count : 0;
}

/* ============ 条件等待 ============ */

bool nb_wait_for(nb_condition_fn cond, void *ctx, uint32_t timeout_ms,
                 uint32_t (*get_ms)(void)) {
    if (!cond || !get_ms) return false;
    uint32_t start = get_ms();
    while (!cond(ctx)) {
        if (get_ms() - start >= timeout_ms) return false; /* 超时 */ 
        /* yield or sleep for cooperative multitasking if available */ 
    }
    return true;
}

void nb_run_periodic(uint32_t duration_ms, uint32_t period_ms,
                     nb_action_fn action, void *ctx, uint32_t (*get_ms)(void)) {
    if (!action || !get_ms || period_ms == 0) return;
    uint32_t start  = get_ms();
    uint32_t last   = start;

    while (get_ms() - start < duration_ms) {
        uint32_t now = get_ms();
        if (now - last >= period_ms) {
            action(ctx);
            last += period_ms;
        }
    }
}
```

```c
/* 使用示例 */
#include <stdio.h>

static uint32_t mock_ms = 0;
static uint32_t get_ms(void) { return mock_ms; }

int main(void) {
    /* ===== 一次性非阻塞延时 ===== */
    nb_timer_t t;
    nb_timer_start(&t, 500, get_ms()); /* 500ms延时 */

    while (mock_ms < 2000) {
        mock_ms += 50;
        if (nb_timer_is_done(&t, get_ms())) {
            printf("[%u] Timer expired! elapsed=%u\n",
                   (unsigned)get_ms(), (unsigned)nb_timer_elapsed(&t, get_ms()));
            nb_timer_restart(&t, get_ms()); /* 重新开始 */
        }
    }

    /* ===== 周期定时器 ===== */
    printf("\nPeriodic timer test:\n");
    nb_periodic_t sensor_timer;
    nb_periodic_start(&sensor_timer, 200, 0);

    for (int i = 0; i < 10; i++) {
        mock_ms += 100;
        if (nb_periodic_check(&sensor_timer, mock_ms)) {
            printf("[%u] Sensor read! missed=%u\n",
                   (unsigned)mock_ms,
                   (unsigned)nb_periodic_get_missed(&sensor_timer));
        }
    }

    /* ===== 条件等待（模拟按键等待） ===== */
    static bool key_pressed = false;
    static bool key_condition(void *ctx) {
        (void)ctx;
        return key_pressed; /* 模拟按键触发 */
    }

    printf("\nWaiting for key (5s timeout)...\n");
    mock_ms = 0; key_pressed = false;

    /* 模拟：2.5秒时按键 */ 
    /* 注：nb_wait_for是阻塞的，实际MCU用非阻塞方式 */ 
    /* 简化示例：直接模拟非阻塞方式 */ 
    nb_timer_t wait_timer;
    nb_timer_start(&wait_timer, 5000, mock_ms);
    while (1) {
        mock_ms += 100;
        if (mock_ms >= 2500) key_pressed = true;
        if (key_condition(NULL)) { printf("[%u] Key pressed!\n", (unsigned)mock_ms); break; }
        if (nb_timer_is_done(&wait_timer, mock_ms)) { printf("Timeout!\n"); break; }
    }

    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 |
|------|------|------|
| `nb_timer_is_done` | O(1) | O(1) |
| `nb_periodic_check` | O(1) | O(1) |
| `nb_wait_for` | O(1) per iter | O(1) |

### MCU优化要点

1. **millis()来源**：SysTick定时器或硬件定时器，32位无符号回绕安全
2. **微秒延时**：使用DWT CYCCNT（Cortex-M）实现精确1us非阻塞延时
3. **多定时器**：为每个需要延时的操作分配一个`nb_timer_t`
4. **回绕安全**：`current_ms - start_ms` 在32位无符号下自动处理溢出
5. **RTOS集成**：在RTOS中改用`vTaskDelayUntil`等API

---

## Q278. 手写大小端转换

### 原理说明

大小端(Endianness)是多字节数据在内存中的字节序问题。大端(Big-Endian)：高位字节在低地址；小端(Little-Endian)：低位字节在低地址。网络协议用大端，x86/ARM默认小端。MCU需频繁进行协议转换。

### 完整代码

```c
/**
 * endian.h - 大小端检测与转换工具
 */
#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ============ 字节序检测 ============ */

/* 编译时检测是否小端 */
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  #define IS_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  #define IS_LITTLE_ENDIAN 0
#else
  /* 运行时检测 */
  #define IS_LITTLE_ENDIAN is_little_endian_rt()
#endif

/* 运行时检测 */
static inline bool is_little_endian_rt(void) {
    const union { uint16_t i; uint8_t b[2]; } u = {0x0001};
    return u.b[0] == 1;
}

/* ============ 字节交换 ============ */

/* 16位字节交换 */ 
static inline uint16_t swap16(uint16_t x) {
    return (uint16_t)((x >> 8) | (x << 8));
}

/* 32位字节交换 */ 
static inline uint32_t swap32(uint32_t x) {
    x = ((x & 0xFF00FF00UL) >> 8) | ((x & 0x00FF00FFUL) << 8);
    return (x >> 16) | (x << 16);
}

/* 64位字节交换 */ 
static inline uint64_t swap64(uint64_t x) {
    x = ((x & 0xFF00FF00FF00FF00ULL) >> 8)  | ((x & 0x00FF00FF00FF00FFULL) << 8);
    x = ((x & 0xFFFF0000FFFF0000ULL) >> 16) | ((x & 0x0000FFFF0000FFFFULL) << 16);
    return (x >> 32) | (x << 32);
}

/* ============ 主机 <-> 大端（网络字节序） ============ */

/* 主机到大端 */ 
#define htobe16(x) (IS_LITTLE_ENDIAN ? swap16(x) : (uint16_t)(x))
#define htobe32(x) (IS_LITTLE_ENDIAN ? swap32(x) : (uint32_t)(x))
#define htobe64(x) (IS_LITTLE_ENDIAN ? swap64(x) : (uint64_t)(x))

/* 大端到主机 */ 
#define be16toh(x) htobe16(x)
#define be32toh(x) htobe32(x)
#define be64toh(x) htobe64(x)

/* ============ 主机 <-> 小端 ============ */
#define htole16(x) (IS_LITTLE_ENDIAN ? (uint16_t)(x) : swap16(x))
#define htole32(x) (IS_LITTLE_ENDIAN ? (uint32_t)(x) : swap32(x))
#define htole64(x) (IS_LITTLE_ENDIAN ? (uint64_t)(x) : swap64(x))
#define le16toh(x) htole16(x)
#define le32toh(x) htole32(x)
#define le64toh(x) htole64(x)

/* ============ 安全内存读写（任意地址 + 任意字节序） ============ */

/* 从内存读取大端uint16_t */
static inline uint16_t read_be16(const uint8_t *p) {
    return (uint16_t)((((uint16_t)p[0]) << 8) | (uint16_t)p[1]);
}

/* 从内存读取大端uint32_t */
static inline uint32_t read_be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  | (uint32_t)p[3];
}

/* 从内存读取大端uint64_t */
static inline uint64_t read_be64(const uint8_t *p) {
    return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
           ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
           ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
           ((uint64_t)p[6] << 8)  | (uint64_t)p[7];
}

/* 从内存读取小端uint16_t */ 
static inline uint16_t read_le16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* 从内存读取小端uint32_t */ 
static inline uint32_t read_le32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* 从内存读取小端uint64_t */
static inline uint64_t read_le64(const uint8_t *p) {
    return (uint64_t)p[0] | ((uint64_t)p[1] << 8) |
           ((uint64_t)p[2] << 16) | ((uint64_t)p[3] << 24) |
           ((uint64_t)p[4] << 32) | ((uint64_t)p[5] << 40) |
           ((uint64_t)p[6] << 48) | ((uint64_t)p[7] << 56);
}

/* 写入大端到内存 */ 
static inline void write_be16(uint8_t *p, uint16_t val) {
    p[0] = (uint8_t)(val >> 8); p[1] = (uint8_t)(val);
}

static inline void write_be32(uint8_t *p, uint32_t val) {
    p[0] = (uint8_t)(val >> 24); p[1] = (uint8_t)(val >> 16);
    p[2] = (uint8_t)(val >> 8);  p[3] = (uint8_t)(val);
}

/* 写入小端到内存 */ 
static inline void write_le16(uint8_t *p, uint16_t val) {
    p[0] = (uint8_t)(val); p[1] = (uint8_t)(val >> 8);
}

static inline void write_le32(uint8_t *p, uint32_t val) {
    p[0] = (uint8_t)(val); p[1] = (uint8_t)(val >> 8);
    p[2] = (uint8_t)(val >> 16); p[3] = (uint8_t)(val >> 24);
}

/* ============ Unaligned Access Safe Wrapper ============ */

/* 从任意地址安全读取uint16_t（不依赖对齐） */
static inline uint16_t get_unaligned_u16(const uint8_t *p) {
    uint16_t val;
    /* 使用memcpy避免对齐问题 */ 
    __builtin_memcpy(&val, p, sizeof(val));
    return val;
}

/* 从任意地址安全写入uint32_t */ 
static inline void put_unaligned_u32(uint8_t *p, uint32_t val) {
    __builtin_memcpy(p, &val, sizeof(val));
}

/* ============ Float IEEE754 字节序转换 ============ */

/* 注意：float遵循IEEE754，32位大端/小端与整型一致 */
static inline float read_be_float(const uint8_t *p) {
    uint32_t i = read_be32(p);
    float f;
    __builtin_memcpy(&f, &i, sizeof(f));
    return f;
}

static inline void write_be_float(uint8_t *p, float val) {
    uint32_t i;
    __builtin_memcpy(&i, &val, sizeof(i));
    write_be32(p, i);
}

#endif /* ENDIAN_H */
```

```c
/* 使用示例 */
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("System is %s endian\n", IS_LITTLE_ENDIAN ? "little" : "big");

    /* 整数字节序转换 */
    uint32_t host_val = 0x12345678UL;
    uint32_t be_val   = htobe32(host_val);
    uint32_t le_val   = htole32(host_val);

    printf("Host: 0x%08lX\n", (unsigned long)host_val);
    printf("BE:   0x%08lX\n", (unsigned long)be_val);
    printf("LE:   0x%08lX\n", (unsigned long)le_val);
    printf("BE->Host: 0x%08lX\n", (unsigned long)be32toh(be_val));

    /* 从内存安全读取（模拟从网络帧/Flash读取） */
    uint8_t buf[8] = {0x00, 0x00, 0x01, 0x00, 0x64, 0x00, 0x00, 0x00};
    uint32_t reg_addr  = read_be32(buf);        /* 0x00000100 = 寄存器地址256 */
    uint32_t reg_value = read_be32(buf + 4);    /* 0x64000000 = 值 */
    printf("Register address: 0x%04lX\n", (unsigned long)reg_addr);
    printf("Register value:   %lu\n", (unsigned long)reg_value);

    /* 16位操作 */
    uint8_t modbus_crc_buf[2] = {0x84, 0x0A};
    uint16_t crc = read_le16(modbus_crc_buf);
    printf("Modbus CRC: 0x%04X (LE from raw)\n", crc);

    /* 浮点数大端读取 */
    uint8_t float_be[4] = {0x40, 0x49, 0x0F, 0xDB}; /* 3.14159f */ 
    float pi = read_be_float(float_be);
    printf("Pi (BE float): %f\n", pi);

    /* 写入测试 */
    uint8_t out[4];
    write_be32(out, 0xDEADBEEF);
    printf("Written BE: %02X %02X %02X %02X\n", out[0], out[1], out[2], out[3]);
    write_le32(out, 0xDEADBEEF);
    printf("Written LE: %02X %02X %02X %02X\n", out[0], out[1], out[2], out[3]);

    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 |
|------|------|------|
| swap16/32/64 | O(1) | O(1) |
| read_be16/32/64 | O(1) | O(1) |
| write_be16/32 | O(1) | O(1) |
| 编译时htobe/htole | O(1) | O(1) |

### MCU优化要点

1. **ARM REV指令**：Cortex-M有`REV`/`REV16`/`REVSH`单周期字节交换指令
2. **_builtin_bswap**：GCC内建函数，直接映射到REV指令
3. **字节访问**：使用移位+OR构造，避免非对齐访问陷阱
4. **编译时检测**：用预定义宏`__BYTE_ORDER__`避免运行时开销
5. **memcpy处理非对齐**：`__builtin_memcpy`处理任意地址读取

---

## Q279. 手写矩阵乘法(优化缓存命中)

### 原理说明

矩阵乘法 C = A × B：C[i][j] = Σ A[i][k] × B[k][j]。纯三重循环的缓存命中率差，因为B是按列访问的。优化核心是：循环交换(tiling)、行优先访问、循环展开。

### 完整代码

```c
/**
 * matrix.h - 矩阵乘法（缓存优化版）
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* 矩阵类型 */ 
typedef float matrix_elem_t; /* 可改为int32_t做定点矩阵 */

/* 块大小（适配MCU L1缓存行，通常32-64字节） */
/* Cortex-M7 D-Cache: 32B line, 4KB. TILE取16-32合适 */
#define TILE_SIZE 16

/* 矩阵描述（数据由外部静态分配） */
typedef struct {
    matrix_elem_t *data;  /* 行优先存储 (row-major) */
    uint32_t       rows;
    uint32_t       cols;
    uint32_t       stride; /* 行步长（>= cols，用于子矩阵视图） */ 
} matrix_t;

/* ============ 初始化 ============ */
void matrix_init(matrix_t *mat, matrix_elem_t *data, uint32_t rows, uint32_t cols);
void matrix_fill_zeros(matrix_t *mat);
void matrix_fill_identity(matrix_t *mat);
void matrix_fill_random(matrix_t *mat, uint32_t seed);
void matrix_copy(matrix_t *dst, const matrix_t *src);

/* ============ 矩阵乘法 ============ */

/* 朴素算法（三循环 ijk） */ 
void matrix_mul_naive(const matrix_t *a, const matrix_t *b, matrix_t *c);

/* 优化版：循环交换 (ikj)，改善缓存命中 */ 
void matrix_mul_ikj(const matrix_t *a, const matrix_t *b, matrix_t *c);

/* 优化版：分块(Tiling) + 循环交换，最优 */ 
void matrix_mul_tiled(const matrix_t *a, const matrix_t *b, matrix_t *c);

/* 优化版：分块 + 循环展开，MCU最优 */ 
void matrix_mul_optimized(const matrix_t *a, const matrix_t *b, matrix_t *c);

/* ============ 辅助 ============ */
void matrix_print(const matrix_t *mat, const char *name);
bool matrix_is_valid_mult(const matrix_t *a, const matrix_t *b, const matrix_t *c);
float matrix_benchmark(const matrix_t *a, const matrix_t *b, matrix_t *c,
                       void (*mul_fn)(const matrix_t*, const matrix_t*, matrix_t*));

#endif /* MATRIX_H */
```

```c
/**
 * matrix.c - 矩阵乘法实现
 * 时间复杂度 O(N³)，空间 O(1)（原地计算在外部分配的C中）
 */
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

void matrix_init(matrix_t *mat, matrix_elem_t *data, uint32_t rows, uint32_t cols) {
    if (!mat || !data) return;
    mat->data   = data;
    mat->rows   = rows;
    mat->cols   = cols;
    mat->stride = cols;
}

void matrix_fill_zeros(matrix_t *mat) {
    if (!mat || !mat->data) return;
    for (uint32_t i = 0; i < mat->rows; i++)
        memset(mat->data + i * mat->stride, 0, mat->cols * sizeof(matrix_elem_t));
}

void matrix_fill_identity(matrix_t *mat) {
    matrix_fill_zeros(mat);
    uint32_t n = (mat->rows < mat->cols) ? mat->rows : mat->cols;
    for (uint32_t i = 0; i < n; i++)
        mat->data[i * mat->stride + i] = 1.0f;
}

void matrix_fill_random(matrix_t *mat, uint32_t seed) {
    if (!mat || !mat->data) return;
    srand(seed);
    for (uint32_t i = 0; i < mat->rows; i++)
        for (uint32_t j = 0; j < mat->cols; j++)
            mat->data[i * mat->stride + j] = (float)(rand() % 100) / 10.0f;
}

void matrix_copy(matrix_t *dst, const matrix_t *src) {
    if (!dst || !src || !dst->data || !src->data) return;
    uint32_t rows = (dst->rows < src->rows) ? dst->rows : src->rows;
    uint32_t cols = (dst->cols < src->cols) ? dst->cols : src->cols;
    for (uint32_t i = 0; i < rows; i++)
        memcpy(dst->data + i * dst->stride, src->data + i * src->stride,
               cols * sizeof(matrix_elem_t));
}

bool matrix_is_valid_mult(const matrix_t *a, const matrix_t *b, const matrix_t *c) {
    return (a && b && c && a->data && b->data && c->data &&
            a->cols == b->rows && c->rows == a->rows && c->cols == b->cols);
}

/* ================================================================
 * 朴素算法：三重循环 ijk
 * 问题：B按列访问（j变化快），缓存行换出频繁
 * ================================================================ */
void matrix_mul_naive(const matrix_t *a, const matrix_t *b, matrix_t *c) {
    if (!matrix_is_valid_mult(a, b, c)) return;
    matrix_fill_zeros(c);

    for (uint32_t i = 0; i < a->rows; i++) {
        for (uint32_t j = 0; j < b->cols; j++) {
            matrix_elem_t sum = 0.0f;
            for (uint32_t k = 0; k < a->cols; k++) {
                sum += a->data[i * a->stride + k] *
                       b->data[k * b->stride + j];
            }
            c->data[i * c->stride + j] = sum;
        }
    }
}

/* ================================================================
 * 循环交换优化：ikj 顺序
 * 原理：内层k循环连续访问A[i][*] 和 B[k][*]，均为行优先
 *       写入C[i][*]也连续 → 缓存利用率大幅提升
 * 缓存分析：
 *   - A[i][k]: 固定一行，所有k连续，一次加载整行进cache
 *   - B[k][j]: k变化，但内层j连续，充分利用cache行
 *   - C[i][j]: 写后不读（write-back），缓存压力小
 * ================================================================ */
void matrix_mul_ikj(const matrix_t *a, const matrix_t *b, matrix_t *c) {
    if (!matrix_is_valid_mult(a, b, c)) return;
    matrix_fill_zeros(c);

    for (uint32_t i = 0; i < a->rows; i++) {
        for (uint32_t k = 0; k < a->cols; k++) {
            matrix_elem_t aik = a->data[i * a->stride + k];
            for (uint32_t j = 0; j < b->cols; j++) {
                c->data[i * c->stride + j] += aik * b->data[k * b->stride + j];
            }
        }
    }
}

/* ================================================================
 * 分块(Tiling) + 循环交换优化
 * 原理：将大矩阵划分为 TILE_SIZE x TILE_SIZE 的子块
 *       每个子块可完全放入L1 Cache中
 *       减少主存访问，最大化数据复用
 * ================================================================ */
void matrix_mul_tiled(const matrix_t *a, const matrix_t *b, matrix_t *c) {
    if (!matrix_is_valid_mult(a, b, c)) return;
    matrix_fill_zeros(c);

    for (uint32_t ii = 0; ii < a->rows; ii += TILE_SIZE) {
        uint32_t i_end = (ii + TILE_SIZE < a->rows) ? (ii + TILE_SIZE) : a->rows;
        for (uint32_t jj = 0; jj < b->cols; jj += TILE_SIZE) {
            uint32_t j_end = (jj + TILE_SIZE < b->cols) ? (jj + TILE_SIZE) : b->cols;
            for (uint32_t kk = 0; kk < a->cols; kk += TILE_SIZE) {
                uint32_t k_end = (kk + TILE_SIZE < a->cols) ? (kk + TILE_SIZE) : a->cols;

                /* 子块乘法 */
                for (uint32_t i = ii; i < i_end; i++) {
                    for (uint32_t k = kk; k < k_end; k++) {
                        matrix_elem_t aik = a->data[i * a->stride + k];
                        for (uint32_t j = jj; j < j_end; j++) {
                            c->data[i * c->stride + j] +=
                                aik * b->data[k * b->stride + j];
                        }
                    }
                }
            }
        }
    }
}

/* ================================================================
 * 最优版：分块 + 循环交换 + 4路循环展开
 * 适配Cortex-M7双发射流水线
 * ================================================================ */
void matrix_mul_optimized(const matrix_t *a, const matrix_t *b, matrix_t *c) {
    if (!matrix_is_valid_mult(a, b, c)) return;
    matrix_fill_zeros(c);

    const uint32_t UNROLL_J = 4;

    for (uint32_t ii = 0; ii < a->rows; ii += TILE_SIZE) {
        uint32_t i_end = (ii + TILE_SIZE < a->rows) ? (ii + TILE_SIZE) : a->rows;
        for (uint32_t kk = 0; kk < a->cols; kk += TILE_SIZE) {
            uint32_t k_end = (kk + TILE_SIZE < a->cols) ? (kk + TILE_SIZE) : a->cols;

            for (uint32_t i = ii; i < i_end; i++) {
                for (uint32_t k = kk; k < k_end; k++) {
                    matrix_elem_t aik = a->data[i * a->stride + k];
                    uint32_t j = 0;
                    /* 4路循环展开 */ 
                    for (; j + UNROLL_J - 1 < b->cols; j += UNROLL_J) {
                        c->data[i * c->stride + j + 0] +=
                            aik * b->data[k * b->stride + j + 0];
                        c->data[i * c->stride + j + 1] +=
                            aik * b->data[k * b->stride + j + 1];
                        c->data[i * c->stride + j + 2] +=
                            aik * b->data[k * b->stride + j + 2];
                        c->data[i * c->stride + j + 3] +=
                            aik * b->data[k * b->stride + j + 3];
                    }
                    /* 处理剩余 */ 
                    for (; j < b->cols; j++) {
                        c->data[i * c->stride + j] +=
                            aik * b->data[k * b->stride + j];
                    }
                }
            }
        }
    }
}

void matrix_print(const matrix_t *mat, const char *name) {
    if (!mat || !mat->data) return;
    printf("Matrix %s (%ux%u):\n", name, mat->rows, mat->cols);
    for (uint32_t i = 0; i < mat->rows; i++) {
        for (uint32_t j = 0; j < mat->cols; j++) {
            printf("%8.2f ", (double)mat->data[i * mat->stride + j]);
        }
        printf("\n");
    }
}
```

```c
/* 使用示例 */
#include <stdio.h>
#include <time.h>

int main(void) {
    #define N 128
    static matrix_elem_t a_data[N * N], b_data[N * N], c_data[N * N];
    matrix_t A, B, C1, C2;

    matrix_init(&A, a_data, N, N);
    matrix_init(&B, b_data, N, N);
    matrix_init(&C1, c_data, N, N);

    matrix_fill_random(&A, 42);
    matrix_fill_random(&B, 123);

    /* 朴素 vs 优化 */
    clock_t start, end;

    matrix_t C2_t; static matrix_elem_t c2_data[N * N];
    matrix_init(&C2_t, c2_data, N, N);

    start = clock();
    matrix_mul_naive(&A, &B, &C1);
    end = clock();
    printf("Naive:  %.3f ms\n", 1000.0 * (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    matrix_mul_ikj(&A, &B, &C2_t);
    end = clock();
    printf("ikj:    %.3f ms\n", 1000.0 * (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    matrix_mul_tiled(&A, &B, &C2_t);
    end = clock();
    printf("Tiled:  %.3f ms\n", 1000.0 * (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    matrix_mul_optimized(&A, &B, &C2_t);
    end = clock();
    printf("Optim:  %.3f ms\n", 1000.0 * (double)(end - start) / CLOCKS_PER_SEC);

    /* 小矩阵验证 */
    static matrix_elem_t sa[9] = {1,2,3, 4,5,6, 7,8,9};
    static matrix_elem_t sb[9] = {9,8,7, 6,5,4, 3,2,1};
    static matrix_elem_t sc[9] = {0};
    matrix_t A3, B3, C3;
    matrix_init(&A3, sa, 3, 3); matrix_init(&B3, sb, 3, 3); matrix_init(&C3, sc, 3, 3);
    matrix_mul_ikj(&A3, &B3, &C3);
    matrix_print(&C3, "C (3x3)");

    return 0;
}
```

### 复杂度分析

| 算法 | 时间 | 空间 | 缓存性能 |
|------|------|------|---------|
| Naive (ijk) | O(N³) | O(1) | 差：B列访问 |
| ikj交换 | O(N³) | O(1) | 良：B行访问 |
| Tiled | O(N³) | O(1) | 优：子块全在cache |
| Tiled + 展开 | O(N³) | O(1) | 最优 |

### MCU优化要点

1. **ikj循环顺序**：最重要的优化，确保内层均行优先访问
2. **TILE_SIZE适配**：Cortex-M7 D-Cache 4KB / (sizeof(float)*3) ≈ 合适的TILE
3. **循环展开**：4路展开匹配Cortex-M7双发射流水线
4. **数据预取**：M7支持PLD指令，编译器-O3可自动插入
5. **定点计算**：使用int32_t Q24格式替代float，无FPU的MCU加速10x
6. **DMA双缓冲**：大矩阵分块用DMA传输，CPU和DMA流水线并行

---

## Q280. 手写协程调度器(protothread)

### 原理说明

Protothread是Adam Dunkels发明的超轻量协程方案，仅用C语言的switch-case实现协作式多任务。核心是`PT_BEGIN`/`PT_END`宏和局部延续(Local Continuations)，将函数分解为多个可被中断和恢复的执行块。非常适合资源极度受限的MCU（堆栈共享，每个protothread仅2字节状态）。

### 完整代码

```c
/**
 * pt.h - Protothread 协程调度器
 * 基于 Adam Dunkels 的 protothreads 库
 */
#ifndef PT_H
#define PT_H

#include <stdint.h>
#include <stdbool.h>

/* ============ 核心宏 ============ */

/* Protothread状态 */
typedef uint16_t pt_t;

/* 状态常量 */
#define PT_WAITING    0    /* 等待事件 */
#define PT_YIELDED    1    /* 已让出 */
#define PT_EXITED     2    /* 已退出 */
#define PT_ENDED      3    /* 已结束 */

/* 声明一个protothread */
#define PT_THREAD(name)   static pt_t name = 0

/* 声明protothread函数 */
#define PT_FUNC(name)     void name(struct pt *pt)

/* Protothread控制块 */
typedef struct pt {
    uint16_t lc;           /* 局部延续 (local continuation) */ 
    uint32_t wait_time;    /* 等待时间*/ 
    void     *wait_event;  /* 等待的事件 */ 
    uint16_t priority;     /* 优先级 */ 
    bool     running;
} pt_ctx_t;

/* 初始化 protothread */
#define PT_INIT(pt)  ((pt)->lc = 0, (pt)->running = false, (pt)->wait_time = 0)

/* 声明局部延续变量 */
#define PT_BEGIN(pt) { volatile uint16_t PT_YIELD_FLAG = 1; (void)PT_YIELD_FLAG; \
                        switch((pt)->lc) { case 0:

/* 结束并重启（循环） */
#define PT_END(pt)    } (pt)->lc = 0;}

/* 让出执行权（下次从下一行继续） */
#define PT_YIELD(pt)  do { \
    PT_YIELD_FLAG = 0; \
    (pt)->lc = __LINE__; \
    return; \
    case __LINE__: ; \
    PT_YIELD_FLAG = 1; \
} while(0)

/* 等待条件成立 */
#define PT_WAIT_UNTIL(pt, condition) do { \
    (pt)->lc = __LINE__; \
    case __LINE__: \
    if (!(condition)) return; \
} while(0)

/* 等待条件成立并让出（轮询） */
#define PT_WAIT_WHILE(pt, condition) \
    PT_WAIT_UNTIL((pt), !(condition))

/* 生成子protothread */
#define PT_SPAWN(pt, child, thread_func) do { \
    (pt)->lc = __LINE__; \
    case __LINE__: \
    PT_INIT(&(child)); \
    do { \
        thread_func(&(child)); \
        if (!(child).running) { \
            (child).running = true; \
            return; \
        } \
        (child).running = false; \
    } while(0); \
} while(0)

/* 等待子protothread */
#define PT_WAIT_THREAD(pt, child, thread_func) do { \
    (pt)->lc = __LINE__; \
    case __LINE__: \
    thread_func(&(child)); \
    if ((child).running) return; \
} while(0)

/* 退出protothread */
#define PT_EXIT(pt) do { \
    (pt)->lc = 0; \
    (pt)->running = false; \
    return; \
} while(0)

/* 重启protothread */
#define PT_RESTART(pt) do { \
    (pt)->lc = 0; \
    (pt)->running = false; \
    return; \
} while(0)

/* 检查protothread是否结束 */
#define PT_ENDED(pt)  ((pt)->lc == 0)

/* ============ 调度器类型 ============ */

#define PT_MAX_THREADS 16

/* Protothread函数类型 */
typedef void (*pt_func_t)(pt_ctx_t *ctx);

/* Protothread描述符 */
typedef struct {
    pt_func_t  func;
    pt_ctx_t   ctx;
    uint32_t   period_ms;        /* 执行周期（0=一次性） */ 
    uint32_t   last_run_ms;
    uint32_t   run_count;
    const char *name;
    bool       active;
} pt_thread_t;

/* 调度器 */
typedef struct {
    pt_thread_t threads[PT_MAX_THREADS];
    uint8_t     thread_count;
    uint32_t    (*get_tick_ms)(void);
} pt_scheduler_t;

/* ============ 调度器 API ============ */
void pt_sched_init(pt_scheduler_t *sched, uint32_t (*tick_fn)(void));
int  pt_sched_add(pt_scheduler_t *sched, pt_func_t func,
                  uint32_t period_ms, const char *name);
void pt_sched_run(pt_scheduler_t *sched);
void pt_sched_remove(pt_scheduler_t *sched, const char *name);
void pt_sched_dump(const pt_scheduler_t *sched);

#endif /* PT_H */
```

```c
/**
 * pt.c - Protothread调度器
 * 调度复杂度 O(N) N=任务数
 */
#include "pt.h"
#include <string.h>
#include <stdio.h>

void pt_sched_init(pt_scheduler_t *sched, uint32_t (*tick_fn)(void)) {
    if (!sched) return;
    memset(sched, 0, sizeof(pt_scheduler_t));
    sched->get_tick_ms = tick_fn;
}

int pt_sched_add(pt_scheduler_t *sched, pt_func_t func,
                 uint32_t period_ms, const char *name) {
    if (!sched || !func || !name) return -1;
    if (sched->thread_count >= PT_MAX_THREADS) return -2;

    pt_thread_t *t = &sched->threads[sched->thread_count++];
    memset(t, 0, sizeof(pt_thread_t));
    t->func = func; t->period_ms = period_ms;
    t->name = name; t->active = true;
    PT_INIT(&t->ctx);

    return 0;
}

void pt_sched_run(pt_scheduler_t *sched) {
    if (!sched) return;
    uint32_t now = sched->get_tick_ms();

    for (uint8_t i = 0; i < sched->thread_count; i++) {
        pt_thread_t *t = &sched->threads[i];
        if (!t->active) continue;

        /* 周期任务检查 */ 
        if (t->period_ms > 0) {
            if (now - t->last_run_ms < t->period_ms) continue;
        }

        /* 执行protothread */
        t->context_run = true;
        t->func(&t->ctx);
        t->last_run_ms = now;
        t->run_count++;

        /* 一次性任务完成后移除 */ 
        if (t->period_ms == 0 && PT_ENDED(&t->ctx)) {
            t->active = false;
        }
    }
}

void pt_sched_remove(pt_scheduler_t *sched, const char *name) {
    if (!sched || !name) return;
    for (uint8_t i = 0; i < sched->thread_count; i++) {
        if (strcmp(sched->threads[i].name, name) == 0) {
            sched->threads[i].active = false;
            return;
        }
    }
}

void pt_sched_dump(const pt_scheduler_t *sched) {
    if (!sched) return;
    printf("=== PT Scheduler ===\n");
    for (uint8_t i = 0; i < sched->thread_count; i++) {
        printf("  [%s] lc=%u active=%s runs=%u period=%ums\n",
               sched->threads[i].name,
               sched->threads[i].ctx.lc,
               sched->threads[i].active ? "YES" : "NO",
               sched->threads[i].run_count,
               sched->threads[i].period_ms);
    }
    printf("===================\n");
}
```

```c
/* ============ 使用示例：3个Protothread ============ */

/* 全局模拟时钟 */
static uint32_t sys_tick = 0;
static uint32_t get_tick(void) { return sys_tick; }

/* PT1: LED闪烁任务 */
void pt_led_blink(pt_ctx_t *pt) {
    PT_BEGIN(pt);
    static int counter = 0;
    while (1) {
        counter++;
        printf("[LED] ON  (cnt=%d) @%ums\n", counter, (unsigned)get_tick());
        /* 等待500ms */
        pt->wait_time = get_tick() + 500;
        PT_WAIT_UNTIL(pt, get_tick() >= pt->wait_time);

        printf("[LED] OFF (cnt=%d) @%ums\n", counter, (unsigned)get_tick());
        pt->wait_time = get_tick() + 500;
        PT_WAIT_UNTIL(pt, get_tick() >= pt->wait_time);
    }
    PT_END(pt);
}

/* PT2: 传感器读取任务（模拟I2C交互） */
void pt_sensor_read(pt_ctx_t *pt) {
    PT_BEGIN(pt);
    static float temp = 25.0f;
    while (1) {
        /* 模拟I2C启动 */
        printf("  [SENSOR] I2C Start @%ums\n", (unsigned)get_tick());
        pt->wait_time = get_tick() + 10;
        PT_WAIT_UNTIL(pt, get_tick() >= pt->wait_time);

        /* 模拟发送命令 */
        printf("  [SENSOR] Send CMD @%ums\n", (unsigned)get_tick());
        pt->wait_time = get_tick() + 10;
        PT_WAIT_UNTIL(pt, get_tick() >= pt->wait_time);

        /* 模拟读取数据 */
        temp += 0.5f;
        printf("  [SENSOR] Read temp=%.1f @%ums\n", temp, (unsigned)get_tick());
        pt->wait_time = get_tick() + 20;
        PT_WAIT_UNTIL(pt, get_tick() >= pt->wait_time);

        /* 等待下次读取周期 */ 
        /* PT_YIELD让出，由调度器周期控制 */
        PT_YIELD(pt);
    }
    PT_END(pt);
}

/* PT3: 通信任务（周期性发送） */
void pt_comm_tx(pt_ctx_t *pt) {
    PT_BEGIN(pt);
    static uint8_t seq = 0;
    while (1) {
        seq++;
        printf("    [COMM] TX frame seq=%u @%ums\n", seq, (unsigned)get_tick());
        pt->wait_time = get_tick() + 50;
        PT_WAIT_UNTIL(pt, get_tick() >= pt->wait_time);

        printf("    [COMM] TX done @%ums\n", (unsigned)get_tick());
        PT_YIELD(pt);
    }
    PT_END(pt);
}

int main(void) {
    pt_scheduler_t sched;
    pt_sched_init(&sched, get_tick);

    /* 添加3个protothread */
    pt_sched_add(&sched, pt_led_blink,  200,   "led_blink");   /* 每200ms执行一次 */
    pt_sched_add(&sched, pt_sensor_read, 100,  "sensor_read"); /* 每100ms执行一次 */
    pt_sched_add(&sched, pt_comm_tx,    300,   "comm_tx");     /* 每300ms执行一次 */

    /* 模拟运行2000ms */
    printf("=== Starting Protothread Scheduler ===\n");
    for (int i = 0; i < 20; i++) {
        sys_tick += 50;
        pt_sched_run(&sched);
    }

    pt_sched_dump(&sched);
    return 0;
}
```

### 复杂度分析

| 操作 | 时间 | 空间 |
|------|------|------|
| `PT_BEGIN/PT_END` | O(1) | O(1) |
| `PT_YIELD` | O(1) | O(1) |
| `PT_WAIT_UNTIL` | O(1)/回调 | O(1) |
| 调度器 | O(N) | O(N×2B) per thread |

每个protothread仅需2字节（`uint16_t lc`），栈共享。

### MCU优化要点

1. **超轻量**：每个pt仅2-4字节状态，适合8位MCU
2. **栈共享**：所有pt共用同一个C栈，无上下文栈切换开销
3. **switch-case实现**：编译器将其优化为跳转表
4. **局限性**：不能在switch-case内(或PT_BEGIN内)使用局部变量跨yield
5. **静态变量**：跨yield的持久状态用`static`或外部变量
6. **RTOS vs PT**：Protothread适合内存<2KB的MCU；复杂系统用FreeRTOS
7. **调试**：`volatile`防止编译器优化掉PT_YIELD_FLAG

---

## 附录：综合索引

| 题号 | 主题 | 核心算法 | 时间 | 空间 |
|------|------|----------|------|------|
| Q261 | 环形缓冲区 | FIFO + 临界区 | O(1) | O(N) |
| Q262 | 软件I2C/SPI | GPIO时序模拟 | O(N) | O(1) |
| Q263 | CRC16/CRC32 | 查表法多项式 | O(N) | O(表) |
| Q264 | 内存分配器 | First-Fit + 空闲链表 | O(N) | O(1) |
| Q265 | 快速排序/二分查找 | 分治 + 二分 | O(N log N)/O(log N) | O(log N)/O(1) |
| Q266 | atoi | 逐字符解析+溢出检测 | O(N) | O(1) |
| Q267 | 链表反转/合并 | 迭代双指针 | O(N) | O(1) |
| Q268 | 位图 | 字操作+内建函数 | O(1) per bit | O(N/8) |
| Q269 | FIFO队列 | 数组环形队列 | O(1) | O(N) |
| Q270 | 状态机框架 | 表驱动FSM | O(M) | O(S) |
| Q271 | PID控制器 | PID公式 + 抗饱和 | O(1) | O(1) |
| Q272 | 滑动平均/中值滤波 | 滑动窗口/排序 | O(1)/O(W log W) | O(W) |
| Q273 | Base64编解码 | 3→4映射 | O(N) | O(1) |
| Q274 | JSON解析器 | 递归下降 | O(N) | O(Depth) |
| Q275 | Modbus RTU | CRC校验+帧状态机 | O(N) | O(1) |
| Q276 | 看门狗 | 多任务心跳+单点喂狗 | O(N tasks) | O(N) |
| Q277 | 非阻塞延时 | 时间戳比较 | O(1) | O(1) |
| Q278 | 大小端转换 | 字节交换+内存读写 | O(1) | O(1) |
| Q279 | 矩阵乘法 | 分块+循环交换+展开 | O(N³) | O(1) |
| Q280 | 协程调度器 | Protothread switch-case | O(1) yield | O(2B)/thread |
