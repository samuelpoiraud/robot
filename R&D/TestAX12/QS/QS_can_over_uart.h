/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : QS_can_over_uart.h
 *	Package : Qualit� Soft
 *	Description : fonctions d'encapsulation des messages CAN
					sur uart.
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100111
 */

#include "../QS/QS_all.h"

#ifndef QS_CAN_OVER_UART_H
#define	QS_CAN_OVER_UART_H

#define SOH 0x01
#define EOT 0x04
#define STX 0x02
#define ETX 0x03


#ifdef USE_UART1
	#ifdef USE_UART1RXINTERRUPT
		bool_e u1rxToCANmsg (CAN_msg_t* dest);
		/*	
		 *	cette fonction lit un octet dans le buffer de reception de l'uart1
		 *	et compl�te le message CAN pass� en argument � partir du point o�
		 *	elle s'est arret�e � son pr�c�dent appel. Elle renvoie ensuite si
		 *	oui ou non elle a fini de compl�ter le message CAN. Elle v�rifie 
		 *  aussi si le message est bien conforme au protocole de communication
		 *  (cf QS)
		 */
	#endif /* def USE_UART1RXINTERRUPT */
	void CANmsgToU1tx (CAN_msg_t* src);
	/*
	 *	Ecrit le contenu du message CAN pass� en argument sur
	 *	l'uart1 en ajoutant l'octet SOH en d�but de message et l'octet EOT
	 *	en fin de message (cf : protocole de communication QS)
	 */
#endif /* def USE_UART1 */

#ifdef USE_UART2
	#ifdef USE_UART2RXINTERRUPT
		 bool_e u2rxToCANmsg (CAN_msg_t* dest);
		/*	
		 *	cette fonction lit un octet dans le buffer de reception de l'uart2
		 *	et compl�te le message CAN pass� en argument � partir du point o�
		 *	elle s'est arret�e � son pr�c�dent appel. Elle renvoie ensuite si
		 *	oui ou non elle a fini de compl�ter le message CAN. Elle v�rifie 
		 *  aussi si le message est bien conforme au protocole de communication
		 *  (cf QS)
		 */
	#endif /* def USE_UART1RXINTERRUPT */
	void CANmsgToU2tx (CAN_msg_t* src);
	/*
	 *	Ecrit le contenu du message CAN pass� en argument sur
	 *	l'uart2 en ajoutant l'octet SOH en d�but de message et l'octet EOT
	 *	en fin de message (cf : protocole de communication QS)
	 */
#endif /* def USE_UART2 */
#endif /* ifndef QS_CAN_OVER_UART_H */

#ifdef USE_UART1
#ifdef USE_UART2

	void CANmsgToU1txAndU2tx (CAN_msg_t* src);	
	
#endif	/* def USE_UART1*/
#endif	/* def USE_UART2*/
