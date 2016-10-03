/*
 * gohome.c
 *
 *  Created on: 12 oct. 2013
 *      Author: Arnaud
 */
#include "gohome.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"
#include "../../../QS/QS_who_am_i.h"
#include "../../../propulsion/movement.h"
#include "../../../propulsion/prop_functions.h"

const char term_cmd_gohome_brief[] = "Déplace le robot à sa position de départ";
const char term_cmd_gohome_help[] =
		"Utilisation : gohome\n";


int term_cmd_gohome(int argc, const char *argv[]) {

	if(argc > 0)
		return EINVAL;

	/*if(QS_WHO_AM_I_get()==SMALL_ROBOT){
		if(global.color == BOT_COLOR){
			PROP_push_goto(960, COLOR_Y(73), FAST, BACKWARD, 0, AVOID_DISABLED, END_AT_BRAKE, PROP_NO_BORDER_MODE, FALSE);
			PROP_push_goto(960, COLOR_Y(300), FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BRAKE, PROP_NO_BORDER_MODE, TRUE);
		}else{
			PROP_push_goto(250, COLOR_Y(73), FAST, BACKWARD, 0, AVOID_DISABLED, END_AT_BRAKE, PROP_NO_BORDER_MODE, FALSE);
			PROP_push_goto(250, COLOR_Y(300), FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BRAKE, PROP_NO_BORDER_MODE, TRUE);
		}
	}else if(QS_WHO_AM_I_get()==BIG_ROBOT){
		PROP_push_goto(603, COLOR_Y(132), FAST, BACKWARD, 0, AVOID_DISABLED, END_AT_BRAKE, PROP_NO_BORDER_MODE, FALSE);
		PROP_push_goto(603, COLOR_Y(300), FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BRAKE, PROP_NO_BORDER_MODE, TRUE);
	}*/
	return 0;
}




