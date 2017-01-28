/*
 *	Club Robot ESEO 2008 - 2010
 *	Harry & Anne
 *
 *	Fichier : QS_DCMotorSpeed.c
 *	Package : Qualité Soft
 *	Description : Gestion des moteurs à courant continu asservis en vitesse
 *  Auteur : Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#include "QS_DCMotorSpeed.h"
#ifdef USE_DC_MOTOR_SPEED

	#include "QS_pwm.h"

	#define MAX_STEP_OF_PWM		20   //[%/10ms]
	#define SPEED_DETECT_LARGE_EPSILON	2


	typedef enum{
		DC_MOTOR_SPEED_NOT_INITIALIZED = 0,
		DC_MOTOR_SPEED_INITIALIZED,
		DC_MOTOR_SPEED_STOPPED
	}DC_MOTOR_SPEED_init_state_e;

/*-----------------------------------------
		Variables globales privées
-----------------------------------------*/

	typedef struct
	{
		DC_MOTOR_SPEED_init_state_e init_state;	// flag indiquant si le moteur a été configuré
		DC_MOTOR_SPEED_config_t config;			// la config du DC_MOTOR_SPEED
		DC_MOTOR_SPEED_state_e cmd_state;		// Etat de la commande

		Sint32 integrator;						// Integrateur pour le PID
		Uint16 cmd_time;						// temps depuis la reception de la commande, en ms

		Uint8 current_pwm_cmd;					// commande PWM actuelle

		DC_MOTOR_SPEED_speed last_speed;		// dernière vitesse
		DC_MOTOR_SPEED_speed wantedSpeed;		// consigne de vitesse
		DC_MOTOR_SPEED_speed previous_error;	// valeur de l'erreur à l'appel précédent du PID, pour calcul du terme derivé
	}DC_MOTOR_SPEED_t;

	static DC_MOTOR_SPEED_t dcMotorSpeed[DC_MOTOR_SPEED_NUMBER];

	static bool_e DC_MOTOR_SPEED_initialized = FALSE;

	static void DC_MOTOR_SPEED_setWay(DC_MOTOR_SPEED_id id, Uint8 value);
	static Uint8 DC_MOTOR_SPEED_getWay(DC_MOTOR_SPEED_id id);

	/*-----------------------------------------
			Initialisation
	-----------------------------------------*/
	void DC_MOTOR_SPEED_init()
	{
		if(DC_MOTOR_SPEED_initialized)
			return;
		DC_MOTOR_SPEED_initialized = TRUE;

		PWM_init();
		// Initialisation
		DC_MOTOR_SPEED_uninitialize_all();
	}

	DC_MOTOR_SPEED_state_e DC_MOTOR_SPEED_get_state(DC_MOTOR_SPEED_id id)
	{
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert((thiss->init_state == DC_MOTOR_SPEED_INITIALIZED) || (thiss->init_state == DC_MOTOR_SPEED_STOPPED));
		return thiss->cmd_state;
	}


	// configurer un moteur CC après initialisation du module QS
	void DC_MOTOR_SPEED_config(DC_MOTOR_SPEED_id id, DC_MOTOR_SPEED_config_t* config)
	{
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		thiss->config = *config;
		thiss->cmd_state = DC_MOTOR_SPEED_IDLE;
		thiss->integrator = 0;
		thiss->cmd_time = 0;
		thiss->last_pwm_cmd = 0;
		thiss->current_pwm_cmd = 0;
		thiss->last_speed = 0;
		thiss->wantedSpeed = 0;
		thiss->previous_error = 0;

		thiss->init_state=DC_MOTOR_SPEED_INITIALIZED;
	}

	static void DC_MOTOR_SPEED_uninitialize_all()
	{
		Uint8 i;
		for (i=0;i<DC_MOTOR_SPEED_NUMBER;i++)
		{
			(dcMotorSpeed[i]).init_state = DC_MOTOR_SPEED_NOT_INITIALIZED;
		}
	}


	/*-----------------------------------------
		Setter pour le sens des moteurs
	-----------------------------------------*/
	static void DC_MOTOR_SPEED_setWay(DC_MOTOR_SPEED_id id, Uint8 value)
	{
		DC_MOTOR_SPEED_config_t* thiss = &(dcMotorSpeed[id].config);
		assert((dcMotorSpeed[id].init_state == DC_MOTOR_SPEED_INITIALIZED) || (dcMotorSpeed[id].init_state == DC_MOTOR_SPEED_STOPPED));
		if(value ^ thiss->inverse_way)
			GPIO_SetBits(thiss->way_latch, thiss->way_bit_number);
		else
			GPIO_ResetBits(thiss->way_latch, thiss->way_bit_number);
	}

	static Uint8 DC_MOTOR_SPEED_getWay(DC_MOTOR_SPEED_id id)
	{
		DC_MOTOR_SPEED_config_t* thiss = &(dcMotorSpeed[id].config);
		assert((dcMotorSpeed[id].init_state == DC_MOTOR_SPEED_INITIALIZED) || (dcMotorSpeed[id].init_state == DC_MOTOR_SPEED_STOPPED));
		return GPIO_ReadInputDataBit(thiss->way_latch, thiss->way_bit_number);
	}

	void DC_MOTOR_SPEED_setInverseWay(DC_MOTOR_SPEED_id id, bool_e inverse){
		DC_MOTOR_SPEED_config_t* thiss = &(dcMotorSpeed[id].config);
		assert((dcMotorSpeed[id].init_state == DC_MOTOR_SPEED_INITIALIZED) || (dcMotorSpeed[id].init_state == DC_MOTOR_SPEED_STOPPED));
		thiss->inverse_way = inverse;
	}


	/*-----------------------------------------
			Ordre de déplacement
	-----------------------------------------*/
	DC_MOTOR_SPEED_speed DC_MOTOR_SPEED_getSpeed(DC_MOTOR_SPEED_id id){
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert((thiss->init_state == DC_MOTOR_SPEED_INITIALIZED) || (thiss->init_state == DC_MOTOR_SPEED_STOPPED));
		return thiss->wantedSpeed;
	}

	void DC_MOTOR_SPEED_setSpeed(DC_MOTOR_SPEED_id id, DC_MOTOR_SPEED_speed speed){
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert((thiss->init_state == DC_MOTOR_SPEED_INITIALIZED) || (thiss->init_state == DC_MOTOR_SPEED_STOPPED));
		thiss->wantedSpeed = speed;
		thiss->cmd_time = 0;
	}

	/*-----------------------------------------
			Change les coefs d'asservissement.
	-----------------------------------------*/
	void DC_MOTOR_SPEED_setCoefs(DC_MOTOR_SPEED_id id, Sint16 Kp, Sint16 Ki, Sint16 Kd, Sint16 Kv) {
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert((thiss->init_state == DC_MOTOR_SPEED_INITIALIZED) || (thiss->init_state == DC_MOTOR_SPEED_STOPPED));
		DC_MOTOR_SPEED_state_e previousState = thiss->cmd_state;
		thiss->cmd_state = DC_MOTOR_SPEED_CONFIG; //Aucun calcul d'asservissement ne doit être fait pendant ce temps

		thiss->config.Kp = Kp;
		thiss->config.Ki = Ki;
		thiss->config.Kd = Kd;
		thiss->config.Kv = Kv;

		thiss->init_state = previousState;
	}

	/*-----------------------------------------
			Récupère les coefs d'asservissement.
	-----------------------------------------*/
	void DC_MOTOR_SPEED_getCoefs(DC_MOTOR_SPEED_id id, Sint16* Kp, Sint16* Ki, Sint16* Kd, Sint16* Kv) {
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert((thiss->init_state == DC_MOTOR_SPEED_INITIALIZED) || (thiss->init_state == DC_MOTOR_SPEED_STOPPED));

		if(Kp) *Kp = thiss->config.Kp;
		if(Ki) *Ki = thiss->config.Ki;
		if(Kd) *Kd = thiss->config.Kd;
		if(Kd) *Kv = thiss->config.Kv;
	}

	/*-------------------------------------------
	  Récupère les coefficients pwm
	--------------------------------------------*/
	void DC_MOTOR_SPEED_setMaxPwm(DC_MOTOR_SPEED_id id, Uint8 max_duty){
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		thiss->config.max_duty = max_duty;
	}

	void DC_MOTOR_SPEED_getMaxPwm(DC_MOTOR_SPEED_id id, Uint8 *max_duty){
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		*max_duty = thiss->config.max_duty;
	}

	/*-----------------------------------------
			Arret de l'asservissement d'un actionneur
	-----------------------------------------*/
	void DC_MOTOR_SPEED_stop(DC_MOTOR_SPEED_id id)
	{
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		if(thiss->init_state == DC_MOTOR_SPEED_INITIALIZED)
		{
			thiss->cmd_state = DC_MOTOR_SPEED_IDLE;
			thiss->init_state = DC_MOTOR_SPEED_STOPPED;

			PWM_stop(thiss->config.pwm_number);
		}
	}


	/*-----------------------------------------
			Arret de tous les asservissements
	-----------------------------------------*/
	void DC_MOTOR_SPEED_stop_all()
	{
		DC_MOTOR_SPEED_id i;
		for (i=0;i<DC_MOTOR_SPEED_NUMBER;i++)
		{
			DC_MOTOR_SPEED_stop(i);
		}
	}

	/*-----------------------------------------
			Reactivation de l'asservissement d'un actionneur
	-----------------------------------------*/
	void DC_MOTOR_SPEED_restart(DC_MOTOR_SPEED_id id)
	{
		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		if(thiss->init_state == DC_MOTOR_SPEED_STOPPED)
		{
			thiss->integrator = 0;
			thiss->cmd_time = 0;
			thiss->cmd_state = DC_MOTOR_SPEED_WORKING;
			thiss->init_state = DC_MOTOR_SPEED_INITIALIZED;
		}
	}

	/*-----------------------------------------
			Reactivation de tous les asservissements
	-----------------------------------------*/
	void DC_MOTOR_SPEED_restart_all()
	{
		Uint8 i;
		for (i=0;i<DC_MOTOR_SPEED_NUMBER;i++)
		{
			DC_MOTOR_SPEED_restart(i);
		}
	}


	/*-----------------------------------------
		Interruption de commande
	-----------------------------------------*/
	void DC_MOTOR_SPEED_process_it()
	{
		if(DC_MOTOR_SPEED_initialized == FALSE)
			return;

		DC_MOTOR_SPEED_t* thiss;
		DC_MOTOR_SPEED_config_t* config;
		DC_MOTOR_SPEED_id id;
		Sint32 differential;
		Sint32 computed_cmd;
		Sint32 error;
		DC_MOTOR_SPEED_speed speed;

		for (id=0; id<DC_MOTOR_SPEED_NUMBER; id++)
		{
			thiss = &(dcMotorSpeed[id]);
			config = &(thiss->config);

			if (thiss->init_state == DC_MOTOR_SPEED_INITIALIZED)
			{

				// Acquisition de la position pour la détection de l'arrêt du moteur
				speed = (config->sensorRead)();
				error = thiss->wantedSpeed-speed;

				//Gestion des changements d'états
				switch(thiss->cmd_state) {
					case DC_MOTOR_SPEED_WORKING:
						thiss->cmd_time += DC_MOTOR_SPEED_TIME_PERIOD;
						if(absolute(error) < (Sint16)config->epsilon && absolute(thiss->previous_error) < (Sint16)config->epsilon)
							thiss->cmd_state = DC_MOTOR_SPEED_IDLE;
						else if(config->timeout && thiss->cmd_time >= config->timeout){
							if(absolute(error) < (Sint16)config->large_epsilon && absolute(thiss->previous_error) < (Sint16)config->large_epsilon)
								thiss->cmd_state = DC_MOTOR_SPEED_IDLE;
							else
								thiss->cmd_state = DC_MOTOR_SPEED_TIMEOUT;
						}
						break;

					case DC_MOTOR_SPEED_IDLE:
						thiss->cmd_time = 0;
						if(absolute(error) < (Sint16)config->large_epsilon)
							thiss->cmd_state = DC_MOTOR_SPEED_WORKING;
						break;

					case DC_MOTOR_SPEED_TIMEOUT:
						break;
				}

				//Gestion des actions dans les états
				if(thiss->cmd_state == DC_MOTOR_SPEED_TIMEOUT || absolute(thiss->wanted_pos - pos) < config->dead_zone/2){
					PWM_stop(config->pwm_number);
				}else{

					/* Integration si on n'est pas en saturation de commande (permet de désaturer plus vite) */
					if(thiss->current_pwm_cmd != config->max_duty)
					{
						thiss->integrator += error; //la multiplication par la période se fait après pour éviter que l'incrément soit nul
					}

					differential = error - thiss->previous_error;

					// Asservissement PID
					// Chaque coefficient multiplié par  1024 d'où les divisions

					computed_cmd = 	(config->Kp * error / 1024)
								+ ((config->Ki * thiss->integrator * DC_MOTOR_SPEED_TIME_PERIOD) >> 20)
								+ (((config->Kd * differential) / DC_MOTOR_SPEED_TIME_PERIOD) >> 10)
								+ (config->Kv * speed);

					// Sens et saturation
					if (computed_cmd > 0)
						DC_MOTOR_SPEED_setWay(id, 1);
					else
						DC_MOTOR_SPEED_setWay(id, 0);

					if (computed_cmd > config->max_duty)
						thiss->current_pwm_cmd = config->max_duty;
					else
						thiss->current_pwm_cmd = (Uint8)computed_cmd;


					// Application de la commande
					Uint8 real_pwm = thiss->current_pwm_cmd;	//On suppose qu'on va prendre la PWM souhaitée.

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

					if(thiss->time_waiting_limit_pwm > DC_MOTOR_SPEED_TIME_PERIOD)
						thiss->time_waiting_limit_pwm -= DC_MOTOR_SPEED_TIME_PERIOD;
					else
						thiss->time_waiting_limit_pwm = 0;

					thiss->last_cmd = real_pwm;
					thiss->previous_error = error;

					PWM_run(real_pwm, config->pwm_number);
				}

			}
		}
	}

	void DC_MOTOR_SPEED_reset_integrator(){
		DC_MOTOR_SPEED_id i;
		for (i=0;i<DC_MOTOR_SPEED_NUMBER;i++)
		{
			DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[i]);
			thiss->integrator = 0;
			thiss->cmd_time = 0;
		}
	}

#endif /* def USE_DCMOTOR2 */
