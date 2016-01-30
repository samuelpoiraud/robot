/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_very_left.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe VERY_LEFT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#include "pompe_very_left.h"

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"


#define LOG_PREFIX "pompe_very_left : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE_VERY_LEFT
#include "../QS/QS_outputlog.h"


void POMPE_VERY_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
}


bool_e POMPE_VERY_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POMPE_VERY_LEFT) {
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_VERY_LEFT_NORMAL:
			case ACT_POMPE_VERY_LEFT_STOP:
				POMPE_VERY_LEFT_command(msg->data.act_msg.order);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {

	}
	return TRUE;
}

//Initialise une commande
void POMPE_VERY_LEFT_command(ACT_order_e command) {

	switch(command) {
		case ACT_POMPE_VERY_LEFT_NORMAL:
			GPIO_SetBits(POMPE_VERY_LEFT_PIN);
			break;

		case ACT_POMPE_VERY_LEFT_STOP:
			GPIO_ResetBits(POMPE_VERY_LEFT_PIN);
			return;

		default: {
			error_printf("Invalid POMPE_VERY_LEFT command: %u, code is broken !\n", command);
			return;
		}
	}
}


