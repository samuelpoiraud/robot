/*
 *	Club Robot ESEO 2011
 *	Check Norris
 *
 *	Fichier : Verbose_can_msg.h
 *	Package : Supervision
 *	Description : Module d'affichage dans un texte "humain" du contenu d'un message can.
 *	Auteur : Nirgal
 *	Version 20110514
 */

#ifndef VERBOSE_CAN_MSG_H
	#define VERBOSE_CAN_MSG_H
	
	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"
	
	/*
		Affiche le message can sous la forme d'un texte intelligible.
	*/
	void VERBOSE_CAN_MSG_print(CAN_msg_t * can_msg);
	void print_ir_result(CAN_msg_t * msg);
	void print_us_result(CAN_msg_t * msg);


#endif /* ndef VERBOSE_CAN_MSG_H */
