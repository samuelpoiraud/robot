/*
 *	Club Robot ESEO 2009 - 2010 - 2012
 *	
 *
 *	Fichier : secretary.c
 *	Package : Balises 2010
 *	Description : gestion du CAN sur la carte Balise Recepteur
 *	Auteur : Nirgal
 *	Version 201203
 */

#ifndef SECRETARY_H
	#define SECRETARY_H
	
	#include "QS/QS_all.h"
	
	void SECRETARY_init(void);
	void SECRETARY_process_it_100ms();
	void SECRETARY_process_main(void);
	
#endif /* ndef SECRETARY_H */




	
