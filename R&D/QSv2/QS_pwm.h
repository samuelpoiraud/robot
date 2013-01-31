/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : QS_pwm.h
 *	Package : Qualite Soft
 *	Description : fonction de manipulation des PWM du dsPIC30F6010A
 *					(PWM1H, PWM2H, PWM3H, PWM4H par défaut)
 *	Auteur : Jacen
 *	Version 20081205
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

	#ifdef QS_PWM_C
		/*	Les alias suivants ne servent que dans QS_pwm.c
		 *	ils ne sont pas destinés à être utilisés hors des
		 *	fonctions de gestion des modules PWM.
		 */
		#include <pwm.h>
		#ifdef FREQ_20MHZ
			#ifdef FREQ_PWM_50HZ
				#define	DEMI_PERIODE_PWM	6250
			#elif defined FREQ_PWM_1KHZ
				#define	DEMI_PERIODE_PWM	5000
			#elif defined FREQ_PWM_10KHZ
				#define	DEMI_PERIODE_PWM	500
			#elif defined FREQ_PWM_20KHZ
				#define	DEMI_PERIODE_PWM	250
			#else /* if defined FREQ_PWM_50KHZ */
				#define	DEMI_PERIODE_PWM	100
			#endif /* def FREQ_PWM */
		#else /* if def FREQ_10MHZ || FREQ_40MHZ */
			#ifdef FREQ_PWM_50HZ
				#define	DEMI_PERIODE_PWM	12500
			#elif defined FREQ_PWM_1KHZ
				#define	DEMI_PERIODE_PWM	10000
			#elif defined FREQ_PWM_10KHZ
				#define	DEMI_PERIODE_PWM	1000
			#elif defined FREQ_PWM_20KHZ
				#define	DEMI_PERIODE_PWM	500
			#else /* if defined FREQ_PWM_50KHZ */
				#define	DEMI_PERIODE_PWM	200
			#endif /* def FREQ_PWM */
		#endif	/* def FREQ_XXMHZ */
		
		#ifdef	DISABLE_PWM1H
			#define PWM1H_STATE	PWM_PDIS1H
		#else
			#define PWM1H_STATE	PWM_PEN1H
		#endif /* def DISABLE_PWM1H */
		#ifdef	DISABLE_PWM2H
			#define PWM2H_STATE	PWM_PDIS2H
		#else
			#define PWM2H_STATE	PWM_PEN2H
		#endif /* def DISABLE_PWM2H */
		#ifdef	DISABLE_PWM3H
			#define PWM3H_STATE	PWM_PDIS3H
		#else
			#define PWM3H_STATE	PWM_PEN3H
		#endif /* def DISABLE_PWM3H */
		#ifdef	DISABLE_PWM4H
			#define PWM4H_STATE	PWM_PDIS4H
		#else
			#define PWM4H_STATE	PWM_PEN4H
		#endif /* def DISABLE_PWM4H */

	#endif /* def PWM_C */
#endif /* ndef PWH_H */
