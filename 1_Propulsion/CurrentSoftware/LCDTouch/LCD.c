/**
 * \file LCD.c
 * \brief Fichier pricncipal de la fonctionnalit� LCD_Touch
 * \author Nirgal, Shinitenshi & Herzaeone
 * \version 20140123
 * \date 23 janvier 2014
 *
 * Fichier d' impl�mentation des fonctions d'initialisation et de routine du LCD_TOUCH
 */
#include "LCD.h"

#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
#include "stm32f4_discovery_lcd.h"
#include "image.h"
#include "LCD_Touch_Calibration.h"
#include "write_lcd.h"
#include "stmpe811qtr.h"
#include "zone.h"
#include "../odometry.h"
#include "../secretary.h"
#include "../QS/QS_CANmsgList.h"

#ifndef LED_GREEN
	#define LED_GREEN GPIOD->ODR12
#endif

/**
 * \def POS_ROBOT(num)
 * \brief Macro qui comprend les positions du robot numero 'num' en x et y si ils sont activ�s
 */
#define POS_ROBOT(num) ((robots[num].enable)?robots[num].y:0) , ((robots[num].enable)?robots[num].x:0)

/**
 * \def robots_updated
 * \brief Macro qui indique si au moins un des robot a �t� mis � jour
 */
#define robots_updated (robots[0].updated || robots[1].updated || robots[2].updated || robots[3].updated)	// au moins l'un des robots a �t� update

/**
 * \def adversary_color
 * \brief Renvoie la couleur de l'adversaire (l'inverse de la notre)
 */
#define adversary_color ((robots[US].color == Rouge)?Jaune:Rouge) // couleur de l'�quipe adverse


	display_robot_t robots[ROBOTS_NUMBER];				/**< D�finition des robots pr�sents: le nombre d�pend du sujet de la coupe */
	robots_e robot_selected = NONE; 					/**< Robot s�lectionn� pour le tactile */
	robots_e previous_robot_selected = ROBOTS_NUMBER;	/**< Robot s�lectionn� p�c�dent */
	volatile uint32_t t_10ms = 0;						/**< variable pour le Timer 10ms (pour cadencer la prise de point tactile). */
	uint8_t current_color = Jaune;						/**< couleur de notre equipe */


/**
 * \fn bool_e is_robot_enabled (robots_e robot)
 * \brief Accesseur qui permet de v�rifier que le robot demand� est actif
 *
 * \param robot le nom ou num�ro du robot � tester
 * \return TRUE si le robot en question est activ� FALSE sinon
 */
bool_e is_robot_enabled(robots_e robot){
	return robots[robot].enable;
}


/**
 * \fn void set_robot_disable(robots_e robot)
 * \brief Accesseur qui permet de d�sactiver le robot demand�
 *
 * \param robot le nom ou num�ro du robot � tester
 */
void set_robot_disable(robots_e robot){
	robots[robot].enable = FALSE;
	delete_previous_robot(&robots[robot]);	// Efface la position pr�c�dente du robot
}

/**
 * \fn void set_robot_disable(robots_e robot)
 * \brief Accesseur qui permet d'activer le robot demand�
 *
 * \param robot le nom ou num�ro du robot � tester
 */
void set_robot_enable(robots_e robot){
	robots[robot].enable = TRUE;
}


/**
 * \fn void LCD_pos_init(void)
 * \brief Initialise les donn�es n�cessaires au fonctionnement de l'affichage
 *
 * Ne doit �tre appel�e que dans la fonction LCD_init()
 */
void LCD_pos_init(void){
	robots_e i;
	for(i=0;i<ROBOTS_NUMBER;i++){
		robots[i].x = 40;
		robots[i].y = 40+40*i;
		robots[i].xprec = 40;
		robots[i].yprec = 40+40*i;
		robots[i].updated = TRUE;
	}
	robots[FRIEND_1].size = 30;
	robots[FRIEND_2].size = 20;
	robots[ADVERSARY_1].size = 35;
	robots[ADVERSARY_2].size = 35;
	robots[FRIEND_1].color = current_color;
	robots[FRIEND_2].color = current_color;
	robots[ADVERSARY_1].color = adversary_color;
	robots[ADVERSARY_2].color = adversary_color;

	robots[FRIEND_1].enable = TRUE;
	robots[FRIEND_2].enable = FALSE;
	robots[ADVERSARY_1].enable = FALSE;
	robots[ADVERSARY_2].enable = FALSE;

}

/**
 * \fn void LCD_init(void)
 * \brief Proc�dure d'initialisation de l'�cran LCD
 *
 * A appeler dans dans la fonction main apr�s la d�finition des ports, avant le lancement des interruptions et la routine
 */
void LCD_init(void)
{
	LCD_pos_init();

	IOE_Config();
	STM32f4_Discovery_LCD_Init();	//initialisation du LCD
	image_Init();					//copie du tableau de la flash vers la ram
	init_zone();					// initialisation des zones importantes du terrain
	//coordon�es actuelles et pr�c�dentes des quatres robots

	//initialisation de la calibration du LCD tactile
	LCD_Clear(White);
	LCD_SetFont(&Font8x12);
	LCD_Clear(White);

	ecriture_info_prosition_v2();	//affichage des lignes d'information
}


/**
 *	\fn void LCD_send_message(void)
 *	\brief Fonction appel�e lors d'un changement d'�tat de l'�cran pour informer par messages CAN la strat�gie des changements (position adversaires et d�placements)
 */
void LCD_send_message(void)
{
	Uint8 data[8];
	Uint8 i;
	CAN_msg_t msg;

	//Si les robots adverses sont activ�s, on envoie leurs positions.
	if(robots[ADVERSARY_1].enable)
		SECRETARY_send_adversary_position((robots[ADVERSARY_2].enable)?FALSE:TRUE,0, robots[ADVERSARY_1].x*10, robots[ADVERSARY_1].y*10, 0, 0, ADVERSARY_DETECTION_FIABILITY_X | ADVERSARY_DETECTION_FIABILITY_Y);
	if(robots[ADVERSARY_2].enable)
		SECRETARY_send_adversary_position(TRUE,(robots[ADVERSARY_1].enable)?1:0, robots[ADVERSARY_2].x*10, robots[ADVERSARY_2].y*10, 0, 0, ADVERSARY_DETECTION_FIABILITY_X | ADVERSARY_DETECTION_FIABILITY_Y);


	switch(robot_selected)
	{
		case FRIEND_1:
			//go pos
			data[0] = 0x20;	//Multipoint, MAINTENANT.
			data[1] = HIGHINT(robots[FRIEND_1].x*10);
			data[2] = LOWINT(robots[FRIEND_1].x*10);
			data[3] = HIGHINT(robots[FRIEND_1].y*10);
			data[4] = LOWINT(robots[FRIEND_1].y*10);
			data[5] = 0x01; 	//slow
			data[6] = 0x00;		//Avant ou arri�re
			data[7] = 0;
#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
			msg.sid = ASSER_GO_POSITION;
			msg.size = 0x8;
			for(i=0;i<msg.size;i++){
				msg.data[i] = data[i];
			}
			SECRETARY_process_CANmsg(&msg);
#else
			SECRETARY_send_canmsg(ASSER_GO_POSITION, data, 8);
#endif
			break;
		case FRIEND_2:
		case ADVERSARY_1:
		case ADVERSARY_2:
			break;
		case NONE:
		default:
			break;
	}
}

/**
 * \fn void LCD_process_main(void)
 * \brief Proc�dure de routine de l'�cran LCD. Elle travail tous les 100ms pour mettre � jour les donn�es
 *
 * Proc�dure � appeler dans la routine de la fonction main (while(1))
 */
void LCD_process_main(void){
	Uint8 i;

	if(t_10ms>=10)
	{	//Toutes les 100ms

		t_10ms=0; // reinit compteur

		robots[US].x = global.position.x/10; //Calcul de la position pour affichage ecran
		robots[US].y = global.position.y/10;

		robots[US].updated = TRUE;			// D�clarer changement pour l'affichage

		current_color = (ODOMETRY_get_color()==RED)?Rouge : Jaune; // R�cup�ration de la couleur
		robots[US].color = current_color;
		robots[FRIEND_2].color = current_color;
		robots[ADVERSARY_1].color = adversary_color;
		robots[ADVERSARY_2].color = adversary_color;

		Calibration_Test_Dispose(&robots[robot_selected] , &robot_selected);

		LCD_send_message();

		if(robots_updated == TRUE){
			for(i=0; i<ROBOTS_NUMBER; i++){
				if(robots[i].enable == TRUE){
					delete_previous_robot(&robots[i]);	// Efface la position pr�c�dente du robot
					display_robot(&robots[i]);			// Affiche le robot a sa nouvelle position
				}
			}


			remplissage_info_position_v2(	robots[US].y,	robots[0].x,
										POS_ROBOT(FRIEND_2),
										POS_ROBOT(ADVERSARY_1),
										POS_ROBOT(ADVERSARY_2)); // MAJ des positions

			if(previous_robot_selected != robot_selected)
				display_bouton(robot_selected,current_color);	//mise � jour des boutons en fonction de notre �quipe et du robot s�lectionn�

			previous_robot_selected = robot_selected;

			LCD_SetCursor(0,0);
			LCD_Affiche_Image();				// Affichage de l'image cr��e
		}

	}

}


/**
 * \fn void LCD_update_robot(robots_e robot, Uint16 x, Uint16 y)
 * \brief Modifie les variables globales de position du robot demand�
 *
 * \param robot Nom ou num�ro du robot dont on modifie les coordonn�es
 * \param x Nouvelle position en x du robot 'robot'
 * \param y Nouvelle position en y du robot 'robot'
 */
void LCD_update_robot(robots_e robot, Uint16 x, Uint16 y)
{
	robots[robot].x = x;
	robots[robot].y = y;
	robots[robot].updated = TRUE;
}

//TODO : update color a partir du broadcast color.

/**
 * \fn void User_handler_TIM3(void)
 * \brief Fonction d'interruption pour le timer3. Incr�mente le compteur
 */
void LCD_process_10ms(void)
{
	t_10ms +=1;
}


#endif
