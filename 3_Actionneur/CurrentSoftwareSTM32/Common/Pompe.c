/*  Club Robot ESEO 2013 - 2014
 *
 *	Fichier : pompe.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : Les deux
 */

#include "Pompe.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "config_pin.h"
#include "Pompe_config.h"

#include "config_debug.h"
#define LOG_PREFIX "Pompe : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE
#include "../QS/QS_outputlog.h"


#define NB_MESURES_COURANT 3

static void POMPE_initDCM();
static void POMPE_command_init(queue_id_t queueId);
static void POMPE_command_run(queue_id_t queueId);
static void POMPE_do_order(Uint8 command, Uint8 param);
static void measures_calcul_send_msg(Sint16 tab_mesures[NB_MESURES_COURANT]);

static bool_e measuring_current = FALSE;
static Uint8 actual_PWM = 0;

void POMPE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	POMPE_initDCM();
}

// Initialisation du moteur, si init ne fait rien
static void POMPE_initDCM() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	PORTS_pwm_init();
	PWM_init();
	PWM_stop(POMPE_PWM_NUM);
	actual_PWM = 0;

	info_printf("Pompe initialisée (DCM) \n");
}

void POMPE_stop() {
	PWM_stop(POMPE_PWM_NUM);
	actual_PWM = 0;
}

void POMPE_init_pos(){
	PWM_stop(POMPE_PWM_NUM);
	actual_PWM = 0;
}

bool_e POMPE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POMPE) {
		POMPE_initDCM();
		switch(msg->data[0]) {
			case ACT_POMPE_NORMAL:
			case ACT_POMPE_REVERSE:
			case ACT_POMPE_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Pompe, &POMPE_run_command, msg->data[1]);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&POMPE_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_POMPE_NORMAL,  100, QUEUE_ACT_Pompe},
								 {ACT_POMPE_REVERSE, 100, QUEUE_ACT_Pompe},
								 {ACT_POMPE_STOP, 0, QUEUE_ACT_Pompe}
							 });
	}

	return FALSE;
}

void POMPE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Pompe) {
			if(init)
				POMPE_command_init(queueId);
			else
				POMPE_command_run(queueId);
	}
}

//Initialise une commande
static void POMPE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 param = QUEUE_get_arg(queueId)->param;

	switch(command) {
		case ACT_POMPE_NORMAL:
		case ACT_POMPE_REVERSE:
			POMPE_do_order(command, param);
			break;

		case ACT_POMPE_STOP:
			PWM_stop(POMPE_PWM_NUM);
			actual_PWM = 0;
			return;

		default: {
			error_printf("Invalid pompe command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_POMPE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void POMPE_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_POMPE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void POMPE_do_order(Uint8 command, Uint8 param){
	if(command == ACT_POMPE_NORMAL)
		POMPE_SENS = 0;
	else if(command == ACT_POMPE_REVERSE)
		POMPE_SENS = 1;
	else{
		debug_printf("commande envoyée à POMPE_do_order inconnue -> %d	%x\n", command, command);
		PWM_stop(POMPE_PWM_NUM);
		actual_PWM = 0;
		return;
	}

	param = (param > 100) ? 100 : param;
	PWM_run(param, POMPE_PWM_NUM);
	actual_PWM = param;
}

void measure_current_pump(){
	static Uint8 nb_mesures;
	static Sint16 tab_mesures[NB_MESURES_COURANT];

	if(measuring_current == FALSE){
		nb_mesures = 0;
		ADC_init();
		measuring_current = TRUE;
	}

	tab_mesures[nb_mesures] = ADC_getValue(ADC_POMPE);
	debug_printf("Mesure courant pompe n° %d : %d\n", nb_mesures, tab_mesures[nb_mesures]);
	nb_mesures ++;
	if(nb_mesures < NB_MESURES_COURANT)
		if(WATCHDOG_create(5, measure_current_pump, FALSE) == 0xFF){
			debug_printf("Création du watchdog pour les mesures de courant impossible\n");
			measuring_current = FALSE;
		}

	if(nb_mesures == NB_MESURES_COURANT){
		measuring_current = FALSE;
		measures_calcul_send_msg(tab_mesures);
	}
}

static void measures_calcul_send_msg(Sint16 tab_mesures[NB_MESURES_COURANT]){
	Sint16 moyenne = 0;
	Uint8 i;
	debug_printf("Mesure de courant de la pompe :\n");
	for(i=0; i<NB_MESURES_COURANT; i++){
		moyenne += tab_mesures[i];
		debug_printf("Valeur : %d \n", tab_mesures[i]);
	}
	moyenne /= NB_MESURES_COURANT;
	debug_printf("Moyenne : %d \n", moyenne);
	debug_printf("Seuil pour la PWM actuel : %d\n", 2*actual_PWM*100);

	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = STRAT_ANSWER_POMPE;

	if(moyenne>2*actual_PWM*100) // Formule à étalonnée avec la pompe...
		msg.data[0] = STRAT_ANSWER_POMPE_YES;
	else
		msg.data[0] = STRAT_ANSWER_POMPE_NO;

	CAN_send(&msg);
}
