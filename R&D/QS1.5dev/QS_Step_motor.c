/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_Step_motor.c
 *	Package : Qualité Software
 *	Description : 	fonction de gestion du moteur pas à pas
 *	Auteur : Jacen
 *	Version 20090220
 */

#define QS_STEP_MOTOR_C
#include "QS_Step_motor.h"

#ifdef USE_STEP_MOTOR

void STEP_MOTOR_init()
{
	global.STEP_MOTOR_order =0;
	TIMER_init();
	STEP_MOTOR_TIMER_RUN(STEP_MOTOR_MS_PER_STEP);
}


void _ISR STEP_MOTOR_TIMER_IT()
{
	static Uint8 current_step =0;
	static Sint16 STEP_MOTOR_position = 0;


	if (STEP_MOTOR_position < global.STEP_MOTOR_order)
	{
		current_step ++; current_step %=STEP_SEQUENCE_SIZE;
		STEP_MOTOR_apply_step(current_step);
		STEP_MOTOR_position++;
	}
	else if (STEP_MOTOR_position > global.STEP_MOTOR_order)
	{
		current_step += STEP_SEQUENCE_SIZE -1; 
		current_step %=STEP_SEQUENCE_SIZE;
		STEP_MOTOR_apply_step(current_step);
		STEP_MOTOR_position--;
	}
	
	/*pour eviter les dépassements : */
	if (global.STEP_MOTOR_order > 30000)
	{
		STEP_MOTOR_position -= global.STEP_MOTOR_order;
		global.STEP_MOTOR_order=0;		
	}
	else if (global.STEP_MOTOR_order < -30000)
	{
		STEP_MOTOR_position += global.STEP_MOTOR_order;
		global.STEP_MOTOR_order=0;
	}
	STEP_MOTOR_TIMER_FLAG =0;
}


void STEP_MOTOR_apply_step(Uint8 step)
{
	
	/* sequence des demi pas d'un moteur pas à pas */
	static const Uint8 step_sequence[STEP_SEQUENCE_SIZE] = 
	{	
		0b1000, 0b1100, 0b0100, 0b0110,
		0b0010, 0b0011, 0b0001, 0b1001
	};
	if (BIT_TEST(step_sequence[step], 1))
	{	
		STEP_MOTOR1 = 1;
		nop();
	}
	else
	{
		STEP_MOTOR1 = 0;
		nop();
	}
	if (BIT_TEST(step_sequence[step], 2))
	{
		STEP_MOTOR2 = 1;
		nop();
	}
	else
	{
		STEP_MOTOR2 = 0;
		nop();
	}
	if (BIT_TEST(step_sequence[step], 3))
	{
		STEP_MOTOR3 = 1;
		nop();
	}
	else
	{
		STEP_MOTOR3 = 0;
		nop();
	}
	if (BIT_TEST(step_sequence[step], 4))
	{
		STEP_MOTOR4 = 1;
		nop();
	}
	else
	{
		STEP_MOTOR4 = 0;
		nop();
	}
}	

#endif /* def USE_STEP_MOTOR */
