/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Ball_launcher.c
 *	Package : Carte actionneur
 *	Description : Asservissement du lanceur de balle
 *  Auteur : Alexis
 *  Version 20130208
 *  Robot : Krusty
 */

#include "KBall_launcher.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_DCMotor2.h"
//#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_timer.h"
#include "../output_log.h"
#include "../act_queue_utils.h"
#include "KBall_launcher_config.h"

#define LOG_PREFIX "BL: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_BALLLAUNCHER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

//Define pour r�cuperer les valeurs du timer d'asservissement et sa p�riode
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

//Constante permettant d'avoir des valeurs ind�pendante de la fr�quence de la carte, utilis� pour le capteur
#if defined(FREQ_10MHZ)
	#define HALL_TIMECOUNTER_PULSE_PER_UNIT 1
#elif defined(FREQ_20MHZ)
	#define HALL_TIMECOUNTER_PULSE_PER_UNIT 2	//pour 20Mhz
#elif defined(FREQ_40MHZ)
	#define HALL_TIMECOUNTER_PULSE_PER_UNIT 4	//pour 40Mhz
#endif

#if DCMOTOR_NB_POS < 2
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif
#if DCM_NUMBER <= BALLLAUNCHER_DCMOTOR_ID
#error "Le nombre de DCMotor disponible n'est pas suffisant, veuillez augmenter DCM_NUMBER"
#endif

/*
 * Technique de captage de la vitesse:
 * A chaque passage de l'aimant, on r�cup�re la valeur du timer de l'asservissement.
 * A chaque overflow du timer d'asservissement, on incr�mente timer_overflow_number.
 * A chaque detection d'aimant, on met dans last_detection_interval, le nombre de cycle du timer d'asservissement:
 *  cycles = timer_overflow_number * periode_timer + valeur_timer_passage_1 - valeur_timer_passage_2
 * last_detection_interval est en fait cycles
 * Si le temps entre 2 passage d'aimant d�passe 850ms, on consid�re que le moteur est arret�.
 */

static Uint16 DCM_timer_period_predivised;

static void BALLLAUNCHER_run_command(queue_id_t queueId, bool_e init);
static Sint16 BALLLAUNCHER_get_speed();

void BALLLAUNCHER_init() {
	DCMotor_config_t ball_launcher_config;
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();

	//Pr�calcul de la constante
	DCM_timer_period_predivised = ((DCM_TIMER_PERIOD_REG+1)/HALL_TIMECOUNTER_PULSE_PER_UNIT);

	ball_launcher_config.sensor_read = &BALLLAUNCHER_get_speed;
	ball_launcher_config.Kp = BALLLAUNCHER_ASSER_KP;
	ball_launcher_config.Ki = BALLLAUNCHER_ASSER_KI;
	ball_launcher_config.Kd = BALLLAUNCHER_ASSER_KD;
	ball_launcher_config.pos[0] = 0;
	ball_launcher_config.pos[1] = 0;	//en tr/min
	ball_launcher_config.pwm_number = BALLLAUNCHER_DCMOTOR_PWM_NUM;
	ball_launcher_config.way_latch = &BALLLAUNCHER_DCMOTOR_PORT_WAY;
	ball_launcher_config.way_bit_number = BALLLAUNCHER_DCMOTOR_PORT_WAY_BIT;
	ball_launcher_config.way0_max_duty = BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY0;
	ball_launcher_config.way1_max_duty = BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY1;
	ball_launcher_config.timeout = BALLLAUNCHER_ASSER_TIMEOUT;
	ball_launcher_config.epsilon = BALLLAUNCHER_ASSER_POS_EPSILON;
	DCM_config(BALLLAUNCHER_DCMOTOR_ID, &ball_launcher_config);
	DCM_stop(BALLLAUNCHER_DCMOTOR_ID);

	BALLLAUNCHER_HALLSENSOR_INT_PRIORITY = 6;
	DCM_TIMER_PRIORITY_REG = 5;
	BALLLAUNCHER_HALLSENSOR_INT_EDGE = 1;  //interrupt on falling edge
	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
	BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 1;

	//Si la priorit� de l'interruption INTx (qui est d�clanch�e lors d'un passage d'un aimant devant le capteur) est de priorit� sup�rieure � celle du timer g�rant l'asservissement,
	//il est possible d'avoir une interruption INTx entre l'overflow du timer (donc il recommence a compter a partir de 0) et l'actualisation du nombre d'overflow dans la variable timer_overflow_number.
	if(BALLLAUNCHER_HALLSENSOR_INT_PRIORITY <= DCM_TIMER_PRIORITY_REG)
		COMPONENT_log(LOG_LEVEL_Error, "Attention ! La priorit� de l'interruption INTx doit �tre sup�rieur � celle de l'asservissement ! (TIMERx) (et si le code marche, qui l'a apport� � lourdes ?)\n");

	COMPONENT_log(LOG_LEVEL_Info, "Lanceur de balle initialis�\n");
}

void BALLLAUNCHER_stop() {
	DCM_stop(BALLLAUNCHER_DCMOTOR_ID);
}

void BALLLAUNCHER_set_speed(Uint16 tr_per_min) {
	if(tr_per_min == 0) {
		COMPONENT_log(LOG_LEVEL_Debug, "Direct cmd: Motor stoped\n");
		DCM_stop(BALLLAUNCHER_DCMOTOR_ID);	//On est sur de l'arreter comme �a, m�me en cas de probl�me capteur
	} else {
		COMPONENT_log(LOG_LEVEL_Debug, "Direct cmd: Run motor at speed: %d\n", tr_per_min);
		DCM_setPosValue(BALLLAUNCHER_DCMOTOR_ID, 1, tr_per_min);
		DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 1);
		DCM_restart(BALLLAUNCHER_DCMOTOR_ID); //Red�marrage si on l'avait arr�t� avec DCM_stop, sinon ne fait rien
	}
}

bool_e BALLLAUNCHER_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_BALLLAUNCHER) {
		switch(msg->data[0]) {
			case ACT_BALLLAUNCHER_ACTIVATE:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_BallLauncher, &BALLLAUNCHER_run_command, U16FROMU8(msg->data[2], msg->data[1]));
				break;

			case ACT_BALLLAUNCHER_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_BallLauncher, &BALLLAUNCHER_run_command, 0);
				break;

			default:
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static Sint16 last_speed_detected;

static void BALLLAUNCHER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(init == TRUE) {
		//Send command
		Uint16 pos_speed = QUEUE_get_arg(queueId)->param;
		if(pos_speed == 0) {
			COMPONENT_log(LOG_LEVEL_Debug, "Motor stoped\n");
			//DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 0);
			DCM_stop(BALLLAUNCHER_DCMOTOR_ID);	//On est sur de l'arreter comme �a, m�me en cas de probl�me capteur
		} else {
			COMPONENT_log(LOG_LEVEL_Debug, "Run motor at speed: %d\n", pos_speed);
			DCM_setPosValue(BALLLAUNCHER_DCMOTOR_ID, 1, pos_speed);
			DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 1);
			DCM_restart(BALLLAUNCHER_DCMOTOR_ID); //Red�marrage si on l'avait arr�t� avec DCM_stop, sinon ne fait rien
		}
	} else {
		Uint8 result, errorCode;
		Uint16 line;
		static Sint16 lastlast_speed = 0;

		if(last_speed_detected != lastlast_speed) {
			lastlast_speed = last_speed_detected;
			COMPONENT_log(LOG_LEVEL_Trace, "Current speed: %d\n", lastlast_speed);
		}

		if(ACTQ_check_status_dcmotor(BALLLAUNCHER_DCMOTOR_ID, FALSE, &result, &errorCode, &line))
			QUEUE_next(queueId, ACT_BALLLAUNCHER, result, errorCode, line);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Syst�me de captage a base du capteur � effet hall, 1 utilisation MAX par robot.
///////////////////////////////////////////////////////////////////////////////////

//Incr�ment de timer par ms
#define HALL_TIMECOUNTER_PULSE_PER_MS (39.0625)

//Temps avant qu'on consid�re que la vitesse est nulle
#define HALL_TIMECOUNTER_MAX_PERIOD_TIME 300 //en ms

static volatile Uint16 hall_last_detection_interval;
static volatile Uint16 hall_full_period_elapsed_time;	//toujours multiple de DCM_TIMER_PERIOD en ms. compter comme �a �vite une multiplication
static volatile bool_e hall_dont_update_timer_overflow_count;	//TRUE si on ne doit pas changer total_elapsed_time_count
static volatile bool_e hall_timer_val_last_detection_valid = FALSE;	//Vrai quand la derni�re valeur du timer enregistr� lors de la derni�re detection est valide.

//Version priorit� BALLLAUNCHER_get_speed < BALLLAUNCHER_HALLSENSOR_INT_ISR
static Sint16 BALLLAUNCHER_get_speed() {

	//Section critique
	BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 0;

	//Mettre � jour le compteur d'overflow que si �a n'a pas d�j� �t� fait par l'IT externe de plus haute priorit�
	if(hall_dont_update_timer_overflow_count == FALSE) {
		//Si on compte jusqu'a timeout la variable, on consid�re que la vitesse est 0
		if(hall_full_period_elapsed_time >= ((Uint16)(HALL_TIMECOUNTER_MAX_PERIOD_TIME*HALL_TIMECOUNTER_PULSE_PER_MS))) {
			hall_last_detection_interval = 0;
			hall_timer_val_last_detection_valid = FALSE;
		} else hall_full_period_elapsed_time += DCM_timer_period_predivised;
	}
	DCM_TIMER_OVERFLOW_REG = 0;	//On met le flag de l'IT de l'asservissement � 0, permet de savoir si total_elapsed_time_count � �t� mis � jour apr�s un overflow ou pas
	BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 1;

	hall_dont_update_timer_overflow_count = FALSE;

	//Cas sp�cial: vitesse nulle
	if(hall_last_detection_interval == 0)
		return 0;

	//Calcul des tr/min, __builtin_divud permet une division utilisant les instructions du proc et non la librarie, histoire d'�viter de perdre inutilement des cycles processeurs
	return (last_speed_detected = (Sint16)__builtin_divud((60000*HALL_TIMECOUNTER_PULSE_PER_MS/BALLLAUNCHER_EDGE_PER_ROTATION), hall_last_detection_interval));
}

void BALLLAUNCHER_HALLSENSOR_INT_ISR() {
	Uint16 hall_timer_val_detection;
	Uint16 hall_full_period_elapsed_time_saved;
	static Uint16 hall_timer_val_last_detection = 0; //valeur du timer � la derni�re detection, valide que si timer_last_detection_valid == TRUE

	hall_full_period_elapsed_time_saved = hall_full_period_elapsed_time;
	hall_timer_val_detection = (DCM_TIMER_VAL_REG/HALL_TIMECOUNTER_PULSE_PER_UNIT);
	if(DCM_TIMER_OVERFLOW_REG) {
		//On v�rifie si notre acquisition est avant ou apr�s l'overflow par comparaison avec la moiti� de la p�riode
		if(hall_timer_val_detection < DCM_timer_period_predivised/2) {
			//Mise � jour du compteur d'overflow, BALLLAUNCHER_get_speed() n'a pas eu le temps de le faire, on doit le faire � sa place
			if(hall_full_period_elapsed_time_saved >= ((Uint16)(HALL_TIMECOUNTER_MAX_PERIOD_TIME*HALL_TIMECOUNTER_PULSE_PER_MS))) {
				hall_last_detection_interval = 0;
				hall_timer_val_last_detection_valid = FALSE;
			} else hall_full_period_elapsed_time_saved += DCM_timer_period_predivised;

			//On indique que BALLLAUNCHER_get_speed() n'aura pas � mettre � jour le compteur d'overflow puisse que c'est d�j� fait
			hall_dont_update_timer_overflow_count = TRUE;
		}
	}

	if(hall_timer_val_detection == DCM_TIMER_PERIOD_REG) {  //L'interruption timer intervient quand TMRx == PRx et pourra �tre d�j� compt� dans hall_full_period_elapsed_time_saved. Si dans cette interruption, TMRx == PRx, on doit enlever le temps d'une periode car TMRx n'est pas encore pass� � 0.
		hall_full_period_elapsed_time_saved -= DCM_timer_period_predivised;
		hall_full_period_elapsed_time = DCM_timer_period_predivised;
	} else hall_full_period_elapsed_time = 0;


	//Si la derni�re detection est valide, on calcule, sinon on fait rien vu qu'on a pas assez d'info
	if(hall_timer_val_last_detection_valid)
		hall_last_detection_interval = hall_full_period_elapsed_time_saved + hall_timer_val_detection - hall_timer_val_last_detection;

	hall_timer_val_last_detection = hall_timer_val_detection;
	hall_timer_val_last_detection_valid = TRUE;

	//Changement de front detect�, on capte le passage de tous les aimants comme �a
	//Mais en fait non, les temps de chargements du condensateur de filtrage sont non sym�triques ...
#if BALLLAUNCHER_EDGE_PER_ROTATION == 2
	BALLLAUNCHER_HALLSENSOR_INT_EDGE = !BALLLAUNCHER_HALLSENSOR_INT_EDGE;
#endif
	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
}

#endif	//I_AM_ROBOT_KRUSTY
