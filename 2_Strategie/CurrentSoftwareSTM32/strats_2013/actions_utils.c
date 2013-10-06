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
#include <stdarg.h>
#include "../act_functions.h"

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
