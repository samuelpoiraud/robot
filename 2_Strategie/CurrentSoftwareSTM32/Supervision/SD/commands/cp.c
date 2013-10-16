#include "cp.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"
#include "../../../QS/QS_outputlog.h"

const char term_cmd_cp_brief[] = "Copie un fichier";
const char term_cmd_cp_help[] =
		"Utilisation : cp SOURCE DEST\n"
		"\n"
		"SOURCE    Fichier source à copier\n"
		"DEST      Destination du fichier copié\n";

int term_cmd_cp(int argc, const char *argv[]) {
	int res;
	FIL file_src, file_dst;
	time32_t start_time;
	UINT bytes_read, bytes_written, total_copied;

	if(argc < 2)
		return EINVAL;

	debug_printf("Opening \"%s\"", argv[0]);
	res = fatfs_err_to_errno(f_open(&file_src, argv[0], FA_OPEN_EXISTING | FA_READ));
	debug_printf("\n");
	if (res)
		return res;

	debug_printf("Creating \"%s\"", argv[1]);
	res = fatfs_err_to_errno(f_open(&file_dst, argv[1], FA_CREATE_ALWAYS | FA_WRITE));
	debug_printf("\n");
	if (res) {
		f_close(&file_src);
		return res;
	}

	debug_printf("Copying file...\n");
	start_time = global.env.absolute_time;
	total_copied = 0;

	for (;;) {
		res = fatfs_err_to_errno(f_read(&file_src, term_common_buffer, term_common_buffer_size, &bytes_read));
		if (res || bytes_read == 0) {
			debug_printf("EOF\n");
			break;
		}
		res = fatfs_err_to_errno(f_write(&file_dst, term_common_buffer, bytes_read, &bytes_written));
		total_copied += bytes_written;
		if (res || bytes_written < bytes_read) {
			debug_printf("disk full\n");
			break;
		}
	}
	debug_printf("%u bytes copied with %lu kB/sec.\n", total_copied, total_copied / (global.env.absolute_time - start_time));

	f_close(&file_dst);
	f_close(&file_src);

	return res;
}
