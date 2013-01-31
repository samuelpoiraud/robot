/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *	Fichier : QS_DCMotor.h
 *	Package : Qualité Soft
 *	Description : Gestion des moteurs à courant continu asservis
 *				(mesure de position par potentiomètre)
 *  Auteur : Gwenn, Jacen
 *  Version 20090414
 */

#ifndef QS_DCMOTOR_H
	#define QS_DCMOTOR_H
	
	#include "../QS/QS_all.h"
	
	#ifdef USE_DCMOTOR
		/*-------------------------------------
			Fonctions
		-------------------------------------*/
	
		// fonction d'initialisation du module QS
		void DCM_init();
	
		// Fonction pour modifier la consigne d'un actionneur
		void DCM_goToPos(Uint8 dc_motor_id, Uint8 pos);
	
		// configurer un moteur CC après initialisation du module QS
		void DCM_config (	Uint8 dc_motor_id,
						Uint8 adc_channel,		// voie à lire pour avoir le retour du moteur dans ADC_getValue(...)
						Uint8 pwm_number,		// numero de la sortie PWM à utiliser (conformément à PWM_run(...))
						Uint16 pos[DCMOTOR_NB_POS],	// valeurs de l'adc pour les différentes positions de l'actionneur
						Uint8 way0_max_duty,	// rapport cyclique maximum de la pwm avec le bit sens à 0
						Uint8 way1_max_duty,	// rapport cyclique maximum de la pwm avec le bit sens à 1
						Sint16 Kp,
						Sint16 Ki,				// valeurs des gains pour le PID
						Sint16 Kd,
						Uint16 timeout,			// timeout en ms
						Uint16* way_latch,		// adresse du port contenant le bit de sens de controle du pont en H
						Uint8 way_bit_number	// numero du bit de sens dans le port
					);
	
	
		/*-------------------------------------
			Dépendances
		-------------------------------------*/
		
		#ifdef QS_DCMOTOR_C
			#include "../QS/QS_pwm.h"
			#include "../QS/QS_timer.h"
			#include "../QS/QS_adc.h"
			#include "../QS/QS_can.h"
			#include "../QS/QS_CANmsgList.h"
		#endif /* def DCM_C	*/
			
	#endif /* def USE_DCMOTOR */
#endif /* ndef QS_DCMOTOR_H */
