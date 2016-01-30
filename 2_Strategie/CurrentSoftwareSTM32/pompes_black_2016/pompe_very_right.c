/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_very_right.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe VERY_RIGHT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#include "pompe_very_right.h"

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"


#define LOG_PREFIX "pompe_very_right : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE_VERY_RIGHT
#include "../QS/QS_outputlog.h"


void POMPE_VERY_RIGHT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
}


bool_e POMPE_VERY_RIGHT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POMPE_VERY_RIGHT) {
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_VERY_RIGHT_NORMAL:
			case ACT_POMPE_VERY_RIGHT_STOP:
				POMPE_VERY_RIGHT_command(msg->data.act_msg.order);
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
void POMPE_VERY_RIGHT_command(ACT_order_e command) {

	switch(command) {
		case ACT_POMPE_VERY_RIGHT_NORMAL:
			GPIO_SetBits(POMPE_VERY_RIGHT_PIN);
			break;

		case ACT_POMPE_VERY_RIGHT_STOP:
			GPIO_ResetBits(POMPE_VERY_RIGHT_PIN);
			return;

		default: {
			error_printf("Invalid POMPE_VERY_RIGHT command: %u, code is broken !\n", command);
			return;
		}
	}
}


