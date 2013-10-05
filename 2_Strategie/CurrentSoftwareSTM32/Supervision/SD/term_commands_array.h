#ifndef TERM_COMMANDS_ARRAY_H
#define TERM_COMMANDS_ARRAY_H

//Return errno value
typedef int (*term_command_fcn_ptr)(int argc, const char *argv[]);

typedef struct {
	const char *name;
	term_command_fcn_ptr execute;
	const char *brief_description;
	const char *full_help;
} term_command_info_t;

extern term_command_info_t term_commands[];
extern const int term_command_info_count;

#endif // TERM_COMMANDS_ARRAY_H
