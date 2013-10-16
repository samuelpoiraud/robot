#include "term_commands_utils.h"
#include "term_io.h"
#include "Libraries/fat_sd/ff.h"
#include "Libraries/fat_sd/diskio.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "../QS/QS_uart.h"
#include "../../QS/QS_outputlog.h"

#define TERM_DATA_BUFFER_SIZE 4096
unsigned char term_common_buffer[TERM_DATA_BUFFER_SIZE] __attribute__ ((aligned (4)));
const int term_common_buffer_size = TERM_DATA_BUFFER_SIZE;

#if _USE_LFN
static char lfn_buffer[_MAX_LFN + 1];   /* Buffer to store the LFN (long file name) */
#endif

//Convert fatfs errors to errno standard values
int fatfs_err_to_errno(FRESULT err) {
	switch(err) {
		case FR_OK: return 0;
		case FR_DISK_ERR: return EIO;
		case FR_INT_ERR: return EFAULT;
		case FR_NOT_READY: return EBUSY;
		case FR_NO_FILE: return ENOENT;
		case FR_NO_PATH: return ENOENT;
		case FR_INVALID_NAME: return EINVAL;
		case FR_DENIED: return EACCES;
		case FR_EXIST: return EEXIST;
		case FR_INVALID_OBJECT: return EBADF;
		case FR_WRITE_PROTECTED: return EROFS;
		case FR_INVALID_DRIVE: return ENODEV;
		case FR_NOT_ENABLED: return EBADF;
		case FR_NO_FILESYSTEM: return ENODEV;
		case FR_MKFS_ABORTED: return ECANCELED;
		case FR_TIMEOUT: return ETIME;
	}

	return -1;
}

//Convert disk errors to errno standard values
int disk_err_to_errno(DRESULT err) {
	switch(err) {
		case RES_OK: return 0;
		case RES_ERROR: return EIO;
		case RES_NOTRDY: return EBUSY;
		case RES_PARERR: return EINVAL;
		case RES_WRPRT: return EROFS;
	}

	return -1;
}

bool_e argtolong(const char *str, int base, long *value) {
	char *ptr;

	if(!str)
		return FALSE;

	long val = strtol(str, &ptr, base);
	if(value && ptr > str)
		*value = val;

	return ptr > str;
}

//Get file & dir count and total used size by files
FRESULT scan_files (const char* path, long *total_dirs, long *total_files, long *total_size)
{
	DIR dir;
	FRESULT res;
	char *fn;
	static char subdir[128];
	FILINFO file_info;

#if _USE_LFN
	file_info.lfname = lfn_buffer;
	file_info.lfsize = sizeof(lfn_buffer);
#endif


	if ((res = f_opendir(&dir, path)) == FR_OK) {
		while (((res = f_readdir(&dir, &file_info)) == FR_OK) && file_info.fname[0]) {
#if _USE_LFN
			fn = *file_info.lfname ? file_info.lfname : file_info.fname;
#else
			fn = file_info.fname;
#endif
			if (file_info.fattrib & AM_DIR) {
				if(total_dirs)
					(*total_dirs)++;

				sprintf(subdir, "%s/%s", path, fn);
				res = scan_files(subdir, total_dirs, total_files, total_size);
				//attention, subdir invalide car static (mis en static pour pas remplir la stack)
				if (res != FR_OK) break;
			} else {
				//				debug_printf("%s/%s\n", path, fn);
				if(total_files)
					(*total_files)++;
				if(total_size)
					*total_size += file_info.fsize;
			}
		}
	}

	return res;
}

//Dump memory
int term_cmd_md(int argc, const char *argv[]) {
	long address, count;
	const BYTE* ptr;

	if(argc < 1)
		return EINVAL;

	if(!argtolong(argv[0], 0, &address))
		return EINVAL;

	if(argc < 2)
		count = 128;
	else if(!argtolong(argv[1], 0, &count))
		count = 128;

	for(ptr = (const BYTE*)address; count >= 16; ptr += 16, count -= 16)
		put_dump(ptr, (DWORD)ptr, 16);
	if(count)
		put_dump(ptr, (DWORD)ptr, count);

	return 0;
}

//Dump buffer
int term_cmd_bd(int argc, const char *argv[]) {
	static long next_address = 0;
	long address, count;

	if(argc < 1)
		address = next_address;
	else if(!argtolong(argv[0], 0, &address))
		return EINVAL;

	next_address = address + 1;

	if(argc < 2)
		count = 32;
	else if(!argtolong(argv[1], 0, &count))
		return EINVAL;

	for (; count; count--, address += 16)
		put_dump(&term_common_buffer[address], address, 16);

	return 0;
}

//Read to buffer
int term_cmd_br(int argc, const char *argv[]) {
	static long next_sector = 0;
	long sector, count;

	if(argc < 1)
		sector = next_sector;
	else if(!argtolong(argv[0], 0, &sector))
		return EINVAL;

	next_sector = sector + 1;

	if(argc < 2)
		count = 1;
	else if(!argtolong(argv[1], 0, &count))
		return EINVAL;

	if(count > sector*512/TERM_DATA_BUFFER_SIZE)
		count = sector*512/TERM_DATA_BUFFER_SIZE;

	debug_printf("rc=%u\n", (WORD)disk_read(0, term_common_buffer, sector, count));

	return 0;
}

//Write buffer
int term_cmd_bw(int argc, const char *argv[]) {
	long sector, count;

	if(argc < 1)
		return EINVAL;

	if(!argtolong(argv[0], 0, &sector))
		return EINVAL;

	if(argc < 2) {
		count = 1;
	} else if(!argtolong(argv[1], 0, &count))
		return EINVAL;

	if(count > sector*512/TERM_DATA_BUFFER_SIZE)
		count = sector*512/TERM_DATA_BUFFER_SIZE;

	debug_printf("rc=%u\n", (WORD)disk_write(0, term_common_buffer, sector, count));

	return 0;
}

//Fill buffer
int term_cmd_bf(int argc, const char *argv[]) {
	long value;

	if(argc < 1)
		return EINVAL;

	if(!argtolong(argv[0], 0, &value))
		return EINVAL;

	memset(term_common_buffer, (BYTE)value, sizeof(TERM_DATA_BUFFER_SIZE));

	return 0;
}

//Show logical drive status
int term_cmd_fs(int argc, const char *argv[]) {
	long free_space;
	FATFS *fs;
	BYTE res;
	//	FILINFO Finfo;

	if(argc < 1)
		return EINVAL;

	res = f_getfree("", (DWORD*)&free_space, &fs);
	if (res) {
		return EIO;
	}

	debug_printf("FAT type = %u (%s)\n"
				 "Bytes/Cluster = %lu\n"
				 "Number of FATs = %u\n"
				 "Root DIR entries = %u\n"
				 "Sectors/FAT = %lu\n"
				 "Number of clusters = %lu\n"
				 "FAT start (lba) = %lu\n"
				 "DIR start (lba,clustor) = %lu\n"
				 "Data start (lba) = %lu\n\n",
				 (WORD)fs->fs_type,
				 (fs->fs_type==FS_FAT12) ? "FAT12" : (fs->fs_type==FS_FAT16) ? "FAT16" : "FAT32",
				 (DWORD)fs->csize * 512,
				 (WORD)fs->n_fats,
				 fs->n_rootdir,
				 fs->sects_fat,
				 (DWORD)fs->max_clust - 2,
				 fs->fatbase,
				 fs->dirbase,
				 fs->database);

	//	acc_size = acc_files = acc_dirs = 0;

	//#if _USE_LFN
	//	Finfo.lfname = Lfname;
	//	Finfo.lfsize = sizeof(Lfname);
	//#endif

	//	res = scan_files(argv[0]);
	//	if (res) {
	//		return EIO;
	//	}
	//	debug_printf("%u files, %lu bytes.\n"
	//				 "%u folders.\n"
	//				 "%lu KB total disk space.\n"
	//				 "%lu KB available.\n",
	//				 acc_files,
	//				 acc_size,
	//				 acc_dirs,
	//				 (fs->max_clust - 2) * (fs->csize / 2),
	//				 p2 * (fs->csize / 2) );

	return 0;
}

//Rename file/dir
int term_cmd_fn(int argc, const char *argv[]) {
	if(argc < 2)
		return EINVAL;

	f_rename(argv[0], argv[1]);

	return 0;
}

//Truncate file
int term_cmd_fv(int argc, const char *argv[]) {
	BYTE res;
	FIL file;

	if(argc < 1)
		return EINVAL;

	res = f_open(&file, argv[0], FA_OPEN_ALWAYS | FA_WRITE);
	if (res)
	{
		return ENOENT;
	}

	f_truncate(&file);

	f_close(&file);

	return 0;
}

//chmod
int term_cmd_fa(int argc, const char *argv[]) {
	long value, mask;

	if(argc < 3)
		return EINVAL;

	if(!argtolong(argv[0], 0, &mask))
		return EINVAL;

	if(!argtolong(argv[1], 0, &value))
		return EINVAL;

	f_chmod(argv[2], value, mask);

	return 0;
}

//Change timestamp
int term_cmd_ft(int argc, const char *argv[]) {
	//	if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
	//	Finfo.fdate = ((p1 - 1980) << 9) | ((p2 & 15) << 5) | (p3 & 31);
	//	if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
	//	Finfo.ftime = ((p1 & 31) << 11) | ((p1 & 63) << 5) | ((p1 >> 1) & 31);

	return 0;
}

//format drive
int term_cmd_fm(int argc, const char *argv[]) {
	//	if (!xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
	//	debug_printf("The drive 0 will be formatted. If you are really sure, use the command fmY instead of fm\n");
	//	if (*ptr == 'Y')
	//		f_mkfs(0, (BYTE)p2, (WORD)p3);

	return 0;
}
