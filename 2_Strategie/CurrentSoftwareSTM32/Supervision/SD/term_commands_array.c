#include "term_commands_array.h"
#include "term_commands.h"

term_command_info_t term_commands[] = {
	{"md", &term_cmd_md},
	{"cp", &term_cmd_cp},
	{"cat", &term_cmd_cat},
	{"clean", &term_cmd_clean},
	{"dd", &term_cmd_dd},
	{"di", &term_cmd_di},
	{"ds", &term_cmd_ds},
	{"bd", &term_cmd_bd},
	{"br", &term_cmd_br},
	{"bw", &term_cmd_bw},
	{"bf", &term_cmd_bf},
	{"fs", &term_cmd_fs},
	{"fn", &term_cmd_fn},
	{"fv", &term_cmd_fv},
	{"fk", &term_cmd_fk},
	{"fa", &term_cmd_fa},
	{"ft", &term_cmd_ft},
	{"fm", &term_cmd_fm},
	{"goto", &term_cmd_goto},
	{"ls", &term_cmd_ls},
	{"rm", &term_cmd_rm},
	{"reset", &term_cmd_reset}
};

int term_command_info_count() {
	return sizeof(term_commands) / sizeof(term_command_info_t);
}
