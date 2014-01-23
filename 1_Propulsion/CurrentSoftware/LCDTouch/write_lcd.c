/**
 * \file  write_lcd.c
 * \brief Programme d'�criture et d'affichage du lcd.
 * \author Shinintenshi && Herzaeone
 * \version 20140123
 * \date 23 janvier 2014
 */


#include "write_lcd.h"

#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)

#include <stdio.h>
#include <string.h>
#include "../QS/QS_all.h"
#include "fonts.h"
#include "stm32f4_discovery_lcd.h"
#include "image.h"


#define TAILLE_ROBOT 10 /*!</\def TAILLE_ROBOT taille d'un robot par rapport � son centre*/



/**
 * \fn void affichage_des_zones(void)
 * \brief affiche le contour des zones importante du terrain sur le lcd.
 */
void affichage_des_zones(void){
	int i;
	for (i=0;i<=13;i++)
			LCD_DrawRect((get_zones()+i)->y_min,(get_zones()+i)->x_min,
						(get_zones()+i)->y_max-(get_zones()+i)->y_min,(get_zones()+i)->x_max-(get_zones()+i)->x_min);
	for(i=0;i<100000;i++);
}

/**
 * \fn void ecriture_info_prosition(uint16_t y,uint16_t x,uint16_t y_ennemie_1,uint16_t x_ennemie_1,
 *							uint16_t y_ennemie_2,uint16_t x_ennemie_2,
 *							uint8_t zone_1,uint8_t zone_2,
 *							uint16_t temps_zone_1,uint16_t temps_zone_2)
 * \brief affichage des lignes d'informations de position des robots
 * ****************************VERSION OBSELETE******************************
 * \param x,y,x_ennemie_1,y_ennemie_1,x_ennemie_2,y_ennemie_2 coordonn�es des trois robots
 *  temps_zone_1,temps_zone_2 temps qu'un robot � pass� dans une certaine zone
 */
void ecriture_info_prosition(uint16_t y,uint16_t x,uint16_t y_ennemie_1,uint16_t x_ennemie_1,
							uint16_t y_ennemie_2,uint16_t x_ennemie_2,
							uint8_t zone_1,uint8_t zone_2,
							uint16_t temps_zone_1,uint16_t temps_zone_2){
	static char  texte_robot[30];
	static char  texte_robot_ennemie_1[30];
	static char  texte_robot_ennemie_2[30];
	static char  texte_zone[30];

	LCD_SetColors(Black,White);
	LCD_SetFont(&Font8x8);

	sprintf(texte_robot,"Robot:x=%4dcm y=%4dcm",x,y);
	sprintf(texte_robot_ennemie_1,"Robot-En1:x =%4dcm y=%4dcm ",x_ennemie_1,y_ennemie_1);
	sprintf(texte_robot_ennemie_2,"Robot-En2:x =%4dcm y=%4dcm ",x_ennemie_2,y_ennemie_2);
	sprintf(texte_zone,"Z1:%2d t:%3dms Z2:%2d t:%3dms",zone_1,temps_zone_1,zone_2,temps_zone_2);

	LCD_DisplayStringLineColon(0,LCD_LINE_26,texte_robot);
	LCD_DisplayStringLineColon(0,LCD_LINE_27,texte_robot_ennemie_1);
	LCD_DisplayStringLineColon(0,LCD_LINE_28,texte_robot_ennemie_2);
	LCD_DisplayStringLineColon(0,LCD_LINE_29,texte_zone);
	LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
 * \fn void remplissage_info_prosition_v2(uint16_t y1,uint16_t x1,uint16_t y2,uint16_t x2,
 *							uint16_t y_ennemie_1,uint16_t x_ennemie_1,
 *							uint16_t y_ennemie_2,uint16_t x_ennemie_2)
 * \brief remplissage des lignes d'informations de position des robots
 *
 * \param param x1,y1,x2,y2,x_ennemie_1,y_ennemie_1,x_ennemie_2,y_ennemie_2 coordonn�es des quatres robots
 */
void remplissage_info_position_v2(uint16_t y1,uint16_t x1,uint16_t y2,uint16_t x2,
							uint16_t y_ennemie_1,uint16_t x_ennemie_1,
							uint16_t y_ennemie_2,uint16_t x_ennemie_2){
	char texte[30];
	int i;
	LCD_SetColors(Black,White);
	LCD_SetFont(&Font8x8);

	sprintf(texte,"%3d",x1);
	LCD_DisplayStringLineColon(6*8,LCD_LINE_26,texte);
	sprintf(texte,"%3d",y1);
	LCD_DisplayStringLineColon(12*8,LCD_LINE_26,texte);

	sprintf(texte,"%3d",x2);
	LCD_DisplayStringLineColon(21*8,LCD_LINE_26,texte);
	sprintf(texte,"%3d",y2);
	LCD_DisplayStringLineColon(27*8,LCD_LINE_26,texte);

	sprintf(texte,"%3d",x_ennemie_1);
	LCD_DisplayStringLineColon(6*8,LCD_LINE_27,texte);
	sprintf(texte,"%3d",y_ennemie_1);
	LCD_DisplayStringLineColon(12*8,LCD_LINE_27,texte);

	sprintf(texte,"%3d",x_ennemie_2);
	LCD_DisplayStringLineColon(21*8,LCD_LINE_27,texte);
	sprintf(texte,"%3d",y_ennemie_2);
	LCD_DisplayStringLineColon(27*8,LCD_LINE_27,texte);


	for (i=0;i<=13;i++){
		if (zone_tableau[i].robot==1){
			switch((get_zones()+i)->analyse){
				case PASSAGE :
					sprintf(texte,"%2d:passage",zone_tableau[i].id_zone);
					break;
				case DURATION :
					sprintf(texte,"%2d:t=%5ld",zone_tableau[i].id_zone,zone_tableau[i].temps);
					break;
				case LOOKING_Y :
					sprintf(texte,"%2d:y=%5d",zone_tableau[i].id_zone,zone_tableau[i].y_recherche);
					break;
				default :
					break;
			}
			LCD_DisplayStringLineColon(4*8,LCD_LINE_28,texte);
		}
		if (zone_tableau[i].robot==2){
			switch((get_zones()+i)->analyse){
				case PASSAGE :
					sprintf(texte,"%2d:passage",zone_tableau[i].id_zone);
					break;
				case DURATION :
					sprintf(texte,"%2d:t=%5ld",zone_tableau[i].id_zone,zone_tableau[i].temps);
					break;
				case LOOKING_Y :
					sprintf(texte,"%2d:y=%5ld",zone_tableau[i].id_zone,zone_tableau[i].y_recherche);
					break;
				default :
					break;
			}
			LCD_DisplayStringLineColon(19*8,LCD_LINE_28,texte);
		}
	}

}

/**
 * \fn void ecriture_info_prosition_v2(void)
 * \brief affichage des lignes d'informations vierges de position des robots
 */
void ecriture_info_prosition_v2(void){

	LCD_SetColors(Black,White);
	LCD_SetFont(&Font8x8);

	LCD_DisplayStringLineColon(0,LCD_LINE_26,"R:1:x=   |y=   ||2:x=   |y=   ");
	LCD_DisplayStringLineColon(0,LCD_LINE_27,"A:1:x=   |y=   ||2:x=   |y=   ");
	LCD_DisplayStringLineColon(0,LCD_LINE_28,"Z:1:           ||2:           ");
	LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
 * \fn bool_e test_Update(uint16_t y,uint16_t x,uint16_t *y_prec,uint16_t *x_prec,uint8_t couleur_robot)
 * \brief mise � jour de l'affichage du robot si celui ci s'est d�plac�
 * \param x,y coordon�es actuel du robot
 *  couleur_robot la couleur d'affichage du robot (en RVB_8bits)
 */
void delete_previous_robot(display_robot_t * robot)
{
	Sint16 i,j;
	for (i=robot->xprec-robot->size/2;i<=robot->xprec+robot->size/2;i++)
		for(j=robot->yprec-robot->size/2;j<=robot->yprec+robot->size/2;j++)
			if(i<199 && j<299)
				Display_Tableau(j+320*i);
	robot->yprec=robot->y;
	robot->xprec=robot->x;
	robot->updated = FALSE;
}

/**
 * \fn  display_robot(uint16_t y,uint16_t x,uint8_t couleur_robot)
 * \brief Fonction d'affichage du robot
 * \param x,y coordon�es actuel du robot
 * couleur_robot la couleur d'affichage du robot (en RVB_8bits)
 */
void display_robot(display_robot_t * robot)
{
	Sint16 i,j;
//	assert(robot->y<=300);
//	assert(robot->x<=200);
	Uint8 color = robot->color;
	for (i = robot->x - robot->size/2; i <= robot->x + robot->size/2 ; i++)//i =(robot->x>robot->size/2)? robot->x-robot->size/2 : 0
		for(j = robot->y - robot->size/2; j <= robot->y + robot->size/2 ; j++)//(robot->y>robot->size/2)? robot->y-robot->size/2 : 0
			if(i<199 && j<299)
				ram_tableauImage[j+320*i]=color;
}

/**
 * \fn void display_bouton(couleur_robot_e couleur_robot,uint8_t couleur_equipe)
 * \brief Fonction dd'affichage des boutons de s�lection des robots
 *
 * \param couleur_robot le robot qui est s�l�ctionner pour le d�placement (affich� en noir)
 * couleur_equipe la couleur de notre �quipe
 */
void display_bouton(couleur_robot_e couleur_robot,uint8_t couleur_equipe){
	uint16_t i,j;
	uint8_t couleur=0;

	for (i=310-BUTTON_SIZE;i<=310+BUTTON_SIZE;i++){
		if (couleur_robot!=JAUNE_1){
			couleur = (couleur_equipe == Jaune)? Jaune : Rouge;
		}else couleur=0;
		for(j=20-BUTTON_SIZE;j<=20+BUTTON_SIZE;j++){
			ram_tableauImage[320*j+i]=couleur;
		}

		if (couleur_robot!=JAUNE_2){
			couleur=(couleur_equipe == Jaune)? Jaune : Rouge;
		}else couleur=0;
		for(j=40-BUTTON_SIZE;j<=40+BUTTON_SIZE;j++){
			ram_tableauImage[320*j+i]=couleur;
		}

		if (couleur_robot!=ROUGE_1){
			couleur=(couleur_equipe == Jaune)? Rouge : Jaune;
		}else couleur=0;
		for(j=80-BUTTON_SIZE;j<=80+BUTTON_SIZE;j++){
			ram_tableauImage[320*j+i]=couleur;
		}

		if (couleur_robot!=ROUGE_2){
			couleur=(couleur_equipe == Jaune)? Rouge : Jaune;
		}else couleur=0;
		for(j=100-BUTTON_SIZE;j<=100+BUTTON_SIZE;j++){
			ram_tableauImage[320*j+i]=couleur;
		}
	}
}

#endif
