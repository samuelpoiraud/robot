/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : Pop_drop_left.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la depose des popcorns gauche
 *  Auteur : Anthony
 *  Version 20130219
 *  Robot : HOLLY
 */
#include "Pop_drop_left.h"

// Reste
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'�num�ration SELFEST)

#ifdef I_AM_ROBOT_BIG

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "Pop_drop_left_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "Pop_drop_left.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POP_DROP_LEFT
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void POP_DROP_LEFT_command_run(queue_id_t queueId);
static void POP_DROP_LEFT_initAX12();
static void POP_DROP_LEFT_command_init(queue_id_t queueId);
static void POP_DROP_LEFT_config(CAN_msg_t* msg);

// Bool�en contenant l'�tat actuel de l'initialisation de l'AX12 (Plusieurs bool�ens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appell�e au lancement de la carte (via ActManager)
void POP_DROP_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	POP_DROP_LEFT_initAX12();
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void POP_DROP_LEFT_reset_config(){
	ax12_is_initialized = FALSE;
	POP_DROP_LEFT_initAX12();
}

//Initialise l'AX12 s'il n'�tait pas aliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void POP_DROP_LEFT_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(POP_DROP_LEFT_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_lowest_voltage(POP_DROP_LEFT_AX12_ID, AX12_MIN_VOLTAGE);
		AX12_config_set_highest_voltage(POP_DROP_LEFT_AX12_ID, AX12_MAX_VOLTAGE);
		AX12_set_torque_limit(POP_DROP_LEFT_AX12_ID, POP_DROP_LEFT_AX12_MAX_TORQUE_PERCENT);
		AX12_config_set_temperature_limit(POP_DROP_LEFT_AX12_ID, POP_DROP_LEFT_AX12_MAX_TEMPERATURE);

		AX12_config_set_maximal_angle(POP_DROP_LEFT_AX12_ID, POP_DROP_LEFT_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(POP_DROP_LEFT_AX12_ID, POP_DROP_LEFT_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(POP_DROP_LEFT_AX12_ID, AX12_BEFORE_LED);
		AX12_config_set_error_before_shutdown(POP_DROP_LEFT_AX12_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void POP_DROP_LEFT_config(CAN_msg_t* msg){
	switch(msg->data[1]){
		case 0 : // Premier �lement de l'actionneur
			ACTMGR_config_AX12(POP_DROP_LEFT_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
	}
}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
void POP_DROP_LEFT_init_pos(){
	POP_DROP_LEFT_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!AX12_set_position(POP_DROP_LEFT_AX12_ID, POP_DROP_LEFT_AX12_INIT_POS))
		debug_printf("   L'AX12 n�%d n'est pas l�\n", POP_DROP_LEFT_AX12_ID);
	else
		debug_printf("   L'AX12 n�%d a �t� initialis� en position\n", POP_DROP_LEFT_AX12_ID);
}

// Fonction appell�e � la fin du match (via ActManager)
void POP_DROP_LEFT_stop(){
	AX12_set_torque_enabled(POP_DROP_LEFT_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e POP_DROP_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POP_DROP_LEFT){
		POP_DROP_LEFT_initAX12();
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_POP_DROP_LEFT_CLOSED :
			case ACT_POP_DROP_LEFT_OPEN :
			case ACT_POP_DROP_LEFT_MID :
			case ACT_POP_DROP_LEFT_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_POP_DROP_LEFT, &POP_DROP_LEFT_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				POP_DROP_LEFT_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les diff�rents �tats que l'actionneur doit r�aliser pour r�ussir le selftest
		SELFTEST_set_actions(&POP_DROP_LEFT_run_command, 9, 3, (SELFTEST_action_t[]){
								 {ACT_POP_DROP_LEFT_CLOSED,		0,  QUEUE_ACT_AX12_POP_DROP_LEFT},
								 {ACT_POP_DROP_LEFT_OPEN,		0,  QUEUE_ACT_AX12_POP_DROP_LEFT},
								 {ACT_POP_DROP_LEFT_CLOSED,		0,  QUEUE_ACT_AX12_POP_DROP_LEFT}
							 });
	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void POP_DROP_LEFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_POP_DROP_LEFT) {    // Gestion des mouvements de POP_DROP_LEFT
		if(init)
			POP_DROP_LEFT_command_init(queueId);
		else
			POP_DROP_LEFT_command_run(queueId);
	}
}

//Initialise une commande
static void POP_DROP_LEFT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	POP_DROP_LEFT_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
		case ACT_POP_DROP_LEFT_CLOSED : *ax12_goalPosition = POP_DROP_LEFT_AX12_IDLE_POS; break;
		case ACT_POP_DROP_LEFT_MID : *ax12_goalPosition = POP_DROP_LEFT_AX12_MID_POS; break;
		case ACT_POP_DROP_LEFT_OPEN : *ax12_goalPosition = POP_DROP_LEFT_AX12_DEPLOYED_POS; break;

		case ACT_POP_DROP_LEFT_STOP :
			AX12_set_torque_enabled(POP_DROP_LEFT_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_POP_DROP_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid exemple command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_POP_DROP_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
		QUEUE_next(queueId, ACT_POP_DROP_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_POP_DROP_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(POP_DROP_LEFT_AX12_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
	if(!AX12_set_position(POP_DROP_LEFT_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que l'AX12 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(POP_DROP_LEFT_AX12_ID).error);
		QUEUE_next(queueId, ACT_POP_DROP_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a �t� envoy�e et l'AX12 l'a bien re�u
	debug_printf("Move POP_Drop_Left ax12 to %d\n", *ax12_goalPosition);
}

//G�re les �tats pendant le mouvement de l'AX12
static void POP_DROP_LEFT_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, POP_DROP_LEFT_AX12_ID, QUEUE_get_arg(queueId)->param, POP_DROP_LEFT_AX12_ASSER_POS_EPSILON, POP_DROP_LEFT_AX12_ASSER_TIMEOUT, POP_DROP_LEFT_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_POP_DROP_LEFT, result, errorCode, line);
}

#endif
