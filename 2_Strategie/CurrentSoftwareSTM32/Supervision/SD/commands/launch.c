/*
 * launch.c
 *
 *  Created on: 14 oct. 2013
 *      Author: Arnaud
 */

#include "launch.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"

const char term_cmd_launch_brief[] = "Lancer le match";
const char term_cmd_launch_help[] =
		"Utilisation : auncun argument\n";


int term_cmd_launch(int argc, const char *argv[]) {

	if(argc > 0)
		return EINVAL;

	global.env.match_started = 1;

	return 0;
}












