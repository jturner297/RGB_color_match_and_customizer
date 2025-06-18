
#include "input.h"

#include "timers.h"

/**************************************************************************************************
* File: input.c
* Handles user input, including configuration, debouncing, and mapping input to game actions.
*************************************************************************************************/

//create button structs
struct button BLUE_BUTTON = {0, 0, 0, 0, B};
struct button GREEN_BUTTON = {0, 0, 0, 0, G};
 struct button RED_BUTTON = {0, 0, 0, 0, R};
 struct button SPECIAL_BUTTON = {0, 0, 0, 0,  SPECIAL};

 //volatile uint32_t button_held = 0;
// volatile uint32_t button_press_timeSTAMP = 0;


//functions
//================================================================================================
// configure_external_switches()
// @parm: none
// @return: none
// 		Setups 4 external push button switches at PA4, PC0, PC2, and PA1 for input. Also configures external
//		interrupts for pins 4, 0, 2 and 1
//		Note: 	BLUE Button = PC0,
//				RED Button = PA4,
//				GREEN Button = PA1
//================================================================================================
void configure_external_switches(void)
{
		//GPIO setup
		RCC->AHB2ENR |= (0x1 << 0); //Port A - already enabled in LED config function
		RCC->AHB2ENR |= (0x1 << 2); //Port C - already enabled in LED config function
		//PA4
		GPIOA->MODER &= ~(0x3 << (2 *4 ));//input pin (PA4 = 00)
		GPIOA->PUPDR = (GPIOA->PUPDR &= ~(0x3 << (2*4)))|(0x1 << 2 * 4);//toggles pull-up (PA4=01)

		//PC0
		GPIOC->MODER &= ~(0x3 << (2 * 0));//input pin (PC0 = 00)
		GPIOC->PUPDR = (GPIOC->PUPDR &= ~(0x3 << (2*0)))|(0x1 << 2 * 0);//toggles pull-up (PC0=01)

		//PC2
		GPIOC->MODER &= ~(0x3 << (2 * 2));//input pin (PC2 = 00)
		GPIOC->PUPDR = (GPIOC->PUPDR &= ~(0x3 << (2*2)))|(0x1 << 2 * 2);//toggles pull-up (PC2=01)

		//PA1
		GPIOA->MODER &= ~(0x3 << (2 * 1));//input pin (PA1 = 00)
		GPIOA->PUPDR = (GPIOA->PUPDR &= ~(0x3 << (2*1)))|(0x1 << 2 * 1);//toggles pull-up (PA1=01)

		//Interrupt Config
	    RCC->APB2ENR |= (0x1 << 0); //Enable system configuration clock for external interrupts

	    // Configure EXTI0 = PC0
	    SYSCFG->EXTICR[0] &= ~(0xF << 0); // clear bits for EXTI0
	    SYSCFG->EXTICR[0] |=  (0x2 << 0); // set to PC (0b0010)

	    // Configure EXTI1 = PA1
	    SYSCFG->EXTICR[0] &= ~(0xF << 4);
	    SYSCFG->EXTICR[0] |=  (0x0 << 4); // PA

	    // Configure EXTI2 = PC2
	    SYSCFG->EXTICR[0] &= ~(0xF << 8);
	    SYSCFG->EXTICR[0] |=  (0x2 << 8); // PC

	    // Configure EXTI4 = PA4
	    SYSCFG->EXTICR[1] &= ~(0xF << 0);
	    SYSCFG->EXTICR[1] |=  (0x0 << 0); // PA

		EXTI -> FTSR1 |= (0x1 << 4)|(0x1 << 0)|(0x1<<2)| (0x1 << 1); //enable falling edge trigger detection
     	EXTI -> IMR1 |= (0x1 << 4)|(0x1 << 0)|(0x1<<2)|(0x1 << 1); //Unmask EXTI4, EXTI0, EXTI2, EXTI1
    	NVIC_SetPriority(EXTI4_IRQn, 3); //pin 4 interrupt: priority level 3
    	NVIC_SetPriority(EXTI0_IRQn, 4);//pin 0 interrupt: priority level 4
    	NVIC_SetPriority(EXTI2_IRQn, 5);//pin 2 interrupt: priority level 5
    	NVIC_SetPriority(EXTI1_IRQn, 6);//pin 1 interrupt: priority level 6

    	NVIC_EnableIRQ(EXTI4_IRQn); //enable interrupt at pin 4
       	NVIC_EnableIRQ(EXTI0_IRQn); //enable interrupt at pin 0
    	NVIC_EnableIRQ(EXTI2_IRQn); //enable interrupt at pin 2
    	NVIC_EnableIRQ(EXTI1_IRQn); //enable interrupt at pin 1
}
//================================================================================================
// configure_Board_Button()
// @parm: none
// @return: none
// 		Setup user button on Nucleo-L476RG board (PC13) for input. Also configures external
//		interrupts at PC13
//================================================================================================
void configure_Board_Button (void)
{
	RCC->AHB2ENR |= (0x1 << 2); //Port C - already enabled in LED config function
	GPIOC->MODER &= ~(0x3 << (2*13));//input 00
	GPIOC->PUPDR &= ~(0x3 << (2*13)); //Already uses hardware pull-up
	GPIOC->PUPDR |=  (0x1 << (2 * 13)); // Pull-up (01)

	RCC->APB2ENR |= (0x1 << 0); //Already done in previous function
	SYSCFG->EXTICR[3] |= (0x2 << 4); //setup PC13 as target pin for EXTI15_10
	EXTI -> FTSR1 |= (0x1 << 13);//enable falling edge trigger detection
	//EXTI->RTSR1 |= (0x1 << 13);
	EXTI->IMR1 |= (0x1 << 13);//Unmask EXTI15_10
	NVIC_SetPriority(EXTI15_10_IRQn, 2);//pin 13 interrupt: priority level 2
	NVIC_EnableIRQ(EXTI15_10_IRQn); //enable interrupt at pin 13
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
void HANDLE_DEBOUNCED_BUTTON(struct button *button){
	if(button->press_ready){
		if(button->ID != SPECIAL){ //if the button is not the special button...
			BUTTON_ACTIONS(button);
		}else if(button->ID == SPECIAL){//the button is the special button
			SPECIAL_BUTTON_ACTIONS(button);
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
void BUTTON_ACTIONS(struct button *button){//regular button pressed
	button->press_ready = 0;
	if (button_mode == count_down){
		   button->color_value -= 100;
				if (button->color_value < 0) {
					  button->color_value = 0;
				}

	}else{ //count_up
        button->color_value += 100;
		button->progress++;
		for (uint32_t i; i < button->progress; i++){

		}
        if (button->color_value > 1000) {
			  button->color_value = 999;
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
void SPECIAL_BUTTON_ACTIONS(struct button *button){
	button->press_ready = 0;
	button_mode^=1;


}






