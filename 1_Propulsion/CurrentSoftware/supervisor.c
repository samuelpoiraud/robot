/*
 *  Club Robot ESEO 2006 - 2007
 *  Game Hoover
 *
 *  Fichier : it.c
 *  Package : Asser
 *  Description : fonctions en it...
 *  Auteur : Koubi  (2009) et Nirgal (2009) inspiré du code de Val' (2007)
 *  Version 200904
 */

#define _SUPERVISOR_C

#include "supervisor.h"
#include "copilot.h"
#include "roadmap.h"
#include "warner.h"
#include "buffer.h"
#include "corrector.h"
#include "pilot.h"
#include "QS/QS_CANmsgList.h"

//Machine a état générale

void SUPERVISOR_error_check_enable(bool_e enable);  //Active la surveillance des erreurs.
void SUPERVISOR_error_check(bool_e raz_cteur_immo);

volatile SUPERVISOR_state_e state = SUPERVISOR_INIT;
volatile Uint8 number_of_rounds_returns;
volatile bool_e error_check_enable;
volatile bool_e error_transmitted;
volatile SUPERVISOR_error_source_e error_source = NO_ERROR;
volatile acknowledge_e current_acknowledge = NO_ACKNOWLEDGE;

void SUPERVISOR_init(void)
{
	SUPERVISOR_state_machine(EVENT_NOTHING_TO_DO,FALSE);
	SUPERVISOR_error_check_enable(TRUE);
}


void SUPERVISOR_process_it(void)
{
	SUPERVISOR_error_check(FALSE);	//Surveillance des erreurs...
}




void SUPERVISOR_state_machine(SUPERVISOR_event_e event, acknowledge_e ack)
{
	//ATTENTION, cette machine est une machine à évènement qui produit un état du supervisor...
	switch(event)
	{
		//EVENEMENT DECLENCHANT LA REMISE A IDLE DU SUPERVISOR.
		case EVENT_ERROR_EXIT:		//no break
		case EVENT_BROADCAST_START:	//no break
			SUPERVISOR_error_check(TRUE);	//Remise à zéro de la vérif d'erreur
			state = SUPERVISOR_IDLE;
		break;
		case EVENT_NOTHING_TO_DO:	//no break
			SUPERVISOR_error_check(TRUE);	//Remise à zéro de la vérif d'erreur
			//Je reste dans l'état où je suis (soit ERROR, soit IDLE...)
		break;
		//NOUVEL ORDRE.
		case EVENT_NEW_ORDER:
			current_acknowledge = ack;
			SUPERVISOR_error_check(TRUE);	//Remise à zéro de la vérif d'erreur
			WARNER_inform(WARNING_NEW_TRAJECTORY, error_source);
			state = SUPERVISOR_TRAJECTORY;
		break;

		//REMONTEE D'INFORMATIONS A TRANSMETTRE.
		case EVENT_BRAKING:
			if(current_acknowledge == ACKNOWLEDGE_ASKED)
			{
				WARNER_inform(WARNING_BRAKE, error_source);
				current_acknowledge = BRAKE_ACKNOWLEDGED;
			}
		break;
		case EVENT_ARRIVED:
			if(current_acknowledge != NO_ACKNOWLEDGE && current_acknowledge != ACKNOWLEDGED)
			{
				if(current_acknowledge == ACKNOWLEDGE_CALIBRATION)
					WARNER_inform(WARNING_CALIBRATION_FINISHED, error_source);
				else
					WARNER_inform(WARNING_ARRIVED, error_source);
				current_acknowledge = ACKNOWLEDGED;
			}
		break;

		//ERREUR RENCONTREE
		case EVENT_ERROR:
			if(error_transmitted == FALSE)
			{  //Premier passage dans l'état d'erreur.
				COPILOT_init();
				PILOT_referential_init();
				PILOT_referential_reset();
				BUFFER_init();

				WARNER_inform(WARNING_ERROR, error_source);
				current_acknowledge = NO_ACKNOWLEDGE;
				error_transmitted = TRUE;
			}
			state = SUPERVISOR_ERROR;
		break;

		//FIN DE MATCH
		case EVENT_BROADCAST_STOP:
			state = SUPERVISOR_MATCH_ENDED;
		break;
		default:
		break;
	}

}

void SUPERVISOR_error_check_enable(bool_e enable)
{
	error_check_enable = enable;
}

void SUPERVISOR_number_of_rounds_returns_increment(void)
{
	number_of_rounds_returns++;
}


void SUPERVISOR_error_check(bool_e reset_error_check)
{
	static Uint16 immobility_counter = 0;

	if(reset_error_check)
	{
		number_of_rounds_returns = 0;
		error_transmitted = FALSE;
		error_source = NO_ERROR;
		immobility_counter = 0;
		return;
	}
	#ifdef SUPERVISOR_DISABLE_ERROR_DETECTION
		return;											//Les erreurs sont désactivées, 		donc on se casse !
	#endif

	if (state == SUPERVISOR_ERROR)				//On est déjà en erreur... 				donc on se casse !
		return;
	if(error_check_enable == FALSE)					 //La détection d'erreur est désactivée, donc on se casse !
		return;

	//	On regarde si on doit y aller...
	//ERREUR de point fictif trop loin... on semble bloqués.
	if (	(CORRECTOR_PD_enable_get_translation()  && absolute(global.ecart_translation)>TRESHOLD_ERROR_TRANSLATION ) ||
			 (CORRECTOR_PD_enable_get_rotation()  && absolute(global.ecart_rotation)>THRESHOLD_ERROR_ROTATION)	 )
	{
		error_source = UNABLE_TO_GO_ERROR;
		 SUPERVISOR_state_machine(EVENT_ERROR, 0);
	}
	//ERREUR DE POINT FICTIF PAS TROP LOIN, MAIS DE ROBOT QU'ARRIVE PAS !
	//Conditions : robot non arrivé, mais robot statique depuis trop longtemps
	if(
				!COPILOT_is_arrived()
				&& (absolute(global.real_speed_translation) < 4*PRECISION_ARRIVE_SPEED_TRANSLATION)
				&& (absolute(global.real_speed_rotation) < 4*PRECISION_ARRIVE_SPEED_ROTATION)
		)
		immobility_counter+=PERIODE_IT_ASSER;
	else
		immobility_counter = 0;

	if(immobility_counter > TRESHOLD_ERROR_IMMOBILITY_MAX_TIME)
	{
		immobility_counter = 0;
		error_source = IMMOBILITY_ERROR;
		SUPERVISOR_state_machine(EVENT_ERROR, 0);
	}

	if (number_of_rounds_returns > TRESHOLD_MAX_NUMBER_OF_ROUNDS_RETURNS)
	{
		error_source = ROUNDS_RETURNS_ERROR;
		SUPERVISOR_state_machine(EVENT_ERROR, 0);
	}
}



SUPERVISOR_state_e SUPERVISOR_get_state(void)
{
	return state;
}

