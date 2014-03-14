/**
 * \file LCD.h
 * \brief Fichier pricncipal de la fonctionnalit� LCD_Touch
 * \author Nirgal, Shinitenshi & Herzaeone
 * \version 20140123
 * \date 23 janvier 2014
 *
 * Fichier de d�claration des fonctions d'initialisation et de routine du LCD_TOUCH
 */

#ifndef LCD_H_
#define LCD_H_

#include "../QS/QS_all.h"


void LCD_init(void);

void LCD_process_main(void);

void LCD_process_10ms(void);


/**
 * \enum robots_e
 * \brief Nomination des robots
 *
 * Enum�ration des possibles robots sur le terrein
 */
typedef enum
{
	FRIEND_1 = 0,	/**< Robot pr�sent physiquement (carte propulsion) */
	FRIEND_2,		/**< Robot alli� */
	ADVERSARY_1,	/**< Premier robot adverse */
	ADVERSARY_2,	/**< Second robot adverse */
	ROBOTS_NUMBER,	/**< Nombre de robots � g�rer */
	NONE 			/**< Aucun robot s�lectionn� */
}robots_e;

#define US FRIEND_1 /**< Macro qui donne un second nom � notre propre robot */


/**
 * \struct display_robot_t
 * \brief Donn�es relatives � chaque robot
 *
 * Informations relatives aux robots tels que la position, la pr�cision, la couleur, la taille, la mise a jour, la couleur et l'activation
 */
typedef struct
{
	Sint16 x;		/**< position en x */
	Sint16 y;		/**< position en y */
	Sint16 teta;
	Sint16 xprec;	/**< position pr�cedent en x */
	Sint16 yprec;	/**< position pr�c�dente en u */
	Sint16 tetaprec;
	Uint16 size;	/**< taille du carr� repr�sentant le robot */
	bool_e updated;	/**< informe si le robot a �t� mis � jour */
	Uint8 color;	/**< couleur du robot en question */
	bool_e enable;	/**< TRUE si le robot est activ� FALSE sinon */
	bool_e friend;  /**< TRUE si robot est � nous (pour affichage) */
}display_robot_t;


bool_e is_robot_enabled(robots_e robot);

//D�sactive un robot s�lectionn�
void set_robot_disable(robots_e robot);

//Active un robot s�lectionn�
void set_robot_enable(robots_e robot);

//x et y en cm.
void LCD_update_robot(robots_e robot, Uint16 x, Uint16 y);

#endif /* LCD_H_ */
