/*
 *	Club Robot ESEO 2008 - 2010
 *	Harry & Anne
 *
 *	Fichier : QS_DCMotorSpeed.h
 *	Package : Qualit� Soft
 *	Description : Gestion des moteurs � courant continu asservis en vitesse
 *  Auteur : Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

/** ----------------  Defines possibles  --------------------
 *	USE_DC_MOTOR_SPEED				: Activation de QS_DCMotorSpeed
 *	DC_MOTOR_SPEED_NUMBER			: D�finit le nombre d'actionneurs asservis
 *	DC_MOTOR_SPEED_TIME_PERIOD		: P�riode d'asserisement [ms]
 *
 * ----------------  Choses � savoir  --------------------
 * La fonction DC_MOTOR_SPEED_process_it() doit �tre appell� p�riodiquement par l'utilisateur
 *	et sa p�riode doit �tre indiqu� dans DC_MOTOR_SPEED_TIME_PERIOD
 */


#ifndef QS_DC_MOTOR_SPEED_H
	#define QS_DC_MOTOR_SPEED_H

	#include "QS_all.h"

	#ifdef USE_DC_MOTOR_SPEED

		typedef Uint8 DC_MOTOR_SPEED_id;
		typedef Uint16 DC_MOTOR_SPEED_speed;	//[RPM]

		//Fonction sans argument renvoyant une vitesse en [RPM]
		typedef DC_MOTOR_SPEED_speed(*DC_MOTOR_SPEED_sensor_read_fun_t)(void);

		typedef enum{
			DC_MOTOR_SPEED_IDLE,
			DC_MOTOR_SPEED_INIT_LAUNCH,
			DC_MOTOR_SPEED_LAUNCH,
			DC_MOTOR_SPEED_RUN,
			DC_MOTOR_SPEED_INIT_RECOVERY,
			DC_MOTOR_SPEED_LAUNCH_RECOVERY,
			DC_MOTOR_SPEED_RUN_RECOVERY,
			DC_MOTOR_SPEED_ERROR
		}DC_MOTOR_SPEED_state_e;	//Pr�cise de quel module on parle avec working_state_e, garde la compatibilit� avec l'ancien nom.

		typedef struct{
			Uint8 pwm_number;				// Numero de la sortie PWM � utiliser (conform�ment � PWM_run(...))
			GPIO_TypeDef* way_latch;		// Adresse du port contenant le bit de sens de controle du pont en H (exemple GPIOA)
			Uint16 way_bit_number;			// Numero du bit de sens dans le port
			bool_e inverse_way;				// Inverser la pin de sens du controle du moteur
			Uint8 max_duty;					// Rapport cyclique maximum de la pwm avec le bit sens � 0

			Sint32 Kp, Ki, Kd, Kv;			// Valeurs des gains pour le PID

			bool_e activateRecovery;		// Activation du mode recovery

			Uint16 timeout;					// Timeout en ms, si la vitesse demand�e n'est pas atteinte avant ce temps, l'asservissement est arret� (�vite de cramer des moteurs). Si cette valeur est 0, il n'y a pas de timeout.

			DC_MOTOR_SPEED_speed epsilon;	// Epsilon, si l'erreur devient inf�rieur � cette variable, la vitesse est consid�r� comme atteinte

			DC_MOTOR_SPEED_sensor_read_fun_t sensorRead;
		}DC_MOTOR_SPEED_config_t;

		/*-------------------------------------
			Fonctions
		-------------------------------------*/

		// Fonction d'initialisation du module QS
		void DC_MOTOR_SPEED_init();

		// Fonction pour modifier la consigne de vitesse du moteur
		void DC_MOTOR_SPEED_setSpeed(DC_MOTOR_SPEED_id id, DC_MOTOR_SPEED_speed speed);

		// Fonction pour obtenir la consigne de vitesse du moteur
		DC_MOTOR_SPEED_speed DC_MOTOR_SPEED_getSpeed(DC_MOTOR_SPEED_id id);

		// Fonction pour obtenir l'�tat du moteur
		DC_MOTOR_SPEED_state_e DC_MOTOR_SPEED_get_state(DC_MOTOR_SPEED_id id);

		// Configurer un moteur CC apr�s initialisation du module QS
		void DC_MOTOR_SPEED_config(DC_MOTOR_SPEED_id id, DC_MOTOR_SPEED_config_t* config);

		// Change les coefs d'asservissement. si le moteur �tait asservis, les nouveaux coef sont automatiquement pris en compte.
		void DC_MOTOR_SPEED_setCoefs(DC_MOTOR_SPEED_id id, Sint16 Kp, Sint16 Ki, Sint16 Kd, Sint16 Kv);

		// R�cup�re les coefs d'asservissement.
		void DC_MOTOR_SPEED_getCoefs(DC_MOTOR_SPEED_id id, Sint16* Kp, Sint16* Ki, Sint16* Kd, Sint16* Kv);

		// R�cup�re les coefficients pwm
		void DC_MOTOR_SPEED_getMaxPwm(DC_MOTOR_SPEED_id id, Uint8 *max_duty);

		// Change les coefficients pwm
		void DC_MOTOR_SPEED_setMaxPwm(DC_MOTOR_SPEED_id id, Uint8 max_duty);

		// Change l'inversion de pilotage du bit de sens
		void DC_MOTOR_SPEED_setInverseWay(DC_MOTOR_SPEED_id id, bool_e inverse_way);

		// Arret de l'asservissement d'un actionneur
		void DC_MOTOR_SPEED_stop(DC_MOTOR_SPEED_id id);

		// Arret de tous les asservissements
		void DC_MOTOR_SPEED_stop_all();

		// Reactivation de l'asservissement d'un actionneur
		void DC_MOTOR_SPEED_restart(DC_MOTOR_SPEED_id id);

		// Reactivation de tous les asservissements
		void DC_MOTOR_SPEED_restart_all();

		// R�initialisation des int�grateurs des actionneurs
		void DC_MOTOR_SPEED_reset_integrator();

		// Fonction � appeler en it pour l'asservissement du moteur
		void DC_MOTOR_SPEED_process_it();

	#endif /* def USE_DCMOTOR_SPEED */
#endif /* ndef QS_DC_MOTOR_SPEED_H */
