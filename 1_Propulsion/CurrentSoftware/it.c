/*
 *  Club Robot ESEO 2006 - 2007
 *  Game Hoover
 *
 *  Fichier : it.c
 *  Package : Asser
 *  Description : fonctions en it...
 *  Auteur : Koubi  (2009) et Nirgal (2009) inspiré du code de Val' (2007)
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
#include "scan_triangle.h"
#include "debug.h"
#if !defined(USE_QSx86) && defined(__dsPIC30F6010A__)
		#include <timer.h>
#endif


#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
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

	//Note : run_us 5000 est beaucoup plus précis que run tout court à 5...
	TIMER1_run_us(PERIODE_IT_ASSER*1000);			//IT trajectoire et Correcteur
	TIMER2_run(100);		//Affichage Leds...
	global.flag_recouvrement_IT = FALSE;
}

		

//TEST non concluant réalisé en 2009 : faire l'odométrie plus souvent (toute les 1ms...)
//mais cela change la vitesse_translation_reelle mesurée... REFLECHIR...
//contactez Samuel ! Ceci est plein de subtilités à la noix de coco...
//je conseille le passage à 1ms que si on passe TOUTE l'IT à cette période... donc que sur un micro plus puissant que le dsPIC30F6010A...
//(car l'IT actuelle peut durer 1ms d'éxécution !)

volatile static global_data_storage_t g2;

									#ifdef X86	
										void fonction_it(void)
									#else
//Sur interruption timer 1...
void _ISR _T1Interrupt()
									#endif
{
#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
	static Uint8 count = 0;
#endif
	LED_USER = 0; //Permet de visualiser a l'oscillo le temps de passage dans l'IT
	TIMER1_AckIT(); /* interruption traitée */
	//A FAIRE EN TOUT DEBUT D'IT POUR AVOIR UNE VITESSE LA PLUS CONSTANTE POSSIBLE...
	ODOMETRY_update();
	
	//Sauvegarde de l'état du système, en mode debug...
	
	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		void debug_save_structure_global(void);
	#endif
	
	
	SECRETARY_process_it();
	WARNER_process_it();	//MAJ des avertisseurs
	JOYSTICK_process_it();
	
		COPILOT_process_it();
		PILOT_process_it();
		SUPERVISOR_process_it();
		SCAN_TRIANGLE_process_it();
	



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

#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
	if(count == 1){
		count = 0;
		ZONE_process_10ms();
	}
	count++;
#endif
}


Uint8 compteur;
																				//gestion des leds
void _ISR _T2Interrupt(void)
{
	#ifdef MODE_PRINTF_TABLEAU
		//Module permettant de visualiser après coup une grande série de valeur quelconque pour chaque IT...
		//dès qu'on est en état ARRET, on affiche les DEBUG_TABLEAU_TAILLE valeurs enregistrées.... c'est long, mais très utile !
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
		// LED_RUN 		: clignote quand le code tourne, reste à 1 en fin de match
		// LED_CAN 		: change d'état à chaque réception de message CAN
		// LED_UART 	: change d'état à chaque réception de caractère UART
		// LED_USER 	: à 1 pendant l'exécution de l'IT, à 0 sinon...
		// LED_USER2 	: clignote pendant une trajectoire
		// LED_ERROR 	: clignote lorsque ROBOT_ERREUR
	switch (SUPERVISOR_get_state())
	{
		case SUPERVISOR_IDLE:
			if(compteur >= 4)
				LED_RUN = 0;
			else
				LED_RUN = 1;
			LED_USER2 = 0;
			LED_ERROR = 0;
		break;

		case SUPERVISOR_TRAJECTORY :
			if(compteur >= 4)
			{
				LED_RUN = 0;
				LED_USER2 = 0;
			}
			else
			{
				LED_RUN = 1;
				LED_USER2 = 1;
			}
			LED_ERROR = 0;
		break;

		case SUPERVISOR_ERROR :
			if(compteur % 2)
			{
				LED_USER2 = 0;
				LED_ERROR = 0;
				LED_RUN = 0;
			}
			else
			{
				LED_USER2 = 1;
				LED_ERROR = 1;
				LED_RUN = 1;
			}
		break;

		case SUPERVISOR_MATCH_ENDED :
			LED_ERROR = 0;
			LED_RUN = 1;
			LED_USER2 = 0;
		break;

		default:
		break;
	}

	TIMER2_AckIT();
}
