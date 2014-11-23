/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_servo.c
 *	Package : Qualite Software
 *	Description : Driver par timer pour servomoteur
 *	Auteur : Jacen / Anthony(2014)
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#include "QS_servo.h"

#ifdef USE_SERVO

#include "QS_timer.h"
#include "QS_ports.h"
#include "../config/config_pin.h"


#define SPEED_FAST		1000	// Vitesse Max, ne pas dépasser inutile
#define SPEED_SLOW		50		// (1000/50)*20ms = 400ms pour faire un tour complet(1000 positions) du servo
#define SPEED_SNAIL		10		// (1000/10)*20ms = 2sec


typedef struct{
	SERVO_speed_e speed;
	Uint16 value, valueEnd; // Ne varie qu'entre 1000 et 2000
}SERVO_s;

/*variables globales pour le pilote de servos */
static volatile SERVO_s SERVOS[10];

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
	for(i=0;i<10;i++){
		SERVOS[i].value = 0;
		SERVOS[i].valueEnd = 0;
		SERVOS[i].speed = SERVO_FAST;
	}

	TIMER_SRC_TIMER_init();
	// Lancement du timer
	TIMER_SRC_TIMER_start_us(MIN_INTERVAL);
}

/*-------------------------------------
Changement de la commande (entre 0 et 1000 us)
-------------------------------------*/
void SERVO_set_cmd(Uint16 cmd, Uint8 num_servo){
	if(num_servo > 9 && num_servo < 0)
		return;

	if(cmd > 1000)
		SERVOS[num_servo].valueEnd = 1000 + MIN_INTERVAL; // Position Max
	else
		SERVOS[num_servo].valueEnd = cmd + MIN_INTERVAL;
}

/*-------------------------------------
Changement de la vitesse de déplacement
-------------------------------------*/
void SERVO_set_speed(SERVO_speed_e speed, Uint8 num_servo){
	SERVOS[num_servo].speed = speed;
}


void SERVO_set_servo(Uint8 servo,bool_e stateUp){
	switch (servo) {
	#ifdef SERVO0
		case 0:
			GPIO_WriteBit(SERVO0, stateUp);
			break;
	#endif
	#ifdef SERVO1
		case 1:
			GPIO_WriteBit(SERVO1, stateUp);
			break;
	#endif
	#ifdef SERVO2
		case 2:
			GPIO_WriteBit(SERVO2, stateUp);
			break;
	#endif
	#ifdef SERVO3
		case 3:
			GPIO_WriteBit(SERVO3, stateUp);
			break;
	#endif
	#ifdef SERVO4
		case 4:
			GPIO_WriteBit(SERVO4, stateUp);
			break;
	#endif
	#ifdef SERVO5
		case 5:
			GPIO_WriteBit(SERVO5, stateUp);
			break;
	#endif
	#ifdef SERVO6
		case 6:
			GPIO_WriteBit(SERVO6, stateUp);
			break;
	#endif
	#ifdef SERVO7
		case 7:
			GPIO_WriteBit(SERVO7, stateUp);
			break;
	#endif
	#ifdef SERVO8
		case 8:
			GPIO_WriteBit(SERVO8, stateUp);
			break;
	#endif
	#ifdef SERVO9
		case 9:
			GPIO_WriteBit(SERVO9, stateUp);
			break;
	#endif
		default:
			break;
	}
}

/*-------------------------------------
	Timer
-------------------------------------*/

void TIMER_SRC_TIMER_interrupt(){
	static Uint8 etat = 0;
	static Uint8 servo = 0;

	TIMER_SRC_TIMER_resetFlag();

	switch(etat){
		case 0:{

			if(SERVOS[servo].value != SERVOS[servo].valueEnd){ // Si différent, prendre la vitesse en compte pour aller jusqu'à la destination finale

				Uint16 speed;
				switch (SERVOS[servo].speed) {
					case SERVO_SLOW:
						speed = SPEED_SLOW;
						break;
					case SERVO_SNAIL:
						speed = SPEED_SNAIL;
						break;
					case SERVO_FAST:
					default:
						speed = SPEED_FAST;
						break;
				}

				if(SERVOS[servo].value < SERVOS[servo].valueEnd){ // Augmenter la valeur courante
					SERVOS[servo].value += speed;

					if(SERVOS[servo].value > SERVOS[servo].valueEnd)
						SERVOS[servo].value = SERVOS[servo].valueEnd;
				}else{ // Diminuer la valeur courante
					SERVOS[servo].value -= speed;

					if(SERVOS[servo].value < SERVOS[servo].valueEnd)
						SERVOS[servo].value = SERVOS[servo].valueEnd;
				}
			}

			SERVO_set_servo(servo,TRUE); // Passe la pin à 1, si valeur différente de 0
			TIMER_SRC_TIMER_start_us(SERVOS[servo].value);

			etat++;

			}break;
		case 1:
			SERVO_set_servo(servo,FALSE);  // Passe la pin à 0
			TIMER_SRC_TIMER_start_us((TIME_VARIATION - SERVOS[servo].value > 0)? TIME_VARIATION-SERVOS[servo].value : 5);

			if(++servo >= NB_SERVO)
				servo = 0;

		default:
			etat = 0;
			break;
	}
}

#endif /* def USE_SERVO */
