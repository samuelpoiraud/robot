/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_servo.c
 *	Package : Qualite Software
 *	Description : Driver par timer pour servomoteur
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#include "QS_servo.h"
#include "QS_timer.h"
#include "QS_ports.h"

#ifdef USE_SERVO

/*variables globales pour le pilote de servos */
static volatile Sint16 m_SERVO_cmd[10];


/*-------------------------------------
	Gestion des servomoteurs
-------------------------------------*/

// Fonction d'interruption du timer
#if !defined(SERVO_TIMER)
	#error "SERVO_TIMER doit etre 1 2 3 ou 4, le watchdog n'est pas utilisable avec ce module"
#else
	#define TIMER_SRC_TIMER_ID SERVO_TIMER

	#include "QS_setTimerSource.h"
#endif
	
#define MIN_INTERVAL		1000


/* lutte contre la non d�claration des servos */
//Format port: GPIOx, bit (a partir de 0
//Par defaut: SERVO sur port C, bit 10
#ifndef SERVO0
	#define SERVO0	GPIOC->ODR10
#endif
#ifndef SERVO1
	#define SERVO1	SERVO0
#endif
#ifndef SERVO2
	#define SERVO2	SERVO1
#endif
#ifndef SERVO3
	#define SERVO3	SERVO2
#endif
#ifndef SERVO4
	#define SERVO4	SERVO3
#endif
#ifndef SERVO5
	#define SERVO5	SERVO4
#endif
#ifndef SERVO6
	#define SERVO6	SERVO5
#endif
#ifndef SERVO7
	#define SERVO7	SERVO6
#endif
#ifndef SERVO8
	#define SERVO8	SERVO7
#endif
#ifndef SERVO9
	#define SERVO9	SERVO8
#endif
/* tous les servos sont d�clar�s, le code compile */



/*-------------------------------------
	Initialisation des servomoteurs
-------------------------------------*/
void SERVO_init() {
	Uint8 i;
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	// Initialisation de la commande; par d�faut 0 (le moteur ne bouge pas)
	for(i=0;i<10;i++)
		m_SERVO_cmd[i] = 0 ;

	TIMER_SRC_TIMER_init();
	// Lancement du timer
	TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
}
		
/*-------------------------------------
Changement de la commande (entre 0 et 2000 us)
-------------------------------------*/
void SERVO_set_cmd(Uint16 cmd, Uint8 num_servo)
{
		m_SERVO_cmd[num_servo] = cmd ;
}


/*-------------------------------------
	Timer
-------------------------------------*/
void TIMER_SRC_TIMER_interrupt()
{
	static Uint8 etat=0;
	static Sint16 blanking=0;

	switch(etat)
	{
		case 0:
			blanking = 20000; // 20 ms de periode 
			if (m_SERVO_cmd[etat])
			{
				SERVO0 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 1:
			SERVO0 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO1 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 2:
			SERVO1 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO2 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 3:
			SERVO2 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO3 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 4:
			SERVO3 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO4 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 5:
			SERVO4 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO5 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 6:
			SERVO5 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO6 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 7:
			SERVO6 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO7 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 8:
			SERVO7 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO8 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 9:
			SERVO8 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO9 = 1;
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
		break;
		default:
			SERVO9 = 0;
			blanking = (blanking >MIN_INTERVAL)?blanking:MIN_INTERVAL;
			TIMER_SRC_TIMER_start_us(blanking);
			etat = 0;
			break;
	}
	TIMER_SRC_TIMER_resetFlag();
}

#endif /* def USE_SERVO */
