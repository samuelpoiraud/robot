/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_ports.c
 *	Package : QSx86
 *	Description : Fonctions d'initialisation des ports
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#include "QS_ports.h"

#define CONFIG_ADDPIN(reg_port, reg_ad, pin)	\
		{ \
			reg_port |= 1 << pin; \
			reg_ad &= ~(1 << pin); \
		}


void PORTS_init (void) 
{
	
}

void PORTS_adc_init(void)
{
	
}
