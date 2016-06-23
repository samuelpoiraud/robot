/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : QS_button.c
 *	Package : QS
 *	Description : Fonctions de gestion des boutons de la carte generique
 *  Auteur : Adrien GIRARD
 *	Auteurs Originaux : Jacen & Ronan
 *	Version 20110324
 */

/** ----------------  Defines possibles  --------------------
 *	USE_BUTTONS					: Activation de QS_buttons
 *
 *	BUTTONS_TIMER				: Numéro du timer choisi pour le fonctionement de QS_buttons
 *	BUTTONS_TIMER_USE_WATCHDOG	: Utiliser watchdog plutôt qu'un timer pour le fonctionnement de QS_buttons
 *
 *	BUTTON0_PORT				: Port du bouton 0
 *	BUTTON1_PORT				: Port du bouton 1
 *	BUTTON2_PORT				: Port du bouton 2
 *	BUTTON3_PORT				: Port du bouton 3
 *	BUTTON4_PORT				: Port du bouton 4
 *	BUTTON5_PORT				: Port du bouton 5
 *	BUTTON6_PORT				: Port du bouton 6
 *	BUTTON7_PORT				: Port du bouton 7
 *
 * ----------------  Choses à savoir  --------------------
 * Si vous ne definisez ni BUTTONS_TIMER, ni BUTTONS_TIMER_USE_WATCHDOG il vous faudra
 * vous même appeller la fonction BUTTONS_process_it() pour le bon fonctionnement de QS_buttons
 */

#include "QS_all.h"

#ifndef QS_BUTTONS_H
	#define QS_BUTTONS_H

	#ifdef USE_BUTTONS

	#include "QS_timer.h"

	typedef void(*button_action_t)(void);

	typedef enum
	{
		BUTTON0=0,
		BUTTON1,
		BUTTON2,
		BUTTON3,
		BUTTON4,
		BUTTON5,
		BUTTON6,
		BUTTON7,
		BUTTONS_NUMBER
	}button_id_e;

	typedef struct
	{
		button_action_t direct_push;
		button_action_t after_long_push;
		Uint8 long_push_time;
		bool_e long_push_already_detected;
	}button_t;

	void BUTTONS_init();
	void BUTTONS_define_actions(button_id_e button_id,button_action_t direct_push, button_action_t after_long_push, Uint8 long_push_time);
	void BUTTONS_update();
	void BUTTONS_process_it(void);
	#endif

#endif /* ndef QS_BUTTON_H */
