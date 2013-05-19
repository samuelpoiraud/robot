/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : QS_pwm.h
 *	Package : Qualite Soft
 *	Description : fonction de manipulation des PWM du dsPIC30F6010A
 *					(PWM1H, PWM2H, PWM3H, PWM4H par d�faut)
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#ifndef QS_PWM_H
	#define QS_PWM_H
	#include "QS_all.h"
		
	/**
	 * Initialise le module PWM
	 */
	void PWM_init(void);

	/**
	 * Stoppe une PWM.
	 *
	 * Identique � PWM_run(0, channel);
	 * @param channel num�ro de la PWM � stopp�
	 */
	void PWM_stop(Uint8 channel);

	/**
	 * Lance une PWM avec un rapport cyclique donn�
	 * @param duty rapport cyclique en pourcent
	 * @param channel num�ro de la PWM
	 */
	void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel);

	/**
	 * Lance une PWM avec un rapport cyclique donn�
	 * @param duty rapport cyclique en pour 25000, duty = 25000 <=> rapport cyclique = 100%
	 * @param channel num�ro de la PWM
	 */
	void PWM_run_fine (Uint16 duty, Uint8 channel);

#endif /* ndef QS_PWM_H */
