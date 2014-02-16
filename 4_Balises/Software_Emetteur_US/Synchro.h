/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : main.h
 *	Package : Projet Standard
 *	Description : fonction principale d'exemple pour le projet 
 *				standard construit par la QS pour exemple, permettant
 *				l'appel des fonctions de test
 *	Auteur : Jacen
 *	Version 20100620
 */

#ifndef SYNCHRO_H
	#define SYNCHRO_H
	
	#include "QS/QS_all.h"
	#include "QS/QS_extern_it.h"

	#define DUREE_STEP		2000	//Durée d'un step [us]
	
	void Synchro_init(void);
	void Synchro_process_main(void);
	
	#ifdef SYNCHRO_C
		#include "QS/QS_timer.h"
		#include "EmissionUS.h"
		#include "main.h"
		
		void Synchro_correction_deviation_timer(void);

		Sint16 Synchro_moyenne_mobile(Sint16 nouvelle_valeur);
		
	#endif
	
#endif /* ndef SYNCHRO_H */
