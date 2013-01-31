/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *  Fichier : Global_vars_types.h
 *  Package : Actionneur
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Gwenn,millman
 *  Version 20100327
 */

#ifndef QS_GLOBAL_VARS_H
	#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

	/*Pour distinguer les actionneurs avant et arrière du robot*/
	typedef enum 
	{
		FRONT=0,
		BACK,
		NB_WAY
	}act_way_e;
