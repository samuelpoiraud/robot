/*
 * calibration.c
 *
 *  Created on: 31 mars 2014
 *      Author: Arnaud
 */
#include "calibration.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"
#include "../../../QS/QS_CANmsgList.h"
#include "../../../QS/QS_can.h"

const char term_cmd_calibration_brief[] = "D�clenche la calibration via un message CAN � la carte propulsion";
const char term_cmd_calibration_help[] =
		"Utilisation : calibration\n";


int term_cmd_calibration(int argc, const char *argv[]) {

	if(argc > 0)
		return EINVAL;

	if(!global.prop.calibrated)
	{
		CAN_msg_t msg;
		msg.sid = PROP_CALIBRATION;
		msg.size = 0;
		CAN_send(&msg);
	}

	return 0;
}




