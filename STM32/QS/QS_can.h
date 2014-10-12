/*
 *  Club Robot ESEO 2006 - 2010
 *  Game Hoover, Archi-Tech', PACMAN
 *
 *  Fichier : QS_can.h
 *  Package : Qualit� Soft
 *  Description : fonction CAN
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 27032011
 */


#ifndef QS_CAN_H
	#define QS_CAN_H

	#include "QS_all.h"

	/* Type de pointeur sur fonction qui v�rifie si le message CAN doit �tre trait� directement */
	typedef bool_e (*direct_treatment_function_pt)(CAN_msg_t* msg);

	/* Prototype */
	void CAN_init(void);

	typedef void(*CAN_callback_action_t)(CAN_msg_t * can_msg);

	void CAN_send(CAN_msg_t* can_msg);
	void CAN_direct_send(Uint11 sid, Uint8 size, Uint8 arg[]);
	void CAN_send_sid(Uint11 sid);
	CAN_msg_t CAN_get_next_msg();
	bool_e CAN_data_ready();
	/* Permet le traitement direct des messages CAN */
	void CAN_set_direct_treatment_function(direct_treatment_function_pt fct);
	void CAN_set_send_callback(CAN_callback_action_t action);

	#ifdef USE_CAN2
		void CAN2_send(CAN_msg_t* can_msg);
		CAN_msg_t CAN2_get_next_msg();
		bool_e CAN2_data_ready();
		/* Permet le traitement direct des messages CAN */
		void CAN2_set_direct_treatment_function(direct_treatment_function_pt fct);
	#endif /* def USE_CAN2 */
#endif
