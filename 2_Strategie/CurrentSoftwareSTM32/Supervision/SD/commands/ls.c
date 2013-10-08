#include "ls.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"

const char term_cmd_ls_brief[] = "Liste les fichiers d'un dossier";
const char term_cmd_ls_help[] =
		"Utilisation : ls [DOSSIER]\n"
		"\n"
		"DOSSIER    Dossier à lister\n";


int term_cmd_ls(int argc, const char *argv[]) {
	BYTE res;
	DIR dir;
	FILINFO file_info;
	long dir_count, file_count, total_size;
	FATFS *fs;
	const char *target;

	if(argc < 1)
		target = "";
	else
		target = argv[0];

	res = fatfs_err_to_errno(f_opendir(&dir, target));
	if (res)
		return res;

	for(;;) {
#if _USE_LFN
		file_info.lfname = (XCHAR*)term_common_buffer;
		file_info.lfsize = term_common_buffer_size;
#endif
		res = fatfs_err_to_errno(f_readdir(&dir, &file_info));
		if (res || !file_info.fname[0])
			break;

		if (file_info.fattrib & AM_DIR) {
			dir_count++;
		} else {
			file_count++;
			total_size += file_info.fsize;
		}
		debug_printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s",
				(file_info.fattrib & AM_DIR) ? 'D' : '-',
				(file_info.fattrib & AM_RDO) ? 'R' : '-',
				(file_info.fattrib & AM_HID) ? 'H' : '-',
				(file_info.fattrib & AM_SYS) ? 'S' : '-',
				(file_info.fattrib & AM_ARC) ? 'A' : '-',
				(file_info.fdate >> 9) + 1980, (file_info.fdate >> 5) & 15, file_info.fdate & 31,
				(file_info.ftime >> 11), (file_info.ftime >> 5) & 63,
				file_info.fsize, &(file_info.fname[0]));
#if _USE_LFN
		debug_printf("  %s\n", file_info.lfname);
#else
		UART1_putc('\n');
#endif
	}

	if(!res) {
		debug_printf("%4lu File(s), %10lu bytes total\n%4lu Dir(s)", file_count, total_size, dir_count);
		if (f_getfree(target, (DWORD*)&total_size, &fs) == FR_OK)
			debug_printf(", %10lu bytes free\n", total_size * fs->csize * 512);
		else
			debug_printf("\n");
	}

	return res;
}
