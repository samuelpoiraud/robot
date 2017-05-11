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
#include "QS/QS_maths.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_adc.h"
#include "scan/scan.h"

/*#define CONVERSION_LASER_LEFT(x)	((Sint32)(-263*(x)+350450)/1000)
#define OFFSET_WIDTH_LASER_LEFT		(146)
#define OFFSET_LENGTH_LASER_LEFT	(60)

#define CONVERSION_LASER_RIGHT(x)	((Sint32)(-270*(x)+35580)/1000)
#define OFFSET_WIDTH_LASER_RIGHT	(146)
#define OFFSET_LENGTH_LASER_RIGHT	(60)
*/


#define CONVERSION_LASER_LEFT(x)	((Sint32)(36148*(x)-6611800)/10000)//((Sint32)(37217*(x)-7096800)/10000)
#define OFFSET_WIDTH_LASER_LEFT		(144)
#define OFFSET_LENGTH_LASER_LEFT	(80)
#define OFFSET_ANGLE_LEFT            -102

#define CONVERSION_LASER_RIGHT(x)	((Sint32)(36052*(x)-6201700)/10000)//((Sint32)(36130*(x)-6247900)/10000)
#define OFFSET_WIDTH_LASER_RIGHT	(144)
#define OFFSET_LENGTH_LASER_RIGHT	(80)
#define OFFSET_ANGLE_RIGHT          -221

#ifdef MODE_PRINT_FIRST_TRAJ

	typedef struct{


		volatile Sint32 acceleration_translation;	//[mm.4096/5ms/5ms]
		volatile Sint32 vitesse_translation;		//[mm.4096/5ms]
		volatile Sint32 position_translation; 		//[mm.4096]

		volatile Sint32 acceleration_rotation;		//[rad.4096.1024/5ms/5ms]
		volatile Sint32 vitesse_rotation;			//[rad.4096.1024/5ms]
		volatile Sint32 position_rotation;			//[rad.4096.1024]

		volatile Sint32 real_speed_translation;		//[mm.4096/5ms]
		volatile Sint32 real_speed_rotation;		//[rad.4096.1024/5ms]

		volatile Sint32 real_position_translation;	//[mm.4096]
		volatile Sint32 real_position_rotation;		//[rad.4096.1024]

		volatile Sint32 ecart_translation;			//[mm.4096]
		volatile Sint32 ecart_rotation;				//[rad.4096.1024]

		volatile Sint16 pwmG;						// Rapport cyclique
		volatile Sint16 pwmD;						// Rapport cyclique

		volatile Sint32 translation_restante;		//[mm.4096]
		volatile Sint32 rotation_restante;			//[rad.4096.1024]

		volatile Sint32 distance_frein;				//[mm.4096]
		volatile Sint32 angle_frein;				//[rad.40963.1024]

		volatile position_t pos;					//[mm] et [rad.4096]


//		volatile Sint16 pos_x;
//		volatile Sint16 pos_y;
//		volatile Sint16 laser_left_x;
//		volatile Sint16 laser_left_y;
//        volatile Sint16 laser_right_x;
//        volatile Sint16 laser_right_y;
//        volatile Sint16 value_right;
//        volatile Sint16 mesure_right_x;
//        volatile Sint16 mesure_right_y;

}debug_saved_t;

    #define DEBUG_TAB_TRAJ_TAILLE 3000

	static volatile debug_saved_t tab[DEBUG_TAB_TRAJ_TAILLE];
	static volatile Uint16 index = 0;

	static volatile bool_e display_wanted = FALSE;

	static void affichage_first_traj(void);
#endif

#ifdef MODE_PRINTF_TABLEAU
	#define DEBUG_TABLEAU_TAILLE 128
	volatile Uint16 debug_index;
	volatile Uint16 debug_index_read;
	//volatile Sint32 debug_tableau[DEBUG_TABLEAU_TAILLE];
	volatile position_t debug_tableau[DEBUG_TABLEAU_TAILLE];
	bool_e mode_printf_tableau_enregistrer;

	static void debug_print_tableau(void);
#endif


#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
	static void debug_save_structure_global(void);
	static void affichage_global(void);
#endif

void DEBUG_init(void)
{

	#ifdef MODE_PRINTF_TABLEAU
		//Module permettant de visualiser après coup une grande série de valeur quelconque pour chaque IT...
		for(global.debug_index_read = 0; global.debug_index_read < DEBUG_TABLEAU_TAILLE; global.debug_index_read++)
		{
			global.debug_tableau[global.debug_index_read].x =0;
			global.debug_tableau[global.debug_index_read].y =0;
			global.debug_tableau[global.debug_index_read].teta =0;
		}
	#endif

	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
		global.flags.disable_virtual_perfect_robot = FALSE;
	#endif
}

void DEBUG_process_main(void)
{
	CAN_msg_t msg;
	if(global.debug.recouvrement_IT)
	{
		global.debug.recouvrement_IT = FALSE;
		debug_printf("#");
		//debug_printf("prévenez samuelp5@gmail.com, recouvrement IT !");//Fuck, il y a des IT trop longues... on remonte l'info au développeur !
		msg.sid = DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT;
		msg.size = SIZE_DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT;
		msg.data.debug_propulsion_erreur_recouvrement_it.duration = global.debug.recouvrement_IT_time;
		msg.data.debug_propulsion_erreur_recouvrement_it.id_it_state_name = global.debug.recouvrement_section;
		CAN_send(&msg);
	}

	#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
		//	affichage_global();
	#endif

	#ifdef MODE_PRINT_FIRST_TRAJ
		if(index >= DEBUG_TAB_TRAJ_TAILLE && display_wanted){
			display_wanted = FALSE;
			affichage_first_traj();
		}
	#endif

}

void DEBUG_process_it(void)
{

#ifdef MODE_PRINTF_TABLEAU
	debug_print_tableau();
#endif

#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
	debug_save_structure_global();
#endif

#ifdef MODE_PRINT_FIRST_TRAJ
	if(global.flags.match_started && index < DEBUG_TAB_TRAJ_TAILLE){
		tab[index].acceleration_rotation = global.acceleration_rotation;
		tab[index].acceleration_translation = global.acceleration_translation;
		tab[index].ecart_rotation = global.ecart_rotation;
		tab[index].ecart_translation = global.ecart_translation;
		tab[index].position_rotation = global.position_rotation;
		tab[index].position_translation = global.position_translation;
		tab[index].real_position_rotation = global.real_position_rotation;
		tab[index].real_position_translation = global.real_position_translation;
		tab[index].real_speed_rotation = global.real_speed_rotation;
		tab[index].real_speed_translation = global.real_speed_translation;
		tab[index].vitesse_rotation = global.vitesse_rotation;
		tab[index].vitesse_translation = global.vitesse_translation;
		tab[index].pwmD = global.pwmD;
		tab[index].pwmG = global.pwmG;
		tab[index].pos.x = global.position.x;
		tab[index].pos.y = global.position.y;
		tab[index].pos.teta = global.position.teta;
		tab[index].translation_restante = global.translation_restante;
		tab[index].rotation_restante = global.rotation_restante;
		tab[index].angle_frein = global.angle_frein;
		tab[index].distance_frein = global.distance_frein;

//        /*GEOMETRY_point_t pos_mesure;
//		bool_e enable;
//		Sint16 valueADC, value;
//		position_t robot;
//		Sint16 cosinus = 0, sinus = 0;
//
//		valueADC = ADC_getValue(ADC_SENSOR_LASER_LEFT);
//		value = CONVERSION_LASER_LEFT(valueADC);
//		robot = global.position; // On récupère la position du robot tout de suite
//		robot.teta = GEOMETRY_modulo_angle(robot.teta);
//		COS_SIN_4096_get(robot.teta, &cosinus, &sinus);
//
//		// On calcule et on stocke la valeur même si le capteur entre en saturation
//		if((value>90)&&(value<290)){
//		tab[index].laser_left_x = (Sint16) (robot.x - (-OFFSET_LENGTH_LASER_LEFT*cosinus + (OFFSET_WIDTH_LASER_LEFT + value)*sinus)/4096.0);
//		tab[index].laser_left_y = (Sint16) (robot.y + (OFFSET_LENGTH_LASER_LEFT*sinus + (OFFSET_WIDTH_LASER_LEFT + value)*cosinus)/4096.0);
//		tab[index].value_left=(Sint16) value;
//
//		cosinus = 0;
//		sinus = 0;
//
//		valueADC = ADC_getValue(ADC_SENSOR_LASER_RIGHT);
//		value = CONVERSION_LASER_RIGHT(valueADC);
//		robot = global.position; // On récupère la position du robot tout de suite
//		robot.teta = GEOMETRY_modulo_angle(robot.teta);
//		COS_SIN_4096_get(robot.teta, &cosinus, &sinus);
//
//		// On calcule et on stocke la valeur même si le capteur entre en saturation
//		tab[index].laser_right_x = (Sint16) (robot.x + (OFFSET_LENGTH_LASER_RIGHT*cosinus + (OFFSET_WIDTH_LASER_RIGHT + value)*sinus))/4096.0;
//		tab[index].laser_right_y = (Sint16) (robot.y + (OFFSET_LENGTH_LASER_RIGHT*sinus - (OFFSET_WIDTH_LASER_RIGHT + value)*cosinus))/4096.0;
//        */
//
//
//
//
//        GEOMETRY_point_t pos_mesure, pos_laser;
//        bool_e enable;
//        Sint16 valueADC, value;
//        position_t robot;
//        Sint16 cosinus = 0, sinus = 0;
//
////        valueADC = ADC_getValue(ADC_SENSOR_LASER_LEFT);
//        value = CONVERSION_LASER_LEFT(TELEMETER_get_ADCvalue_left());
//
//        robot = global.position; // On récupère la position du robot tout de suite
//        robot.teta = GEOMETRY_modulo_angle(robot.teta);
//        COS_SIN_4096_get(robot.teta, &cosinus, &sinus);
//
//
//        pos_laser.x = robot.x + (OFFSET_LENGTH_LASER_LEFT*cosinus - OFFSET_WIDTH_LASER_LEFT*sinus)/4096.0;
//        pos_laser.y = robot.y + (OFFSET_LENGTH_LASER_LEFT*sinus + OFFSET_WIDTH_LASER_LEFT*cosinus)/4096.0;
//
//
//        COS_SIN_4096_get(robot.teta+OFFSET_ANGLE_LEFT, &cosinus, &sinus);
//
//        pos_mesure.x=pos_laser.x-(value * sinus)/4096;
//        pos_mesure.y=pos_laser.y+(value * cosinus)/4096;
//
//        tab[index].value_right=value;
//
//        tab[index].laser_left_x=pos_mesure.x;
//        tab[index].laser_left_y=pos_mesure.y;
//
//        //tab[index].laser_left_x=pos_mesure.x;
//        //tab[index].laser_left_y=pos_mesure.y;
//
//       // valueADC = ADC_getValue(ADC_SENSOR_LASER_RIGHT);
//        value = CONVERSION_LASER_RIGHT(TELEMETER_get_ADCvalue_right());
//
// //       value = CONVERSION_LASER_RIGHT(valueADC);
//    //    printf("%d\n",valueADC);
//
//        robot = global.position; // On récupère la position du robot tout de suite
//        robot.teta = GEOMETRY_modulo_angle(robot.teta);
//        COS_SIN_4096_get(robot.teta, &cosinus, &sinus);
//
//        // On calcule et on stocke la position du laser (c'est à dire de début d'émission du rayon laser)
//        pos_laser.x = (Sint16) (robot.x + (OFFSET_LENGTH_LASER_RIGHT*cosinus + OFFSET_WIDTH_LASER_RIGHT*sinus)/4096.0);
//        pos_laser.y = (Sint16) (robot.y + (OFFSET_LENGTH_LASER_RIGHT*sinus - OFFSET_WIDTH_LASER_RIGHT*cosinus)/4096.0);
//
//        COS_SIN_4096_get(robot.teta+OFFSET_ANGLE_RIGHT, &cosinus, &sinus);
//        pos_mesure.x=pos_laser.x+(value) * sinus/4096;
//        pos_mesure.y=pos_laser.y-(value * cosinus)/4096;
//
//        tab[index].laser_right_x=pos_laser.x;
//        tab[index].laser_right_y=pos_laser.y;
//        tab[index].value_right = value;
//        tab[index].mesure_right_x=pos_mesure.x;
//        tab[index].mesure_right_y=pos_mesure.y;

        index++;

	}
#endif
}

#ifdef MODE_PRINT_FIRST_TRAJ

	void DEBUG_display(void){
		display_wanted = TRUE;
	}

	static void affichage_first_traj(void){
		Uint16 i;


		debug_printf("t(ms);");
		debug_printf("acceleration_rotation;");
		debug_printf("acceleration_translation;");
		debug_printf("ecart_rotation;");
		debug_printf("ecart_translation;");
		debug_printf("position_rotation;");
		debug_printf("position_translation;");
		debug_printf("real_position_rotation;");
		debug_printf("real_position_translation;");
		debug_printf("real_speed_rotation;");
		debug_printf("real_speed_translation;");
		debug_printf("vitesse_rotation;");
		debug_printf("vitesse_translation;");
		debug_printf("pwmD;");
		debug_printf("pwmG;");
		debug_printf("translation_restante;");
		debug_printf("rotation_restante;");
		debug_printf("distance_frein;");
		debug_printf("angle_frein;");
		debug_printf("pos.x;");
		debug_printf("pos.y;");
		debug_printf("pos.teta\n");


		for(i = 0; i < DEBUG_TAB_TRAJ_TAILLE && i < index; i++){
			debug_printf("%d;", i*5);
			debug_printf("%ld;", tab[i].acceleration_rotation);
			debug_printf("%ld;", tab[i].acceleration_translation);
			debug_printf("%ld;", tab[i].ecart_rotation);
			debug_printf("%ld;", tab[i].ecart_translation);
			debug_printf("%ld;", tab[i].position_rotation);
			debug_printf("%ld;", tab[i].position_translation);
			debug_printf("%ld;", tab[i].real_position_rotation);
			debug_printf("%ld;", tab[i].real_position_translation);
			debug_printf("%ld;", tab[i].real_speed_rotation);
			debug_printf("%ld;", tab[i].real_speed_translation);
			debug_printf("%ld;", tab[i].vitesse_rotation);
			debug_printf("%ld;", tab[i].vitesse_translation);
			debug_printf("%d;", tab[i].pwmD);
			debug_printf("%d;", tab[i].pwmG);
			debug_printf("%ld;", tab[i].translation_restante);
			debug_printf("%ld;", tab[i].rotation_restante);
			debug_printf("%ld;", tab[i].distance_frein);
			debug_printf("%ld;", tab[i].angle_frein);
			debug_printf("%ld;", tab[i].pos.x);
			debug_printf("%ld;", tab[i].pos.y);
			debug_printf("%d;\n", tab[i].pos.teta);

			Uint32 y;
			for(y=0;y<10000;y++);
		}
	}
#endif

#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
	static void affichage_global(void)
	{
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
	}

	static volatile global_data_storage_t SAVE;

	static void debug_save_structure_global(void)
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
	volatile bool_e we_touch_border = FALSE;

	#include "Copilot.h"
		//Dans ce mode de debug très spécial, on fait croire au code que la position/vitesse réelle du robot est celle du point fictif.
		//Ainsi, les écarts sont nuls, il n'y a pas d'erreur, et les commandes moteurs sont nulles.
		//Le robot semble "parfait"............c'est un robot virtuel qui se déplace
	void DEBUG_envoi_point_fictif_alteration_coordonnees_reelles(void)
	{
		if(global.flags.disable_virtual_perfect_robot)
		{
			//Robot virtuel parfait désactivé, car on a reçu des message de position venant d'une autre propulsion !
			//(on est donc très probablement relié à un robot, virtuel ou réel...)
			global.real_speed_translation = 0;
			global.real_speed_rotation = 0;
			//La position sera mise à jour sur réception des messages de position...
		}
		else
		{
			if(COPILOT_is_arrived())
			{
				global.real_speed_translation = 0;
				global.real_speed_rotation = 0;
				we_touch_border = FALSE;
			}
			else
			{
				global.real_speed_translation = global.position_translation;
				global.real_speed_rotation = global.position_rotation;
			}
			if(COPILOT_get_border_mode() != NOT_BORDER_MODE)
			{
				Sint16 robotSize = (COPILOT_get_way()==FORWARD) ? get_calibration_forward_distance() : get_calibration_backward_distance();
				if(global.position.x <= robotSize)
				{
					global.real_speed_translation = 0;
					global.position.x = robotSize;
					we_touch_border = TRUE;
				}
				if(global.position.y <= robotSize)
				{
					global.real_speed_translation = 0;
					global.position.y = robotSize;
					we_touch_border = TRUE;
				}
				if(global.position.x >= FIELD_SIZE_X-robotSize)
				{
					global.real_speed_translation = 0;
					global.position.x = FIELD_SIZE_X-robotSize;
					we_touch_border = TRUE;
				}
				if(global.position.y >= FIELD_SIZE_Y-robotSize){
					global.real_speed_translation = 0;
					global.position.y = FIELD_SIZE_Y-robotSize;
					we_touch_border = TRUE;
				}

				//Spécifique terrain 2017

				// Tasseau zone de départ bleu lorsque le robot est dans la zone de départ
				if(global.position.x >= 360 - robotSize && global.position.x <= 370 && global.position.y > 0  && global.position.y  < 710)
				{
					global.real_speed_translation = 0;
					global.position.x = 360 - robotSize;
					we_touch_border = TRUE;
				}
				// Tasseau zone de départ bleu lorsque le robot est en dehors de la zone de départ
				if(global.position.x >= 371 && global.position.x <= (382 + robotSize) && global.position.y > 0  && global.position.y  < 710)
				{
					global.real_speed_translation = 0;
					global.position.x = 382 + robotSize;
					we_touch_border = TRUE;
				}
				// Tasseau zone de départ jaune lorsque le robot est dans la zone de départ
				if(global.position.x >= 360 - robotSize && global.position.x <= 370 && global.position.y > 2290  && global.position.y  < 3000)
				{
					global.real_speed_translation = 0;
					global.position.x = 360 - robotSize;
					we_touch_border = TRUE;
				}
				// Tasseau zone de départ jaune lorsque le robot est en dehors de la zone de départ
				if(global.position.x >= 371 && global.position.x <= 382 + robotSize && global.position.y > 2290  && global.position.y  < 3000)
				{
					global.real_speed_translation = 0;
					global.position.x = 382 + robotSize;
					we_touch_border = TRUE;
				}
				// Tasseau de dépose modules côté bleu à côté de la bordure
				if(global.position.x > 700 && global.position.x < 1150 && global.position.y > 0  && global.position.y  <= 102 + robotSize)
				{
					global.real_speed_translation = 0;
					global.position.y = 102 + robotSize;
					we_touch_border = TRUE;
				}
				// Tasseau de dépose modules côté jaune à côté de la bordure
				if(global.position.x > 700 && global.position.x < 1150 && global.position.y >= 2898 - robotSize  && global.position.y  < 3000)
				{
					global.real_speed_translation = 0;
					global.position.y = 2898 - robotSize;
					we_touch_border = TRUE;
				}

				// Tasseau de dépose modules milieu (côté bleu)
				if(global.position.x > 800 && global.position.x < 2000 && global.position.y >= 1432 - robotSize  && global.position.y  < 1500)
				{
					global.real_speed_translation = 0;
					global.position.y = 1432 - robotSize;
					we_touch_border = TRUE;
				}
				// Tasseau de dépose modules milieu (côté jaune)
				if(global.position.x > 800 && global.position.x < 2000 && global.position.y > 1500  && global.position.y  <= 1568 + robotSize)
				{
					global.real_speed_translation = 0;
					global.position.y = 1568 + robotSize;
					we_touch_border = TRUE;
				}

				//Tasseau incliné de dépose modules bleu (côté bleu)
				GEOMETRY_point_t quadri_bb[4] = {(GEOMETRY_point_t){1434, 934},
												 (GEOMETRY_point_t){1859, 1359},
												 (GEOMETRY_point_t){1859 + (68 + robotSize)*cos(-PI4096/4), 1359 + (68 + robotSize)*sin(-PI4096/4)},
												 (GEOMETRY_point_t){1434 + (68 + robotSize)*cos(-PI4096/4), 934 + (68 + robotSize)*sin(-PI4096/4)}};
				GEOMETRY_segment_t seg_bb = {quadri_bb[3], quadri_bb[4]};
				if(is_in_quadri(quadri_bb, (GEOMETRY_point_t){global.position.x, global.position.y}))
				{
					global.real_speed_translation = 0;
					GEOMETRY_point_t proj = GEOMETRY_proj_on_line(seg_bb, (GEOMETRY_point_t){global.position.x, global.position.y});
					global.position.x = proj.x;
					global.position.y = proj.y;
					we_touch_border = TRUE;
				}

				//Tasseau incliné de dépose modules bleu (côté jaune)
				GEOMETRY_point_t quadri_bj[4] = {(GEOMETRY_point_t){1434, 934},
												 (GEOMETRY_point_t){1859, 1359},
												 (GEOMETRY_point_t){1859 + (68 + robotSize)*cos(3*PI4096/4), 1359 + (68 + robotSize)*sin(3*PI4096/4)},
												 (GEOMETRY_point_t){1434 + (68 + robotSize)*cos(3*PI4096/4), 934 + (68 + robotSize)*sin(3*PI4096/4)}};
				GEOMETRY_segment_t seg_bj =  {quadri_bj[3], quadri_bj[4]};
				if(is_in_quadri(quadri_bj, (GEOMETRY_point_t){global.position.x, global.position.y}))
				{
					global.real_speed_translation = 0;
					GEOMETRY_point_t proj = GEOMETRY_proj_on_line(seg_bj, (GEOMETRY_point_t){global.position.x, global.position.y});
					global.position.x = proj.x;
					global.position.y = proj.y;
					we_touch_border = TRUE;
				}

				//Tasseau incliné de dépose modules jaune (côté bleu)
				GEOMETRY_point_t quadri_jb[4] = {(GEOMETRY_point_t){1434, 934},
												 (GEOMETRY_point_t){1859, 1359},
												 (GEOMETRY_point_t){1859 + (68 + robotSize)*cos(-3*PI4096/4), 1641 + (68 + robotSize)*sin(-3*PI4096/4)},
												 (GEOMETRY_point_t){1434 + (68 + robotSize)*cos(-3*PI4096/4), 2066 + (68 + robotSize)*sin(-3*PI4096/4)}};
				GEOMETRY_segment_t seg_jb = {quadri_jb[3], quadri_jb[4]};
				if(is_in_quadri(quadri_jb, (GEOMETRY_point_t){global.position.x, global.position.y}))
				{
					global.real_speed_translation = 0;
					GEOMETRY_point_t proj = GEOMETRY_proj_on_line(seg_jb, (GEOMETRY_point_t){global.position.x, global.position.y});
					global.position.x = proj.x;
					global.position.y = proj.y;
					we_touch_border = TRUE;
				}

				//Tasseau incliné de dépose modules bleu (côté jaune)
				GEOMETRY_point_t quadri_jj[4] = {(GEOMETRY_point_t){1434, 934},
												 (GEOMETRY_point_t){1859, 1359},
												 (GEOMETRY_point_t){1859 + (68 + robotSize)*cos(PI4096/4), 1641 + (68 + robotSize)*sin(PI4096/4)},
												 (GEOMETRY_point_t){1434 + (68 + robotSize)*cos(PI4096/4), 2066 + (68 + robotSize)*sin(PI4096/4)}};
				GEOMETRY_segment_t seg_jj =  {quadri_jj[3], quadri_jj[4]};
				if(is_in_quadri(quadri_jj, (GEOMETRY_point_t){global.position.x, global.position.y}))
				{
					global.real_speed_translation = 0;
					GEOMETRY_point_t proj = GEOMETRY_proj_on_line(seg_jj, (GEOMETRY_point_t){global.position.x, global.position.y});
					global.position.x = proj.x;
					global.position.y = proj.y;
					we_touch_border = TRUE;
				}
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

	bool_e DEBUG_get_we_touch_border(void)
	{
		return we_touch_border;
	}

#endif
