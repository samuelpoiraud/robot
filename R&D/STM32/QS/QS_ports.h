/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_ports.h
 *  Package : Qualité Soft
 *  Description : Gestion des entrées-sorties GPIO
 *  Auteur : Gwenn
 *  Version 20100418
 */

#ifndef QS_PORTS_H
	#define QS_PORTS_H
	
	#include "QS_all.h"
	
	/** Configure le module GPIO suivant global_config.h
	 **/
	void PORTS_init(void);
	
	
	/** Ecrit une valeur sur les LEDs
		* param value Paramètre sur 8 bits écrit sur les LEDs
	 **/
	void LED_write(Uint8 value);

#endif /* ndef QS_PORTS_H */
