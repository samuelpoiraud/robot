#include "led.h"

#include "../QS/QS_ports.h"

/* Private defines */
#define TIME_BLINK_OFF 	0
#define TIME_BLINK_1HZ 	1000
#define TIME_BLINK_4HZ 	250
#define TIME_BLINK_10MS 100

typedef struct{
	blinkLED_e blink;
	Uint16 counter;
	bool_e stateUp;
}LED_t;

static LED_t leds[LED_NUMBER_BEACON_EYE];
static led_color_e LED_colorLed = LED_COLOR_BLACK;

/* Private functions */
static void LED_setState(ledBeaconEye_e led, bool_e stateUP);
static Uint16 LED_getBlinkTime(blinkLED_e blink);
static void LED_setColorLed(led_color_e color);

void LED_init() {
	static bool_e initialized = FALSE;

	if(initialized) {
		return;
	}

	Uint8 i;
	for(i = 0; i < LED_NUMBER_BEACON_EYE; i++){
		LED_setState(i, FALSE);
		leds[i].stateUp = FALSE;
		leds[i].counter = 0;
		leds[i].blink = OFF;
	}

	LED_setColor(LED_COLOR_RED);

	initialized = TRUE;
}

void LED_setColor(led_color_e color) {
	LED_colorLed = color;
	LED_setState(LED_COLOR_BEACON_EYE, TRUE);
}

void LED_processIt(Uint8 ms) {
	Uint8 i; // Compteur pour la fréquence

	for(i = 0; i < LED_NUMBER_BEACON_EYE; i++){
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
				leds[i].counter = LED_getBlinkTime(leds[i].blink);
				leds[i].stateUp = !leds[i].stateUp;
				LED_setState(i, leds[i].stateUp);
			}
		}
	}
}

/***************************************************************
 *                       Fonctions privées
 ***************************************************************/

/**
 * @brief Modifier l'état d'une led
 * @param led Identifiant de la led
 * @param stateUP Etat de la led
 */
static void LED_setState(ledBeaconEye_e led, bool_e stateUP) {

	assert(led >= 0 && led < LED_NUMBER_BEACON_EYE);

	switch (led) {
#ifdef LED_0_PORT
		case LED_0_BEACON_EYE:
			GPIO_WriteBit(LED_0_PORT, stateUP);
			break;
#endif
#ifdef LED_1_PORT
		case LED_1_BEACON_EYE:
			GPIO_WriteBit(LED_1_PORT, stateUP);
			break;
#endif
#ifdef LED_2_PORT
		case LED_2_BEACON_EYE:
			GPIO_WriteBit(LED_2_PORT, stateUP);
			break;
#endif
		case LED_COLOR_BEACON_EYE:
			LED_setColorLed( (stateUP)?LED_colorLed:LED_COLOR_BLACK);
			break;
		default:
			break;
	}
}

/**
 * @brief Obtenir le temps de clignotement en fonction de l'option choisie
 * @param blink Option de clignotement
 * @return Temps entre deux changements d'état de la led
 */
static Uint16 LED_getBlinkTime(blinkLED_e blink) {

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

/**
 * @brief Modifier la couleur de la led indiquant la couleur
 * @param color Couleur de la led couleur
 */
static void LED_setColorLed(led_color_e color) {

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	if(color & 0b100){
		GPIO_InitStructure.GPIO_Pin = LED_RED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_ResetBits(LED_RED_PORT);
	}else{
		GPIO_InitStructure.GPIO_Pin = LED_RED_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}

	if(color & 0b010){
		GPIO_InitStructure.GPIO_Pin = LED_GREEN_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_ResetBits(LED_GREEN_PORT);
	}else{
		GPIO_InitStructure.GPIO_Pin = LED_GREEN_MASK;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}

	if(color & 0b001){
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
