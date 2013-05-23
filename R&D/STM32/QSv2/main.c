/*
 * main.c
 *
 *  Created on: 15 mai 2013
 *      Author: doc
 */


#include "QS/QS_sys.h"
#include "QS/QS_ports.h"
#include "QS/QS_buttons.h"
#include "QS/QS_pwm.h"

//PORTD
#define LED_GREEN      12	//LD4
#define LED_ORANGE     13	//LD3
#define LED_RED        14	//LD5
#define LED_BLUE       15	//LD6

void onButton() {
	LED_RUN = !LED_RUN;
}

void _T2Interrupt() {
	LED_CAN = !LED_CAN;
	TIMER2_AckIT();
}

int main()
{
	SYS_init();
	PORTS_init();
	BUTTONS_init();
	PWM_init();

	//PWM_run(30, 1);

	BUTTONS_define_actions(BUTTON1, &onButton, NULL, 10);
	BUTTONS_define_actions(BUTTON2, &onButton, NULL, 10);
	BUTTONS_define_actions(BUTTON3, &onButton, NULL, 10);
	BUTTONS_define_actions(BUTTON4, &onButton, NULL, 10);

    while (1)
    {
    	BUTTONS_update();
    }
}


