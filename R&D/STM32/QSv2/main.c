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
#include "QS/QS_uart.h"
#include "QS/QS_adc.h"

//PORTD
#define LED_GREEN      12	//LD4
#define LED_ORANGE     13	//LD3
#define LED_RED        14	//LD5
#define LED_BLUE       15	//LD6

void onButton() {
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
}

void onLongButton() {
	LED_RUN = !LED_RUN;
}

void _T1Interrupt() {
	LED_CAN = !LED_CAN;
	TIMER1_AckIT();
}

volatile CAN_msg_t lastmsg;

int main()
{
	SYS_init();
	PORTS_init();
	BUTTONS_init();
	PWM_init();
	TIMER_init();
	CAN_init();
	UART_init();
	ADC_init();

	LED_UART = 0;

	CAN_send_sid(123);
	CAN_send_sid(203);
	CAN_send_sid(002);
	CAN_send_sid(602);
	CAN_send_sid(430);

	TIMER1_run(250);
	NVIC_DisableIRQ(TIM1_TRG_COM_TIM11_IRQn);
	//On ne doit pas avoir d'IT timer

	BUTTONS_define_actions(BUTTON1, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON2, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON3, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON4, &onButton, &onLongButton, 1);

	UART2_putc(0x05);

    while (1)
    {
    	static bool_e led_timer_on = FALSE;

    	BUTTONS_update();
    	while(CAN_data_ready()) {
    		CAN_msg_t msg = CAN_get_next_msg();
    		lastmsg = msg;
    		LED_USER = !LED_USER;
    	}
    	if(TIM_GetITStatus(TIM11, TIM_IT_Update) && led_timer_on == FALSE) {
    		led_timer_on = TRUE;
    		LED_UART = 1;
    	}
    	PWM_run_fine(((Sint32)ADC_getValue(0))*25000/1024, 1);
    }
}


