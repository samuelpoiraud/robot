#include "environment.h"
#include "IHM/led.h"
#include "IHM/switch.h"
#include "IHM/button.h"
#include "LCD/low layer/ssd2119.h"

void ENVIRONMENT_init() {

	// Initialisation des variables globales
	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	global.flags.match_suspended = FALSE;
	global.absolute_time = 0;
	global.match_time = 0;
	global.current_color = BOT_COLOR;

	// Initialisation de l'IHM
	LED_init();
	BUTTON_init();
	SWITCH_init();
}

void ENVIRONMENT_processIt(Uint8 ms) {

	// On met à jour le temps du match si ce dernier est commencé
	if(global.flags.match_started && !global.flags.match_over && !global.flags.match_suspended) {
		global.match_time++;
	}

	// On regarde si le match est terminé
	if (!global.flags.match_over && !global.flags.match_suspended) {
		if (MATCH_DURATION != 0 && (global.match_time >= (MATCH_DURATION))) {
			global.flags.match_over = TRUE;
		}
	}
}

void ENVIRONMENT_processMain() {

	SWITCH_processMain();
	BUTTON_processMain();
}
void ENVIRONMENT_setColor(color_e color) {

	// Changement de la couleur
	global.current_color = color;

	// Mise à jour de la led indiquand la couleur
	LED_setColor((color == TOP_COLOR) ? LED_COLOR_BLUE : LED_COLOR_YELLOW);

	// TODO: Mettre à jour les zones

}
