/*
 * LCD.h
 *
 *  Created on: 23 déc. 2013
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
	ROBOTS_NUMBER
}robots_e;

typedef struct
{
	Uint16 x;
	Uint16 y;
	Uint16 xprec;
	Uint16 yprec;
	Uint16 size;
	bool_e updated;
	Uint8 color;
}display_robot_t;

//x et y en cm.
void LCD_update_robot(robots_e robot, Uint16 x, Uint16 y);

#endif /* LCD_H_ */
