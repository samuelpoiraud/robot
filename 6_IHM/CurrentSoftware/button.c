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
#define TIME_LONG_PUSH  1000	// (en ms) Va attendre 1s avant d'executer le long push
#define TIME_REPEAT		200		// (en ms) Une fois le long push effectué, il se répete tous les 200ms

// Pour les buttons qui communiquent avec le monde extérieure
static bool_e buttons[BP_NUMBER_IHM]; // Pour détecter les appuies long, pour la répitition évite de rentrer dans directe push à la fin de l'appuie
static Uint16 push_time[BP_NUMBER_IHM]={0};

// Au démarrage, n'envoyera pas de messsage CAN sur l'état des boutons
static bool_e initialized = FALSE;

void BUTTONS_IHM_init(){
	Uint8 i;

	if(initialized)
		return;

	// Init buttons du monde extérieure
	for(i=0;i<BP_NUMBER_IHM;i++){
		buttons[i] = FALSE;
	}

	initialized = TRUE;
}


void BUTTONS_IHM_update(){
	static Uint16 buttons_were_pressed = 0x00; //état des buttons dans le passage précédent
	Uint16 buttons_pressed = 0x00; //état des buttons
	Uint16 buttons_rising_edge; //état des buttons
	Uint16 buttons_falling_edge; //état des boutons
	Uint8 i;


	// Gère tous les buttons qui envoient un message vers le monde extérieure
	buttons_pressed = 0;
#ifdef BUTTON_SELFTEST_PORT
	buttons_pressed = (BUTTON_SELFTEST_PORT)? buttons_pressed|1 : buttons_pressed;
#endif
#ifdef BUTTON_CALIBRATION_PORT
	buttons_pressed = (BUTTON_CALIBRATION_PORT)? buttons_pressed|(1<<1) : buttons_pressed;
#endif
#ifdef BUTTON_PRINTMATCH_PORT
	buttons_pressed = (BUTTON_PRINTMATCH_PORT)? buttons_pressed|(1<<2) : buttons_pressed;
#endif
#ifdef BUTTON_OK_PORT
	buttons_pressed = (BUTTON_OK_PORT)? buttons_pressed|(1<<3) : buttons_pressed;
#endif
#ifdef BUTTON_UP_PORT
	buttons_pressed = (BUTTON_UP_PORT)? buttons_pressed|(1<<4) : buttons_pressed;
#endif
#ifdef BUTTON_DOWN_PORT
	buttons_pressed = (BUTTON_DOWN_PORT)? buttons_pressed|(1<<5) : buttons_pressed;
#endif
#ifdef BUTTON_SET_PORT
	buttons_pressed = (BUTTON_SET_PORT)? buttons_pressed|(1<<6) : buttons_pressed;
#endif
#ifdef BUTTON_RFU_PORT
	buttons_pressed = (BUTTON_RFU_PORT)? buttons_pressed|(1<<7) : buttons_pressed;
#endif
#ifdef BUTTON0_PORT
	buttons_pressed = (BUTTON0_PORT)? buttons_pressed|(1<<8) : buttons_pressed;
#endif
#ifdef BUTTON1_PORT
	buttons_pressed = (BUTTON1_PORT)? buttons_pressed|(1<<9) : buttons_pressed;
#endif
#ifdef BUTTON2_PORT
	buttons_pressed = (BUTTON2_PORT)? buttons_pressed|(1<<10) : buttons_pressed;
#endif
#ifdef BUTTON3_PORT
	buttons_pressed = (BUTTON3_PORT)? buttons_pressed|(1<<11) : buttons_pressed;
#endif
#ifdef BUTTON4_PORT
	buttons_pressed = (BUTTON4_PORT)? buttons_pressed|(1<<12) : buttons_pressed;
#endif
#ifdef BUTTON5_PORT
	buttons_pressed = (BUTTON5_PORT)? buttons_pressed|(1<<13) : buttons_pressed;
#endif


	//détection des fronts montant
	buttons_rising_edge = (~buttons_were_pressed) & buttons_pressed;

	for(i=0;i<BP_NUMBER_IHM;i++){
		if(buttons_rising_edge&(1<<i)){ // Détecte les fronts montant sur chaque bouton
			push_time[i] = TIME_LONG_PUSH;
			buttons[i] = FALSE;
		}
		else{
			buttons_falling_edge = buttons_were_pressed & (~buttons_pressed);

			if((buttons_pressed & (1<<i)) && push_time[i] == 0 && initialized){// long push
				buttons[i] = TRUE;
				push_time[i] = TIME_REPEAT;
				BUTTONS_IHM_send_msg(i,TRUE);

			#ifdef VERBOSE_BOUTON
				switch(i){
					case BP_SELFTEST_IHM:	debug_printf("bp_selftest, long push\n"); break;
					case BP_CALIBRATION_IHM:debug_printf("bp_calibration, long push\n");break;
					case BP_PRINTMATCH_IHM:	debug_printf("bp_print_match, long push\n");break;
					case BP_OK_IHM:			debug_printf("bp_ok, long push\n");break;
					case BP_UP_IHM:			debug_printf("bp_up, long push\n");break;
					case BP_DOWN_IHM:		debug_printf("bp_down, long push\n");break;
					case BP_SET_IHM:		debug_printf("bp_set, long push\n");break;
					case BP_RFU_IHM	:		debug_printf("bp_rfu, long push\n");break;
					default:				debug_printf("Button %d active, long push\n",i);
				}
			#endif
			}
			else if((buttons_falling_edge & (1<<i)) && initialized && !buttons[i]){ // direct push
				BUTTONS_IHM_send_msg(i,FALSE);

			#ifdef VERBOSE_BOUTON
				switch(i){
					case BP_SELFTEST_IHM:	debug_printf("bp_selftest, direct push\n"); break;
					case BP_CALIBRATION_IHM:debug_printf("bp_calibration, direct push\n");break;
					case BP_PRINTMATCH_IHM:	debug_printf("bp_print_match, direct push\n");break;
					case BP_OK_IHM:			debug_printf("bp_ok, direct push\n");break;
					case BP_UP_IHM:			debug_printf("bp_up, direct push\n");break;
					case BP_DOWN_IHM:		debug_printf("bp_down, direct push\n");break;
					case BP_SET_IHM:		debug_printf("bp_set, direct push\n");break;
					case BP_RFU_IHM	:		debug_printf("bp_rfu, direct push\n");break;
					default:				debug_printf("Button %d active, direct push\n",i);
				}
			#endif
			}

		}
	}

	buttons_were_pressed = buttons_pressed;
}

void BUTTONS_IHM_send_msg(button_ihm_e button_id,bool_e push_long){
	CAN_msg_t msg;
	msg.size = 2;
	msg.sid = IHM_BUTTON;
	msg.data[0] = button_id;
	msg.data[1] = push_long;
	CAN_send(&msg);
}


void BUTTONS_IHM_process_it(Uint8 ms){
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
