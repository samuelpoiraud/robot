/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : main.h
 *	Package : Carte Principale
 *	Description : Ordonnancement de la carte Principale
 *	Auteur : Jacen, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"


#ifndef MAIN_H
	#define MAIN_H
	
	#include "QS/QS_ports.h"
	#include "QS/QS_uart.h"
	#include "Stacks.h"
	#include "environment.h"
	#include "brain.h"
	#include "clock.h"
	#include "actions_tests.h"
	#include "actions.h"
	
	void RCON_read();
	void _ISR _MathError();
	void _ISR _StackError();
	void _ISR _AddressError();
	void _ISR _OscillatorFail();
	
#endif /* ndef MAIN_H */
