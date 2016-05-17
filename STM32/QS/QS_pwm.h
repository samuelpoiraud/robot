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

/** ----------------  Defines possibles  --------------------
 *  USE_PWM_MODULE				: Activation du module PWM
 *	USE_PWM1					: Activation de la PWM num�ro 1
 *	USE_PWM2					: Activation de la PWM num�ro 2
 *	USE_PWM3					: Activation de la PWM num�ro 3
 *	USE_PWM4					: Activation de la PWM num�ro 4
 *
 *  PWM_FREQ					: Fr�quence du module PWM
 *
 * ----------------  Choses � savoir  --------------------
 *	VERBOSE_MODE				: Verbosit� des modifications des commandes des PWMs
 */

#ifndef QS_PWM_H
	#define QS_PWM_H
	#include "QS_all.h"

#ifdef USE_PWM_MODULE

	#include "QS_clocks_freq.h"

	#ifndef PWM_FREQ
		#error "PWM_FREQ n'est pas d�fini"
	#endif

	#if PWM_FREQ < 0
		#error "PWM_FREQ doit �tre sup�rieur � 0"
	#endif

	#if PWM_FREQ > 200000
		#error "Les ponts en H actuel ne supporte pas cette fr�quence. Si c'est le cas un jour, veuillez passer ce #error en #warning. si tout ce qui existe au robot peut supporter 200Khz, veuillez supprimer ce warning/error"
	#endif

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

	#define PWM_HIGH_SPEED_DUTY 100
	/**
	 * Lance une PWM avec un rapport cyclique donn�
	 * @param duty rapport cyclique en pourcent
	 * @param channel num�ro de la PWM
	 */
	void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel);

	/**
	 * Change la fr�quence des PWM.
	 * Min : 26 Hz
	 * Max : de l'ordre du MHz (selon la config de l'horloge du p�riph�rique).
	 */
	void PWM_set_frequency(Uint32 freq);


	/**
	 * Retourne la valeur du rapport cyclique de la PWM donn�
	 * @param channel num�ro de la PWM
	 */
	Uint8 PWM_get_duty(Uint8 channel);

	#define PWM_FINE_DUTY 25000
	#if PWM_FREQ <= (TIM_CLK2_FREQUENCY_HZ / PWM_FINE_DUTY)
		/**
		 * Lance une PWM avec un rapport cyclique donn�
		 * @param duty rapport cyclique en pour 25000, duty = 25000 <=> rapport cyclique = 100%
		 * @param channel num�ro de la PWM
		 */
		void PWM_run_fine (Uint16 duty, Uint8 channel);
	#endif

#endif

#endif /* ndef QS_PWM_H */
