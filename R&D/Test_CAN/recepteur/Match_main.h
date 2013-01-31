/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Match_main.h
 *	Package : Projet Standard
 *	Description : fonction principale d'exemple pour le projet 
 *				standard construit par la QS pour exemple, pour
 *				utilisation en Match
 *	Auteur : Jacen
 *	Version 20080924
 */

#ifndef MATCH_MAIN_H
	#define MATCH_MAIN_H
	
	#include "../QS/QS_all.h"
	#ifdef MATCH_MODE
		#include "../QS/QS_ports.h"
		#include "../QS/QS_pwm.h"
		#include "../QS/QS_uart.h"
		#include <stdio.h>
	#endif /* def MATCH_MODE */
#endif /* ndef MATCH_MAIN_H */
