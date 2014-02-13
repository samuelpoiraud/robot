/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Test_main.c
 *	Package : Carte Principale
 *	Description : Modele de piles pour Carte P
 *	Auteur : Jacen (Modifié par Ronan)
 *	Version 20110313
 */

#define STACKS_C
#include "Stacks.h"
#include "QS/QS_outputlog.h"

typedef struct
{
	stack_id_e stack_id;
	stack_action_t action[STACKS_SIZE];
	Uint8 stack_top;
	timeout_e timeout;
}stacks_t;

/* Piles contenant une action, un sommet et état de timeout */
static stacks_t stacks[ACTUATORS_NB];

void CAN_send_debug(char* string);

void STACKS_init(void)
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;

	Uint8 i;
	CAN_init();
	stacks_t* stack;

	for (i=0; i<ACTUATORS_NB; i++)
	{
		stack=&stacks[i];
		stack->stack_id=i;
		stack->action[0].action_function=&wait_forever;
		stack->action[0].initial_time=0;
		stack->stack_top=0;
		stack->timeout=0;
	}
}

/* fait tourner chaque sommet de pile pour MaJ */
void STACKS_run(void)
{
	stack_id_e i;
	stacks_t* stack;
	for (i=0; i<ACTUATORS_NB; i++)
	{
		stack=&stacks[i];
		stack->action[stack->stack_top].action_function(i,FALSE);
	}
}

/* retourne le sommet de la pile désignée */
Uint8 STACKS_get_top(stack_id_e stack)
{
	return (stacks[stack].stack_top);
}

/* affecte le sommet de la pile */
void STACKS_set_top(stack_id_e stack, Uint8 index)
{
	stacks[stack].stack_top=index;
}

/* retourne la fonction à l'index désiré et de la pile désignée */
generic_fun_t STACKS_get_action(stack_id_e stack, Uint8 index)
{
	return (stacks[stack].action[index].action_function);
}

/* retourne le temps d'éxectution de la fonction à l'index désiré de la pile désignée */
time32_t STACKS_get_action_initial_time(stack_id_e stack, Uint8 index)
{
	return (stacks[stack].action[index].initial_time);
}

/* affecte le timeout à la pile désignée */
void STACKS_set_timeout(stack_id_e stack, bool_e timeout)
{
	stacks[stack].timeout=timeout;
}

/* Ajoute une fonction en sommet de pile et l'initialise */
void STACKS_push(stack_id_e stack_id, generic_fun_t command, bool_e run)
{
	stacks_t* stack=&stacks[stack_id];
	assert(stack->stack_top < STACKS_SIZE - 1);
	stack->action[++stack->stack_top].action_function=command;
	stack->action[stack->stack_top].initial_time=global.env.match_time;
	if(run)
	{
		command(stack_id,TRUE);
	}
}
/* Retire la fonction en sommet de pile et reinitialise la suivante. */
void STACKS_pull(stack_id_e stack_id)
{
	CAN_send_debug("AAAAAAA");
	stacks_t* stack=&stacks[stack_id];
	assert(stack->stack_top > 0);
	stack->action[--stack->stack_top].initial_time=global.env.match_time;
	stack->action[stack->stack_top].action_function(stack_id,TRUE);

}

/* vide la pile */
void STACKS_flush(stack_id_e stack_id)
{
	CAN_send_debug("BBBBBBB");
	stacks[stack_id].stack_top=0;
}

/* vide toutes les piles */
void STACKS_flush_all()
{
	stack_id_e i;
	for (i=0; i<ACTUATORS_NB; i++)
		STACKS_flush(i);
}

bool_e STACKS_wait_end_auto_pull (stack_id_e stack_id, bool_e* got_timeout)
{
	stacks_t* stack=&stacks[stack_id];
	*got_timeout=FALSE;	//On suppose qu'il n'y a pas de timeout.
	if (stack->timeout)
	{
		#ifdef VERBOSE_MODE
			debug_printf("TIMEOUT (stack_id : %d)\n", stack_id);
		#endif
		*got_timeout=TRUE;	//En fait, il y a un timeout.
		if(STACKS_get_action(stack_id,STACKS_get_top(stack_id))!=&wait_forever)
			STACKS_pull(stack_id);
	} else if (STACKS_get_action(stack_id,STACKS_get_top(stack_id))==&wait_forever)
	{
		return TRUE;	//On a atteint le bas de la pile.
	}
	return FALSE;
}

void STACKS_clear_timeouts()
{
	stack_id_e i;
	for (i=0; i<ACTUATORS_NB; i++)
		stacks[i].timeout=NO_TIMEOUT;
}
