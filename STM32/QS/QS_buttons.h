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

// Switch de la carte IHM
	typedef enum
	{
		SWITCH0=0,
		SWITCH1,
		SWITCH2,
		SWITCH3,
		SWITCH4,
		SWITCH5,
		SWITCH6,
		SWITCH7,
		SWITCH8,
		SWITCH9,
		SWITCH10,
		SWITCH11,
		SWITCH12,
		SWITCH13,
		SWITCH14,
		SWITCH15,
		SWITCH16,
		SWITCH17,
		SWITCH18,
		SWITCHS_NUMBER
	}switch_ihm_e;

// Button de la carte ihm
	typedef enum
	{
		BUTTON0_IHM=0,
		BUTTON1_IHM,
		BUTTON2_IHM,
		BUTTON3_IHM,
		BUTTON4_IHM,
		BUTTON5_IHM,
		BUTTON6_IHM,
		BUTTON7_IHM,
		BUTTONS_IHM_NUMBER
	}button_ihm_e;

	void BUTTONS_init();
	void BUTTONS_define_actions(button_id_e button_id,button_action_t direct_push, button_action_t after_long_push, Uint8 long_push_time);
	void BUTTONS_update();
	void BUTTONS_process_it(void);
	#endif

#endif /* ndef QS_BUTTON_H */
