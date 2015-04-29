/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_DCMotor.c
 *	Package : Qualit� Soft
 *	Description : Gestion des moteurs � courant continu asservis en position
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#include "QS_DCMotor2.h"
#ifdef USE_DCMOTOR2

#include "QS_pwm.h"
#ifdef USE_CAN
	#include "QS_can.h"
#endif
#include "QS_CANmsgList.h"


	#define PWM_LIMIT_ON_CHANGE		20


	typedef enum
	{
		INITIALIZED,
		NOT_INITIALIZED,
		STOPPED
	}init_state_e;

/*-----------------------------------------
		Variables globales priv�es
-----------------------------------------*/

	typedef struct
	{
		DCMotor_config_t config;	// la config du DCMotor
		DCM_working_state_e cmd_state;	// Etat de la derni�re commande: Position atteinte, en mouvement, timeout
		Sint32 integrator;			// Integrateur pour le PID
		Uint16 cmd_time;			// temps depuis la reception de la commande, en ms
		Uint8 posToGo;				// consigne, en numero de position actionneur
		Uint8 current_cmd;			// commande PWM actuelle
		Uint16 wanted_pos;			// Position voulue
		Sint16 previous_error;		// valeur de l'erreur � l'appel pr�c�dent du PID, pour calcul du terme deriv�
		init_state_e init_state;	// flag indiquant si le moteur a �t� configur�
		Uint8 time_waiting_limit_pwm;// temps en multiple de DCM_TIME_PERIOD qu'on attend avant de relacher la limite de pwm
	}DCMotor_t;

	static DCMotor_t DCMotors[DCM_NUMBER];

	static bool_e DCM_initialized = FALSE;

/*-----------------------------------------
		Prototypes des fonctions priv�es
-----------------------------------------*/
	// Accesseur
	static void DCM_setWay(Uint8 dc_motor_id, Uint8 value);
	static Uint8 DCM_getWay(Uint8 dc_motor_id);
	// Envoie un message CAN pour informer de la position des ascenseurs
	//static void DCM_sendCAN(Uint8 dc_motor_id, Uint8 pos, Uint16 posInUnits, Uint16 error);
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
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state==STOPPED));
	return this->cmd_state;
}


// configurer un moteur CC apr�s initialisation du module QS
void DCM_config (Uint8 dc_motor_id, DCMotor_config_t* config)
{
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	this->config = *config;
	this->cmd_state=DCM_IDLE;
	this->integrator=0;
	this->cmd_time=0;
	this->posToGo=0;
	this->current_cmd=0;
	this->wanted_pos=0;
	this->previous_error=0;

	this->init_state=INITIALIZED;
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
	DCMotor_config_t* this = &(DCMotors[dc_motor_id].config);
	assert((DCMotors[dc_motor_id].init_state == INITIALIZED) || (DCMotors[dc_motor_id].init_state==STOPPED));
	if(value){
		if(this->inverseDirection)
			GPIO_ResetBits(this->way_latch, this->way_bit_number);
		else
			GPIO_SetBits(this->way_latch, this->way_bit_number);
	}else{
		if(this->inverseDirection)
			GPIO_SetBits(this->way_latch, this->way_bit_number);
		else
			GPIO_ResetBits(this->way_latch, this->way_bit_number);
	}
}

static Uint8 DCM_getWay(Uint8 dc_motor_id)
{
	DCMotor_config_t* this = &(DCMotors[dc_motor_id].config);
	assert((DCMotors[dc_motor_id].init_state == INITIALIZED) || (DCMotors[dc_motor_id].init_state==STOPPED));
	return GPIO_ReadInputDataBit(this->way_latch, this->way_bit_number);
}

void DCM_setWayDirection(Uint8 dc_motor_id, bool_e inverse){
	DCMotor_config_t* this = &(DCMotors[dc_motor_id].config);
	assert((DCMotors[dc_motor_id].init_state == INITIALIZED) || (DCMotors[dc_motor_id].init_state==STOPPED));
	this->inverseDirection = inverse;
}



/*-----------------------------------------
		Ordre de d�placement
-----------------------------------------*/
void DCM_goToPos(Uint8 dc_motor_id, Uint8 pos)
{
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state==STOPPED));
	this->posToGo = pos;
	this->cmd_time = 0;
	this->cmd_state = DCM_WORKING;
}

/*-----------------------------------------
		Changement de la valeur d'une position
-----------------------------------------*/
void DCM_setPosValue(Uint8 dc_motor_id, Uint8 pos_to_update, Sint16 value_pos, Uint16 value_speed) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state==STOPPED));
	this->config.pos[pos_to_update] = value_pos;
	this->config.speed[pos_to_update] = value_speed;
	if(this->posToGo == pos_to_update) {
		this->cmd_time = 0;
		this->cmd_state = DCM_WORKING;
	}
}

/*-----------------------------------------
		R�cup�ration de la valeur d'une position
-----------------------------------------*/
Sint16 DCM_getPosValue(Uint8 dc_motor_id, Uint8 pos_to_get) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state==STOPPED));
	return this->config.pos[pos_to_get];
}

/*-----------------------------------------
		R�cup�ration de la vitesse pour une position
-----------------------------------------*/
Uint16 DCM_getPosSpeed(Uint8 dc_motor_id, Uint8 pos_to_get) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state==STOPPED));
	return this->config.speed[pos_to_get];
}

/*-----------------------------------------
		Change les coefs d'asservissement.
-----------------------------------------*/
void DCM_setCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state == STOPPED));
	init_state_e previousState = this->init_state;

	this->init_state = STOPPED; //Aucun calcul d'asservissement ne doit �tre fait pendant ce temps

	this->config.Kp = Kp;
	this->config.Ki = Ki;
	this->config.Kd = Kd;

	this->init_state = previousState;
}

/*-----------------------------------------
		R�cup�re les coefs d'asservissement.
-----------------------------------------*/
void DCM_getCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state == STOPPED));

	if(Kp) *Kp = this->config.Kp;
	if(Ki) *Ki = this->config.Ki;
	if(Kd) *Kd = this->config.Kd;
}

/*-----------------------------------------
  Change les double coefs d'asservissement.
-----------------------------------------*/
void DCM_setDoubleCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd, Sint16 Kp2, Sint16 Ki2, Sint16 Kd2){
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state == STOPPED));
	init_state_e previousState = this->init_state;

	this->init_state = STOPPED; //Aucun calcul d'asservissement ne doit �tre fait pendant ce temps

	this->config.Kp = Kp;
	this->config.Ki = Ki;
	this->config.Kd = Kd;
	this->config.Kp2 = Kp2;
	this->config.Ki2 = Ki2;
	this->config.Kd2 = Kd2;

	this->init_state = previousState;
}

/*-----------------------------------------
  R�cup�re les double coefs d'asservissement.
-----------------------------------------*/
void DCM_getDoubleCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd, Sint16* Kp2, Sint16* Ki2, Sint16* Kd2){
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state == STOPPED));

	if(Kp) *Kp = this->config.Kp;
	if(Ki) *Ki = this->config.Ki;
	if(Kd) *Kd = this->config.Kd;
	if(Kp2) *Kp2 = this->config.Kp2;
	if(Ki2) *Ki2 = this->config.Ki2;
	if(Kd2) *Kd2 = this->config.Kd2;
}

/*-----------------------------------------
  Change si le moteur fonctionne en double PID
-----------------------------------------*/
void DCM_setDoublePID(Uint8 dc_motor_id, bool_e double_PID){
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state == STOPPED));

	init_state_e previousState = this->init_state;
	this->init_state = STOPPED; //Aucun calcul d'asservissement ne doit �tre fait pendant ce temps

	this->config.double_PID = double_PID;

	this->init_state = previousState;
}

/*-----------------------------------------
  Change si le moteur fonctionne en double PID
-----------------------------------------*/
bool_e DCM_getDoublePID(Uint8 dc_motor_id){
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state == STOPPED));

	return this->config.double_PID;
}

/*-------------------------------------------
  R�cup�re les coefficients pwm
--------------------------------------------*/
void DCM_getPwmWay(Uint8 dc_motor_id, Uint8 *way0_max_duty, Uint8 *way1_max_duty){
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	*way0_max_duty = this->config.way0_max_duty;
	*way1_max_duty = this->config.way1_max_duty;
}

/*-------------------------------------------
  Change les coefficients pwm
--------------------------------------------*/
void DCM_setPwmWay(Uint8 dc_motor_id, Uint8 way0_max_duty, Uint8 way1_max_duty){
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	this->config.way0_max_duty = way0_max_duty;
	this->config.way1_max_duty = way1_max_duty;
}

/*-----------------------------------------
		Arret de l'asservissement d'un actionneur
-----------------------------------------*/
void DCM_stop(Uint8 dc_motor_id)
{
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	if(this->init_state == INITIALIZED)
	{
		this->init_state = STOPPED;
		this->cmd_state = DCM_IDLE;
		PWM_stop(this->config.pwm_number);
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
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	if(this->init_state == STOPPED)
	{
		this->integrator = 0;
		this->cmd_time = 0;
		this->cmd_state = DCM_WORKING;
		this->init_state = INITIALIZED;
		this->wanted_pos = this->config.sensor_read();
	}
	this->time_waiting_limit_pwm = DCM_TIME_PERIOD*5;
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

	DCMotor_t* this;
	DCMotor_config_t* config;
	Uint8 dc_motor_id;
	Sint32 differential;
	Sint32 computed_cmd;
	Sint16 error;

	for (dc_motor_id = 0; dc_motor_id < DCM_NUMBER; dc_motor_id++)
	{
		this = &(DCMotors[dc_motor_id]);
		config = &(this->config);

		if (this->init_state == INITIALIZED)
		{
			// Gestion de la vitesse de la mise en position de l'actionneur
			if(config->speed[this->posToGo] == 0)
				this->wanted_pos = config->pos[this->posToGo];
			if(this->wanted_pos > config->pos[this->posToGo]){
				this->wanted_pos -= config->speed[this->posToGo];
				if(this->wanted_pos < config->pos[this->posToGo])
					this->wanted_pos = config->pos[this->posToGo];
			}else if(this->wanted_pos < config->pos[this->posToGo]){
				this->wanted_pos += config->speed[this->posToGo];
				if(this->wanted_pos > config->pos[this->posToGo])
					this->wanted_pos = config->pos[this->posToGo];
			}

			// Acquisition de la position pour la d�tection de l'arr�t du moteur
			error = this->wanted_pos-(config->sensor_read)();

			//Gestion des changements d'�tats
			switch(this->cmd_state) {
				case DCM_WORKING:
					this->cmd_time += DCM_TIME_PERIOD;
					if(absolute(error) < (Sint16)config->epsilon && absolute(this->previous_error) < (Sint16)config->epsilon)
						this->cmd_state = DCM_IDLE;
					else if(config->timeout && this->cmd_time >= config->timeout)
						this->cmd_state = DCM_TIMEOUT;
					break;

				case DCM_IDLE:
					this->cmd_time = 0;
					if(absolute(error) >= (Sint16)config->epsilon)
						this->cmd_state = DCM_WORKING;
					break;

				case DCM_TIMEOUT:
					break;
			}

			//Gestion des actions dans les �tats
			if(this->cmd_state == DCM_TIMEOUT) {
				PWM_stop(config->pwm_number);
			} else {

				if(this->config.double_PID == FALSE ||  this->wanted_pos < (config->sensor_read)()){

					// Asservissement PID
					/* integration si on n'est pas en saturation de commande (permet de d�saturer plus vite) */
					/* l'expression si dessous vaut pour erreur = consigne-position */
					if(!(		( (DCM_getWay(dc_motor_id)) && (this->current_cmd == config->way1_max_duty) && (!((config->Ki>0) ^ (error>0))))
							||	(!(DCM_getWay(dc_motor_id)) && (this->current_cmd == config->way0_max_duty) && (!((config->Ki>0) ^ (error<0))))
						))
					{
						this->integrator += error;
						//la multiplication par la p�riode se fait apr�s pour �viter que l'incr�ment soit nul
					}
					differential = error - this->previous_error;
					/*computed_cmd = 	(__builtin_mulss(config->Kp, error) >> 10) // / 1024)
									+ ((__builtin_mulss(config->Ki, this->integrator) * DCM_TIMER_PERIOD) >> 20) // / 1048576)
									+ (__builtin_divsd(__builtin_mulss(config->Kd, differential), (DCM_TIMER_PERIOD*1024)));
					 */
					//TODO: clean �a, pas de Kd quand on est a la bonne position
	//				if(abs(error) < (Sint16)config->epsilon && abs(this->previous_error) < (Sint16)config->epsilon) {
	//					computed_cmd = 	((Sint32)(config->Kp * (Sint32)error) / 1024)
	//								+ (((Sint32)(config->Ki) * this->integrator * DCM_TIMER_PERIOD) / 1048576);
	//				} else {
						computed_cmd = 	((Sint32)(config->Kp * (Sint32)error) / 1024)
									+ (((Sint32)(config->Ki) * this->integrator * DCM_TIME_PERIOD) / 1048576)
									+ (((Sint32)(config->Kd) * differential)/DCM_TIME_PERIOD) / 1024;
	//				}
				}else{
					// Asservissement PID
					/* integration si on n'est pas en saturation de commande (permet de d�saturer plus vite) */
					/* l'expression si dessous vaut pour erreur = consigne-position */
					if(!(		( (DCM_getWay(dc_motor_id)) && (this->current_cmd == config->way1_max_duty) && (!((config->Ki2>0) ^ (error>0))))
							||	(!(DCM_getWay(dc_motor_id)) && (this->current_cmd == config->way0_max_duty) && (!((config->Ki2>0) ^ (error<0))))
						))
					{
						this->integrator += error;
						//la multiplication par la p�riode se fait apr�s pour �viter que l'incr�ment soit nul
					}
					differential = error - this->previous_error;
					/*computed_cmd = 	(__builtin_mulss(config->Kp, error) >> 10) // / 1024)
									+ ((__builtin_mulss(config->Ki, this->integrator) * DCM_TIMER_PERIOD) >> 20) // / 1048576)
									+ (__builtin_divsd(__builtin_mulss(config->Kd, differential), (DCM_TIMER_PERIOD*1024)));
					 */
					//TODO: clean �a, pas de Kd quand on est a la bonne position
	//				if(abs(error) < (Sint16)config->epsilon && abs(this->previous_error) < (Sint16)config->epsilon) {
	//					computed_cmd = 	((Sint32)(config->Kp * (Sint32)error) / 1024)
	//								+ (((Sint32)(config->Ki) * this->integrator * DCM_TIMER_PERIOD) / 1048576);
	//				} else {
						computed_cmd = 	((Sint32)(config->Kp2 * (Sint32)error) / 1024)
									+ (((Sint32)(config->Ki2) * this->integrator * DCM_TIME_PERIOD) / 1048576)
									+ (((Sint32)(config->Kd2) * differential)/DCM_TIME_PERIOD) / 1024;
	//				}
				}

				this->previous_error = error;

				// Sens et saturation
				if (computed_cmd > 0)
				{
					DCM_setWay(dc_motor_id, 1);
					if (computed_cmd > config->way0_max_duty)
						this->current_cmd = config->way0_max_duty;
					else this->current_cmd = (Uint8)computed_cmd;
				}
				else
				{
					DCM_setWay(dc_motor_id, 0);
					computed_cmd = -computed_cmd;
					if (computed_cmd > config->way1_max_duty)
						this->current_cmd = config->way1_max_duty;
					else this->current_cmd = (Uint8)computed_cmd;
				}


				// application de la commande

				Uint8 real_pwm;
				if(this->time_waiting_limit_pwm == 0 || this->current_cmd < PWM_LIMIT_ON_CHANGE)		// Si la limite de pwm est pass� on applique la commande
					real_pwm = this->current_cmd;
				else
					real_pwm = PWM_LIMIT_ON_CHANGE;

				if(this->time_waiting_limit_pwm > DCM_TIME_PERIOD)
					this->time_waiting_limit_pwm -= DCM_TIME_PERIOD;
				else
					this->time_waiting_limit_pwm = 0;

				PWM_run(real_pwm, config->pwm_number);
			}
		}
	}
}


/*-----------------------------------------
Envoie un retour CAN / signal de fin de parcours
-----------------------------------------*/
//D�sactiv�
/*
static void DCM_sendCAN(Uint8 dc_motor_id, Uint8 pos, Uint16 posInUnits, Uint16 error)
{
	#ifdef USE_CAN
		// Pr�paration
		CAN_msg_t msg;
		msg.sid = ACT_DCM_POS;
		msg.data[0] = dc_motor_id;
		msg.data[1] = pos;
		msg.size = 2;
		// Envoi
		CAN_send(&msg);
	#endif

}
//*/

#endif /* def USE_DCMOTOR2 */
