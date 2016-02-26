/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : QualitÃ© Soft
 *  Description : Module RF: synchro + envoi messages CAN (attention c'est trÃ¨s lent ! 4,8kbits/sec, 13 octets/paquet + nos donnÃ©es)
 *  Auteur : amurzeau
 */

/** ----------------  Defines possibles  --------------------
 *	USE_RF						: Activation de QS_rf
 *	RF_TIMER_ID					: Numéro du timer choisi pour ce module
 *	RF_USE_WATCHDOG				: Utiliser watchdog plutôt qu'un timer pour le fonctionnement de ce module
 *	RF_UART						: Définir l'UART sur lequel est branché le module RF
 */

/** --------------- Configuration des modules RF (Radiocrafts RC1240) -------------------------
 *	Pour utiliser les modules de Radiocrafts RC1240, il faut réaliser une configuration de la mémoire de ces modules.
 *  Cette configuration est indispensable pour pouvoir synchroniser ces modules dans le cadre de leur utilisation au club
 *  robot.
 *  L'opération à effectuer consiste à changer les valeurs de trois paramètres à l'intérieur de la mémoire du module RC1240.
 *  Les valeurs à modifier sont:
 *	    END_OF_PACKET:  adresse = 0x11        nouvelle valeur = 0xC1
 *      ADDRESS_MODE:    adresse = 0x14        nouvelle valeur = 0x00
 *      CRC_MODE:       adresse = 0x15        nouvelle valeur = 0x00
 *  Mode opératoire:
 *  Pour rendre opérationnel ces modules, vous devez décommenter le define CONFIG_RF_RC1240 dans le fichier config_use.h.
 *  Branchez vous sur l'UART de la carte balise correspondante et ouvrez un Docklight. Programmer la carte balise et lancez
 *  l'éxécution du programme.
 *  1) Entrez la lettre 'M', cela permet de rentrer dans le mode de configuration mémoire
 *  2) Attendre le prompt '>' renvoyez par le module
 *  3) Entrez le caractère 0x11 puis 0xC1 pour la modification de END_OF_PACKET
 *  4) Entrez le caractère 0x14 puis 0x00 pour la modification de ADDRESS_MODE
 *  5) Entrez le caractère 0x15 puis 0x00 pour la modification de CRC_MODE
 *  6) Entrez le caractère 'X' pour terminer la configuaration mémoire et rendre effectif les modifications.
 *  7) Attendre le prompt '>' renvoyez par le module
 *  8) Vous pouvez entrer le caractère '0' (zéro) pour avoir un dump de la mémoire (liste de toutes valeurs de la mémoire)
 *     et ainsi vérifier que les modifications ont été effectuées.
 *  9) Terminer la configuration en entrant le caractère 0xFF
 *
 *  Pour entrer des valeurs héxadécimales sur Docklight, allez dans Tools -> Options -> Control Characters Shortcuts.
 *  Vous pouvez ensuite créer des raccourcis pour les valeurs que vous souhaitez entrer.
 *
 */


#ifndef QS_RF_H
	#define QS_RF_H

	#include "QS_all.h"
	#include "QS_CANmsgList.h"

	#ifdef USE_RF

		// !!!!!!!!!!!! MAX 7 modules !!!!!!!!!!!! (0-6, 7 = broadcast)
		typedef enum {
			RF_SMALL,
			RF_BIG,
			RF_FOE1,
			RF_FOE2,
			RF_BROADCAST = 7
		} RF_module_e;
		#define RF_MODULE_COUNT 4

		typedef enum {
			RF_PT_SynchroRequest,
			RF_PT_SynchroResponse,
			RF_PT_Can,
			RF_PT_None = 3
		} RF_packet_type_e;

		typedef union {
			Uint8 raw_data;
			struct {
				RF_packet_type_e type : 2;
				RF_module_e sender_id : 3;
				RF_module_e target_id : 3;
			};
		} RF_header_t;

		typedef void (*RF_onReceive_ptr)(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
		typedef void (*RF_onCanMsg_ptr)(CAN_msg_t* msg);

		void RF_init(RF_module_e module, RF_onReceive_ptr onReceiveCallback, RF_onCanMsg_ptr onCanMsgCallback);
		RF_module_e RF_get_module_id();
		void RF_putc(Uint8 c);
		void RF_can_send(RF_module_e target_id, CAN_msg_t *msg);
		void RF_synchro_request(RF_module_e target_id);
		void RF_synchro_response(RF_module_e target_id, Sint16 timer_offset);

		//void RF_set_channel(Uint8 channel);
		//void RF_set_output_power(Uint8 power);
		//void RF_set_destination_id(Uint8 address);

		//Uint8 RF_get_channel();
		//Uint8 RF_get_output_power();
		//Uint8 RF_get_rssi();
		//Uint8 RF_get_destination_id();

	#endif

#endif // QS_RF_H
