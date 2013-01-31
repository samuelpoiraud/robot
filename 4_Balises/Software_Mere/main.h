/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : main.h
 *	Package : Projet Standard
 *	Description : fonction principale d'exemple pour le projet 
 *				standard construit par la QS pour exemple, permettant
 *				l'appel des fonctions de test
 *	Auteur : Jacen
 *	Version 20081010
 */

#ifndef MAIN_H
	#define MAIN_H
	
	#include "QS/QS_all.h"


	typedef enum
	{
		ADVERSARY_1 = 0,
		ADVERSARY_2,
		ADVERSARY_NUMBER
	}adversary_e;

	
	void initialisation (void);

//	#define abs(a)	(a>0?a:-a)
//	#define manhattan_dist16(xa,ya,xb,yb)	(abs((Sint16)yb-(Sint16)ya)+abs((Sint16)xb-(Sint16)xa))
//	#define dist_x16(xa,xb)	abs((Sint16)xb-(Sint16)xa)
//	#define dist_y16(ya,yb)	abs((Sint16)yb-(Sint16)ya)

#endif /* ndef MAIN_H */
