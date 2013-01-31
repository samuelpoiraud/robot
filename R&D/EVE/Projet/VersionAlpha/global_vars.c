/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : global_vars.c
 *	Package : EVE
 *	Description : Variables globales utilisés dans le simulateur
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111215
 */

#define GLOBAL_VARS_C

#include "global_vars.h"

// Fonction d'initialisation des variables globales
Uint16 GLOBAL_VARS_init()
{
	Uint16 init_counter;

	// Initialisation de l'utilisation des cartes
	pthread_mutex_lock(&global.use_cards_mutex);
	for(init_counter=0;init_counter<CARDS_NB;init_counter++)
	{
		global.use_cards[init_counter] = 0;
	}
	pthread_mutex_unlock(&global.use_cards_mutex);

	// Initialisation de l'état des threads
	pthread_mutex_lock(&global.threads_state_mutex);
	for(init_counter=0;init_counter<THREADS_NUMBER;init_counter++)
	{
		global.threads_state[init_counter] = FALSE;
	}
	pthread_mutex_unlock(&global.threads_state_mutex);

	// Initialisation de l'horloge générale
	pthread_mutex_lock(&global.global_time_mutex);
	global.global_time = 0;
	pthread_mutex_unlock(&global.global_time_mutex);

	// Initialisation de l'état de la simulation
	GLOBAL_VARS_set_sim_status(SIMULATION_STOP);

	// Initialisation de l'état reset des cartes
	GLOBAL_VARS_set_reset_cards(FALSE);

	// Initialisation de notebook_page
	pthread_mutex_lock(&global.notebook_page_mutex);
	global.notebook_page = 0;
	pthread_mutex_unlock(&global.notebook_page_mutex);

	return END_SUCCESS;
}

// Fonction permettant de connaître l'état actuel de la simulation
Uint16 GLOBAL_VARS_get_sim_status()
{
	Uint16 current_status;
	pthread_mutex_lock(&global.sim_status_mutex);
	current_status = global.sim_status;
	pthread_mutex_unlock(&global.sim_status_mutex);

	return current_status;
}

// Fonction permettant de mettre à jour l'état courant de la simulation
Uint16 GLOBAL_VARS_set_sim_status(Uint16 new_status)
{
	pthread_mutex_lock(&global.sim_status_mutex);
	global.sim_status = new_status;
	pthread_mutex_unlock(&global.sim_status_mutex);

	return END_SUCCESS;
}

// Fonction permettant de connaître la valeur de use_cards
Uint16 GLOBAL_VARS_get_use_cards(Uint16 card_id)
{
	Uint16 result;
	if(card_id >= CARDS_NB)
	{
		EVE_printf("Error : card_id > CARDS_NB\n");
		return 0;	// On a une erreur
	}

	pthread_mutex_lock(&global.use_cards_mutex);
	result = global.use_cards[card_id];
	pthread_mutex_unlock(&global.use_cards_mutex);

	return result;
}

// Fonction permettant de connaître la valeur de threads_state
bool_e GLOBAL_VARS_get_threads_state(Uint16 thread_id)
{
	bool_e state = FALSE;

	pthread_mutex_lock(&global.threads_state_mutex);
	state = global.threads_state[thread_id];
	pthread_mutex_unlock(&global.threads_state_mutex);

	return state;
}

// Fonction permettant de mettre à jour la valeur de threads_state
Uint16 GLOBAL_VARS_set_threads_state(Uint16 thread_id, bool_e state)
{
	pthread_mutex_lock(&global.threads_state_mutex);
	global.threads_state[thread_id] = state;
	pthread_mutex_unlock(&global.threads_state_mutex);

	return END_SUCCESS;
}

// Fonction permettant de connaître si un reset a été effectué
bool_e GLOBAL_VARS_get_reset_cards()
{
	bool_e state = FALSE;

	pthread_mutex_lock(&global.reset_cards_mutex);
	state = global.reset_cards;
	pthread_mutex_unlock(&global.reset_cards_mutex);

	return state;
}

// Fonction permettant de mettre à jour la valeur reset_cards
Uint16 GLOBAL_VARS_set_reset_cards(bool_e state)
{
	pthread_mutex_lock(&global.reset_cards_mutex);
	global.reset_cards = state;
	pthread_mutex_unlock(&global.reset_cards_mutex);

	return END_SUCCESS;
}

