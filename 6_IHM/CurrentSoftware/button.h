/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : button.h
 *	Package : Carte IHM
 *	Description : Contrôle les boutons, code inspiré de QS_button
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "QS/QS_buttons.h"

typedef void(*button_ihm_action_t)(void);

typedef enum
{
	BP_SELFTEST=0,
	BP_CALIBRATION,
	BP_PRINTMATCH,
	BP_OK,
	BP_UP,
	BP_DOWN,
	BP_SET,
	BP_RFU,
	BP_INT_NUMBER
}button_int_id_e;

typedef struct
{
	button_ihm_action_t direct_push;
	button_ihm_action_t after_long_push;
	Uint8 long_push_time;
	bool_e long_push_already_detected;
}button_ihm_t;

void BUTTONS_IHM_init();

// Définit les fonctions associé pour les boutons internes
void BUTTONS_IHM_define_actions(button_int_id_e button_id,button_ihm_action_t direct_push, button_ihm_action_t after_long_push, Uint8 long_push_time);

// Envois un message CAN pour les boutons qui communiquent avec l'extérieure
void BUTTONS_IHM_send_msg(button_ihm_e button_id,bool_e push_long);
void BUTTONS_IHM_update();
void BUTTONS_IHM_process_it(void);
void BUTTONS_IHM_VERBOSE(void);

#endif /* ndef BUTTON_H */
