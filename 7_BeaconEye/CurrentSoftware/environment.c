/*
 * environment.h
 *
 *  Created on: 20 déc. 2014
 *      Author: Anthony
 */

#include "environment.h"
#include "QS/QS_buttons.h"
#include "QS/QS_ports.h"
#include "QS/QS_outputlog.h"
#include "zone.h"


#define TIME_BLINK_OFF 	0
#define TIME_BLINK_1HZ 	1000
#define TIME_BLINK_4HZ 	250
#define TIME_BLINK_10MS 100

#define LED_NUMBER_IHM 3

static time32_t match_duration = MATCH_DURATION;

typedef struct{
	blinkLED_e blink; // Temps de clignotement *10 en ms. Si 50, va clignoter toutes les 500ms
	Uint16 counter;
	bool_e stateUp;
}LED_t;

static LED_t leds[LED_NUMBER_IHM];
static volatile color_e current_color = BOT_COLOR;
static Uint8 t_ms;


void set_COLOR(led_color_e led_color);
void set_LED(led_e led, bool_e stateUP);
Uint16 get_blink_state(blinkLED_e blink);
void leds_process_it(Uint8 ms);
void button_laser(void);
void button_0(void);


void ENV_init(void){
	static bool_e initialized = FALSE;

	if(initialized)
		return;

	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	global.flags.match_suspended = FALSE;
	global.absolute_time = 0;
	global.match_time = 0;

	Uint8 i;
	for(i = 0; i < LED_NUMBER_IHM; i++){
		set_LED(i, FALSE);
		leds[i].stateUp = FALSE;
		leds[i].counter = 0;
		leds[i].blink = OFF;
	}

	set_COLOR(LED_COLOR_BLACK);

	initialized = TRUE;
	t_ms = 0;
}

void button_0(void){

	if(!BUTTON0_PORT){
		if(global.flags.LASER == TRUE)
		{
			GPIO_WriteBit(MOS_LASER, FALSE);
			global.flags.LASER = FALSE;
		}
		else
		{
			GPIO_WriteBit(MOS_LASER, TRUE);
			global.flags.LASER = TRUE;
		}
	}
	else
	{

	}

}

void button_laser(void){
	if(!BUTTON_LASER_PORT){ 			// Appui sur le bouton
		//debug_printf("BP LASER \n");
		if(!global.flags.match_started){
			global.flags.match_started = TRUE;
			global.match_time = 0;
		}
	}else{ 								//Relachement du bouton

	}
}

void ENV_process_main(void){
	static Uint8 prevSW_color = BOT_COLOR, prevBP_laser = 1, prevBP0 = 1;

	if(t_ms > 20){
		BUTTONS_update();

		if(prevSW_color != SWITCH_COLOR_BEACON_EYE){
			ENV_set_color((current_color==BOT_COLOR)?TOP_COLOR:BOT_COLOR);
			prevSW_color = SWITCH_COLOR_BEACON_EYE;
		}

		if(prevBP_laser != BUTTON_LASER_PORT){
			button_laser();
			prevBP_laser = BUTTON_LASER_PORT;
		}
		if(prevBP0 != BUTTON0_PORT){
			button_0();
			prevBP0 = BUTTON0_PORT;
		}

		t_ms = 0;
	}



}

void ENV_set_color(color_e new_color){
	current_color = new_color;	//On inverse la couleur...
	ZONE_clean_all_detections();	//On réinitialise toutes les zones (évènements par défaut...)
	ZONE_new_color(current_color);	//Zones par défaut...
	set_COLOR((current_color==BOT_COLOR)?LED_COLOR_MAGENTA:LED_COLOR_GREEN);
}

color_e ENV_get_color(void){
	return current_color;
}

void ENV_process_it(void){
	BUTTONS_process_it();
	leds_process_it(1);
	t_ms++;

	if(global.flags.match_started && !global.flags.match_over && !global.flags.match_suspended){
				global.match_time++;
			}

			 if (!global.flags.match_over && !global.flags.match_suspended) {
				if (match_duration != 0 && (global.match_time >= (match_duration))) {
					//MATCH QUI SE TERMINE
					global.flags.match_over = TRUE;
				}
			 }


}

void ENV_set_led(led_e led, blinkLED_e blink){
	leds[led].stateUp = (blink == OFF)?FALSE:TRUE;
	leds[led].blink = blink;
	leds[led].counter = get_blink_state(blink);

	set_LED(led, leds[led].stateUp);
}

void leds_process_it(Uint8 ms){
	Uint8 i; // Compteur pour la fréquence

	for(i=0;i<LED_NUMBER_IHM;i++){
		if(leds[i].blink == OFF) // Si led non active
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

Uint16 get_blink_state(blinkLED_e blink){
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
		GPIO_InitStructure.GPIO_Pin = LED_RED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_ResetBits(LED_RED_PORT);
	}else{
		GPIO_InitStructure.GPIO_Pin = LED_RED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}

	if(led_color & 0b010){
		GPIO_InitStructure.GPIO_Pin = LED_GREEN_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_ResetBits(LED_GREEN_PORT);
	}else{
		GPIO_InitStructure.GPIO_Pin = LED_GREEN_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}

	if(led_color & 0b001){
		GPIO_InitStructure.GPIO_Pin = LED_BLUE_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_ResetBits(LED_BLUE_PORT);
	}else{
		GPIO_InitStructure.GPIO_Pin = LED_BLUE_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
}

void set_LED(led_e led, bool_e stateUP){
	assert(led >= 0 && led < LED_NUMBER);

	switch (led) {
#ifdef LED_0_PORT
		case LED_0:
			GPIO_WriteBit(LED_0_PORT, stateUP);
			break;
#endif
#ifdef LED_1_PORT
		case LED_1:
			GPIO_WriteBit(LED_1_PORT, stateUP);
			break;
#endif
#ifdef LED_2_PORT
		case LED_2:
			GPIO_WriteBit(LED_2_PORT, stateUP);
			break;
#endif
		default:
			break;
	}
}
