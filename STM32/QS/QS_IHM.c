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
#include <stdarg.h>

#define SWITCH_ON       0x80
#define SWITCH_OFF		0x00
#define SWITCH_MASK		0x7F

volatile static bool_e switchs[SWITCHS_NUMBER];

void IHM_leds_send_msg(Uint8 size, led_ihm_t led, ...){
	assert(size < 8);

	Uint8 i = 0;
	CAN_msg_t msg;
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

void IHM_switchs_update(CAN_msg_t * msg){
	Uint8 i;

	for(i=0;i<msg->size;i++)
	  switchs[msg->data[i] &SWITCH_MASK] = (msg->data[i] & SWITCH_ON)?TRUE:FALSE;
}

void IHM_switchs_update_all(CAN_msg_t * msg){
	Uint8 i;
	Uint32 swit = U32FROMU8(msg->data[0], msg->data[1], msg->data[2], msg->data[3]);

	for(i=0;i<SWITCHS_NUMBER;i++)
		switchs[0] = swit & (1 << i);
}

bool_e IHM_switchs_get(switch_ihm_e swit){
	return switchs[swit];
}
