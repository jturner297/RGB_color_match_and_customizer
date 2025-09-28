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

//Macros/Constants

/******************************
*		Time keeping
*******************************/
#define START_SCREEN_TIME 2000 //duration of the start screen state


#define SYSTEM_MODE_SWITCH_DURATION 1000
#define FAIL_SCREEN_DURATION 1500 //time the fail screen state is active
#define CELEBRATION_DURATION 3500 //time the celebration screen state is active
#define NEXT_GAME_COOLDOWN_DURATION 1000
#define LED_TOGGLE_TIME 250 //time an LED stays on when one of the 4 standard buttons is pressed.

#define TIME_500MS 500
#define TIME_1000MS 1000
#define TIME_1500MS 1500
#define TIME_2000MS 2000
#define TIME_3000MS 3000
#define TIME_3600MS 3600

#define TIME_10_SEC 10000


//enums
enum system_states {start_screen_display, in_color_match, in_color_customizer};
enum game_modes {color_match, color_customizer };

enum color_match_states {preparing_game, test, skip_test, entering_win_screen, win_screen}; //states associated with color match mode
enum color_customizer_states {preparing, free_play}; //states associated with color customizer mode

enum identifications {R, G, B, SPECIAL, ALL, UNASSIGNED_ID};





//Structures
struct color_channel{ //only used for strobing effect
	uint32_t is_going_up; //flag that denotes when a channel is increasing
	int32_t level; //denotes the level/value the channel is at
};

struct button{ //button attributes
	volatile uint32_t press_pending; //flag pending button press
	volatile uint32_t debounce_counter; //counter for debouncing
	volatile uint32_t press_ready; //flag for valid button press

	volatile int32_t *linked_color_value; //each standard button has a linked color/channel: RED, GREEN, BLUE

	volatile uint32_t progress; //each press increases the progress/brightness level of the LED
	struct LED_block *progress_bar;//each standard button is linked to a progress bar, which tracks the progress/brightness level

	volatile uint32_t is_held; //flag the denotes when a button is held
	volatile uint32_t held_duration; //holds the calculated hold duration
	volatile uint32_t pressTIMESTAMP; //marks when the button is officially pressed

	enum identifications ID; //gives the button an associated ID: R, G, B, SPECIAL
	GPIO_TypeDef *port; //GPIOx
	uint32_t pin; //associated GPIO pin num
    IRQn_Type irq_number; //associated irq number
};

struct GPIO_config{ //GPIO information for LED control
	GPIO_TypeDef *port; //GPIOx
	uint32_t pin; //Pin number
	uint32_t clock_index; //0 for A, 1 for B, etc... (only needed for RGB LED)
};
struct colors{ //used for RGB channels
	volatile int32_t red_value;  //red channel value
	volatile int32_t green_value; //green channel value
	volatile int32_t blue_value; //blue channel value
};

struct RGB_Light_Emitting_Diode{ //RGB LED attributes
	TIM_TypeDef *TIMx; //TIMx - each RGB LED has an associated timer for PWM control
	 uint32_t TIMx_clk_enable_mask; //for configuration purposes
	struct GPIO_config PIN_config[3]; //each RGB LED uses 3 GPIO pin configurations
	uint32_t num_of_pins; //num_of_pins for GPIO config
	uint32_t AF_number; //Alternate function number
	struct colors RGB_channels; //each RGB LED has dedicated channels for each color with each color having it's own value

};

struct LED_block{ //LED cluster attributes
	struct GPIO_config* leds; //GPIO information for the LEDs in the block
	uint32_t num_of_leds; //number of LEDs present in the block

};


struct GPIO_LED_config{//only used for configuring every standard LED
	GPIO_TypeDef *port; //GPIOx
	uint32_t pins[15]; //Pin number
	uint32_t num_of_pins; //number of pins in cluster

};



//Function Prototypes
void RESET_AND_RETURN_TO_START(void);

//extern variables
extern enum color_match_states gamestate;
extern enum color_customizer_states free_play_state;
extern enum system_states system_state;
extern enum game_modes game_mode_selected;
extern volatile uint32_t msTimer;
extern uint32_t system_timeSTAMP;
extern uint32_t game_mode_switched;
extern volatile  uint32_t update_LED_animation;
extern volatile uint32_t accept_input;


#endif /* MAIN_H */
