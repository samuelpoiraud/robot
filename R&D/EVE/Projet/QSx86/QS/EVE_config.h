/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : EVE_config.h
 *	Package : QSx86
 *	Description : Configurations à modifier selon la carte utilisée
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20120113
 */

#ifndef EVE_CONFIG_H
	#define EVE_CONFIG_H

	#include "QS_all.h"

	// Ce fichier est le seul à modifier en fonction de l'utilisation de la QSx86
	
	// Définition de l'utilisation des interruptions
	#define USE_T1Interrupt
	//#define USE_T2Interrupt
	#define USE_T3Interrupt
	//#define USE_T4Interrupt

#endif /* EVE_CONFIG_H */

