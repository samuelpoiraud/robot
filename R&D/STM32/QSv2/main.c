/*
 * main.c
 *
 *  Created on: 15 mai 2013
 *      Author: doc
 */


#include "QS/QS_sys.h"
#include "QS/QS_ports.h"
#include "QS/QS_buttons.h"
#include "QS/QS_timer.h"

//PORTD
#define LED_GREEN      12
#define LED_ORANGE     13
#define LED_RED        14
#define LED_BLUE       15

void onButton() {
	LED_RUN = !LED_RUN;
}

void _T1Interrupt() {
	LED_CAN = !LED_CAN;
}

int main()
{
	SYS_init();
	PORTS_init();
	BUTTONS_init();
	TIMER_init();

	TIMER1_run(250);

	BUTTONS_define_actions(BUTTON1, &onButton, NULL, 10);
	BUTTONS_define_actions(BUTTON2, &onButton, NULL, 10);
	BUTTONS_define_actions(BUTTON3, &onButton, NULL, 10);
	BUTTONS_define_actions(BUTTON4, &onButton, NULL, 10);

    while (1)
    {
    }
}


