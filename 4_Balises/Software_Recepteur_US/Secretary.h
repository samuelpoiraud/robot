/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Secretary.h
 *	Package : Projet Balise Récepteur US
 *	Description : Gestion de la communication avec l'extérieur.
 *	Auteur : Nirgal
 *	Version 201205
 */
 
#ifndef SECRETARY_H
	#define SECRETARY_H
	
	#include "ReceptionUS.h"
	
	void SECRETARY_init(void);
	
	void SECRETARY_process_main(void);

	void SECRETARY_add_datas(adversary_e current_adversary, Uint16 distance, Uint8 fiability);
	
	
#endif /* ndef SECRETARY */
