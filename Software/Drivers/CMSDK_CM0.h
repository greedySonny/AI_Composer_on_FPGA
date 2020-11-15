/**************************************************************************//**
 * @file     CMSDK_CM0.h
 * @brief    CMSIS Cortex-M0 Core Peripheral Access Layer Header File for
 *           Device CMSDK
 * @version  V3.01
 * @date     06. March 2012
 *
 * @note
 * Copyright (C) 2010-2012 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef CMSDK_H
#define CMSDK_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup CMSDK_Definitions CMSDK Definitions
  This file defines all structures and symbols for CMSDK:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - Peripheral definitions
  @{
*/


/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/
/** @addtogroup CMSDK_CMSIS Device CMSIS Definitions
  Configuration of the Cortex-M0 Processor and Core Peripherals
  @{
*/

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

typedef enum IRQn
{
/******  Cortex-M0 Processor Exceptions Numbers ***************************************************/

/* ToDo: use this Cortex interrupt numbers if your device is a CORTEX-M0 device                   */
  NonMaskableInt_IRQn           = -14,      /*!<  2 Cortex-M0 Non Maskable Interrupt              */
  HardFault_IRQn                = -13,      /*!<  3 Cortex-M0 Hard Fault Interrupt                */
  SVCall_IRQn                   = -5,       /*!< 11 Cortex-M0 SV Call Interrupt                   */
  PendSV_IRQn                   = -2,       /*!< 14 Cortex-M0 Pend SV Interrupt                   */
  SysTick_IRQn                  = -1,       /*!< 15 Cortex-M0 System Tick Interrupt               */

/******  CMSDK Specific Interrupt Numbers *********************************************************/
	TIMER0_IRQn                   = 0,
	TIMER1_IRQn                   = 1,
    UART0_IRQn                    = 2,        /*!< UART 0 RX Interrupt                               */
	MIDI_IRQn					  = 3,
	NN_IRQn					      = 4
	
//  UARTRX1_IRQn                  = 2,       /*!< UART 1 RX Interrupt                               */
//  UARTTX1_IRQn                  = 3,       /*!< UART 1 TX Interrupt                               */
//  UARTRX2_IRQn                  = 4,       /*!< UART 2 RX Interrupt                               */
//  UARTTX2_IRQn                  = 5,       /*!< UART 2 TX Interrupt                               */
//  PORT0_ALL_IRQn                = 6,       /*!< Port 0, 2 combined Interrupt, Uart3 Rx            */
//  PORT1_ALL_IRQn                = 7,       /*!< Port 1,3  combined Interrupt, Uart3 Tx            */
//  TIMER0_IRQn                   = 8,       /*!< TIMER 0 Interrupt                                 */
//  TIMER1_IRQn                   = 9,       /*!< TIMER 1 Interrupt                                 */
//  DUALTIMER_IRQn                = 10,      /*!< Dual Timer Interrupt                              */
//  SPI_ALL_IRQn                  = 11,      /*!< SPI Combined interrupt                            */
//  UARTOVF_IRQn                  = 12,      /*!< UART combined Overflow Interrupt                  */
//  ETHERNET_IRQn                 = 13,      /*!< Ethernet if present                               */
//  I2S_IRQn                      = 14,      /*!< Audio I2S Interrupt                               */
//  DMA_IRQn                      = 15,      /*!< PL230 DMA Done + Error Interrupt, and Touchscreen */
//  PORT0_0_IRQn                  = 16,      /*!< UART4 Rx, GPIO individual pin 0.                  */
//  PORT0_1_IRQn                  = 17,      /*!< UART4 Rx, GPIO individual pin 1                   */
//  PORT0_2_IRQn                  = 18,      /*!< GPIO 0 individual pin 2                           */
//  PORT0_3_IRQn                  = 19,      /*!< GPIO 0 individual pin 3                           */
//  PORT0_4_IRQn                  = 20,      /*!< GPIO 0 individual pin 4                           */
//  PORT0_5_IRQn                  = 21,      /*!< GPIO 0 individual pin 5                           */
//  PORT0_6_IRQn                  = 22,      /*!< GPIO 0 individual pin 6                           */
//  PORT0_7_IRQn                  = 23,      /*!< GPIO 0 individual pin 7                           */
//  PORT0_8_IRQn                  = 24,      /*!< GPIO 0 individual pin 8                           */
//  PORT0_9_IRQn                  = 25,      /*!< GPIO 0 individual pin 9                           */
//  PORT0_10_IRQn                 = 26,      /*!< GPIO 0 individual pin 10                          */
//  PORT0_11_IRQn                 = 27,      /*!< GPIO 0 individual pin 11                          */
//  PORT0_12_IRQn                 = 28,      /*!< GPIO 0 individual pin 12                          */
//  PORT0_13_IRQn                 = 29,      /*!< GPIO 0 individual pin 13                          */
//  PORT0_14_IRQn                 = 30,      /*!< GPIO 0 individual pin 14                          */
//  PORT0_15_IRQn                 = 31,      /*!< GPIO 0 individual pin 15                          */
} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0 Processor and Core Peripherals */
#define __CM0_REV                 0x0000    /*!< Core Revision r0p0                               */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */

/*@}*/ /* end of group CMSDK_CMSIS */


#include "core_cm0.h"                       /* Cortex-M0 processor and core peripherals           */
#include "system_CMSDK_CM0.h"               /* CMSDK System include file                          */


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/
/** @addtogroup CMSDK_Peripherals CMSDK Peripherals
  CMSDK Device Specific Peripheral registers structures
  @{
*/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
/** @addtogroup CMSDK_UART CMSDK Universal Asynchronous Receiver/Transmitter
  memory mapped structure for CMSDK_UART
  @{
*/
typedef struct
{
  __IO   uint32_t  DATA;          /*!< Offset: 0x000 Data Register    (R/W) */
  __IO   uint32_t  STATE;         /*!< Offset: 0x004 Status Register  (R/W) */
  __IO   uint32_t  CTRL;          /*!< Offset: 0x008 Control Register (R/W) */
  union {
    __I    uint32_t  INTSTATUS;   /*!< Offset: 0x00C Interrupt Status Register (R/ ) */
    __O    uint32_t  INTCLEAR;    /*!< Offset: 0x00C Interrupt Clear Register ( /W) */
    };
  __IO   uint32_t  BAUDDIV;       /*!< Offset: 0x010 Baudrate Divider Register (R/W) */

} CMSDK_UART_TypeDef;

/* CMSDK_UART DATA Register Definitions */

#define CMSDK_UART_DATA_Pos               0                                             /*!< CMSDK_UART_DATA_Pos: DATA Position */
#define CMSDK_UART_DATA_Msk              (0xFFul << CMSDK_UART_DATA_Pos)                /*!< CMSDK_UART DATA: DATA Mask */

#define CMSDK_UART_STATE_RXOR_Pos         3                                             /*!< CMSDK_UART STATE: RXOR Position */
#define CMSDK_UART_STATE_RXOR_Msk         (0x1ul << CMSDK_UART_STATE_RXOR_Pos)          /*!< CMSDK_UART STATE: RXOR Mask */

#define CMSDK_UART_STATE_TXOR_Pos         2                                             /*!< CMSDK_UART STATE: TXOR Position */
#define CMSDK_UART_STATE_TXOR_Msk         (0x1ul << CMSDK_UART_STATE_TXOR_Pos)          /*!< CMSDK_UART STATE: TXOR Mask */

#define CMSDK_UART_STATE_RXBF_Pos         1                                             /*!< CMSDK_UART STATE: RXBF Position */
#define CMSDK_UART_STATE_RXBF_Msk         (0x1ul << CMSDK_UART_STATE_RXBF_Pos)          /*!< CMSDK_UART STATE: RXBF Mask */

#define CMSDK_UART_STATE_TXBF_Pos         0                                             /*!< CMSDK_UART STATE: TXBF Position */
#define CMSDK_UART_STATE_TXBF_Msk         (0x1ul << CMSDK_UART_STATE_TXBF_Pos )         /*!< CMSDK_UART STATE: TXBF Mask */

#define CMSDK_UART_CTRL_HSTM_Pos          6                                             /*!< CMSDK_UART CTRL: HSTM Position */
#define CMSDK_UART_CTRL_HSTM_Msk          (0x01ul << CMSDK_UART_CTRL_HSTM_Pos)          /*!< CMSDK_UART CTRL: HSTM Mask */

#define CMSDK_UART_CTRL_RXORIRQEN_Pos     5                                             /*!< CMSDK_UART CTRL: RXORIRQEN Position */
#define CMSDK_UART_CTRL_RXORIRQEN_Msk     (0x01ul << CMSDK_UART_CTRL_RXORIRQEN_Pos)     /*!< CMSDK_UART CTRL: RXORIRQEN Mask */

#define CMSDK_UART_CTRL_TXORIRQEN_Pos     4                                             /*!< CMSDK_UART CTRL: TXORIRQEN Position */
#define CMSDK_UART_CTRL_TXORIRQEN_Msk     (0x01ul << CMSDK_UART_CTRL_TXORIRQEN_Pos)     /*!< CMSDK_UART CTRL: TXORIRQEN Mask */

#define CMSDK_UART_CTRL_RXIRQEN_Pos       3                                             /*!< CMSDK_UART CTRL: RXIRQEN Position */
#define CMSDK_UART_CTRL_RXIRQEN_Msk       (0x01ul << CMSDK_UART_CTRL_RXIRQEN_Pos)       /*!< CMSDK_UART CTRL: RXIRQEN Mask */

#define CMSDK_UART_CTRL_TXIRQEN_Pos       2                                             /*!< CMSDK_UART CTRL: TXIRQEN Position */
#define CMSDK_UART_CTRL_TXIRQEN_Msk       (0x01ul << CMSDK_UART_CTRL_TXIRQEN_Pos)       /*!< CMSDK_UART CTRL: TXIRQEN Mask */

#define CMSDK_UART_CTRL_RXEN_Pos          1                                             /*!< CMSDK_UART CTRL: RXEN Position */
#define CMSDK_UART_CTRL_RXEN_Msk          (0x01ul << CMSDK_UART_CTRL_RXEN_Pos)          /*!< CMSDK_UART CTRL: RXEN Mask */

#define CMSDK_UART_CTRL_TXEN_Pos          0                                             /*!< CMSDK_UART CTRL: TXEN Position */
#define CMSDK_UART_CTRL_TXEN_Msk          (0x01ul << CMSDK_UART_CTRL_TXEN_Pos)          /*!< CMSDK_UART CTRL: TXEN Mask */

#define CMSDK_UART_INTSTATUS_RXORIRQ_Pos  3                                             /*!< CMSDK_UART CTRL: RXORIRQ Position */
#define CMSDK_UART_CTRL_RXORIRQ_Msk       (0x01ul << CMSDK_UART_INTSTATUS_RXORIRQ_Pos)  /*!< CMSDK_UART CTRL: RXORIRQ Mask */

#define CMSDK_UART_CTRL_TXORIRQ_Pos       2                                             /*!< CMSDK_UART CTRL: TXORIRQ Position */
#define CMSDK_UART_CTRL_TXORIRQ_Msk       (0x01ul << CMSDK_UART_CTRL_TXORIRQ_Pos)       /*!< CMSDK_UART CTRL: TXORIRQ Mask */

#define CMSDK_UART_CTRL_RXIRQ_Pos         1                                             /*!< CMSDK_UART CTRL: RXIRQ Position */
#define CMSDK_UART_CTRL_RXIRQ_Msk         (0x01ul << CMSDK_UART_CTRL_RXIRQ_Pos)         /*!< CMSDK_UART CTRL: RXIRQ Mask */

#define CMSDK_UART_CTRL_TXIRQ_Pos         0                                             /*!< CMSDK_UART CTRL: TXIRQ Position */
#define CMSDK_UART_CTRL_TXIRQ_Msk         (0x01ul << CMSDK_UART_CTRL_TXIRQ_Pos)         /*!< CMSDK_UART CTRL: TXIRQ Mask */

#define CMSDK_UART_BAUDDIV_Pos            0                                             /*!< CMSDK_UART BAUDDIV: BAUDDIV Position */
#define CMSDK_UART_BAUDDIV_Msk           (0xFFFFFul << CMSDK_UART_BAUDDIV_Pos)          /*!< CMSDK_UART BAUDDIV: BAUDDIV Mask */

/*@}*/ /* end of group CMSDK_UART */


/*----------------------------- Timer (TIMER) -------------------------------*/
/** @addtogroup CMSDK_TIMER CMSDK Timer
  @{
*/
typedef struct
{
  __IO   uint32_t  CTRL;          /*!< Offset: 0x000 Control Register (R/W) */
  __IO   uint32_t  VALUE;         /*!< Offset: 0x004 Current Value Register (R/W) */
  __IO   uint32_t  RELOAD;        /*!< Offset: 0x008 Reload Value Register  (R/W) */
  union {
    __I    uint32_t  INTSTATUS;   /*!< Offset: 0x00C Interrupt Status Register (R/ ) */
    __O    uint32_t  INTCLEAR;    /*!< Offset: 0x00C Interrupt Clear Register ( /W) */
    };

} CMSDK_TIMER_TypeDef;

/* CMSDK_TIMER CTRL Register Definitions */

#define CMSDK_TIMER_CTRL_IRQEN_Pos          3                                              /*!< CMSDK_TIMER CTRL: IRQEN Position */
#define CMSDK_TIMER_CTRL_IRQEN_Msk          (0x01ul << CMSDK_TIMER_CTRL_IRQEN_Pos)         /*!< CMSDK_TIMER CTRL: IRQEN Mask */

#define CMSDK_TIMER_CTRL_SELEXTCLK_Pos      2                                              /*!< CMSDK_TIMER CTRL: SELEXTCLK Position */
#define CMSDK_TIMER_CTRL_SELEXTCLK_Msk      (0x01ul << CMSDK_TIMER_CTRL_SELEXTCLK_Pos)     /*!< CMSDK_TIMER CTRL: SELEXTCLK Mask */

#define CMSDK_TIMER_CTRL_SELEXTEN_Pos       1                                              /*!< CMSDK_TIMER CTRL: SELEXTEN Position */
#define CMSDK_TIMER_CTRL_SELEXTEN_Msk       (0x01ul << CMSDK_TIMER_CTRL_SELEXTEN_Pos)      /*!< CMSDK_TIMER CTRL: SELEXTEN Mask */

#define CMSDK_TIMER_CTRL_EN_Pos             0                                              /*!< CMSDK_TIMER CTRL: EN Position */
#define CMSDK_TIMER_CTRL_EN_Msk             (0x01ul << CMSDK_TIMER_CTRL_EN_Pos)            /*!< CMSDK_TIMER CTRL: EN Mask */

#define CMSDK_TIMER_VAL_CURRENT_Pos         0                                              /*!< CMSDK_TIMER VALUE: CURRENT Position */
#define CMSDK_TIMER_VAL_CURRENT_Msk         (0xFFFFFFFFul << CMSDK_TIMER_VAL_CURRENT_Pos)  /*!< CMSDK_TIMER VALUE: CURRENT Mask */

#define CMSDK_TIMER_RELOAD_VAL_Pos          0                                              /*!< CMSDK_TIMER RELOAD: RELOAD Position */
#define CMSDK_TIMER_RELOAD_VAL_Msk          (0xFFFFFFFFul << CMSDK_TIMER_RELOAD_VAL_Pos)   /*!< CMSDK_TIMER RELOAD: RELOAD Mask */

#define CMSDK_TIMER_INTSTATUS_Pos           0                                              /*!< CMSDK_TIMER INTSTATUS: INTSTATUSPosition */
#define CMSDK_TIMER_INTSTATUS_Msk           (0x01ul << CMSDK_TIMER_INTSTATUS_Pos)          /*!< CMSDK_TIMER INTSTATUS: INTSTATUSMask */

#define CMSDK_TIMER_INTCLEAR_Pos            0                                              /*!< CMSDK_TIMER INTCLEAR: INTCLEAR Position */
#define CMSDK_TIMER_INTCLEAR_Msk            (0x01ul << CMSDK_TIMER_INTCLEAR_Pos)           /*!< CMSDK_TIMER INTCLEAR: INTCLEAR Mask */

/*@}*/ /* end of group CMSDK_TIMER */



/*-------------------- General Purpose Input Output (GPIO) -------------------*/

/** @addtogroup CMSDK_GPIO CMSDK GPIO
  @{
*/
typedef struct
{
  __IO   uint32_t  DATA;             /*!< Offset: 0x000 DATA Register (R/W) */
  __IO   uint32_t  DATAOUT;          /*!< Offset: 0x004 Data Output Latch Register (R/W) */
         uint32_t  RESERVED0[2];
  __IO   uint32_t  OUTENABLESET;     /*!< Offset: 0x010 Output Enable Set Register  (R/W) */
  __IO   uint32_t  OUTENABLECLR;     /*!< Offset: 0x014 Output Enable Clear Register  (R/W) */
  __IO   uint32_t  ALTFUNCSET;       /*!< Offset: 0x018 Alternate Function Set Register  (R/W) */
  __IO   uint32_t  ALTFUNCCLR;       /*!< Offset: 0x01C Alternate Function Clear Register  (R/W) */
  __IO   uint32_t  INTENSET;         /*!< Offset: 0x020 Interrupt Enable Set Register  (R/W) */
  __IO   uint32_t  INTENCLR;         /*!< Offset: 0x024 Interrupt Enable Clear Register  (R/W) */
  __IO   uint32_t  INTTYPESET;       /*!< Offset: 0x028 Interrupt Type Set Register  (R/W) */
  __IO   uint32_t  INTTYPECLR;       /*!< Offset: 0x02C Interrupt Type Clear Register  (R/W) */
  __IO   uint32_t  INTPOLSET;        /*!< Offset: 0x030 Interrupt Polarity Set Register  (R/W) */
  __IO   uint32_t  INTPOLCLR;        /*!< Offset: 0x034 Interrupt Polarity Clear Register  (R/W) */
  union {
    __I    uint32_t  INTSTATUS;      /*!< Offset: 0x038 Interrupt Status Register (R/ ) */
    __O    uint32_t  INTCLEAR;       /*!< Offset: 0x038 Interrupt Clear Register ( /W) */
    };
         uint32_t RESERVED1[241];
  __IO   uint32_t LB_MASKED[256];    /*!< Offset: 0x400 - 0x7FC Lower byte Masked Access Register (R/W) */
  __IO   uint32_t UB_MASKED[256];    /*!< Offset: 0x800 - 0xBFC Upper byte Masked Access Register (R/W) */
} CMSDK_GPIO_TypeDef;

#define CMSDK_GPIO_DATA_Pos            0                                          /*!< CMSDK_GPIO DATA: DATA Position */
#define CMSDK_GPIO_DATA_Msk            (0xFFFFul << CMSDK_GPIO_DATA_Pos)          /*!< CMSDK_GPIO DATA: DATA Mask */

#define CMSDK_GPIO_DATAOUT_Pos         0                                          /*!< CMSDK_GPIO DATAOUT: DATAOUT Position */
#define CMSDK_GPIO_DATAOUT_Msk         (0xFFFFul << CMSDK_GPIO_DATAOUT_Pos)       /*!< CMSDK_GPIO DATAOUT: DATAOUT Mask */

#define CMSDK_GPIO_OUTENSET_Pos        0                                          /*!< CMSDK_GPIO OUTEN: OUTEN Position */
#define CMSDK_GPIO_OUTENSET_Msk        (0xFFFFul << CMSDK_GPIO_OUTEN_Pos)         /*!< CMSDK_GPIO OUTEN: OUTEN Mask */

#define CMSDK_GPIO_OUTENCLR_Pos        0                                          /*!< CMSDK_GPIO OUTEN: OUTEN Position */
#define CMSDK_GPIO_OUTENCLR_Msk        (0xFFFFul << CMSDK_GPIO_OUTEN_Pos)         /*!< CMSDK_GPIO OUTEN: OUTEN Mask */

#define CMSDK_GPIO_ALTFUNCSET_Pos      0                                          /*!< CMSDK_GPIO ALTFUNC: ALTFUNC Position */
#define CMSDK_GPIO_ALTFUNCSET_Msk      (0xFFFFul << CMSDK_GPIO_ALTFUNC_Pos)       /*!< CMSDK_GPIO ALTFUNC: ALTFUNC Mask */

#define CMSDK_GPIO_ALTFUNCCLR_Pos      0                                          /*!< CMSDK_GPIO ALTFUNC: ALTFUNC Position */
#define CMSDK_GPIO_ALTFUNCCLR_Msk      (0xFFFFul << CMSDK_GPIO_ALTFUNC_Pos)       /*!< CMSDK_GPIO ALTFUNC: ALTFUNC Mask */

#define CMSDK_GPIO_INTENSET_Pos        0                                          /*!< CMSDK_GPIO INTEN: INTEN Position */
#define CMSDK_GPIO_INTENSET_Msk        (0xFFFFul << CMSDK_GPIO_INTEN_Pos)         /*!< CMSDK_GPIO INTEN: INTEN Mask */

#define CMSDK_GPIO_INTENCLR_Pos        0                                          /*!< CMSDK_GPIO INTEN: INTEN Position */
#define CMSDK_GPIO_INTENCLR_Msk        (0xFFFFul << CMSDK_GPIO_INTEN_Pos)         /*!< CMSDK_GPIO INTEN: INTEN Mask */

#define CMSDK_GPIO_INTTYPESET_Pos      0                                          /*!< CMSDK_GPIO INTTYPE: INTTYPE Position */
#define CMSDK_GPIO_INTTYPESET_Msk      (0xFFFFul << CMSDK_GPIO_INTTYPE_Pos)       /*!< CMSDK_GPIO INTTYPE: INTTYPE Mask */

#define CMSDK_GPIO_INTTYPECLR_Pos      0                                          /*!< CMSDK_GPIO INTTYPE: INTTYPE Position */
#define CMSDK_GPIO_INTTYPECLR_Msk      (0xFFFFul << CMSDK_GPIO_INTTYPE_Pos)       /*!< CMSDK_GPIO INTTYPE: INTTYPE Mask */

#define CMSDK_GPIO_INTPOLSET_Pos       0                                          /*!< CMSDK_GPIO INTPOL: INTPOL Position */
#define CMSDK_GPIO_INTPOLSET_Msk       (0xFFFFul << CMSDK_GPIO_INTPOL_Pos)        /*!< CMSDK_GPIO INTPOL: INTPOL Mask */

#define CMSDK_GPIO_INTPOLCLR_Pos       0                                          /*!< CMSDK_GPIO INTPOL: INTPOL Position */
#define CMSDK_GPIO_INTPOLCLR_Msk       (0xFFFFul << CMSDK_GPIO_INTPOL_Pos)        /*!< CMSDK_GPIO INTPOL: INTPOL Mask */

#define CMSDK_GPIO_INTSTATUS_Pos       0                                          /*!< CMSDK_GPIO INTSTATUS: INTSTATUS Position */
#define CMSDK_GPIO_INTSTATUS_Msk       (0xFFul << CMSDK_GPIO_INTSTATUS_Pos)       /*!< CMSDK_GPIO INTSTATUS: INTSTATUS Mask */

#define CMSDK_GPIO_INTCLEAR_Pos        0                                          /*!< CMSDK_GPIO INTCLEAR: INTCLEAR Position */
#define CMSDK_GPIO_INTCLEAR_Msk        (0xFFul << CMSDK_GPIO_INTCLEAR_Pos)        /*!< CMSDK_GPIO INTCLEAR: INTCLEAR Mask */

#define CMSDK_GPIO_MASKLOWBYTE_Pos     0                                          /*!< CMSDK_GPIO MASKLOWBYTE: MASKLOWBYTE Position */
#define CMSDK_GPIO_MASKLOWBYTE_Msk     (0x00FFul << CMSDK_GPIO_MASKLOWBYTE_Pos)   /*!< CMSDK_GPIO MASKLOWBYTE: MASKLOWBYTE Mask */

#define CMSDK_GPIO_MASKHIGHBYTE_Pos    0                                          /*!< CMSDK_GPIO MASKHIGHBYTE: MASKHIGHBYTE Position */
#define CMSDK_GPIO_MASKHIGHBYTE_Msk    (0xFF00ul << CMSDK_GPIO_MASKHIGHBYTE_Pos)  /*!< CMSDK_GPIO MASKHIGHBYTE: MASKHIGHBYTE Mask */

/*@}*/ /* end of group CMSDK_GPIO */


/*------------- System Control (SYSCON) --------------------------------------*/
/** @addtogroup CMSDK_SYSCON CMSDK System Control
  @{
*/
typedef struct
{
  __IO   uint32_t  REMAP;          /*!< Offset: 0x000 Remap Control Register (R/W) */
  __IO   uint32_t  PMUCTRL;        /*!< Offset: 0x004 PMU Control Register (R/W) */
  __IO   uint32_t  RESETOP;        /*!< Offset: 0x008 Reset Option Register  (R/W) */
  __IO   uint32_t  EMICTRL;        /*!< Offset: 0x00C EMI Control Register  (R/W) */
  __IO   uint32_t  RSTINFO;        /*!< Offset: 0x010 Reset Information Register (R/W) */
} CMSDK_SYSCON_TypeDef;

#define CMSDK_SYSCON_REMAP_Pos                 0
#define CMSDK_SYSCON_REMAP_Msk                 (0x01ul << CMSDK_SYSCON_REMAP_Pos)               /*!< CMSDK_SYSCON MEME_CTRL: REMAP Mask */

#define CMSDK_SYSCON_PMUCTRL_EN_Pos            0
#define CMSDK_SYSCON_PMUCTRL_EN_Msk            (0x01ul << CMSDK_SYSCON_PMUCTRL_EN_Pos)          /*!< CMSDK_SYSCON PMUCTRL: PMUCTRL ENABLE Mask */

#define CMSDK_SYSCON_LOCKUPRST_RESETOP_Pos     0
#define CMSDK_SYSCON_LOCKUPRST_RESETOP_Msk     (0x01ul << CMSDK_SYSCON_LOCKUPRST_RESETOP_Pos)   /*!< CMSDK_SYSCON SYS_CTRL: LOCKUP RESET ENABLE Mask */

#define CMSDK_SYSCON_EMICTRL_SIZE_Pos          24
#define CMSDK_SYSCON_EMICTRL_SIZE_Msk          (0x00001ul << CMSDK_SYSCON_EMICTRL_SIZE_Pos)     /*!< CMSDK_SYSCON EMICTRL: SIZE Mask */

#define CMSDK_SYSCON_EMICTRL_TACYC_Pos         16
#define CMSDK_SYSCON_EMICTRL_TACYC_Msk         (0x00007ul << CMSDK_SYSCON_EMICTRL_TACYC_Pos)    /*!< CMSDK_SYSCON EMICTRL: TURNAROUNDCYCLE Mask */

#define CMSDK_SYSCON_EMICTRL_WCYC_Pos          8
#define CMSDK_SYSCON_EMICTRL_WCYC_Msk          (0x00003ul << CMSDK_SYSCON_EMICTRL_WCYC_Pos)     /*!< CMSDK_SYSCON EMICTRL: WRITECYCLE Mask */

#define CMSDK_SYSCON_EMICTRL_RCYC_Pos          0
#define CMSDK_SYSCON_EMICTRL_RCYC_Msk          (0x00007ul << CMSDK_SYSCON_EMICTRL_RCYC_Pos)     /*!< CMSDK_SYSCON EMICTRL: READCYCLE Mask */

#define CMSDK_SYSCON_RSTINFO_SYSRESETREQ_Pos   0
#define CMSDK_SYSCON_RSTINFO_SYSRESETREQ_Msk   (0x00001ul << CMSDK_SYSCON_RSTINFO_SYSRESETREQ_Pos) /*!< CMSDK_SYSCON RSTINFO: SYSRESETREQ Mask */

#define CMSDK_SYSCON_RSTINFO_WDOGRESETREQ_Pos  1
#define CMSDK_SYSCON_RSTINFO_WDOGRESETREQ_Msk  (0x00001ul << CMSDK_SYSCON_RSTINFO_WDOGRESETREQ_Pos) /*!< CMSDK_SYSCON RSTINFO: WDOGRESETREQ Mask */

#define CMSDK_SYSCON_RSTINFO_LOCKUPRESET_Pos   2
#define CMSDK_SYSCON_RSTINFO_LOCKUPRESET_Msk   (0x00001ul << CMSDK_SYSCON_RSTINFO_LOCKUPRESET_Pos) /*!< CMSDK_SYSCON RSTINFO: LOCKUPRESET Mask */

/*@}*/ /* end of group CMSDK_SYSCON */





typedef struct
{
	__IO uint32_t LEDOUT;

}CMSDK_LED_TypeDef;


typedef struct
{
	__IO uint32_t CR; // CTRL
	__IO uint32_t SR; // STATE

}DDR3CTRL_TypeDef;

typedef struct
{
	__IO 	uint32_t CR;
	__IO	uint32_t SR;
	__IO	uint32_t INT;
	__IO 	uint32_t DATA;
	
	__IO	uint32_t BIAS_ADDR;
	__IO	uint32_t WEIGHT_ADDR;
	__IO 	uint32_t IN_CH;
	__IO 	uint32_t OUT_CH;
	__IO	uint32_t CMD;
	__IO 	uint32_t COMPENSATION;
	__IO 	uint32_t LAYER;

}NN_CTRL_TypeDef;

typedef struct
{

  __IO    uint32_t  CR0;
	__IO    uint32_t  CR1;
	__IO    uint32_t  DR;
	__IO    uint32_t  SR;
	__IO    uint32_t  CPSR;
	__IO    uint32_t  IMSC;
	__IO    uint32_t  RIS;
	__IO    uint32_t  MIS;
	__IO    uint32_t  ICR;
	__IO    uint32_t  DMACR;

}PL022_SPI_TypeDef;

typedef struct
{
	__IO 	uint32_t CR;
	__IO	uint32_t SR;
	__IO    uint32_t INT;
	__IO    uint32_t DATA;
	
	__IO    uint32_t CMD;
	__IO    uint32_t SOUNDADDR;
	__IO 	uint32_t TRACK_SEL;
	__IO 	uint32_t NUM;

}MIDI_TypeDef;

/*------------------- PrimeCell APB GPIO --------------------------------------*/
/** @addtogroup CMSDK_PL061 CMSDK APB GPIO
  @{
*/
typedef struct
{

__IO    uint32_t  DATA[256];
__IO    uint32_t  DIR;
__IO    uint32_t  INTSENSE;
__IO    uint32_t  INTBOTHEDGE;
__IO    uint32_t  INTEVENT;
__IO    uint32_t  INTMASK;
__O     uint32_t  RAWINTSTAT;
__O     uint32_t  MASKINTSTAT;
__I     uint32_t  INTCLR;
__IO    uint32_t  MODECTRL;

}APBGPIO_TypeDef;

#define CMSDK_PL061_DATA_Pos               0                                              /*!< CMSDK_PL061 DATA: DATA Position */
#define CMSDK_PL061_DATA_Msk              (0xFFFFFFFFul << CMSDK_PL061_LOAD_Pos)          /*!< CMSDK_PL061 DATA: DATA Mask */

#define CMSDK_PL061_DIR_Pos                0                                              /*!< CMSDK_PL061 DIR: Data Direction Position */
#define CMSDK_PL061_DIR_Msk               (0x1ul << CMSDK_PL061_DIR_Pos)                  /*!< CMSDK_PL061 DIR: Data Direction  Mask */

#define CMSDK_PL061_INTSENSE_Pos           0                                              /*!< CMSDK_PL061 INTSENSE: INT SENSE Position */
#define CMSDK_PL061_INTSENSE_Msk          (0x1ul << CMSDK_PL061_INTSENSE_Pos)             /*!< CMSDK_PL061 INTSENSE: INT SENSE Mask */

#define CMSDK_PL061_INTBOTHEDGE_Pos        0                                              /*!< CMSDK_PL061 INTBOTHEDGE: INT BOTH EDGE Position */
#define CMSDK_PL061_INTBOTHEDGE_Msk       (0x1ul << CMSDK_PL061_INTBOTHEDGE_Pos)          /*!< CMSDK_PL061 INTBOTHEDGE: INT BOTH EDGE Mask */

#define CMSDK_PL061_INTEVENT_Pos           0                                              /*!< CMSDK_PL061 INTEVENT: INT EVENT Position */
#define CMSDK_PL061_INTEVENT_Msk          (0x1ul << CMSDK_PL061_INTEVENT_Pos)             /*!< CMSDK_PL061 INTEVENT: INT EVENT Mask */

#define CMSDK_PL061_INTMASK_Pos            0                                              /*!< CMSDK_PL061 INTMASK: INT MASK Position */
#define CMSDK_PL061_INTMASK_Msk           (0x1ul << CMSDK_PL061_INTMASK_Pos)              /*!< CMSDK_PL061 INTMASK: INT MASK Mask */

#define CMSDK_PL061_RAWINTSTAT_Pos         0                                              /*!< CMSDK_PL061 RAWINTSTAT: Raw Int Status Position */
#define CMSDK_PL061_RAWINTSTAT_Msk        (0x1ul << CMSDK_PL061_RAWINTSTAT_Pos)           /*!< CMSDK_PL061 RAWINTSTAT: Raw Int Status Mask */

#define CMSDK_PL061_MASKINTSTAT_Pos        0                                              /*!< CMSDK_PL061 MASKINTSTAT: Mask Int Status Position */
#define CMSDK_PL061_MASKINTSTAT_Msk       (0x1ul << CMSDK_PL061_MASKINTSTAT_Pos)          /*!< CMSDK_PL061 MASKINTSTAT: Mask Int Status Mask */

#define CMSDK_PL061_INTCLR_Pos             0                                              /*!< CMSDK_PL061 INTCLR: Int Clear Position */
#define CMSDK_PL061_INTCLR_Msk            (0x1ul << CMSDK_PL061_INTCLR_Pos)               /*!< CMSDK_PL061 INTCLR: Int Clear Mask */

#define CMSDK_PL061_MODECTRL_HWEN_Pos      0                                              /*!< CMSDK_PL061 MODECTRL_HWEN: Mode Control Hardware Enable Position */
#define CMSDK_PL061_MODECTRL_HWEN_Msk     (0x1ul << CMSDK_PL061_MODECTRL_HWEN_Pos)        /*!< CMSDK_PL061 MODECTRL_HWEN: Mode Control Hardware Enable Mask */


/*@}*/ /* end of group  CMSDK_PL061 */


#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

/*@}*/ /* end of group CMSDK_Peripherals */


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/** @addtogroup CMSDK_MemoryMap CMSDK Memory Mapping
  @{
*/

/* Peripheral and SRAM base address */
#define CMSDK_FLASH_BASE        (0x00000000UL)  /*!< (FLASH     ) Base Address */
#define CMSDK_SRAM_BASE         (0x20000000UL)  /*!< (SRAM      ) Base Address */
#define CMSDK_PERIPH_BASE       (0x40000000UL)  /*!< (Peripheral) Base Address */
#define DDR3_BASE				(0x50000000UL)


#define CMSDK_RAM_BASE          (0x20000000UL)
#define CMSDK_APB_BASE          (0x40000000UL)
#define CMSDK_AHB_BASE          (0x40010000UL)

/* APB peripherals                                                           */
#define CMSDK_TIMER0_BASE       (CMSDK_APB_BASE + 0x0000UL)
#define CMSDK_TIMER1_BASE       (CMSDK_APB_BASE + 0x1000UL)
#define CMSDK_LED_BASE       	(CMSDK_APB_BASE + 0x2000UL)
#define MIDI_BASE       		(CMSDK_APB_BASE + 0x3000UL)
#define CMSDK_UART0_BASE        (CMSDK_APB_BASE + 0x4000UL)
#define CMSDK_UART1_BASE        (CMSDK_APB_BASE + 0x5000UL)
#define PL022_SPI0_BASE			(CMSDK_APB_BASE + 0x6000UL)

/* AHB peripherals                                                           */
#define CMSDK_GPIO0_BASE        (CMSDK_AHB_BASE + 0x0000UL)
#define CMSDK_GPIO1_BASE        (CMSDK_AHB_BASE + 0x1000UL)
#define NN_CTRL_BASE			(CMSDK_AHB_BASE + 0x2000UL)
#define DDR3CTRL_BASE			(CMSDK_AHB_BASE + 0x3000UL)


/*@}*/ /* end of group CMSDK_MemoryMap */


/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/

/** @addtogroup CMSDK_PeripheralDecl CMSDK Peripheral Declaration
  @{
*/

#define CMSDK_TIMER0            ((CMSDK_TIMER_TypeDef  *) CMSDK_TIMER0_BASE  )
#define CMSDK_TIMER1            ((CMSDK_TIMER_TypeDef  *) CMSDK_TIMER1_BASE  )
#define CMSDK_LED               ((CMSDK_LED_TypeDef    *) CMSDK_LED_BASE     )
#define MIDI					((MIDI_TypeDef   	   *) MIDI_BASE    		 )
#define CMSDK_UART0             ((CMSDK_UART_TypeDef   *) CMSDK_UART0_BASE   )
#define CMSDK_UART1             ((CMSDK_UART_TypeDef   *) CMSDK_UART1_BASE   )



#define CMSDK_GPIO0             ((CMSDK_GPIO_TypeDef   *) CMSDK_GPIO0_BASE   )
#define CMSDK_GPIO1             ((CMSDK_GPIO_TypeDef   *) CMSDK_GPIO1_BASE   )
#define DDR3CTRL				((DDR3CTRL_TypeDef     *) DDR3CTRL_BASE      )
#define NN_CTRL					((NN_CTRL_TypeDef      *) NN_CTRL_BASE       )
#define PL022_SPI0				((PL022_SPI_TypeDef    *) PL022_SPI0_BASE    )

/*@}*/ /* end of group CMSDK_PeripheralDecl */

/*@}*/ /* end of group CMSDK_Definitions */

#ifdef __cplusplus
}
#endif

#endif  /* CMSDK_H */

#ifdef SIMULATION
#define CLK_FREQ 1000
#else
#define CLK_FREQ 40000000UL
#endif

