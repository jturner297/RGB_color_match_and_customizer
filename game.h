/*
 * game.h
 *
 *  Created on: Jun 25, 2025
 *      Author: Justin Turner
 */

#ifndef GAME_H_
#define GAME_H_
#include "main.h"


#define NUMBER_OF_COLORS 15
#define COLOR_HISTORY_SIZE (10)
#define MAX_AMOUNT_OF_GAME_COLORS 30

extern uint32_t color_match_indicators_on;


extern struct color_channel red ;
extern struct color_channel green;
extern struct color_channel blue;

extern struct colors default_colors;
extern struct colors preset_colors[NUMBER_OF_COLORS];
extern struct colors previously_tested_color;

void START_GAME(uint32_t currentTIME_ms);
void GAME_OVER(void);



void HANDLE_COLOR_STROBE (struct color_channel *color);
void HANDLE_WINNING_ANIMATION(void);
void CLEAR_PROGRESS(struct button *button);
void CLEAR_ALL_PROGRESS(void);
void HANDLE_BUTTON_PROGRESS_BAR(struct button *button);
void COLOR_FADE_EFFECT(void);
void HANDLE_COLOR_MATCH_INDICATOR(void);

void CONFIGURE_RNG(void);
uint32_t RNG_GetRandomNumber(void);




int is_in_history(uint32_t index);
void update_history(uint32_t index);


void GENERATE_NEW_COLOR(void);
void TURN_OFF_PROGRESS_BARS(struct button *button);
void TURN_OFF_ALL_PROGRESS_BARS(void);


#endif /* GAME_H_ */
