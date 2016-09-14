/*
 *	Club Robot ESEO 2011-2012
 *	Shark & Fish
 *
 *	Fichier : QS_can_over_xbee.c
 *	Package : Qualit� Soft
 *	Description : fonctions d'encapsulation des messages CAN
					pour envoi via module XBEE configur� en mode API
 *	Auteur : Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20120224
 */

/** ----------------  Defines possibles  --------------------
 *	USE_XBEE					: Active QS_can_over_xbee
 *	XBEE_PLUGGED_ON_UART1		: Configuration de l'XBEE via l'UART 1
 *	XBEE_PLUGGED_ON_UART2		: Configuration de l'XBEE via l'UART 2
 *	XBEE_PLUGGED_ON_UART3		: Configuration de l'XBEE via l'UART 2
 *	XBEE_PLUGGED_ON_UART4		: Configuration de l'XBEE via l'UART 2
 *	XBEE_PLUGGED_ON_UART5		: Configuration de l'XBEE via l'UART 2
 *	XBEE_PLUGGED_ON_UART6		: Configuration de l'XBEE via l'UART 2
 *	XBEE_SIMULATION				: Envoi des messages CAN du XBEE sur le CAN
 *
 * ----------------  Choses � savoir  --------------------
 *	Il faut d�finir XBEE_PLUGGED_ON_UART1 ou 2/3/4/5/6 obligatoirement
 *	SD_ENABLE					: Sauvegarde sur la SD des messages transit�s sur le XBEE	(Ne marche que sur la carte strat�gie ayant une SD)
 */

// TODO dire que XBEE_RESET doit etre d�finit

#include "../QS_all.h"

#ifndef QS_XBEE_H
	#define	QS_XBEE_H

	#ifdef USE_XBEE

		#if !(defined XBEE_PLUGGED_ON_UART1 || defined XBEE_PLUGGED_ON_UART2 || defined XBEE_PLUGGED_ON_UART3 \
				|| defined XBEE_PLUGGED_ON_UART4 || defined XBEE_PLUGGED_ON_UART5 || defined XBEE_PLUGGED_ON_UART6)
			#error "Vous devez d�finir l'UART o� est branch� le XBEE"
			#error "Pour cela, d�finissez XBEE_PLUGGED_ON_UART1 ou 2/3/4/5/6"
		#endif

		void XBEE_init(void);

		void XBEE_processMain(void);


	#endif // def USE_XBEE

#endif /* ifndef QS_XBEE_H */
