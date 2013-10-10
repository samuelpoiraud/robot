/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_Step_motor.c
 *	Package : Qualité Software
 *	Description : 	fonction de gestion du moteur pas à pas
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20090220
 */

#define QS_STEP_MOTOR_C
#include "QS_Step_motor.h"

#ifdef USE_STEP_MOTOR

#include "QS_timer.h"
#include "QS_ports.h"

#ifdef NEW_CONFIG_ORGANISATION
	#include "config_pin.h"
#endif

/*variables globales pour le moteur pas à pas */
static volatile Sint16 m_order;


/* pour fonctionnement interne */
static void STEP_MOTOR_apply_step(Uint8 step);

#define STEP_SEQUENCE_SIZE	8

#ifdef STEP_MOTOR_TIMER
#define TIMER_SRC_TIMER_ID STEP_MOTOR_TIMER
#endif
#ifdef STEP_MOTOR_USE_WATCHDOG
#define TIMER_SRC_USE_WATCHDOG
#endif

#include "QS_setTimerSource.h"


void STEP_MOTOR_init()
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	m_order =0;
	TIMER_SRC_TIMER_init();

	TIMER_SRC_TIMER_start_ms(STEP_MOTOR_MS_PER_STEP);
}

void STEP_MOTOR_add_steps(Sint8 nb_of_steps)
{
	m_order += nb_of_steps;
}	

void TIMER_SRC_TIMER_interrupt()
{
	static Uint8 current_step =0;
	static Sint16 STEP_MOTOR_position = 0;


	if (STEP_MOTOR_position < m_order)
	{
		current_step ++; current_step %=STEP_SEQUENCE_SIZE;
		STEP_MOTOR_apply_step(current_step);
		STEP_MOTOR_position++;
	}
	else if (STEP_MOTOR_position > m_order)
	{
		current_step += STEP_SEQUENCE_SIZE -1; 
		current_step %=STEP_SEQUENCE_SIZE;
		STEP_MOTOR_apply_step(current_step);
		STEP_MOTOR_position--;
	}
	
	/*pour eviter les dépassements : */
	if (m_order > 30000)
	{
		STEP_MOTOR_position -= m_order;
		m_order=0;		
	}
	else if (m_order < -30000)
	{
		STEP_MOTOR_position += m_order;
		m_order=0;
	}

	TIMER_SRC_TIMER_resetFlag();
}


static void STEP_MOTOR_apply_step(Uint8 step)
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
