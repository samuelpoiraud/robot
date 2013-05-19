/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_watchdog.c
 *  Package : Qualité Soft
 *  Description : Timers abstraits / watchdog
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#include "QS_watchdog.h"

#ifdef USE_WATCHDOG

#include "QS_timer.h"

#if WATCHDOG_TIMER == 1
	#define WATCHDOG_TIMER_RUN	TIMER1_run
	#define WATCHDOG_TIMER_IT	TIM1_UP_TIM10_IRQHandler
	#define WATCHDOG_TIMER_HANDLE TIM1
#elif WATCHDOG_TIMER == 2
	#define WATCHDOG_TIMER_RUN	TIMER2_run
	#define WATCHDOG_TIMER_IT	TIM2_IRQHandler
	#define WATCHDOG_TIMER_HANDLE TIM2
#elif WATCHDOG_TIMER == 3
	#define WATCHDOG_TIMER_RUN	TIMER3_run
	#define WATCHDOG_TIMER_IT	TIM3_IRQHandler
	#define WATCHDOG_TIMER_HANDLE TIM3
#elif WATCHDOG_TIMER == 4
	#define WATCHDOG_TIMER_RUN	TIMER4_run
	#define WATCHDOG_TIMER_IT	TIM4_IRQHandler
	#define WATCHDOG_TIMER_HANDLE TIM4
#else
	#error "WATCHDOG_TIMER doit etre 1 2 3 ou 4"
#endif /* WATCHDOG_TIMER == n */

typedef struct
{
	bool_e initialized;
	bool_e enabled;
	timeout_t timeout;
	watchdog_callback_fun_t callback;
	bool_e* flag;
}watchdog_t;

static volatile watchdog_t watchdog[WATCHDOG_MAX_COUNT];


void WATCHDOG_init(void)
{
	watchdog_id_t i;
	TIMER_init();
	for (i=0; i<WATCHDOG_MAX_COUNT; i++)
		watchdog[i].initialized = FALSE;
	WATCHDOG_TIMER_RUN(WATCHDOG_QUANTUM);
}


/** Privée: ajoute un nouveau watchdog
	* Cette fonction est utilisée pour ajoute un nouveau watchdog avec les caractéristiques spécifiés
**/
watchdog_id_t WATCHDOG_new(timeout_t t, watchdog_callback_fun_t func, bool_e* flag)
{
	/* Vars */
	Uint8 i;
	watchdog_id_t id = 255;
	assert(t >= WATCHDOG_QUANTUM);

	TIM_ITConfig(WATCHDOG_TIMER_HANDLE, TIM_IT_Update, DISABLE);

	

	for (i = 0; i < WATCHDOG_MAX_COUNT; i++)
		if((watchdog[i].initialized) && watchdog[i].callback == func)
		{
			return 255;	//PAS DE CREATION !
		}



	/* Parcours des watchdogs jusqu'à trouver un espace libre */
	for (i = 0; i < WATCHDOG_MAX_COUNT; i++)
	{
		if (!(watchdog[i].initialized))
		{
			/* On ne veut pas de temps bizzaroïde */
			watchdog[i].timeout = t - t%WATCHDOG_QUANTUM;
			watchdog[i].callback = func;
			watchdog[i].flag = flag;
			id = i;
			watchdog[i].enabled = TRUE;
			watchdog[i].initialized = TRUE;
			break;
		}
	}	
	if(id== 255)
		debug_printf("WD : TABLEAU FULL");

	//FIXME: vérifier si il était déjà activé avant ou pas
	TIM_ITConfig(WATCHDOG_TIMER_HANDLE, TIM_IT_Update, ENABLE);
	return id;
}


watchdog_id_t WATCHDOG_create(timeout_t t, watchdog_callback_fun_t f){
	assert(f != NULL);
 	return WATCHDOG_new(t, f, NULL);
}

watchdog_id_t WATCHDOG_create_flag(timeout_t t, bool_e* f){
	assert(f != NULL);
	*f = FALSE;
 	return WATCHDOG_new(t, NULL, f);
}

/* Arrêt d'un watchdog par mise à zéro de ses descripteurs */
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

/* Interruption appellée toutes les QUANTUM ms */
void _ISR WATCHDOG_TIMER_IT()
{
	if(TIM_GetITStatus(WATCHDOG_TIMER_HANDLE, TIM_IT_Update)) {
		watchdog_id_t i;
		watchdog_callback_fun_t callback = 0;

		/* Parcours de tous les watchdogs */
		for (i = 0; i < WATCHDOG_MAX_COUNT; i++)
		{
			if (watchdog[i].initialized)
			{
				if(watchdog[i].timeout)
				{
					watchdog[i].timeout -= WATCHDOG_QUANTUM;
				}
				else if(watchdog[i].enabled)
				{
					callback = watchdog[i].callback;

					if (watchdog[i].flag)
						*(watchdog[i].flag)=TRUE;

					/* Arrêt du watchdog */
					WATCHDOG_stop(i);

					/*On appelle la fonction de callback après le stop pour permettre une recréation des fonctions callback par elles-mêmes*/
					if(callback)
						callback();
				}
			}
		}
		TIM_ClearITPendingBit(WATCHDOG_TIMER_HANDLE, TIM_IT_Update);
	}
}

#endif /* def USE_WATCHDOG */
