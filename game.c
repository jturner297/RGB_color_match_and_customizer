
#include "game.h"
#include "input.h"
#include "leds.h"


/**************************************************************************************************
* File: game.c
* Contains functions relating to game assets and logic
*************************************************************************************************/

uint32_t guided_mode_on = 0;
uint32_t current_color;

struct color_channel red = {1,0};
struct color_channel green = {1,0};
struct color_channel blue = {1,0};




struct colors default_colors = {0,0,0}; // no/clear colors
struct colors previously_tested_color = {0,0,0};//stores the color channel values that the player was just tested on
												//used in win animation
struct colors preset_colors[NUMBER_OF_COLORS] = {//colors used in match mode
    {1000,    0,    0},   // Red
    {   0, 1000,    0},   // Green
    {   0,    0, 1000},   // Blue
    {1000, 1000,    0},   // Green Yellow
    {   0, 1000, 1000},   // Cyan
    {1000,    0, 1000},   // Magenta
    {800,  400,    0},    // Bright Yellow
    {600,    0,  600},    // Purple
    {200,  600, 1000},    // Sky blue
    {1000,  1000,  1000} ,    // white
    //expansion pack
    {1000,  0,  200},     // hot pink
    {200,  800,  200},     // mint green
    {200,  0,  1000},     // dark purple
    {1000,  200,  0},     // orange yellow
    {0,  800,  200}     // turquoise
};


//================================================================================================
// START_COLOR_CUSTOMIZER_GAME()
// @parm: currentTIME_ms = Current system time in milliseconds.
// @return: none
// 		Starts the color match game
//================================================================================================
void START_COLOR_MATCH_GAME(uint32_t currentTIME_ms){
	 RESET_RGB_LED_CHANNELS();
	system_timeSTAMP = currentTIME_ms;
	system_state = in_color_match;
	 gamestate = preparing_game;
}
//================================================================================================
// START_COLOR_CUSTOMIZER_GAME()
// @parm: currentTIME_ms = Current system time in milliseconds.
// @return: none
// 		Starts the color customizer game
//================================================================================================
void START_COLOR_CUSTOMIZER_GAME(uint32_t currentTIME_ms){
	RESET_RGB_LED_CHANNELS();
	system_timeSTAMP = currentTIME_ms;
	system_state = in_color_customizer;
	free_play_state = preparing;

}
//================================================================================================
// CLEAR_GAME_HUD()
// @parm: none
// @return: none
// 		Clears all visual display elements
//================================================================================================
void CLEAR_GAME_HUD(void){
	RESET_RGB_LED_CHANNELS();
	RESET_ALL_BRIGHTNESS();
	TURN_OFF_LED_BLOCK(&GUIDED_MODE_INDICATOR);

}

//================================================================================================
// ENTER_A_GAME_MODE()
// @parm: none
// @return: none
// 		Enters a game mode based on the game_mode_selected variable
//================================================================================================
void ENTER_A_GAME_MODE(void){
	update_LED_animation = 0; //The system is leaving the start screen, so this flag should be cleared
	RESET_RGB_LED_CHANNELS(); //safely resets / turns off RGB LEDs to prepare for game
	switch(game_mode_selected){
	case color_match:
		START_COLOR_MATCH_GAME(msTimer);
		break;
	case color_customizer:
		START_COLOR_CUSTOMIZER_GAME(msTimer);
	break;
	}//end switch
}
//================================================================================================
// CONFIGURE_RNG()
// @parm: none
// @return: none
// 		Configures hardware RNG
//================================================================================================
void CONFIGURE_RNG(void){
	RCC->AHB2ENR |= (0x1 << 18); //enables the clock to the RNG peripheral
	RNG->CR |= (0x1 << 2);  //starts the RNG so that random numbers can be generated from it
}

//================================================================================================
// RNG_GetRandomNumber()
// @parm: none
// @return: a random number from the hardware RNG, or a fallback value
// 		Attempts to retrieve a random number from the hardware RNG peripheral. Tries up to 10 times
//		to check if a random number is ready. If successful, returns the random number. If not,
//		returns the current msTimer value as a fallback.
//================================================================================================
uint32_t RNG_GetRandomNumber(void) {
    for (int i = 0; i < 10; i++) { //Tries up to 10 times to get a valid random number from RNG:
        if (RNG->SR & RNG_SR_DRDY) { // if a new random number is ready...
            return RNG->DR; //return the ready random value
        }
    }
    return msTimer; //Fallback if RNG fails or is too slow:
}


uint32_t color_history[COLOR_HISTORY_SIZE] = {0}; //array that holds color history
//================================================================================================
// is_in_history()
// @parm: index
// @return: 1 if index is found in color_history[], 0 otherwise
// 		Iterates over color_history[], an array of previously used colors.
//		If the index is found anywhere in the history, it returns 1 (true).
//		If it loops through the entire array and doesn’t find the index, it returns 0 (false).
//================================================================================================
int is_in_history(uint32_t index) {
    for (int i = 0; i < COLOR_HISTORY_SIZE; i++) {
        if (color_history[i] == index) return 1;
    }
    return 0;
}

//================================================================================================
// update_history()
// @parm: index = the new color index to store in history
// @return: none
// 		Manages an array of previously used colors by inserting a new index
//		into the color_history[] array and advancing the position.
//================================================================================================
void update_history(uint32_t index) {
	static uint32_t history_index = 0;
	color_history[history_index] = index;
    history_index = (history_index + 1) % COLOR_HISTORY_SIZE;
}

//================================================================================================
// GENERATE_NEW_COLOR()
// @parm: none
// @return: none
// 		Generates a new random color. Uses previous history to make sure it is not the same as
//		the previous color
//================================================================================================
void GENERATE_NEW_COLOR(void){
	static uint32_t new_color;
	do { //keep finding new colors
		new_color = RNG_GetRandomNumber() % NUMBER_OF_COLORS;
	} while (is_in_history(new_color)); //until the new color is not in the history
	update_history(new_color); //add new color to the history
	current_color = new_color; //overwrite the current color with the newly generated color
	SYSTEM_LED.RGB_channels = preset_colors[current_color]; //set the SYSTEM RGB LED to the new color
}




//================================================================================================
// HANDLE_GUIDED_MODE_INDICATOR()
// @parm: none
// @return: none
// 		Compares each RGB channel of the player's LED against the system's LED. If a channel matches,
//		the corresponding guided mode indicator LED is turned on; otherwise, it is turned off.
//		If guided mode is disabled, all indicator LEDs are turned off.
//================================================================================================
void HANDLE_GUIDED_MODE_INDICATOR(void){
	if(guided_mode_on){ //if guided mode is on...

		if (PLAYER_LED.RGB_channels.red_value == SYSTEM_LED.RGB_channels.red_value){ //red match
			GUIDED_MODE_INDICATOR_LEDS[0].port->ODR |= (0x1 << 	GUIDED_MODE_INDICATOR_LEDS[0].pin);
		}
		else{
			GUIDED_MODE_INDICATOR_LEDS[0].port->ODR &= ~(0x1 << 	GUIDED_MODE_INDICATOR_LEDS[0].pin);
		}

		if (PLAYER_LED.RGB_channels.green_value == SYSTEM_LED.RGB_channels.green_value ){ //green match
				GUIDED_MODE_INDICATOR_LEDS[1].port->ODR |= (0x1 << 	GUIDED_MODE_INDICATOR_LEDS[1].pin);
		}else{
			GUIDED_MODE_INDICATOR_LEDS[1].port->ODR &= ~(0x1 << 	GUIDED_MODE_INDICATOR_LEDS[1].pin);
		}

		if ( PLAYER_LED.RGB_channels.blue_value == SYSTEM_LED.RGB_channels.blue_value){ //blue match
			GUIDED_MODE_INDICATOR_LEDS[2].port->ODR |= (0x1 << 	GUIDED_MODE_INDICATOR_LEDS[2].pin);
		}
		else{
			GUIDED_MODE_INDICATOR_LEDS[2].port->ODR &= ~(0x1 << 	GUIDED_MODE_INDICATOR_LEDS[2].pin);
		}

	}
	else{ //if guided mode is off
		TURN_OFF_LED_BLOCK(&GUIDED_MODE_INDICATOR);
	}

}
//================================================================================================
// HANDLE_STEP_SIZE_INDICATOR()
// @parm: none
// @return: none
// 		Used in color customizer mode: this function handles led on/off logic for the step size
//		indicator.
//================================================================================================
void HANDLE_STEP_SIZE_INDICATOR(void){

	GUIDED_MODE_INDICATOR_LEDS[custom_step_size].port->ODR |= (0x1 << 	GUIDED_MODE_INDICATOR_LEDS[custom_step_size].pin);

	//calculate which LED to turn off
	static uint32_t prevLED;
	//if the custom step size is greater than 0, then the previous LED is (custom_step_size - 1)
	//else it is 0, so the previous LED in the array is 2
	prevLED = (custom_step_size > 0)? 	(custom_step_size - 1): 2;

	GUIDED_MODE_INDICATOR_LEDS[prevLED].port->ODR &= ~(0x1 << 	GUIDED_MODE_INDICATOR_LEDS[prevLED].pin);

}


//================================================================================================
// HANDLE_STEP_SIZE_INDICATOR()
// @parm: none
// @return: none
// 		Used in conjunction with SysTick. Handles the winning animation.
//================================================================================================
void HANDLE_WINNING_ANIMATION(void){
	static uint32_t blink_phase = 0;
	blink_phase^=1; //toggle blink phase
	if(blink_phase == 1){
		SYSTEM_LED.RGB_channels = preset_colors[current_color]; //show color
		PLAYER_LED.RGB_channels = default_colors; //clear
	}else{
		SYSTEM_LED.RGB_channels = default_colors; //clear
		PLAYER_LED.RGB_channels = preset_colors[current_color]; //show color
	}
}

//================================================================================================
// TURN_OFF_PROGRESS_BARS()
// @parm: button  = Pointer to the button struct representing the button
// @return: none
// 		Turns off a progress bar
//================================================================================================
void TURN_OFF_PROGRESS_BARS(struct button *button){
   	for (uint32_t i = 0; i < button->progress_bar->num_of_leds; i++) {
  	    button->progress_bar->leds[i].port->ODR &= ~(1 << button->progress_bar->leds[i].pin);
  	}
}

//================================================================================================
// TURN_OFF_ALL_PROGRESS_BARS()
// @parm: none
// @return: none
// 		Turns off all progress bars at once. Unused since, progress bars are not togglable and are
//		mandatory now
//================================================================================================
void TURN_OFF_ALL_PROGRESS_BARS(void){
	TURN_OFF_PROGRESS_BARS(&RED_BUTTON);
	TURN_OFF_PROGRESS_BARS(&GREEN_BUTTON);
	TURN_OFF_PROGRESS_BARS(&BLUE_BUTTON);
}



//================================================================================================
// RESET_BRIGHTNESS()
// @parm: button  = Pointer to the button struct representing the button
// @return: none
// 		Resets the button brightness progress
//================================================================================================
void RESET_BRIGHTNESS(struct button *button){
	 *(button->linked_color_value) = 0; //reset individual color back to 0
	button->progress =0;
	TURN_OFF_PROGRESS_BARS(button); //just in case
}

//================================================================================================
// RESET_ALL_BRIGHTNESS()
// @parm: none
// @return: none
// 		Resets the brightness progress for all buttons
//================================================================================================
void RESET_ALL_BRIGHTNESS(void){
	RESET_BRIGHTNESS(&RED_BUTTON);
	RESET_BRIGHTNESS(&GREEN_BUTTON);
	RESET_BRIGHTNESS(&BLUE_BUTTON);
}

void HANDLE_BUTTON_PROGRESS_BAR(struct button *button){
	uint32_t prev_led;
	prev_led = (button->progress >  0 )? button->progress-2: 4;
	(button->progress_bar->leds[prev_led].port->ODR) &= ~(0x1 <<  (button->progress_bar->leds[ prev_led].pin));
		(button->progress_bar->leds[button->progress-1].port->ODR) |= (0x1 <<  (button->progress_bar->leds[button->progress-1].pin));



}


void HANDLE_ALL_BUTTON_PROGRESS_BARS(void){
	HANDLE_BUTTON_PROGRESS_BAR(&RED_BUTTON);
	HANDLE_BUTTON_PROGRESS_BAR(&BLUE_BUTTON);
	HANDLE_BUTTON_PROGRESS_BAR(&GREEN_BUTTON);

}



//================================================================================================
// HANDLE_COLOR_FADE()
// @parm: none
// @return: none
// 		Handles the color fading for the color fade effect.
//================================================================================================
void HANDLE_COLOR_FADE (struct color_channel *color){
	   const uint32_t fade_adjustment_level = 15;
	if(color->is_going_up){
		color->level+=	fade_adjustment_level;
		}
	else{
		color->level-=	fade_adjustment_level;
		}
	if(	color->level >= 1000){
		color->is_going_up = 0;
	}
	else if(color->level <= 0){
		color->is_going_up = 1;
	}
}

uint32_t fade_phase = 0;

//================================================================================================
// COLOR_FADE_EFFECT()
// @parm: none
// @return: none
// 		Called during the start screen. If color customizer is selected, it will play a rainbow fade
//		animation. It emphasizes the endless amount of colors the player can create.
//================================================================================================
void COLOR_FADE_EFFECT(void){

	switch (fade_phase) {
	case 0: // red up, green up
		HANDLE_COLOR_FADE(&red);
		HANDLE_COLOR_FADE(&green);
		blue.level = 0;
		if (red.level >= 1000 && green.level >= 1000) fade_phase++;
		break;
	case 1: // red down, green up
		red.is_going_up = 0; HANDLE_COLOR_FADE(&red);
		green.is_going_up = 1; HANDLE_COLOR_FADE(&green);
		blue.level = 0;
		if (red.level == 0 && green.level >= 1000) fade_phase++;
		break;
	case 2: // green up, blue up
		HANDLE_COLOR_FADE(&green);
		HANDLE_COLOR_FADE(&blue);
		red.level = 0;
		if (green.level >= 1000 && blue.level >= 1000) fade_phase++;
		break;
	case 3: // green down, blue up
		green.is_going_up = 0; HANDLE_COLOR_FADE(&green);
		blue.is_going_up = 1; HANDLE_COLOR_FADE(&blue);
		red.level = 0;
		if (green.level == 0 && blue.level >= 1000) fade_phase++;
		break;
	case 4: // blue up, red up
		HANDLE_COLOR_FADE(&blue);
		HANDLE_COLOR_FADE(&red);
		green.level = 0;
		if (blue.level >= 1000 && red.level >= 1000) fade_phase++;
		break;
	case 5: // blue down, red up
		blue.is_going_up = 0; HANDLE_COLOR_FADE(&blue);
		red.is_going_up = 1; HANDLE_COLOR_FADE(&red);
		green.level = 0;
		if (blue.level == 0 && red.level >= 1000) fade_phase = 0;
		break;
	}
	UPDATE_RGB_CHANNEL_VALUES(&SYSTEM_LED, red.level, green.level, blue.level);
	UPDATE_RGB_CHANNEL_VALUES(&PLAYER_LED, red.level, green.level, blue.level);
	if (game_mode_switched == 1){
		RESET_FADE_EFFECT();
	}

}
 uint32_t tick_phase = 0;

 //================================================================================================
 // COLOR_TICK_EFFECT()
 // @parm: none
 // @return: none
 // 		Called during the start screen. If color match is selected, it will play an animation that
 //			shows all the available/preset colors in the game.
 //================================================================================================
void COLOR_TICK_EFFECT(void){
	tick_phase = (tick_phase+1)%NUMBER_OF_COLORS;
	SYSTEM_LED.RGB_channels = preset_colors[tick_phase];
	PLAYER_LED.RGB_channels  =preset_colors[tick_phase];
	if (game_mode_switched == 1){
		RESET_TICK_EFFECT();
	}


}

//================================================================================================
// RESET_TICK_EFFECT()
// @parm: none
// @return: none
// 		Fully resets tick effect by setting tick phase back to 0
//================================================================================================
void RESET_TICK_EFFECT(void){
		tick_phase = 0;
	}

//================================================================================================
// RESET_FADE()
// @parm: none
// @return: none
// 		Resets the fade settings for a color back to default
//================================================================================================
void RESET_FADE(struct color_channel *color){
	color->is_going_up = 1;
	color->level = 0;
}
//================================================================================================
// RESET_FADE_EFFECT()
// @parm: none
// @return: none
// 		Fully resets the fade effect by resetting the fade settings for all colors
//================================================================================================
void RESET_FADE_EFFECT(void){
		fade_phase = 0;
			RESET_FADE(&red);
			RESET_FADE(&green);
			RESET_FADE(&blue);
	}


//================================================================================================
// HANDLE_LED_ANIMATION()
// @parm: none
// @return: none
// 		Called within the SysTick Handler. Handles LED animations
//================================================================================================
void HANDLE_LED_ANIMATION (void){
	  if(system_state == start_screen_display){ //if the system is in the start screen....
		 static uint32_t animation_speed;
		 //determine which animation speed to use based on the selected game mode
		 animation_speed = (game_mode_selected == color_match) ? TICK_EFFECT_SPEED: FADE_EFFECT_SPEED;
		 if(msTimer%animation_speed == 0){
			 update_LED_animation = 1;
		  }
	  }
	  else if(system_state == in_color_match && gamestate == win_screen){ //if system is in a win state....
		  if(msTimer%WIN_ANIMATION_SPEED == 0){
			  update_LED_animation = 1;
		  }
	  }
}
