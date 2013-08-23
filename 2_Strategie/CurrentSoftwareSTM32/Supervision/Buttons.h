/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : Buttons.h
 *	Package : Supervision
 *	Description : Gestion de l'appui sur le bouton de la carte supervision 
 *	Auteur : Ronan
 *	Version 20100422
 */
 
#include "QS/QS_all.h"

#ifndef BUTTON_H
	#define BUTTON_H
	
	#include "QS/QS_buttons.h"
	#include "Buffer.h"
	#include "Interface.h"
	#include "Eeprom_can_msg.h"
	
	void BUTTON_init();
	void BUTTON_update();
	
		
#endif
