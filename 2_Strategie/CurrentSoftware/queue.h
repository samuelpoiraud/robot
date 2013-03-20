/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : queue.h
 *	Package : Carte Principale (Stratégie)
 *	Description : files d'actions actionneurs
 *  Auteur : Alexis (basé sur queue.c/h de la carte actionneur (18/03/2013, Jacen(modifié par Aurélien)))
 *  Version 20130318
 */

#include "QS/QS_all.h"

#ifndef QUEUE_H
#define QUEUE_H

#include "clock.h"


//Ce fichier est lié à act_function.c/h (a cause de l'argument QUEUE_arg_t surtout)

typedef Uint8 queue_size_t;
typedef enum {
	ACT_QUEUE_BallLauncher,
	ACT_QUEUE_Plate,
	ACT_QUEUE_Hammer,
	ACT_QUEUE_BallInflater,
	ACT_QUEUE_LiftLeft,
	ACT_QUEUE_LiftRight,
	NB_QUEUE
} queue_id_e;

#define ACT_ARG_NOFALLBACK_SID 0xFFF
#define ACT_ARG_NOTIMEOUT ((time32_t)-1)
#define ACT_ARG_USE_DEFAULT 0

//Version allégé pour nos besoin d'un message CAN, histoire de pas prendre trop de RAM dans la variable act_args ...
typedef struct {
	Uint11 sid;
	Uint8 data[3];
	Uint8 size;
} ACT_can_msg_t;

//Pour timeout: utiliser ACT_ARG_NOTIMEOUT pour ne pas mettre de timeout
typedef struct {
	Uint16 timeout;         //Temps avant timeout de l'action en ms (relatif au démarrage de l'execution de l'action).
	ACT_can_msg_t msg;          //Message à envoyer pour executer l'action
	ACT_can_msg_t fallbackMsg;  //Quand on ne peut pas aller a la position demandé, envoyer ce message. Utilisé pour ACT_BEHAVIOR_GoalUnreachable. Si le SID est ACT_ARG_NOFALLBACK_SID, aucun message de sera envoyé et l'erreur indiqué au code de strat sera de reessayer la la strat qui utilise cette commande plus tard (ACT_FUNCTION_RetryLater).
} QUEUE_arg_t;

/*
toute action contenant des variables statiques doit être utilisée
avec un seul actionneur
*/
typedef void(*action_function_t)(queue_id_e this, bool_e init);

void QUEUE_init();

/*Renvoie l'argument associé à la fonction, le contenu peut être modifié */
QUEUE_arg_t* QUEUE_get_arg(queue_id_e queue_id);

/*Renvoie le moment ou l'action a été initialisée */
time32_t QUEUE_get_initial_time(queue_id_e queue_id);

/*fonction de gestion des files*/
void QUEUE_run();

/*Ajout d'une action dans une file*/
bool_e QUEUE_add(queue_id_e queue_id, action_function_t action, QUEUE_arg_t arg);

/* Retire la fonction en tete de file et initialise la suivante. */
void QUEUE_next(queue_id_e queue_id);

/* Indique qu'une erreur est survenue lors de l'execution d'une fonction dans la file indiquée. Les fonctions suivant dans la file pourront agir en conséquence. */
void QUEUE_set_error(queue_id_e queue_id, bool_e error);

/* Retourne TRUE si une erreur est survenue lors de l'execution d'un fonction dans la file indiquée. */
bool_e QUEUE_has_error(queue_id_e queue_id);

/* vide la file */
void QUEUE_reset(queue_id_e queue_id);

/* vide toutes les files */
void QUEUE_reset_all();

#endif /* ndef QUEUE_H */
