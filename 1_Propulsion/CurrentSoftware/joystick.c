/*
*  Club Robot ESEO 2006 - 2012
*
*  Fichier : joystick.h
*  Package : Propulsion
*  Description : Gestion du joystick permettant un pilotage manuel du robot
*  Auteur : 2009 Nirgal
*  Version 201203
*/



#include "joystick.h"

volatile Uint8 joystick_enable_time;	//[ms]
volatile Sint8 joystick_duty_left;
volatile Sint8 joystick_duty_right;
volatile bool_e joystick_button_A;
volatile bool_e joystick_button_B;

void JOYSTICK_init(void)
{
	joystick_enable_time = 0;	//0 signifie : pas de commande joystick valide...
}

bool_e JOYSTICK_is_enable(void)
{
	return (joystick_enable_time>0)?TRUE:FALSE;
}

void JOYSTICK_process_it(void)
{
	if(joystick_enable_time > PERIODE_IT_ASSER)
		joystick_enable_time -= PERIODE_IT_ASSER;	//le temps passe... si cette variable atteint 0, le joytick se désactive !
	else
		joystick_enable_time = 0;
}

Sint8 JOYSTICK_get_duty_left(void)
{
	return joystick_duty_left;
}

Sint8 JOYSTICK_get_duty_right(void)
{
	return joystick_duty_right;
}

void JOYSTICK_enable(Uint8 ms, Sint8 duty_left, Sint8 duty_right, bool_e buttonA, bool_e buttonB)
{
	joystick_enable_time = ms;
	joystick_duty_left = duty_left;
	joystick_duty_right = duty_right;
	joystick_button_A = buttonA;
	joystick_button_B = buttonB;
}


