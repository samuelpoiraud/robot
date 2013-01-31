/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_DCMotor.c
 *	Package : Qualité Soft
 *	Description : Gestion des moteurs à courant continu asservis
 *				(mesure de position par potentiomètre)
 *  Auteur : Gwenn, Jacen
 *  Version 20090410
 */

#define QS_DCMOTOR_C

#include "../QS/QS_DCMotor.h"
#ifdef USE_DCMOTOR


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
#else
	#error "DCM_TIMER doit etre 1 2 ou 3"
#endif /* DCM_TIMER == n */

/*-----------------------------------------
		Variables globales privées
-----------------------------------------*/

	typedef struct
	{
		Uint8 adc_channel;		// voie à lire pour avoir le retour du moteur dans ADC_getValue(...)
		Uint8 pwm_number;		// numero de la sortie PWM à utiliser (conformément à PWM_run(...))
		Uint16 pos[DCMOTOR_NB_POS];	// valeurs de l'adc pour les différentes positions de l'actionneur
		Uint8 way0_max_duty;	// rapport cyclique maximum de la pwm avec le bit sens à 0
		Uint8 way1_max_duty;	// rapport cyclique maximum de la pwm avec le bit sens à 1
		Sint16 Kp, Ki, Kd;		// valeurs des gains pour le PID
		Uint16 timeout;			// timeout en ms
		Uint16* way_latch;		// adresse du port contenant le bit de sens de controle du pont en H
		Uint8 way_bit_number;	// numero du bit de sens dans le port
	}DCMotor_config_t;	
	
	typedef struct
	{
		DCMotor_config_t config;	// la config du DCMotor
		bool_e cmd_flag;			// Flag indiquant que l'actionneur est en mouvement
		Sint32 integrator;			// Integrateur pour le PID
		Uint16 cmd_time;			// temps depuis la reception de la commande, en ms
		Uint8 posToGo;				// consigne, en numero de position actionneur
		Uint8 current_cmd;			// commande PWM actuelle
		Sint16 previous_error;		// valeur de l'erreur à l'appel précédent du PID, pour calcul du terme derivé
		bool_e initialized;			// flag indiquant si le moteur a été configuré
	}DCMotor_t;

	static DCMotor_t DCMotors[DCM_NUMBER];

/*-----------------------------------------
		Prototypes des fonctions privées
-----------------------------------------*/
	// Accesseur
	static void DCM_setWay(Uint8 dc_motor_id, Uint8 value);
	static Uint8 DCM_getWay(Uint8 dc_motor_id);
	// Envoie un message CAN pour informer de la position des ascenseurs
	static void DCM_sendCAN(Uint8 dc_motor_id, Uint8 pos);
	// Baisse les drapeaux d'initialisation des moteurs
	static void DCM_uninitialize_all();
/*-----------------------------------------
		Initialisation
-----------------------------------------*/
void DCM_init()
{
	// Initialisation
	DCM_uninitialize_all();
	// Lancement de l'interruption de commande
	DCM_TIMER_RUN(DCM_TIMER_PERIOD);
}

void DCM_config (	Uint8 dc_motor_id,
					Uint8 adc_channel,		// voie à lire pour avoir le retour du moteur dans ADC_getValue(...)
					Uint8 pwm_number,		// numero de la sortie PWM à utiliser (conformément à PWM_run(...))
					Uint16 pos[DCMOTOR_NB_POS],	// valeurs de l'adc pour les différentes positions de l'actionneur
					Uint8 way0_max_duty,	// rapport cyclique maximum de la pwm avec le bit sens à 0
					Uint8 way1_max_duty,	// rapport cyclique maximum de la pwm avec le bit sens à 1
					Sint16 Kp,
					Sint16 Ki,				// valeurs des gains pour le PID
					Sint16 Kd,
					Uint16 timeout,			// timeout en ms
					Uint16* way_latch,		// adresse du port contenant le bit de sens de controle du pont en H
					Uint8 way_bit_number	// numero du bit de sens dans le port
				)
{
	Uint8 i;
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	DCMotor_config_t* config = &(this->config);
	this->cmd_flag=0;
	this->integrator=0;
	this->cmd_time=0;
	this->posToGo=0;
	this->current_cmd=0;
	this->previous_error=0;
	
	config->adc_channel=adc_channel;
	config->pwm_number=pwm_number;
	config->way0_max_duty=way0_max_duty;
	config->way1_max_duty=way1_max_duty;
	config->Kp=Kp;
	config->Ki=Ki;
	config->Kd=Kd;
	config->timeout=timeout;
	config->way_latch=way_latch;
	config->way_bit_number=way_bit_number;

	for(i=0; i<DCMOTOR_NB_POS;i++)
		config->pos[i]=pos[i];
	this->initialized=TRUE;
}

static void DCM_uninitialize_all()
{
	Uint8 i;
	for (i=0;i<DCM_NUMBER;i++)
		(DCMotors[i]).initialized = FALSE;
}	


/*-----------------------------------------
	Setter pour le sens des moteurs
-----------------------------------------*/
static void DCM_setWay(Uint8 dc_motor_id, Uint8 value)
{
	DCMotor_config_t* this = &(DCMotors[dc_motor_id].config);
	if (value)
		BIT_SET(*(this->way_latch), this->way_bit_number);
	else
		BIT_CLR(*(this->way_latch), this->way_bit_number);
}	

static Uint8 DCM_getWay(Uint8 dc_motor_id)
{
	DCMotor_config_t* this = &(DCMotors[dc_motor_id].config);
	return BIT_TEST(*(this->way_latch), this->way_bit_number);
}	



/*-----------------------------------------
		Ordre de déplacement
-----------------------------------------*/
void DCM_goToPos(Uint8 dc_motor_id, Uint8 pos)
{
	DCMotor_t* this = &(DCMotors[dc_motor_id]);
	debug_printf("Déplacement de %d en %d\r\n", dc_motor_id, pos);
	this->posToGo = pos;
	this->cmd_flag = TRUE;
	this->cmd_time = 0;
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
	
	for (dc_motor_id = 0; dc_motor_id < DCM_NUMBER; dc_motor_id++)
	{
		this = &(DCMotors[dc_motor_id]);
		config = &(this->config);
		this->cmd_time ++;
		
		// Acquisition de la position pour la détection de l'arrêt du moteur
		Sint16 error = config->pos[this->posToGo]-ADC_getValue(config->adc_channel);
		if	(	(	(abs(error) < DCM_EPSILON) 
					|| (this->cmd_time > (config->timeout/DCM_TIMER_PERIOD))
				)
				&& 	
				(this->cmd_flag)
			)
		{
			this->cmd_flag=FALSE;
			DCM_sendCAN(dc_motor_id, this->posToGo);
		}
		
		// Asservissement PID
		/* integration si on n'est pas en saturation de commande (permet de désaturer plus vite) */
		/* l'expression si dessous vaut pour erreur = consigne-position */
		if(!(		( (DCM_getWay(dc_motor_id)) && (this->current_cmd == config->way1_max_duty) && (!((config->Ki>0) ^ (error>0))))
				||	(!(DCM_getWay(dc_motor_id)) && (this->current_cmd == config->way0_max_duty) && (!((config->Ki>0) ^ (error<0))))
			))
		{
			this->integrator += error;
			/* la multiplication par la période se fait après pour éviter que l'incrément soit nul */
		}
		differential = error - this->previous_error;
		computed_cmd = 	((Sint32)config->Kp * (Sint32)error)
						+ (((Sint32)config->Ki * this->integrator * DCM_TIMER_PERIOD)/1024)
						+ (((Sint32)config->Kd * differential * 1024)/DCM_TIMER_PERIOD);
	
		// Sens et saturation
		if (computed_cmd < 0)
		{
			DCM_setWay(dc_motor_id, 0);
			this->current_cmd =	computed_cmd>>10;
			if (this->current_cmd > config->way0_max_duty)
				this->current_cmd = config->way0_max_duty;
		}
		else 
		{
			DCM_setWay(dc_motor_id, 1);
			this->current_cmd =	(-computed_cmd)>>10;
			if (this->current_cmd > config->way1_max_duty)
				this->current_cmd = config->way1_max_duty;
		}
		// application de la commande
		PWM_run(this->current_cmd, config->pwm_number);
	}
	// acquittement de l'interruption
	DCM_TIMER_FLAG = 0;
}


/*-----------------------------------------
Envoie un retour CAN / signal de fin de parcours
-----------------------------------------*/
static void DCM_sendCAN(Uint8 dc_motor_id, Uint8 pos)
{	
	// Préparation
	CAN_msg_t msg;
	msg.sid = ACT1_DCM_STATE;
	msg.data[0] = dc_motor_id;
	msg.data[1] = pos;	
	msg.size=2;
	// Envoi
	debug_printf("\n\rActionneur %d en fin de déplacement au niveau %d\r\n", dc_motor_id, pos);
	CAN_send(&msg);
}

#endif /* def USE_DCMOTOR */
