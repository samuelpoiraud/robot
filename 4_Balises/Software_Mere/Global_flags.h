/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Flags définis specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen
 *  Version 20081010
 */

#ifndef GLOBAL_FLAGS_H
	#define GLOBAL_FLAGS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_flags.h, lire le CQS."
	#endif

	volatile bool_e it;
	volatile bool_e timer1, timer2, timer3, timer4;
	/*  Note : Flags communs à tous les codes
	 *	volatile bool_e u1rx;	// message reçu sur uart1
	 *	volatile bool_e u2rx;	// message reçu sur uart2
	 *	volatile bool_e canrx;	// message reçu sur le bus can
	 *	volatile bool_e can2rx;	// message reçu sur le bus can2
	 */

#endif /* ndef GLOBAL_VARS_H */
