 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : secretary.h
 *  Package : Propulsion
 *  Description : Assure la communication entre le monde ext�rieur et le syst�me.
 *					Ins�re des ordres sur la roadmap... Rend compte des actions effectu�s et des erreurs.
 *  Auteur : Asser 2006 - modif by Val' / refactoring 2009 par Nirgal (conformit� QS par Jacen)
 *  Version 201203
 */

#ifndef _SECRETARY_H_
	#define _SECRETARY_H_
	#include "QS/QS_all.h"
	#include "QS/QS_can.h"

	typedef enum{
		FROM_CAN = 0,
		FROM_UART,
		FROM_XBEE,
		FROM_LCD,
		TO_CAN,
		TO_UART,
		TO_XBEE
	}MAIL_from_to_e;

		void SECRETARY_init(void);

		void SECRETARY_process_main(void);
	/*
	Fonction qui traite les messages re�us.
	Selon le cas, elle peut ajouter des ordres dans la roadmap, configurer le syst�me...
	*/
	void SECRETARY_process_CANmsg(CAN_msg_t* msg, MAIL_from_to_e from);

	void SECRETARY_send_adversary_position(bool_e it_is_the_last_adversary, Uint8 adversary_number, Uint16 x, Uint16 y, Sint16 teta, Uint16 distance, Uint8 fiability);

	void SECRETARY_send_foe_detected(Uint16 x, Uint16 y, Uint16 dist, Sint16 angle, bool_e adv_hokuyo, bool_e timeout);

	void SECRETARY_send_trajectory_for_test_coefs_finished(Uint16 duration);

	void SECRETARY_send_cup_position(bool_e it_is_the_last_cup, bool_e error_scan, bool_e cup_detected, Sint16 x, Sint16 y);

	void SECRETARY_send_bloc_position(bool_e error_scan, Sint16 x, Sint16 y);

	/*
	Fonction permettant d'envoyer un message au monde ext�rieur.
	Selon le message indiqu� en param�tre, un certain nombre
	d'arguments seront ajout�s et envoy�s, conform�ment � la QS.
	*/
	void SECRETARY_process_send(Uint11 sid, prop_warning_reason_e reason, SUPERVISOR_error_source_e error_source);

	void SECRETARY_process_it(void);

	void SECRETARY_mailbox_add(CAN_msg_t * msg, MAIL_from_to_e from);

	void SECRETARY_send_selftest_result(bool_e result);

	bool_e SECRETARY_is_selftest_validated(void);

	void SECRETARY_send_canmsg(CAN_msg_t * msg);			//NE DOIT PAS ETRE APPELEE EN IT !
	void SECRETARY_send_canmsg_from_it(CAN_msg_t * msg);	//NE DOIT PAS ETRE APPELEE EN TACHE DE FOND !

#endif	//def _SECRETARY_H
