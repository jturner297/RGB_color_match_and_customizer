#include <stdint.h>
#include <stm32l476xx.h>
#include <stddef.h> // For NULL definition

#include "main.h"
#include "input.h"
#include "timers.h"

/***************************************************************************************************
* @file main.c
* @brief Main program body
* @author: Justin Turner
* @version 2.0
* ------------------------------------------------------------------------------------------------
*  RGB LED test
**************************************************************************************************
*/

struct GPIO_config PROGRESS_BAR_LEDS[]={
	    {GPIOA, 6},
	    {GPIOA, 11},
	    {GPIOA, 12},
	    {GPIOB, 9},
	    {GPIOC, 5},
	    {GPIOB, 8},
	    {GPIOC, 6},
	    {GPIOC, 9},
	    {GPIOC, 8}
};




struct LED_block PROGRESS_BAR ={
	.leds = PROGRESS_BAR_LEDS , //GPIO information for the LEDs in the block
	 .num_of_leds = 9 //number of LEDs present in the block

};

struct GPIO_LED_config GPIOA_LED_config={
		.port = GPIOA,
		.pins = {5,6, 11, 12}, //Pin number
		.num_of_pins = 4,
		.GPIOx_clk_enable_num = 0
};
struct GPIO_LED_config GPIOB_LED_config={
		.port = GPIOB,
		.pins = {8,9}, //Pin number
		.num_of_pins = 2,
		.GPIOx_clk_enable_num = 1
};
struct GPIO_LED_config GPIOC_LED_config={
		.port = GPIOC,
		.pins = {8,9,6,5}, //Pin number
		.num_of_pins = 4,
		.GPIOx_clk_enable_num = 2
};


void CONFIGURE_STANDARD_LEDS(struct GPIO_LED_config *GPIOx_LED_config){

	for (uint32_t i = 0; i < GPIOx_LED_config->num_of_pins; i++){ //runs 3 times
			RCC->AHB2ENR |= (0x1 << GPIOx_LED_config->GPIOx_clk_enable_num);
			//configure pins for output  mode
			GPIOx_LED_config->port->MODER &= ~(0x3 << (2 * GPIOx_LED_config->pins[i])); //clear
			GPIOx_LED_config->port->MODER |= (0x1 << (2 *GPIOx_LED_config->pins[i])); //10
			GPIOx_LED_config->port->OTYPER &= ~(0x1 << GPIOx_LED_config->pins[i]);
			GPIOx_LED_config->port->OSPEEDR &= ~(0x3 << (2 *GPIOx_LED_config->pins[i])); //10
			GPIOx_LED_config->port->PUPDR &= ~(0x3 << (2 *GPIOx_LED_config->pins[i])); //10
		}
}
//just configures pins



void SETUP_ALTERNATE_FUNCTION_PINS(struct RGB_Light_Emitting_Diode *RGB_LED){
	uint32_t AFR_index, shift;


	for (uint32_t i = 0; i < RGB_LED->num_of_pins; i++){ //runs 3 times
		RCC->AHB2ENR |= (0x1 << RGB_LED->PIN_config[i].clock_index);

		//configure pins for alternating function mode
		RGB_LED->PIN_config[i].port->MODER &= ~(0x3 << (2 * RGB_LED->PIN_config[i].pin)); //clear
		RGB_LED->PIN_config[i].port->MODER |= (0x2 << (2 *RGB_LED->PIN_config[i].pin)); //10
		//port->OTYPER &= ~(0x1 << pins[i]);
		//port->OSPEEDR &= ~(0x3 << (2 * pins[i]));
		//port->PUPDR &= ~(0x3 << (2 * pins[i]));

		AFR_index = (RGB_LED->PIN_config[i].pin < 8 ) ? 0 : 1; //if pin number is less than 8, use AF low, else use AF high
		shift = 4 * (RGB_LED->PIN_config[i].pin % 8); //calculate bit shit

		RGB_LED->PIN_config[i].port->AFR[AFR_index] &= ~((0xF) << (shift));
		RGB_LED->PIN_config[i].port->AFR[AFR_index] |= (RGB_LED->AF_number << (shift));
	}
}



void SETUP_PWN(struct RGB_Light_Emitting_Diode *RGB_LED){

	RCC->APB1ENR1 |= RGB_LED->TIMx_clk_enable_mask;// Enable TIM3 clock

	// Prescaler and ARR for ~1kHz PWM (80 MHz clock)
	RGB_LED->TIMx->PSC = 79;   // 80 MHz / (79+1) = 1 MHz
	RGB_LED->TIMx->ARR = 999;  // 1 MHz / (999+1) = 1 kHz

	RGB_LED->TIMx->CCMR1 &= ~((0x7 << 4) | (0x7 << 12)); // Clear bits for OC1M and OC2M in CCMR1
	RGB_LED->TIMx->CCMR1 |= ((0x6 << 4) | (0x6 << 12)); // Set OC1M and OC2M to PWM mode 1 (110 binary = 0x6)

	RGB_LED->TIMx->CCMR1 |= (1 << 3) | (1 << 11);// Enable preload for CCR1 and CCR2

	RGB_LED->TIMx->CCMR2 &= ~(0x7 << 4); // CCMR2 controls channels 3 and 4
	RGB_LED->TIMx->CCMR2 |= (0x6 << 4);
	RGB_LED->TIMx->CCMR2 |= (1 << 3);// Enable preload for CCR3


	RGB_LED->TIMx->CCER |= (1 << 0) | (1 << 4) | (1 << 8);
	RGB_LED->TIMx->CR1 |= (1 << 7);
}

enum system_states button_mode = count_up;

struct RGB_Light_Emitting_Diode PLAYER_LED = {
		.TIMx = TIM2,
		.TIMx_clk_enable_mask = RCC_APB1ENR1_TIM2EN,

		.PIN_config = {
		{ GPIOA, 0, 0 },
		{ GPIOB, 3, 1 },
		{ GPIOB, 10, 1 }
			},
			.num_of_pins = 3,
			.AF_number = 1

};

struct RGB_Light_Emitting_Diode SYSTEM_LED = {
		.TIMx = TIM3,
		.TIMx_clk_enable_mask = RCC_APB1ENR1_TIM3EN,

		.PIN_config = {
		{ GPIOB, 5, 1 },
		{ GPIOB, 4, 1 },
		{ GPIOB, 0, 1 }
		},
		.num_of_pins = 3,
		.AF_number = 2
};





volatile uint32_t msTimer = 0;

void SETUP_RGB_LED(struct RGB_Light_Emitting_Diode *RGB_LED){
	SETUP_ALTERNATE_FUNCTION_PINS(RGB_LED);
	SETUP_PWN(RGB_LED);
}


int main(void)
{
	SETUP_RGB_LED(&PLAYER_LED);
	SETUP_RGB_LED(&SYSTEM_LED);





	//--initialize hardware------------------------------------------------------------------------
	CONFIGURE_STANDARD_LEDS(&GPIOA_LED_config);
	CONFIGURE_STANDARD_LEDS(&GPIOB_LED_config);
	CONFIGURE_STANDARD_LEDS(&GPIOC_LED_config);

	configureSysTickInterrupt();
	configure_Board_Button(); //configure the board button and its interrupt
	configure_external_switches();//configure the switches and their dedicated interrupts
	TIM3->CR1 |= TIM_CR1_CEN;//start timer
	TIM2->CR1 |= TIM_CR1_CEN;//start timer
	startSysTickTimer_MACRO;






		//uint32_t GPIOC_pins_AF[] = {8,9,6,5,7 , 10,12};//LEDs connected to port C's pins





	while (1)
	{

			//GPIOC->ODR |= (0x1 << 9);
			PROGRESS_BAR.leds[8].port->ODR |= (0x1 << PROGRESS_BAR.leds[8].pin);


		TIM3->CCR2 = 0;  // channel 2 duty
		TIM3->CCR1 = 0;  // channel 1 duty
		TIM3->CCR3 = 999;

		TIM2->CCR1 = RED_BUTTON.color_value; //r
			TIM2->CCR2 = GREEN_BUTTON.color_value; //g
			TIM2->CCR3 = BLUE_BUTTON.color_value; //b

		HANDLE_POST_DEBOUNCED_BUTTONS();

	}
}//end main


//================================================================================================
// EXTI4_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the BLUE button
//================================================================================================
void EXTI4_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 4)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 4);  // Clear interrupt flag
		  INITIATE_DEBOUNCE_PROTOCOL(&RED_BUTTON, msTimer);

	}
}






//================================================================================================
// EXTI1_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the GREEN button
//================================================================================================
void EXTI1_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 1)) { //if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 1);  // Clear interrupt flag
			  INITIATE_DEBOUNCE_PROTOCOL(&GREEN_BUTTON, msTimer);
	}
}

//================================================================================================
// EXTI0_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the RED button
//================================================================================================
void EXTI0_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 0)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 0);  // Clear interrupt flag
			  INITIATE_DEBOUNCE_PROTOCOL(&BLUE_BUTTON, msTimer);

	}
}

//================================================================================================
// EXTI15_10_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the SPECIAL button
//================================================================================================
void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 13)) { //if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 13);  // Clear interrupt flag
			INITIATE_DEBOUNCE_PROTOCOL(&SPECIAL_BUTTON, msTimer);


	}
}
//================================================================================================
// SysTick_Handler()
// @parm: none
// @return: none
// 		Increments msTimer variable and handles the debounce protocols for all buttons.
//================================================================================================
void SysTick_Handler(void)
{
	  msTimer++; //goes up every 1ms
	  HANDLE_BUTTON_DEBOUNCE_PROTOCOLS(msTimer); //validates the buttons

}

//================================================================================================
// HANDLE_POST_DEBOUNCED_BUTTON()
// @parm: currentTIME_ms - Current system time in milliseconds.
// @return: none
// 		Calls HANDLE_DEBOUNCED_BUTTON() for all buttons, executing any valid post-debounce
//		actions.
//================================================================================================
void HANDLE_POST_DEBOUNCED_BUTTONS(void){
	HANDLE_DEBOUNCED_BUTTON(&SPECIAL_BUTTON);
	HANDLE_DEBOUNCED_BUTTON(&BLUE_BUTTON);
	HANDLE_DEBOUNCED_BUTTON(&RED_BUTTON);
	HANDLE_DEBOUNCED_BUTTON(&GREEN_BUTTON);

}
//================================================================================================
// HANDLE_POST_DEBOUNCED_BUTTON()
// @parm: currentTIME_ms - Current system time in milliseconds.
// @return: none
// 		Runs the debounce protocol for all buttons.
//================================================================================================
void HANDLE_BUTTON_DEBOUNCE_PROTOCOLS(uint32_t currentTIME_ms){
	 DEBOUNCE_PROTOCOL(&BLUE_BUTTON, currentTIME_ms);
	 DEBOUNCE_PROTOCOL(&RED_BUTTON, currentTIME_ms);
	 DEBOUNCE_PROTOCOL(&GREEN_BUTTON,  currentTIME_ms);
	 DEBOUNCE_PROTOCOL(&SPECIAL_BUTTON,  currentTIME_ms);


}



/* unused
//================================================================================================
// EXTI2_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the YELLOW button
//================================================================================================
void EXTI2_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 2)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 2);  // Clear interrupt flag
		  if (gamestate == test){ //only begin process if game is in test phase
		  INITIATE_DEBOUNCE_PROTOCOL(&YELLOW_BUTTON, msTimer);
		  }
	}
}*/



























