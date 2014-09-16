/*
 *  Club Robot ESEO 2006 - 2014
 *  Game Hoover, ..., Krusty & Tiny, Pierre & Guy
 *
 *  Fichier : it.c
 *  Package : Asser
 *  Description : fonctions en it...
 *  Auteur : Koubi  (2009) et Nirgal (2009) inspir� du code de Val' (2007), Herzaeone(2014)
 *  Version 200904
 */

#define _IT_C

#include "it.h"



#include "QS/QS_timer.h"
#include "odometry.h"
#include "copilot.h"
#include "pilot.h"
#include "supervisor.h"
#include "warner.h"
#include "joystick.h"
#include "secretary.h"
#include "debug.h"
#include "hokuyo.h"
#include "main.h"
#include "detection.h"

#if !defined(USE_QSx86) && defined(__dsPIC30F6010A__)
		#include <timer.h>
#endif


#if defined (LCD_TOUCH)
	#include "LCDTouch/LCD.h"
	#include "LCDTouch/zone.h"
#endif


void IT_init(void)
{
	//Et c'est parti pour les it !!!
	//Tache d'interruption principale...
	TIMER_init();
#if !defined(USE_QSx86) && defined(__dsPIC30F6010A__)
	ConfigIntTimer1(T1_INT_PRIOR_5 & T1_INT_OFF);
//	ConfigIntTimer3(T3_INT_PRIOR_1 & T3_INT_OFF);
#elif !defined(USE_QSx86) && defined(STM32F40XX)
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 10);
#endif

	//Note : run_us 5000 est beaucoup plus pr�cis que run tout court � 5...
	TIMER1_run_us(PERIODE_IT_ASSER*1000);			//IT trajectoire et Correcteur
	TIMER2_run(100);		//Affichage Leds...
	global.flag_recouvrement_IT = FALSE;
}



//TEST non concluant r�alis� en 2009 : faire l'odom�trie plus souvent (toute les 1ms...)
//mais cela change la vitesse_translation_reelle mesur�e... REFLECHIR...
//contactez Samuel ! Ceci est plein de subtilit�s � la noix de coco...
//je conseille le passage � 1ms que si on passe TOUTE l'IT � cette p�riode... donc que sur un micro plus puissant que le dsPIC30F6010A...
//(car l'IT actuelle peut durer 1ms d'�x�cution !)

volatile static global_data_storage_t g2;

									#ifdef X86
										void fonction_it(void)
									#else
//Sur interruption timer 1...
void _ISR _T1Interrupt()
									#endif
{
#if defined (LCD_TOUCH)
	static Uint8 count = 0;
#endif
	LED_USER = 0; //Permet de visualiser a l'oscillo le temps de passage dans l'IT
	TIMER1_AckIT(); /* interruption trait�e */
	//A FAIRE EN TOUT DEBUT D'IT POUR AVOIR UNE VITESSE LA PLUS CONSTANTE POSSIBLE...
	ODOMETRY_update();

	//Sauvegarde de l'�tat du syst�me, en mode debug...

	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		void debug_save_structure_global(void);
	#endif


	SECRETARY_process_it();
	WARNER_process_it();	//MAJ des avertisseurs
	JOYSTICK_process_it();

	COPILOT_process_it();
	PILOT_process_it();
	SUPERVISOR_process_it();
	AVOIDANCE_process_it();
	MAIN_process_it(PERIODE_IT_ASSER);

	#ifdef USE_HOKUYO
		HOKUYO_process_it(PERIODE_IT_ASSER);
	#endif

	DETECTION_process_it();

	#ifdef MODE_PRINTF_TABLEAU
		debug_print_tableau();
	#endif

	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
		DEBUG_process_it();
	#endif

	g2 = global;
	LED_USER = 1; //Permet de visualiser a l'oscillo le temps de passage dans l'IT
	if(TIMER1_getITStatus())	//L'IT est trop longue ! il y a recouvrement !!!
		global.flag_recouvrement_IT = TRUE;

#ifdef LCD_TOUCH
	if(count == 1){
		count = 0;
		//ZONE_process_10ms();
		LCD_process_10ms();
	}
	count++;
#endif
}


Uint8 compteur;
																				//gestion des leds
void _ISR _T2Interrupt(void)
{
	#ifdef MODE_PRINTF_TABLEAU
		//Module permettant de visualiser apr�s coup une grande s�rie de valeur quelconque pour chaque IT...
		//d�s qu'on est en �tat ARRET, on affiche les DEBUG_TABLEAU_TAILLE valeurs enregistr�es.... c'est long, mais tr�s utile !
		if(global.arrived == ROBOT_ARRIVE)
		{
			global.mode_printf_tableau_enregistrer = FALSE;
			for(global.debug_index_read = 0; global.debug_index_read < DEBUG_TABLEAU_TAILLE; global.debug_index_read++)
				printf("%d > %d \t %d \t %d \n", global.debug_index_read, global.debug_tableau[global.debug_index_read].x, global.debug_tableau[global.debug_index_read].y, global.debug_tableau[global.debug_index_read].teta);
			global.mode_printf_tableau_enregistrer = TRUE;
		}
	#endif

	compteur++;
	if(compteur > 7)
		compteur=0;

		//Explications :
		// LED_RUN 		: clignote quand le code tourne, reste � 1 en fin de match
		// LED_CAN 		: change d'�tat � chaque r�ception de message CAN
		// LED_UART 	: change d'�tat � chaque r�ception de caract�re UART
		// LED_USER 	: � 1 pendant l'ex�cution de l'IT, � 0 sinon...
		// LED_SELFTEST	: clignote tant que le selftest n'est pas valid�
		// LED_ERROR 	: clignote lorsque ROBOT_ERREUR
	switch (SUPERVISOR_get_state())
	{
		case SUPERVISOR_IDLE:
			//no break;
		case SUPERVISOR_TRAJECTORY :
			if(compteur >= 4)
				LED_RUN = 0;
			else
				LED_RUN = 1;
			LED_ERROR = 0;
		break;
		case SUPERVISOR_ERROR :
			if(compteur % 2)
			{
				LED_ERROR = 0;
				LED_RUN = 0;
			}
			else
			{
				LED_ERROR = 1;
				LED_RUN = 1;
			}
		break;

		case SUPERVISOR_MATCH_ENDED :
			LED_ERROR = 0;
			LED_RUN = 1;
		break;

		default:
		break;
	}

	if(SECRETARY_is_selftest_validated())
		LED_SELFTEST = 1;
	else
	{
		if(compteur >= 4)
			LED_SELFTEST = 0;
		else
			LED_SELFTEST = 1;
	}

	TIMER2_AckIT();
}
