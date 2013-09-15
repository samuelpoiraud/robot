/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : QS_pwm.c
 *	Package : Qualite Soft
 *	Description : fonction de manipulation des PWM du dsPIC30F6010A
 *					(PWM1H, PWM2H, PWM3H, PWM4H par defaut)
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#include "QS_pwm.h"

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
	#elif defined FREQ_PWM_40KHZ
		#define	DEMI_PERIODE_PWM	125
	#else /* if defined FREQ_PWM_50KHZ */
		#define	DEMI_PERIODE_PWM	100
	#endif /* def FREQ_PWM */
#elif defined(FREQ_INTERNAL_CLK)
	#ifdef FREQ_PWM_50HZ
		#define	DEMI_PERIODE_PWM	16150
	#elif defined FREQ_PWM_1KHZ
		#define	DEMI_PERIODE_PWM	12920
	#elif defined FREQ_PWM_10KHZ
		#define	DEMI_PERIODE_PWM	1292
	#elif defined FREQ_PWM_20KHZ
		#define	DEMI_PERIODE_PWM	646
	#else /* if defined FREQ_PWM_50KHZ */
		#define	DEMI_PERIODE_PWM	258
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
	#elif defined FREQ_PWM_40KHZ
		#define	DEMI_PERIODE_PWM	250
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

void PWM_init(void)			/* Initialisation du périphérique PWM */
{
	Uint16 config1, config2, config3;
	/* configuration des quatres canaux pour ceux gérés par les générateurs de PWM */

	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;


	/* réglage en fonction de la vitesse d'horloge. Reglage special PWM 50Hz */
	#ifdef FREQ_PWM_50HZ
		#ifdef FREQ_10MHZ
			config1 = PWM_EN & PWM_IDLE_STOP & PWM_OP_SCALE1 & PWM_IPCLK_SCALE16 & PWM_MOD_FREE;	
		#elif defined FREQ_INTERNAL_CLK
			config1 = PWM_EN & PWM_IDLE_STOP & PWM_OP_SCALE1 & PWM_IPCLK_SCALE16 & PWM_MOD_FREE;	
		#elif defined FREQ_20MHZ
			config1 = PWM_EN & PWM_IDLE_STOP & PWM_OP_SCALE1 & PWM_IPCLK_SCALE64 & PWM_MOD_FREE;
		#else /* if defined FREQ_40MHZ */
			config1 = PWM_EN & PWM_IDLE_STOP & PWM_OP_SCALE1 & PWM_IPCLK_SCALE64 & PWM_MOD_FREE;
		#endif
	#else /* cas normal */
		#ifdef FREQ_10MHZ
			config1 = PWM_EN & PWM_IDLE_STOP & PWM_OP_SCALE1 & PWM_IPCLK_SCALE1 & PWM_MOD_FREE;
		#elif defined FREQ_INTERNAL_CLK
			config1 = PWM_EN & PWM_IDLE_STOP & PWM_OP_SCALE1 & PWM_IPCLK_SCALE1 & PWM_MOD_FREE;
		#elif defined FREQ_20MHZ
			config1 = PWM_EN & PWM_IDLE_STOP & PWM_OP_SCALE1 & PWM_IPCLK_SCALE4 & PWM_MOD_FREE;
		#else /* if defined FREQ_40MHZ */
			config1 = PWM_EN & PWM_IDLE_STOP & PWM_OP_SCALE1 & PWM_IPCLK_SCALE4 & PWM_MOD_FREE;
		#endif
	#endif

	#ifdef USE_PWM_L
	config2 = 	PWM_PEN4L & PWM_PEN3L & PWM_PEN2L & PWM_PEN1L & PWM_PDIS4H & PWM_PDIS3H
				& PWM_PDIS2H & PWM_PDIS1H & PWM_MOD1_COMP & PWM_MOD2_COMP & PWM_MOD3_COMP 
				& PWM_MOD4_COMP;
	#else /* utilisation des PWM_H, cas normal. */
	config2 = 	PWM4H_STATE & PWM3H_STATE & PWM2H_STATE & PWM1H_STATE & PWM_PDIS4L & PWM_PDIS3L
				& PWM_PDIS2L & PWM_PDIS1L & PWM_MOD1_COMP & PWM_MOD2_COMP & PWM_MOD3_COMP 
				& PWM_MOD4_COMP;
	#endif /* def USE_PWM_L*/

	config3 = PWM_SEVOPS1 & PWM_OSYNC_PWM & PWM_UEN;
	
	OpenMCPWM(DEMI_PERIODE_PWM, 0, config1, config2, config3);
}

void PWM_run(Uint8 duty /* en pourcent */, Uint8 channel)
{
	#ifdef USE_PWM_L
	duty = duty <100 ? 100-duty:1;
	#endif
	#ifdef FREQ_20MHZ
	if (channel && (channel <=4))
	{
		#ifdef FREQ_PWM_50HZ
			SetDCMCPWM(channel, duty * 125, 0);
		#elif defined FREQ_PWM_1KHZ
			SetDCMCPWM(channel, duty * 100, 0);
		#elif defined FREQ_PWM_10KHZ
			SetDCMCPWM(channel, duty * 10, 0);
		#elif defined FREQ_PWM_20KHZ
			SetDCMCPWM(channel, duty * 5, 0);
		#else /* if defined FREQ_PWM_50KHZ */
			SetDCMCPWM(channel, duty*2, 0);
		#endif /* def FREQ_PWM */
	}
	#elif defined(FREQ_INTERNAL_CLK)
	if (channel && (channel <=4))
	{
		#ifdef FREQ_PWM_50HZ
			SetDCMCPWM(channel, duty * 323, 0);
		#elif defined FREQ_PWM_1KHZ
			SetDCMCPWM(channel, duty * 258, 0);
		#elif defined FREQ_PWM_10KHZ
			SetDCMCPWM(channel, duty * 26, 0);
		#elif defined FREQ_PWM_20KHZ
			SetDCMCPWM(channel, duty * 13, 0);
		#else /* if defined FREQ_PWM_50KHZ */
			SetDCMCPWM(channel, duty*5, 0);
		#endif /* def FREQ_PWM */
	}
	#else /* if def FREQ_10MHZ || FREQ_40MHZ */
	if (channel && (channel <=4))
	{
		#ifdef FREQ_PWM_50HZ
			SetDCMCPWM(channel, duty * 250, 0);
		#elif defined FREQ_PWM_1KHZ
			SetDCMCPWM(channel, duty * 200, 0);
		#elif defined FREQ_PWM_10KHZ
			SetDCMCPWM(channel, duty * 20, 0);
		#elif defined FREQ_PWM_20KHZ
			SetDCMCPWM(channel, duty * 10, 0);
		#else /* if defined FREQ_PWM_50KHZ */
			SetDCMCPWM(channel, duty*4, 0);
		#endif /* def FREQ_PWM */
	}
	#endif
}

#ifdef FREQ_PWM_50HZ
void PWM_run_fine (Uint16 duty /* en pour 25000 */, Uint8 channel)
{
	#ifdef USE_PWM_L
	duty = duty < 25000 ? 25000-duty:1;
	#endif
	#ifdef FREQ_INTERNAL_CLK
		#warning PWM_run_fine indisponible avec oscillateur interne
	#endif
	#ifdef FREQ_20MHZ
		duty /= 2;
	#endif
	if (channel && (channel <=4))
	{
		SetDCMCPWM(channel, duty, 0);
	}
}
#endif /* def FREQ_PWM_50HZ */   

void PWM_stop(Uint8 channel)
{
	if (channel && (channel <=4))
		SetDCMCPWM(channel,0,0);
}
