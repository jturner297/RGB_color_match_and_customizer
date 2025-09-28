
#ifndef TIMERS_H_
#define TIMERS_H_
#include "main.h"
/**************************************************************************************************
* File: timers.h
* Header file for timers.c
*************************************************************************************************/
/******************************
*	Timer Macros/Constants
*******************************/
#define startSysTickTimer_MACRO (SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk)

#define SYS_CLK_FREQ 4000000// default frequency of the device = 4 MHZ
#define cntclk 1000

#define TWO_HZ_SPEED 2
#define THREE_HZ_SPEED 3
#define FOUR_HZ_SPEED 4
#define SIX_HZ_SPEED 6

#define SYSTICK_1MS ((SYS_CLK_FREQ / 1000) - 1)

void configureSysTickInterrupt(void);

#endif /* TIMERS_H_ */
