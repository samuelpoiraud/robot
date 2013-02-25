/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Long_launcher.c
 *	Package : Carte actionneur
 *	Description : Asservissement du lanceur de balle
 *  Auteur : Alexis
 *  Version 20130208
 *  Robot : Krusty
 */

#include "KBall_launcher.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_timer.h"
#include "../output_log.h"

#define LOG_PREFIX "BL: "

//Define pour récuperer les valeurs du timer d'asservissement et sa période
#if DCM_TIMER == 1
	#define DCM_TIMER_VAL_REG    TMR1
	#define DCM_TIMER_PERIOD_REG PR1
	#define DCM_TIMER_PRIORITY_REG IPC0bits.T1IP
	#define DCM_TIMER_OVERFLOW_REG IFS0bits.T1IF
#elif DCM_TIMER == 2
	#define DCM_TIMER_VAL_REG    TMR2
	#define DCM_TIMER_PERIOD_REG PR2
	#define DCM_TIMER_PRIORITY_REG IPC1bits.T2IP
	#define DCM_TIMER_OVERFLOW_REG IFS0bits.T2IF
#elif DCM_TIMER == 3
	#define DCM_TIMER_VAL_REG    TMR3
	#define DCM_TIMER_PERIOD_REG PR3
	#define DCM_TIMER_PRIORITY_REG IPC1bits.T3IP
	#define DCM_TIMER_OVERFLOW_REG IFS0bits.T3IF
#elif DCM_TIMER == 4
	#define DCM_TIMER_VAL_REG    TMR4
	#define DCM_TIMER_PERIOD_REG PR4
	#define DCM_TIMER_PRIORITY_REG IPC5bits.T4IP
	#define DCM_TIMER_OVERFLOW_REG IFS1bits.T4IF
#else
	#error "DCM_TIMER doit etre 1 2 3 ou 4"
#endif /* DCM_TIMER == n */

static DCMotor_config_t ball_launcher_config;

/*
 * Technique de captage de la vitesse:
 * A chaque passage de l'aimant, on récupère la valeur du timer de l'asservissement.
 * A chaque overflow du timer d'asservissement, on incrémente timer_overflow_number.
 * A chaque detection d'aimant, on met dans last_detection_interval, le nombre de cycle du timer d'asservissement:
 *  cycles = timer_overflow_number * periode_timer + valeur_timer_passage_1 - valeur_timer_passage_2
 * last_detection_interval est en fait cycles
 * Si le temps entre 2 passage d'aimant dépasse 850ms, on considère que le moteur est arreté.
 */

static Sint16 BALLLAUNCHER_get_speed();

void BALLLAUNCHER_init() {
	ball_launcher_config.sensor_read = &BALLLAUNCHER_get_speed;
	ball_launcher_config.Kp = BALLLAUNCHER_ASSER_KP;
	ball_launcher_config.Ki = BALLLAUNCHER_ASSER_KI;
	ball_launcher_config.Kd = BALLLAUNCHER_ASSER_KD;
	ball_launcher_config.pos[0] = 0;
	ball_launcher_config.pos[1] = BALLLAUNCHER_TARGET_SPEED;	//en tr/min
	ball_launcher_config.pwm_number = BALLLAUNCHER_DCMOTOR_PWM_NUM;
	ball_launcher_config.way_latch = (Uint16*)&BALLLAUNCHER_DCMOTOR_PORT_WAY;
	ball_launcher_config.way_bit_number = BALLLAUNCHER_DCMOTOR_PORT_WAY_BIT;
	ball_launcher_config.way0_max_duty = BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY0;
	ball_launcher_config.way1_max_duty = BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY1;
	ball_launcher_config.timeout = 2000;
	ball_launcher_config.epsilon = 500;	//TODO: à ajuster plus correctement
	DCM_config(BALLLAUNCHER_DCMOTOR_ID, &ball_launcher_config);

	BALLLAUNCHER_HALLSENSOR_INT_PRIORITY = 5;
	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
	BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 1;

	//Si la priorité de l'interruption INTx (qui est déclanchée lors d'un passage d'un aimant devant le capteur) est de priorité supérieure à celle du timer gérant l'asservissement,
	//il est possible d'avoir une interruption INTx entre l'overflow du timer (donc il recommence a compter a partir de 0) et l'actualisation du nombre d'overflow dans la variable timer_overflow_number.
	if(BALLLAUNCHER_HALLSENSOR_INT_PRIORITY <= DCM_TIMER_PRIORITY_REG)
		OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Attention ! La priorité de l'interruption INTx doit être supérieur à celle de l'asservissement ! (TIMERx) (et si le code marche, qui l'a apporté à lourdes ?)\n");
}

void BALLLAUNCHER_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;
	if(msg->sid == ACT_BALLLAUNCHER) {
		switch(msg->data[0]) {
			case ACT_BALLLAUNCHER_ACTIVATE:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {	//Si on est pas en verbose_mode, l'assert sera ignoré et la suite risque de vraiment planter ...
					 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_BallLauncher);
					 QUEUE_add(queueId, &BALLLAUNCHER_run_command, msg->data[1] | (msg->data[2] << 8), QUEUE_ACT_BallLauncher);
					 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_BallLauncher);
				} else {	//on indique qu'on a pas géré la commande
					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLLAUNCHER & 0xFF, ACT_BALLLAUNCHER_ACTIVATE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
					CAN_send(&resultMsg);
				}
				break;

			case ACT_BALLLAUNCHER_STOP:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_BallLauncher);
					QUEUE_add(queueId, &BALLLAUNCHER_run_command, 0, QUEUE_ACT_BallLauncher);
					QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_BallLauncher);
				} else {	//on indique qu'on a pas géré la commande
					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLLAUNCHER & 0xFF, ACT_BALLLAUNCHER_STOP, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
					CAN_send(&resultMsg);
				}
				break;

			default:
				OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"invalid CAN msg data[0]=%hhu !\n", msg->data[0]);
		}
	}
}

void BALLLAUNCHER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_BallLauncher) {
		if(init == TRUE) {
			//Send command
			Sint16 pos_speed = QUEUE_get_arg(queueId);
			if(pos_speed == 0)
				//DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 0);
				DCM_stop(BALLLAUNCHER_DCMOTOR_ID);	//On est sur de l'arreter comme ça, même en cas de problème capteur
			else if(pos_speed > 0) {
				DCM_setPosValue(BALLLAUNCHER_DCMOTOR_ID, 1, pos_speed);
				DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 1);
				DCM_restart(BALLLAUNCHER_DCMOTOR_ID); //Redémarrage si on l'avait arrêté avec DCM_stop, sinon ne fait rien
			} else OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"pos_speed invalide: %u\n", (Uint16)pos_speed);
		} else {
			DCM_working_state_e asserState = DCM_get_state(BALLLAUNCHER_DCMOTOR_ID);
			CAN_msg_t resultMsg;

			if(QUEUE_has_error(queueId)) {
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
			} else if(asserState == DCM_TIMEOUT) {
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_TIMEOUT;
				QUEUE_set_error(queueId);
			} else if(asserState == DCM_IDLE) {
				resultMsg.data[2] = ACT_RESULT_DONE;
				resultMsg.data[3] = ACT_RESULT_ERROR_OK;
			} else return;	//Operation is not finished, do nothing

			resultMsg.sid = ACT_RESULT;
			resultMsg.data[0] = ACT_BALLLAUNCHER & 0xFF;
			resultMsg.data[1] = (QUEUE_get_arg(queueId) == 0)? ACT_BALLLAUNCHER_STOP : ACT_BALLLAUNCHER_ACTIVATE;
			resultMsg.size = 4;

			CAN_send(&resultMsg);
			QUEUE_behead(queueId);	//gestion terminée
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Système de captage a base du capteur à effet hall, 1 utilisation MAX par robot.
///////////////////////////////////////////////////////////////////////////////////

//Constante permettant d'avoir des valeurs indépendante de la fréquence de la carte
#if defined(FREQ_10MHZ)
	#define HALL_TIMECOUNTER_PULSE_PER_UNIT 1
#elif defined(FREQ_20MHZ)
	#define HALL_TIMECOUNTER_PULSE_PER_UNIT 2	//pour 20Mhz
#elif defined(FREQ_40MHZ)
	#define HALL_TIMECOUNTER_PULSE_PER_UNIT 4	//pour 40Mhz
#endif

//Incrément de timer par ms
#define HALL_TIMECOUNTER_PULSE_PER_MS (39.0625)

//Temps avant qu'on considère que la vitesse est nulle
#define HALL_TIMECOUNTER_MAX_PERIOD_TIME 300 //en ms

static Uint16 hall_last_detection_interval;
static Uint16 hall_full_period_elapsed_time;	//toujours multiple de DCM_TIMER_PERIOD en ms. compter comme ça évite une multiplication
static bool_e hall_dont_update_timer_overflow_count;	//TRUE si on ne doit pas changer total_elapsed_time_count
static bool_e hall_timer_val_last_detection_valid = FALSE;	//Vrai quand la dernière valeur du timer enregistré lors de la dernière detection est valide.

//Version priorité BALLLAUNCHER_get_speed < BALLLAUNCHER_HALLSENSOR_INT_ISR
static Sint16 BALLLAUNCHER_get_speed() {

	//Section critique
	BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 0;

	//Mettre à jour le compteur d'overflow que si ça n'a pas déjà été fait par l'IT externe de plus haute priorité
	if(hall_dont_update_timer_overflow_count == FALSE) {
		//Si on compte jusqu'a timeout la variable, on considère que la vitesse est 0
		if(hall_full_period_elapsed_time >= (HALL_TIMECOUNTER_MAX_PERIOD_TIME*HALL_TIMECOUNTER_PULSE_PER_MS)) {
			hall_last_detection_interval = 0;
			hall_timer_val_last_detection_valid = FALSE;
		} else hall_full_period_elapsed_time += ((DCM_TIMER_PERIOD_REG+1)/HALL_TIMECOUNTER_PULSE_PER_UNIT);
	}
	DCM_TIMER_OVERFLOW_REG = 0;	//On met le flag de l'IT de l'asservissement à 0, permet de savoir si total_elapsed_time_count à été mis à jour après un overflow ou pas
	BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 1;

	hall_dont_update_timer_overflow_count = FALSE;

	//Cas spécial: vitesse nulle
	if(hall_last_detection_interval == 0)
		return 0;

	//Calcul des tr/min, __builtin_divud permet une division utilisant les instructions du proc et non la librarie, histoire d'éviter de perdre inutilement des cycles processeurs
	return (Sint16)__builtin_divud((60000*HALL_TIMECOUNTER_PULSE_PER_MS/BALLLAUNCHER_EDGE_PER_ROTATION), hall_last_detection_interval);
}

void BALLLAUNCHER_HALLSENSOR_INT_ISR() {
	Uint16 hall_timer_val_detection;
	Uint16 hall_full_period_elapsed_time_saved;
	static Uint16 hall_timer_val_last_detection = 0; //valeur du timer à la dernière detection, valide que si timer_last_detection_valid == TRUE

	hall_full_period_elapsed_time_saved = hall_full_period_elapsed_time;
	hall_timer_val_detection = (DCM_TIMER_VAL_REG/HALL_TIMECOUNTER_PULSE_PER_UNIT);
	if(DCM_TIMER_OVERFLOW_REG) {
		//On vérifie si notre acquisition est avant ou après l'overflow par comparaison avec la moitié de la période
		if(hall_timer_val_detection < DCM_TIMER_PERIOD_REG/(2*HALL_TIMECOUNTER_PULSE_PER_UNIT)) {
			//Mise à jour du compteur d'overflow, BALLLAUNCHER_get_speed() n'a pas eu le temps de le faire, on doit le faire à sa place
			if(hall_full_period_elapsed_time_saved >= (HALL_TIMECOUNTER_MAX_PERIOD_TIME*HALL_TIMECOUNTER_PULSE_PER_MS)) {
				hall_last_detection_interval = 0;
				hall_timer_val_last_detection_valid = FALSE;
			} else hall_full_period_elapsed_time_saved += ((DCM_TIMER_PERIOD_REG+1)/HALL_TIMECOUNTER_PULSE_PER_UNIT);

			//On indique que BALLLAUNCHER_get_speed() n'aura pas à mettre à jour le compteur d'overflow puisse que c'est déjà fait
			hall_dont_update_timer_overflow_count = TRUE;
		}
	}

	hall_full_period_elapsed_time = 0;

	//Si la dernière detection est valide, on calcule, sinon on fait rien vu qu'on a pas assez d'info
	if(hall_timer_val_last_detection_valid)
		hall_last_detection_interval = hall_full_period_elapsed_time_saved + hall_timer_val_detection - hall_timer_val_last_detection;

	hall_timer_val_last_detection = hall_timer_val_detection;
	hall_timer_val_last_detection_valid = TRUE;

	//Changement de front detecté, on capte le passage de tous les aimants comme ça
	BALLLAUNCHER_HALLSENSOR_INT_EDGE = !BALLLAUNCHER_HALLSENSOR_INT_EDGE;
	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
}

#endif	//I_AM_ROBOT_KRUSTY
