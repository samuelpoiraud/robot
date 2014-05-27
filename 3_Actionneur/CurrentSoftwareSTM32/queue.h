/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : queue.h
 *	Package : Actionneur
 *	Description : files d'actions actionneurs
 *  Auteurs : Jacen (modifié par Aurélien)
 *  Version 20110225
 */

#include "QS/QS_all.h"

#ifndef QUEUE_H
#define QUEUE_H

#include "clock.h"


/* Info sur l'enum QUEUE_act_e:
 *  Cet enum est défini dans le fichier Global_vars_types.h du robot compilé et contient la liste des actionneurs utilisés pour effectuer des actions.
 *  Pour Krusty, voir Krusty/KGLobal_vars_types.h
 *  Pour Tiny, voir Tiny/TGlobal_vars_types.h
 */

typedef Uint8 queue_size_t;
typedef Uint8 queue_id_t;
typedef Sint16 sem_id_t;

//Retourn FALSE quand il y a erreur, TRUE sinon.
typedef bool_e (*OperationFinishedCallback)(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

typedef struct {
	Uint8 canCommand;
	Uint16 param;
	OperationFinishedCallback callback; //function called when operation is finished, can be NULL
} QUEUE_arg_t;

#define NB_QUEUE	10
#define QUEUE_SIZE	32
#define NB_SYNCHRO 8
#define QUEUE_SYNCHRO_TIMEOUT 150 //15 secondes

// Valeur renvoyée par QUEUE_create si l'opération echoue
#define QUEUE_CREATE_FAILED	0xFF

//Valeur renvoyé par QUEUE_sem_create si l'opération echoue
#define QUEUE_SEM_CREATE_FAILED	0xFF

/*
toute action contenant des variables statiques doit être utilisée
avec un seul actionneur (protégé par sémaphore)
*/
typedef void(*action_t)(queue_id_t thiss, bool_e init);

void QUEUE_init();

/*Renvoie l'argument associé à la fonction, le contenu peut être modifié*/
QUEUE_arg_t* QUEUE_get_arg(queue_id_t queue_id);

/*Renvoie l'actionneur utilisé*/
QUEUE_act_e QUEUE_get_act(queue_id_t queue_id);

/*Renvoie le moment ou l'action a été initialisée*/
clock_time_t QUEUE_get_initial_time(queue_id_t queue_id);

/*Renvoie si la file rentrée en paramètre est disponible*/
bool_e is_available(queue_id_t queue_id);

/*Création d'une file*/
queue_id_t QUEUE_create();

/*fonction de gestion des files*/
void QUEUE_run();

/*Ajout d'une action dans une file*/
void QUEUE_add(queue_id_t queue_id, action_t action, QUEUE_arg_t optionnal_arg, QUEUE_act_e optionnal_act);

/* Retire la fonction en tete de file et reinitialise la suivante. */
void QUEUE_behead(queue_id_t queue_id);

/* Appelle la fonction OperationFinishedCallback et passe à la fonction suivante. Voir en haut du .h */
void QUEUE_next(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

/* Indique qu'une erreur est survenue lors de l'execution d'une fonction dans la file indiquée. Les fonctions suivant dans la file pourront agir en conséquence. */
void QUEUE_set_error(queue_id_t queue_id);

/* Retourne TRUE si une erreur est survenue lors de l'execution d'un fonction dans la file indiquée. */
bool_e QUEUE_has_error(queue_id_t queue_id);

/* Retourne le nombre d'action dans la file (celle courante + celles en attentes ou 0 si la file est inactive) */
Uint8 QUEUE_pending_num(queue_id_t queue_id);

/* Récupère les informations de l'action suivante, les informations sont mises dans les variables pointeur s'ils ne sont pas NULL */
/* Exemple: QUEUE_next_action_info(queue_id, &next_action, NULL, &next_optionnal_act); */
bool_e QUEUE_next_action_info(queue_id_t queue_id, action_t *action, QUEUE_arg_t *optionnal_arg, QUEUE_act_e *optionnal_act);

/* vide la file et rend la semaphore de l'actionneur associé*/
void QUEUE_flush(queue_id_t queue_id);

/* vide toutes les files */
void QUEUE_flush_all();

/*Prend l'actionneur (sémaphore), le numéro du semaphore doit être mis dans le paramètre param (voir QUEUE_arg_t), 0 si on utilise le semaphore de l'actionneur*/
void QUEUE_take_sem(queue_id_t thisa, bool_e init);

/*Libère l'actionneur, le numéro du semaphore doit être mis dans le paramètre param (voir QUEUE_arg_t), 0 si on utilise le semaphore de l'actionneur*/
void QUEUE_give_sem(queue_id_t thisa, bool_e init);

/*Prend la sémaphore de synchronisation*/
sem_id_t QUEUE_sem_create();

/*Rend la sémaphore de synchronisation*/
void QUEUE_sem_delete(queue_id_t thisa, bool_e init);

/*Attend la synchronisation*/
void QUEUE_wait_synchro(queue_id_t thisa, bool_e init);

#endif /* ndef QUEUE_H */
