/*
 *  Club Robot ESEO 2015
 *
 *  Package : Carte Balise réceptrice
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Arnaud
 */

#ifndef CONFIG_GLOBAL_VARS_TYPES_H
	#define CONFIG_GLOBAL_VARS_TYPES_H

	#include "../QS/QS_types.h"

	typedef enum
	{
		ETAT_CAN_BALISE_STANDBY,
		ETAT_CAN_BALISE_DEMANDE_D_ENVOI,
		ETAT_CAN_BALISE_ATTENTE_POSITION_DE_NOTRE_ROBOT,
		ETAT_CAN_BALISE_POSITION_DE_NOTRE_ROBOT_RECUE,
		ETAT_CAN_BALISE_POSITION_DE_NOTRE_ROBOT_NON_RECUE
	}etat_can_balise_e;


#endif /* ndef CONFIG_GLOBAL_VARS_TYPES_H */
