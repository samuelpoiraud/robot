/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : queue.h
 *	Package : Carte Principale (Strat�gie)
 *	Description : files d'actions actionneurs
 *  Auteur : Alexis (bas� sur queue.c/h de la carte actionneur (18/03/2013, Jacen(modifi� par Aur�lien)))
 *  Version 20130318
 */

#include "QS/QS_all.h"

#ifndef QUEUE_H
#define QUEUE_H

#include "clock.h"

#define QUEUE_SIZE	16

//Ce fichier est li� � act_function.c/h (a cause de l'argument QUEUE_arg_t surtout)

typedef Uint8 queue_size_t;
typedef enum {
	//Holly
	ACT_QUEUE_Torch_locker,
	ACT_QUEUE_Pop_drop_left,
	ACT_QUEUE_Pop_drop_right,

	//Wood
	ACT_QUEUE_Small_arm,
	ACT_QUEUE_Pince_Gauche,
	ACT_QUEUE_Pince_Droite,
	//Common

	NB_QUEUE
} queue_id_e;

#define ACT_ARG_NOTIMEOUT ((time32_t)-1)

//Version all�g� pour nos besoin d'un message CAN, histoire de pas prendre trop de RAM dans la variable act_args ...
typedef struct {
	Uint11 sid;
	Uint8 data[8];
	Uint8 size;
} ACT_can_msg_t;

//Pour timeout: utiliser ACT_ARG_NOTIMEOUT pour ne pas mettre de timeout
typedef struct {
	Uint16 timeout;         //Temps avant timeout de l'action en ms (relatif au d�marrage de l'execution de l'action).
	ACT_can_msg_t msg;          //Message � envoyer pour executer l'action
	ACT_can_msg_t fallbackMsg;  //Quand on ne peut pas aller a la position demand�, envoyer ce message. Utilis� pour ACT_BEHAVIOR_GoalUnreachable. Si le SID est ACT_ARG_NOFALLBACK_SID, aucun message de sera envoy� et l'erreur indiqu� au code de strat sera de reessayer la la strat qui utilise cette commande plus tard (ACT_FUNCTION_RetryLater).
} QUEUE_arg_t;

/*
toute action contenant des variables statiques doit �tre utilis�e
avec un seul actionneur
*/
typedef void(*action_function_t)(queue_id_e this, bool_e init);

void QUEUE_init();

/*Renvoie l'argument associ� � la fonction, le contenu peut �tre modifi� */
QUEUE_arg_t* QUEUE_get_arg(queue_id_e queue_id);

/*Renvoie le moment ou l'action a �t� initialis�e */
time32_t QUEUE_get_initial_time(queue_id_e queue_id);

/*fonction de gestion des files*/
void QUEUE_run();

/*Ajout d'une action dans une file*/
bool_e QUEUE_add(queue_id_e queue_id, action_function_t action, QUEUE_arg_t arg);

/* Retire la fonction en tete de file et initialise la suivante. */
void QUEUE_next(queue_id_e queue_id);

/* Indique qu'une erreur est survenue lors de l'execution d'une fonction dans la file indiqu�e. Les fonctions suivant dans la file pourront agir en cons�quence. */
void QUEUE_set_error(queue_id_e queue_id, bool_e error);

/* Retourne TRUE si une erreur est survenue lors de l'execution d'un fonction dans la file indiqu�e. */
bool_e QUEUE_has_error(queue_id_e queue_id);

/* vide la file */
void QUEUE_reset(queue_id_e queue_id);

/* vide toutes les files */
void QUEUE_reset_all();

#endif /* ndef QUEUE_H */
