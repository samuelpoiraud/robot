/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : QS_sys.c
 *	Package : Qualite Soft
 *	Description : Configuration du STM32 - Horloges - Démarrage
 *	Auteur : Gwenn
 *	Version 20100421
 */

#ifndef QS_SYS_H
	#define QS_SYS_H
	
	#include "QS_all.h"
	
	/**
		* Configurations essentielles de la carte
		* HSE = 8MHz
		* PLLSRC = 8MHz
		* PLLCLK = 72MHz
		* SYSCLK = 72MHz
		* PCLK1 = 36MHz
		* PCLK2 = 72MHz
	**/
	void SYS_init(void);

#endif /* ndef QS_SYS_H */
