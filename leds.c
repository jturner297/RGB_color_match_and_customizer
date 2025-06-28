#include "leds.h"

/**************************************************************************************************
* File: leds.c
* Defines functions used for LED control and configuration.
*************************************************************************************************/
struct RGB_Light_Emitting_Diode PLAYER_LED = {
		.TIMx = TIM2,
		.TIMx_clk_enable_mask = RCC_APB1ENR1_TIM2EN,

		.PIN_config = {
		{ GPIOA, 0, 0 },
		{ GPIOB, 3, 1 },
		{ GPIOB, 10, 1 }
			},
			.num_of_pins = 3,
			.AF_number = 1,
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
		.AF_number = 2,
};




struct GPIO_config RED_PROGRESS_BAR_LEDS[]={
		{GPIOA, 10},
		{GPIOC, 4},
		{GPIOB, 13},
		{GPIOB, 14},
	    {GPIOB, 15},
};
struct LED_block RED_PROGRESS_BAR ={
	.leds = RED_PROGRESS_BAR_LEDS , //GPIO information for the LEDs in the block
	 .num_of_leds = 5 //number of LEDs present in the block

};

struct GPIO_config GREEN_PROGRESS_BAR_LEDS[]={
		{GPIOB, 11},
		{GPIOA, 7},
		{GPIOB, 12},
		{GPIOA, 6},
	    {GPIOA, 11},
};
struct LED_block GREEN_PROGRESS_BAR ={
	.leds = GREEN_PROGRESS_BAR_LEDS , //GPIO information for the LEDs in the block
	 .num_of_leds = 5 //number of LEDs present in the block

};

struct GPIO_config BLUE_PROGRESS_BAR_LEDS[]={
	    {GPIOA, 12}, //blue
	    {GPIOB, 9},
	    {GPIOC, 5},
	    {GPIOB, 8},
	    {GPIOC, 6},
};

struct LED_block BLUE_PROGRESS_BAR ={
	.leds = BLUE_PROGRESS_BAR_LEDS , //GPIO information for the LEDs in the block
	 .num_of_leds = 5 //number of LEDs present in the block

};

struct GPIO_config COLOR_MATCH_INDICATOR_LEDS[]={

		{GPIOC, 11}, //red
		{GPIOC, 10}, //green
		{GPIOC, 12}, //blue
};
struct LED_block COLOR_MATCH_INDICATOR ={
	.leds = COLOR_MATCH_INDICATOR_LEDS , //GPIO information for the LEDs in the block
	 .num_of_leds = 3 //number of LEDs present in the block

};



struct GPIO_LED_config GPIOA_LED_config={
		.port = GPIOA,
		.pins = {12, 7,6,11, 10, 15}, //Pin number
		.num_of_pins = 6,
};
struct GPIO_LED_config GPIOB_LED_config={
		.port = GPIOB,
		.pins = {8,9 ,11, 12, 13,14,15}, //Pin number
		.num_of_pins =7,
};
struct GPIO_LED_config GPIOC_LED_config={
		.port = GPIOC,
		.pins = {8,9,6,5, 4, 11, 10, 12}, //Pin number
		.num_of_pins = 8,
};





void CONFIGURE_STANDARD_LEDS(struct GPIO_LED_config *GPIOx_LED_config){

	for (uint32_t i = 0; i < GPIOx_LED_config->num_of_pins; i++){ //runs 3 times
			if (GPIOx_LED_config->port == GPIOA){
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
			}
	    	else if (GPIOx_LED_config->port == GPIOB){
	    		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	    	}
	    	else if (GPIOx_LED_config->port == GPIOC){
	    		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	    	}
			//configure pins for output  mode
			GPIOx_LED_config->port->MODER &= ~(0x3 << (2 * GPIOx_LED_config->pins[i])); //clear
			GPIOx_LED_config->port->MODER |= (0x1 << (2 *GPIOx_LED_config->pins[i])); //10
			GPIOx_LED_config->port->OTYPER &= ~(0x1 << GPIOx_LED_config->pins[i]);
			GPIOx_LED_config->port->OSPEEDR &= ~(0x3 << (2 *GPIOx_LED_config->pins[i])); //10
			GPIOx_LED_config->port->PUPDR &= ~(0x3 << (2 *GPIOx_LED_config->pins[i])); //10
		}
}

void SETUP_ALTERNATE_FUNCTION_PINS(struct RGB_Light_Emitting_Diode *RGB_LED){
	uint32_t AFR_index, shift;


	for (uint32_t i = 0; i < RGB_LED->num_of_pins; i++){ //runs 3 times
		//RCC->AHB2ENR |= (0x1 << RGB_LED->PIN_config[i].clock_index);

		if (RGB_LED->PIN_config[i].port == GPIOA){
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		}
    	else if (RGB_LED->PIN_config[i].port == GPIOB){
    		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    	}
    	else if (RGB_LED->PIN_config[i].port == GPIOC){
    		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    	}

		//configure pins for alternating function mode
		RGB_LED->PIN_config[i].port->MODER &= ~(0x3 << (2 * RGB_LED->PIN_config[i].pin)); //clear
		RGB_LED->PIN_config[i].port->MODER |= (0x2 << (2 *RGB_LED->PIN_config[i].pin)); //10
		//port->OTYPER &= ~(0x1 << pins[i]);
		RGB_LED->PIN_config[i].port->OSPEEDR &= ~(0x3 << (2 * RGB_LED->PIN_config[i].pin)); //clear
		RGB_LED->PIN_config[i].port->OSPEEDR |= (0x1 << (2 * RGB_LED->PIN_config[i].pin)); //clear
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

	RGB_LED->TIMx->CR1 |= (0x1 << 0); //start timer
}

void SETUP_RGB_LED(struct RGB_Light_Emitting_Diode *RGB_LED){
	SETUP_ALTERNATE_FUNCTION_PINS(RGB_LED);
	SETUP_PWN(RGB_LED);
}

void CONFIGURE_ALL_LEDS(void){
	SETUP_RGB_LED(&PLAYER_LED);
	SETUP_RGB_LED(&SYSTEM_LED);

	CONFIGURE_STANDARD_LEDS(&GPIOA_LED_config);
	CONFIGURE_STANDARD_LEDS(&GPIOB_LED_config);
	CONFIGURE_STANDARD_LEDS(&GPIOC_LED_config);
}

void RESET_RGB_CHANNEL(struct colors *channels) {
    channels->red_value = 0;
    channels->green_value = 0;
    channels->blue_value = 0;
}
//easy way to specifaclly update values
void UPDATE_RGB_CHANNEL_VALUES(struct RGB_Light_Emitting_Diode *RGB_LED, int32_t r_value, int32_t g_value, int32_t b_value){
	RGB_LED->RGB_channels.red_value = r_value;
	RGB_LED->RGB_channels.green_value = g_value;
	RGB_LED->RGB_channels.blue_value = b_value;
}
void RESET_RGB_LED_CHANNELS(void){ //zeroes out all channels
	    RESET_RGB_CHANNEL(&SYSTEM_LED.RGB_channels);
	    RESET_RGB_CHANNEL(&PLAYER_LED.RGB_channels);

}

void DISPLAY_RGB_LED(struct RGB_Light_Emitting_Diode *RGB_LED){
    RGB_LED->TIMx->CCR1 = RGB_LED->RGB_channels.red_value;
    RGB_LED->TIMx->CCR2 = RGB_LED->RGB_channels.green_value;
    RGB_LED->TIMx->CCR3 = RGB_LED->RGB_channels.blue_value;
}
void TURN_OFF_LED_BLOCK(struct LED_block *LED_group){
	for (uint32_t i = 0; i < LED_group->num_of_leds; i++) {
		LED_group->leds[i].port->ODR &= ~(1 << LED_group->leds[i].pin);
  	}
}



