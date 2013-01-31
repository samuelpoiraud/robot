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
 *	Version 20100620
 */

#ifndef SYNCHRO_H
	#define SYNCHRO_H
	
	#include "QS/QS_all.h"
	#include "QS/QS_extern_it.h"
	
	void Synchro_init(void);
	void Synchro_process_main(void);
	bool_e SYNCHRO_get_synchro_received(void);
	

#endif /* ndef SYNCHRO_H */
