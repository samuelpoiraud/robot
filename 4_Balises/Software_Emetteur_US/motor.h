/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : motor.h
 *	Package : Projet Balise Récepteur US
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit être le plus constante possible.
 *	Auteur : Nirgal
 *	Version 201203
 */

#ifndef MOTOR_H
	#define MOTOR_H
	
	#include "QS/QS_all.h"

	
	void MOTOR_init(void);

	void MOTOR_togle_enable(void);

	void MOTOR_process_main(void);
	
	
#endif /* ndef MOTOR_H */
