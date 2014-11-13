/*
 *  Club Robot ESEO 2006 - 2010
 *  Game Hoover, Phoboss, Archi-Tech', PACMAN
 *
 *  Fichier : QS_configCheck.h
 *  Package : Qualit� Soft
 *  Description : Verification de la presence d'une configuration
 *					du PIC dans le projet (Global_config.h)
 *  Auteur : Axel Voitier (et un peu savon aussi)
 *	R�vision 2008-2009 : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20091014
 */

#ifndef QS_CONFIGCHECK_H
	#define QS_CONFIGCHECK_H

	#include "QS_all.h"

	#include "../config/config_pin.h"

	#if !defined(HCLK_FREQUENCY_HZ) || HCLK_FREQUENCY_HZ > 168000000
		#error "Une frequence d'horloge HCLK < 168Mhz doit �tre selectionn�e"
	#endif
	#if !defined(PCLK1_FREQUENCY_HZ) || PCLK1_FREQUENCY_HZ > 42000000
		#error "Une frequence d'horloge PCLK1 < 42Mhz doit �tre selectionn�e"
	#endif
	#if !defined(PCLK2_FREQUENCY_HZ) || PCLK2_FREQUENCY_HZ > 84000000
		#error "Une frequence d'horloge PCLK2 < 84Mhz doit �tre selectionn�e"
	#endif

	#ifndef I_AM
		#error "La carte n'est pas identifi�e (cf I_AM dans Global_config.h)"
	#endif

	#ifndef PORT_A_IO_MASK
		#error "Le port A n'est pas configur�"
	#endif
	#ifndef PORT_B_IO_MASK
		#error "Le port B n'est pas configur�"
	#endif
	#ifndef PORT_C_IO_MASK
		#error "Le port C n'est pas configur�"
	#endif
	#ifndef PORT_D_IO_MASK
		#error "Le port D n'est pas configur�"
	#endif
	#ifndef PORT_E_IO_MASK
		#error "Le port E n'est pas configur�"
	#endif

	#if defined(USE_UART2) && defined(USE_AX12_SERVO)
		#error "Il est interdit d'utiliser le module QS_ax12 et l'UART2 !"
	#endif /* def USE_UART2 */

#endif	/* ndef QS_CONFIGCHECK_H */
