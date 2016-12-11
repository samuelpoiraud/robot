#include "button.h"

#include "../QS/QS_ports.h"
#include "../QS/QS_outputlog.h"

#define ANTI_BOUNCE_TIME (20) // en ms

static void BUTTON_stateChange0(void);
static void BUTTON_stateChange1(void);

void BUTTON_init(void) {
	static bool_e initialized = FALSE;

	if(initialized) {
		return;
	}
}

void BUTTON_processMain(void) {
	static time32_t previousTime = 0;

#ifdef BUTTON_0_PORT
	static Uint8 previousButton0 = 1;
#endif
#ifdef BUTTON_1_PORT
	static Uint8 previousButton1 = 1;
#endif

	if((previousTime + ANTI_BOUNCE_TIME) < global.absolute_time){

		if(previousButton0 != BUTTON_0_PORT){
			BUTTON_stateChange0();
			previousButton0 = BUTTON_0_PORT;
		}

		if(previousButton1 != BUTTON_1_PORT){
			BUTTON_stateChange1();
			previousButton1 = BUTTON_1_PORT;
		}

		previousTime = global.absolute_time;
	}
}

/**
 * @brief Est appelé lors d'un changement d'état du bouton 0.
 */
static void BUTTON_stateChange0(void) {

	if(!BUTTON_0_PORT){
		// Le bouton est appuyé
		debug_printf("Appuye sur le bouton 0.\n");
	}else{
		// Le bouton est relâché
		debug_printf("Relachement du bouton 0.\n");
	}
}

/**
 * @brief Est appelé lors d'un changement d'état du bouton 1.
 */
static void BUTTON_stateChange1(void) {

	if(!BUTTON_1_PORT){
		// Le bouton est appuyé
		debug_printf("Appuye sur le bouton 1.\n");

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

	}else{
		// Le bouton est relâché
		debug_printf("Relachement du bouton 1.\n");
	}
}
