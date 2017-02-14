#include "switch.h"

#include "../QS/QS_ports.h"
#include "../environment.h"
#include "../QS/QS_outputlog.h"
#include "terminal.h"

#define ANTI_BOUNCE_TIME (20) // en ms

static void SWITCH_stateChanged(void);

void SWITCH_init(void) {

	static bool_e initialized = FALSE;

	if(initialized) {
		return;
	}

	SWITCH_stateChanged();
}

void SWITCH_processMain(void) {
	static Uint8 previousSwitchColor = 2;
	static time32_t previousTime = 0;

	if((previousTime + ANTI_BOUNCE_TIME) < global.absolute_time){
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

	if(SWITCH_COLOR_BEACON_EYE == TRUE) {
		ENVIRONMENT_setColor(TOP_COLOR);
	} else {
		ENVIRONMENT_setColor(BOT_COLOR);
	}

}
