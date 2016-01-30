/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_middle_left.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe MIDDLE_LEFT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#include "pompe_middle_left.h"

#define POMPE_MIDDLE_LEFT_PIN 0,0

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"


#define LOG_PREFIX "pompe_middle_left : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE_MIDDLE_LEFT
#include "../QS/QS_outputlog.h"


void POMPE_MIDDLE_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
}


bool_e POMPE_MIDDLE_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POMPE_MIDDLE_LEFT) {
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_MIDDLE_LEFT_NORMAL:
			case ACT_POMPE_MIDDLE_LEFT_STOP:
				POMPE_MIDDLE_LEFT_command(msg->data.act_msg.order);
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
void POMPE_MIDDLE_LEFT_command(ACT_order_e command) {

	switch(command) {
		case ACT_POMPE_MIDDLE_LEFT_NORMAL:
			GPIO_SetBits(POMPE_MIDDLE_LEFT_PIN);
			break;

		case ACT_POMPE_MIDDLE_LEFT_STOP:
			GPIO_ResetBits(POMPE_MIDDLE_LEFT_PIN);
			return;

		default: {
			error_printf("Invalid POMPE_MIDDLE_LEFT command: %u, code is broken !\n", command);
			return;
		}
	}
}


