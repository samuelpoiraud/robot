/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_DCMotor.h
 *	Package : Qualité Soft
 *	Description : Gestion des moteurs à courant continu asservis en position
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#ifndef QS_DCMOTOR_H
	#define QS_DCMOTOR_H
	
	#include "QS_all.h"
	
	#ifdef USE_DCMOTOR

		//fonction sans argument renvoyant une valeur de type Sint16
		typedef Sint16(*sensor_read_fun_t)(void);

		typedef enum
		{
			DCM_IDLE,
			DCM_WORKING,
			DCM_TIMEOUT
		}working_state_e;
		typedef working_state_e DCM_working_state_e;	//Précise de quel module on parle avec working_state_e, garde la compatibilité avec l'ancien nom.

		typedef struct
		{
			sensor_read_fun_t sensor_read;	//fonction a exectuer pour obtenir la position courante
			Uint8 pwm_number;				// numero de la sortie PWM à utiliser (conformément à PWM_run(...))
			Sint16 pos[DCMOTOR_NB_POS];		// valeurs de l'adc pour les différentes positions de l'actionneur
			Uint8 way0_max_duty;			// rapport cyclique maximum de la pwm avec le bit sens à 0
			Uint8 way1_max_duty;			// rapport cyclique maximum de la pwm avec le bit sens à 1
			Sint16 Kp, Ki, Kd;				// valeurs des gains pour le PID
			Uint16 timeout;					// timeout en ms
			Uint16* way_latch;				// adresse du port contenant le bit de sens de controle du pont en H
			Uint8 way_bit_number;			// numero du bit de sens dans le port
		}DCMotor_config_t;	

		/*-------------------------------------
			Fonctions
		-------------------------------------*/
	
		// fonction d'initialisation du module QS
		void DCM_init();
	
		// Fonction pour modifier la consigne d'un actionneur
		void DCM_goToPos(Uint8 dc_motor_id, Uint8 pos);

		// Indique si le moteur a fini son travail
		working_state_e DCM_get_state (Uint8 dc_motor_id);
	
		// configurer un moteur CC après initialisation du module QS
		void DCM_config ( Uint8 dc_motor_id, DCMotor_config_t* config);
	
		// Arret de l'asservissement d'un actionneur
		void DCM_stop(Uint8 dc_motor_id);

		// Arret de tous les asservissements
		void DCM_stop_all();

		// Reactivation de l'asservissement d'un actionneur
		void DCM_restart(Uint8 dc_motor_id);

		// Reactivation de tous les asservissements
		void DCM_restart_all();

	#endif /* def USE_DCMOTOR */
#endif /* ndef QS_DCMOTOR_H */
