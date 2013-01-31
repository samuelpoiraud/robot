/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Secretary.h
 *	Package : Projet Balise Mère
 *	Description : Gestion de la communication avec l'extérieur.
 *	Auteur : Nirgal
 *	Version 201208
 */
 
#ifndef SECRETARY_H
	#define SECRETARY_H
	
	#include "Brain_US.h"
	
	void SECRETARY_init(void);
	
	void SECRETARY_process_main(void);
	
	void SECRETARY_send_adversary_position(bool_e us_ir, adversary_e adversary, Sint16 x, Sint16 y, Uint8 fiability);
		
	void SECRETARY_can_send_beacon_enable_periodic_sending(void);
	
	void SECRETARY_can_send_beacon_disable_periodic_sending(void);
	
#endif /* ndef SECRETARY */
