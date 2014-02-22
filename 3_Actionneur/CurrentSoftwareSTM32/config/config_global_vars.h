/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Variables globales d�finies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen
 */

#ifndef CONFIG_GLOBAL_VARS_H
	#define CONFIG_GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement config_global_vars.h, lire le CQS."
	#endif

	bool_e match_started;
	bool_e alim;
	Uint16 alim_value;			// en mV

#endif /* ndef CONFIG_GLOBAL_VARS_H */
