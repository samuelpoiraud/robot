/*
 *  Club Robot ESEO 2008 - 2012
 *
 *  Fichier : supervisor.h
 *  Package : Asser
 *  Description : Surveillance générale - Détection des erreurs - Etat général de fonctionnement
 *  Auteur : Nirgal 2012
 *  Version 201203
 */
 
#ifndef _SUPERVISOR_H
	#define _SUPERVISOR_H

	#include "QS/QS_all.h"
		
	

	
	typedef enum {
		SUPERVISOR_INIT = 0,
		SUPERVISOR_IDLE,
		SUPERVISOR_TRAJECTORY,
		SUPERVISOR_ERROR,
		SUPERVISOR_MATCH_ENDED
	} SUPERVISOR_state_e;
		
	typedef enum
	{
		EVENT_NEW_ORDER,
		EVENT_BRAKING,
		EVENT_ARRIVED,
		EVENT_NOTHING_TO_DO,
		EVENT_BROADCAST_START,
		EVENT_BROADCAST_STOP,
		EVENT_ERROR,
		EVENT_ERROR_EXIT
	}SUPERVISOR_event_e;
		
	
	#include "warner.h"
	
	void SUPERVISOR_state_machine(SUPERVISOR_event_e event, acknowledge_e ack);
	
	void SUPERVISOR_init(void);
	void SUPERVISOR_process_it(void);
	
	SUPERVISOR_state_e SUPERVISOR_get_state(void);
	void SUPERVISOR_number_of_rounds_returns_increment(void);

#endif //def _SUPERVISOR_H
