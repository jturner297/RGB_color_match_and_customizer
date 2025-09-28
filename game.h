

#ifndef GAME_H_
#define GAME_H_
#include "main.h"

/**************************************************************************************************
* File: input.h
* Header file for game.c
*************************************************************************************************/

#define FADE_EFFECT_SPEED 50
#define TICK_EFFECT_SPEED 300
#define WIN_ANIMATION_SPEED 300

#define NUMBER_OF_COLORS 15
#define COLOR_HISTORY_SIZE (10)
#define MAX_AMOUNT_OF_GAME_COLORS 30

extern uint32_t guided_mode_on;


extern struct color_channel red ;
extern struct color_channel green;
extern struct color_channel blue;

extern struct colors default_colors;
extern struct colors preset_colors[NUMBER_OF_COLORS];
extern struct colors previously_tested_color;

void START_COLOR_MATCH_GAME(uint32_t currentTIME_ms);
void START_COLOR_CUSTOMIZER_GAME(uint32_t currentTIME_ms);
void CLEAR_GAME_HUD(void);
void ENTER_A_GAME_MODE(void);


void HANDLE_COLOR_STROBE (struct color_channel *color);
void HANDLE_WINNING_ANIMATION(void);
void RESET_BRIGHTNESS(struct button *button);
void RESET_ALL_BRIGHTNESS(void);



void HANDLE_BUTTON_PROGRESS_BAR(struct button *button);
void HANDLE_ALL_BUTTON_PROGRESS_BARS(void);


void COLOR_FADE_EFFECT(void);
void HANDLE_GUIDED_MODE_INDICATOR(void);

void CONFIGURE_RNG(void);
uint32_t RNG_GetRandomNumber(void);
void COLOR_TICK_EFFECT(void);



int is_in_history(uint32_t index);
void update_history(uint32_t index);


void GENERATE_NEW_COLOR(void);
void TURN_OFF_PROGRESS_BARS(struct button *button);
void TURN_OFF_ALL_PROGRESS_BARS(void);
void HANDLE_STEP_SIZE_INDICATOR(void);


void RESET_FADE_EFFECT(void);
void RESET_TICK_EFFECT(void);

void HANDLE_LED_ANIMATION (void);


#endif /* GAME_H_ */
