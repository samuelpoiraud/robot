/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : Global_vars_types.h
 *  Package : Standard_Project
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen
 *  Version 20081130
 */

#ifndef QS_GLOBAL_VARS_H
	#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

/*
 *	inserez ici vos inclusions de fichers et vos definitions de types
 *	nécessaires à Global_vars.h
 */

	typedef enum 
	{ 	
		ETAT_CAN_BALISE_STANDBY,
		ETAT_CAN_BALISE_DEMANDE_D_ENVOI,
		ETAT_CAN_BALISE_ATTENTE_POSITION_DE_NOTRE_ROBOT, 
		ETAT_CAN_BALISE_POSITION_DE_NOTRE_ROBOT_RECUE,
		ETAT_CAN_BALISE_POSITION_DE_NOTRE_ROBOT_NON_RECUE
	} etat_can_balise_e;


	
	
