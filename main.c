#include <stdint.h>
#include <stm32l476xx.h>
#include <stddef.h> // For NULL definition
#include "leds.h"
#include "main.h"
#include "input.h"
#include "timers.h"
#include "game.h"


/***************************************************************************************************
* @file main.c
* @brief Main program body
* @author: Justin Turner
* @version 2.0
* ------------------------------------------------------------------------------------------------
*  RGB LED color match
**************************************************************************************************
*/

volatile uint32_t accept_input = 0;

enum gamestates gamestate = start_screen_display;






uint32_t system_timeSTAMP = 0;






volatile uint32_t msTimer = 0;










volatile uint32_t update_LED_animation = 0;





volatile uint32_t strobe_speed = 1;













//uint32_t new_color;

int main(void)
{
	//--initialize hardware------------------------------------------------------------------------
	CONFIGURE_ALL_LEDS();
	CONFIGURE_ALL_BUTTONS();
	configureSysTickInterrupt();
	CONFIGURE_RNG();


	startSysTickTimer_MACRO;

	//Attach color channels to their associated button
	 RED_BUTTON.linked_color_value = &PLAYER_LED.RGB_channels.red_value;
	 GREEN_BUTTON.linked_color_value = &PLAYER_LED.RGB_channels.green_value;
	 BLUE_BUTTON.linked_color_value = &PLAYER_LED.RGB_channels.blue_value;
	//Attach progress bars/blocks to their associated button
	 RED_BUTTON.progress_bar = &RED_PROGRESS_BAR;
	 GREEN_BUTTON.progress_bar = &GREEN_PROGRESS_BAR;
	 BLUE_BUTTON.progress_bar = &BLUE_PROGRESS_BAR;





			RESET_RGB_LED_CHANNELS();




	 while (1)
	{

		DISPLAY_RGB_LED(&SYSTEM_LED);
		DISPLAY_RGB_LED(&PLAYER_LED);


		color_match_indicators_on ? EASY_MODE_LED_ON : EASY_MODE_LED_OFF;

		switch(gamestate){
		case start_screen_display://automate

			if (update_LED_animation == 1){


				COLOR_FADE_EFFECT();
			    UPDATE_RGB_CHANNEL_VALUES(&SYSTEM_LED, red.level, green.level, blue.level);
			    UPDATE_RGB_CHANNEL_VALUES(&PLAYER_LED, red.level, green.level, blue.level);


				    update_LED_animation = 0;
			}
			break;




		case preparing_game:
			TEST_ACTIVE_LED_ON;
			if (msTimer - system_timeSTAMP >= 1000){

				 GENERATE_NEW_COLOR();



				accept_input =1; //enable input
				gamestate = test;
				}
			break;//
		case test: //the player needs to admire their work after the test

				HANDLE_BUTTON_PROGRESS_BAR(&RED_BUTTON);
				HANDLE_BUTTON_PROGRESS_BAR(&BLUE_BUTTON);
				HANDLE_BUTTON_PROGRESS_BAR(&GREEN_BUTTON);




				HANDLE_COLOR_MATCH_INDICATOR();

			if(    PLAYER_LED.RGB_channels.red_value == SYSTEM_LED.RGB_channels.red_value && //win
				    PLAYER_LED.RGB_channels.green_value == SYSTEM_LED.RGB_channels.green_value &&
				    PLAYER_LED.RGB_channels.blue_value == SYSTEM_LED.RGB_channels.blue_value ){
				accept_input =0;
				TEST_ACTIVE_LED_OFF;
				WIN_LED_ON;
				previously_tested_color = SYSTEM_LED.RGB_channels;//save the tested color

				system_timeSTAMP = msTimer;
				gamestate = entering_win_screen;
			}
			break;
		case game_cancelled:
			accept_input =0;


			TEST_ACTIVE_LED_OFF;
			GAME_OVER();

    			  system_timeSTAMP = msTimer;
    			gamestate = preparing_game;
			break;
		case entering_win_screen:
					if (msTimer - system_timeSTAMP >= 2000){
						GAME_OVER();
					}


			if (msTimer - system_timeSTAMP >= 3000){



				system_timeSTAMP =	msTimer;
						gamestate = win_screen;
			}
			break;
		case win_screen:
			if(update_LED_animation){
				update_LED_animation = 0;
				HANDLE_WINNING_ANIMATION();
			}
			if (msTimer - system_timeSTAMP >= 3600){
				WIN_LED_OFF;



				RESET_RGB_LED_CHANNELS();
				system_timeSTAMP =	msTimer;
						gamestate = preparing_game;
			}
			break;


		}





		HANDLE_POST_DEBOUNCED_BUTTONS(msTimer);

	}
}//end main


//================================================================================================
// EXTI4_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the RED button
//================================================================================================
void EXTI4_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 4)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 4);  // Clear interrupt flag
		  if(gamestate == start_screen_display){

			  START_GAME(msTimer);

		}
		  if (accept_input == 1){ //only begin process if game is in test phase
		  INITIATE_DEBOUNCE_PROTOCOL(&RED_BUTTON, msTimer);
		  }
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
		  if(gamestate == start_screen_display){

				  START_GAME(msTimer);

			}
		  if (accept_input == 1){ //only begin process if game is in test phase
			  INITIATE_DEBOUNCE_PROTOCOL(&GREEN_BUTTON, msTimer);
		  }
	}
}

//================================================================================================
// EXTI0_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the BLUE button
//================================================================================================
void EXTI0_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 0)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 0);  // Clear interrupt flag
		  if(gamestate == start_screen_display){

				  START_GAME(msTimer);

			}
		  if (accept_input == 1){ //only begin process if game is in test phase
			  INITIATE_DEBOUNCE_PROTOCOL(&BLUE_BUTTON, msTimer);
		  }

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
	  if(gamestate == start_screen_display){
		  if(msTimer%30 == 0){
			  update_LED_animation = 1;
		  }

	  }
	  else if(gamestate == win_screen){
		  if(msTimer%300 == 0){
					  update_LED_animation = 1;
				  }
	  }


}































