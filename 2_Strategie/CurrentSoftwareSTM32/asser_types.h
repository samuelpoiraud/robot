/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : asser_types.h
 *	Package : Carte Principale
 *	Description : 	types spécifiques de l'asser
 *	Auteur : Jacen
 *	Version 20100509
 */

#include "QS/QS_all.h"

#ifndef ASSER_TYPES_H
#define ASSER_TYPES_H

	typedef enum
	{
		NOW=0x00, 
		END_OF_BUFFER=0x10, 
		NO_MULTIPOINT=0x00, 
		MULTIPOINT=0x20,
		ABSOLUTE=0x00, 
		RELATIVE=0x0F
	} ASSER_mode_e;
	
	
	typedef enum
	{
		END_AT_LAST_POINT = 0,
		END_AT_BREAK
	} ASSER_end_condition_e;

#endif /* ndef ASSER_TYPES_H */
