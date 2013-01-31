/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : PGA.h
 *	Package : Projet Balise
 *	Description : Gestion du PGA (Programmable Gain Amplifier : Ampli à Gain Variable)
 
 *	Auteur : Nirgal
 *	Version 201012
 */

#ifndef PGA_H
	#define PGA_H
	
	#include "QS/QS_all.h"
	#include "QS/QS_timer.h"
	#include "ReceptionUS.h"
	#include <spi.h>
	
	void PGA_maj_ampli(Uint16 distance_estimee);
	void PGA_init(void);
	void PGA_set_ampli(Uint8 ampli);

	#ifdef PGA_C
				
		
	#endif
	
#endif /* ndef SYNCHRO_H */
