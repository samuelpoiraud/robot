/*----------------------------------------------------------------------*/
/* FAT file system test terminal for FatFs R0.07a  (C)ChaN, 2008        */
/* modified by Martin Thomas 4/2009 for the STM32 example               */
/* updated and modified by Samuel Poiraud 9/2013						*/
/*----------------------------------------------------------------------*/

#include <string.h> /* memset et al*/
#include "../../QS/QS_all.h"
#include "../../QS/QS_uart.h"
#include "../../QS/QS_outputlog.h"
#include "Libraries/fat_sd/integer.h"
#include "Libraries/fat_sd/diskio.h"
#include "Libraries/fat_sd/ff.h"
#include "term_io.h"
#include "ff_test_term.h"
#include "../../propulsion/prop_functions.h"

#include <string.h>
#include <ctype.h>
#include "term_commands_array.h"
#include "term_commands_utils.h"  //to check size
#include "commands/help.h"
#include <string.h>

bool_e execute_command(int argc, const char *argv[]) {
	Uint8 i;
	int err;

	for(i = 0; i < term_command_info_count; i++) {
		if(strcmp(argv[0], term_commands[i].name) == 0) {
			err = term_commands[i].execute(argc-1, argv+1);
			if(err) {
				debug_printf("%s: Error %d: %s\n", argv[0], err, strerror(err));
			}

			assert(term_common_buffer_size == 512); // (note: term_common_buffer_size est constant mais en m�moire, donc quelqu'un peut l'�craser et cette valeur ne sera plus constante. Ceci dit, la transformer en define ne r�soudra pas le probl�me de l'overflow de tableau)

			if(err == 22) //EINVAL invalid argument
				term_cmd_help(1, argv);

			return TRUE;
		}
	}

	debug_printf("Unknown command %s, type help to show all available commands\n", argv[0]);

	return FALSE;
}

void parse_command(const char *cmd) {
	static char cmd_buffer[128];
	static const char *args[16];
	Uint8 cmd_idx, arg_idx;

	//Copie la ligne de commande en mettant des \0 � la fin des arguments (pour les lires ind�pendamment � partir de args[])
	for(cmd_idx = arg_idx = 0; cmd[cmd_idx] && cmd_idx < 127 && arg_idx < 15; cmd_idx++) {
		if(cmd_idx > 0 && !isspace((int)cmd[cmd_idx-1]) && isspace((int)cmd[cmd_idx])) {
			cmd_buffer[cmd_idx] = '\0';
		} else {
			cmd_buffer[cmd_idx] = cmd[cmd_idx];
		}

		if((cmd_idx == 0 || isspace((int)cmd[cmd_idx-1])) && !isspace((int)cmd[cmd_idx])) {
			args[arg_idx] = &cmd_buffer[cmd_idx];
			arg_idx++;
		}

	}
	args[arg_idx] = 0;
	cmd_buffer[cmd_idx] = 0;

	execute_command(arg_idx, args);
}

//Return false si une sortie du terminal est demand�e
bool_e terminal_process_main (void)
{
	char * ptr;
	bool_e ret;

	typedef enum
	{
		INIT = 0,
		SEND_PROMPT,
		WAIT_COMMAND,
		EXECUTE_COMMAND
	}state_e;
	static state_e state = INIT;

	ret = TRUE;	//On garde la main
	switch (state)
	{
		case INIT:
			debug_printf("terminal ready : type \"help\" for command list\n");
			state = SEND_PROMPT;
			break;
		case SEND_PROMPT:
			debug_printf(">");
			state = WAIT_COMMAND;
			break;
		case WAIT_COMMAND:
			ptr = get_command();
			if(ptr)
			{
				parse_command(ptr);
				state = SEND_PROMPT;
			}
			break;
		default:
			state = INIT;
			break;
	}
	if(ret == FALSE)
		state = INIT;	//Une sortie du terminal est demand�e...
	return ret;
}

