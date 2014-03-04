 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : secretary.h
 *  Package : Propulsion
 *  Description : Assure la communication entre le monde extérieur et le système.
 *					Insère des ordres sur la roadmap... Rend compte des actions effectués et des erreurs.
 *  Auteur : Asser 2006 - modif by Val' / refactoring 2009 par Nirgal (conformité QS par Jacen)
 *  Version 201203
 */

#ifndef _SECRETARY_H_
	#define _SECRETARY_H_
	#include "QS/QS_all.h"
	#include "LCDTouch/LCD.h"

		void SECRETARY_init(void);

		void SECRETARY_process_main(void);
	/*
	Fonction qui traite les messages reçus.
	Selon le cas, elle peut ajouter des ordres dans la roadmap, configurer le système...
	*/
	void SECRETARY_process_CANmsg(CAN_msg_t* msg);

	void SECRETARY_send_adversary_position(bool_e it_is_the_last_adversary, Uint8 adversary_number, Uint16 x, Uint16 y, Sint16 teta, Uint16 distance, Uint8 fiability);

	void SECRETARY_send_trajectory_for_test_coefs_finished(Uint16 duration);

	#ifdef LCD_TOUCH
		void SECRETARY_send_friend_position(Sint16 x, Sint16 y);
	#endif

	/*
	Fonction permettant d'envoyer un message au monde extérieur.
	Selon le message indiqué en paramètre, un certain nombre
	d'arguments seront ajoutés et envoyés, conformément à la QS.
	*/
	void SECRETARY_process_send(Uint11 sid, Uint8 reason, SUPERVISOR_error_source_e error_source);

	#ifdef SCAN_TRIANGLE
		void SECRETARY_send_triangle_position(bool_e it_is_the_last_triangle, Uint8 triangle_level, Uint8 triangle_number, Sint16 x, Sint16 y, Sint16 teta);
		void SECRETARY_send_triangle_warner(bool_e present, Uint8 triangle_number);
	#endif

	void SECRETARY_process_it(void);

	void SECRETARY_mailbox_add(CAN_msg_t * msg);

	void SECRETARY_send_selftest_result(bool_e result);

	bool_e SECRETARY_is_selftest_validated(void);

#endif	//def _SECRETARY_H
