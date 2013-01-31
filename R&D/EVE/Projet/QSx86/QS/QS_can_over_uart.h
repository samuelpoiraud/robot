/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_can_over_uart.h
 *	Package : QSx86
 *	Description : Fonctions d'encapsulation des messages CAN sur UART
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
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
		 *	et complète le message CAN passé en argument à partir du point où
		 *	elle s'est arretée à son précédent appel. Elle renvoie ensuite si
		 *	oui ou non elle a fini de complèter le message CAN. Elle vérifie 
		 *  aussi si le message est bien conforme au protocole de communication
		 *  (cf QS)
		 */
	#endif /* def USE_UART1RXINTERRUPT */
	void CANmsgToU1tx (CAN_msg_t* src);
	/*
	 *	Ecrit le contenu du message CAN passé en argument sur
	 *	l'uart1 en ajoutant l'octet SOH en début de message et l'octet EOT
	 *	en fin de message (cf : protocole de communication QS)
	 */
#endif /* def USE_UART1 */

#ifdef USE_UART2
	#ifdef USE_UART2RXINTERRUPT
		 bool_e u2rxToCANmsg (CAN_msg_t* dest);
		/*	
		 *	cette fonction lit un octet dans le buffer de reception de l'uart2
		 *	et complète le message CAN passé en argument à partir du point où
		 *	elle s'est arretée à son précédent appel. Elle renvoie ensuite si
		 *	oui ou non elle a fini de complèter le message CAN. Elle vérifie 
		 *  aussi si le message est bien conforme au protocole de communication
		 *  (cf QS)
		 */
	#endif /* def USE_UART1RXINTERRUPT */
	void CANmsgToU2tx (CAN_msg_t* src);
	/*
	 *	Ecrit le contenu du message CAN passé en argument sur
	 *	l'uart2 en ajoutant l'octet SOH en début de message et l'octet EOT
	 *	en fin de message (cf : protocole de communication QS)
	 */
#endif /* def USE_UART2 */
#endif /* ifndef QS_CAN_OVER_UART_H */
