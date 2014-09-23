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
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "it.h"
#include "button.h"
#include "switch.h"


#if defined (STM32F40XX)
	#include "QS/QS_sys.h"
#endif

volatile Uint8 t_ms = 0;

void SWITCH_change_color();

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
	CAN_init();

	IT_init();

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//Détermine le robot sur lequel est branchée la carte.
	debug_printf("--- Hello, I'm IHM (%s) ---\n", QS_WHO_AM_I_get_name());

	BUTTONS_IHM_init();
	//BUTTONS_define_actions(BUTTON0,&blue_button_action, &calibration_button_action, 1);
	//BUTTONS_define_actions(BUTTON1,&calibration_button_action, NULL, 1);

	SWITCHS_init();
	SWITCHS_INT_define_actions(SW_COLOR,&SWITCH_change_color);
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

	}

	return 0;
}


void MAIN_process_it(Uint8 ms){
	t_ms += ms;
}

void SWITCH_change_color(){
	CAN_msg_t msg;
	msg.sid = BROADCAST_COULEUR;
	msg.data[0] = ((SWITCH_COLOR==1)?RED:BLUE);
	msg.size=1;
	CAN_send(&msg);
	debug_printf("COLOR\r\n");
}
