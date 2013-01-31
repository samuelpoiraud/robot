/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_pwm.c
 *	Package : QSx86
 *	Description : Fonctions de manipulation de PWM
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20120113
 */

#define QS_PWM_C

#include "QS_pwm.h"


void PWM_init(void)			/* Initialisation du périphérique PWM */
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;
}

void PWM_run(Uint8 duty /* en pourcent */, Uint8 channel)
{
	if (channel && (channel <=4))
	{
		EVE_SetDCMCPWM(channel,duty);
	}
}

#ifdef FREQ_PWM_50HZ
void PWM_run_fine (Uint16 duty /* en pour 25000 */, Uint8 channel)
{
	#ifdef FREQ_INTERNAL_CLK
		#warning PWM_run_fine indisponible avec oscillateur interne
	#endif
	if (channel && (channel <=4))
	{
		EVE_SetDCMCPWM(channel, duty);
	}
}
#endif /* def FREQ_PWM_50HZ */   

void PWM_stop(Uint8 channel)
{
	if (channel && (channel <=4))
	{
		EVE_SetDCMCPWM(channel,0);
	}
}

// Fonction qui lance une PWM de valeur 'duty' sur le canal 'channel'
static void EVE_SetDCMCPWM(Uint8 channel, Uint16 duty)
{
	/*EVE_QS_data_msg_t pwm_data;

	pwm_data.mtype = QUEUE_QS_DATA_MTYPE;
	pwm_data.data_id = QS_PWM;
	pwm_data.data1 = channel;
	pwm_data.data2 = duty;

	// Envoi de l'information à l'IHM
	EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,pwm_data);*/

	// Nouvelle version : on stocke les valeurs dans les variables globales
	// Le message sera envoyé en contenant plus d'informations

	// On indique qu'une nouvelle valeur est dispo sur le port
	switch(channel)
	{
		case PWM_CHANNEL_1:
			eve_global.pwm_power[0] = duty;
			LATEbits.LATE1 = 1;
			break;

		case PWM_CHANNEL_2:
			eve_global.pwm_power[1] = duty;
			LATEbits.LATE3 = 1;
			break;

		case PWM_CHANNEL_3:
			eve_global.pwm_power[2] = duty;
			LATEbits.LATE5 = 1;
			break;

		case PWM_CHANNEL_4:
			eve_global.pwm_power[3] = duty;
			LATEbits.LATE7 = 1;
			break;

		default:
			// Canal de PWM inconnu
			break;
	}
}

