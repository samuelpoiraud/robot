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

static volatile bool_e motor_enable;
#define PERIOD_IT_MOTOR 2 //ms
#define KV		 	(Sint16)(8)
#define KP			(Sint16)(32)
#define KD			(Sint16)(4)
#define KI			(Sint16)(2)
#define COMMAND 	(Sint16)(70)//pas par 2ms = 2048*17 pas par secondes = 17 tours par seconde



void MOTOR_init(void)
{
	QEI_init();
	PWM_init();
	motor_enable = TRUE;
	TIMER4_run(PERIOD_IT_MOTOR);
}	

volatile Sint16 count, error, delta_error, error_prec;
volatile Sint32 integral = 0;


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
	
	speed = abs(count - count_prec);	//Calcul vitesse
		
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
}	

void MOTOR_process_main(void)
{
//	Uint8 i;
	//debug_printf("d%d\tV%d",duty,speed)	;		
	//debug_printf("\n");
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
