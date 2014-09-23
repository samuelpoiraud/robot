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

typedef struct{
	bool_e enable;
	Uint8 time; // Base de temps pour la frequence
	Uint8 i; // Compteur pour la fréquence
}LED_t;

static LED_t leds[LED_NUMBER_IHM];

void set_LED(led_ihm_e led, bool_e stateUP);

void LEDS_init(){
	static bool_e initialized = FALSE;

	if(initialized)
		return;

	Uint8 i;
	for(i = 0; i < LED_NUMBER_IHM; i++){
		set_LED(i, FALSE);
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


