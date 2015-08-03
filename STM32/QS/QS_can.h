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

/** ----------------  Defines possibles  --------------------
 *
 *	USE_CAN						: Activation du bus CAN 1 et des fonctions en rapport
 *	USE_CAN2					: Non impl�ment�
 *	CAN_BUF_SIZE				: La nombre de message CAN maximum dans le buffer
 *	CAN_SEND_TIMEOUT_ENABLE		: Activation de la d�tection d'un timeout sur l'�mussion de message CAN
 *	TIMEOUT_CAN_SEND			: Temps avant le timeout d'une �misssion de message CAN						Par d�faut � 4196
 *	QS_CAN_RX_IT_PRI			: Modification de la priorit� de l'IT de r�ception du bus CAN				Par d�faut � 1
 *
 *
 *  ----------------  Choses � savoir  --------------------
 *	MODE_SIMULATION				: A pour effet de supprimer l'envoi de message CAN
 */

#ifndef QS_CAN_H
	#define QS_CAN_H

	#include "QS_all.h"
	#include "QS_CANmsgList.h"

	/* Type de pointeur sur fonction qui v�rifie si le message CAN doit �tre trait� directement */
	typedef bool_e (*direct_treatment_function_pt)(CAN_msg_t* msg);
	typedef void(*CAN_callback_action_t)(CAN_msg_t * can_msg);

	/* Prototype */
	void CAN_init(void);

	void CAN_send(CAN_msg_t* can_msg);
	void CAN_send_sid(Uint11 sid);

	CAN_msg_t CAN_get_next_msg();
	bool_e CAN_data_ready();

	/* Permet le traitement direct des messages CAN */
	void CAN_set_direct_treatment_function(direct_treatment_function_pt fct);
	void CAN_set_send_callback(CAN_callback_action_t action);
#endif
