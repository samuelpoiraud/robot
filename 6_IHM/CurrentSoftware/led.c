/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : led.c
 *	Package : Carte IHM
 *	Description : Contrôle les leds
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#include "led.h"

#include "QS/QS_macro.h"
#include "QS/QS_ports.h"
#include "Global_config.h"

#define LED_ID 0x00011111
#define TIME_BLINK_OFF 0
#define TIME_BLINK_1HZ 100
#define TIME_BLINK_4HZ 25
#define TIME_BLINK_10MS 10


typedef struct{
	blinkLED_ihm_e blink; // Temps de clignotement *10 en ms. Si 50, va clignoter toutes les 500ms
	Uint8 counter;
	bool_e stateUp;
}LED_t;

static LED_t leds[LED_NUMBER_IHM];

void set_LED(led_ihm_e led, bool_e stateUP);
Uint8 get_blink_state(blinkLED_ihm_e blink);

void LEDS_init(){
	static bool_e initialized = FALSE;

	if(initialized)
		return;

	Uint8 i;
	for(i = 0; i < LED_NUMBER_IHM; i++){
		set_LED(i, FALSE);
		leds[i].stateUp = FALSE;
		leds[i].counter = 0;
		leds[i].blink = OFF;
	}

	initialized = TRUE;
}

void set_LED(led_ihm_e led, bool_e stateUP){
	assert(led >= 0 && led < LED_NUMBER_IHM);

	switch (led) {
		case LED_OK_IHM:
			LED_IHM_OK = stateUP;
			break;
		case LED_UP_IHM:
			LED_IHM_UP = stateUP;
			break;
		case LED_DOWN_IHM:
			LED_IHM_DOWN = stateUP;
			break;
		case LED_SET_IHM:
			LED_IHM_SET = stateUP;
			break;
		case LED_GREEN_IHM:
			GREEN_LEDS = stateUP;
			break;
		case LED_RED_IHM:
			RED_LEDS = stateUP;
			break;
		case LED_BLUE_IHM:
			BLUE_LEDS = stateUP;
			break;
		case LED_0_IHM:
			LED0_PORT = stateUP;
			break;
		case LED_1_IHM:
			LED1_PORT = stateUP;
			break;
		case LED_2_IHM:
			LED2_PORT = stateUP;
			break;
		case LED_3_IHM:
			LED3_PORT = stateUP;
			break;
		case LED_4_IHM:
			LED4_PORT = stateUP;
			break;
		case LED_5_IHM:
			LED5_PORT = stateUP;
			break;
		default:
			break;
	}
}

void LEDS_process_it(void){
	Uint8 i; // Compteur pour la fréquence

	for(i=0;i<BP_NUMBER_IHM;i++){
		if(leds[i].blink == OFF) // Si led non active
			continue;

		// Led active
		if(leds[i].blink != ON){ // Clignotement
			if(leds[i].counter == 0){
				leds[i].counter = get_blink_state(leds[i].blink);
				leds[i].stateUp = !leds[i].stateUp;
				set_LED(i, leds[i].stateUp);
			}

			leds[i].counter--;
		}
	}
}

void LEDS_get_msg(CAN_msg_t *msg){
	Uint8 i,id,blink;

	for(i = 0; i < msg->size; i++){
		id = msg->data[i] & LED_ID;
		blink = msg->data[i] >> 5;

		leds[id].stateUp = (blink == OFF)?FALSE:TRUE;
		leds[id].blink = blink;
		leds[id].counter = get_blink_state(blink);

		set_LED(id, leds[id].stateUp);
	}
}

Uint8 get_blink_state(blinkLED_ihm_e blink){
	Uint8 value;

	switch(blink){
		case OFF:
			value = TIME_BLINK_OFF;
			break;
		case BLINK_1HZ:
			value = TIME_BLINK_1HZ;
			break;
		case SPEED_BLINK_4HZ:
			value = TIME_BLINK_4HZ;
			break;
		case FLASH_BLINK_10MS:
			value = TIME_BLINK_10MS;
			break;
		default:
			break;
	}

	return value;
}
