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
#include "QS/QS_can.h"

//PORTD
#define LED_GREEN      12	//LD4
#define LED_ORANGE     13	//LD3
#define LED_RED        14	//LD5
#define LED_BLUE       15	//LD6

void onButton() {
}

void onLongButton() {
	LED_RUN = !LED_RUN;
}

void _T2Interrupt() {
	LED_CAN = !LED_CAN;
	TIMER2_AckIT();
}

volatile CAN_msg_t lastmsg;

int main()
{
	SYS_init();
	PORTS_init();
	BUTTONS_init();
	PWM_init();
	TIMER_init();

	LED_UART = 0;

	//PWM_run(30, 1);
	CAN_send_sid(123);
	CAN_send_sid(203);
	CAN_send_sid(703);
	CAN_send_sid(002);
	CAN_send_sid(430);

	TIMER2_run(250);
	NVIC_DisableIRQ(TIM8_BRK_TIM12_IRQn);
	//On ne doit pas avoir d'IT timer

	BUTTONS_define_actions(BUTTON1, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON2, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON3, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON4, &onButton, &onLongButton, 1);

    while (1)
    {
    	static bool_e led_timer_on = FALSE;

    	BUTTONS_update();
    	while(CAN_data_ready()) {
    		CAN_msg_t msg = CAN_get_next_msg();
    		lastmsg = msg;
    		LED_USER = !LED_USER;
    	}
    	if(TIM_GetITStatus(TIM12, TIM_IT_Update) && led_timer_on == FALSE) {
    		led_timer_on = TRUE;
    		LED_UART = 1;
    	}
    }
}


