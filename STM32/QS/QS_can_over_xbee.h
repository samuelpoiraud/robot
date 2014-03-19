/*
 *	Club Robot ESEO 2011-2012
 *	Shark & Fish
 *
 *	Fichier : QS_can_over_xbee.c
 *	Package : Qualité Soft
 *	Description : fonctions d'encapsulation des messages CAN
					pour envoi via module XBEE configuré en mode API
 *	Auteur : Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20120224
 */

#include "QS_all.h"

#ifndef QS_CAN_OVER_XBEE_H
#define	QS_CAN_OVER_XBEE_H

#ifdef USE_XBEE

#if !(defined XBEE_PLUGGED_ON_UART1 || defined XBEE_PLUGGED_ON_UART2)
	#error "Vous devez définir l'UART où est branché le XBEE"
	#error "Pour cela, définissez XBEE_PLUGGED_ON_UART1 ou XBEE_PLUGGED_ON_UART2"
#endif

#define SOH 0x01
#define EOT 0x04
#define STX 0x02
#define ETX 0x03

	//Listez ici l'ensemble des modules utilisés
	typedef enum
	{
		MODULE_C = 0,	//Coordinateur.
		MODULE_1,
		MODULE_2,
		MODULE_3,
		MODULE_NUMBER	//nombre de module... = forcément le nombre suivant dans l'énumération !
	}module_id_e;
	
	#ifdef QS_CAN_OVER_XBEE_C
		//ADRESSES PHYSIQUE DES MODULES XBEE UTILISES !!! (= numéro de série = on ne peut pas le changer)
		//Attention, l'ordre doit correspondre à l'énumération module_id_e !
		//L'ajout d'une gomette numérotée sur chaque module est vivement conseillé !!!
		const Uint8 module_address[MODULE_NUMBER][8] =
			{{0x00, 0x13, 0xA2, 0x00, 0x40, 0x61, 0x49, 0x66},	//COORDINATEUR
			 {0x00, 0x13, 0xA2, 0x00, 0x40, 0x5D, 0xFB, 0x8D},	//MODULE 1
			 {0x00, 0x13, 0xA2, 0x00, 0x40, 0x5D, 0xF9, 0xFB},	//MODULE 2
			 {0x00, 0x13, 0xA2, 0x00, 0x40, 0x9E, 0xAF, 0xA1}	//MODULE 3
			};
	#endif		


	//Possibilité pour faire correspondre l'applicatif aux modules utilisés :
	#define BIG_ROBOT_MODULE	MODULE_1
	#define SMALL_ROBOT_MODULE	MODULE_C
	#define BALISE_MERE			MODULE_2
	
	void CAN_over_XBee_init(module_id_e me, module_id_e destination);
	
	/*
	@function	permettant de recevoir des messages CAN via une liaison ZigBee, et en provenance de notre module XBee
	@param		dest est un pointeur vers le message CAN à recevoir
	@return 	un booléen qui indique si un message a été reçu. (si TRUE, on peut lire *dest sinon, *dest ne vaut rien d'intéressant !!!)
	@pre		AU PLUS SOUVENT : on peut appeler cette fonction en boucle infinie
	@pre		AU MOINS SOUVENT : il faut appeler cette fonction à chaque caractère reçu sur l'UARTxbee
	@post		Si une frame correcte est reçue, cette fonction tente d'y trouver le message CAN.
	*/
	bool_e XBeeToCANmsg (CAN_msg_t* dest);
	
	
	/*
	@function 	permettant d'envoyer un message CAN via une liaison ZigBee au module XBee de destination par défaut, ou a l'ensemble des modules joignables.
	@note		les messages CAN transmis respectent le protocole définit pour le transfert de message CAN over UART (SOH, EOT, et nombre de data constantes !)
				intérêt : c'est plus simple !
	@param		src est un pointeur vers le message CAN à envoyer
	@param		broadcast : si TRUE, le msg est envoyé à tout les modules joignables, si FALSE, seulement au module par défaut (configuré dans l'init)
	@post		cette fonction ne peut pas garantir que le message à été envoyé. Mais elle a tenté de transmettre la demande d'envoi au module XBee !
	@pre		le module XBee doit être allumé, et en état de fonctionnement réseau. (une attente de quelques secondes après reset est conseillée avant d'envoyer des messages)
	*/
	void CANMsgToXbee(CAN_msg_t * src, bool_e broadcast);

	void XBEE_send_sid(Uint11 sid, bool_e broadcast);


	/*
	@function 	permettant d'envoyer un message CAN via une liaison ZigBee à un autre module XBee DONT LA DESTINATION EST CONNUE.
	@note		les messages CAN transmis respectent le protocole définit pour le transfert de message CAN over UART (SOH, EOT, et nombre de data constantes !)
				intérêt : c'est plus simple !
	@param		src est un pointeur vers le message CAN à envoyer
	@param		module_dest est l'un des modules de destination connu. Voir QS_can_over_xbee.h pour une liste des modules connus !
	@post		cette fonction ne peut pas garantir que le message à été envoyé. Mais elle a tenté de transmettre la demande d'envoi au module XBee !
	@pre		le module XBee doit être allumé, et en état de fonctionnement réseau. (une attente de quelques secondes après reset est conseillée avant d'envoyer des messages)
	*/
	void CANMsgToXBeeDestination(CAN_msg_t * src, module_id_e destination);

	void CAN_over_XBee_process_main(void);
	void CAN_over_XBee_every_second(void);
	bool_e XBee_is_destination_reachable(void);
	bool_e XBee_is_module_reachable(module_id_e module);
	void XBEE_ping_pong_enable(bool_e enable);

	
#endif // def USE_XBEE

#endif /* ifndef QS_CAN_OVER_XBEE_H */
