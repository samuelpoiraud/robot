/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_DCMotor2.h
 *	Package : Qualité Soft
 *	Description : Gestion des moteurs à courant continu asservis en position
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

/** ----------------  Defines possibles  --------------------
 *	USE_DCMOTOR2				: Activation de QS_DCMotor2
 *	DCM_NUMBER					: Définit le nombre d'actionneurs asservis
 *	DCM_TIME_PERIOD				: Période d'asserisement [ms]
 *	DCMOTOR_NB_POS				: Nombre maximum de positions à gérer par moteur
 *
 * ----------------  Choses à savoir  --------------------
 * La fonction DCM_process_it() doit être appellé périodiquement par l'utilisateur
 *	et sa période doit être indiqué dans DCM_TIME_PERIOD
 */


#ifndef QS_DCMOTOR2_H
	#define QS_DCMOTOR2_H

	#include "QS_all.h"

	#ifdef USE_DCMOTOR2

		//Fonction sans argument renvoyant une valeur de type Sint16
		typedef Sint16(*sensor_read_fun_t)(void);

		typedef enum
		{
			DCM_IDLE,
			DCM_WORKING,
			DCM_TIMEOUT
		} DCM_working_state_e;	//Précise de quel module on parle avec working_state_e, garde la compatibilité avec l'ancien nom.

		typedef struct
		{
			sensor_read_fun_t sensor_read;	// Fonction a exectuer pour obtenir la position courante
			Uint8 pwm_number;				// Numero de la sortie PWM à utiliser (conformément à PWM_run(...))
			GPIO_TypeDef* way_latch;		// Adresse du port contenant le bit de sens de controle du pont en H (exemple GPIOA)
			Uint16 way_bit_number;			// Numero du bit de sens dans le port
			bool_e inverse_way;				// Inverser la pin de sens du controle du moteur
			Uint8 way0_max_duty;			// Rapport cyclique maximum de la pwm avec le bit sens à 0
			Uint8 way1_max_duty;			// Rapport cyclique maximum de la pwm avec le bit sens à 1

			Sint16 pos[DCMOTOR_NB_POS];		// Valeurs de l'adc pour les différentes positions de l'actionneur

			bool_e double_PID;				// TRUE si utilisation du double PID
			Sint32 Kp, Ki, Kd;				// Valeurs des gains pour le PID (En mode double PID ces valeurs correspondent aux gains au dessus de la consigne)
			Sint32 Kp2, Ki2, Kd2;			// Seulement utilisé si 'double_PID' est à TRUE. Valeurs des gains pour le PID si on est en dessous de la consigne

			Uint16 timeout;					// Timeout en ms, si la position demandée n'est pas atteinte avant ce temps, l'asservissement est arreté (évite de cramer des moteurs). Si cette valeur est 0, il n'y a pas de timeout.

			Uint16 dead_zone;				// Zone morte dans laquelle l'asservissement est désactivé

			Uint16 epsilon;					// Epsilon, si l'erreur devient inférieur à cette variable, la position est considéré comme atteinte
			Uint16 large_epsilon;			// Au timeout de la position, si l'erreur est inférieur à large_epsilon, la position est considéré comme atteinte
		} DCMotor_config_t;

		/*-------------------------------------
			Fonctions
		-------------------------------------*/

		// Fonction d'initialisation du module QS
		void DCM_init();

		// Fonction pour modifier la consigne d'un actionneur
		void DCM_goToPos(Uint8 dc_motor_id, Uint8 pos);

		// Indique si le moteur a fini son travail
		DCM_working_state_e DCM_get_state (Uint8 dc_motor_id);

		// Configurer un moteur CC après initialisation du module QS
		void DCM_config ( Uint8 dc_motor_id, DCMotor_config_t* config);

		// Change la valeur d'une position sans arrêter l'asservissement. Si le moteur était asservi à cette position, l'asservissement prend en compte le changement
		void DCM_setPosValue(Uint8 dc_motor_id, Uint8 pos_to_update, Sint16 value_pos);

		// Récupère la valeur d'une position
		Sint16 DCM_getPosValue(Uint8 dc_motor_id, Uint8 pos_to_get);

		// Change si le moteur fonctionne en double PID
		void DCM_setDoublePID(Uint8 dc_motor_id, bool_e double_PID);

		// Récupère si le moteur fonctionne en double PID
		bool_e DCM_getDoublePID(Uint8 dc_motor_id);

		// Change les coefs d'asservissement. si le moteur était asservis, les nouveaux coef sont automatiquement pris en compte.
		void DCM_setCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd);

		// Récupère les coefs d'asservissement.
		void DCM_getCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd);

		// Change les double coefs d'asservissement. si le moteur était asservis, les nouveaux coef sont automatiquement pris en compte.
		void DCM_setDoubleCoefs(Uint8 dc_motor_id, Sint16 Kp, Sint16 Ki, Sint16 Kd, Sint16 Kp2, Sint16 Ki2, Sint16 Kd2);

		// Récupère les double coefs d'asservissement.
		void DCM_getDoubleCoefs(Uint8 dc_motor_id, Sint16* Kp, Sint16* Ki, Sint16* Kd, Sint16* Kp2, Sint16* Ki2, Sint16* Kd2);

		// Récupère les coefficients pwm
		void DCM_getPwmWay(Uint8 dc_motor_id, Uint8 *way0_max_duty, Uint8 *way1_max_duty);

		// Change les coefficients pwm
		void DCM_setPwmWay(Uint8 dc_motor_id, Uint8 way0_max_duty, Uint8 way1_max_duty);

		// Change l'inversion de pilotage du bit de sens
		void DCM_setInverseWay(Uint8 dc_motor_id, bool_e inverse_way);

		// Arret de l'asservissement d'un actionneur
		void DCM_stop(Uint8 dc_motor_id);

		// Arret de tous les asservissements
		void DCM_stop_all();

		// Reactivation de l'asservissement d'un actionneur
		void DCM_restart(Uint8 dc_motor_id);

		// Reactivation de tous les asservissements
		void DCM_restart_all();

		// Réinitialisation des intégrateurs des actionneurs
		void DCM_reset_integrator();

		// Fonction à appeler en it pour l'asservissement du moteur
		void DCM_process_it();

	#endif /* def USE_DCMOTOR2 */
#endif /* ndef QS_DCMOTOR2_H */
