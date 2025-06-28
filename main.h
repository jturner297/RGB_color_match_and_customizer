#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "stm32l476xx.h"
#include <stddef.h>// For NULL definition

/**
**************************************************************************************************
* File: main.h
* Defines the constants, enums, structures, and function prototypes
**************************************************************************************************
*/
//Macros

/******************************
*			Timers
*******************************/
#define startSysTickTimer_MACRO (SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk)
#define startTIM2_MACRO (TIM2->CR1 |= (1 << 0)) //start timer
#define stopTIM2_MACRO (TIM2->CR1 &= ~(1 << 0)) //stop timer

#define SYS_CLK_FREQ 4000000// default frequency of the device = 4 MHZ
#define cntclk 1000

#define TWO_HZ_SPEED 2
#define THREE_HZ_SPEED 3
#define FOUR_HZ_SPEED 4
#define SIX_HZ_SPEED 6

#define SYSTICK_1MS ((SYS_CLK_FREQ / 1000) - 1)


/******************************
*			Buttons
*******************************/


/******************************
*			 LEDS
*******************************/



#define PATTERN_SELECT_LED_ON(level) SELECTED_PATTERN_INDICATOR_LEDS[level].port->ODR |= (0x1 << SELECTED_PATTERN_INDICATOR_LEDS[level].pin)
#define PATTERN_SELECT_LED_OFF(level) SELECTED_PATTERN_INDICATOR_LEDS[level].port->ODR &= ~(0x1 << SELECTED_PATTERN_INDICATOR_LEDS[level].pin)

#define SPECIAL_LED_ON (GPIOA->ODR |= (0x1 << 5))
#define SPECIAL_LED_OFF (GPIOA->ODR &= ~(0x1 << 5))


/******************************
*		Time keeping
*******************************/
#define START_SCREEN_TIME 2000 //duration of the start screen state
#define PATTERN_RELOAD_TIME 800 //time it takes to start displaying pattern


#define SYSTEM_MODE_SWITCH_DURATION 1000
#define FAIL_SCREEN_DURATION 1500 //time the fail screen state is active
#define CELEBRATION_DURATION 3500 //time the celebration screen state is active
#define NEXT_GAME_COOLDOWN_DURATION 1000
#define LED_TOGGLE_TIME 250 //time an LED stays on when one of the 4 standard buttons is pressed.

#define TIME_500MS 500
#define TIME_10_SEC 10000






//enums
enum button_modes {count_up, count_down};

enum gamestates {start_screen_display, preparing_game, test, game_cancelled, entering_win_screen, win_screen};
	//fail_screen, celebration_screen, wait_for_next_game , system_mode_switch};

enum identifications {R, G, B, SPECIAL, ALL, UNASSIGNED_ID};
enum game_modes {color_match, color_customizer};



//Structures

struct color_channel{ //for strobing effect
	uint32_t is_going_up;
	int32_t level;
};

struct button{
	volatile uint32_t press_pending; //flag pending button press
	volatile uint32_t debounce_counter; //counter for debouncing
	volatile uint32_t press_ready; //flag for valid button press

	volatile uint32_t *linked_color_value;

	volatile uint32_t progress;
	struct LED_block *progress_bar;

	volatile uint32_t is_held;
	volatile uint32_t held_duration;
	volatile uint32_t pressTIMESTAMP;

	enum identifications ID; //gives the button an associated ID: B, G, Y, R, SPECIAL
	GPIO_TypeDef *port; //GPIOx
	uint32_t pin;
    IRQn_Type irq_number;
};

struct GPIO_config{
	GPIO_TypeDef *port; //GPIOx
	uint32_t pin; //Pin number
	uint32_t clock_index;
};
struct colors{
	volatile uint32_t red_value;
	volatile uint32_t green_value;
	volatile uint32_t blue_value;
};

struct RGB_Light_Emitting_Diode{
	TIM_TypeDef *TIMx; //GPIOx
	 uint32_t TIMx_clk_enable_mask;
	struct GPIO_config PIN_config[3];
	uint32_t num_of_pins;
	uint32_t AF_number;
	struct colors RGB_channels;

};

struct LED_block{
	struct GPIO_config* leds; //GPIO information for the LEDs in the block
	uint32_t num_of_leds; //number of LEDs present in the block

};





struct GPIO_LED_config{//for configuring every standard LED
	GPIO_TypeDef *port; //GPIOx
	uint32_t pins[15]; //Pin number
	uint32_t num_of_pins;

};

struct GAME{//attach to RGB LED struct?
	volatile int32_t red_color_value;
	volatile int32_t green_color_value;
	volatile int32_t blue_color_value;
};



//Function Prototypes

//extern variables
extern enum system_states button_mode;
extern enum gamestates gamestate;
extern volatile uint32_t msTimer;
extern uint32_t system_timeSTAMP;












#endif /* MAIN_H */
