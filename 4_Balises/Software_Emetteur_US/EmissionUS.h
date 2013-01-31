/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : EmissionUS.h
 *	Package : Balise Récepteur US
 *	Description : 
 *	Auteur : Nirgal
 *	Version 201012
 */

#ifndef EMISSIONUS_H
	#define EMISSIONUS_H
	
	#include "QS/QS_all.h"
	#include "QS/QS_timer.h"
	#include "QS/QS_pwm.h"
	
	void EmissionUS_next_step(void);
	
	void EmissionUS_init(void);
	void EmissionUS_stop(void);
	void EmissionUS_step_init(void);
		

#endif /* ndef EMISSIONUS_H */
