#include "timers.h"
/**************************************************************************************************
* File: timers.c
* Defines functions related to timer control and configuration
*************************************************************************************************/
//================================================================================================
// configureSysTickInterrupt()
// @parm: none
// @return: none
// 		Configures the hardware so the SysTick timer will trigger every 1ms
//================================================================================================
void configureSysTickInterrupt(void)
{
	SysTick->CTRL = 0; //disable SysTick timer
	NVIC_SetPriority(SysTick_IRQn, 1); //set priority level at 1
	SysTick->LOAD = SYSTICK_1MS; //set the counter reload value 1ms
	SysTick->VAL = 0; //reset SysTick timer value
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //use system clock
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //enable SysTick interrupts
}





