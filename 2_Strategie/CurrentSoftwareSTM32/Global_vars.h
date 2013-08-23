/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : Global_vars.h
 *  Package : Carte Principale
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen
 *  Version 20090521
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_vars.h, lire le CQS."
	#endif

	/*	Structure sauvegardant l'evolution de l'environnement 
	 *	(position, fin de mouvement, ...)
	 */

	environment_t env;
		
	volatile Uint8 chrono_long_push; //compteur des quarts de seconde pout détecté un appui égal à 2 secondes
#endif /* ndef GLOBAL_VARS_H */
