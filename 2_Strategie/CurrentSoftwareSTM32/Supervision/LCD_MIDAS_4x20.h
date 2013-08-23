/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD_MIDAS_4x20.c
 *	Package : Supervision
 *	Description : implémentation du driver de l'écran LCD MCCOG42005A6W-BNMLWI
 *	Licence : CeCILL-C
 *	Auteur : NIRGAL
 *	Version 201307
 */

#ifndef LCD_MIDAS_4X20_H
#define	LCD_MIDAS_4X20_H

#define RESET_PIN	LATGbits.LATG7
#define RESET_TRIS	TRISGbits.TRISG7

/*
	These functions must be called once prior to others, in the right order :
*/
void LCD_I2C_init(void);		//First init : I2C
void LCD_init(void);			//Second init : LCD


/*
	Write the text in "string" (until the '\0') at the current cursor position
*/
void LCD_Write_text(char * string);

/*
	If you need to change the contrast. 7 is a good choice in my case.
*/
void LCD_set_contrast(Uint8 contrast);

/*
	line : from 0 to LCD_SIZE_LINE-1
	column : from 0 to LCD_SIZE_COLUMN-1
*/
void LCD_set_cursor(Uint8 line, Uint8 column);

/*
	To show or mask cursor... If show, to blink or not it
*/
void LCD_cursor_display( bool_e show, bool_e blink);

/*
	Test process, should be called in loop...
*/
void LCD_test(void);

/*
	Clear the display
*/
void LCD_clear_display(void);

#endif	/* LCD_MIDAS_4X20_H */

