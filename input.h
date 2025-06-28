
#ifndef INPUT_H_
#define INPUT_H_

#include "main.h"
/**************************************************************************************************
* File: input.h
* Header file for input.c
*************************************************************************************************/
#define DEBOUNCE_DELAY 150//arbitrary value that provided an appropriate delay while still being responsive
#define RedButtonPressed    ((!(GPIOA->IDR & (0x1 << 4))))   // PA4
#define GreenButtonPressed  ((!(GPIOA->IDR & (0x1 << 1))))   // PA1
#define BlueButtonPressed   ((!(GPIOC->IDR & (0x1 << 0))))   // PC0
#define SpecialButtonPressed ((!(GPIOC->IDR & (0x1 << 13)))) // PC13 (Board button)

//create button structs
extern struct button RED_BUTTON;
extern  struct button GREEN_BUTTON;
extern  struct button BLUE_BUTTON;
extern  struct button SPECIAL_BUTTON;



//function prototypes
void configure_external_switches(void);
void configure_Board_Button (void);
void INITIATE_DEBOUNCE_PROTOCOL(struct button *button, uint32_t currentTIME_ms);
void DEBOUNCE_PROTOCOL(struct button *button, uint32_t currentTIME_ms);
void HANDLE_DEBOUNCED_BUTTON(struct button *button, uint32_t currentTIME_ms);
void SPECIAL_BUTTON_ACTIONS(struct button *button, uint32_t currentTIME_ms);
void BUTTON_ACTIONS(struct button *button, uint32_t currentTIME_ms);//regular button pressed
void configure_buttons(struct button *button, uint32_t priority);
void CONFIGURE_ALL_BUTTONS(void);
void HANDLE_POST_DEBOUNCED_BUTTONS(uint32_t currentTIME_ms);
void HANDLE_BUTTON_DEBOUNCE_PROTOCOLS(uint32_t currentTIME_ms);
#endif /* INPUT_H_ */
