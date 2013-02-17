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

#include "../QS/QS_DCMotor.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_timer.h"

#if DCM_TIMER == 1
	#define DCM_TIMER_VAL_REG    TMR1
	#define DCM_TIMER_PERIOD_REG PR1
	#define DCM_TIMER_PRIORITY_REG IPC0bits.T1IP
	#define DCM_TIMER_ENABLE_REG IEC0bits.T1IE
	#define DCM_TIMER_OVERFLOW_REG IFS0bits.T1IF
#elif DCM_TIMER == 2
	#define DCM_TIMER_VAL_REG    TMR2
	#define DCM_TIMER_PERIOD_REG PR2
	#define DCM_TIMER_PRIORITY_REG IPC1bits.T2IP
	#define DCM_TIMER_ENABLE_REG IEC0bits.T2IE
	#define DCM_TIMER_OVERFLOW_REG IFS0bits.T2IF
#elif DCM_TIMER == 3
	#define DCM_TIMER_VAL_REG    TMR3
	#define DCM_TIMER_PERIOD_REG PR3
	#define DCM_TIMER_PRIORITY_REG IPC1bits.T3IP
	#define DCM_TIMER_ENABLE_REG IEC0bits.T3IE
	#define DCM_TIMER_OVERFLOW_REG IFS0bits.T3IF
#elif DCM_TIMER == 4
	#define DCM_TIMER_VAL_REG    TMR4
	#define DCM_TIMER_PERIOD_REG PR4
	#define DCM_TIMER_PRIORITY_REG IPC5bits.T4IP
	#define DCM_TIMER_ENABLE_REG IEC1bits.T4IE
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

//Change la vitesse du lanceur de balle. Attention les vitesses peuvent être négative ou positive.
static void BALLLAUNCHER_set_target_speed(Sint16 tr_min);

void BALLLAUNCHER_init() {
	ball_launcher_config.sensor_read = &BALLLAUNCHER_get_speed;
	ball_launcher_config.Kp = BALLLAUNCHER_ASSER_KP*1024;
	ball_launcher_config.Ki = BALLLAUNCHER_ASSER_KI*1048576;
	ball_launcher_config.Kd = BALLLAUNCHER_ASSER_KD*1024;
	ball_launcher_config.pos[0] = 0;
	ball_launcher_config.pos[1] = (BALLLAUNCHER_TARGET_SPEED/60.0*DCM_TIMER_PERIOD/1000*BALLLAUNCHER_EDGE_PER_ROTATION);	//BALLLAUNCHER_TARGET_SPEED en tr/min (utilisation de 60.0 et pas 60 pour forcer le compilateur à faire un calcul sans overflow sur la constante)
	ball_launcher_config.pwm_number = BALLLAUNCHER_DCMOTOR_PWM_NUM;
	ball_launcher_config.way_latch = (Uint16*)&BALLLAUNCHER_DCMOTOR_PORT_WAY;
	ball_launcher_config.way_bit_number = BALLLAUNCHER_DCMOTOR_PORT_WAY_BIT;
	ball_launcher_config.way0_max_duty = BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY0;
	ball_launcher_config.way1_max_duty = BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY1;
	ball_launcher_config.timeout = 2000; //FIXME: Mettre une valeur appropriée
	DCM_config(BALLLAUNCHER_DCMOTOR_ID, &ball_launcher_config);

	BALLLAUNCHER_HALLSENSOR_INT_PRIORITY = 4;
	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
	BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 1;

	//Si la priorité de l'interruption INTx (qui est déclanchée lors d'un passage d'un aimant devant le capteur) est de priorité supérieure à celle du timer gérant l'asservissement,
	//il est possible d'avoir une interruption INTx entre l'overflow du timer (donc il recommence a compter a partir de 0) et l'actualisation du nombre d'overflow dans la variable timer_overflow_number.
	if(BALLLAUNCHER_HALLSENSOR_INT_PRIORITY >= DCM_TIMER_PRIORITY_REG)
		debug_printf("BL: Attention ! La priorité de l'interruption INTx devrait être inférieur à celle de l'asservissement (TIMERx)\n");
}

void BALLLAUNCHER_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;
	if(msg->sid == ACT_BALLLAUNCHER) {
		switch(msg->data[0]) {
			case ACT_BALLLAUNCHER_ACTIVATE:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_BallLauncher);
				 QUEUE_add(queueId, &BALLLAUNCHER_run_command, msg->data[1] | (msg->data[2] << 8), QUEUE_ACT_BallLauncher);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_BallLauncher);
				 break;

			case ACT_BALLLAUNCHER_STOP:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_BallLauncher);
				 QUEUE_add(queueId, &BALLLAUNCHER_run_command, 0, QUEUE_ACT_BallLauncher);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_BallLauncher);
				 break;

			default:
				debug_printf("BL: invalid CAN msg data[0]=%d !\n", msg->data[0]);
		}
	}
}

void BALLLAUNCHER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_BallLauncher) {
		if(init == TRUE) {
			//Send command
			Sint16 pos_speed = QUEUE_get_arg(queueId);
			if(pos_speed == 0)
				DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 0);
			else if(pos_speed > 0) {
				BALLLAUNCHER_set_target_speed(pos_speed);
				DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 1);
			} else debug_printf("BL: pos_speed invalide: %u\n", pos_speed);
			QUEUE_behead(queueId);	//gestion terminée
		}
	}
}

static void BALLLAUNCHER_set_target_speed(Sint16 tr_min) {
	Uint16 val;
	//On doit diviser, sinon le nombre a multiplier est < que 1 (donc c'est 0 pour un entier)

	//Utilise la fonction de division dans une librarie, potentiellement lente
	//DCM_setPosValue(BALLLAUNCHER_DCMOTOR_ID, 1, tr_min / (Uint16)(60.0*1000/BALLLAUNCHER_EDGE_PER_ROTATION/DCM_TIMER_PERIOD));
	//Avec de l'assembleur: division en 18 cycles, mais c'est de l'assembleur
	//(utilisation de 60.0 et pas 60 pour forcer le compilateur à faire un calcul sans overflow sur la constante)
	asm("repeat #17\n"
	    "div.s %[trmin], %[div]\n"
	    : "=a"(val)	//output
	    : [trmin] "r"(tr_min), [div] "e"((Uint16)(60.0*1000/BALLLAUNCHER_EDGE_PER_ROTATION/DCM_TIMER_PERIOD))	//input
		: "w1");	//registres modifiés après l'opération: le compilateur ne peu pas l'utiliser pour garder une valeur a travers le code assembleur
	DCM_setPosValue(BALLLAUNCHER_DCMOTOR_ID, 1, val);
}


//TODO: en fait utiliser du 32bits n'est pas nécessaire, a revoir cette partie de code
#ifdef BALLLAUNCHER_USE_HIGH_PRIORITY_TIMER

//données d'aquisition de la vitesse
static Uint8 timer_overflow_number;	//incrémenté à chaque fois que la fonction de capteur de l'asservissement est appelée (après l'overflow de son timer)
static Uint32 last_detection_interval; //intervale entre 2 passage d'aimant, en valeur timer

//Vitesse minimale detectable: 35 tr/min = 857 msec/tr
//Version priorité BALLLAUNCHER_get_speed > BALLLAUNCHER_HALLSENSOR_INT_ISR
static Sint16 BALLLAUNCHER_get_speed() {
	//S'il s'est rien passé pendant 850ms, on considère que le moteur est arreté.
	//Si le temps trouvé entre 2 detections est supérieur à 1048560 us, on considère aussi que le moteur est arreté (pour des raisons de problèmes d'overflow sur des opérations suivantes.)
	if(timer_overflow_number >= (850/DCM_TIMER_PERIOD) || last_detection_interval >= (65535*16*TIMER_PULSE_PER_US)) {
		last_detection_interval = 0;
		return 0;
	} else timer_overflow_number++;

	//Cas spécial: moteur stoppé
	if(last_detection_interval == 0)
		return 0;

	//Vitesse maximale détectable: 32767 tr/min, 1832 us entre chaque tours
	if(last_detection_interval <= (1832*TIMER_PULSE_PER_US/BALLLAUNCHER_EDGE_PER_ROTATION))
		return 32767;

	//Calcul des tr/min
	//60*1000000/BALLLAUNCHER_EDGE_PER_ROTATION est la vitesse en tr/min si entre 2 passage d'aimant il y a 1us
	//La valeur est grande et en usec, dépasserait 65535, de plus moins de précision est demandée -> division plus forte dès le début
	//Attention, last_detection_interval ne pas dépasser une valeur corespondant à plus de 1048560usec (on regarde déjà si c'est supérieur à 850000usec avant)

	//__builtin_divud est une division utilisant une instruction et non la librarie qui fait la division sans instruction de division (sauf qu'on a l'instruction)
	//prototype de la fonction: Uint16 __builtin_divud(Uint32 val1, Uint16 val2), le resultat est val1/val2
	if(last_detection_interval >= (65535*TIMER_PULSE_PER_US)) {
		return __builtin_divud((60*1000000/BALLLAUNCHER_EDGE_PER_ROTATION/16), __builtin_divud(last_detection_interval, (TIMER_PULSE_PER_US*16)));
	} else {
		return __builtin_divud((60*1000000/BALLLAUNCHER_EDGE_PER_ROTATION), __builtin_divud(last_detection_interval, TIMER_PULSE_PER_US));
	}
	
	//Jamais exécuté ici
}

void BALLLAUNCHER_HALLSENSOR_INT_ISR() {
	bool_e dcm_timer_state;
	Uint16 timer_val;
	Uint8 timer_overflow_number_saved;
	static Uint16 timer_last_detection = 0; //valeur du timer à la dernière detection
	static bool_e timer_last_detection_valid = FALSE;

	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;

#ifdef VERBOSE_MODE
	//Si le timer a overflow, timer_overflow_number doit être strictement supérieur à 1, si c'est pas le cas il y a un bug
	if(timer_overflow_number == 0 && timer_val < timer_last_detection) {
		debug_printf("BL: Erreur dans BALLLAUNCHER_HALLSENSOR_INT_ISR: 0 overflow mais la valeur timer est inférieur à l'ancienne !!\n");
	}
#endif

	//section critique, le timer de l'asservissement ne doit pas déclencher une IT a ce moment car timer_overflow_number ne doit pas être modifié
	dcm_timer_state = DCM_TIMER_ENABLE_REG;
	DCM_TIMER_ENABLE_REG = 0;
	timer_val = DCM_TIMER_VAL_REG;
	//Si il y a eu un overflow entre la désactivation de l'interruption et après la récupération du timer
	//On ne peut garantir que la valeur récupérée est avant l'overflow, donc on ignore ce passage d'aimant.
	//Ca ne devrai pas arriver souvent, mais si on ne l'ignore pas, ça peut causer des sauts de vitesse capté plus ou moins importantes ...
	if(DCM_TIMER_OVERFLOW_REG) {
		DCM_TIMER_ENABLE_REG = dcm_timer_state;
		timer_last_detection_valid = FALSE;
	} else {
		timer_overflow_number_saved = timer_overflow_number;
		timer_overflow_number = 0;
		DCM_TIMER_ENABLE_REG = dcm_timer_state;

		//Attention, du 32bits dans l'air
		//La fonction __builtin force l'utilisation d'une isntruction au lieu d'une librarie pour des raisons de vitesse d'exécution
		//prototype: unsigned long __builtin_muluu(const unsigned p0, const unsigned p1);, opération: return_value = p0*p1
		if(timer_last_detection_valid)
			last_detection_interval = __builtin_muluu(timer_overflow_number_saved, DCM_TIMER_PERIOD_REG) + timer_val - timer_last_detection;
		timer_last_detection = timer_val;
		timer_last_detection_valid = TRUE;
	}

	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
	BALLLAUNCHER_HALLSENSOR_INT_EDGE = !BALLLAUNCHER_HALLSENSOR_INT_EDGE;
}

#else	//l'IT INTx est de plus haute priorité


//FIXME: Ajouter 1 à DCM_TIMER_PERIOD_REG ?

#define TIMECOUNTER_PULSE_PER_UNIT 1
//#define TIMECOUNTER_PULSE_PER_UNIT 2	//pour 20Mhz  Ces deux comenté ne seront probablement jamais utilisés
//#define TIMECOUNTER_PULSE_PER_UNIT 4	//pour 40Mhz

static Uint16 last_detection_interval;
static Uint16 total_elapsed_time_count;	//toujours multiple de DCM_TIMER_PERIOD en ms. compter comme ça évite une multiplication
static bool_e dont_increment_timer_overflow_count;	//TRUE si on ne doit pas changer total_elapsed_time_count

//Vitesse minimale detectable: 35 tr/min = 857 msec/tr
//Version priorité BALLLAUNCHER_get_speed < BALLLAUNCHER_HALLSENSOR_INT_ISR
static Sint16 BALLLAUNCHER_get_speed() {
	//Si on compte jusqu'a overflow la variable, on considère que la vitesse est 0
	if(total_elapsed_time_count >= (0xFFFF - (DCM_TIMER_PERIOD_REG/TIMECOUNTER_PULSE_PER_UNIT))) {
		last_detection_interval = 0;
	} else {
		BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 0;
		if(dont_increment_timer_overflow_count == FALSE)
			total_elapsed_time_count += (DCM_TIMER_PERIOD_REG/TIMECOUNTER_PULSE_PER_UNIT);
		DCM_TIMER_OVERFLOW_REG = 0;	//BIDOUILLAGE ICI !!! On met le flag de l'IT de l'asservissement à 0, permet de savoir si total_elapsed_time_count à été mis à jour après un overflow ou pas
		BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 1;
		dont_increment_timer_overflow_count = FALSE;
	}

	//Calcul des tr/min
	return last_detection_interval;

	//Jamais exécuté ici
}

void BALLLAUNCHER_HALLSENSOR_INT_ISR() {
	Uint16 timer_val;
	Uint16 timer_overflow_number_saved;
	static Uint16 timer_last_detection = 0; //valeur du timer à la dernière detection
	static bool_e timer_last_detection_valid = FALSE;

	timer_overflow_number_saved = total_elapsed_time_count;
	if(DCM_TIMER_OVERFLOW_REG) {
		timer_overflow_number_saved += (DCM_TIMER_PERIOD_REG/TIMECOUNTER_PULSE_PER_UNIT);
		total_elapsed_time_count = 0;
		dont_increment_timer_overflow_count = TRUE;
	} else total_elapsed_time_count = 0;

	timer_val = (DCM_TIMER_VAL_REG/TIMECOUNTER_PULSE_PER_UNIT);

	if(timer_last_detection_valid)
		last_detection_interval = timer_overflow_number_saved + timer_val - timer_last_detection;
	timer_last_detection = timer_val;
	timer_last_detection_valid = TRUE;
	
	BALLLAUNCHER_HALLSENSOR_INT_EDGE = !BALLLAUNCHER_HALLSENSOR_INT_EDGE;
	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
}

#endif //BALLLAUNCHER_USE_HIGH_PRIORITY_TIMER

#endif	//I_AM_ROBOT_KRUSTY
