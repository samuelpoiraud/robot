#ifndef TERM_COMMANDS_H
#define TERM_COMMANDS_H

typedef int (*term_command_fcn_ptr)(const char *parameters);

typedef struct {
	const char *name;
	term_command_fcn_ptr execute;
} term_command_info_t;
/*
int term_cmd_md(const char *parameters);
int term_cmd_cp(const char *parameters);
int term_cmd_cat(const char *parameters);
int term_cmd_clean(const char *parameters);
int term_cmd_dd(const char *parameters);
int term_cmd_di(const char *parameters);
int term_cmd_ds(const char *parameters);
int term_cmd_bd(const char *parameters);
int term_cmd_br(const char *parameters);
int term_cmd_bw(const char *parameters);
int term_cmd_bf(const char *parameters);
int term_cmd_fi(const char *parameters);
int term_cmd_fs(const char *parameters);
int term_cmd_fo(const char *parameters);
int term_cmd_fc(const char *parameters);
int term_cmd_fe(const char *parameters);
int term_cmd_fd(const char *parameters);
int term_cmd_fr(const char *parameters);
int term_cmd_fw(const char *parameters);
int term_cmd_fn(const char *parameters);
int term_cmd_fv(const char *parameters);
int term_cmd_fk(const char *parameters);
int term_cmd_fa(const char *parameters);
int term_cmd_ft(const char *parameters);
int term_cmd_fm(const char *parameters);
int term_cmd_fz(const char *parameters);
int term_cmd_goto(const char *parameters);
int term_cmd_ls(const char *parameters);
int term_cmd_rm(const char *parameters);
int term_cmd_reset(const char *parameters);

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
	{"fi", &term_cmd_fi},
	{"fs", &term_cmd_fs},
	{"fo", &term_cmd_fo},
	{"fc", &term_cmd_fc},
	{"fe", &term_cmd_fe},
	{"fd", &term_cmd_fd},
	{"fr", &term_cmd_fr},
	{"fw", &term_cmd_fw},
	{"fn", &term_cmd_fn},
	{"fv", &term_cmd_fv},
	{"fk", &term_cmd_fk},
	{"fa", &term_cmd_fa},
	{"ft", &term_cmd_ft},
	{"fm", &term_cmd_fm},
	{"fz", &term_cmd_fz},
	{"goto", &term_cmd_goto},
	{"ls", &term_cmd_ls},
	{"rm", &term_cmd_rm},
	{"reset", &term_cmd_reset}
};
*/

#endif // TERM_COMMANDS_H
