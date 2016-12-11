#include "switch.h"

#include "../QS/QS_ports.h"
#include "led.h"

#define ANTI_BOUNCE_TIME (20) // en ms

static void SWITCH_stateChanged(void);

void SWITCH_init(void) {

	static bool_e initialized = FALSE;

	if(initialized) {
		return;
	}
}

void SWITCH_processMain(void) {
	static Uint8 previousSwitchColor = BOT_COLOR;
	static time32_t previousTime = 0;

	if((previousTime + ANTI_BOUNCE_TIME) > global.absolute_time){
		if(previousSwitchColor != SWITCH_COLOR_BEACON_EYE){
			SWITCH_stateChanged();
			previousSwitchColor = SWITCH_COLOR_BEACON_EYE;
		}
		previousTime = global.absolute_time;
	}
}

/**
 * @brief Appelé lors d'un changement d'état du switch de la couleur
 */
static void SWITCH_stateChanged(void) {

	// Changement de la couleur
	global.current_color = (global.current_color==BOT_COLOR)?TOP_COLOR:BOT_COLOR;

	// Mise à jour de la led indiquand la couleur
	LED_setColor((global.current_color==BOT_COLOR)?LED_COLOR_MAGENTA:LED_COLOR_GREEN);

	// TODO: Mettre à jour les zones
	// TODO; Rotation de l'écran
}
