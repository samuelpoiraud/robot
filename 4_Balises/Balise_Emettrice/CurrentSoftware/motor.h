/*
 *	Club Robot ESEO 2012-2015
 *
 *	Fichier : motor.h
 *	Package : Balise Emettrice
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit être le plus constante possible.
 *	Auteur : Nirgal(PIC)/Arnaud(STM32)
 *	Version 201203
 */

#ifndef MOTOR_H
	#define MOTOR_H

	#include "QS/QS_all.h"

	void MOTOR_init(void);

	void MOTOR_process_it(Uint8 period_it);

	Uint8 MOTOR_get_duty_filtered(void);

#endif /* ndef MOTOR_H */
