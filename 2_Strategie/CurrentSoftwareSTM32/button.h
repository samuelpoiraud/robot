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
#include "asser_functions.h"

 
#ifndef BUTTON_H
	#define BUTTON_H
	#include "QS/QS_timer.h"
	#include "QS/QS_buttons.h"
	#include "QS/QS_adc.h"
	#include "act_functions.h"
	#include "avoidance.h"

	void BUTTON_init();
	void BUTTON_update(); 
	void BUTTON_start();
	void BUTTON_calibration();
	void BUTTON_change_color();
	void BUTTON_rotation();
	void BUTTON_servo();
	void BUTTON_pi_rotation();
	void BUTTON_translation();
	void SWITCH_change_color(void);
	
#endif /* ndef BUTTON_H */
