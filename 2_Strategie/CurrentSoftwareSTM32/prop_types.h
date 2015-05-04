/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : prop_types.h
 *	Package : Carte Principale
 *	Description : 	types spécifiques de l'asser
 *	Auteur : Jacen
 *	Version 20100509
 */

#include "QS/QS_all.h"

#ifndef PROP_TYPES_H
#define PROP_TYPES_H

	typedef enum
	{
		NOW=0x00,				//Facultatif... (par défaut)
		END_OF_BUFFER=0x10,
		NO_MULTIPOINT=0x00,		//Facultatif... (par défaut)
		MULTIPOINT=0x20,
		ABSOLUTE=0x00,			//Facultatif... (par défaut)
		RELATIVE=0x01,
		BORDER_MODE=0x02,
		NO_ACKNOWLEDGE=0x40,	//On peut demander qu'il n'y ait pas d'acquittement
		ACKNOWLEDGE=0x00,		//Facultatif... (par défaut)
		NO_BORDER_MODE=0x00		//Facultatif... (par défaut)
	} PROP_mode_e;


	typedef enum
	{
		END_AT_LAST_POINT = 0,
		END_AT_BREAK
	} PROP_end_condition_e;

#endif /* ndef PROP_TYPES_H */
