/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : QS_IHM.c
 *	Package : Carte IHM
 *	Description :
 *	Auteur : Anthony
 *	Version 2014/09/24
 */

#include "QS_IHM.h"
#include "QS_can.h"
#include "QS_CANmsgList.h"
#include "QS_outputlog.h"
#include <stdarg.h>
#include "../main.h"

typedef struct{
	ihm_button_action_t direct_push;
	ihm_button_action_t after_long_push;
	Uint8 long_push_time;
	bool_e long_push_already_detected;
}ihm_button_t;

volatile static bool_e switchs[SWITCHS_NUMBER_IHM];
static ihm_button_t buttons[BP_NUMBER_IHM];
volatile static bool_e *match_started;

void switchs_update(CAN_msg_t * msg);
void switchs_update_all(CAN_msg_t * msg);
void button_update(CAN_msg_t * msg);

void IHM_init(volatile bool_e *matchStarted){
	Uint8 i;

	static bool_e initialized = FALSE;
	if(initialized)
		return;

	for(i=0;i<SWITCHS_NUMBER_IHM;i++)
		switchs[i] = FALSE;

	for(i=0;i<BP_NUMBER_IHM;i++)
		IHM_define_act_button((button_ihm_e)i,NULL,NULL);

	switchs[BIROUTE] = TRUE;			// Activation Biroute
	switchs[SWITCH_RAW_DATA] = TRUE;		// Activation Debug
	switchs[SWITCH_VERBOSE] = TRUE;		// Activation Verbose
	switchs[SWITCH_XBEE] = TRUE;		// Activation Xbee
	switchs[SWITCH_EVIT] = TRUE;		// Activation Evit
	switchs[SWITCH18_DISABLE_ASSER] = FALSE;		// Activation Asser

	match_started = matchStarted;
}

void IHM_leds_send_msg(Uint8 size, led_ihm_t led, ...){
	assert(size < 8);

	Uint8 i = 0;
	CAN_msg_t msg;

	size = MIN(8,size);

	msg.sid = IHM_SET_LED;
	msg.size = size;

	va_list va;
	va_start(va, led);

	msg.data.ihm_set_led.led[0].id = led.id;
	msg.data.ihm_set_led.led[0].blink = led.blink;

	for (i = 1; i < size; i++){
		led = va_arg(va, led_ihm_t);
		msg.data.ihm_set_led.led[i].id = led.id;
		msg.data.ihm_set_led.led[i].blink = led.blink;
	}

	va_end(va);

	CAN_send(&msg);
}


void IHM_set_led_color(led_color_e color)
{
	CAN_msg_t msg;
	msg.sid = IHM_SET_LED_COLOR;
	msg.size = 1;
	msg.data.ihm_set_led_color.led_color = (Uint8)color;
	CAN_send(&msg);
}


void switchs_update(CAN_msg_t * msg){
	Uint8 i;


	for(i=0; i<msg->size; i++)
		switchs[msg->data.ihm_switch.switchs[i].id] = msg->data.ihm_switch.switchs[i].state;
}

void switchs_update_all(CAN_msg_t * msg){
	Uint8 i;
	Uint32 swit = msg->data.ihm_switch_all.switch_mask;

	for(i=0;i<SWITCHS_NUMBER_IHM;i++)
		switchs[i] = (swit >> i) & 1;
}

void button_update(CAN_msg_t * msg){
	ihm_button_t* button = &(buttons[msg->data.ihm_button.id]);
	ihm_button_action_t action;

	if(msg->data.ihm_button.long_push == FALSE) { // direct push
		action = button->direct_push;
		if(action != NULL)
			(*action)();

	}else{
		action = button->after_long_push;
		if(action != NULL){
			(*action)();
		}
	}
}

bool_e IHM_switchs_get(switch_ihm_e swit){
	return switchs[swit];

}

void IHM_process_main(CAN_msg_t* msg){
#ifdef IHM_SWITCH_DISABLE_IN_MATCH
	if(match_started == NULL || (match_started != NULL && (*match_started) == FALSE)){
#endif
		switch (msg->sid){
			case IHM_BUTTON:
				button_update(msg);
				break;
			case IHM_SWITCH:
				switchs_update(msg);
				break;
			case IHM_SWITCH_ALL:
				switchs_update_all(msg);
				break;
			default:
				break;
		}
#ifdef IHM_SWITCH_DISABLE_IN_MATCH
	}
#endif
}

void IHM_define_act_button(button_ihm_e button_id,ihm_button_action_t direct_push, ihm_button_action_t after_long_push){
	assert(button_id<BP_NUMBER_IHM&&button_id>=0);
	ihm_button_t* button = &(buttons[button_id]);
	button->direct_push= direct_push;
	button->after_long_push= after_long_push;
}
