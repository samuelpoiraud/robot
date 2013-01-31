/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_configCheck.h
 *	Package : QSx86
 *	Description : Vérification de la présence de la configuration du pic 
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#ifndef QS_CONFIGCHECK_H
	#define QS_CONFIGCHECK_H

	#if !(defined(FREQ_10MHZ) || defined(FREQ_INTERNAL_CLK) || defined(FREQ_20MHZ) || defined(FREQ_40MHZ))
		#error "Une frequence d'horloge doit être sélectionnée"
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
	#ifndef PORT_F_IO_MASK
		#error "Le port F n'est pas configuré"
	#endif
	#ifndef PORT_G_IO_MASK
		#error "Le port G n'est pas configuré"
	#endif
	
	#if defined(USE_UART2) && defined(USE_AX12_SERVO)
		#error "Il est interdit d'utiliser le module QS_ax12 et l'UART2 !"
	#endif /* def USE_UART2 */

#endif	/* ndef QS_CONFIGCHECK_H */
