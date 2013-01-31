/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : global_clock.c
 *	Package : EVE
 *	Description : Horloge générale de gestion du temps
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111215
 */

#define GLOBAL_CLOCK_C

#include "global_clock.h"

// Thread de la gestion du temps général
void* GLOBAL_CLOCK_thread()
{
	Uint16 error;
	Uint16 current_status;
	char* memory_reference;

	GLOBAL_VARS_set_threads_state(THREAD_GLOBAL_CLOCK,TRUE);

	// Initialisation de l'horloge, création et attachement de la zone mémoire
	error = EVE_create_and_bound_global_clock_memory(&memory_reference);
	if(error != END_SUCCESS)
	{
		// On a une erreur donc on ne peut pas aller plus loin
		EVE_printf("Thread global clock : Fail create clock memory\n");
		GLOBAL_VARS_set_threads_state(THREAD_GLOBAL_CLOCK,FALSE);
		return END_THREAD;
	}

	// Mise à zéro de l'horloge
	error = GLOBAL_CLOCK_update(memory_reference, 0);
	if(error != END_SUCCESS)
	{
		// Impossible de mettre à jour l'horloge
		EVE_printf("Thread global clock : Fail set 0 clock memory\n");
		GLOBAL_VARS_set_threads_state(THREAD_GLOBAL_CLOCK,FALSE);
		return END_THREAD;
	}

	// Boucle d'incrémentation du temps
	current_status = GLOBAL_VARS_get_sim_status();

	// Doit être désactivée lorsque l'IHM envoie un STOP général
	while(current_status != SIMULATION_QUIT)
	{
		if(current_status == SIMULATION_RUN)
		{
			// Attente du temps de progression en microsecondes
			usleep(GLOBAL_CLOCK_TIME_PROGRESS);

			error = GLOBAL_CLOCK_update(memory_reference, global.global_time+1);
			if(error != END_SUCCESS)
			{
				// Impossible de mettre à jour l'horloge
				EVE_printf("Thread global clock : Fail set ++ clock memory\n");
				GLOBAL_VARS_set_threads_state(THREAD_GLOBAL_CLOCK,FALSE);
				return END_THREAD;
			}
		}
		else if(current_status == SIMULATION_PAUSE)
		{
			usleep(100000);	// Libère du temps CPU
		}
		else if(current_status == SIMULATION_STOP)
		{
			// On remet l'horloge à zéro pour la prochaine simulation
			error = GLOBAL_CLOCK_update(memory_reference, 0);
			if(error != END_SUCCESS)
			{
				// Impossible de mettre à jour l'horloge
				EVE_printf("Thread global clock : Fail reboot clock memory\n");
				GLOBAL_VARS_set_threads_state(THREAD_GLOBAL_CLOCK,FALSE);
				return END_THREAD;
			}
		}

		// Mise à jour de l'état de la simulation pour la prochaine boucle
		current_status = GLOBAL_VARS_get_sim_status();
	}

	error = GLOBAL_CLOCK_unbound_memory(memory_reference);
	if(error != END_SUCCESS)
	{
		printf("Fail unbound clock\n");
	}

	GLOBAL_VARS_set_threads_state(THREAD_GLOBAL_CLOCK,FALSE);

	return END_THREAD;
}

// Fonction d'écriture pour mettre l'horloge à l'heure
static Uint16 GLOBAL_CLOCK_update(char* memory_reference, EVE_time_t new_time)
{
	EVE_clock_memory_t new_data;

	// Mise en place des nouvelles valeurs dans la structure
	new_data.global_time_memory = new_time;

	pthread_mutex_lock(&global.global_time_mutex);

	// Mise à jour de la variable globale
	global.global_time = new_time;

	// Ecriture dans la mémoire partagée de la nouvelle valeur de l'horloge
	*((EVE_clock_memory_t*)memory_reference) = new_data;

	pthread_mutex_unlock(&global.global_time_mutex);

	return END_SUCCESS;
}

// Fonction de destruction du temps général
static Uint16 GLOBAL_CLOCK_unbound_memory(char* memory_reference)
{
	Uint16 error;

	error = EVE_unbound_global_clock_memory(memory_reference);
	if(error != END_SUCCESS)
	{
		return GLOBAL_CLOCK_errors(ERROR_UNBOUND_CLOCK,"Error to unbound clock");
	}
	return END_SUCCESS;
}

// Gestion des retours d'erreurs
static Uint16 GLOBAL_CLOCK_errors(Uint16 error_id, char* commentary)
{
	switch(error_id)
	{
		case END_SUCCESS:
			printf("END_SUCCESS : %s\n",commentary);
			break;

		case ERROR_UNBOUND_CLOCK:
			printf("ERROR_UNBOUND_CLOCK : %s\n",commentary);
			break;

		case ERROR_CREATE_CLOCK:
			printf("ERROR_CREATE_CLOCK : %s\n",commentary);
			break;

		case ERROR_UPDATE_CLOCK:
			printf("ERROR_UPDATE_CLOCK : %s\n",commentary);
			break;

		default:
			printf("error_id inconnu : %d, %s\n",error_id,commentary);
			break;
	}

	return error_id;
}

