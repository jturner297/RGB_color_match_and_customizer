
#include "input.h"
#include "leds.h"
#include "timers.h"
#include "game.h"

/**************************************************************************************************
* File: input.c
* Handles user input, including configuration, debouncing, and mapping input to game actions.
*************************************************************************************************/



//create button structs
struct button RED_BUTTON = {
    .port = GPIOA,
    .pin = 4,
    .ID = R,
    .irq_number = EXTI4_IRQn,

    // other initializers zeroed as needed...
};

struct button GREEN_BUTTON = {
    .port = GPIOA,
    .pin = 1,
    .irq_number = EXTI1_IRQn
};

struct button BLUE_BUTTON = {
    .port = GPIOC,
    .pin = 0,
    .ID = B,
	.irq_number = EXTI0_IRQn
};
struct button SPECIAL_BUTTON = {
    .port = GPIOC,
    .pin = 13,
    .ID = SPECIAL,
    .irq_number = EXTI15_10_IRQn
};




//functions
void configure_buttons(struct button *button, uint32_t priority)
{
    uint32_t exti_port_source = 0;
		//GPIO setup
		if (button->port == GPIOA){
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
			 exti_port_source = 0;
		}
    	else if (button->port == GPIOB){
    		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    		exti_port_source = 1;

    	}
    	else if (button->port == GPIOC){
    		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    		 exti_port_source = 2;
    	}


		//GPIO setup
		button->port->MODER &= ~(0x3 << (2 * button->pin));//input pin (00)
		button->port->PUPDR = (button->port->PUPDR &= ~(0x3 << (2*button->pin)))|(0x1 << 2 * button->pin);//toggles pull-up (01)

		//Interrupt Config
	    RCC->APB2ENR |= (0x1 << 0); //Enable system configuration clock for external interrupts

	    // EXTI config register index and shift
	    uint32_t exti_cr_index = button->pin / 4;
	    uint32_t exti_cr_shift = (button->pin % 4) * 4;

	    // Configure EXTI0 = PC0
	    SYSCFG->EXTICR[exti_cr_index] &= ~(0xF << exti_cr_shift); // clear bits for EXTI0
	    SYSCFG->EXTICR[exti_cr_index] |=  (exti_port_source << exti_cr_shift); // set to PC (0b0010)

		EXTI -> FTSR1 |= (0x1 << button->pin);
		EXTI -> RTSR1 |= (0x1 << button->pin);
     	EXTI -> IMR1 |= (0x1 << button->pin);

     	NVIC_SetPriority(button->irq_number, priority); //pin 4 interrupt: priority level 3
    	NVIC_EnableIRQ(button->irq_number); //enable interrupt at pin 4

}



//================================================================================================
// INITIATE_DEBOUNCE_PROTOCOL()
// @parm: *button = Pointer to the button struct representing the button
// @parm: currentTIME_ms = Current system time in milliseconds.
// @return: none
//	 	Initiates the debouncing process by marking the press as pending and recording time
//================================================================================================
void INITIATE_DEBOUNCE_PROTOCOL(struct button *button, uint32_t currentTIME_ms){
  	button->press_ready = 0; //clear
	button->press_pending = 1; //flag that determines if a press is pending
  	button->debounce_counter = currentTIME_ms;//creates a timestamp for the debounce
}
//================================================================================================
// DEBOUNCE_PROTOCOL()
// @parm: *button = Pointer to the button struct representing the button
// @parm: currentTIME_ms = Current system time in milliseconds.
// @return: none
//	 	Once a button press is pending, waits for a delay to validate the press
//================================================================================================
void DEBOUNCE_PROTOCOL(struct button *button, uint32_t currentTIME_ms){
	if(button->press_pending == 1){ //if a press is pending
	    	if(currentTIME_ms - button->debounce_counter >= DEBOUNCE_DELAY){// if debouncing is over.....
	    		button->press_pending = 0; //clear press pending flag
	    	  	button->debounce_counter = 0;//reset d
	    		button->press_ready = 1;
	    	}
	  }
}

//================================================================================================
// HANDLE_DEBOUNCED_BUTTON()
// @parm: *button = Pointer to the button struct representing the button
// @parm: currentTIME_ms = Current system time in milliseconds.
// @return: none
//	 	If the button press is validated, perform the appropriate  action based on the button's ID
//================================================================================================
void HANDLE_DEBOUNCED_BUTTON(struct button *button, uint32_t currentTIME_ms){
	if(button->press_ready){
		if(button->ID != SPECIAL){ //if the button is not the special button...
			BUTTON_ACTIONS(button, currentTIME_ms);
		}else if(button->ID == SPECIAL){//the button is the special button
			SPECIAL_BUTTON_ACTIONS(button, currentTIME_ms);
		}
	}
}



//================================================================================================
// BUTTON_ACTIONS()
//
// @parm: *button - pointer to button struct
// @parm: currentTIME_ms - current system time in milliseconds
// @parm: *pattern - pointer to GAME_PATTERN struct
//@return: none
// 		Handles presses that come from the standard buttons
//================================================================================================
void BUTTON_ACTIONS(struct button *button, uint32_t currentTIME_ms){//regular button pressed
	button->press_ready = 0; //debounce handled
	// uint32_t prev_led = 0;

	 if (!button->is_held){
		button->is_held = 1; //set button_held flag
		button->held_duration = 0;
		button->pressTIMESTAMP = currentTIME_ms; //used to calculate how long the button was pressed for
	}

	  if(button->is_held){
		  if(!(button->port->IDR & (0x1 << button->pin))){
			  	  //do nothing - wait for release
		  }
		  else{//released
			  button->is_held = 0; //set button_held flag
			  button->held_duration= currentTIME_ms - button->pressTIMESTAMP;

			  if (button->held_duration < TIME_500MS) {
				 *(button->linked_color_value) =   (*(button->linked_color_value)+200)%1200;
				button->progress =   (button->progress+1)%6 ;


			  }else if (button->held_duration > 500){ //&& button->held_duration < 2000 ){ //long press behavior

	  	          	  *(button->linked_color_value) = 0; //reset individual color back to 0
	  	          	CLEAR_PROGRESS(button);

			  }//else if( button->held_duration > 2000){

				//  *(button->linked_color_value) = 1000;



	  	       // }
		  }
	  }
}




//================================================================================================
// SPECIAL_BUTTON_ACTIONS()
// @parm: *button = Pointer to the button struct representing the button
// @parm: currentTIME_ms - current system time in milliseconds
// @return: none
//	 	Handles the logic/behavior for a special button press
//================================================================================================
void SPECIAL_BUTTON_ACTIONS(struct button *button, uint32_t currentTIME_ms){
	button->press_ready = 0;


	if (!button->is_held){
			button->is_held = 1; //set button_held flag
			button->held_duration = 0;
			button->pressTIMESTAMP = currentTIME_ms; //used to calculate how long the button was pressed for
		}

		  if(button->is_held){
			  if(!(button->port->IDR & (0x1 << button->pin))){
				  	  //do nothing - wait for release
			  }
			  else{//released
				  button->is_held = 0; //set button_held flag
				  button->held_duration= currentTIME_ms - button->pressTIMESTAMP;
				  if (button->held_duration < TIME_500MS) {

					  if(gamestate == test){
						  color_match_indicators_on^=1; //toggle the answer key
					  }





				  }else if (button->held_duration > 500 && button->held_duration < 2000 ){ //long press behavior
		  	          		 if(gamestate == test){
		  	          			 gamestate = game_cancelled;
		  	          		 }







				  }else if( button->held_duration > 2000){
						 gamestate = game_cancelled;




		  	        }
			  }

	}
}


void CONFIGURE_ALL_BUTTONS(void){
	configure_buttons(&RED_BUTTON, 3);
	configure_buttons(&GREEN_BUTTON, 6);
	configure_buttons(&BLUE_BUTTON, 4);
	configure_buttons(&SPECIAL_BUTTON,  2);
}

//================================================================================================
// HANDLE_POST_DEBOUNCED_BUTTON()
// @parm: currentTIME_ms - Current system time in milliseconds.
// @return: none
// 		Calls HANDLE_DEBOUNCED_BUTTON() for all buttons, executing any valid post-debounce
//		actions.
//================================================================================================
void HANDLE_POST_DEBOUNCED_BUTTONS(uint32_t currentTIME_ms){
	HANDLE_DEBOUNCED_BUTTON(&SPECIAL_BUTTON, currentTIME_ms);
	HANDLE_DEBOUNCED_BUTTON(&BLUE_BUTTON, currentTIME_ms);
	HANDLE_DEBOUNCED_BUTTON(&RED_BUTTON, currentTIME_ms);
	HANDLE_DEBOUNCED_BUTTON(&GREEN_BUTTON, currentTIME_ms);

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


