/*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : debug.c
 *  Package : Propulsion
 *  Description : Fonctions de débogage, ou de test du code. L'idée de ce fichier est de polluer le moins possible le reste du code avec des routines de débog.
 *  Auteur : Nirgal
 *  Version 201203
 */

#include "debug.h"
#include "pilot.h"
#include "odometry.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can.h"

	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		void debug_save_structure_global(void);
	#endif
	
	#ifdef MODE_PRINTF_TABLEAU
		void debug_print_tableau(void);

		#define DEBUG_TABLEAU_TAILLE 128
		volatile Uint16 debug_index;
		volatile Uint16 debug_index_read;
		//volatile Sint32 debug_tableau[DEBUG_TABLEAU_TAILLE];
		volatile position_t debug_tableau[DEBUG_TABLEAU_TAILLE];
		bool_e mode_printf_tableau_enregistrer;
	#endif
	
		
	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
		void DEBUG_envoi_point_fictif_alteration_coordonnees_reelles(void);
		void DEBUG_envoi_point_fictif_process_main(void);
	#endif
	
	
void DEBUG_init(void)
{
	//debug_printf("size of global : %d\n",sizeof(global_data_storage_t));

	#ifdef MODE_PRINTF_TABLEAU
		//Module permettant de visualiser après coup une grande série de valeur quelconque pour chaque IT...
		for(global.debug_index_read = 0; global.debug_index_read < DEBUG_TABLEAU_TAILLE; global.debug_index_read++)
		{
			global.debug_tableau[global.debug_index_read].x =0;
			global.debug_tableau[global.debug_index_read].y =0;
			global.debug_tableau[global.debug_index_read].teta =0;
		}	
	#endif
	
	
		//ODOMETRY_set(0,0,0);
//		SEQUENCES_calibrate(BACKWARD);
	//TEST.
	/*	//ENVOI D'ORDRES DIRECTEMENT A L'INITIALISATION...
	CAN_msg_t msg;
	msg.sid = ASSER_GO_POSITION;
	msg.data[0] = 0x30;
	msg.data[1] = 0x04;
	msg.data[2] = 0x00;
	msg.data[3] = 0x04;
	msg.data[4] = 0x00;
	msg.data[5] = 0x00;
	msg.data[6] = 0x00;
	msg.data[7] = 0x01;
	msg.size = 8;
	SECRETARY_mailbox_add(&msg);
	msg.data[0] = 0x30;
	msg.data[1] = 0x08;
	msg.data[2] = 0x00;
	msg.data[3] = 0x0C;
	msg.data[4] = 0x00;
	msg.data[5] = 0x00;
	msg.data[6] = 0x00;
	msg.data[7] = 0x01;
	SECRETARY_mailbox_add(&msg);
	*/
}	

void DEBUG_process_main(void)
{
	CAN_msg_t msg;
	if(global.flag_recouvrement_IT)
	{
		global.flag_recouvrement_IT = FALSE;	
		debug_printf("#");//debug_printf("prévenez samuelp5@gmail.com, recouvrement IT !");//Fuck, il y a des IT trop longues... on remonte l'info au développeur !
		msg.sid = DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT;
		msg.size = 8;
		msg.data[0] = 'E';
		msg.data[1] = 'R';
		msg.data[2] = 'R';
		msg.data[3] = 'O';
		msg.data[4] = 'R';
		msg.data[5] = 'P';
		msg.data[6] = 'R';
		msg.data[7] = 'O';
		CAN_send(&msg);
	}
		
	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
		DEBUG_envoi_point_fictif_process_main();
	#endif
	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		//	affichage_global();
	#endif
}	
#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
	static bool_e envoyer_point_fictif = FALSE;
#endif		
void DEBUG_process_it(void)
{
	static Uint16 compteur_it;
	compteur_it = (compteur_it + 1) % 40;	//un envoi pour 40 IT = un envoi pour 200ms = 5 fois par seconde...
	if( !compteur_it )
	{
		#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
			envoyer_point_fictif = TRUE;
		#endif
		//CAN_send_point_fictif(global.position.x, global.position.y, global.position.teta);
		
	}
	//if(COPILOT_is_arrived_rotation() == ARRIVED)
	//	printf("%d\t, %d\t, %d\t, %ld\t, %ld\n", global.position.x, global.position.y, global.position.teta, (Sint32)PILOT_get_destination_rotation(), (Sint32)global.ecart_rotation_prec);		
}
		
void affichage_global(void)
{
	
	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		//on demande une sauvegarde de l'état actuel de la variable globale
		global.flags.enable_save_global = TRUE;
		//on attend...
		while(global.flags.enable_save_global);
		
		debug_printf("----------------------\n");
		if(SAVE.robot_arrive == ROBOT_ARRIVE)
			debug_printf("arrivé\n");
		else
			debug_printf("pas arrivé\n");
		if(SAVE.robot_erreur == EN_ERREUR)
			debug_printf("en erreur\n");
		else
			debug_printf("pas erreur\n");
		debug_printf("ETAT : %d\n",SAVE.etat);
		debug_printf("ETAT arrive trans: %d\n",SAVE.robot_arrive_translation);
		debug_printf("ETAT arrive rot: %d\n",SAVE.robot_arrive_rotation);
	/*	debug_printf("ETAT freine trans: %d\n",SAVE.robot_freine_translation);
		debug_printf("ETAT freine rot: %d\n",SAVE.robot_freine_rotation);
		debug_printf("pos trans : %ld\n",SAVE.position_translation/4096);	
		debug_printf("pos rot : %ld\n",SAVE.position_rotation);
		debug_printf("reel vit : %ld\n", SAVE.vitesse_translation_reelle/4096);
		debug_printf("reel trans : %ld\n", SAVE.position_translation_reelle/4096);
		debug_printf("reel rot : %ld\n", SAVE.position_rotation_reelle);
		debug_printf("ecr pos : %ld\n", SAVE.ecart_translation/4096);
		debug_printf("ecr rot : %ld\n", SAVE.ecart_rotation);
		debug_printf("acc trans : %ld\n",SAVE.acceleration_translation);
		debug_printf("vit trans : %ld\n",SAVE.vitesse_translation/4096);
		debug_printf("pos trans : %ld\n",SAVE.position_translation/4096);
		debug_printf("acc rot : %ld\n",SAVE.acceleration_rotation);
		debug_printf("vit rot : %ld\n",SAVE.vitesse_rotation);
		debug_printf("pos rot : %ld\n",SAVE.position_rotation);
		debug_printf("dest rot : %ld\n",SAVE.destination_rotation);
		debug_printf("dest trans : %ld\n",SAVE.translation_destination/4096);
		debug_printf("pos x : %d\n", SAVE.position.x);
		debug_printf("pos y : %d\n",SAVE.position.y);*/
	//	debug_printf("pos teta : %d\n",SAVE.position.teta);
		debug_printf("----------------------\n");
	#endif
		
}	
	

	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		volatile global_data_storage_t SAVE;
	
		void debug_save_structure_global(void)
		{
			//si on nous demande une sauvegarde de l'état actuel _ utile pour le debug !
			if(global.flags.enable_save_global)
			{
				global.flags.enable_save_global = FALSE;
				SAVE = global;
			}	
		}	
	#endif



	#ifdef MODE_PRINTF_TABLEAU
		void debug_print_tableau(void)
		{
			if(global.mode_printf_tableau_enregistrer == TRUE)
			{
				//Module permettant de visualiser après coup une grande série de valeur quelconque pour chaque IT...
				//dès qu'on est en état ARRET, on affiche les 1024 valeurs enregistrées.... c'est long, mais très utile !
				global.debug_index = (global.debug_index + 1) % DEBUG_TABLEAU_TAILLE;
				
//				if(global.type_trajectoire == TRANSLATION || global.type_trajectoire == COURBE)
//					global.debug_tableau[global.debug_index] = global.position_point_fictif; 
				//On peut bien sur changer la valeur sauvegardée ici !
					//L'affichage se fait dans T2_interrupt !
			}
		}		
	#endif



	
	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
		#include "Copilot.h"
			//Dans ce mode de debug très spécial, on fait croire au code que la position/vitesse réelle du robot est celle du point fictif.
			//Ainsi, les écarts sont nuls, il n'y a pas d'erreur, et les commandes moteurs sont nulles.
			//Le robot semble "parfait"............c'est un robot virtuel qui se déplace
		void DEBUG_envoi_point_fictif_alteration_coordonnees_reelles(void)
		{
				global.real_speed_translation = global.position_translation;//global.ecart_translation_prec;
				global.real_speed_rotation = global.position_rotation << 10;//global.ecart_rotation_prec << 10;
				if(COPILOT_get_border_mode() == BORDER_MODE_WITH_UPDATE_POSITION)
				{
					if(global.position.x < -400)
					{
						global.real_speed_translation = 0;
						global.position.x = -400;
					}
					if(global.position.y < -300)
					{
						global.real_speed_translation = 0;
						global.position.y = -300;
					}
				}	
			/*	en gros, la vitesse à prendre en début d'IT est l'écart entre le robot et le nouveau point fictif
				c'est à dire que l'on rejoint le point fictif parfaitement...
				il y avait un écart 'X', on l'annule en prenant la bonne vitesse !
				donc on a en permanence la vitesse entre deux points fictifs consécutifs...
			
				pas de pb en fin de trajectoire.
			*/


			//donc l'intégrale de ces vitesse, est la position actuelle du robot... et celle du point fictif.
			//cette intrégrale est faite dans l'odométrie, juste après l'appel de cette fonction...
		}
	



		void DEBUG_envoi_point_fictif_process_main(void)
		{
			if(envoyer_point_fictif == TRUE)
			{
				envoyer_point_fictif = FALSE;
				Sint16 teta;
				teta = (((Sint32)(global.position.teta))*180)/PI4096;
				if(teta <0)
					teta += 360;
		
				printf("%d\t%d\t%d\tdr%ld\ter%ld\tpr%ld\trpr%ld\n", 
						global.position.x, global.position.y, teta, 
						(Sint32)PILOT_get_destination_rotation(), 
						(Sint32)global.ecart_rotation_prec,
						global.position_rotation,
						global.real_position_rotation>>10);	
//				CAN_send_point_fictif(global.position.x, global.position.y, global.position.teta);
			}
		}

	
	#endif
	









/*
	A chaque IT, on peut vouloir visualiser des signaux comme ceci :

	//sortie optionelle de l'image des positions roues codeuses sur les PWM3L et 4L
	//Il faut dans ce cas mettre un filtre passe pas sur ces sorties pour visu a l'oscillo
	//	PWM_run((Sint8)(global.position.teta/500+50),3);
	//	PWM_run((Sint8)(global.position.teta/500+50),4);
*/
