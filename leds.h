

#ifndef LEDS_H_
#define LEDS_H_
#include "main.h"
/**************************************************************************************************
* File: leds.h
* Header file for leds.c
*************************************************************************************************/
#define TEST_ACTIVE_LED_ON (GPIOC->ODR |= (0x1 << 9))
#define TEST_ACTIVE_LED_OFF (GPIOC->ODR &= ~(0x1 << 9))
#define TEST_ACTIVE_LED_TOGGLE (GPIOC->ODR ^= (0x1 << 9))

#define WIN_LED_ON (GPIOC->ODR |= (0x1 << 8))
#define WIN_LED_OFF (GPIOC->ODR &= ~(0x1 << 8))
#define WIN_LED_TOGGLE (GPIOC->ODR ^= (0x1 << 8))

#define EASY_MODE_LED_ON (GPIOA->ODR |= (0x1 << 15))
#define EASY_MODE_LED_OFF (GPIOA->ODR &= ~(0x1 << 15))
#define EASY_MODE_LED_TOGGLE (GPIOA->ODR ^= (0x1 << 15))



extern struct RGB_Light_Emitting_Diode PLAYER_LED;



extern struct RGB_Light_Emitting_Diode SYSTEM_LED;



extern struct GPIO_config RED_PROGRESS_BAR_LEDS[];
extern struct LED_block RED_PROGRESS_BAR;

extern struct GPIO_config GREEN_PROGRESS_BAR_LEDS[];
extern struct LED_block GREEN_PROGRESS_BAR;

extern struct GPIO_config BLUE_PROGRESS_BAR_LEDS[];
extern struct LED_block BLUE_PROGRESS_BAR;

extern struct GPIO_config COLOR_MATCH_INDICATOR_LEDS[];
extern struct LED_block COLOR_MATCH_INDICATOR;



extern struct GPIO_LED_config GPIOA_LED_config;
extern struct GPIO_LED_config GPIOB_LED_config;
extern struct GPIO_LED_config GPIOC_LED_config;





void CONFIGURE_STANDARD_LEDS( struct GPIO_LED_config *GPIOx_LED_config);
void SETUP_ALTERNATE_FUNCTION_PINS( struct RGB_Light_Emitting_Diode *RGB_LED);
void SETUP_PWN( struct RGB_Light_Emitting_Diode *RGB_LED);
void SETUP_RGB_LED( struct RGB_Light_Emitting_Diode *RGB_LED);
void CONFIGURE_ALL_LEDS(void);
void RESET_RGB_CHANNEL( struct colors *channels);
void UPDATE_RGB_CHANNEL_VALUES( struct RGB_Light_Emitting_Diode *RGB_LED, int32_t r_value, int32_t g_value, int32_t b_value);
void RESET_RGB_LED_CHANNELS(void);
void DISPLAY_RGB_LED(struct RGB_Light_Emitting_Diode *RGB_LED);

void TURN_OFF_LED_BLOCK(struct LED_block *LED_group);

#endif /* LEDS_H_ */
