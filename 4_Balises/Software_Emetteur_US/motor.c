/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : motor.c
 *	Package : Projet Balise Récepteur US
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit être le plus constante possible.
 *	Auteur : Nirgal
 *	Version 201203
 */

#include "motor.h"
#include "QS/QS_qei.h"
#include "QS/QS_pwm.h"
#include <pwm.h>
#include "QS/QS_timer.h"
#include "QS_outputlog.h"
#include "Global_config.h"
#include "QS/QS_rf.h"
#include "QS/QS_watchdog.h"

static volatile bool_e motor_enable;
#define PERIOD_IT_MOTOR 2 //ms
#define KV		 	(Sint16)(8)
#define KP			(Sint16)(32)
#define KD			(Sint16)(4)
#define KI			(Sint16)(2)
#define COMMAND 	(Sint16)(70)//pas par 2ms = 2048*17 pas par secondes = 17 tours par seconde


#define DUTY_FILTER			30
#define START_FILTER_DUTY	30	// Donne une valeur pour le démarrage, évite le pic du début
#define THRESHOLD_DUTY_MAX	36	// La batterie a une tension inférieure à 6,8V


void MOTOR_send_msg_battery_low(void);

void MOTOR_init(void)
{
	QEI_init();
	PWM_init();
	motor_enable = TRUE;
	TIMER4_run(PERIOD_IT_MOTOR);
}	

static volatile Sint16 count, error, delta_error, error_prec;
static volatile Sint32 integral = 0;

// compteur pour la reptition des messages quand la batterie est faible
static bool_e battery_low_send = FALSE;
static volatile Uint16 compt_bat_low = 0;

#define REPITION_MSG_BAT_LOW		2500

#define NB_IT_PER_TURN			(Uint8)(30)	//it = 2ms, 1 tour = 60ms => 30 it par tour.
#define	NB_ENCODER_STEP_PER_IT	(Uint8)(68)	
static volatile Sint16 speed;
static volatile Sint16 duty;

//A appeler toutes les 2 ms.
//ce choix est arbitraire, et très lié au fait que le reste du code utilise également avec une IT 2ms
void MOTOR_process_it(void)
{
	static Sint16 count_prec = 0;
	static Sint16 error_prec = 0;
	static Sint16 sum_error = 0;
	Sint16 count, error, delta_error;
	
	count = QEI_get_count();			//Lecture codeur.
	
	speed = absolute(count - count_prec);	//Calcul vitesse
		
	error = COMMAND - speed;			//Calcul de l'erreur sur la vitesse
	
	delta_error = error - error_prec;	//Calcul de la dérivée de l'erreur sur la vitesse
	
	sum_error += (speed < 10)?0:error;	//Calcul de l'intégrale de l'erreur sur la vitesse (avec sécurité si vitesse faible !)

	//Calcul du rapport cyclique de la PWM à envoyer au moteur.
	duty = (		KV * COMMAND 
				+ 	KP * error 
				+ 	KD * delta_error
				+ 	KI * sum_error
			)/16;

	if(duty <0)	//Ecretage mini
		duty = 1;
	
	SetDCMCPWM(2, duty, 0);	
	
	//On sauvegarde pour le prochain tour.
	count_prec = count;
	error_prec = error;

	if(battery_low_send)
		compt_bat_low++;
}	

void MOTOR_process_main(void)
{
	static Uint16 filter_duty = START_FILTER_DUTY;
	//debug_printf("d%d\tV%d\tFilter_duty %d",duty,speed,filter_duty);
	//debug_printf("\n");
	
	// Mise en place d'un filtre, pour les pics de valeurs et/ou le démarrage
	filter_duty = (filter_duty*(100-DUTY_FILTER) + (duty*DUTY_FILTER))/100 ;

	if((filter_duty > THRESHOLD_DUTY_MAX && !battery_low_send) || compt_bat_low > REPITION_MSG_BAT_LOW){
		battery_low_send = TRUE;
		compt_bat_low = 0;

		CAN_msg_t msg;
		msg.sid = STRAT_BALISE_BATTERY_LOW;
		msg.size = 1;
		msg.data[0] = I_AM_CARTE_BALISE;

		RF_can_send(RF_SMALL, &msg);
		RF_can_send(RF_BIG, &msg);

		debug_printf("send message\n");
	}
}

void MOTOR_togle_enable(void)
{
	motor_enable = !motor_enable;
}	

void _ISR _T4Interrupt(void)
{
	MOTOR_process_it();
	IFS1bits.T4IF = 0;
}	