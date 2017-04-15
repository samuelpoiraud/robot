/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : button.c
 *	Package : Carte IHM
 *	Description : Contrôle les boutons, code inspiré de QS_button
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#include "button.h"

#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"

// Temps pour les buttons externes pour un appuie long
#define TIME_LONG_PUSH  1000	// (en ms) Va attendre 1s avant d'executer le long push et le premier long push
#define TIME_REPEAT		200		// (en ms) Une fois le long push effectué, il se répete tous les 200ms

// Pour les buttons qui communiquent avec le monde extérieure
static bool_e buttons[BP_NUMBER_IHM]; // Pour détecter les appuies long, pour la répitition évite de rentrer dans directe push à la fin de l'appuie
static bool_e buttons_first_long[BP_NUMBER_IHM]; // Pour le premier long push appuye plus rapide par la suite
static Sint16 push_time[BP_NUMBER_IHM]={0};

// Au démarrage, n'envoyera pas de messsage CAN sur l'état des boutons
static bool_e initialized = FALSE;

void BUTTONS_init(){
	Uint8 i;

	if(initialized)
		return;

	// Init buttons du monde extérieure
	for(i=0;i<BP_NUMBER_IHM;i++){
		buttons[i] = FALSE;
		buttons_first_long[i] = FALSE;
	}

	initialized = TRUE;
}


void BUTTONS_update(){
	static Uint16 buttons_were_pressed = 0xFFFF; //état des buttons dans le passage précédent
	Uint16 buttons_pressed = 0xFFFF; //état des buttons
	Uint16 buttons_rising_edge; //état des buttons
	Uint16 buttons_falling_edge; //état des boutons
	Uint8 i;

	// Gère tous les buttons qui envoient un message vers le monde extérieure
	buttons_pressed = 0;
	#ifdef  BUTTON_SELFTEST_PORT
		buttons_pressed |= (BUTTON_SELFTEST_PORT)? 		(1<<BP_SELFTEST_IHM)			: 0;
	#endif
	#ifdef BUTTON_CALIBRATION_PORT
		buttons_pressed |= (BUTTON_CALIBRATION_PORT)? 	(1<<BP_CALIBRATION_IHM)			: 0;
	#endif
	#ifdef BUTTON_PRINTMATCH_PORT
		buttons_pressed |= (BUTTON_PRINTMATCH_PORT)? 	(1<<BP_PRINTMATCH_IHM)			: 0;
	#endif
	#ifdef BUTTON_SUSPEND_RESUMSE_PORT
		buttons_pressed |= (BUTTON_SUSPEND_RESUMSE_PORT)? 	(1<<BP_SUSPEND_RESUME_MATCH_IHM) : 0;
	#endif
	#ifdef BUTTON0_PORT
		buttons_pressed |= (BUTTON0_PORT)? 				(1<<BP_0_IHM)					: 0;
	#endif
	#ifdef BUTTON1_PORT
		buttons_pressed |= (BUTTON1_PORT)? 				(1<<BP_1_IHM)					: 0;
	#endif
	#ifdef BUTTON2_PORT
		buttons_pressed |= (BUTTON2_PORT)? 				(1<<BP_2_IHM)					: 0;
	#endif
	#ifdef BUTTON3_PORT
		buttons_pressed |= (BUTTON3_PORT)? 				(1<<BP_3_IHM)					: 0;
	#endif
	#ifdef BUTTON4_PORT
		buttons_pressed |= (BUTTON4_PORT)? 				(1<<BP_4_IHM)					: 0;
	#endif
	#ifdef BUTTON5_PORT
		buttons_pressed |= (BUTTON5_PORT)? 				(1<<BP_5_IHM)					: 0;
	#endif


	//détection des fronts montant
	buttons_falling_edge = buttons_were_pressed & (~buttons_pressed);

	for(i=0;i<BP_NUMBER_IHM;i++){

		if(buttons_falling_edge & (1<<i)){ // Détecte les fronts descendant sur chaque bouton
			push_time[i] = TIME_LONG_PUSH;
			buttons[i] = FALSE;
			buttons_first_long[i] = FALSE;
		}
		else{
			buttons_rising_edge = (~buttons_were_pressed) & buttons_pressed;

			if((((~buttons_pressed) & (1<<i)) && push_time[i] <= 0) && initialized){// long push
				if(!buttons_first_long[i]){ // First long push
					buttons_first_long[i] = TRUE;
					push_time[i] = TIME_LONG_PUSH;
				}else
					push_time[i] = TIME_REPEAT;

				buttons[i] = TRUE;
				BUTTONS_send_msg(i,TRUE);
			}else if((buttons_rising_edge & (1<<i)) && initialized && !buttons[i]){ // direct push
				BUTTONS_send_msg(i,FALSE);
			}
		}
	}

	buttons_were_pressed = buttons_pressed;
}

void BUTTONS_send_msg(button_ihm_e button_id,bool_e push_long){
	CAN_msg_t msg;
	msg.sid = IHM_BUTTON;
	msg.size = SIZE_IHM_BUTTON;
	msg.data.ihm_button.id = button_id;
	msg.data.ihm_button.long_push = push_long;
	CAN_send(&msg);
}


void BUTTONS_process_it(Uint8 ms){
	Uint8 i;
	for(i=0;i<BP_NUMBER_IHM;i++){
		if(push_time[i] != 0){
			if(push_time[i] < ms)
				push_time[i] = 0;
			else
				push_time[i] -= (Uint8)(ms);
		}
	}
}
