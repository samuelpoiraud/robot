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

#include "../QS/QS_servo.h"

#ifdef USE_SERVO

/*variables globales pour le pilote de servos */
static volatile Sint16 m_SERVO_cmd[10];


/*-------------------------------------
	Gestion des servomoteurs
-------------------------------------*/

// Fonction d'interruption du timer
/*#if SERVO_TIMER == 1
		#define SERVO_TIMER_RUN_FINE	TIMER1_run_us
		#define SERVO_TIMER_IT			_T1Interrupt
		#define SERVO_TIMER_FLAG		IFS0bits.T1IF
#elif SERVO_TIMER == 2
		#define SERVO_TIMER_RUN_FINE	TIMER2_run_us
		#define SERVO_TIMER_IT			_T2Interrupt
		#define SERVO_TIMER_FLAG		IFS0bits.T2IF
#elif SERVO_TIMER == 3
		#define SERVO_TIMER_RUN_FINE	TIMER3_run_us
		#define SERVO_TIMER_IT			_T3Interrupt
		#define SERVO_TIMER_FLAG		IFS0bits.T3IF
#else
	#error "SERVO_TIMER doit etre 1 2 ou 3"
#endif *//* SERVO_TIMER == n */
	
#define MIN_INTERVAL		1000


/* lutte contre la non déclaration des servos */
/*#ifndef SERVO0
	#define SERVO0	PORTAbits.RA14
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
*//* tous les servos sont déclarés, le code compile */



/*-------------------------------------
	Initialisation des servomoteurs
-------------------------------------*/
void SERVO_init() {
	Uint8 i;
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	// Initialisation de la commande; par défaut 0 (le moteur ne bouge pas)
	for(i=0;i<10;i++)
		m_SERVO_cmd[i] = 0 ;
	TIMER_init();
	// Lancement du timer
	//SERVO_TIMER_RUN_FINE(MIN_INTERVAL);	
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
void _ISR SERVO_TIMER_IT()
{
	//static Uint8 etat=0;
	//static Sint16 blanking=0;

/*	switch(etat)
	{
		case 0:
			blanking = 20000; // 20 ms de periode 
			if (m_SERVO_cmd[etat])
			{
				SERVO0 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 1:
			SERVO0 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO1 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 2:
			SERVO1 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO2 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 3:
			SERVO2 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO3 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 4:
			SERVO3 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO4 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 5:
			SERVO4 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO5 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 6:
			SERVO5 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO6 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 7:
			SERVO6 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO7 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 8:
			SERVO7 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO8 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
			break;
		case 9:
			SERVO8 = 0;
			if (m_SERVO_cmd[etat])
			{
				SERVO9 = 1;
				SERVO_TIMER_RUN_FINE(m_SERVO_cmd[etat]);
				blanking -= m_SERVO_cmd[etat];
			}
			else
			{
				SERVO_TIMER_RUN_FINE(MIN_INTERVAL);
				blanking -= MIN_INTERVAL;
			}
			etat++;
		break;
		default:
			SERVO9 = 0;
			blanking = (blanking >MIN_INTERVAL)?blanking:MIN_INTERVAL;
			SERVO_TIMER_RUN_FINE(blanking);
			etat = 0;
			break;
	}*/	
	//SERVO_TIMER_FLAG=0;
}

#endif /* def USE_SERVO */
