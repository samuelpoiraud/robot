/*
 * launch.c
 *
 *  Created on: 14 oct. 2013
 *      Author: Arnaud
 */

#include "stop.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"
#include "../../../QS/QS_CANmsgList.h"
#include "../../../QS/QS_can.h"
#include "../../../actuator/queue.h"

const char term_cmd_stop_brief[] = "Stop tout";
const char term_cmd_stop_help[] =
		"Utilisation : auncun argument\n";


int term_cmd_stop(int argc, const char *argv[]) {

	if(argc > 0)
		return EINVAL;

	CAN_send_sid(BROADCAST_STOP_ALL);
	global.flags.match_over = TRUE;

	return 0;
}












