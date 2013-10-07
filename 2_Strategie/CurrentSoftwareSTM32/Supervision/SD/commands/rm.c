#include "rm.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../Libraries/fat_sd/ff.h"
#include "../../../QS/QS_uart.h"
#include <string.h>

const char term_cmd_rm_brief[] = "Supprime des fichiers";
const char term_cmd_rm_help[] =
		"Utilisation : rm FICHIER...\n"
		"\n"
		"FICHIER... Fichiers à supprimer séparé par un espace\n";

int term_cmd_rm(int argc, const char *argv[]) {
	int i;
	BYTE res;

	if(argc < 1)
		return EINVAL;

	for(i=0; i < argc; i++) {
		res = fatfs_err_to_errno(f_unlink(argv[i]));
		if(res)
			debug_printf("Impossible de supprimer %s : %s\n", argv[i], strerror(res));
		else
			debug_printf("%s supprimé\n", argv[i]);
	}

	return 0;
}
