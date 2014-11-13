/*
 *  Club Robot ESEO 2006 - 2015
 *  Game Hoover, ..., Krusty & Tiny, Pierre & Guy, Holly & Wood
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
#include "QS/QS_ports.h"
#include "config/config_pin.h"
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
#include "avoidance.h"
#include "gyroscope.h"

#if !defined(USE_QSx86) && defined(__dsPIC30F6010A__)
		#include <timer.h>
#endif


#if defined (LCD_TOUCH)
	#include "LCDTouch/LCD.h"
	#include "LCDTouch/zone.h"
#endif

static void display_led(void);

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
	#ifdef USE_GYROSCOPE
		TIMER1_run_us(1000);		// IT du gyro
	#endif
	TIMER2_run_us(1000*PERIODE_IT_ASSER);			//IT trajectoire et Correcteur
	global.flag_recouvrement_IT = FALSE;
}


void _ISR _T1Interrupt(void)
{
	#ifdef USE_GYROSCOPE
		bool_e trash;
		GYRO_get_speed_rotation(&trash, FALSE);	//Acquisition gyro, non suivie d'une exploitation...
	#endif

	TIMER1_AckIT();
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
void _ISR _T2Interrupt()
									#endif
{
	static Uint16 led_display_it = 0;
#if defined (LCD_TOUCH)
	static Uint8 count = 0;
#endif
	GPIO_ResetBits(LED_USER); //Permet de visualiser a l'oscillo le temps de passage dans l'IT
	TIMER2_AckIT(); /* interruption trait�e */

	//A FAIRE EN TOUT DEBUT D'IT POUR AVOIR UNE VITESSE LA PLUS CONSTANTE POSSIBLE...
	ODOMETRY_update();

	//Sauvegarde de l'�tat du syst�me, en mode debug...

	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		void debug_save_structure_global(void);
	#endif

	SECRETARY_process_it();
	WARNER_process_it();	//MAJ des avertisseurs
	JOYSTICK_process_it();
	AVOIDANCE_process_it();

	COPILOT_process_it();
	PILOT_process_it();
	SUPERVISOR_process_it();
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

	#ifdef LCD_TOUCH
		if(count == 1){
			count = 0;
			//ZONE_process_10ms();
			LCD_process_10ms();
		}
		count++;
	#endif

	// Affichage des leds toutes les 500ms
	led_display_it++;
	if(led_display_it >= 500){
		display_led();
		led_display_it = 0;
	}

	g2 = global;
	if(TIMER2_getITStatus())	//L'IT est trop longue ! il y a recouvrement !!!
		global.flag_recouvrement_IT = TRUE;
	GPIO_SetBits(LED_USER);  //Permet de visualiser a l'oscillo le temps de passage dans l'IT
}




Uint8 compteur;
																				//gestion des leds
static void display_led(void)
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
				GPIO_ResetBits(LED_RUN);
			else
				GPIO_SetBits(LED_RUN);

			GPIO_ResetBits(LED_ERROR);
		break;
		case SUPERVISOR_ERROR :
			if(compteur % 2)
			{
				GPIO_ResetBits(LED_ERROR);
				GPIO_ResetBits(LED_RUN);
			}
			else
			{
				GPIO_SetBits(LED_ERROR);
				GPIO_SetBits(LED_RUN);
			}
		break;

		case SUPERVISOR_MATCH_ENDED :
			GPIO_ResetBits(LED_ERROR);
			GPIO_SetBits(LED_RUN);
		break;

		default:
		break;
	}

	if(SECRETARY_is_selftest_validated())
		GPIO_SetBits(LED_SELFTEST);
	else
	{
		if(compteur >= 4)
			GPIO_ResetBits(LED_SELFTEST);
		else
			GPIO_SetBits(LED_SELFTEST);
	}
}
