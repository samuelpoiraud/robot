/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : motor.h
 *	Package : Projet Balise R�cepteur US
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit �tre le plus constante possible.
 *	Auteur : Nirgal
 *	Version 201203
 */

#ifndef MOTOR_H
	#define MOTOR_H
	
	#include "QS/QS_all.h"

	
	void MOTOR_init(void);

	void MOTOR_togle_enable(void);

	void MOTOR_process_main(void);

	void MOTOR_send_bat_state(void);
	
	
#endif /* ndef MOTOR_H */
