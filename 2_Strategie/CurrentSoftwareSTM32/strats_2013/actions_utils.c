/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_utils.c
 *	Package : Carte Stratégie
 *	Description : Offre des fonctions simplifiant le code de strategie
 *	Auteur : amurzeau
 *	Version 27 avril 2013
 */

#include "actions_utils.h"
#include "../act_functions.h"
#include "../output_log.h"
#include <stdarg.h>

Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state) {
	ACT_function_result_e result = ACT_get_last_action_result(act_queue_id);
	switch(result) {
		case ACT_FUNCTION_InProgress:
			return in_progress_state;

		case ACT_FUNCTION_Done:
			return success_state;

		case ACT_FUNCTION_ActDisabled:
		case ACT_FUNCTION_RetryLater:
			return failed_state;

		default:
			debug_printf("/!\\ check_act_status:%d: in default case, result = %d\n", __LINE__, result);
			return failed_state;
	}
}

Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state) {
	switch(sub_action) {
		case IN_PROGRESS:
			return in_progress_state;

		case END_OK:
			return success_state;

		case END_WITH_TIMEOUT:
		case FOE_IN_PATH:
		case NOT_HANDLED:
			return failed_state;

		default:
			debug_printf("/!\\ check_sub_action_result:%d: in default case, sub_action = %d\n", __LINE__, sub_action);
			return failed_state;
	}
}



Uint16 wait_hammer(Uint16 progress, Uint16 success, Uint16 fail)
{
	Uint16 ret = progress;
	switch(ACT_get_last_action_result(ACT_QUEUE_Hammer))
	{
		case ACT_FUNCTION_Done:
			ret = success;
		break;
		case ACT_FUNCTION_ActDisabled:
		case ACT_FUNCTION_RetryLater:
			ret = fail;
		break;
		case ACT_FUNCTION_InProgress:
		break;
		default:
		break;
	}
	return ret;
}


bool_e all_gifts_done(void)
{
	if( /*(global.env.map_elements[GOAL_Cadeau0] == ELEMENT_DONE) &&
		(global.env.map_elements[GOAL_Cadeau1] == ELEMENT_DONE) &&
		*/(global.env.map_elements[GOAL_Cadeau2] == ELEMENT_DONE) &&
		(global.env.map_elements[GOAL_Cadeau3] == ELEMENT_DONE)		)
		return TRUE;
	else
		return FALSE;
}

void UTILS_CAN_send_state_changed(Uint16 state_machine_id, Uint8 old_state, Uint8 new_state, Uint8 nb_params, ...) {
	va_list l;
	CAN_msg_t msg;
	Uint8 i;

	msg.sid = DEBUG_STRAT_STATE_CHANGED;
	msg.data[0] = HIGHINT(state_machine_id);
	msg.data[1] = LOWINT(state_machine_id);
	msg.data[2] = old_state;
	msg.data[3] = new_state;
	va_start(l, nb_params);

	if(nb_params > 4)
		nb_params = 4;

	for(i = 0; i < nb_params; i++)
		msg.data[4 + i] = va_arg(l, int);

	msg.size = i + 4;
	CAN_send(&msg);

	va_end(l);
}

void UTILS_LOG_state_changed(const char* sm_name, Uint16 sm_id, const char* old_state_name, Uint8 old_state_id, const char* new_state_name, Uint8 new_state_id) {
	UTILS_CAN_send_state_changed(sm_id, old_state_id, new_state_id, 0);
	OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, LOG_LEVEL_Info, "state change: %s(0x04%X): %s(%d) -> %s(%d)\n",
			sm_name, sm_id,
			old_state_name, old_state_id,
			new_state_name, new_state_id);
}
