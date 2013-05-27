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

	#include "QS_clocks_freq.h"

	#ifdef FREQ_PWM_200KHZ
		#error "Les ponts en H actuel ne supporte pas cette fréquence. Si c'est le cas un jour, veuillez passer ce #error en #warning. si tout ce qui existe au robot peut supporter 200Khz, veuillez supprimer ce warning/error"
		#define PWM_FREQ 200000
	#endif
	#ifdef FREQ_PWM_50KHZ
		#define PWM_FREQ 50000
	#endif
	#ifdef FREQ_PWM_20KHZ
		#define PWM_FREQ 20000
	#endif
	#ifdef FREQ_PWM_10KHZ
		#define PWM_FREQ 10000
	#endif
	#ifdef FREQ_PWM_1KHZ
		#define PWM_FREQ 1000
	#endif
	#ifdef FREQ_PWM_50HZ
		#define PWM_FREQ 50
	#endif
		
	/**
	 * Initialise le module PWM
	 */
	void PWM_init(void);

	/**
	 * Stoppe une PWM.
	 *
	 * Identique à PWM_run(0, channel);
	 * @param channel numéro de la PWM à stoppé
	 */
	void PWM_stop(Uint8 channel);

	#define PWM_HIGH_SPEED_DUTY 100
	/**
	 * Lance une PWM avec un rapport cyclique donné
	 * @param duty rapport cyclique en pourcent
	 * @param channel numéro de la PWM
	 */
	void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel);

	#define PWM_FINE_DUTY 25000
#if PWM_FREQ <= (TIM_CLK2_FREQUENCY_HZ / PWM_FINE_DUTY)
	/**
	 * Lance une PWM avec un rapport cyclique donné
	 * @param duty rapport cyclique en pour 25000, duty = 25000 <=> rapport cyclique = 100%
	 * @param channel numéro de la PWM
	 */
	void PWM_run_fine (Uint16 duty, Uint8 channel);
#endif

#endif /* ndef QS_PWM_H */
