/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_servo.h
 *	Package : Qualite Software
 *	Description : Driver par timer pour servomoteur
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#ifndef QS_SERVO_H
#define QS_SERVO_H
#include "QS_all.h"
#include "QS_timer.h"

#ifdef USE_SERVO
	/*-------------------------------------
		Configuration des servomoteurs
	-------------------------------------*/

	// Initialisation des servomoteurs
	void SERVO_init() ;

	// Changement de la commande (entre MIN et MAX_COMMANDE_SERVO us)
	void SERVO_set_cmd(Uint16 cmd, Uint8 num_servo);

#endif /* def USE_SERVO */	
#endif /* ndef QS_SERVO_H */
