/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : Global_vars_types.h
 *  Package : balises
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen
 *  Version 20081205
 */

#ifndef QS_GLOBAL_VARS_H
	#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

/*
 *	inserez ici vos inclusions de fichiers et vos definitions de types
 *	nécessaires à Global_vars.h
 */
	typedef struct
	{
		volatile Sint16 debut;	//instant de début de détection
		volatile Sint16 fin;	//instant de fin de détection
	} detection;
	
