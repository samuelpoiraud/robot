/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_types.h
 *	Package : QSx86
 *	Description : Définition des types du code
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#ifndef QS_TYPES_H
	#define QS_TYPES_H

	/* Type de base pour le dsPIC */
	typedef unsigned char Uint8;
	typedef signed char Sint8;
	typedef unsigned short Uint16;
	typedef signed short Sint16;
	typedef unsigned long Uint32;
	typedef signed long Sint32;
	/* Type pour les SID du protocole CAN */
	typedef Uint16	Uint11;

	typedef enum 
	{
		FALSE=0,
		TRUE
	} bool_e;
	
	typedef enum
	{
		RED=0, PURPLE=1
	} color_e;
	
	typedef struct 
	{
		Uint11 sid;
		Uint8 data[8];
		Uint8 size;
	}CAN_msg_t;

	typedef enum 
	{ 	
		CARTE_ASSER, 
		CARTE_ACT, 
		CARTE_STRAT,
		CARTE_SUPER,
		CARTE_BALISE
	} cartes_e;

#endif /* ndef QS_TYPES_H */
