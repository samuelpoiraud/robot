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
 *  Version 20091014
 */

#ifndef QS_CONFIGCHECK_H
	#define QS_CONFIGCHECK_H

	#if !(defined(FREQ_10MHZ) || defined(FREQ_INTERNAL_CLK) || defined(FREQ_20MHZ) || defined(FREQ_40MHZ))
		#error "Une frequence d'horloge doit �tre s�lectionn�e"
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
	#ifndef PORT_F_IO_MASK
		#error "Le port F n'est pas configur�"
	#endif
	#ifndef PORT_G_IO_MASK
		#error "Le port G n'est pas configur�"
	#endif

#endif	/* ndef QS_CONFIGCHECK_H */
