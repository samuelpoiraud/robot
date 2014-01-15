/*
 * LCD.h
 *
 *  Created on: 23 d�c. 2013
 *      Author: Nirgal
 */

#ifndef LCD_H_
#define LCD_H_

#include "../QS/QS_all.h"


void LCD_init(void);

void LCD_process_main(void);

void LCD_process_10ms(void);



typedef enum
{
	FRIEND_1 = 0,
	FRIEND_2,
	ADVERSARY_1,
	ADVERSARY_2,
	ROBOTS_NUMBER,
	NONE // Utile pour la s�lection
}robots_e;

#define US FRIEND_1

typedef struct
{
	Uint16 x;
	Uint16 y;
	Uint16 xprec;
	Uint16 yprec;
	Uint16 size;
	bool_e updated;
	Uint8 color;
	bool_e enable;
}display_robot_t;

//Permet de savoir si le robot "robot" a �t� activ�
bool_e is_robot_enabled(robots_e robot);

//D�sactive un robot s�lectionn�
void set_robot_disable(robots_e robot);

//Active un robot s�lectionn�
void set_robot_enable(robots_e robot);

//x et y en cm.
void LCD_update_robot(robots_e robot, Uint16 x, Uint16 y);

#endif /* LCD_H_ */
