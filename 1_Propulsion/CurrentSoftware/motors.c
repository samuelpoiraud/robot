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
#include "QS/QS_ports.h"
#include "supervisor.h"
#include "joystick.h"
#include "QS/QS_pwm.h"
#include "QS/QS_IHM.h"

#define MAX_PWM_BORDER_MODE		45
#define MAX_STEP_OF_PWM			10

volatile bool_e border_mode = 0;
static Uint8 last_duty_left = 0;
static Uint8 last_duty_right = 0;

void MOTORS_init(void)
{
	PWM_init();
	MOTORS_reset();	//RAZ des sorties Moteur.
}

void MOTORS_reset(void)
{
	PWM_run(0, PWM_MOTEUR_1);
	PWM_run(0, PWM_MOTEUR_2);
	last_duty_left = 0;
	last_duty_right = 0;
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
		if (state == SUPERVISOR_ERROR || state == SUPERVISOR_MATCH_ENDED || !IHM_switchs_get(SWITCH_ASSER))
		{	//Moteurs ne bougent pas...
			duty_left = 0;
			duty_right = 0;
		}
	}

	/////////////////////////////////////////////////////////
	//Commande du sens des moteurs (sens vers les ponts en H)
	if(duty_right<0)
	{
		GPIO_WriteBit(SENS_MOTEUR_1, MOTOR_BACKWARD);
		duty_right=-duty_right;
	}
	else
		GPIO_WriteBit(SENS_MOTEUR_1, MOTOR_FORWARD);


	if(duty_left<0)
	{
		GPIO_WriteBit(SENS_MOTEUR_2, MOTOR_BACKWARD);
		duty_left=-duty_left;
	}
	else
		GPIO_WriteBit(SENS_MOTEUR_2, MOTOR_FORWARD);

	/////////////////////////////////////////////////////////
#ifdef LIMITATION_PWM_BORDER_MODE
	// Ecretage de sécurité batterie border mode
	if(border_mode){
		if (duty_right > MAX_PWM_BORDER_MODE)
			duty_right = MAX_PWM_BORDER_MODE;

		if (duty_left > MAX_PWM_BORDER_MODE)
			duty_left = MAX_PWM_BORDER_MODE;
	}
#endif
	/////////////////////////////////////////////////////////
	//Commande de la PWM avec écretage de sécurité
	if (duty_right>CLIPPING_DUTY)
		duty_right=CLIPPING_DUTY;	/*vitesse max*/

	if (duty_left>CLIPPING_DUTY)
		duty_left=CLIPPING_DUTY;


	//Si la PWM demandée est ELOIGNEE de la précédente... on y va mollo !
	if(duty_left > last_duty_left + MAX_STEP_OF_PWM)
		duty_left = last_duty_left + MAX_STEP_OF_PWM;

	if(duty_right > last_duty_right + MAX_STEP_OF_PWM)
		duty_right = last_duty_right + MAX_STEP_OF_PWM;

	last_duty_left = duty_left;
	last_duty_right = duty_right;

	PWM_run( (Sint8)duty_right, PWM_MOTEUR_1);
	PWM_run( (Sint8)duty_left, PWM_MOTEUR_2);
	/////////////////////////////////////////////////////////
}

void MOTORS_set_border_mode(bool_e border_mode_wanted){
	border_mode = border_mode_wanted;
}

