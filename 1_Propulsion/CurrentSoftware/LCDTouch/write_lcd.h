/**
 * \file  write_lcd.c
 * \brief Programme d'écriture et d'affichage du lcd.
 * \author Franck.H
 * \version 1
 * \date 4 déc. 2013
 */

#ifndef WRITE_LCD_H_
#define WRITE_LCD_H_

#include "zone.h"
#include "LCD.h"
/**
 * \enum couleur_robot_e
 * \brief sélection d'un robot.
 */
typedef enum{
	JAUNE_1=0, /*!< robot 1 (les bouton correspondant son dans l'ordre croissant sur le lcd). */
	JAUNE_2, /*!< robot 2. */
	ROUGE_1, /*!< robot 3. */
	ROUGE_2 /*!< robot 4. */
}couleur_robot_e;

#define BUTTON_SIZE 7 /*!<taille d'un bouton (par rapport au centre)*/

void affichage_des_zones(void);
void ecriture_info_prosition(uint16_t y,uint16_t x,
							uint16_t y_ennemie_1,uint16_t x_ennemie_1,
							uint16_t y_ennemie_2,uint16_t x_ennemie_2,
							uint8_t zone_1,uint8_t zone_2,
							uint16_t temps_zone_1,uint16_t temps_zone_2);
void ecriture_info_prosition_v2(void);
void remplissage_info_position_v2(uint16_t y1,uint16_t x1,uint16_t y2,uint16_t x2,
							uint16_t y_ennemie_1,uint16_t x_ennemie_1,
							uint16_t y_ennemie_2,uint16_t x_ennemie_2);
void delete_previous_robot(display_robot_t * pos);
void display_background(uint16_t y_prec,uint16_t x_prec);
void display_robot(display_robot_t * robot);
void display_bouton(couleur_robot_e couleur,uint8_t couleur_equipe);
#endif /* WRITE_LCD_H_ */
