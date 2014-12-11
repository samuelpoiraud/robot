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

void switchs_update(CAN_msg_t * msg);
void switchs_update_all(CAN_msg_t * msg);
void button_update(CAN_msg_t * msg);

void IHM_init(){
	Uint8 i;

	static bool_e initialized = FALSE;
	if(initialized)
		return;

	for(i=0;i<SWITCHS_NUMBER_IHM;i++)
		switchs[i] = 0;

	for(i=0;i<BP_NUMBER_IHM;i++){
		IHM_define_act_button((button_ihm_e)i,NULL,NULL);
	}
}

void IHM_leds_send_msg(Uint8 size, led_ihm_t led, ...){
	assert(size < 8);

	Uint8 i = 0;
	CAN_msg_t msg;
	msg.sid = IHM_SET_LED;
	msg.size = size;

	va_list va;
	va_start(va, led);

	msg.data[0] = (led.blink << 5) | led.id;

	for (i = 1; i < size; i++){
		led = va_arg(va, led_ihm_t);
		msg.data[i] = (led.blink << 5) | led.id;
	}

	va_end(va);

	CAN_send(&msg);
}

void switchs_update(CAN_msg_t * msg){
	Uint8 i;

	for(i=0;i<SWITCHS_NUMBER_IHM;i++)
		switchs[msg->data[i] & IHM_SWITCH_MASK] = (msg->data[i] & IHM_SWITCH_ON)?TRUE:FALSE;
}

void switchs_update_all(CAN_msg_t * msg){
	Uint8 i;
	Uint32 swit = U32FROMU8(msg->data[0], msg->data[1], msg->data[2], msg->data[3]);

	for(i=0;i<SWITCHS_NUMBER_IHM;i++)
		switchs[i] = (swit >> i) & 1;
}

void button_update(CAN_msg_t * msg){
	ihm_button_t* button = &(buttons[msg->data[0]]);
	ihm_button_action_t action;

	if(msg->data[1] == FALSE) { // direct push
		action = button->direct_push;
		if(action != NULL)
			(*action)();

	}else{
		action = button->after_long_push;
		if(action != NULL)
			(*action)();
	}
}

bool_e IHM_switchs_get(switch_ihm_e swit){
#ifdef FDP_2014
	#ifdef I_AM_CARTE_STRAT
	bool_e value;

	switch (swit) {
		case BIROUTE:
			value = BIROUTE_PORT;
			break;
		case SWITCH_DEBUG:
			value = SWITCH_DEBUG_PORT;
			break;
		case SWITCH_VERBOSE:
			value = SWITCH_VERBOSE_PORT;
			break;
		case SWITCH_XBEE:
			value = SWITCH_XBEE_PORT;
			break;
		case SWITCH_SAVE:
			value = SWITCH_SAVE_PORT;
			break;
		case SWITCH_COLOR:
			value = SWITCH_COLOR_PORT;
			break;
		case SWITCH_LCD:
			value = SWITCH_LCD_PORT;
			break;
		case SWITCH_EVIT:
			value = SWITCH_EVIT_PORT;
			break;
		case SWITCH_STRAT_1:
			value = SWITCH_STRAT_1_PORT;
			break;
		case SWITCH_STRAT_2:
			value = SWITCH_STRAT_2_PORT;
			break;
		case SWITCH_STRAT_3:
			value = SWITCH_STRAT_3_PORT;
			break;
		default:
			break;
	}

	return value;

	#elif defined(I_AM_CARTE_PROP)
	bool_e value;

	switch (swit) {
		case SWITCH_ASSER:
			value = SWITCH_LEFT_PORT;
			break;
		default:
			break;
	}

	return value;
	#endif

#else
	return switchs[swit];
#endif

}

void IHM_process_main(CAN_msg_t* msg){
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
}

void IHM_define_act_button(button_ihm_e button_id,ihm_button_action_t direct_push, ihm_button_action_t after_long_push){
	assert(button_id<BP_NUMBER_IHM&&button_id>=0);
	ihm_button_t* button = &(buttons[button_id]);
	button->direct_push=direct_push;
	button->after_long_push=after_long_push;
}
