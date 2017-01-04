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

#include "RTC.h"

#include "../QS/QS_i2c.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_can.h"
#include "../QS/QS_outputlog.h"

#if defined(USE_RTC) && !defined(USE_I2C2)
	#warning "RTC a besoin de I2C2 pour fonctionner. Veuiller definir USE_I2C2 dans config_qs.h. Le module RTC sera inactif ..."
	#undef USE_RTC
#endif

#define	DS1307_I2C_ADDRESS			0xd0

static bool_e I2C_read_RTC(Uint8 address, Uint8 cmd, Uint8 * data, Uint8 size);
static bool_e I2C_write_RTC(Uint8 address, Uint8 * data, Uint8 size);

volatile date_t local_date;
volatile bool_e local_date_updated = FALSE;
volatile bool_e periodic_it_was_called = FALSE;
volatile bool_e rtc_updated = FALSE;	//lorsque nous mettons à jour la RTC, il faut autoriser la fonction get à la lire à nouveau...

void RTC_init(void)
{
	#ifdef USE_RTC
		I2C_init();
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

		if(I2C_write_RTC(DS1307_I2C_ADDRESS, datas, 9))
		{
			debug_printf("RTC mise à jours\n");
			rtc_updated = TRUE;
		}
		else
		{
			debug_printf("RTC ERROR I2C\n");
			return FALSE;
		}

		return TRUE;
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
		static Uint8 nb_try = 0;
		Uint8 datas[7];
		Uint8 cmd = 0x00;	//@ du premier registre
		if(nb_try < 2 || rtc_updated)
		{
			rtc_updated = FALSE;
			nb_try++;
			if(I2C_read_RTC(DS1307_I2C_ADDRESS, cmd, datas, 7))
			{
				local_date.seconds 	= (datas[0] & 0x0f) + (((datas[0] & 0x70) >> 4) * 10);		//(Bit 7 is osc flag bit - dump)
				local_date.minutes 	= (datas[1] & 0x0f) + (((datas[1] & 0x70) >> 4) * 10);
				local_date.hours 	= (datas[2] & 0x0f) + (((datas[2] & 0x30) >> 4) * 10);
				local_date.day 		= (datas[3] & 0x07);
				local_date.date 	= (datas[4] & 0x0f) + (((datas[4] & 0x30) >> 4) * 10);
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
	msg.size = SIZE_DEBUG_RTC_TIME;
	msg.data.debug_rtc_time.seconde = date.seconds;
	msg.data.debug_rtc_time.minute = date.minutes;
	msg.data.debug_rtc_time.heure = date.hours;
	msg.data.debug_rtc_time.journee = date.day;
	msg.data.debug_rtc_time.jour = date.date;
	msg.data.debug_rtc_time.mois = date.month;
	msg.data.debug_rtc_time.annee = date.year;
	CAN_send(&msg);
}

static bool_e I2C_read_RTC(Uint8 address, Uint8 cmd, Uint8 * data, Uint8 size){
	Uint8 reg[1] = {cmd};

	return I2C_Read(I2C2_I2C_HANDLE, address, reg, 1, data, size);
}

static bool_e I2C_write_RTC(Uint8 address, Uint8 * data, Uint8 size){
	return I2C_Read(I2C2_I2C_HANDLE, address, NULL, 0, data, size);
}
