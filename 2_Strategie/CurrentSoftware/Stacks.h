/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech
 *
 *	Fichier : Stack.h
 *	Package : Carte Principale
 *	Description : Modele de piles pour Carte P
 *	Auteur : Jacen (Modifi� par Ronan)
 *	Version 20110313
 */

#include "QS/QS_all.h"

#ifndef STACKS_H
#define STACKS_H

#include "QS/QS_can.h"

typedef enum
{
	ASSER, ACT_STACK_BallLauncher, ACT_STACK_Plate, ACTUATORS_NB
}stack_id_e;

/*
 *	Mod�le de d�claration d'un pointeur sur fonction
 *	int (*pt2Function)(float, char, char) = NULL;
 */
 
/* ATTENTION toute action contenant des variables statiques doit �tre
   utilis�e avec un seul actionneur */
typedef void (*action_fun_t)(stack_id_e this, bool_e init);

/* Anciennement code � �t� impl�ment� par Christian Dreher et �tait plac�
*  dans Global_vars.h. Il y avait le commentaire suivant :
*
*  On m'a demand� de commenter la ligne suivante, en pr�textant que le
*  commentaire ajout� en fin de ligne ne serait pas suffisant pour expliciter
*  son utilit� :
*  Il s'agit ni plus ni moins que d'un tableau � deux dimensions
*  de pointeurs sur fonctions du type void foo (bool_e init);
*
*  Les fonctions point�es g�rerons la supervision de l'execution des taches
*  par les actionneurs.
*  Dans la dimension de ACTUATORS_NB, on choisira la pile de l'actionneur
*  et la dimension de STACKS_SIZE indique la hauteur des piles d'actions
*	 
*  void (*stacks[ACTUATORS_NB][STACKS_SIZE])(bool_e init);	//j'adore cette d�claration
*/

typedef struct
{
	action_fun_t action_function;
	time32_t initial_time;
}stack_action_t;

typedef enum
{
	NO_TIMEOUT=0,
	GOTO_TIMEOUT, GOTO_MULTI_POINT_TIMEOUT, RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT, 
	GOANGLE_TIMEOUT, RUSH_TIMEOUT,ACT_PREPARE_CLAMP_FRONT_TIMEOUT, 
	ACT_PREPARE_CLAMP_BACK_TIMEOUT, ACT_TAKE_FRONT_PAWN_TIMEOUT,
	ACT_TAKE_BACK_PAWN_TIMEOUT, ACT_FILED_FRONT_PAWN_TIMEOUT,
	ACT_FILED_BACK_PAWN_TIMEOUT, ACT_DCM_SET_POS_TIMEOUT
}timeout_e;

#include "Generic_functions.h"

void STACKS_init(void);

/* fait tourner chaque sommet de pile pour MaJ */
void STACKS_run(void);

/* retourne le sommet de la pile d�sign�e */
Uint8 STACKS_get_top(stack_id_e stack);

/* affecte le sommet � la pile d�sign�e */
void STACKS_set_top(stack_id_e stack, Uint8 index);

/* retourne la fonction � l'index d�sir� de la pile d�sign�e */
generic_fun_t STACKS_get_action(stack_id_e stack, Uint8 index);

/* retourne le temps d'�xectution de la fonction � l'index d�sir� de la pile d�sign�e */
time32_t STACKS_get_action_initial_time(stack_id_e stack, Uint8 index);

/* affecte le timeout � la pile d�sign�e */
void STACKS_set_timeout(stack_id_e stack, bool_e timeout);

/* ajoute une commande en sommet de pile et l'initialise */
void STACKS_push(stack_id_e stack_id, generic_fun_t command, bool_e init);

/* retire la fonction en sommet de pile reinitialise la suivante. */
void STACKS_pull(stack_id_e stack_id);

/* vide la pile */
void STACKS_flush(stack_id_e stack_id);

/* vide toutes les piles */
void STACKS_flush_all();

/* 	indique si une pile est vide (ie si son sommet est wait_forever) et d�pile
 *	automatiquement le sommet de la pile si il tombe en timeout, en passant le
 *	bool�en point� en argument � vrai.
 */
bool_e STACKS_wait_end_auto_pull (stack_id_e stack_id, bool_e* got_timeout);

/* passe tous les indicateurs de timeout � NO_TIMEOUT. Appel dans env_clean */
void STACKS_clear_timeouts();

#endif /* def STACKS_H */
