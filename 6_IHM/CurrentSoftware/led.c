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

#include "QS/QS_ports.h"

#define LED_ID 0b00011111
#define TIME_BLINK_OFF 	0
#define TIME_BLINK_1HZ 	1000
#define TIME_BLINK_4HZ 	250
#define TIME_BLINK_10MS 100

//Le poids faible : état de la led GREEN
//Le poids moyen : état de la led RED
//Le poids fort : état de la led BLUE
#define LED_GREEN	0x01
#define LED_RED		0x02
#define LED_BLUE	0x04


typedef struct{
	blinkLED_ihm_e blink; // Temps de clignotement *10 en ms. Si 50, va clignoter toutes les 500ms
	Uint16 counter;
	bool_e stateUp;
}LED_t;

static LED_t leds[LED_NUMBER_IHM];

void set_LED(led_ihm_e led, bool_e stateUP);
Uint16 get_blink_state(blinkLED_ihm_e blink);
void set_COLOR(led_color_e led_color);

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
#ifdef LED_IHM_OK
		case LED_OK_IHM:
			GPIO_WriteBit(LED_IHM_OK, stateUP);
			break;
#endif
#ifdef LED_IHM_UP
		case LED_UP_IHM:
			GPIO_WriteBit(LED_IHM_UP, stateUP);
			break;
#endif
#ifdef LED_IHM_DOWN
		case LED_DOWN_IHM:
			GPIO_WriteBit(LED_IHM_DOWN, stateUP);
			break;
#endif
#ifdef LED_IHM_SET
		case LED_SET_IHM:
			GPIO_WriteBit(LED_IHM_SET, stateUP);
			break;
#endif
#ifdef LED0_PORT
		case LED_0_IHM:
			GPIO_WriteBit(LED0_PORT, stateUP);
			break;
#endif
#ifdef LED1_PORT
		case LED_1_IHM:
			GPIO_WriteBit(LED1_PORT, stateUP);
			break;
#endif
#ifdef LED2_PORT
		case LED_2_IHM:
			GPIO_WriteBit(LED2_PORT, stateUP);
			break;
#endif
#ifdef LED3_PORT
		case LED_3_IHM:
			GPIO_WriteBit(LED3_PORT, stateUP);
			break;
#endif
#ifdef LED4_PORT
		case LED_4_IHM:
			GPIO_WriteBit(LED4_PORT, stateUP);
			break;
#endif
#ifdef LED5_PORT
		case LED_5_IHM:
			GPIO_WriteBit(LED5_PORT, stateUP);
			break;
#endif
		default:
			break;
	}
}

void LEDS_process_it(Uint8 ms){
	Uint8 i; // Compteur pour la fréquence

	for(i=0;i<LED_NUMBER_IHM;i++){
		if(leds[i].blink == OFF || i == LED_COLOR_IHM) // Si led non active
			continue;

		// Led active
		if(leds[i].blink != ON){ // Different de OFF et different de ON => Forcement l'un des modes de clignotement
			//Ecoulement du temps.
			if(leds[i].counter > ms)
				leds[i].counter-= ms;
			else
				leds[i].counter = 0;

			if(leds[i].counter == 0){
				leds[i].counter = get_blink_state(leds[i].blink);
				leds[i].stateUp = !leds[i].stateUp;
				set_LED(i, leds[i].stateUp);
			}
		}
	}
}

void LEDS_get_msg(CAN_msg_t *msg){
	Uint8 i,id,blink;

	for(i = 0; i < msg->size; i++){
		id = msg->data[i] & LED_ID;
		blink = msg->data[i] >> 5;

		if(id == LED_COLOR_IHM)
			set_COLOR(blink);
		else{
			leds[id].stateUp = (blink == OFF)?FALSE:TRUE;
			leds[id].blink = blink;
			leds[id].counter = get_blink_state(blink);

			set_LED(id, leds[id].stateUp);
		}
	}
}

Uint16 get_blink_state(blinkLED_ihm_e blink){
	Uint16 value;

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

void set_COLOR(led_color_e led_color){

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	if(led_color & 0b100){
		GPIO_InitStructure.GPIO_Pin = RED_LED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		GPIO_ResetBits(RED_LED);

	}else{
		GPIO_InitStructure.GPIO_Pin = RED_LED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}

	if(led_color & 0b010){
		GPIO_InitStructure.GPIO_Pin = GREEN_LED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		GPIO_ResetBits(GREEN_LED);

	}else{
		GPIO_InitStructure.GPIO_Pin = GREEN_LED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}

	if(led_color & 0b001){
		GPIO_InitStructure.GPIO_Pin = BLUE_LED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		GPIO_ResetBits(BLUE_LED);

	}else{
		GPIO_InitStructure.GPIO_Pin = BLUE_LED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}
}
