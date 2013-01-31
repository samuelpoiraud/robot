/*
 *  Club Robot ESEO 2006 - 2009
 *  Game Hoover, Phoboss, Archi-Tech'
 *
 *  Fichier : QS_types.h
 *  Package : Qualité Soft
 *  Description : Définition des types pour tout code du robot
 *  Auteur : Axel Voitier (et un peu savon aussi)
 *	Revision 2008-2009 : Jacen
 *  Version 20081128
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
		FALSE,
		TRUE
	} bool_e;
	
	typedef struct 
	{
		Uint11 sid;
		Uint8 data[8];
		Uint8 size;
	}CAN_msg_t;

	typedef enum 
	{ 	
		CARTE_ASSER, 
		CARTE_ACT1, 
		CARTE_ACT2, 
		CARTE_P, 
		CARTE_SUPER,
		CARTE_CAPTEUR,
		CARTE_BALISE
	} cartes_e;

#endif /* ndef QS_TYPES_H */
