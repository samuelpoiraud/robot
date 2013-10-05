#include "ls.h"
#include <errno.h>
#include "../term_commands_array.h"
#include "../../../QS/QS_uart.h"
#include <string.h>

const char term_cmd_help_brief[] = "Affiche une aide sur les commandes";
const char term_cmd_help_help[] =
		"Utilisation : help [cmd...]\n"
		"\n"
		"cmd...     Affiche l'aide détaillée des commandes listées\n";

static int get_cmd_id(const char *name) {
	int i;

	for(i = 0; i < term_command_info_count; i++) {
		if(!strcmp(term_commands[i].name, name)) {
			return i;
		}
	}

	return -1;
}


int term_cmd_help(int argc, const char *argv[]) {
	if(argc < 1) {
		int i;

		for(i = 0; i < term_command_info_count; i++) {
			debug_printf("%s - %s\n", term_commands[i].name, term_commands[i].brief_description);
		}
		debug_printf("\n");
	} else {
		int i;

		for(i = 0; i < argc; i++) {
			int cmd_id = get_cmd_id(argv[i]);

			debug_printf("%s - %s\n%s\n", term_commands[cmd_id].name, term_commands[cmd_id].brief_description, term_commands[cmd_id].full_help);
		}
	}

	return 0;
}
