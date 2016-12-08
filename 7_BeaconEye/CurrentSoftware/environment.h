/*
 * environment.h
 *
 *  Created on: 20 déc. 2014
 *      Author: Anthony
 */


#ifndef ENVIRONMENT_H_
	#define ENVIRONMENT_H_

	#include "QS/QS_all.h"

	typedef enum{
		LED_0 = 0,
		LED_RED = 0,
		LED_1 = 1,
		LED_GREEN = 1,
		LED_2 = 2,
		LED_BLUE = 2,
		LED_NUMBER = 3
	}led_e;

	#define MATCH_DURATION 				90000UL //90 secondes)

	void ENV_init(void);
	void blue_button(void);
	void ENV_set_color(color_e new_color);
	color_e ENV_get_color(void);
	void ENV_process_main(void);
	void ENV_process_it(void);
	void ENV_set_led(led_e led, blinkLED_e blink);

#endif /* ENVIRONMENT_H_ */






