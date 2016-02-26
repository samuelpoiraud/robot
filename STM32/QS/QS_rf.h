/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Qualité Soft
 *  Description : Module RF: synchro + envoi messages CAN (attention c'est très lent ! 4,8kbits/sec, 13 octets/paquet + nos données)
 *  Auteur : amurzeau
 */

/** ----------------  Defines possibles  --------------------
 *	USE_RF						: Activation de QS_rf
 *	RF_TIMER_ID					: Num�ro du timer choisi pour ce module
 *	RF_USE_WATCHDOG				: Utiliser watchdog plut�t qu'un timer pour le fonctionnement de ce module
 *	RF_UART						: D�finir l'UART sur lequel est branch� le module RF
 */

/** --------------- Configuration des modules RF (Radiocrafts RC1240) -------------------------
 *	Pour utiliser les modules de Radiocrafts RC1240, il faut r�aliser une configuration de la m�moire de ces modules.
 *  Cette configuration est indispensable pour pouvoir synchroniser ces modules dans le cadre de leur utilisation au club
 *  robot.
 *  L'op�ration � effectuer consiste � changer les valeurs de trois param�tres � l'int�rieur de la m�moire du module RC1240.
 *  Les valeurs � modifier sont:
 *	    END_OF_PACKET:  adresse = 0x11        nouvelle valeur = 0xC1
 *      ADDRESS_MODE:    adresse = 0x14        nouvelle valeur = 0x00
 *      CRC_MODE:       adresse = 0x15        nouvelle valeur = 0x00
 *  Mode op�ratoire:
 *  Pour rendre op�rationnel ces modules, vous devez d�commenter le define CONFIG_RF_RC1240 dans le fichier config_use.h.
 *  Branchez vous sur l'UART de la carte balise correspondante et ouvrez un Docklight. Programmer la carte balise et lancez
 *  l'�x�cution du programme.
 *  1) Entrez la lettre 'M', cela permet de rentrer dans le mode de configuration m�moire
 *  2) Attendre le prompt '>' renvoyez par le module
 *  3) Entrez le caract�re 0x11 puis 0xC1 pour la modification de END_OF_PACKET
 *  4) Entrez le caract�re 0x14 puis 0x00 pour la modification de ADDRESS_MODE
 *  5) Entrez le caract�re 0x15 puis 0x00 pour la modification de CRC_MODE
 *  6) Entrez le caract�re 'X' pour terminer la configuaration m�moire et rendre effectif les modifications.
 *  7) Attendre le prompt '>' renvoyez par le module
 *  8) Vous pouvez entrer le caract�re '0' (z�ro) pour avoir un dump de la m�moire (liste de toutes valeurs de la m�moire)
 *     et ainsi v�rifier que les modifications ont �t� effectu�es.
 *  9) Terminer la configuration en entrant le caract�re 0xFF
 *
 *  Pour entrer des valeurs h�xad�cimales sur Docklight, allez dans Tools -> Options -> Control Characters Shortcuts.
 *  Vous pouvez ensuite cr�er des raccourcis pour les valeurs que vous souhaitez entrer.
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
