/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_watchdog.c
 *  Package : Qualité Soft
 *  Description : Timers abstraits / watchdog
 *  Auteur : Gwenn
 *  Version 20100424
 */

#define QS_WATCHDOG_C
#include "../QS/QS_watchdog.h"

#ifdef USE_WATCHDOG

/** Suit une partie rébarbative
	* Permet de définir de manière simple quel timer est utilisé
	* au moyen de WATCHDOG_TIMER
**/
#if WATCHDOG_TIMER == 2
		#define WATCHDOG_TIMER_RUN	TIMER2_run
		#define WATCHDOG_TIMER_IT	TIM2_IRQHandler
		#define WATCHDOG_TIMER_FLAG	TIM2
#elif WATCHDOG_TIMER == 5
		#define WATCHDOG_TIMER_RUN	TIMER5_run
		#define WATCHDOG_TIMER_IT	TIM5_IRQHandler
		#define WATCHDOG_TIMER_FLAG	TIM5
#elif WATCHDOG_TIMER == 6
		#define WATCHDOG_TIMER_RUN	TIMER6_run
		#define WATCHDOG_TIMER_IT	TIM6_IRQHandler
		#define WATCHDOG_TIMER_FLAG	TIM6
#elif WATCHDOG_TIMER == 7
		#define WATCHDOG_TIMER_RUN	TIMER7_run
		#define WATCHDOG_TIMER_IT	TIM7_IRQHandler
		#define WATCHDOG_TIMER_FLAG	TIM7
#else
	#error "WATCHDOG_TIMER doit être TIM2, TIM5, TIM6 TIM7"
#endif



void WATCHDOG_init(void){
	TIMER_init();
	WATCHDOG_TIMER_RUN(QUANTUM);
}


watchdog_id_t WATCHDOG_new(timeout_t t, func_ptr_t func, bool_e* flag){
	
	/* Vars */
	Uint8 i;
	watchdog_id_t id = 254;
	// TODO assert(t >= QUANTUM);
	
	/* Parcours des watchdogs jusqu'à trouver un espace libre */
	for (i = 0; i < WATCHDOG_MAX_COUNT; i++){
		if (!(Watchdog_timeout_list[i]) && !(Watchdog_function_list[i])){
			
			/* On ne veut pas de temps bizzaroïde */
			Watchdog_timeout_list[i] = t - t%QUANTUM;
			Watchdog_function_list[i] = func;
			Watchdog_flag_list[i] = flag;
			// TODO debug_printf("WD%d: %dms\r\n", i, Watchdog_timeout_list[i]);
			
			id = i;
			break;
		}
	}	
	return id;
}


watchdog_id_t WATCHDOG_create(timeout_t t, func_ptr_t f){
	// TODO assert(f != NULL);
 	return WATCHDOG_new(t, f, NULL);
}

watchdog_id_t WATCHDOG_create_flag(timeout_t t, bool_e* f){
	// TODO assert(f != NULL);
	*f = FALSE;
 	return WATCHDOG_new(t, NULL, f);
}

/* Arrêt d'un watchdog par mise à zéro de ses descripteurs */
void WATCHDOG_stop(watchdog_id_t id){
	Watchdog_function_list[id] = NULL;
	Watchdog_timeout_list[id] = (Uint32)NULL;
	Watchdog_flag_list[id] = NULL;
}


/* Interruption appellée toutes les QUANTUM ms */
void WATCHDOG_TIMER_IT(void){
	Uint8 i;
	func_ptr_t func;
	bool_e* flag;
	
	/* Parcours de tous les watchdogs */
	for (i = 0; i < WATCHDOG_MAX_COUNT; i++){
		
		func = Watchdog_function_list[i];
		flag = Watchdog_flag_list[i];
		
		/* Décrément du watchdog si non expiré */
		if(Watchdog_timeout_list[i] >= QUANTUM)
			Watchdog_timeout_list[i] -= QUANTUM;
		
		/* Appel de la fonction ou MAJ flag si watchdog expiré et fonction correspondante bien définie */
		else if (func || flag){
			
			/* Appel d'une fonction timeout */
			if (func)		(*func)();
			
			/* Mise à TRUE du flag demandé */
			else if (flag) 	*flag = TRUE;
			
			/* Arrêt du watchdog */
			WATCHDOG_stop(i);
			// TODO debug_printf("WD%d: OK\r\n", i);
		}
	}
	TIM_ClearITPendingBit(WATCHDOG_TIMER_FLAG, TIM_IT_Update);
}

#endif /* def USE_WATCHDOG */
