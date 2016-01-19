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

// VERY_LEFT d'une pompe

// Ajout l'actionneur dans QS_CANmsgList.h
// Ajout d'une valeur dans l'énumération de la queue dans config_(big/small)/config_global_vars_types.h
// Formatage : QUEUE_ACT_POMPE_VERY_LEFT
// Ajout de la déclaration de l'actionneur dans ActManager dans le tableau actionneurs
// Ajout de la verbosité dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// Un define VERY_LEFT_PIN doit avoir été ajouté au fichier config_big/config_pin.h // config_small/config_pin.h
// Ajout des postions dans QS_types.h dans l'énum ACT_order_e (avec "ACT_" et sans "_POS" à la fin)
// Mise à jour de config/config_debug.h

// Optionnel:
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'énumération SELFTEST)

// En stratégie
// ajout d'une d'une valeur dans le tableau act_link_SID_Queue du fichier act_functions.c/h
// ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'évitement du robot

// En stratégie Optionnel
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosité dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)

#if 1

#define POMPE_VERY_LEFT_PIN 0,0

//#ifdef

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

//#endif

#endif

