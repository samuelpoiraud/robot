/*
 * print_match.c
 *
 *  Created on: 29 april 2014
 *      Author: Arnaud
 */
#include "print_match.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"
#include "../SD.h"

const char term_cmd_print_match_brief[] = "Affiche le numéro du match voulu";
const char term_cmd_print_match_help[] =
		"Utilisation : print_match nb_match\n"
		"\n"
		"nb_match       numéro du match voulue\n";


int term_cmd_print_match(int argc, const char *argv[]) {

	Sint32 nb_match;

	if(argc < 1)
		return EINVAL;
	else if(argc > 1)
		return EINVAL;

	if(!argtolong(argv[0], 10, &nb_match))
		return EINVAL;

	SD_print_match(nb_match);

	return 0;
}




