#include "mkdir.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../Libraries/fat_sd/ff.h"
#include <string.h>

const char term_cmd_mkdir_brief[] = "Cr�er des dossier";
const char term_cmd_mkdir_help[] =
		"Utilisation : mkdir DOSSIER...\n"
		"\n"
		"DOSSIER... Nom des dossiers � cr�er\n";

int term_cmd_mkdir(int argc, const char *argv[]) {
	int i, res;

	if(argc < 1)
		return EINVAL;

	for(i = 0; i < argc; i++) {
		res = fatfs_err_to_errno(f_mkdir(argv[i]));
		if(res == 0)
			debug_printf("Dossier %s cr��\n", argv[i]);
		else
			debug_printf("Impossible de cr�er le dossier %s: %s\n", argv[i], strerror(res));
	}

	return 0;
}
