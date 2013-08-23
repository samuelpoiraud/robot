/*
 *	Club Robot ESEO 2011
 *	Check Norris
 *
 *	Fichier : eeprom_can_msg.h
 *	Package : Supervision
 *	Description : Module de gestion de l'enregistrement des messages can dans l'eeprom SPI
 *	Auteur : Nirgal
 *	Version 20110514
 */

#ifndef EEPROM_CAN_MSG_H
	#define EEPROM_CAN_MSG_H
	
	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"
	#include "../QS/QS_spi.h"
	#include "Eeprom.h"
	#include "Verbose_can_msg.h"
	#include "RTC.h"
	#include "Selftest.h"
	
	/*
	@brief	fonction d'initialisation du module de sauvegarde en EEPROM. Très importante, cette fonction scrute l'état actuel de la mémoire pour en reprendre le cours !
	@post 	initialized est passé à TRUE
	@note	Vérification de présence mémoire : lecture adresse 0, si != 0xCAFEDECA, écriture de 0xCAFEDECA, relecture et vérif...
	*/
	
	void EEPROM_CAN_MSG_init();
	
	/*
	@brief 	fonction incluant la machine à état du module de sauvegarde en EEPROM. 
	@pre	Cette fonction est appelée à chaque nouveau message reçu
	@pre	initialized doit être à TRUE ! sinon rien n'est fait. (lorsque la mémoire n'est pas reliée notamment !)
	*/
	void EEPROM_CAN_MSG_process_msg(CAN_msg_t * msg);

	/*
	@brief	lance l'affichage de tout les messages du prochain match à afficher.
	@note	au reset, le "prochain match à afficher" est le dernier enregistré. Ensuite, à chaque appel, on passe au match suivant (du plus jeune au plus vieux)
	@post	si on a affiché tout les matchs enregistrés, un message est affiché pour nous prévenir.
	*/
	void EEPROM_CAN_MSG_verbose_previous_match(void);

	/*
	@brief	lance l'affichage de tout les messages d'un match dont on connait l'adresse de l'entête.
	@pre	match_address_x8 doit être dans le range des match_address, sinon un message d'information indique l'erreur.	
	*/
	void EEPROM_CAN_MSG_verbose_match(Uint16 match_address_x8);
	
	void print_all_msg();
	
#endif /* ndef EEPROM_CAN_MSG_H */
