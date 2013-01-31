/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_ports.h
 *  Package : Qualit� Soft
 *  Description : Gestion des entr�es-sorties GPIO
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
		* param value Param�tre sur 8 bits �crit sur les LEDs
	 **/
	void LED_write(Uint8 value);

#endif /* ndef QS_PORTS_H */
