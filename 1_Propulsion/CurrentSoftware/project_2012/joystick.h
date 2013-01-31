 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : joystick.h
 *  Package : Propulsion
 *  Description : Gestion du joystick permettant un pilotage manuel du robot
 *  Auteur : 2009 Nirgal
 *  Version 201203
 */

#ifndef _JOYSTICK_H
#define _JOYSTICK_H

	#include "QS/QS_all.h"

	void JOYSTICK_init(void);
	bool_e JOYSTICK_is_enable(void);
	void JOYSTICK_process_it(void);
	Sint8 JOYSTICK_get_duty_left(void);
	Sint8 JOYSTICK_get_duty_right(void);
	void JOYSTICK_enable(Uint8 ms, Sint8 duty_left, Sint8 duty_right, bool_e buttonA, bool_e buttonB);

#endif

