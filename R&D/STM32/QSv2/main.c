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
#include "QS/QS_DCMotor2.h"
#include "QS/QS_qei.h"

int dcmotor_pos;

void onButton() {
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
	DCM_goToPos(0, dcmotor_pos);
	dcmotor_pos++;
}

void onLongButton() {
	LED_RUN = !LED_RUN;
}


volatile CAN_msg_t lastmsg;

Sint16 get_qei_value() {
	LED_UART = !LED_UART;

	return QEI1_get_count();
}

int main()
{
	SYS_init();
	PORTS_init();
	BUTTONS_init();
	PWM_init();
	CAN_init();
	UART_init();
	DCM_init();

	CAN_send_sid(123);
	CAN_send_sid(203);
	CAN_send_sid(002);
	CAN_send_sid(602);
	CAN_send_sid(430);

	BUTTONS_define_actions(BUTTON1, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON2, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON3, &onButton, &onLongButton, 1);
	BUTTONS_define_actions(BUTTON4, &onButton, &onLongButton, 1);

	DCMotor_config_t motor_config;
	motor_config.Kd = 0;
	motor_config.Ki = 0;
	motor_config.Kp = 100;
	motor_config.epsilon = 100;
	motor_config.pos[0] = 0;
	motor_config.pos[1] = 100;
	motor_config.pos[2] = 1000;
	motor_config.pwm_number = 3;
	motor_config.way_latch = (volatile unsigned int *)&(GPIOC->ODR);
	motor_config.way_bit_number = 4;
	motor_config.sensor_read = &get_qei_value;
	motor_config.timeout = 0;
	motor_config.way0_max_duty = 30;
	motor_config.way1_max_duty = 30;
	DCM_config(0, &motor_config);
	dcmotor_pos = 0;
	DCM_goToPos(0, dcmotor_pos);

	int i = 0;
	int error = 5/i;	//Test trap division par 0 (UsageFault)

    while (1)
    {
    	BUTTONS_update();
    	while(CAN_data_ready()) {
    		CAN_msg_t msg = CAN_get_next_msg();
    		lastmsg = msg;
    	}
    	PWM_run_fine(QEI1_get_count(), 1);
    	debug_printf("test %d\n", error);
    }
}


