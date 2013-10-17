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
#include "QS_timer.h"
#ifdef USE_CAN
	#include "QS_can.h"
#endif
#include "QS_CANmsgList.h"



/*-----------------------------------------
		Selection du timer
-----------------------------------------*/
#if DCM_TIMER == 1
		#define DCM_TIMER_RUN 	TIMER1_run
		#define DCM_TIMER_IT	_T1Interrupt
		#define DCM_TIMER_FLAG	IFS0bits.T1IF
#elif DCM_TIMER == 2
		#define DCM_TIMER_RUN	TIMER2_run
		#define DCM_TIMER_IT	_T2Interrupt
		#define DCM_TIMER_FLAG	IFS0bits.T2IF
#elif DCM_TIMER == 3
		#define DCM_TIMER_RUN	TIMER3_run
		#define DCM_TIMER_IT	_T3Interrupt
		#define DCM_TIMER_FLAG	IFS0bits.T3IF
#elif DCM_TIMER == 4
		#define DCM_TIMER_RUN	TIMER4_run
		#define DCM_TIMER_IT	_T4Interrupt
		#define DCM_TIMER_FLAG	IFS1bits.T4IF
#else
	#error "DCM_TIMER doit etre 1 2 3 ou 4"
#endif /* DCM_TIMER == n */


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
		Uint8 current_cmd;			// commande PWM actuelle
		Sint16 previous_error;		// valeur de l'erreur à l'appel précédent du PID, pour calcul du terme derivé
		init_state_e init_state;	// flag indiquant si le moteur a été configuré
	}DCMotor_t;

	static DCMotor_t DCMotors[DCM_NUMBER];

/*-----------------------------------------
		Prototypes des fonctions privées
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
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	PWM_init();
	// Initialisation
	DCM_uninitialize_all();
	// Lancement de l'interruption de commande
	DCM_TIMER_RUN(DCM_TIMER_PERIOD);
}

DCM_working_state_e DCM_get_state (Uint8 dc_motor_id)
{
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state==STOPPED));
	return this->cmd_state;
}


// configurer un moteur CC après initialisation du module QS
void DCM_config (Uint8 dc_motor_id, DCMotor_config_t* config)
{
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	this->config = *config;
	this->cmd_state=DCM_IDLE;
	this->integrator=0;
	this->cmd_time=0;
	this->posToGo=0;
	this->current_cmd=0;
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
	if (value)
		BIT_SET(*(this->way_latch), this->way_bit_number);
	else
		BIT_CLR(*(this->way_latch), this->way_bit_number);
}	

static Uint8 DCM_getWay(Uint8 dc_motor_id)
{
	DCMotor_config_t* this = &(DCMotors[dc_motor_id].config);
	assert((DCMotors[dc_motor_id].init_state == INITIALIZED) || (DCMotors[dc_motor_id].init_state==STOPPED));
	return BIT_TEST(*(this->way_latch), this->way_bit_number);
}



/*-----------------------------------------
		Ordre de déplacement
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
void DCM_setPosValue(Uint8 dc_motor_id, Uint8 pos_to_update, Sint16 new_value) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state==STOPPED));
	this->config.pos[pos_to_update] = new_value;
	if(this->posToGo == pos_to_update) {
		this->cmd_time = 0;
		this->cmd_state = DCM_WORKING;
	}
}

/*-----------------------------------------
		Récupération de la valeur d'une position
-----------------------------------------*/
Sint16 DCM_getPosValue(Uint8 dc_motor_id, Uint8 pos_to_get) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state==STOPPED));
	return this->config.pos[pos_to_get];
}

/*-----------------------------------------
        Change les coefs d'asservissement.
-----------------------------------------*/
void DCM_setCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state == STOPPED));
	init_state_e previousState = this->init_state;

	this->init_state = STOPPED; //Aucun calcul d'asservissement ne doit être fait pendant ce temps

	this->config.Kp = Kp;
	this->config.Ki = Ki;
	this->config.Kd = Kd;

	this->init_state = previousState;
}

/*-----------------------------------------
        Récupère les coefs d'asservissement.
-----------------------------------------*/
void DCM_getCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd) {
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	assert((this->init_state == INITIALIZED) || (this->init_state == STOPPED));

	if(Kp) *Kp = this->config.Kp;
	if(Ki) *Ki = this->config.Ki;
	if(Kd) *Kd = this->config.Kd;
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
void _ISR DCM_TIMER_IT()
{
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
			// Acquisition de la position pour la détection de l'arrêt du moteur
			error = config->pos[this->posToGo]-(config->sensor_read)();

			//Gestion des changements d'états
			switch(this->cmd_state) {
				case DCM_WORKING:
					this->cmd_time += DCM_TIMER_PERIOD;
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

			//Gestion des actions dans les états
			if(this->cmd_state == DCM_TIMEOUT) {
				PWM_stop(config->pwm_number);
			} else {
				// Asservissement PID
				/* integration si on n'est pas en saturation de commande (permet de désaturer plus vite) */
				/* l'expression si dessous vaut pour erreur = consigne-position */
				if(!(		( (DCM_getWay(dc_motor_id)) && (this->current_cmd == config->way1_max_duty) && (!((config->Ki>0) ^ (error>0))))
						||	(!(DCM_getWay(dc_motor_id)) && (this->current_cmd == config->way0_max_duty) && (!((config->Ki>0) ^ (error<0))))
					))
				{
					this->integrator += error;
					//la multiplication par la période se fait après pour éviter que l'incrément soit nul
				}
				differential = error - this->previous_error;
				/*computed_cmd = 	(__builtin_mulss(config->Kp, error) >> 10) // / 1024)
								+ ((__builtin_mulss(config->Ki, this->integrator) * DCM_TIMER_PERIOD) >> 20) // / 1048576)
								+ (__builtin_divsd(__builtin_mulss(config->Kd, differential), (DCM_TIMER_PERIOD*1024)));
				 */
				//TODO: clean ça, pas de Kd quand on est a la bonne position
//				if(absolute(error) < (Sint16)config->epsilon && absolute(this->previous_error) < (Sint16)config->epsilon) {
//					computed_cmd = 	((Sint32)(config->Kp * (Sint32)error) / 1024)
//								+ (((Sint32)(config->Ki) * this->integrator * DCM_TIMER_PERIOD) / 1048576);
//				} else {
					computed_cmd = 	((Sint32)(config->Kp * (Sint32)error) / 1024)
								+ (((Sint32)(config->Ki) * this->integrator * DCM_TIMER_PERIOD) / 1048576)
								+ (((Sint32)(config->Kd) * differential)/DCM_TIMER_PERIOD) / 1024;
//				}

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
				PWM_run(this->current_cmd, config->pwm_number);
			}
		}	
	}
	// acquittement de l'interruption
	DCM_TIMER_FLAG = 0;
}


/*-----------------------------------------
Envoie un retour CAN / signal de fin de parcours
-----------------------------------------*/
//Désactivé
/*
static void DCM_sendCAN(Uint8 dc_motor_id, Uint8 pos, Uint16 posInUnits, Uint16 error)
{
	#ifdef USE_CAN
		// Préparation
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
