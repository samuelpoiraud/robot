/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_buttons.H
 *	Package : QSx86
 *	Description : Fonctions de gestion des boutons de la carte générique
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#include "QS_all.h"
 
#ifndef QS_BUTTONS_H
	#define QS_BUTTONS_H
	
	#ifdef USE_BUTTONS

	#include "QS_timer.h"
	
	#define BUTTONS_NUMBER 4
	
	typedef void(*button_action_t)(void);

	typedef enum 
	{
		BUTTON1=0,
		BUTTON2,
		BUTTON3,
		BUTTON4
	}button_id_e;

	typedef struct
	{
		button_action_t direct_push;
	 	button_action_t after_long_push;
		Uint8 long_push_time;
	}button_t;

	void BUTTONS_init();
	void BUTTONS_define_actions(button_id_e button_id,button_action_t direct_push, button_action_t after_long_push, Uint8 long_push_time);
	void BUTTONS_update();
	
	#endif
	
#endif /* ndef QS_BUTTON_H */
