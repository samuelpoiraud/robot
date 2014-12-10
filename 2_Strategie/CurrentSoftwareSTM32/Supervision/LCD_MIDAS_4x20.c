/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD_MIDAS_4x20.c
 *	Package : Supervision
 *	Description : impl�mentation du driver de l'�cran LCD MCCOG42005A6W-BNMLWI (controleur : SSD1803A)
 *	Licence : CeCILL-C
 *	Auteur : NIRGAL
 *	Version 201307
 */

#include "../QS/QS_i2c.h"
#include "../QS/QS_outputlog.h"
#include "LCD_MIDAS_4x20.h"

#if defined(USE_LCD) && !defined(USE_I2C2)
	#warning "LCD a besoin de I2C2 pour fonctionner. Veuiller definir USE_I2C2 dans config_qs.h. Le module LCD sera inactif ..."
	#undef USE_LCD
#endif


//si n�gatif ou non d�fini : pas de d�sactivation LCD en cas d'erreurs
#define LCD_MAX_I2C_ERROR 0   //On ne veut aucune erreur, sinon on d�sactive le LCD

#define LCDADDR 0x78
#define LCD_SIZE_LINE	4
#define LCD_SIZE_COLUMN	20


#define CONTROL_BYTE_FOR_DATA						0x40
#define CONTROL_BYTE_FOR_COMMAND					0x00

#define COMMAND_CLEAR_DISPLAY 						0x01
#define COMMAND_8BIT_4LINES_NORMAL_RE1_IS0			0x3A	//Extended command access RE = 1
#define COMMAND_8BIT_4LINES_REVERSE_RE1_IS0			0x3B	//Extended command access RE = 1
#define COMMAND_8BIT_4LINES_RE0_IS1					0x39	//Extented command access IS = 1
#define COMMAND_8BIT_4LINES_RE0_IS0					0x38	//Normal mode...


//Command from extended set (RE = 1, IS = 0)
#define COMMAND_BS1_1								0x1E
#define COMMAND_POWER_DOWN_DISABLE					0x02
#define COMMAND_SEGMENT_NORMAL_WAY					0x05
#define COMMAND_NW									0x09

//Command from extended set (RE = 0, IS = 1)
#define COMMAND_DISPLAY_ON_CURSOR_ON_BLINK_ON		0x0F
#define COMMAND_DISPLAY_ON_CURSOR_ON_BLINK_OFF		0x0E
#define COMMAND_DISPLAY_ON_CURSOR_OFF_BLINK_OFF		0x0C
#define COMMAND_BS0_1								0x1C
#define COMMAND_INTERNAL_DIVIDER					0x13
#define COMMAND_CONTRAST							0x77
#define COMMAND_POWER_ICON_CONTRAST					0x5C
#define COMMAND_FOLLOWER_CONTROL					0x6E


#define ADDRESS_DDRAM								0x80


static bool_e initialized = FALSE;
static void LCD_handle_i2c_result(bool_e result);



//////////////////////////////////////////////I2C : low level functions //////////////////////

void LCD_I2C_init(void)
{
#ifdef USE_LCD
	I2C_init();
#endif
	initialized = TRUE;
}



//rw : 0 to write, 1 to read
typedef enum
{
	WRITING_FRAME = 0,
	READING_FRAME = 1
}read_write_e;


/*
Uint8 LCD_I2C_read_byte(bool_e send_ack)
{
	Uint8 ret;
	IdleI2C();
	ret = MasterReadI2C();
	IdleI2C();
	if(send_ack)
	{
		I2CCONbits.ACKDT = 0;
		I2CCONbits.ACKEN = 1;
	}
	return ret;
}
*/

////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS : low level for LCD


void LCD_send_command(Uint8 command)
{
	if(!initialized)
		return;

	Uint8 datas[3];
	datas[0] = CONTROL_BYTE_FOR_COMMAND;		// Control byte (C0 = 0, D/C = C)
	datas[1] = command;
#ifdef USE_LCD
	LCD_handle_i2c_result(I2C2_write(LCDADDR, datas, 2, TRUE));
#endif
}


void LCD_clear_display(void)
{
	LCD_send_command(COMMAND_CLEAR_DISPLAY);	//Clear display
}

/*
	Initialize the LCD.
*/
void LCD_init(void){
	volatile Uint32 i;
	for(i=0;i<100000;i++);

#ifdef FDP_2014
	GPIO_ResetBits(LCD_RESET_PORT);
	LCD_bitsReset(FALSE);
#else
	LCD_bitsReset(FALSE);
#endif
	for(i=0;i<100000;i++);	//Delay > 10ms.
#ifdef FDP_2014
	GPIO_SetBits(LCD_RESET_PORT);
	LCD_bitsReset(TRUE);
#else
	LCD_bitsReset(TRUE);
#endif
	for(i=0;i<10000;i++);	//Delay > 1ms

	LCD_send_command(COMMAND_CLEAR_DISPLAY);	//Clear display

	//RE = 1  |  IS = 0
	LCD_send_command(COMMAND_8BIT_4LINES_NORMAL_RE1_IS0);		//Function set : 8bit 4line RE = 1, IS = 0, Not reverse
	//LCD_Command(COMMAND_8BIT_4LINES_REVERSE_RE1_IS0);	//Function set : RE = 1, REVERSE
	LCD_send_command(COMMAND_BS1_1);	//Set BS1 = 1 (1/6 bias)
	LCD_send_command(COMMAND_POWER_DOWN_DISABLE);	//Power down disable
	LCD_send_command(COMMAND_SEGMENT_NORMAL_WAY);	// Segment bidirection : in the right way
	LCD_send_command(COMMAND_NW);	//NW = 1

	//RE = 0  |  IS = 1
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS1);	//Function set : RE = 0, IS = 1
	LCD_send_command(COMMAND_DISPLAY_ON_CURSOR_ON_BLINK_ON);	//Display ON (with cursor...)
	LCD_send_command(COMMAND_BS0_1);	//Set BS0 = 1 (1/6 bias) / Internal frequency..
	LCD_send_command(COMMAND_INTERNAL_DIVIDER);	//Internal divider
	LCD_send_command(COMMAND_CONTRAST);	//Constrast
	LCD_send_command(COMMAND_POWER_ICON_CONTRAST);	//Power Icon control contrast
	LCD_send_command(COMMAND_FOLLOWER_CONTROL);	//Follower Control

	//RE = 0  | IS = 0
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS0);	//Function set : 8bit 4line RE = 0, IS = 0

}

////////////////////////////////////////////////////////////////////////


//Contrast : 0 to 63.. (in my case, < 10 is a good choice)
void LCD_set_contrast(Uint8 contrast)
{
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS1);	//Function set : RE = 0, IS = 1
	LCD_send_command(0x70 | (contrast & 0x0F));			//Constrast
	LCD_send_command(COMMAND_POWER_ICON_CONTRAST | ((contrast >> 4) & 0x03));	//Power Icon control contrast
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS0);	//Function set : RE = 0, IS = 0
}

#define SIZE_READ 160
//Function used only for debug...
/*
void LCD_Dump(void)
{
	Uint8 ret[SIZE_READ],i;
	printf("READ %x ->\n  0000 ",(int)(0));

	LCD_send_command(0);

	LCD_I2C_begin_frame(READING_FRAME);
	LCD_I2C_send_byte(CONTROL_BYTE_FOR_COMMAND);		// Control byte (C0 = 0, D/C = C)

	for(i=0;i<SIZE_READ;i++)
	{
		ret[i] = LCD_I2C_read_byte((i<SIZE_READ-1)?TRUE:FALSE);	//Send Ack	-> NO ack at the last byte
	}

	LCD_I2C_end_frame();

	for(i=0;i<SIZE_READ;i++)
	{
		if(i == 0)
			printf("xx ");
		else
			printf("%02X ", ret[i]);
		if(i%8 == 7)
			printf("\n  %04X ", i+1);
	}
	printf("\n");
}
*/

void LCD_cursor_display(bool_e show, bool_e blink)
{
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS1);	//Function set : RE = 0, IS = 1
	LCD_send_command(0x0C | ((show)?0x02:0x00) | ((blink)?0x01:0x00));
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS0);	//Function set : RE = 0, IS = 0
}

/*
	line : from 0 to LCD_SIZE_LINE-1
	column : from 0 to LCD_SIZE_COLUMN-1
*/
void LCD_set_cursor(Uint8 line, Uint8 column)
{
	line = MIN(line,LCD_SIZE_LINE-1);
	column = MIN(column,LCD_SIZE_COLUMN-1);

	LCD_send_command(ADDRESS_DDRAM | (line*0x20 + column));
}

/*
Uint8 Read_AC(void)
{
	Uint8 ac, id;
	LCD_I2C_begin_frame(WRITING_FRAME);
	LCD_I2C_send_byte(CONTROL_BYTE_FOR_COMMAND);
	LCD_I2C_end_frame();

	LCD_I2C_begin_frame(READING_FRAME);
	ac = LCD_I2C_read_byte(TRUE);
	id = LCD_I2C_read_byte(FALSE);	//id should be 0x1A
	LCD_I2C_end_frame();

	//printf("ac=%02X | id=%02X\n",ac,id);
	return ac;
}
*/
/*
 * string ends with '\0'...
 */
void LCD_Write_text(char * string)
{
	Uint16 i;
	Uint8 datas[21];
	Uint8 index;
	index = 0;

	if(!initialized)
		return;

	datas[index++] = CONTROL_BYTE_FOR_DATA;     // Control byte for Data

	for(i=0;string[i];i++)
	{
		if(index >= 21)
			break;
		switch(string[i])	//For special characters
		{
			case '�':	datas[index++] = (0x5E);	break;
			case '�':	datas[index++] = (0xA5);	break;
			case '�':	datas[index++] = (0x7B);	break;
			case '�':	datas[index++] = (0x7F);	break;
			//case '':	datas[index++] = (0x86);	break;
			case '�':	datas[index++] = (0xA9);	break;
			case '�':	datas[index++] = (0xC7);	break;
			//case '�':	datas[index++] = (0x89);	break;
			case '�':	datas[index++] = (0xA4);	break;
			//case '�':	datas[index++] = (0x8B);	break;
			//case '�':	datas[index++] = (0x8C);	break;
			//case '':	datas[index++] = (0x8D);	break;
			//case '�':	datas[index++] = (0x8E);	break;
			case '|':	datas[index++] = (0xFE);	break;
			case '_':	datas[index++] = (0xC4);	break;
			case '�':	datas[index++] = (0xDC);	break;
			case (char)(0xFF):	datas[index++] = (0x1F);	break;	//FULL SQUARE

			default:
				datas[index++] = string[i];
			break;
		}
	}
#ifdef USE_LCD
	LCD_handle_i2c_result(I2C2_write(LCDADDR, datas, index, TRUE));
#endif
}

/*
This function must be called PERIODICALLY..
 */
void LCD_test(void)
{
	static bool_e done = FALSE;
	static Uint32 n = 0;
	char tmp[10];
	if(!done)
	{
		LCD_cursor_display(FALSE,FALSE);
		LCD_set_cursor(0, 0);
		LCD_Write_text("Bonjour !");
		done = TRUE;
		LCD_set_cursor(1, 3);
		LCD_Write_text("Ligne 2");

		LCD_set_cursor(2, 0);
		LCD_Write_text("D�but Ligne 3");

		LCD_set_cursor(29, 0);
		LCD_Write_text("�������...!");
	}
	else
	{
		LCD_set_cursor(0, 10);
		n++;
		sprintf(tmp, "%ld", n);
		LCD_Write_text(tmp);
	}
}

static void LCD_handle_i2c_result(bool_e result) {
	static Uint8 error_count = 0;

#if defined(LCD_MAX_I2C_ERROR) && LCD_MAX_I2C_ERROR >= 0
	if(result == FALSE)
		error_count++;
	else
		error_count = 0;

	if(error_count > LCD_MAX_I2C_ERROR) {
		//initialized = FALSE;
		//debug_printf("LCD: trop d'erreur I2C, LCD off\n");
		error_count = 0;
		debug_printf("LCD: trop d'erreur I2C\n");
	}
#endif
}

// Envoie un message CAN pour set ou reset le bit du LCD qui se trouve sur la carte IHM  (fdp 2015)
void LCD_bitsReset(bool_e set){
	CAN_msg_t msg;
	msg.sid = IHM_LCD_BIT_RESET;
	msg.data[0] = set;
	msg.size = 1;
	CAN_send(&msg);
}
