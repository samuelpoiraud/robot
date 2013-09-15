/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : QS_pwm.h
 *	Package : Qualite Soft
 *	Description : fonction de manipulation des PWM du dsPIC30F6010A
 *					(PWM1H, PWM2H, PWM3H, PWM4H par défaut)
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */
 
#ifndef QS_PWM_H
	#define QS_PWM_H
	#include "QS_all.h"
		
	void PWM_init(void);
	void PWM_stop(Uint8 channel);
	void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel);
	#ifdef FREQ_PWM_50HZ
		void PWM_run_fine (Uint16 duty, Uint8 channel);
		/*	duty en pour 25000, 
		 */
	#endif /* def FREQ_PWM_50HZ */

#endif /* ndef PWH_H */
