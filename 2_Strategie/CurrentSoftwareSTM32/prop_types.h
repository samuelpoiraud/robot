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
		NOW=0x00, 
		END_OF_BUFFER=0x10, 
		NO_MULTIPOINT=0x00, 
		MULTIPOINT=0x20,
		ABSOLUTE=0x00, 
		RELATIVE=0x0F
	} PROP_mode_e;
	
	
	typedef enum
	{
		END_AT_LAST_POINT = 0,
		END_AT_BREAK
	} PROP_end_condition_e;

#endif /* ndef PROP_TYPES_H */
