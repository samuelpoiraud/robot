#include "cat.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"

const char term_cmd_cat_brief[] = "Affiche un fichier";
const char term_cmd_cat_help[] =
		"Utilisation : cat FICHIER\n"
		"\n"
		"FICHIER    Fichier à lire\n";

int term_cmd_cat(int argc, const char *argv[]) {
	BYTE res;
	FIL file;
	UINT bytes_read, i;

	if(argc < 1)
		return EINVAL;

	res = fatfs_err_to_errno(f_open(&file, argv[0], FA_OPEN_EXISTING | FA_READ));
	if (res)
		return res;

	debug_printf("Reading file...%s\n", argv[0]);
	while(1)
	{
		res = fatfs_err_to_errno(f_read(&file, term_common_buffer, term_common_buffer_size, &bytes_read));
		if(bytes_read == 0) {
			debug_printf("\n...End of file %s\n", argv[0]);
			break;
		}
		if (res) {
			break;   // error or eof
		}
		for(i=0; i<bytes_read; i++)
			putchar(term_common_buffer[i]);
	}

	f_close(&file);

	return res;
}
