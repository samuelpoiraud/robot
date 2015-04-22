/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : motor.c
 *	Package : Projet Balise Récepteur US
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit être le plus constante possible.
 *	Auteur : Nirgal
 *	Version 201203
 */

#include "motor.h"
#include "QS/QS_qei.h"
#include "QS/QS_pwm.h"
#include <pwm.h>
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "Global_config.h"
#include "QS/QS_rf.h"
#include "SynchroRF.h"

#define AROUND(x)   ((((x)>0)?((Sint16)(x)+0.5):((Sint16)(x)-0.5)))

#define NB_EMISSION_TOUR	(Sint16)(2)	 // Nombre d'emission par tour
#define PERIOD_IT_MOTOR		 2 //ms
#define KV		 	(Sint16)(8)
#define KP			(Sint16)(32)
#define KD			(Sint16)(4)
#define KI			(Sint16)(2)
#define COMMAND		(Sint16)(AROUND(2048 * (NB_EMISSION_TOUR * 1000. / (PERIODE_FLASH*2 - NO_FLASH_TIME*2))/500.))  // pas par 2ms
								//		 arrondi(2048*	(2 * 1000 / (50*2 - 4*2) )   / 500   )
									  //	 cela donne :  85
										//2048*(nb_tour*1000/temps_emission) pas par secondes
										//(nb_tour*1000/temps_emission) tours par seconde

#define FILTER_BATTERY			8
static volatile Uint16 duty_filtered = 0;

#define NB_IT_PER_TURN			(Uint8)(30)	//it = 2ms, 1 tour = 60ms => 30 it par tour.
#define	NB_ENCODER_STEP_PER_IT	(Uint8)(68)	

void MOTOR_init(void)
{
	QEI_init();
	PWM_init();
}

//A appeler toutes les 2 ms.
//ce choix est arbitraire, et très lié au fait que le reste du code utilise également avec une IT 2ms
void MOTOR_process_it(void)
{
	static Sint16 count_prec = 0;
	static Sint16 error_prec = 0;
	static Sint16 sum_error = 0;
	Sint16 count, error, delta_error;
	Sint16 duty;
	Sint16 speed;

	count = QEI_get_count();		//Lecture codeur.
	
	speed = absolute(count - count_prec);	//Calcul vitesse

	error = COMMAND - speed;		//Calcul de l'erreur sur la vitesse
	
	delta_error = error - error_prec;	//Calcul de la dérivée de l'erreur sur la vitesse
	
	sum_error += (speed < 10)?0:error;	//Calcul de l'intégrale de l'erreur sur la vitesse (avec sécurité si vitesse faible !)

	//Calcul du rapport cyclique de la PWM à envoyer au moteur.
	duty = (		KV * COMMAND		//85*8 -> 680   -> /16 -> 42 pour 500 -> 8% de PWM
				+ 	KP * error 
				+ 	KD * delta_error
				+ 	KI * sum_error
			)/16;

	if(duty <0)	//Ecretage mini
		duty = 1;
	if(duty > 500)
		duty = 500;			//Duty s'exprime de 0 à 500 (0 à 100%)
	
	SetDCMCPWM(2, duty, 0);	
	
	//On sauvegarde pour le prochain tour.
	count_prec = count;
	error_prec = error;

	//duty est en [0 à 500]
	//on veut un nombre de [0 à 100] (c'est le plus simple...et mieux pour la RF) -> on divise par 5.
	//puis, on filtre en prenant 8/128 de la valeur actuelle    +    120/128 de la valeur précédente.
	duty_filtered = (duty_filtered*(128-FILTER_BATTERY) + (Uint16)(duty/5)*FILTER_BATTERY)  /  128;
	
	//Vérification de non débordement avec les valeurs max :
					//(	100 * 120    +    ( 500/5  * 8  )   )		/   128
					//				12800							/	128
					//				c'est bon !
}	


Uint8 MOTOR_get_duty_filtered(void)
{
	return (Uint8)(duty_filtered);	//[de 0 à 100]
}

