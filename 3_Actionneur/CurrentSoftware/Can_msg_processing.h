/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : Can_msg_processing.h
 *	Package : Carte actionneur
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Aur�lien
 *  Version 20110225
 */

#ifndef CAN_MSG_PROCESSING_H
	#define CAN_MSG_PROCESSING_H
	#include "QS/QS_all.h"
	#include "queue.h"

	//Met sur la pile une action qui sera g�r�e par act_function_ptr avec en param�tre param. L'action est prot�g�e par semaphore avec act_id
	//Cette fonction est appel�e par les fonctions de traitement des messages CAN de chaque actionneur.
	void CAN_push_operation_from_msg(CAN_msg_t* msg, QUEUE_act_e act_id, action_t act_function_ptr, Uint16 param);

	//Traite les messages CAN re�us
	void CAN_process_msg(CAN_msg_t* msg_to_process);

	//Envoie le message CAN de retour � la strat (et affiche des infos de debuggage si activ�)
	void CAN_sendResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode);

	//Comme CAN_sendResult mais ajoute un param�tre au message. Peut servir pour debuggage.
	void CAN_sendResultWithParam(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 param);

	//Comme CAN_sendResultWithParam mais le param�tre est consid�r� comme �tant un num�ro de ligne.
	void CAN_sendResultWitExplicitLine(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 lineNumber);

	//Macro pour avoir la ligne a laquelle cette macro est utilis� comme param�tre � CAN_sendResultWithParam
	#define CAN_sendResultWithLine(originalSid, originalCommand, result, errorCode) CAN_sendResultWitExplicitLine(originalSid, originalCommand, result, errorCode, __LINE__)
	
#endif /* ndef CAN_MSG_PROCESSING_H */
