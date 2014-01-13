/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : act_queue_utils.h
 *	Package : Carte Actionneur
 *	Description : Propose des fonctions pour g�rer les actions avec la pile.
 *	Auteur : Alexis
 *	Version 20130420
 */

#ifndef ACT_QUEUE_UTILS_H
#define	ACT_QUEUE_UTILS_H
	#include "queue.h"

	//Met sur la pile une action qui sera g�r�e par act_function_ptr avec en param�tre param. L'action est prot�g�e par semaphore avec act_id
	//Cette fonction est appel�e par les fonctions de traitement des messages CAN de chaque actionneur.
	void ACTQ_push_operation_from_msg(CAN_msg_t* msg, QUEUE_act_e act_id, action_t act_function_ptr, Uint16 param);

	//Envoie le message CAN de retour � la strat (et affiche des infos de debuggage si activ�)
	void ACTQ_sendResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode);

	//Comme CAN_sendResult mais ajoute un param�tre au message. Peut servir pour debuggage.
	void ACTQ_sendResultWithParam(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 param);

	//Comme CAN_sendResultWithParam mais le param�tre est consid�r� comme �tant un num�ro de ligne.
	void ACTQ_sendResultWitExplicitLine(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 lineNumber);

	//Macro pour avoir la ligne a laquelle cette macro est utilis� comme param�tre � CAN_sendResultWithParam
	#define ACTQ_sendResultWithLine(originalSid, originalCommand, result, errorCode) ACTQ_sendResultWitExplicitLine(originalSid, originalCommand, result, errorCode, __LINE__)


	//Gestion des �tats des actionneurs (ax12 / DCMotor)
	//Retourne TRUE si l'action en cours n'est pas termin�e, sinon FALSE

	/** Verifie l'�tat de l'ax12 et g�re la file en cons�quence.
	 *
	 * @param queueId l'identifiant de la file
	 * @param ax12Id l'identifiant de l'ax12 � g�rer
	 * @param wantedPosition position voulue en degr�
	 * @param pos_epsilon pr�cision de la position voulue, tant qu'on � pas atteint wantedPosition_degre +/- pos_epsilon on consid�re qu'on est pas rendu � la bonne position
	 * @param timeout_ms_x100 timeout en centaine de msec (10 = 1 sec)
	 * @param large_epsilon comme pos_epsilon mais utilis� apr�s un timeout. Apr�s un timeout, on v�rifie si on est proche de la position voulue (utile pour des pinces par ex), si oui il n'y a pas d'erreur
	 *
	 * @param result pointeur vers une variable qui contiendra le r�sultat de l'op�ration si la fonction retourne TRUE (ACT_RESULT*)
	 * @param error_code pointeur vers une variable qui contiendra le l'erreur li� a l'op�ration si la fonction retourne TRUE (ACT_RESULT_ERROR*)
	 * @param line pointeur vers une variable qui contiendra la ligne qui affecte l'erreur dans les variables pointeurs
	 * @return TRUE si l'ax12 a fini sa commande, FALSE sinon
	 */
	bool_e ACTQ_check_status_ax12(queue_id_t queueId, Uint8 ax12Id, Uint16 wantedPosition, Uint16 pos_epsilon, Uint16 timeout_ms_x100, Uint16 large_epsilon, Uint8* result, Uint8* error_code, Uint16* line);

	/** Verifie l'�tat d'un moteur DC et g�re la file en cons�quence.
	 *
	 * @param dcmotor_id identifiant du moteur (pass� en parametre des fonctions de DCMotor2.h)
	 * @param timeout_is_ok TRUE si lors d'un timeout, le resultat doit �tre Ok, FALSE si on doit indiquer une erreur. (Mettre � TRUE pour des moteur qui devrons forcer en continu sur qquechose et qui n'atteindront jamais la position voulue)
	 *
	 * @param result pointeur vers une variable qui contiendra le r�sultat de l'op�ration si la fonction retourne TRUE (ACT_RESULT*)
	 * @param error_code pointeur vers une variable qui contiendra le l'erreur li� a l'op�ration si la fonction retourne TRUE (ACT_RESULT_ERROR*)
	 * @param line pointeur vers une variable qui contiendra la ligne qui affecte l'erreur dans les variables pointeurs
	 * @return TRUE si le moteur a fini sa commande, FALSE sinon
	 */
	bool_e ACTQ_check_status_dcmotor(Uint8 dcmotor_id, bool_e timeout_is_ok, Uint8* result, Uint8* error_code, Uint16* line);

	//Callback
	//Renvoie un retour � la strat dans tous les cas
	bool_e ACTQ_finish_SendResult(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

	//Retour � la strat seulement si l'op�ration � fail
	bool_e ACTQ_finish_SendResultIfFail(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

	//Retour � la strat seuelement si l'op�ration � reussi
	bool_e ACTQ_finish_SendResultIfSuccess(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

	//Ne fait aucun retour
	bool_e ACTQ_finish_SendNothing(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);


#endif	/* ACT_QUEUE_UTILS_H */
