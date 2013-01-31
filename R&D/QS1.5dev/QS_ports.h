/*
 *  Club Robot ESEO 2006 - 2009
 *  Game Hoover, Phoboss, Archi-Tech'
 *
 *  Fichier : QS_ports.h
 *  Package : Qualite Soft
 *  Description : Header de la fonction d'initialisation des ports.
 *  Auteur : Axel Voitier
 *	Revision 2008-2009 : Jacen
 *  Version 20080731
 */

#ifndef QS_PORTS_H
	#define QS_PORTS_H

	#include "QS_all.h"

	void PORTS_init (void);
	void PORTS_adc_init(void);

	#ifdef QS_PORTS_C

		#define CONFIG_ADDPIN(reg_port, reg_ad, pin) { \
		reg_port |= 1 << pin; \
		reg_ad &= ~(1 << pin); \
		}

	#endif /* def QS_PORTS_C */

#endif /* ndef QS_PORTS_H */
