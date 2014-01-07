/*
 * LCD.c
 *
 *  Created on: 23 déc. 2013
 *      Author: Nirgal
 */

#include "LCD.h"

#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
#include "stm32f4_discovery_lcd.h"
#include "image.h"
#include "LCD_Touch_Calibration.h"
#include "write_lcd.h"
#include "stmpe811qtr.h"
#include "zone.h"
#include "../secretary.h"
#include "../QS/QS_CANmsgList.h"

#ifndef LED_GREEN
	#define LED_GREEN GPIOD->ODR12
#endif

	display_robot_t robots[ROBOTS_NUMBER];
	robots_e robot_selected = FRIEND_1; 	//robot sélectionné pour le tactile
	robots_e previous_robot_selected = FRIEND_2;
	bool_e robots_updated = TRUE;
	volatile uint32_t t_10ms = 0; /*!< variable pour le Timer 10ms (pour cadencer la prise de point tactile). */
	uint8_t current_color = Jaune;		//couleur de notre equipe
	uint8_t adversary_color = Rouge;		//couleur de notre equipe

void LCD_pos_init(void)
{
	robots_e i;
	for(i=0;i<ROBOTS_NUMBER;i++)
	{
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

	previous_robot_selected = FRIEND_2;
	robot_selected = FRIEND_1;
}


void LCD_init(void)
{
	LCD_pos_init();

	IOE_Config();
	STM32f4_Discovery_LCD_Init();	//initialisation du LCD
	image_Init();					//copie du tableau de la flash vers la ram
	init_zone();					// initialisation des zones importantes du terrain
	//coordonées actuelles et précédentes des quatres robots

	//initialisation de la calibration du LCD tactile
	LCD_Clear(White);
	LCD_SetFont(&Font8x12);
	//Lcd_Touch_Calibration();
	LCD_Clear(White);

	ecriture_info_prosition_v2();	//affichage des lignes d'information
}

void LCD_send_message(void)
{
	Uint8 data[8];

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
			data[6] = 0x00;		//Avant ou arrière
			data[7] = 0;
			SECRETARY_send_canmsg(ASSER_GO_POSITION, data, 8);
			break;
		case FRIEND_2:
			//force pos friend 2
			data[0] = robots[FRIEND_2].x/2;	//X [2cm]
			data[1] = robots[FRIEND_2].y/2;	//Y [2cm]
			SECRETARY_send_canmsg(STRAT_FRIEND_FORCE_POSITION, data, 2);
			break;
		case ADVERSARY_1:
		case ADVERSARY_2:
			//force pos adversaries
			/*		0 : ADVERSARY_NUMBER	//de 0 à n, il peut y avoir plus de deux adversaires si l'on inclut notre ami...
			 * 		1 :  x [2cm]
			 * 		2 :  y [2cm]
			 * 		3-4 : teta
			 * 		5 : distance [2cm]
			 * 		6 : fiability	:    "0 0 0 0 d t y x" (distance, teta, y, x) : 1 si fiable, 0 sinon.
			 */

			data[0] = 0;	//ADVERSARY 0
			data[1] = robots[ADVERSARY_1].x/2;	//X [2cm]
			data[2] = robots[ADVERSARY_1].y/2;	//Y [2cm]
			data[3] = 0;	//teta
			data[4] = 0;	//teta
			data[5] = 0;	//distance
			data[6] = 0x03;	//fiability : x et y fiables
			SECRETARY_send_canmsg(STRAT_ADVERSARIES_POSITION, data, 7);

			data[0] = 1;	//ADVERSARY 1
			data[1] = robots[ADVERSARY_2].x/2;	//X [2cm]
			data[2] = robots[ADVERSARY_2].y/2;	//Y [2cm]
			SECRETARY_send_canmsg(STRAT_ADVERSARIES_POSITION, data, 7);
			break;
		default:
			break;
	}
}

void LCD_process_main(void)
{
	static bool_e bt_prec = FALSE;		//état du bouton user de la stm32
	bool_e bt_current;
	robots_e i;


	bt_current = GPIOA->IDR0;
	if(!bt_prec && bt_current)	//changement d'équipe et remise à zero de toutes les variables
	{
		LCD_Clear(White);

		robot_selected=FRIEND_1;
		image_Init();					//copie du tableau de la flash vers la ram
		init_zone();

		adversary_color = current_color;
		current_color = (current_color == Jaune)? Rouge : Jaune;
		LCD_pos_init();

		ecriture_info_prosition_v2();
	}
	bt_prec=bt_current;

	if(t_10ms>=10)
	{	//Toutes les 100ms
		LED_GREEN = 1;
		if(Calibration_Test_Dispose(&robots[robot_selected],&robot_selected))
		{
			robots_updated = TRUE;
			LCD_send_message();
		}
		t_10ms=0;

		/*
		 * si on a de nouvelles coordonées pour un robot
		 * atribution de ces coordonées au robot voulu
		 * mise à jour de l'affichage des robots
			 * mise à jour des informations lié aux robots et aux zones
		 */
		/*
		 * mise à jour du tableau contenant toutes les information relatives aux zones
		 */
		test_zones(robots[ADVERSARY_1].x,robots[ADVERSARY_1].y,1);
		test_zones(robots[ADVERSARY_2].x,robots[ADVERSARY_2].y,2);
		LED_GREEN = 0;
	}

	//Insérer ici les autres sources de modif des coordonnées des robots.



	if(robots_updated)
	{
		LED_GREEN = 1;
		//On efface les emplacement des robots qui ont bougé.
		for(i=0;i<ROBOTS_NUMBER;i++)
		{
			if(robots[i].updated)
				delete_previous_robot(&robots[i]);
		}

		//On affiche les robots sur l'image.
		for(i=0;i<ROBOTS_NUMBER;i++)
			display_robot(&robots[i]);

		remplissage_info_position_v2(	robots[FRIEND_1].y		,robots[FRIEND_1].x	,robots[FRIEND_2].y	,robots[FRIEND_2].x,
										robots[ADVERSARY_1].y	,robots[ADVERSARY_1].x	,robots[ADVERSARY_2].y	,robots[ADVERSARY_2].x);

		if(previous_robot_selected != robot_selected)
		{
			display_bouton(robot_selected,current_color);	//mise à jour des boutons en fonction de notre équipe et du robot sélectionné
			previous_robot_selected = robot_selected;
		}

		LCD_SetCursor(0,0);
		LCD_Affiche_Image();
		LED_GREEN = 0;
	}

}

void LCD_update_robot(robots_e robot, Uint16 x, Uint16 y)
{
	robots[robot].x = x;
	robots[robot].y = y;
	robots[robot].updated = TRUE;
	robots_updated = TRUE;
}

//TODO : update color a partir du broadcast color.

/**
 * \fn void User_handler_TIM3(void)
 * \brief Fonction d'interruption pour le timer3
 */
void LCD_process_10ms(void)
{
	t_10ms +=1;
}


#endif
