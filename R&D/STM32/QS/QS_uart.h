/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_uart.h
 *  Package : Qualité Soft
 *  Description : Gestion de l'UART
 *  Auteur : Gwenn
 *  Version 20100419
 */


#ifndef QS_UART_H
	#define QS_UART_H
	
	#include "QS_all.h"
	
	#ifdef USE_UART
	
	
	/**	Réalise l'initialisation du module UART
		* L'UART4 est utilisée et est disponible sur le connecteur fond de panier
	**/
	void UART_init();
	
	
	#ifdef QS_UART_C
		static bool_e initialized = FALSE;
	#endif
	
	#endif /* def USE_UART */
	
#endif /* ndef QS_UART_H */
