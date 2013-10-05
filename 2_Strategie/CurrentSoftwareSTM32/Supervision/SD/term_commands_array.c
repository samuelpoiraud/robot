#include "term_commands_array.h"
#include "term_commands_utils.h"
#include "commands/commands.h"

#ifndef NULL
#define NULL (void*)0
#endif

#define DECLARE_CMD(cmd) {#cmd, &term_cmd_ ## cmd, term_cmd_ ## cmd ## _brief, term_cmd_ ## cmd ## _help}

term_command_info_t term_commands[] = {
/*   name, execute_function, brief desc, cmd help */
	DECLARE_CMD(cat),
	DECLARE_CMD(clean),
	DECLARE_CMD(cp),
	DECLARE_CMD(disk),
	DECLARE_CMD(goto),
	DECLARE_CMD(ls),
	DECLARE_CMD(mkdir),
	DECLARE_CMD(rm)

//	{"md", &term_cmd_md, NULL, NULL},
//	{"dd", &term_cmd_dd, NULL, NULL},
//	{"di", &term_cmd_di, NULL, NULL},
//	{"ds", &term_cmd_ds, NULL, NULL},
//	{"bd", &term_cmd_bd, NULL, NULL},
//	{"br", &term_cmd_br, NULL, NULL},
//	{"bw", &term_cmd_bw, NULL, NULL},
//	{"bf", &term_cmd_bf, NULL, NULL},
//	{"fs", &term_cmd_fs, NULL, NULL},
//	{"fn", &term_cmd_fn, NULL, NULL},
//	{"fv", &term_cmd_fv, NULL, NULL},
//	{"fa", &term_cmd_fa, NULL, NULL},
//	{"ft", &term_cmd_ft, NULL, NULL},
//	{"fm", &term_cmd_fm, NULL, NULL}
};

const int term_command_info_count = sizeof(term_commands) / sizeof(term_command_info_t);
