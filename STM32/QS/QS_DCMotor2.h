/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_DCMotor2.h
 *	Package : Qualit� Soft
 *	Description : Gestion des moteurs � courant continu asservis en position
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#ifndef QS_DCMOTOR2_H
	#define QS_DCMOTOR2_H

	#include "QS_all.h"

	#ifdef USE_DCMOTOR2

		//fonction sans argument renvoyant une valeur de type Sint16
		typedef Sint16(*sensor_read_fun_t)(void);

		typedef enum
		{
			DCM_IDLE,
			DCM_WORKING,
			DCM_TIMEOUT
		} DCM_working_state_e;	//Pr�cise de quel module on parle avec working_state_e, garde la compatibilit� avec l'ancien nom.

		typedef struct
		{
			sensor_read_fun_t sensor_read;	//fonction a exectuer pour obtenir la position courante
			Uint8 pwm_number;				// numero de la sortie PWM � utiliser (conform�ment � PWM_run(...))
			Sint16 pos[DCMOTOR_NB_POS];		// valeurs de l'adc pour les diff�rentes positions de l'actionneur
			Uint16 speed[DCMOTOR_NB_POS];	// valeurs de la vitesse � laquel l'ont veut se rendre � la position en unit� de capteur / temps de l'IT appellante de DCM_process_it
			Uint8 way0_max_duty;			// rapport cyclique maximum de la pwm avec le bit sens � 0
			Uint8 way1_max_duty;			// rapport cyclique maximum de la pwm avec le bit sens � 1
			bool_e double_PID;				// TRUE si utilisation du double PID
			Sint16 Kp, Ki, Kd;				// valeurs des gains pour le PID (si double PID -> valeurs pour si on est au dessus de la consigne)
			Sint16 Kp2, Ki2, Kd2;			// (utilis� si double PID) valeurs des gains pour le PID si on est en dessous de la consigne
			GPIO_TypeDef* way_latch;		// adresse du port contenant le bit de sens de controle du pont en H (exemple GPIOA)
			Uint16 way_bit_number;			// numero du bit de sens dans le port
			Uint16 timeout;					// timeout en ms, si la position demand�e n'est pas atteinte avant ce temps, l'asservissement est arret� (�vite de cramer des moteurs). Si cette valeur est 0, il n'y a pas de timeout.
			Uint16 epsilon;					// epsilon, si l'erreur devient inf�rieur � cette variable, la position est consid�r� comme atteinte
			Uint16 large_epsilon;
			bool_e inverseDirection;		// Inverser la pin de sens du controle du moteur
			bool_e stop_on_idle;
		} DCMotor_config_t;

		/*-------------------------------------
			Fonctions
		-------------------------------------*/

		// fonction d'initialisation du module QS
		void DCM_init();

		// Fonction pour modifier la consigne d'un actionneur
		void DCM_goToPos(Uint8 dc_motor_id, Uint8 pos);

		// Indique si le moteur a fini son travail
		DCM_working_state_e DCM_get_state (Uint8 dc_motor_id);

		// configurer un moteur CC apr�s initialisation du module QS
		void DCM_config ( Uint8 dc_motor_id, DCMotor_config_t* config);

		//Change la valeur d'une position sans arr�ter l'asservissement. Si le moteur �tait asservi � cette position, l'asservissement prend en compte le changement
		void DCM_setPosValue(Uint8 dc_motor_id, Uint8 pos_to_update, Sint16 value_pos, Uint16 value_speed);

		//R�cup�re la valeur d'une position
		Sint16 DCM_getPosValue(Uint8 dc_motor_id, Uint8 pos_to_get);

		//R�cup�re la vitesse pour une position
		Uint16 DCM_getPosSpeed(Uint8 dc_motor_id, Uint8 pos_to_get);

		// Change si le moteur fonctionne en double PID
		void DCM_setDoublePID(Uint8 dc_motor_id, bool_e double_PID);

		// R�cup�re si le moteur fonctionne en double PID
		bool_e DCM_getDoublePID(Uint8 dc_motor_id);

		//Change les coefs d'asservissement. si le moteur �tait asservis, les nouveaux coef sont automatiquement pris en compte.
		void DCM_setCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd);

		//R�cup�re les coefs d'asservissement.
		void DCM_getCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd);

		//Change les double coefs d'asservissement. si le moteur �tait asservis, les nouveaux coef sont automatiquement pris en compte.
		void DCM_setDoubleCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd, Sint16 Kp2, Sint16 Ki2, Sint16 Kd2);

		//R�cup�re les double coefs d'asservissement.
		void DCM_getDoubleCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd, Sint16* Kp2, Sint16* Ki2, Sint16* Kd2);

		//R�cup�re les coefficients pwm
		void DCM_getPwmWay(Uint8 dc_motor_id, Uint8 *way0_max_duty, Uint8 *way1_max_duty);
		// Change les coefficients pwm
		void DCM_setPwmWay(Uint8 dc_motor_id, Uint8 way0_max_duty, Uint8 way1_max_duty);

		void DCM_setWayDirection(Uint8 dc_motor_id, bool_e inverse);

		// Arret de l'asservissement d'un actionneur
		void DCM_stop(Uint8 dc_motor_id);

		// Arret de tous les asservissements
		void DCM_stop_all();

		// Reactivation de l'asservissement d'un actionneur
		void DCM_restart(Uint8 dc_motor_id);

		// Reactivation de tous les asservissements
		void DCM_restart_all();

		void DCM_reset_integrator();

		//-----------------------------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// Fonction � appeler en it pour l'asservissement du moteur
		void DCM_process_it();

	#endif /* def USE_DCMOTOR2 */
#endif /* ndef QS_DCMOTOR2_H */
