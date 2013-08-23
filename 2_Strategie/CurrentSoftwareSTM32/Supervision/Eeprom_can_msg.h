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
	@brief	fonction d'initialisation du module de sauvegarde en EEPROM. Tr�s importante, cette fonction scrute l'�tat actuel de la m�moire pour en reprendre le cours !
	@post 	initialized est pass� � TRUE
	@note	V�rification de pr�sence m�moire : lecture adresse 0, si != 0xCAFEDECA, �criture de 0xCAFEDECA, relecture et v�rif...
	*/
	
	void EEPROM_CAN_MSG_init();
	
	/*
	@brief 	fonction incluant la machine � �tat du module de sauvegarde en EEPROM. 
	@pre	Cette fonction est appel�e � chaque nouveau message re�u
	@pre	initialized doit �tre � TRUE ! sinon rien n'est fait. (lorsque la m�moire n'est pas reli�e notamment !)
	*/
	void EEPROM_CAN_MSG_process_msg(CAN_msg_t * msg);

	/*
	@brief	lance l'affichage de tout les messages du prochain match � afficher.
	@note	au reset, le "prochain match � afficher" est le dernier enregistr�. Ensuite, � chaque appel, on passe au match suivant (du plus jeune au plus vieux)
	@post	si on a affich� tout les matchs enregistr�s, un message est affich� pour nous pr�venir.
	*/
	void EEPROM_CAN_MSG_verbose_previous_match(void);

	/*
	@brief	lance l'affichage de tout les messages d'un match dont on connait l'adresse de l'ent�te.
	@pre	match_address_x8 doit �tre dans le range des match_address, sinon un message d'information indique l'erreur.	
	*/
	void EEPROM_CAN_MSG_verbose_match(Uint16 match_address_x8);
	
	void print_all_msg();
	
#endif /* ndef EEPROM_CAN_MSG_H */
