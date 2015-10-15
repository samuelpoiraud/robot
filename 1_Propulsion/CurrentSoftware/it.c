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
#include "scan_cup.h"

typedef enum{
	IT_STATE_NONE = 0,
	IT_STATE_ODOMETRY,
	IT_STATE_SECRETARY,
	IT_STATE_WARNER,
	IT_STATE_JOYSTICK,
	IT_STATE_AVOIDANCE,
	IT_STATE_COPILOT,
	IT_STATE_PILOT,
	IT_STATE_SUPERVISOR,
	IT_STATE_MAIN,
	IT_STATE_HOKUYO,
	IT_STATE_DETECTION,
	IT_STATE_CHOC_DETECTION,
	IT_STATE_SCAN_CUP,
	IT_STATE_DEBUG,
	IT_STATE_LCD
}it_state_e;

void IT_test_state(time32_t time_begin, it_state_e it_state, bool_e *over_time);
static void display_led(void);

void IT_init(void)
{
	//Et c'est parti pour les it !!!
	//Tache d'interruption principale...
	TIMER_init();

	//Note : run_us 5000 est beaucoup plus pr�cis que run tout court � 5...
	#ifdef USE_GYROSCOPE
		TIMER1_run_us(1000);		// IT du gyro
	#endif
	TIMER2_run_us(1000*PERIODE_IT_ASSER);			//IT trajectoire et Correcteur
	global.debug.recouvrement_IT = FALSE;
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
	time32_t begin_it_time = global.absolute_time;
	bool_e first_overtime = FALSE;
	GPIO_ResetBits(LED_USER); //Permet de visualiser a l'oscillo le temps de passage dans l'IT
	TIMER2_AckIT(); /* interruption trait�e */

	//A FAIRE EN TOUT DEBUT D'IT POUR AVOIR UNE VITESSE LA PLUS CONSTANTE POSSIBLE...
	ODOMETRY_update();
	IT_test_state(begin_it_time, IT_STATE_ODOMETRY, &first_overtime);

	//Sauvegarde de l'�tat du syst�me, en mode debug...

	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		void debug_save_structure_global(void);
	#endif

	SECRETARY_process_it();
	IT_test_state(begin_it_time, IT_STATE_SECRETARY, &first_overtime);

	WARNER_process_it();	//MAJ des avertisseurs
	IT_test_state(begin_it_time, IT_STATE_WARNER, &first_overtime);

	JOYSTICK_process_it();
	IT_test_state(begin_it_time, IT_STATE_JOYSTICK, &first_overtime);

	AVOIDANCE_process_it();
	IT_test_state(begin_it_time, IT_STATE_AVOIDANCE, &first_overtime);

	COPILOT_process_it();
	IT_test_state(begin_it_time, IT_STATE_COPILOT, &first_overtime);

	PILOT_process_it();
	IT_test_state(begin_it_time, IT_STATE_PILOT, &first_overtime);

	SUPERVISOR_process_it();
	IT_test_state(begin_it_time, IT_STATE_SUPERVISOR, &first_overtime);

	MAIN_process_it(PERIODE_IT_ASSER);
	IT_test_state(begin_it_time, IT_STATE_MAIN, &first_overtime);

	#ifdef USE_HOKUYO
		HOKUYO_process_it(PERIODE_IT_ASSER);
	#endif
	IT_test_state(begin_it_time, IT_STATE_HOKUYO, &first_overtime);

	DETECTION_process_it();
	IT_test_state(begin_it_time, IT_STATE_DETECTION, &first_overtime);

	#ifdef DETECTION_CHOC
		DETECTION_CHOC_process_it();
		IT_test_state(begin_it_time, IT_STATE_DETECTION, &first_overtime);
	#endif

	#ifdef SCAN_CUP
		SCAN_CUP_process_it();
	#endif
	IT_test_state(begin_it_time, IT_STATE_SCAN_CUP, &first_overtime);

	#ifdef MODE_PRINTF_TABLEAU
		debug_print_tableau();
	#endif

	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
		DEBUG_process_it();
	#endif
	IT_test_state(begin_it_time, IT_STATE_DEBUG, &first_overtime);

	// Affichage des leds toutes les 500ms
	led_display_it++;
	if(led_display_it >= 500){
		display_led();
		led_display_it = 0;
	}

	g2 = global;
	if(TIMER2_getITStatus()){	//L'IT est trop longue ! il y a recouvrement !!!
		global.debug.recouvrement_IT = TRUE;
		global.debug.recouvrement_IT_time = global.absolute_time - begin_it_time;
	}
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

void IT_test_state(time32_t time_begin, it_state_e it_state, bool_e* over_time){
	if(*over_time)
		return;

	if(global.absolute_time - time_begin > 5){
		global.debug.recouvrement_section = it_state;
		*over_time = TRUE;
	}
}
