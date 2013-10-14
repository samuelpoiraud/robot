/*
 * gohome.c
 *
 *  Created on: 12 oct. 2013
 *      Author: Arnaud
 */
#include "gohome.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../asser_functions.h"
#include "../../../QS/QS_all.h"
#include "../../../QS/QS_who_am_i.h"
#include "../../../avoidance.h"

const char term_cmd_gohome_brief[] = "Déplace le robot à sa position de départ";
const char term_cmd_gohome_help[] =
		"Utilisation : gohome\n";


int term_cmd_gohome(int argc, const char *argv[]) {

	if(argc > 0)
		return EINVAL;

	if(QS_WHO_AM_I_get()==TINY){
		if(global.env.color == RED){
			ASSER_push_goto(250, COLOR_Y(72), FAST, BACKWARD, 0, END_AT_BREAK, FALSE);
			ASSER_push_goto(250, COLOR_Y(200), FAST, BACKWARD, 0, END_AT_BREAK, TRUE);
		}else{
			ASSER_push_goto(250, COLOR_Y(72), FAST, FORWARD, 0, END_AT_BREAK, FALSE);
			ASSER_push_goto(250, COLOR_Y(200), FAST, FORWARD, 0, END_AT_BREAK, TRUE);
		}

	}else if(QS_WHO_AM_I_get()==KRUSTY){
		ASSER_push_goto(1000, COLOR_Y(120), FAST, BACKWARD, 0, END_AT_BREAK, FALSE);
		ASSER_push_goto(1000, COLOR_Y(300), FAST, BACKWARD, 0, END_AT_BREAK, TRUE);
	}

	return 0;
}




