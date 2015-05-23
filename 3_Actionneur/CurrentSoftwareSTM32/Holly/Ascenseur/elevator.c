/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : elevator.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'ascenseur de l'actionneur spot
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */


#include "elevator.h"

#ifdef I_AM_ROBOT_BIG

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_qei.h"
#include "../QS/QS_DCMotor2.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "elevator_config.h"

#define LOG_PREFIX "elevator : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ELEVATOR
#include "../QS/QS_outputlog.h"

static volatile bool_e ELEVATOR_ready = FALSE;
static volatile bool_e encoder_ready = FALSE;
static volatile Sint16 ELEVATOR_position = 0;

static void ELEVATOR_command_run(queue_id_t queueId);
static void ELEVATOR_command_init(queue_id_t queueId);

// Fonction appell�e au lancement de la carte (via ActManager)
void ELEVATOR_init() {
	static bool_e initialized = FALSE;
	DCMotor_config_t dcconfig;

	if(initialized)
		return;
	initialized = TRUE;

	PWM_init();
	DCM_init();
	QEI_init();
	dcconfig.sensor_read = &ELEVATOR_get_position;
	dcconfig.double_PID = FALSE;
	dcconfig.Kp = ELEVATOR_KP;
	dcconfig.Ki = ELEVATOR_KI;
	dcconfig.Kd = ELEVATOR_KD;
	dcconfig.Kp2 = ELEVATOR_KP2;
	dcconfig.Ki2 = ELEVATOR_KI2;
	dcconfig.Kd2 = ELEVATOR_KD2;
	dcconfig.pos[0] = 0;
	dcconfig.speed[0] = 0;
	dcconfig.pwm_number = ELEVATOR_PWM_NUM;
	dcconfig.way_latch = ELEVATOR_PORT_WAY;
	dcconfig.way_bit_number = ELEVATOR_PORT_WAY_BIT;
	dcconfig.way0_max_duty = ELEVATOR_MAX_PWM_WAY0;
	dcconfig.way1_max_duty = ELEVATOR_MAX_PWM_WAY1;
	dcconfig.timeout = ELEVATOR_ASSER_TIMEOUT;
	dcconfig.epsilon = ELEVATOR_ASSER_POS_EPSILON;
	dcconfig.large_epsilon = ELEVATOR_ASSER_POS_LARGE_EPSILON;
	dcconfig.inverseDirection = FALSE;
	dcconfig.stop_on_idle = FALSE;
	DCM_config(ELEVATOR_ID, &dcconfig);
	DCM_stop(ELEVATOR_ID);
}

void ELEVATOR_state_machine(){
	typedef enum{
		INIT,
		WAIT_FDC,
		INIT_POS,
		WAIT_POS,
		RUN,
		DEACTIVATE
	}state_e;
	static state_e state = INIT, last_state = -1;
	static time32_t begin_detection = 0, begin_wait = 0;
	static bool_e last_alim = FALSE;
	Uint8 result, error_code;
	Uint16 line;
	bool_e entrance = (state != last_state)? TRUE : FALSE;
	last_state = state;

	if(entrance)
		debug_printf("%d -> %d\n", last_state, state);


	switch (state) {
		case INIT:
			ELEVATOR_init();
			DCM_setWayDirection(ELEVATOR_ID, TRUE);
			GPIO_ResetBits(ELEVATOR_DCM_SENS);
			PWM_run(30, ELEVATOR_PWM_NUM);
			state = WAIT_FDC;
			break;

		case WAIT_FDC:
			if(entrance){
				begin_detection = 0;
			}

			if(!last_alim && global.alim)
					begin_wait = global.absolute_time;
			last_alim = global.alim;

			if(ELEVATOR_FDC && begin_detection == 0)
				begin_detection = global.absolute_time;

			if(ELEVATOR_FDC && begin_detection != 0 && global.absolute_time - begin_detection > 500){
				PWM_run(0, ELEVATOR_PWM_NUM);
				QEI1_set_count(0);
				encoder_ready = TRUE;
				state = INIT_POS;
			}else if(global.alim && global.absolute_time - begin_wait > ELEVATOR_ASSER_TIMEOUT+1000)
				state = DEACTIVATE;

			if(!ELEVATOR_FDC)
				begin_detection = 0;
			break;

		case INIT_POS:
			DCM_setPosValue(ELEVATOR_ID, 0, ACT_ELEVATOR_INIT_POS, ACT_ELEVATOR_SPEED);
			DCM_goToPos(ELEVATOR_ID, 0);
			DCM_restart(ELEVATOR_ID);
			state = WAIT_POS;
			break;

		case WAIT_POS:
			if(ACTQ_check_status_dcmotor(ELEVATOR_ID, FALSE, &result, &error_code, &line)){
				if(result == ACT_RESULT_DONE){
					ELEVATOR_ready = TRUE;
					debug_printf("Win !\n");
					state = RUN;
				}else{
					DCM_stop(ELEVATOR_ID);

					debug_printf("Fail !\n");
					state = INIT;
				}
			}
			break;

		case RUN:
			break;

		case DEACTIVATE:
			if(entrance)
				PWM_run(0, ELEVATOR_PWM_NUM);

			break;
	}
}

void ELEVATOR_process_it(){
	if(encoder_ready){
		static Sint16 count_prec = 0;
		Sint16 count = QEI1_get_count();
		Sint32 delta = count - count_prec;

		if(delta > 512)
			delta -= 65536;
		else if(delta < -512)
			delta += 65536;

		count_prec = count;

		ELEVATOR_position += delta * ELEVATOR_QEI_COEF;
	}
}

Sint16 ELEVATOR_get_position(){
	return (Sint16)(-ELEVATOR_position*1.5628 + 8000);

}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void ELEVATOR_reset_config(){}

// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void ELEVATOR_config(CAN_msg_t* msg){
	switch(msg->data[1]){
		case 0:
			if(msg->data[2] != 0)
				DCM_setPwmWay(ELEVATOR_ID, msg->data[2], msg->data[2]);
			else
				DCM_setPwmWay(ELEVATOR_ID, ELEVATOR_MAX_PWM_WAY0, ELEVATOR_MAX_PWM_WAY1);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
	}
}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
/// Ignor� ici mise en position fait par le process_main
void ELEVATOR_init_pos(){}

// Fonction appell�e � la fin du match (via ActManager)
void ELEVATOR_stop(){
	DCM_stop(ELEVATOR_ID);
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e ELEVATOR_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_ELEVATOR){
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
            case ACT_ELEVATOR_BOT :
            case ACT_ELEVATOR_MID :
            case ACT_ELEVATOR_MID_LOW :
            case ACT_ELEVATOR_ESTRAD :
			case ACT_ELEVATOR_PRE_TOP :
			case ACT_ELEVATOR_TOP :
			case ACT_ELEVATOR_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_ELEVATOR, &ELEVATOR_run_command, 0, TRUE);
				break;

			case ACT_CONFIG :
				ELEVATOR_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les diff�rents �tats que l'actionneur doit r�aliser pour r�ussir le selftest
		SELFTEST_set_actions(&ELEVATOR_run_command, 0, 3, (SELFTEST_action_t[]){
								 {ACT_ELEVATOR_BOT,		0,  QUEUE_ACT_ELEVATOR},
								 {ACT_ELEVATOR_TOP,		0,  QUEUE_ACT_ELEVATOR},
								 {ACT_ELEVATOR_MID,		0,  QUEUE_ACT_ELEVATOR}
							 });
	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void ELEVATOR_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_ELEVATOR) {    // Gestion des mouvements de ELEVATOR
		if(init)
			ELEVATOR_command_init(queueId);
		else
			ELEVATOR_command_run(queueId);
	}
}

//Initialise une commande
static void ELEVATOR_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* dcm_goalPosition = &QUEUE_get_arg(queueId)->param;

	*dcm_goalPosition = 0x8000;

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
        case ACT_ELEVATOR_BOT :		*dcm_goalPosition = ACT_ELEVATOR_BOT_POS; break;
        case ACT_ELEVATOR_MID_LOW :	*dcm_goalPosition = ACT_ELEVATOR_MID_LOW_POS; break;
        case ACT_ELEVATOR_MID :		*dcm_goalPosition = ACT_ELEVATOR_MID_POS; break;
		case ACT_ELEVATOR_PRE_TOP : *dcm_goalPosition = ACT_ELEVATOR_PRE_TOP_POS; break;
        case ACT_ELEVATOR_TOP :		*dcm_goalPosition = ACT_ELEVATOR_TOP_POS; break;
        case ACT_ELEVATOR_ESTRAD :  *dcm_goalPosition = ACT_ELEVATOR_ESTRAD_POS; break;

		case ACT_ELEVATOR_STOP :
			DCM_stop(ELEVATOR_ID);
			QUEUE_next(queueId, ACT_ELEVATOR, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid exemple command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_ELEVATOR, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ELEVATOR_ready == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
		QUEUE_next(queueId, ACT_ELEVATOR, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*dcm_goalPosition == 0x8000) {
		error_printf("Invalid dcm position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_ELEVATOR, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	DCM_setPosValue(ELEVATOR_ID, 0, *dcm_goalPosition, ACT_ELEVATOR_SPEED);
	DCM_goToPos(ELEVATOR_ID, 0);
	DCM_restart(ELEVATOR_ID);
	debug_printf("Placement en position %d du moteur DC lanc�\n", *dcm_goalPosition);
}

//G�re les �tats pendant le mouvement du moteur DC
static void ELEVATOR_command_run(queue_id_t queueId) {
	Uint8 result, error_code;
	Uint16 line;

	if(ACTQ_check_status_dcmotor(ELEVATOR_ID, FALSE, &result, &error_code, &line)){
		debug_printf("Elevator pos : %d\n", ELEVATOR_get_position());
		QUEUE_next(queueId, ACT_ELEVATOR, result, error_code, line);
	}
}

#endif
