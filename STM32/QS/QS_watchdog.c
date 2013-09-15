/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_watchdog.c
 *  Package : Qualit� Soft
 *  Description : Timers abstraits / watchdog
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#include "QS_watchdog.h"

#ifdef USE_WATCHDOG

#ifndef WATCHDOG_TIMER
#error "WATCHDOG_TIMER doit etre 1 2 3 ou 4"
#else
	#define TIMER_SRC_TIMER_ID WATCHDOG_TIMER
	#undef TIMER_SRC_USE_WATCHDOG  //On doit �tre sur de ne pas utiliser un watchdog comme timer pour les watchdog, ce qui ferait une boucle � l'init ...
	#include "QS_setTimerSource.h"
#endif

typedef struct
{
	timeout_t timeout;
	timeout_t counter;
	watchdog_callback_fun_t callback;
	bool_e initialized;
	bool_e enabled;
	volatile bool_e* flag;
	bool_e periodic;
}watchdog_t;

static volatile watchdog_t watchdog[WATCHDOG_MAX_COUNT];


void WATCHDOG_init(void)
{
	static bool_e initialized = FALSE;
	watchdog_id_t i;
	if(initialized)
		return;
	TIMER_SRC_TIMER_init();
	for (i=0; i<WATCHDOG_MAX_COUNT; i++)
		watchdog[i].initialized = FALSE;
	TIMER_SRC_TIMER_start_ms(WATCHDOG_QUANTUM);
	initialized = TRUE;
}


/** Priv�e: ajoute un nouveau watchdog
	* Cette fonction est utilis�e pour ajoute un nouveau watchdog avec les caract�ristiques sp�cifi�s
**/
watchdog_id_t WATCHDOG_new(timeout_t t, watchdog_callback_fun_t func, volatile bool_e * flag, bool_e is_periodic)
{
	/* Vars */
	Uint8 i;
	watchdog_id_t id = 255;
	assert(t >= WATCHDOG_QUANTUM);

	TIMER_SRC_TIMER_DisableIT();

	for (i = 0; i < WATCHDOG_MAX_COUNT; i++)
		if((watchdog[i].initialized) && func != NULL && watchdog[i].callback == func)
		{
			return 255;	//PAS DE CREATION !
		}
		


	/* Parcours des watchdogs jusqu'� trouver un espace libre */
	for (i = 0; i < WATCHDOG_MAX_COUNT; i++)
	{
		if (!(watchdog[i].initialized))
		{
			/* On ne veut pas de temps bizzaro�de */
			watchdog[i].timeout = t - t%WATCHDOG_QUANTUM;
			watchdog[i].callback = func;
			watchdog[i].flag = flag;
			watchdog[i].periodic = is_periodic;
			id = i;
			watchdog[i].enabled = TRUE;
			watchdog[i].initialized = TRUE;
			break;
		}
	}	
	if(id== 255)
		debug_printf("WD : TABLEAU FULL");

	TIMER_SRC_TIMER_EnableIT();
	return id;
}


watchdog_id_t WATCHDOG_create(timeout_t t, watchdog_callback_fun_t f, bool_e is_periodic){
	assert(f != NULL);
 	return WATCHDOG_new(t, f, NULL, is_periodic);
}

watchdog_id_t WATCHDOG_create_flag(timeout_t t, volatile bool_e * f){
	assert(f != NULL);
	*f = FALSE;
 	return WATCHDOG_new(t, NULL, f, FALSE);
}

/* Arr�t d'un watchdog par mise � z�ro de ses descripteurs */
void WATCHDOG_stop(watchdog_id_t id)
{
	watchdog[id].initialized = FALSE;
}

void WATCHDOG_disable_timeout(watchdog_id_t id) {
	watchdog[id].enabled = FALSE;
}

void WATCHDOG_enable_timeout(watchdog_id_t id) {
	watchdog[id].enabled = TRUE;
}

/* Interruption appell�e toutes les QUANTUM ms */
void TIMER_SRC_TIMER_interrupt()
{
	watchdog_id_t i;
	watchdog_callback_fun_t callback = 0;
	
	/* Parcours de tous les watchdogs */
	for (i = 0; i < WATCHDOG_MAX_COUNT; i++)
	{
		if (watchdog[i].initialized)
		{
			//si le watchdog est d�clench�, pas la peine d'aller prendre un overflow s'il est d�sactiv� (c'est m�me plut�t conseill� pour �viter les bugs)
			if(watchdog[i].counter < watchdog[i].timeout)
				watchdog[i].counter += WATCHDOG_QUANTUM;

			//Si timeout == WATCHDOG_QUANTUM, on doit rentrer dans le if a tous les coups (counter aura �t� mis de 0 � WATCHDOG_QUANTUM avant)
			if(watchdog[i].enabled && watchdog[i].counter >= watchdog[i].timeout)
			{
				callback = watchdog[i].callback;
				
				if (watchdog[i].flag)
					*(watchdog[i].flag)=TRUE;
					
				/* Arr�t du watchdog si non p�riodique */
				if(watchdog[i].periodic == FALSE)
					WATCHDOG_stop(i);
				
				watchdog[i].counter = 0;

				/*On appelle la fonction de callback apr�s le stop pour permettre une recr�ation des fonctions callback par elles-m�mes*/
				if(callback)
					callback();
			}	
		}
	}
	TIMER_SRC_TIMER_resetFlag();
}

#endif /* def USE_WATCHDOG */
