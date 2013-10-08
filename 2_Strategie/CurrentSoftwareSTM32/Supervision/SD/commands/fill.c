#include "fill.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_uart.h"
#include "../Libraries/fat_sd/ff.h"
#include <string.h>

const char term_cmd_fill_brief[] = "Rempli un fichier de 0 (pour tester la vitesse SPI)";
const char term_cmd_fill_help[] =
		"Utilisation : fill NOM TAILLE\n"
		"\n"
		"NOM        Nom du fichier à écrire\n"
		"TAILLE     Nombre de 0 à écrire\n";

int term_cmd_fill(int argc, const char *argv[]) {
	int res;
	long taille;
	FIL file_dst;
	time32_t start_time;
	UINT bytes_written, total_copied;

	if(argc < 2)
		return EINVAL;

	if(!argtolong(argv[1], 0, &taille))
		return EINVAL;

	if(taille <= 0)
		return EINVAL;

	debug_printf("Creating \"%s\"", argv[0]);
	res = fatfs_err_to_errno(f_open(&file_dst, argv[0], FA_CREATE_ALWAYS | FA_WRITE));
	debug_printf("\n");
	if (res)
		return res;

	memset(term_common_buffer, '0', term_common_buffer_size);

	debug_printf("Filling file...\n");
	start_time = global.env.absolute_time;
	total_copied = 0;

	for (;total_copied < taille;) {
		res = fatfs_err_to_errno(f_write(&file_dst, term_common_buffer, term_common_buffer_size, &bytes_written));
		total_copied += bytes_written;
		if (res || bytes_written < term_common_buffer_size) {
			debug_printf("disk full\n");
			break;
		}
	}
	debug_printf("%u bytes copied with %lu kB/sec.\n", total_copied, total_copied / (global.env.absolute_time - start_time));

	f_close(&file_dst);

	return res;
}
