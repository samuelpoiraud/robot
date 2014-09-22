/*
 *	Club Robot 2014
 *
 *	Fichier : main.c
 *	Package : IHM
 *	Description : fonction principale
 *	Auteur : Anthony
 *	Version 20080924
 */


#include "main.h"
#include "QS/QS_all.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_buttons.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_watchdog.h"

#include "button.h"
#include "switch.h"


#if defined (STM32F40XX)
	#include "QS/QS_sys.h"
#endif

void SWITCHS_update(void);
volatile Uint8 t_ms = 0;


void initialisation(void)
{
	#if defined(STM32F40XX)
		SYS_init();
	#endif
	// Config des ports
	PORTS_init();

	volatile Uint32 i;
	for(i=0;i<1000000;i++);	//tempo (env 50ms) pour un bon fonctionnement de l'UART lorsqu'on branche les cartes. Sinon, les premiers printf ne sont pas envoyés -> ????

	UART_init();

	//Doit se faire AVANT ODOMETRY_init() !!!

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//Détermine le robot sur lequel est branchée la carte.
	debug_printf("--- Hello, I'm PROP (%s) ---\n", QS_WHO_AM_I_get_name());


	SWITCHS_init();
	//BUTTONS_define_actions(BUTTON0,&blue_button_action, &calibration_button_action, 1);
	//BUTTONS_define_actions(BUTTON1,&calibration_button_action, NULL, 1);


}

int main (void){
	initialisation();

	while(1){

		if(t_ms > 20)	//Pour éviter les rebonds
		{
			t_ms = 0;
			BUTTONS_update();			//Gestion des boutons
			SWITCHS_update();			//Surveillance des switchs
		}


		#ifdef VERBOSE_MODE
			BUTTONS_IHM_VERBOSE();
			SWITCHS_VERBOSE();
		#endif

		MAIN_process_it(5);
	}

	return 0;
}


void MAIN_process_it(Uint8 ms){
	t_ms += ms;
}
