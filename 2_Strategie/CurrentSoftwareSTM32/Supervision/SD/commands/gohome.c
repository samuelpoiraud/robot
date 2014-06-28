/*
 * gohome.c
 *
 *  Created on: 12 oct. 2013
 *      Author: Arnaud
 */
#include "gohome.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../prop_functions.h"
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
			PROP_push_goto(960, COLOR_Y(73), FAST, BACKWARD, 0, END_AT_BREAK, FALSE);
			PROP_push_goto(960, COLOR_Y(300), FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);
		}else{
			PROP_push_goto(250, COLOR_Y(73), FAST, BACKWARD, 0, END_AT_BREAK, FALSE);
			PROP_push_goto(250, COLOR_Y(300), FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);
		}
	}else if(QS_WHO_AM_I_get()==KRUSTY){
		PROP_push_goto(603, COLOR_Y(132), FAST, BACKWARD, 0, END_AT_BREAK, FALSE);
		PROP_push_goto(603, COLOR_Y(300), FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);
	}
	return 0;
}




