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
*  RGB Color Match + Customizer
**************************************************************************************************
*/

enum system_states system_state = start_screen_display; //sets default system state - must always start at start_screen_display
enum game_modes game_mode_selected = color_match; //sets default game mode, can be changed by tapping special button

enum color_match_states gamestate; //controls the states within the color match mode
enum color_customizer_states free_play_state;//controls the states within the color customizer mode

volatile uint32_t accept_input = 0; //controls when player input is accepted
volatile uint32_t msTimer = 0; //used in the SysTick for time keeping and debouncing
volatile uint32_t update_LED_animation = 0; //used in SysTick for LED animation.
uint32_t system_timeSTAMP = 0; //Used to mark key system events
uint32_t game_mode_switched = 0; //flag used in the start screen that flags when the game mode is switched

//================================================================================================
// RESET_AND_RETURN_TO_START()
// @parm: none
// @return: none
// 		Cleans up all game assets so that the system can safely return to the start screen
//================================================================================================
void RESET_AND_RETURN_TO_START(void){
	start_screen_requested = 1; //lets the game know that the user wants to exit completely
	CLEAR_GAME_HUD();  //clear game visual elements like progress bar, match indicators, and RGB LED
	gamestate= skip_test; //cancels the game just in case (match mode)
	custom_step_size = 0; //reset step size back to 0 (customizer mode)

	accept_input =0; //disable input - not needed in the start screen
	guided_mode_on = 0; //turn off color match indicator

	TEST_ACTIVE_LED_OFF; //turn off test indicator
	WIN_LED_OFF; //turn off win indicator
	system_timeSTAMP = 0; //reset system timestamp
	RESET_FADE_EFFECT(); //ensures the animation starts fresh when going back to start screen
	RESET_TICK_EFFECT(); //ensures the animation starts fresh when going back to start screen
	system_state = start_screen_display; //return to start screen
}


int main(void)
{
	//--initialize hardware------------------------------------------------------------------------
	CONFIGURE_ALL_LEDS();
	CONFIGURE_ALL_BUTTONS();
	configureSysTickInterrupt();
	CONFIGURE_RNG();

	startSysTickTimer_MACRO;
	RESET_RGB_LED_CHANNELS(); //Ensures both RGB LEDS start off

	while (1)
	{

		DISPLAY_RGB_LED(&SYSTEM_LED);
		DISPLAY_RGB_LED(&PLAYER_LED);

		if(system_state == in_color_match){ //if the system is in color match mode

			guided_mode_on ? GUIDED_MODE_LED_ON : GUIDED_MODE_LED_OFF;
		}
		else{
			GUIDED_MODE_LED_OFF;
		}

		switch(system_state){
		case start_screen_display://landing page for the system, here the player can press the special button to switch between the two modes
			start_screen_requested = 0; //clear
			if (update_LED_animation == 1){//if the LED update flag is set in the SysTick....

				switch(game_mode_selected){
				/*each mode has it's own dedicated animation that lets
				 the player know which one they are about to start*/
				case color_match: //shows all the colors the player will be tested on
					COLOR_TICK_EFFECT();
					break;
				case color_customizer://shows a rainbow fade effect to emphasize endless color mixing
					COLOR_FADE_EFFECT();
					break;
				}
				if(game_mode_switched){ //if game mode is switched (the special button is pressed)....
					RESET_RGB_LED_CHANNELS(); //reset RGB channel values
				    if (game_mode_selected == color_match) {
				        // Force the LED update immediately — don't wait for next SysTick
				        SYSTEM_LED.RGB_channels = preset_colors[0];
				        PLAYER_LED.RGB_channels = preset_colors[0];
				    }
					game_mode_switched = 0; //clear
				}
				update_LED_animation = 0; //clear update flag
			}
			break;
		case in_color_match://system is in color match mode also known as game
			switch(gamestate){
				case preparing_game: //prepare the test/game
					TEST_ACTIVE_LED_ON; //lets the player know the game is about to start
					if (msTimer - system_timeSTAMP >= TIME_1000MS){ //wait
						 GENERATE_NEW_COLOR(); //use RNG to generate a new color
						accept_input =1; //enable input
						gamestate = test; //start test/game
						}
					break;
				case test: //the player needs to admire their work after the test

					HANDLE_ALL_BUTTON_PROGRESS_BARS();
					HANDLE_GUIDED_MODE_INDICATOR(); //if the player has turned on guided mode they will be shown when a value is correct

					if(PLAYER_LED.RGB_channels.red_value == SYSTEM_LED.RGB_channels.red_value && //the player has successfully matched all channels
					PLAYER_LED.RGB_channels.green_value == SYSTEM_LED.RGB_channels.green_value &&
					PLAYER_LED.RGB_channels.blue_value == SYSTEM_LED.RGB_channels.blue_value ){

						accept_input =0; //disable player input because test is over

						TEST_ACTIVE_LED_OFF; //test is over, turn off the test indicator
						WIN_LED_ON; //turn on win indicator

						previously_tested_color = SYSTEM_LED.RGB_channels;//save the tested color for win animation

						system_timeSTAMP = msTimer;
						gamestate = entering_win_screen;
					}
					break;
				case skip_test: //special button held
					accept_input =0;


					TEST_ACTIVE_LED_OFF;
					CLEAR_GAME_HUD();  //clear game visual elements like progress bar, match indicators, and rgb LED
					if(start_screen_requested != 1){ //if the play has not requested to return to the start screen
						//prepare next test
						system_timeSTAMP = msTimer;
						gamestate = preparing_game;
					}

					break;
				case entering_win_screen: //the play successfully replicated the color
					if (msTimer - system_timeSTAMP >= TIME_2000MS){ //gives the player some time to see their hardwork before clearing the HUD
						CLEAR_GAME_HUD(); //clear game visual elements like progress bar, match indicators, and rgb LED
					}
					if (msTimer - system_timeSTAMP >= TIME_3000MS){ //show nothing for a while
						//transition into win screen
						system_timeSTAMP =	msTimer;
						gamestate = win_screen;
					}
					break;
				case win_screen:
					if(update_LED_animation){//if the LED update flag is set in the SysTick....
						HANDLE_WINNING_ANIMATION();
						update_LED_animation = 0;
					}
					if (msTimer - system_timeSTAMP >= TIME_3600MS){//after some time has passed...
						WIN_LED_OFF; //turn off win LED
						RESET_RGB_LED_CHANNELS(); //clear RGB channels
						//transition into next game
						system_timeSTAMP =	msTimer;
						gamestate = preparing_game;
					}
					break;
				}
			break;
			case in_color_customizer: //system is in color customizer mode also known as free play
				switch(free_play_state){
				case preparing: //preparing
					if(msTimer - system_timeSTAMP >= TIME_500MS){//wait
						//enable input and transition into free play
						accept_input = 1;
						WIN_LED_ON;
						TEST_ACTIVE_LED_ON;
						free_play_state = free_play;
					}
					break;
				case free_play:
					SYSTEM_LED.RGB_channels = PLAYER_LED.RGB_channels; //To create the illusion that the player is controlling both at once
					HANDLE_STEP_SIZE_INDICATOR();
					HANDLE_ALL_BUTTON_PROGRESS_BARS();
					break;
				}



				break;
		}






		HANDLE_POST_DEBOUNCED_BUTTONS(msTimer);//Handle the actions for valid button presses

	}
}//end main


//================================================================================================
// EXTI4_IRQHandler()
// @parm: none
// @return: none
//		Starts the game or initiates the debounce protocol for the RED button
//================================================================================================
void EXTI4_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 4)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 4);  // Clear interrupt flag
		  if(system_state == start_screen_display){
			  ENTER_A_GAME_MODE();
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
//		Starts the game or initiates the debounce protocol for the GREEN button
//================================================================================================
void EXTI1_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 1)) { //if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 1);  // Clear interrupt flag
		  if(system_state == start_screen_display){
			  ENTER_A_GAME_MODE();
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
//		Starts the game or initiates the debounce protocol for the BLUE button
//================================================================================================
void EXTI0_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 0)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 0);  // Clear interrupt flag
		  if(system_state == start_screen_display){
			  ENTER_A_GAME_MODE();
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
// 		Increments msTimer variable, handles the debounce protocols, and LED animation updates
//================================================================================================
void SysTick_Handler(void)
{
	  msTimer++; //goes up every 1ms
	  HANDLE_BUTTON_DEBOUNCE_PROTOCOLS(msTimer); //validates the buttons
	  HANDLE_LED_ANIMATION();

}






























