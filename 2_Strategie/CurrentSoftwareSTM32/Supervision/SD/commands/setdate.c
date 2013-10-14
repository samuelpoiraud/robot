/*
 * setdate.c
 *
 *  Created on: 14 oct. 2013
 *      Author: Arnaud
 */

#include "setdate.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"
#include "../../RTC.h"

const char term_cmd_setdate_brief[] = "Mise à jour de la RTC";
const char term_cmd_setdate_help[] =
		"Utilisation : setdate année mois jour journée heure minute\n"
		"\n"
		"année      2013 -> 13\n"
		"journée    1 -> Lundi\n";


int term_cmd_setdate(int argc, const char *argv[]) {
	date_t date;
	long temp;
	date.seconds = 0;

	if(argc > 6)
		return EINVAL;
	else if(argc < 6)
		return EINVAL;

	if(!argtolong(argv[0], 0, &temp))
		return EINVAL;
	date.year = (Uint8)temp;
	if(!argtolong(argv[1], 0, &temp))
			return EINVAL;
	date.month = (Uint8)temp;
	if(!argtolong(argv[2], 0, &temp))
			return EINVAL;
	date.date = (Uint8)temp;
	if(!argtolong(argv[3], 0, &temp))
				return EINVAL;
	date.day = (Uint8)temp;
	if(!argtolong(argv[4], 0, &temp))
			return EINVAL;
	date.hours = (Uint8)temp;
	if(!argtolong(argv[5], 0, &temp))
			return EINVAL;
	date.minutes = (Uint8)temp;

	RTC_set_time(&date);

	return 0;
}






