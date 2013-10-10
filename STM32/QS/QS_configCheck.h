/*
 *  Club Robot ESEO 2006 - 2010
 *  Game Hoover, Phoboss, Archi-Tech', PACMAN
 *
 *  Fichier : QS_configCheck.h
 *  Package : Qualité Soft
 *  Description : Verification de la presence d'une configuration
 *					du PIC dans le projet (Global_config.h)
 *  Auteur : Axel Voitier (et un peu savon aussi)
 *	Révision 2008-2009 : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20091014
 */

#ifndef QS_CONFIGCHECK_H
	#define QS_CONFIGCHECK_H

	#ifdef NEW_CONFIG_ORGANISATION
		#include "config_pin.h"
	#endif

	#if !defined(HCLK_FREQUENCY_HZ) || HCLK_FREQUENCY_HZ > 168000000
		#error "Une frequence d'horloge HCLK < 168Mhz doit être selectionnée"
	#endif
	#if !defined(PCLK1_FREQUENCY_HZ) || PCLK1_FREQUENCY_HZ > 42000000
		#error "Une frequence d'horloge PCLK1 < 42Mhz doit être selectionnée"
	#endif
	#if !defined(PCLK2_FREQUENCY_HZ) || PCLK2_FREQUENCY_HZ > 84000000
		#error "Une frequence d'horloge PCLK2 < 84Mhz doit être selectionnée"
	#endif

	#ifndef I_AM
		#error "La carte n'est pas identifiée (cf I_AM dans Global_config.h)"
	#endif

	#ifndef PORT_A_IO_MASK
		#error "Le port A n'est pas configuré"
	#endif
	#ifndef PORT_B_IO_MASK
		#error "Le port B n'est pas configuré"
	#endif
	#ifndef PORT_C_IO_MASK
		#error "Le port C n'est pas configuré"
	#endif
	#ifndef PORT_D_IO_MASK
		#error "Le port D n'est pas configuré"
	#endif
	#ifndef PORT_E_IO_MASK
		#error "Le port E n'est pas configuré"
	#endif
	
	#if defined(USE_UART2) && defined(USE_AX12_SERVO)
		#error "Il est interdit d'utiliser le module QS_ax12 et l'UART2 !"
	#endif /* def USE_UART2 */

#endif	/* ndef QS_CONFIGCHECK_H */
