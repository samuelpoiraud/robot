/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Gwenn, millman
 *  Version 20100413
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_vars.h, lire le CQS."
	#endif
	
	bool_e pawn_front_detected;
	bool_e pawn_back_detected;
	bool_e switch_clamp_front;
	bool_e switch_clamp_back;		
#endif /* ndef GLOBAL_VARS_H */
