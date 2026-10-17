/*
 * wait.c
 *
 *  Created on: Jul 20, 2018
 *      Author: nxf44615
 */

#include <stdint.h>
#include <stdbool.h>
#include <clock_manager.h>
#include "BSPTypeDef.h"
#include "wait.h"

/** Gets needed cycles for specified delay in microseconds,
 *  calculation is based on core clock frequency. */
#define S32_GET_CYCLES_FOR_US(us, freq) (((freq) / 1000U) * (us) / 1000U)

/**
 * @brief Waits for exact number of cycles which can be expressed as multiple of 4.
 *
 * MOV - 1 cycle
 * SUB - 1 cycle
 * BNE - 1 cycle or 2 cycles if jump is realized
 *
 * @param cycles        Number of cycles to wait.
 */
#if defined(__thumb__) && !defined(__thumb2__) /* Thumb instruction set only */
#define S32_WAIT_FOR_MUL4_CYCLES(cycles) \
  __asm( \
    "mov r0, %[cycles] \n\t" \
    "0: \n\t"                \
      "sub r0, #4 \n\t"      \
      "nop \n\t"             \
    "bne 0b \n\t"            \
     :                       \
     : [cycles] "r" (cycles) \
     : "r0", "r1", "cc"      \
  )

#else /* Thumb2 or A32 instruction set */
#define S32_WAIT_FOR_MUL4_CYCLES(cycles) \
  __asm( \
    "movs r0, %[cycles] \n"  \
    "0: \n"                  \
      "subs r0, r0, #4 \n"   \
      "nop \n\t"             \
    "bne 0b \n"              \
     :                       \
     : [cycles] "r" (cycles) \
     : "r0", "r1", "cc"      \
  )
#endif

/**
 * @brief This function returns core clock frequency.
 */
static uint32_t S32_WAIT_GetSystemClockFreq(void);


/*FUNCTION**********************************************************************
 *
 * Function Name : FS65_WaitUs
 * Description : This function returns core clock frequency.
 *
 *END**************************************************************************/
static uint32_t S32_WAIT_GetSystemClockFreq(void)
{
uint32_t freq;

CLOCK_SYS_GetFreq(CORE_CLK, &freq);

return freq;
}

/* This function waits for specified amount of microseconds. */
void S32_WaitUs(u16 delayUs)
{
    uint32_t cycles = (uint32_t) S32_GET_CYCLES_FOR_US(delayUs, S32_WAIT_GetSystemClockFreq());

    /* Advance to next multiple of 4. Value 0x04U ensures that the number
     * is not zero. */
    cycles = (cycles & 0xFFFFFFFCU) | 0x04U;
    S32_WAIT_FOR_MUL4_CYCLES(cycles);
}
