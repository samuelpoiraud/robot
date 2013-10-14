/*
 * date.c
 *
 *  Created on: 14 oct. 2013
 *      Author: Arnaud
 */


#include "date.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"
#include "../../RTC.h"

const char term_cmd_date_brief[] = "Affichage date";
const char term_cmd_date_help[] =
		"Utilisation : Aucun argument\n"
		"\n";


int term_cmd_date(int argc, const char *argv[]) {
	date_t date;

	if(argc > 0)
		return EINVAL;

	RTC_get_time(&date);

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

	return 0;
}






