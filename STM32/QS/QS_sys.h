/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : QS_sys.c
 *	Package : Qualite Soft
 *	Description : Configuration du STM32 - Horloges - Démarrage
 *	Auteur : Gwenn
 *	Version 20100421
 */

/** ----------------  Defines possibles  --------------------
 *	STDOUT_USART				: USART sur lequel sort le flux STDOUT		Par d�faut � 1 (sur l'UART1)
 *	STDERR_USART				: USART sur lequel sort le flux STDERR		Par d�faut � 1 (sur l'UART1)
 *	STDIN_USART					: USART sur lequel rentre le flux STDIN		Par d�faut � 1 (sur l'UART1)
 */


#ifndef QS_SYS_H
	#define QS_SYS_H
	
	#include "QS_all.h"
	
	/**
	 * Configurations essentielles de la carte
	 */
	void SYS_init(void);

#endif /* ndef QS_SYS_H */
