#include "it.h"

#include "QS/QS_timer.h"
#include "main.h"

#define IT_TIME		1		// en ms (maximum 65)


void IT_init(void){
	TIMER_init();
	TIMER1_run_us(1000*IT_TIME);			//IT clock vitesse de 1ms
}


//Sur interruption timer 1...
void _ISR _T1Interrupt(){

	// On met � jour le temps du match si ce dernier est commenc�
	if(global.flags.match_started && !global.flags.match_over && !global.flags.match_suspended) {
				global.match_time++;
	}

	// On regarde si le match est termin�
	if (!global.flags.match_over && !global.flags.match_suspended) {
		if (90 != 0 && (global.match_time >= (90))) {
			global.flags.match_over = TRUE;
		}
	}

	//ENV_process_it();
	//ZONE_process_it_1ms();
	//LCD_process_it_1ms();
	//SECRETARY_process_ms();
	TIMER1_AckIT();
}
