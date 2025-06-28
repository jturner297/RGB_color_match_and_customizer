/*
 * game.c
 *
 *  Created on: Jun 25, 2025
 *      Author: Justin Turner
 */

#include "game.h"
#include "input.h"
#include "leds.h"

uint32_t color_match_indicators_on = 0;
uint32_t current_color;

struct color_channel red = {1,0};
struct color_channel green = {1,0};
struct color_channel blue = {1,0};

struct colors default_colors = {0,0,0};
struct colors previously_tested_color = {0,0,0};
struct colors preset_colors[NUMBER_OF_COLORS] = {
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
    //dlc
    {1000,  0,  200},     // hot pink
    {200,  800,  200},     // mint green
    {200,  0,  1000},     // dark purple
    {1000,  200,  0},     // orange yellow
    {0,  800,  200}     // turquoise
};



void START_GAME(uint32_t currentTIME_ms){
	 RESET_RGB_LED_CHANNELS();
	system_timeSTAMP = currentTIME_ms;
	 gamestate = preparing_game;
}
void GAME_OVER(void){
	RESET_RGB_LED_CHANNELS();
	CLEAR_ALL_PROGRESS();
	TURN_OFF_LED_BLOCK(&COLOR_MATCH_INDICATOR);

}


void CONFIGURE_RNG(void){
	RCC->AHB2ENR |= (0x1 << 18);
	RNG->CR |= (0x1 << 2);
}
uint32_t RNG_GetRandomNumber(void) {
    for (int i = 0; i < 10; i++) {
        if (RNG->SR & RNG_SR_DRDY) {
            return RNG->DR;
        }
    }
    return SysTick->VAL;
}


uint32_t color_history[COLOR_HISTORY_SIZE] = {0};

int is_in_history(uint32_t index) {
    for (int i = 0; i < COLOR_HISTORY_SIZE; i++) {
        if (color_history[i] == index) return 1;
    }
    return 0;
}

void update_history(uint32_t index) {
	static uint32_t history_index = 0;
	color_history[history_index] = index;
    history_index = (history_index + 1) % COLOR_HISTORY_SIZE;
}


void GENERATE_NEW_COLOR(void){
	static uint32_t new_color;
	do {
		new_color = RNG_GetRandomNumber() % NUMBER_OF_COLORS;
	} while (is_in_history(new_color)); //makes sure player does not get the same color as before
	update_history(new_color);
	current_color = new_color;
	SYSTEM_LED.RGB_channels = preset_colors[current_color];
}





void HANDLE_COLOR_MATCH_INDICATOR(void){
	if(color_match_indicators_on){
		if (PLAYER_LED.RGB_channels.red_value == SYSTEM_LED.RGB_channels.red_value){
				COLOR_MATCH_INDICATOR_LEDS[0].port->ODR |= (0x1 << 	COLOR_MATCH_INDICATOR_LEDS[0].pin);
			}
			else{
				COLOR_MATCH_INDICATOR_LEDS[0].port->ODR &= ~(0x1 << 	COLOR_MATCH_INDICATOR_LEDS[0].pin);
			}
			if (PLAYER_LED.RGB_channels.green_value == SYSTEM_LED.RGB_channels.green_value ){
				COLOR_MATCH_INDICATOR_LEDS[1].port->ODR |= (0x1 << 	COLOR_MATCH_INDICATOR_LEDS[1].pin);
			}else{
				COLOR_MATCH_INDICATOR_LEDS[1].port->ODR &= ~(0x1 << 	COLOR_MATCH_INDICATOR_LEDS[1].pin);
			}
			if (  PLAYER_LED.RGB_channels.blue_value == SYSTEM_LED.RGB_channels.blue_value){
				COLOR_MATCH_INDICATOR_LEDS[2].port->ODR |= (0x1 << 	COLOR_MATCH_INDICATOR_LEDS[2].pin);
			}
			else{
				COLOR_MATCH_INDICATOR_LEDS[2].port->ODR &= ~(0x1 << 	COLOR_MATCH_INDICATOR_LEDS[2].pin);
			}
	}
	else{
		TURN_OFF_LED_BLOCK(&COLOR_MATCH_INDICATOR);
	}

}


void HANDLE_WINNING_ANIMATION(void){
	static uint32_t blink_phase = 0;
	blink_phase^=1;
	if(blink_phase == 1){
		SYSTEM_LED.RGB_channels = preset_colors[current_color];
		PLAYER_LED.RGB_channels = default_colors;
	}else{
		SYSTEM_LED.RGB_channels = default_colors;
		PLAYER_LED.RGB_channels = preset_colors[current_color];
	}
}


void TURN_OFF_PROGRESS_BARS(struct button *button){
   	for (uint32_t i = 0; i < button->progress_bar->num_of_leds; i++) {
  	    button->progress_bar->leds[i].port->ODR &= ~(1 << button->progress_bar->leds[i].pin);
  	}
}

void TURN_OFF_ALL_PROGRESS_BARS(void){ //only used if the progress bar is togglable
	TURN_OFF_PROGRESS_BARS(&RED_BUTTON);
	TURN_OFF_PROGRESS_BARS(&GREEN_BUTTON);
	TURN_OFF_PROGRESS_BARS(&BLUE_BUTTON);
}




void CLEAR_PROGRESS(struct button *button){
	button->progress =0;
	TURN_OFF_PROGRESS_BARS(button); //just in case
}
void CLEAR_ALL_PROGRESS(void){
	CLEAR_PROGRESS(&RED_BUTTON);
	CLEAR_PROGRESS(&GREEN_BUTTON);
	CLEAR_PROGRESS(&BLUE_BUTTON);
}
void HANDLE_BUTTON_PROGRESS_BAR(struct button *button){
	uint32_t prev_led;
	prev_led = (button->progress >  0 )? button->progress-2: 4;
	(button->progress_bar->leds[prev_led].port->ODR) &= ~(0x1 <<  (button->progress_bar->leds[ prev_led].pin));
	(button->progress_bar->leds[ button->progress-1].port->ODR) |= (0x1 <<  (button->progress_bar->leds[button->progress-1].pin));

}




//#define fade_adjustment_level 15;

void HANDLE_COLOR_FADE (struct color_channel *color){
	   const uint32_t fade_adjustment_level = 15;
	if(color->is_going_up){
		color->level+=	fade_adjustment_level;//color->adjustment_level;
		}
	else{
		color->level-=	fade_adjustment_level;//color->adjustment_level;
		}
	if(	color->level >= 1000){
		color->is_going_up = 0;
	}
	else if(color->level <= 0){
		color->is_going_up = 1;
	}
}

void COLOR_FADE_EFFECT(void){
	static uint32_t fade_phase = 0;
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

}




