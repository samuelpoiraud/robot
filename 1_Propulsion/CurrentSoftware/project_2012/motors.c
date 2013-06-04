/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : moteur.c
 *  Package : Asser
 *  Description : commande des moteurs
 *  Auteur : Nirgal (2009)
 *  Version 201005
 */

#define _MOTEUR_C

#include "motors.h"
#include "supervisor.h"
#include "joystick.h"
#include "QS/QS_pwm.h"

void MOTORS_init(void)
{
	PWM_init();
	MOTORS_reset();	//RAZ des sorties Moteur.
}
	
void MOTORS_reset(void)
{
	//on ne peut pas mettre 0 (car utilisation des PWM L)
	PWM_run(1, PWM_MOTEUR_1);
	PWM_run(1, PWM_MOTEUR_2);
}



void MOTORS_update(Sint16 duty_left, Sint16 duty_right)
{
	SUPERVISOR_state_e state;
	state = SUPERVISOR_get_state();
	//Si on est pas en erreur...
	if(JOYSTICK_is_enable())
	{
		//Dans le mode joystick, on écrase le code normal ici !!!
		duty_left = JOYSTICK_get_duty_left();
		duty_right = JOYSTICK_get_duty_right();
	}
	else	// qu'en absence de joystick... donc TOUT LE TEMPS !
	{
		if (state == SUPERVISOR_ERROR || state == SUPERVISOR_MATCH_ENDED)
		{	//Moteurs ne bougent pas...
			duty_left = 0;
			duty_right = 0;	
		}
	}

	/////////////////////////////////////////////////////////	
	//Commande du sens des moteurs (sens vers les ponts en H)
	if(duty_right<0)
	{
		SENS_MOTEUR_1= MOTOR_BACKWARD;
		duty_right=-duty_right;
	}
	else 
		SENS_MOTEUR_1= MOTOR_FORWARD;

					
	if(duty_left<0)
	{
		SENS_MOTEUR_2= MOTOR_BACKWARD;
		duty_left=-duty_left;
	}
	else 
		SENS_MOTEUR_2= MOTOR_FORWARD;
	
	/////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	//Commande de la PWM avec écretage de sécurité
	if (duty_right>CLIPPING_DUTY)
		duty_right=CLIPPING_DUTY;	/*vitesse max*/
	PWM_run( (Sint8)duty_right, PWM_MOTEUR_1);
	
	if (duty_left>CLIPPING_DUTY)
		duty_left=CLIPPING_DUTY;
	PWM_run( (Sint8)duty_left, PWM_MOTEUR_2);
	/////////////////////////////////////////////////////////	
}

