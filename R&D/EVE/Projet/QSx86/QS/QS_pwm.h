/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_pwm.h
 *	Package : QSx86
 *	Description : Fonctions de manipulation de PWM
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20120113
 */
 
#ifndef QS_PWM_H
	#define QS_PWM_H
	#include "QS_all.h"
		
	void PWM_init(void);
	void PWM_stop(Uint8 channel);
	void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel);
	#ifdef FREQ_PWM_50HZ
		void PWM_run_fine (Uint16 duty, Uint8 channel);
		//	duty en pour 25000, 

	#endif /* def FREQ_PWM_50HZ */

	#ifdef QS_PWM_C
		// Fonction qui lance une PWM de valeur 'duty' sur le canal 'channel'
		static void EVE_SetDCMCPWM(Uint8 channel, Uint16 duty);
	#endif /* QS_PWM_C */

#endif /* ndef PWH_H */
