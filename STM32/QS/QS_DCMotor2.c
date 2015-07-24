/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_DCMotor.c
 *	Package : Qualité Soft
 *	Description : Gestion des moteurs à courant continu asservis en position
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#include "QS_DCMotor2.h"
#ifdef USE_DCMOTOR2

#include "QS_pwm.h"
#include "QS_can.h"
#include "QS_CANmsgList.h"


	#define MAX_STEP_OF_PWM		20   //[%/10ms]
	#define SPEED_DETECT_LARGE_EPSILON	2


	typedef enum
	{
		INITIALIZED,
		NOT_INITIALIZED,
		STOPPED
	}init_state_e;

/*-----------------------------------------
		Variables globales privées
-----------------------------------------*/

	typedef struct
	{
		DCMotor_config_t config;	// la config du DCMotor
		DCM_working_state_e cmd_state;	// Etat de la dernière commande: Position atteinte, en mouvement, timeout
		Sint32 integrator;			// Integrateur pour le PID
		Uint16 cmd_time;			// temps depuis la reception de la commande, en ms
		Uint8 posToGo;				// consigne, en numero de position actionneur
		Uint8 last_cmd;				// commande PWM précédente
		Uint8 current_cmd;			// commande PWM actuelle
		Sint16 last_pos;			// dernière position
		Sint16 wanted_pos;			// Position voulue
		Sint16 previous_error;		// valeur de l'erreur à l'appel précédent du PID, pour calcul du terme derivé
		init_state_e init_state;	// flag indiquant si le moteur a été configuré
		Uint8 time_waiting_limit_pwm;// temps en multiple de DCM_TIME_PERIOD qu'on attend avant de relacher la limite de pwm
	}DCMotor_t;

	static DCMotor_t DCMotors[DCM_NUMBER];

	static bool_e DCM_initialized = FALSE;

/*-----------------------------------------
		Prototypes des fonctions privées
-----------------------------------------*/
	// Accesseur
	static void DCM_setWay(Uint8 dc_motor_id, Uint8 value);
	static Uint8 DCM_getWay(Uint8 dc_motor_id);
	// Baisse les drapeaux d'initialisation des moteurs
	static void DCM_uninitialize_all();
/*-----------------------------------------
		Initialisation
-----------------------------------------*/
void DCM_init()
{
	if(DCM_initialized)
		return;
	DCM_initialized = TRUE;

	PWM_init();
	// Initialisation
	DCM_uninitialize_all();
}

DCM_working_state_e DCM_get_state (Uint8 dc_motor_id)
{
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state==STOPPED));
	return thiss->cmd_state;
}


// configurer un moteur CC après initialisation du module QS
void DCM_config (Uint8 dc_motor_id, DCMotor_config_t* config)
{
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	thiss->config = *config;
	thiss->cmd_state=DCM_IDLE;
	thiss->integrator=0;
	thiss->cmd_time=0;
	thiss->posToGo=0;
	thiss->current_cmd=0;
	thiss->last_cmd=0;
	thiss->wanted_pos=0;
	thiss->previous_error=0;

	thiss->init_state=INITIALIZED;
}

static void DCM_uninitialize_all()
{
	Uint8 i;
	for (i=0;i<DCM_NUMBER;i++)
	{
		(DCMotors[i]).init_state = NOT_INITIALIZED;
	}
}


/*-----------------------------------------
	Setter pour le sens des moteurs
-----------------------------------------*/
static void DCM_setWay(Uint8 dc_motor_id, Uint8 value)
{
	DCMotor_config_t* thiss = &(DCMotors[dc_motor_id].config);
	assert((DCMotors[dc_motor_id].init_state == INITIALIZED) || (DCMotors[dc_motor_id].init_state==STOPPED));
	if(value ^ thiss->inverse_way)
		GPIO_SetBits(thiss->way_latch, thiss->way_bit_number);
	else
		GPIO_ResetBits(thiss->way_latch, thiss->way_bit_number);
}

static Uint8 DCM_getWay(Uint8 dc_motor_id)
{
	DCMotor_config_t* thiss = &(DCMotors[dc_motor_id].config);
	assert((DCMotors[dc_motor_id].init_state == INITIALIZED) || (DCMotors[dc_motor_id].init_state==STOPPED));
	return GPIO_ReadInputDataBit(thiss->way_latch, thiss->way_bit_number);
}

void DCM_setInverseWay(Uint8 dc_motor_id, bool_e inverse){
	DCMotor_config_t* thiss = &(DCMotors[dc_motor_id].config);
	assert((DCMotors[dc_motor_id].init_state == INITIALIZED) || (DCMotors[dc_motor_id].init_state==STOPPED));
	thiss->inverse_way = inverse;
}



/*-----------------------------------------
		Ordre de déplacement
-----------------------------------------*/
void DCM_goToPos(Uint8 dc_motor_id, Uint8 pos)
{
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state==STOPPED));
	thiss->time_waiting_limit_pwm = DCM_TIME_PERIOD*5;
	thiss->posToGo = pos;
	thiss->cmd_time = 0;
	thiss->cmd_state = DCM_WORKING;
}

/*-----------------------------------------
		Changement de la valeur d'une position
-----------------------------------------*/
void DCM_setPosValue(Uint8 dc_motor_id, Uint8 pos_to_update, Sint16 value_pos) {
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state==STOPPED));
	thiss->config.pos[pos_to_update] = value_pos;
	if(thiss->posToGo == pos_to_update) {
		thiss->cmd_time = 0;
		thiss->cmd_state = DCM_WORKING;
	}
}

/*-----------------------------------------
		Récupération de la valeur d'une position
-----------------------------------------*/
Sint16 DCM_getPosValue(Uint8 dc_motor_id, Uint8 pos_to_get) {
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state==STOPPED));
	return thiss->config.pos[pos_to_get];
}

/*-----------------------------------------
		Change les coefs d'asservissement.
-----------------------------------------*/
void DCM_setCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd) {
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state == STOPPED));
	init_state_e previousState = thiss->init_state;

	thiss->init_state = STOPPED; //Aucun calcul d'asservissement ne doit être fait pendant ce temps

	thiss->config.Kp = Kp;
	thiss->config.Ki = Ki;
	thiss->config.Kd = Kd;

	thiss->init_state = previousState;
}

/*-----------------------------------------
		Récupère les coefs d'asservissement.
-----------------------------------------*/
void DCM_getCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd) {
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state == STOPPED));

	if(Kp) *Kp = thiss->config.Kp;
	if(Ki) *Ki = thiss->config.Ki;
	if(Kd) *Kd = thiss->config.Kd;
}

/*-----------------------------------------
  Change les double coefs d'asservissement.
-----------------------------------------*/
void DCM_setDoubleCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd, Sint16 Kp2, Sint16 Ki2, Sint16 Kd2){
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state == STOPPED));
	init_state_e previousState = thiss->init_state;

	thiss->init_state = STOPPED; //Aucun calcul d'asservissement ne doit être fait pendant ce temps

	thiss->config.Kp = Kp;
	thiss->config.Ki = Ki;
	thiss->config.Kd = Kd;
	thiss->config.Kp2 = Kp2;
	thiss->config.Ki2 = Ki2;
	thiss->config.Kd2 = Kd2;

	thiss->init_state = previousState;
}

/*-----------------------------------------
  Récupère les double coefs d'asservissement.
-----------------------------------------*/
void DCM_getDoubleCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd, Sint16* Kp2, Sint16* Ki2, Sint16* Kd2){
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state == STOPPED));

	if(Kp) *Kp = thiss->config.Kp;
	if(Ki) *Ki = thiss->config.Ki;
	if(Kd) *Kd = thiss->config.Kd;
	if(Kp2) *Kp2 = thiss->config.Kp2;
	if(Ki2) *Ki2 = thiss->config.Ki2;
	if(Kd2) *Kd2 = thiss->config.Kd2;
}

/*-----------------------------------------
  Change si le moteur fonctionne en double PID
-----------------------------------------*/
void DCM_setDoublePID(Uint8 dc_motor_id, bool_e double_PID){
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state == STOPPED));

	init_state_e previousState = thiss->init_state;
	thiss->init_state = STOPPED; //Aucun calcul d'asservissement ne doit être fait pendant ce temps

	thiss->config.double_PID = double_PID;

	thiss->init_state = previousState;
}

/*-----------------------------------------
  Change si le moteur fonctionne en double PID
-----------------------------------------*/
bool_e DCM_getDoublePID(Uint8 dc_motor_id){
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	assert((thiss->init_state == INITIALIZED) || (thiss->init_state == STOPPED));

	return thiss->config.double_PID;
}

/*-------------------------------------------
  Récupère les coefficients pwm
--------------------------------------------*/
void DCM_getPwmWay(Uint8 dc_motor_id, Uint8 *way0_max_duty, Uint8 *way1_max_duty){
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	*way0_max_duty = thiss->config.way0_max_duty;
	*way1_max_duty = thiss->config.way1_max_duty;
}

/*-------------------------------------------
  Change les coefficients pwm
--------------------------------------------*/
void DCM_setPwmWay(Uint8 dc_motor_id, Uint8 way0_max_duty, Uint8 way1_max_duty){
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	thiss->config.way0_max_duty = way0_max_duty;
	thiss->config.way1_max_duty = way1_max_duty;
}

/*-----------------------------------------
		Arret de l'asservissement d'un actionneur
-----------------------------------------*/
void DCM_stop(Uint8 dc_motor_id)
{
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	if(thiss->init_state == INITIALIZED)
	{
		thiss->init_state = STOPPED;
		thiss->cmd_state = DCM_IDLE;
		PWM_stop(thiss->config.pwm_number);
	}
}


/*-----------------------------------------
		Arret de tous les asservissements
-----------------------------------------*/
void DCM_stop_all()
{
	Uint8 i;
	for (i=0;i<DCM_NUMBER;i++)
	{
		DCM_stop(i);
	}
}

/*-----------------------------------------
		Reactivation de l'asservissement d'un actionneur
-----------------------------------------*/
void DCM_restart(Uint8 dc_motor_id)
{
	DCMotor_t* thiss = &(DCMotors[dc_motor_id]);
	if(thiss->init_state == STOPPED)
	{
		thiss->integrator = 0;
		thiss->cmd_time = 0;
		thiss->cmd_state = DCM_WORKING;
		thiss->init_state = INITIALIZED;
		thiss->wanted_pos = thiss->config.sensor_read();
	}
}

/*-----------------------------------------
		Reactivation de tous les asservissements
-----------------------------------------*/
void DCM_restart_all()
{
	Uint8 i;
	for (i=0;i<DCM_NUMBER;i++)
	{
		DCM_restart(i);
	}
}


/*-----------------------------------------
	Interruption de commande
-----------------------------------------*/
void DCM_process_it()
{
	if(DCM_initialized == FALSE)
		return;

	DCMotor_t* thiss;
	DCMotor_config_t* config;
	Uint8 dc_motor_id;
	Sint32 differential;
	Sint32 computed_cmd;
	Sint32 error;
	Sint16 pos;

	for (dc_motor_id = 0; dc_motor_id < DCM_NUMBER; dc_motor_id++)
	{
		thiss = &(DCMotors[dc_motor_id]);
		config = &(thiss->config);

		if (thiss->init_state == INITIALIZED)
		{
			thiss->wanted_pos = config->pos[thiss->posToGo];

			// Acquisition de la position pour la détection de l'arrêt du moteur
			pos = (config->sensor_read)();
			error = thiss->wanted_pos-pos;

			//Gestion des changements d'états
			switch(thiss->cmd_state) {
				case DCM_WORKING:
					thiss->cmd_time += DCM_TIME_PERIOD;
					if(absolute(error) < (Sint16)config->epsilon && absolute(thiss->previous_error) < (Sint16)config->epsilon)
						thiss->cmd_state = DCM_IDLE;
					else if(config->timeout && thiss->cmd_time >= config->timeout){
						if(absolute(error) < (Sint16)config->large_epsilon && absolute(thiss->previous_error) < (Sint16)config->large_epsilon)
							thiss->cmd_state = DCM_IDLE;
						else
							thiss->cmd_state = DCM_TIMEOUT;
					}
					break;

				case DCM_IDLE:
					thiss->cmd_time = 0;
					if(absolute(error) < (Sint16)config->large_epsilon)
						thiss->cmd_state = DCM_WORKING;
					break;

				case DCM_TIMEOUT:
					break;
			}

			//Gestion des actions dans les états
			if(thiss->cmd_state == DCM_TIMEOUT || absolute(thiss->wanted_pos - pos) < config->dead_zone/2){
				PWM_stop(config->pwm_number);
			}else{

				/* Integration si on n'est pas en saturation de commande (permet de désaturer plus vite) */
				if(thiss->current_cmd != config->way0_max_duty && thiss->current_cmd != config->way1_max_duty)
				{
					thiss->integrator += error; //la multiplication par la période se fait après pour éviter que l'incrément soit nul
				}

				differential = error - thiss->previous_error;

				// Asservissement PID
				// Chaque coefficient multiplié par  1024 d'où les divisions

				if(thiss->config.double_PID == FALSE || thiss->wanted_pos < pos){

					computed_cmd = 	(config->Kp * error / 1024)
								+ ((config->Ki * thiss->integrator * DCM_TIME_PERIOD) >> 20)
								+ (((config->Kd * differential) / DCM_TIME_PERIOD) >> 10);
				}else{

					computed_cmd = 	(config->Kp2 * error >> 10)
								+ ((config->Ki2 * thiss->integrator * DCM_TIME_PERIOD) >> 20)
								+ (((config->Kd2 * differential) / DCM_TIME_PERIOD) >> 10);
				}

				// Sens et saturation
				if (computed_cmd > 0)
				{
					DCM_setWay(dc_motor_id, 1);
					if (computed_cmd > config->way0_max_duty)
						thiss->current_cmd = config->way0_max_duty;
					else
						thiss->current_cmd = (Uint8)computed_cmd;
				}
				else
				{
					DCM_setWay(dc_motor_id, 0);
					computed_cmd = -computed_cmd;
					if (computed_cmd > config->way1_max_duty)
						thiss->current_cmd = config->way1_max_duty;
					else
						thiss->current_cmd = (Uint8)computed_cmd;
				}


				// Application de la commande
				Uint8 real_pwm = thiss->current_cmd;	//On suppose qu'on va prendre la PWM souhaitée.

				if(thiss->time_waiting_limit_pwm != 0)	//on est en 'début d'ordre', une nouvelle consigne est récemment arrivée
				{
					if(absolute(thiss->current_cmd - thiss->last_cmd) > MAX_STEP_OF_PWM)
					{		//Si la dernière PWM est LOIN de la PWM souhaitée... alors on s'en rapproche DOUCEMENT.
						if(thiss->current_cmd > thiss->last_cmd)
							real_pwm = thiss->last_cmd + MAX_STEP_OF_PWM;		//Ca monte.. On s'en rapproche en montant.
						else
							real_pwm = thiss->last_cmd - MAX_STEP_OF_PWM;		//Ca descend..On s'en rapproche en descendant.
					}
				}

				if(thiss->time_waiting_limit_pwm > DCM_TIME_PERIOD)
					thiss->time_waiting_limit_pwm -= DCM_TIME_PERIOD;
				else
					thiss->time_waiting_limit_pwm = 0;

				thiss->last_cmd = real_pwm;
				thiss->previous_error = error;

				PWM_run(real_pwm, config->pwm_number);
			}

		}
	}
}

void DCM_reset_integrator(){
	Uint8 i;
	for (i=0;i<DCM_NUMBER;i++)
	{
		DCMotor_t* thiss = &(DCMotors[i]);
		thiss->integrator = 0;
		thiss->cmd_time = 0;
	}
}

#endif /* def USE_DCMOTOR2 */
