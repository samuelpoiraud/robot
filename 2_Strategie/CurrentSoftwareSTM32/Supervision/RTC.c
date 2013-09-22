/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : RTC.c
 *	Package : Supervision
 *	Description : Gestion de la RTC I2C sur dsPIC30F6010A
 *	Auteur : Nirgal
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *  Code inspiré de : EMBEDDED-CODE.COM / MAXIM DS1307 REAL TIME CLOCK DRIVER /
 *	Version 20110518
 */
 
#define QS_I2C_C
#include "RTC.h"

	#include "../QS/QS_i2c.h"
	#include "../QS/QS_CANmsgList.h"
	#include "../QS/QS_can.h"

	#define	DS1307_I2C_ADDRESS			0xd0

	#define	RTC_I2C_START_I2C()					I2C_GenerateSTART(I2C2_I2C_HANDLE, ENABLE)										//Generate bus start condition
	#define	RTC_I2C_START_IN_PROGRESS_BIT		FALSE											//Bit indicating start is still in progress
	#define	RTC_I2C_RESTART_I2C()				I2C_GenerateSTART(I2C2_I2C_HANDLE, ENABLE)								//Generate bus restart condition
	#define	RTC_I2C_RESTART_IN_PROGRESS_BIT		FALSE									//Bit indicating re-start is still in progress
	#define	RTC_I2C_STOP_I2C()					I2C_GenerateSTOP(I2C2_I2C_HANDLE, ENABLE)										//Generate bus stop condition
	#define	RTC_I2C_STOP_IN_PROGRESS_BIT		FALSE									//Bit indicating Stop is still in progress
	#define	RTC_I2C_WRITE_BYTE(a)				I2C_SendData(I2C2_I2C_HANDLE,a)												//Write byte to I2C device
	#define	RTC_I2C_TX_IN_PROGRESS_BIT			(!I2C_GetFlagStatus(I2C2_I2C_HANDLE,I2C_FLAG_BTF)										//Bit indicating transmit byte is still in progress
	#define	RTC_I2C_ACK_NOT_RECEIVED_BIT		I2C_GetFlagStatus(I2C2_I2C_HANDLE,I2C_FLAG_AF)									//Bit that is high when ACK was not received
	#define	RTC_I2C_READ_BYTE					I2C_ReceiveData(I2C2_I2C_HANDLE)													//Read byte from I2C device function / result byte of RTC_I2C_READ_FUNCTION_START
	#define RTC_I2C_ACK()							I2C_AcknowledgeConfig(I2C2_I2C_HANDLE,ENABLE)			//Generate bus ACK condition
	#define RTC_I2C_NOT_ACK()						I2C_AcknowledgeConfig(I2C2_I2C_HANDLE,DISABLE)			//Generate bus Not ACK condition
	#define	RTC_I2C_IDLE_I2C()					while (I2C_GetFlagStatus(I2C2_I2C_HANDLE,I2C_FLAG_BUSY))			//Test if I2C1 module is idle (wait until it is ready for next operation)



volatile date_t local_date;
volatile bool_e local_date_updated = FALSE;
volatile bool_e periodic_it_was_called = FALSE;

void RTC_init(void) 
{
	#ifdef USE_RTC
	#ifdef USE_I2C2
		I2C_init();
	#endif // def USE_I2C
	#endif	//def USE_RTC
}


//**********************************************
//The fields are converted from binary to BCD before being sent to the RTC
//All fields are 0 - #, except day, date and month, which are 1 - #
//Pointers are used for comanilty with rtc_get_time
//Uses 24 hour clock, not 12 hour
Uint8 RTC_set_time  (date_t * date)
{
	debug_printf("RTC : nouvelle date demandée : ");
	switch(date->day)
	{
		case 1: debug_printf("lundi"); 			break;
		case 2: debug_printf("mardi"); 			break;
		case 3: debug_printf("mercredi"); 		break;
		case 4: debug_printf("jeudi");	 		break;
		case 5: debug_printf("vendredi"); 		break;
		case 6: debug_printf("samedi"); 		break;
		case 7: debug_printf("dimanche"); 		break;
		default: debug_printf("invalid_day"); break;
	}
	debug_printf(" %.2d/%.2d/20%.2d %.2d:%.2d:%.2d\n", date->date,date->month,date->year,date->hours,date->minutes,date->seconds);
	if (date->seconds > 59 || date->minutes > 59 || date->hours > 23 || date->day > 7 || date->day == 0 || date->date > 31 || date->month > 12  || date->year > 99)
	{
		debug_printf("RTC ERROR : La date demandée n'est pas cohérente. \n");
		return FALSE;
	}
	#ifdef USE_RTC
	Uint8 temp;
	Uint8 datas[9];

	datas[0] = 0x00;

	temp = (date->seconds / 10);
	datas[1] = ((date->seconds - (temp * 10)) + (temp << 4)) & 0x7f;	//Bit7 = enable oscillator

	temp = (date->minutes / 10);
	datas[2] = (date->minutes - (temp * 10)) + (temp << 4);

	temp = (date->hours / 10);
	datas[3] = 	((date->hours - (temp * 10)) + (temp << 4)) & 0x3f;		//Bit6 low = set format to 24 hour

	temp = (date->day / 10);
	datas[4] = (date->day - (temp * 10)) + (temp << 4);

	temp = (date->date / 10);
	datas[5] = (date->date - (temp * 10)) + (temp << 4);

	temp = (date->month / 10);
	datas[6] = (date->month - (temp * 10)) + (temp << 4);

	temp = (date->year / 10);
	datas[7] = (date->year - (temp * 10)) + (temp << 4);

	datas[8] = 0x00;

	local_date_updated = FALSE;	//La date locale n'est plus à jour. On ne prend pas pour autant la nouvelle date comme date locale, puisqu'en cas d'échec, mieux vaut avoir relu la date RTC réelle.

	return I2C2_write(DS1307_I2C_ADDRESS, datas, 9, TRUE);
	#else
		return FALSE;
	#endif

	
}


//Returns:
//	1 if sucessful, 0 if not.
//All fields are converted to binary values
//All fields are 0 - #, except day, date and month, which are 1 - #
//Uses 24 hour clock, not 12 hour
Uint8 RTC_get_time (date_t * date)
{
	date->seconds = 0;
	date->minutes = 0;
	date->hours = 0;
	date->day = 0;
	date->date = 1;
	date->month = 1;
	date->year = 0;
	#ifdef USE_RTC
	Uint8 datas[7];
	datas[0] = 0x00;	//@ du premier registre
	if(I2C2_write(DS1307_I2C_ADDRESS, datas, 1, FALSE))	//Condition de stop non envoyée...
	{
		if(I2C2_read(DS1307_I2C_ADDRESS, datas, 7))	//Le START sera donc ici un RESTART
		{
			local_date.seconds 	= (datas[0] & 0x0f) + (((datas[0] & 0x70) >> 4) * 10);		//(Bit 7 is osc flag bit - dump)
			local_date.minutes 	= (datas[1] & 0x0f) + (((datas[1] & 0x70) >> 4) * 10);
			local_date.hours 	= (datas[2] & 0x0f) + (((datas[2] & 0x30) >> 4) * 10);
			local_date.day 		= (datas[3] & 0x07);
			local_date.date 	= (datas[4] & 0x0f) + (((datas[3] & 0x30) >> 4) * 10);
			local_date.month 	= (datas[5] & 0x0f) + (((datas[5] & 0x10) >> 4) * 10);
			local_date.year 	= (datas[6] & 0x0f) + ((datas[6] >> 4) * 10);
			local_date_updated = TRUE;
			*date = local_date;
			return TRUE;
		}
	}
	#endif
	return FALSE;
}


//Interroge la RTC SEULEMENT SI nécessaire.
//  Il n'est pas nécessaire d'interroger la RTC si on dispose de la date en local et que le process_it_1sec a été appelée au moins une fois.
Uint8 RTC_get_local_time (date_t * date)
{
	if(local_date_updated && periodic_it_was_called)
	{
		*date = local_date;
		return TRUE;
	}
	else
		return RTC_get_time(date);
}


//Fonction devant être appelée chaque seconde précisément, OU pas du tout... mais pas entre les deux !
void RTC_process_it_1sec(void)
{
	periodic_it_was_called = TRUE;
	if(local_date_updated == FALSE)	//si on a pas de date locale, inutile de la mettre à jour !
		return;

	local_date.seconds++;
	if(local_date.seconds >= 60)
	{
		local_date.seconds = 0;
		local_date.minutes++;
		if(local_date.minutes >= 60)
		{
			local_date.minutes = 0;
			local_date.hours++;
			if(local_date.hours >= 24)
			{
				local_date.hours = 0;
				local_date.day++;
				local_date.date++;
				if(local_date.day >= 7)
					local_date.day = 1;
				//On ignore volontairement la gestion du débordement de la date.
				//Ce bug ne se produit qu'une fois par mois... à minuit, le dernier jour. Et au prochain reset, il disparait.
			}
		}
	}

}


void RTC_print_time(void)
{
	date_t date;
	RTC_get_local_time(&date);
	switch(date.day)
	{
		case 1: debug_printf("lundi"); 			break;
		case 2: debug_printf("mardi"); 			break;
		case 3: debug_printf("mercredi"); 		break;
		case 4: debug_printf("jeudi");	 		break;
		case 5: debug_printf("vendredi"); 		break;
		case 6: debug_printf("samedi"); 		break;
		case 7: debug_printf("dimanche"); 		break;
		default: debug_printf("invalid_day"); break;
	}	
	debug_printf(" %.2d/%.2d/20%.2d %.2d:%.2d:%.2d\n", date.date,date.month,date.year,date.hours,date.minutes,date.seconds);
}	

void RTC_can_send(void)
{
	CAN_msg_t msg;
	date_t date;
	RTC_get_local_time(&date);
	msg.sid = DEBUG_RTC_TIME;
	msg.size = 7;
	msg.data[0] = date.seconds;
	msg.data[1] = date.minutes;
	msg.data[2] = date.hours;
	msg.data[3] = date.day;
	msg.data[4] = date.date;
	msg.data[5] = date.month;
	msg.data[6] = date.year;
	CAN_send(&msg);	
}	


