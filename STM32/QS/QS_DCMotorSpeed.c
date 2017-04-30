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

	#define MAX_STEP_OF_PWM				20   //[%/10ms]
	#define SPEED_DETECT_LARGE_EPSILON	2
	#define TIME_OF_MOTOR_STEPPING		50	// [ms]


	typedef enum{
		DC_MOTOR_SPEED_CONFIG_NOT_INITIALIZED = 0,
		DC_MOTOR_SPEED_CONFIG_INITIALIZED,
		DC_MOTOR_SPEED_CONFIG_CHANGING,
	}DC_MOTOR_SPEED_config_state_e;

/*-----------------------------------------
		Variables globales privées
-----------------------------------------*/

	typedef struct
	{
		DC_MOTOR_SPEED_config_state_e configState;	// flag indiquant si le moteur a été configuré
		DC_MOTOR_SPEED_config_t config;				// la config du DC_MOTOR_SPEED
		DC_MOTOR_SPEED_state_e state;				// Etat de la commande

		Sint32 integrator;							// Integrateur pour le PID
		Uint16 time;								// temps depuis la reception de la commande, en ms

		Uint8 currentPwm;							// commande PWM actuelle
		Uint8 lastPwm;								// commande PWM ancienne

		time32_t timeWaitingLimitPwm;				// Temps

		DC_MOTOR_SPEED_speed lastSpeed;				// dernière vitesse
		DC_MOTOR_SPEED_speed wantedSpeed;			// consigne de vitesse
		DC_MOTOR_SPEED_speed previousError;			// valeur de l'erreur à l'appel précédent du PID, pour calcul du terme derivé
		time32_t timeInError;						// Temps du début de l'erreur

		time32_t timeBeginRecovery;					// Temps du début du mode recovery
	}DC_MOTOR_SPEED_t;

	static DC_MOTOR_SPEED_t dcMotorSpeed[DC_MOTOR_SPEED_NUMBER];

	static bool_e DC_MOTOR_SPEED_initialized = FALSE;

	static void DC_MOTOR_SPEED_setWay(DC_MOTOR_SPEED_id id, Uint8 value);
	static Uint8 DC_MOTOR_SPEED_getWay(DC_MOTOR_SPEED_id id);
	static void DC_MOTOR_SPEED_uninitialize_all();

	/*-----------------------------------------
			Initialisation
	-----------------------------------------*/
	void DC_MOTOR_SPEED_init()
	{
		if(DC_MOTOR_SPEED_initialized)
			return;
		DC_MOTOR_SPEED_initialized = TRUE;

		PWM_init();
		DC_MOTOR_SPEED_uninitialize_all();
	}

	DC_MOTOR_SPEED_state_e DC_MOTOR_SPEED_get_state(DC_MOTOR_SPEED_id id){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert(thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED);
		return thiss->state;
	}


	// configurer un moteur CC après initialisation du module QS
	void DC_MOTOR_SPEED_config(DC_MOTOR_SPEED_id id, DC_MOTOR_SPEED_config_t* config){
		assert(id < DC_MOTOR_SPEED_NUMBER);
		assert(config != NULL);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		thiss->config = *config;
		thiss->state = DC_MOTOR_SPEED_IDLE;
		thiss->integrator = 0;
		thiss->time = 0;
		thiss->currentPwm = 0;
		thiss->lastSpeed = 0;
		thiss->wantedSpeed = 0;
		thiss->previousError = 0;
		thiss->timeInError = 0;
		thiss->timeBeginRecovery = 0;

		thiss->configState=DC_MOTOR_SPEED_CONFIG_INITIALIZED;
	}

	static void DC_MOTOR_SPEED_uninitialize_all(){
		Uint8 i;
		for (i=0;i<DC_MOTOR_SPEED_NUMBER;i++)
		{
			(dcMotorSpeed[i]).configState = DC_MOTOR_SPEED_CONFIG_NOT_INITIALIZED;
		}
	}


	/*-----------------------------------------
		Setter pour le sens des moteurs
	-----------------------------------------*/
	static void DC_MOTOR_SPEED_setWay(DC_MOTOR_SPEED_id id, Uint8 value){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_config_t* thiss = &(dcMotorSpeed[id].config);
		assert(dcMotorSpeed[id].configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED);
		if(value)
			GPIO_SetBits(thiss->way_latch, thiss->way_bit_number);
		else
			GPIO_ResetBits(thiss->way_latch, thiss->way_bit_number);
	}

	static Uint8 DC_MOTOR_SPEED_getWay(DC_MOTOR_SPEED_id id){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_config_t* thiss = &(dcMotorSpeed[id].config);
		assert(dcMotorSpeed[id].configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED);
		return GPIO_ReadOutputDataBit(thiss->way_latch, thiss->way_bit_number);
	}

	/*-----------------------------------------
			Ordre de déplacement
	-----------------------------------------*/
	DC_MOTOR_SPEED_speed DC_MOTOR_SPEED_getWantedSpeed(DC_MOTOR_SPEED_id id){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert((thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED));
		return thiss->wantedSpeed;
	}

	DC_MOTOR_SPEED_speed DC_MOTOR_SPEED_getSpeed(DC_MOTOR_SPEED_id id){
			assert(id < DC_MOTOR_SPEED_NUMBER);

			DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
			DC_MOTOR_SPEED_config_t* config = &(thiss->config);
			Sint16 speed = 0;
			assert((thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED));
			if(config->simulateWay){
				if(DC_MOTOR_SPEED_getWay(id)){
					speed = -(config->sensorRead)();
				}else{
					speed = (config->sensorRead)();
				}
			}else{
				speed = (config->sensorRead)();
			}
			return speed;
		}

	void DC_MOTOR_SPEED_setSpeed(DC_MOTOR_SPEED_id id, DC_MOTOR_SPEED_speed speed){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert(thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED);
		thiss->wantedSpeed = speed;
		thiss->time = 0;
		thiss->state = DC_MOTOR_SPEED_INIT_LAUNCH;
	}

	/*-----------------------------------------
			Change les coefs d'asservissement.
	-----------------------------------------*/
	void DC_MOTOR_SPEED_setCoefs(DC_MOTOR_SPEED_id id, Sint16 Kp, Sint16 Ki, Sint16 Kd, Sint16 Kv){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert((thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED));
		DC_MOTOR_SPEED_config_state_e previousConfigState = thiss->configState;

		thiss->configState = DC_MOTOR_SPEED_CONFIG_CHANGING;

		thiss->config.Kp = Kp;
		thiss->config.Ki = Ki;
		thiss->config.Kd = Kd;
		thiss->config.Kv = Kv;

		thiss->configState = previousConfigState;
	}

	/*-----------------------------------------
			Récupère les coefs d'asservissement.
	-----------------------------------------*/
	void DC_MOTOR_SPEED_getCoefs(DC_MOTOR_SPEED_id id, Sint16* Kp, Sint16* Ki, Sint16* Kd, Sint16* Kv) {
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		assert((thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED));

		if(Kp) *Kp = thiss->config.Kp;
		if(Ki) *Ki = thiss->config.Ki;
		if(Kd) *Kd = thiss->config.Kd;
		if(Kd) *Kv = thiss->config.Kv;
	}

	/*-------------------------------------------
	  Récupère les coefficients pwm
	--------------------------------------------*/
	void DC_MOTOR_SPEED_setMaxPwm(DC_MOTOR_SPEED_id id, Uint8 max_duty){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		thiss->config.max_duty = max_duty;
	}

	void DC_MOTOR_SPEED_getMaxPwm(DC_MOTOR_SPEED_id id, Uint8 *max_duty){
		assert(id < DC_MOTOR_SPEED_NUMBER);
		assert(max_duty != NULL);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		*max_duty = thiss->config.max_duty;
	}

	Sint8 DC_MOTOR_SPEED_getCurrentPwm(DC_MOTOR_SPEED_id id){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		DC_MOTOR_SPEED_config_t* config = &(thiss->config);
		Sint8 pwm = 0;
		if(config->simulateWay){
			if(DC_MOTOR_SPEED_getWay(id)){
				pwm = -thiss->currentPwm;
			}else{
				pwm = thiss->currentPwm;
			}
		}else{
			pwm = thiss->currentPwm;
		}
		return pwm;
	}


	/*-----------------------------------------
			Arret de l'asservissement d'un actionneur
	-----------------------------------------*/
	void DC_MOTOR_SPEED_stop(DC_MOTOR_SPEED_id id){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		if(thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED)
		{
			thiss->state = DC_MOTOR_SPEED_IDLE;
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
	void DC_MOTOR_SPEED_restart(DC_MOTOR_SPEED_id id){
		assert(id < DC_MOTOR_SPEED_NUMBER);

		DC_MOTOR_SPEED_t* thiss = &(dcMotorSpeed[id]);
		if(thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED)
		{
			thiss->integrator = 0;
			thiss->time = 0;
			thiss->state = DC_MOTOR_SPEED_LAUNCH;
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
		DC_MOTOR_SPEED_speed speed, wantedSpeed;

		for (id=0; id<DC_MOTOR_SPEED_NUMBER; id++)
		{
			thiss = &(dcMotorSpeed[id]);
			config = &(thiss->config);

			if (thiss->configState == DC_MOTOR_SPEED_CONFIG_INITIALIZED)
			{

				// Simulation du sens de rotation du moteur
				if(config->simulateWay){
					if(DC_MOTOR_SPEED_getWay(id)){
						speed = -(config->sensorRead)();
					}else{
						speed = (config->sensorRead)();
					}
				}else{
					speed = (config->sensorRead)();
				}

				// Acquisition de la position pour la détection de l'arrêt du moteur
				if(thiss->state == DC_MOTOR_SPEED_INIT_RECOVERY || thiss->state == DC_MOTOR_SPEED_LAUNCH_RECOVERY || thiss->state == DC_MOTOR_SPEED_RUN_RECOVERY){
					wantedSpeed = config->speedRecovery;
				}else{
					wantedSpeed = thiss->wantedSpeed;
				}

				error = wantedSpeed - speed;

				thiss->time += DC_MOTOR_SPEED_TIME_PERIOD;

				//Gestion des changements d'états
				switch(thiss->state){

					case DC_MOTOR_SPEED_IDLE:
						break;

					case DC_MOTOR_SPEED_INIT_LAUNCH:
						thiss->time = 0;
						thiss->timeInError = 0;
						thiss->timeBeginRecovery = 0;
						thiss->state = DC_MOTOR_SPEED_LAUNCH;
						thiss->timeWaitingLimitPwm = TIME_OF_MOTOR_STEPPING;
						break;

					case DC_MOTOR_SPEED_LAUNCH:
						if(absolute(error) < (Sint16)config->epsilon && absolute(thiss->previousError) < (Sint16)config->epsilon)
							thiss->state = DC_MOTOR_SPEED_RUN;
						else if(config->timeout && thiss->time >= config->timeout){
							if(config->activateRecovery)
								thiss->state = DC_MOTOR_SPEED_INIT_RECOVERY;
							else
								thiss->state = DC_MOTOR_SPEED_ERROR;
						}
						break;

					case DC_MOTOR_SPEED_RUN:
						if(absolute(error) > (Sint16)config->epsilon){
							if(thiss->timeInError == 0)
								thiss->timeInError = global.absolute_time;
							else if(global.absolute_time - thiss->timeInError > config->timeout){
								if(config->activateRecovery)
									thiss->state = DC_MOTOR_SPEED_LAUNCH_RECOVERY;
								else
									thiss->state = DC_MOTOR_SPEED_ERROR;
							}
						}else{
							thiss->timeInError = 0;
						}

						break;

					case DC_MOTOR_SPEED_INIT_RECOVERY:
						thiss->time = 0;
						thiss->timeInError = 0;
						thiss->timeBeginRecovery = global.absolute_time;
						thiss->state = DC_MOTOR_SPEED_LAUNCH_RECOVERY;
						thiss->timeWaitingLimitPwm = TIME_OF_MOTOR_STEPPING;
						thiss->integrator = 0; // On remet à 0 la somme de l'intégrateur pour changer plus vite de sens
						break;

					case DC_MOTOR_SPEED_LAUNCH_RECOVERY:
						if(absolute(error) < (Sint16)config->epsilon && absolute(thiss->previousError) < (Sint16)config->epsilon)
							thiss->state = DC_MOTOR_SPEED_RUN_RECOVERY;
						else if(global.absolute_time - thiss->timeBeginRecovery > config->timeRecovery)
							thiss->state = DC_MOTOR_SPEED_INIT_LAUNCH;
						break;

					case DC_MOTOR_SPEED_RUN_RECOVERY:
						if(global.absolute_time - thiss->timeBeginRecovery > config->timeRecovery){
							thiss->state = DC_MOTOR_SPEED_INIT_LAUNCH;
							thiss->integrator = 0; // On remet à 0 la somme de l'intégrateur pour repartir plus vite dans le bon sens
						}
						break;

					case DC_MOTOR_SPEED_ERROR:
						break;
				}

				// Gestion des actions
				switch(thiss->state){

					case DC_MOTOR_SPEED_IDLE:
					case DC_MOTOR_SPEED_ERROR:
						PWM_stop(config->pwm_number);
						break;

					case DC_MOTOR_SPEED_INIT_LAUNCH:
					case DC_MOTOR_SPEED_INIT_RECOVERY:
						break;

					case DC_MOTOR_SPEED_LAUNCH:
					case DC_MOTOR_SPEED_RUN:
					case DC_MOTOR_SPEED_LAUNCH_RECOVERY:
					case DC_MOTOR_SPEED_RUN_RECOVERY:

						/* Integration si on n'est pas en saturation de commande (permet de désaturer plus vite) */
						if(thiss->currentPwm != config->max_duty){
							thiss->integrator += error; //la multiplication par la période se fait après pour éviter que l'incrément soit nul
						}

						differential = error - thiss->previousError;

						// Asservissement PID
						// Chaque coefficient multiplié par  1024 d'où les divisions

						computed_cmd = 	((config->Kp * error) / 1024)
									+ ((config->Ki * thiss->integrator * DC_MOTOR_SPEED_TIME_PERIOD) >> 20)
									+ (((config->Kd * differential) / DC_MOTOR_SPEED_TIME_PERIOD) >> 10)
									+ ((config->Kv * wantedSpeed) / 1024);

						// Sens et saturation
						if (computed_cmd > 0)
							DC_MOTOR_SPEED_setWay(id, 0);
						else
							DC_MOTOR_SPEED_setWay(id, 1);

						computed_cmd = absolute(computed_cmd);

						if (computed_cmd > config->max_duty)
							thiss->currentPwm = config->max_duty;
						else
							thiss->currentPwm = (Uint8)computed_cmd;


						// Application de la commande
						Uint8 realPwm = thiss->currentPwm;	//On suppose qu'on va prendre la PWM souhaitée.

						if(thiss->timeWaitingLimitPwm != 0)	//on est en 'début d'ordre', une nouvelle consigne est récemment arrivée
						{
							if(absolute(thiss->currentPwm - thiss->lastPwm) > MAX_STEP_OF_PWM)
							{		//Si la dernière PWM est LOIN de la PWM souhaitée... alors on s'en rapproche DOUCEMENT.
								if(thiss->currentPwm > thiss->lastPwm)
									realPwm = thiss->lastPwm + MAX_STEP_OF_PWM;		//Ca monte.. On s'en rapproche en montant.
								else
									realPwm = thiss->lastPwm - MAX_STEP_OF_PWM;		//Ca descend..On s'en rapproche en descendant.
							}
						}

						if(thiss->timeWaitingLimitPwm > DC_MOTOR_SPEED_TIME_PERIOD)
							thiss->timeWaitingLimitPwm -= DC_MOTOR_SPEED_TIME_PERIOD;
						else
							thiss->timeWaitingLimitPwm = 0;

						thiss->lastPwm = realPwm;
						thiss->previousError = error;

						PWM_run(realPwm, config->pwm_number);

						break;
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
			thiss->time = 0;
		}
	}

#endif /* def USE_DCMOTOR2 */
