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

#ifdef USE_SERVO

#include "QS_timer.h"
#include "QS_ports.h"

#ifdef NEW_CONFIG_ORGANISATION
	#include "config_pin.h"
#endif

/*variables globales pour le pilote de servos */
static volatile Sint16 m_SERVO_cmd[10];

void SERVO_set_servo(Uint8 servo,bool_e stateUp);

#include "QS_outputlog.h"

/*-------------------------------------
	Gestion des servomoteurs
-------------------------------------*/

// Fonction d'interruption du timer
#if !defined(SERVO_TIMER)
	#error "SERVO_TIMER doit etre 1 2 3 4 ou 5, le watchdog n'est pas utilisable avec ce module"
#else
	#define TIMER_SRC_TIMER_ID SERVO_TIMER

	#include "QS_setTimerSource.h"
#endif

#define MIN_INTERVAL		1000
#define TIME_VARIATION		2000
#define NB_SERVO			10		// Ne pas changer, sinon ne risque plus d'avoir 20ms

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

	TIMER_SRC_TIMER_init();
	// Lancement du timer
	TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
}

/*-------------------------------------
Changement de la commande (entre 0 et 2000 us)
-------------------------------------*/
void SERVO_set_cmd(Uint16 cmd, Uint8 num_servo){
	if(cmd > 2000)
		m_SERVO_cmd[num_servo] = 2000;
	else
		m_SERVO_cmd[num_servo] = cmd;
}


void SERVO_set_servo(Uint8 servo,bool_e stateUp){
	switch (servo) {
	#ifdef SERVO0
		case 0:
			SERVO0 = stateUp;
			break;
	#endif
	#ifdef SERVO1
		case 1:
			SERVO1 = stateUp;
			break;
	#endif
	#ifdef SERVO2
		case 2:
			SERVO2 = stateUp;
			break;
	#endif
	#ifdef SERVO3
		case 3:
			SERVO3 = stateUp;
			break;
	#endif
	#ifdef SERVO4
		case 4:
			SERVO4 = stateUp;
			break;
	#endif
	#ifdef SERVO5
		case 5:
			SERVO5 = stateUp;
			break;
	#endif
	#ifdef SERVO6
		case 6:
			SERVO6 = stateUp;
			break;
	#endif
	#ifdef SERVO7
		case 7:
			SERVO7 = stateUp;
			break;
	#endif
	#ifdef SERVO8
		case 8:
			SERVO8 = stateUp;
			break;
	#endif
	#ifdef SERVO9
		case 9:
			SERVO9 = stateUp;
			break;
	#endif
			break;
		default:
			break;
	}
}

/*-------------------------------------
	Timer
-------------------------------------*/

void TIMER_SRC_TIMER_interrupt(){

	debug_var = 1;

	static Uint8 etat=0;
	static Uint8 servo=0;

	switch(etat){
		case 0:
			if (m_SERVO_cmd[servo]){ // Passe la pin à 1, si valeur différente de 0
				SERVO_set_servo(servo,TRUE);
				TIMER_SRC_TIMER_start_us(m_SERVO_cmd[servo]);
			}else
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);

			etat++;

			break;
		case 1:
			SERVO_set_servo(servo,FALSE);  // Passe la pin à 0
			if (m_SERVO_cmd[servo])
				TIMER_SRC_TIMER_start_us((TIME_VARIATION - m_SERVO_cmd[servo] > 0)? TIME_VARIATION-m_SERVO_cmd[servo] : 5);
			else
				TIMER_SRC_TIMER_start_us(MIN_INTERVAL);

			servo++;

			if(servo >= NB_SERVO)
				servo = 0;

		default:
			etat = 0;
			break;
	}

	TIMER_SRC_TIMER_resetFlag();

	debug_var = 0;
}

#endif /* def USE_SERVO */
