/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_servo.h
 *	Package : Qualite Software
 *	Description : Driver par timer pour servomoteur
 *	Auteur : Jacen / Anthony(2014)
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#ifndef QS_SERVO_H
#define QS_SERVO_H
#include "QS_all.h"

#ifdef USE_SERVO
	/*-------------------------------------
		Configuration des servomoteurs
	-------------------------------------*/

	typedef enum{
		SERVO_FAST,
		SERVO_SLOW,
		SERVO_SNAIL
	}SERVO_speed_e;

	// Initialisation des servomoteurs
	void SERVO_init() ;

	// Changement de la commande (entre MIN et MAX_COMMANDE_SERVO us)
	void SERVO_set_cmd(Uint16 cmd, Uint8 num_servo);

	void SERVO_set_speed(SERVO_speed_e speed, Uint8 num_servo);

#endif /* def USE_SERVO */
#endif /* ndef QS_SERVO_H */
