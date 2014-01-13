/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : act_queue_utils.h
 *	Package : Carte Actionneur
 *	Description : Propose des fonctions pour gérer les actions avec la pile.
 *	Auteur : Alexis
 *	Version 20130420
 */

#ifndef ACT_QUEUE_UTILS_H
#define	ACT_QUEUE_UTILS_H
	#include "queue.h"

	//Met sur la pile une action qui sera gérée par act_function_ptr avec en paramètre param. L'action est protégée par semaphore avec act_id
	//Cette fonction est appelée par les fonctions de traitement des messages CAN de chaque actionneur.
	void ACTQ_push_operation_from_msg(CAN_msg_t* msg, QUEUE_act_e act_id, action_t act_function_ptr, Uint16 param);

	//Envoie le message CAN de retour à la strat (et affiche des infos de debuggage si activé)
	void ACTQ_sendResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode);

	//Comme CAN_sendResult mais ajoute un paramètre au message. Peut servir pour debuggage.
	void ACTQ_sendResultWithParam(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 param);

	//Comme CAN_sendResultWithParam mais le paramètre est considéré comme étant un numéro de ligne.
	void ACTQ_sendResultWitExplicitLine(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 lineNumber);

	//Macro pour avoir la ligne a laquelle cette macro est utilisé comme paramètre à CAN_sendResultWithParam
	#define ACTQ_sendResultWithLine(originalSid, originalCommand, result, errorCode) ACTQ_sendResultWitExplicitLine(originalSid, originalCommand, result, errorCode, __LINE__)


	//Gestion des états des actionneurs (ax12 / DCMotor)
	//Retourne TRUE si l'action en cours n'est pas terminée, sinon FALSE

	/** Verifie l'état de l'ax12 et gère la file en conséquence.
	 *
	 * @param queueId l'identifiant de la file
	 * @param ax12Id l'identifiant de l'ax12 à gérer
	 * @param wantedPosition position voulue en degré
	 * @param pos_epsilon précision de la position voulue, tant qu'on à pas atteint wantedPosition_degre +/- pos_epsilon on considère qu'on est pas rendu à la bonne position
	 * @param timeout_ms_x100 timeout en centaine de msec (10 = 1 sec)
	 * @param large_epsilon comme pos_epsilon mais utilisé après un timeout. Après un timeout, on vérifie si on est proche de la position voulue (utile pour des pinces par ex), si oui il n'y a pas d'erreur
	 *
	 * @param result pointeur vers une variable qui contiendra le résultat de l'opération si la fonction retourne TRUE (ACT_RESULT*)
	 * @param error_code pointeur vers une variable qui contiendra le l'erreur lié a l'opération si la fonction retourne TRUE (ACT_RESULT_ERROR*)
	 * @param line pointeur vers une variable qui contiendra la ligne qui affecte l'erreur dans les variables pointeurs
	 * @return TRUE si l'ax12 a fini sa commande, FALSE sinon
	 */
	bool_e ACTQ_check_status_ax12(queue_id_t queueId, Uint8 ax12Id, Uint16 wantedPosition, Uint16 pos_epsilon, Uint16 timeout_ms_x100, Uint16 large_epsilon, Uint8* result, Uint8* error_code, Uint16* line);

	/** Verifie l'état d'un moteur DC et gère la file en conséquence.
	 *
	 * @param dcmotor_id identifiant du moteur (passé en parametre des fonctions de DCMotor2.h)
	 * @param timeout_is_ok TRUE si lors d'un timeout, le resultat doit être Ok, FALSE si on doit indiquer une erreur. (Mettre à TRUE pour des moteur qui devrons forcer en continu sur qquechose et qui n'atteindront jamais la position voulue)
	 *
	 * @param result pointeur vers une variable qui contiendra le résultat de l'opération si la fonction retourne TRUE (ACT_RESULT*)
	 * @param error_code pointeur vers une variable qui contiendra le l'erreur lié a l'opération si la fonction retourne TRUE (ACT_RESULT_ERROR*)
	 * @param line pointeur vers une variable qui contiendra la ligne qui affecte l'erreur dans les variables pointeurs
	 * @return TRUE si le moteur a fini sa commande, FALSE sinon
	 */
	bool_e ACTQ_check_status_dcmotor(Uint8 dcmotor_id, bool_e timeout_is_ok, Uint8* result, Uint8* error_code, Uint16* line);

	//Callback
	//Renvoie un retour à la strat dans tous les cas
	bool_e ACTQ_finish_SendResult(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

	//Retour à la strat seulement si l'opération à fail
	bool_e ACTQ_finish_SendResultIfFail(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

	//Retour à la strat seuelement si l'opération à reussi
	bool_e ACTQ_finish_SendResultIfSuccess(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

	//Ne fait aucun retour
	bool_e ACTQ_finish_SendNothing(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);


#endif	/* ACT_QUEUE_UTILS_H */
