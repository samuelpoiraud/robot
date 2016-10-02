/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : button.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion du bouton et de la biroute
 *	Auteur : Jacen & Ronan
 *	Version 20100408
 */

#include "QS/QS_all.h"


#ifndef BUTTON_H
	#define BUTTON_H
	#include "QS/QS_all.h"
	#include "QS/QS_timer.h"
	#include "QS/QS_buttons.h"
	#include "QS/QS_adc.h"
	#include "actuator/act_functions.h"
	#include "propulsion/avoidance.h"
	#include "propulsion/prop_functions.h"

	void BUTTON_init();
	void BUTTON_update();
	void BUTTON_start();
	void BUTTON_calibration();
	void BUTTON_change_color();
	void BUTTON_rotation();
	void BUTTON_servo();
	void BUTTON_pi_rotation();
	void BUTTON_translation();
	void BUTTON_0_long_push(void);
	void BUTTON_3_long_push(void);
	void BUTTON_4_long_push(void);
	void BUTTON_5_long_push(void);
	void SWITCH_change_color(void);
	void BOUTTON_go_to_home();

#endif /* ndef BUTTON_H */
