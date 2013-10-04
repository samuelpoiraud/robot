#include "term_commands.h"
#include "term_io.h"
#include "Libraries/fat_sd/ff.h"
#include "Libraries/fat_sd/diskio.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "../QS/QS_uart.h"
#include "../asser_functions.h"

#define TERM_DATA_BUFFER_SIZE 4096
static BYTE data_buffer[TERM_DATA_BUFFER_SIZE] __attribute__ ((aligned (4)));

//Convert error integer to string
static void put_rc(FRESULT error_id)
{
	static const char *error_strings[] = {
		"OK",
		"NOT_READY",
		"NO_FILE",
		"FR_NO_PATH",
		"INVALID_NAME",
		"INVALID_DRIVE",
		"DENIED",
		"EXIST",
		"RW_ERROR",
		"WRITE_PROTECTED",
		"NOT_ENABLED",
		"NO_FILESYSTEM",
		"INVALID_OBJECT",
		"MKFS_ABORTED",
		"TIMEOUT"
	};
	static const Uint8 errno_count = sizeof(error_strings) / sizeof(const char*);

	if(error_id >= errno_count)
		debug_printf("rc=%u UNKNOWN_ERROR\n", (UINT)error_id);
	else
		debug_printf("rc=%u FR_%s\n", (UINT)error_id, error_strings[error_id]);
}

static long argtolong(const char *str, int base, bool_e *ok) {
	char *ptr;

	if(!str) {
		if(ok)
			*ok = FALSE;
		return 0;
	}

	long val = strtol(str, &ptr, base);
	if(ok)
		*ok = ptr > str;

	return val;
}

static FRESULT scan_files (const char* path)
{
	DIR dirs;
	FRESULT res;
	char *fn;
	static char subdir[128];
	FILINFO Finfo;


	if ((res = f_opendir(&dirs, path)) == FR_OK) {
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
//#if _USE_LFN
//				fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
//#else
				fn = Finfo.fname;
//#endif
			if (Finfo.fattrib & AM_DIR) {
//				acc_dirs++;
				sprintf(subdir, "%s/%s", path, fn);
				res = scan_files(path);
				//attention, subdir invalide car static (mis en static pour pas remplir la stack)
				if (res != FR_OK) break;
			} else {
//				debug_printf("%s/%s\n", path, fn);
//				acc_files++;
//				acc_size += Finfo.fsize;
			}
		}
	}

	return res;
}

//Dump memory
int term_cmd_md(int argc, const char *argv[]) {
	long address, count;
	const BYTE* ptr;
	bool_e conv_ok;

	if(argc < 1)
		return EINVAL;

	address = argtolong(argv[0], 0, &conv_ok);
	if(!conv_ok)
		return EINVAL;

	if(argc < 2) {
		count = 128;
	} else {
		count = argtolong(argv[1], 0, &conv_ok);
		if(!conv_ok)
			count = 128;
	}

	for(ptr = (const BYTE*)address; count >= 16; ptr += 16, count -= 16)
		put_dump(ptr, (DWORD)ptr, 16);
	if(count)
		put_dump(ptr, (DWORD)ptr, count);

	return 0;
}

//Copy file from 0 to 1
int term_cmd_cp(int argc, const char *argv[]) {
	BYTE res;
	FIL file_src, file_dst;
	time32_t start_time;
	UINT bytes_read, bytes_written, total_copied;

	if(argc < 2)
		return EINVAL;

	debug_printf("Opening \"%s\"", argv[0]);
	res = f_open(&file_src, argv[0], FA_OPEN_EXISTING | FA_READ);
	debug_printf("\n");
	if (res) {
		put_rc(res);
		return ENOENT;
	}
	debug_printf("Creating \"%s\"", argv[1]);
	res = f_open(&file_dst, argv[1], FA_CREATE_ALWAYS | FA_WRITE);
	debug_printf("\n");
	if (res) {
		put_rc(res);
		f_close(&file_src);
		return EACCES;
	}

	debug_printf("Copying file...\n");
	start_time = global.env.absolute_time;
	total_copied = 0;

	for (;;) {
		res = f_read(&file_src, data_buffer, TERM_DATA_BUFFER_SIZE, &bytes_read);
		if (res || bytes_read == 0) {
			debug_printf("EOF\n");
			res = 0;
			break;
		}
		res = f_write(&file_dst, data_buffer, bytes_read, &bytes_written);
		total_copied += bytes_written;
		if (res || bytes_written < bytes_read) {
			debug_printf("disk full\n");
			res = ENOSPC;   // error no space
			break;
		}
	}
	debug_printf("%u bytes copied with %lu kB/sec.\n", total_copied, total_copied / (global.env.absolute_time - start_time));

	f_close(&file_dst);
	f_close(&file_src);

	return res;
}

//Put file content to output
int term_cmd_cat(int argc, const char *argv[]) {
	BYTE res;
	FIL file;
	UINT bytes_read, i;

	if(argc < 1)
		return EINVAL;

	res = f_open(&file, argv[0], FA_OPEN_EXISTING | FA_READ);
	if (res)
	{
		put_rc(res);
		return ENOENT;
	}

	debug_printf("Reading file...%s\n", argv[0]);
	while(1)
	{
		res = f_read(&file, data_buffer, TERM_DATA_BUFFER_SIZE, &bytes_read);
		if(bytes_read == 0)
		{
			debug_printf("\n...End of file %s\n", argv[0]);
			break;
		}
		if (res) {
			put_rc(res);
			break;   // error or eof
		}
		for(i=0;i<bytes_read;i++)
			UART1_putc(data_buffer[i]);
	}

	f_close(&file);

	return 0;
}

//Delete all match files
int term_cmd_clean(int argc, const char *argv[]) {
	BYTE res;

	if(argc < 1)
		return EINVAL;
	argv = argv; //anti warning argv non utilisé

	debug_printf("Cleaning all match files - Cela peut prendre plusieurs minutes\n");
	debug_printf("Si vous ne vouliez pas supprimer les matchs enregistrés, RESETTEZ VITE LA CARTE STRATEGIE !!!\n");
	debug_printf("Un point = 100 fichiers de match traités, commence au match 9999\n");

	Sint16 match;
	char path[12];
	for(match = 9999; match>=0; match--)
	{
		sprintf(path, "%04d.MCH", match);
		res = f_unlink(path);
		if(res == RES_OK)
		{
			debug_printf("\n%s deleted\n",path);
		}
		if(match % 100 == 0)
			debug_printf(".");
	}
	res = f_unlink("index.inf");
	if(res == RES_OK)
	{
		debug_printf("\nindex.inf deleted\n");
	}
	debug_printf("Vous pouvez maintenant reseter la carte Stratégie (si vous lancez un match maintenant, il ne sera pas enregistré).");

	return 0;
}

//Dump sector
int term_cmd_dd(int argc, const char *argv[]) {
	long sect;
	bool_e conv_ok;
	static long next_sector = 0;
	const BYTE* ptr;
	BYTE res;
	long ofs;

	if(argc < 1) {
		sect = next_sector;
	} else {
		sect = argtolong(argv[0], 0, &conv_ok);
		if(!conv_ok)
			sect = next_sector;
	}

	next_sector = sect + 1;

#if TERM_DATA_BUFFER_SIZE < 512
#error "terminal buffer must be > 512 bytes"
#endif

	res = disk_read(0, data_buffer, sect, 1);
	if (res) {
		debug_printf("rc=%d\n", (WORD)res);
		return EIO;
	}

	debug_printf("Sector: %lu\n", sect);

	for (ptr=(const BYTE*)data_buffer, ofs = 0; ofs < 0x200; ptr+=16, ofs+=16)
		put_dump(ptr, ofs, 16);

	return 0;
}

//Initialize disk
int term_cmd_di(int argc, const char *argv[]) {
	argc = argc;
	argv = argv;

	debug_printf("rc=%d\n", (WORD)disk_initialize(0));

	return 0;
}

//Show disk status
int term_cmd_ds(int argc, const char *argv[]) {
	data_buffer[0] = 2;
	long data;

	argc = argc;
	argv = argv;

	if (disk_ioctl(0, CTRL_POWER, data_buffer) == RES_OK )
		debug_printf("Power is %s\n", data_buffer[1] ? "ON" : "OFF");

	data = 0;
	if (disk_ioctl(0, GET_SECTOR_COUNT, &data) == RES_OK)
		debug_printf("Drive size: %lu sectors\n", data);

	data = 0;
	if (disk_ioctl(0, GET_SECTOR_SIZE, &data) == RES_OK)
		debug_printf("Sector size: %lu\n", data);

	data = 0;
	if (disk_ioctl(0, GET_BLOCK_SIZE, &data) == RES_OK)
		debug_printf("Erase block size: %lu sectors\n", data);

	data = 0;
	if (disk_ioctl(0, MMC_GET_TYPE, &data) == RES_OK)
		debug_printf("MMC/SDC type: %lu\n", data);

	if (disk_ioctl(0, MMC_GET_CSD, data_buffer) == RES_OK){
		debug_printf("CSD:\n");
		put_dump(data_buffer, 0, 16);
	}

	if (disk_ioctl(0, MMC_GET_CID, data_buffer) == RES_OK){
		debug_printf("CID:\n");
		put_dump(data_buffer, 0, 16);
	}

	if (disk_ioctl(0, MMC_GET_OCR, data_buffer) == RES_OK){
		debug_printf("OCR:\n");
		put_dump(data_buffer, 0, 4);
	}

	if (disk_ioctl(0, MMC_GET_SDSTAT, data_buffer) == RES_OK) {
		debug_printf("SD Status:\n");
		for (data = 0; data < 64; data += 16)
			put_dump(data_buffer+data, data, 16);
	}

	return 0;
}

//Dump buffer
int term_cmd_bd(int argc, const char *argv[]) {
	static long next_address = 0;
	long address, count;
	bool_e conv_ok;

	if(argc < 1) {
		address = next_address;
	} else {
		address = argtolong(argv[0], 0, &conv_ok);
		if(!conv_ok)
			return EINVAL;
	}

	next_address = address + 1;

	if(argc < 2) {
		count = 32;
	} else {
		count = argtolong(argv[1], 0, &conv_ok);
		if(!conv_ok)
			return EINVAL;
	}

	for (; count; count--, address += 16)
		put_dump(&data_buffer[address], address, 16);

	return 0;
}

//Read to buffer
int term_cmd_br(int argc, const char *argv[]) {
	static long next_sector = 0;
	long sector, count;
	bool_e conv_ok;

	if(argc < 1) {
		sector = next_sector;
	} else {
		sector = argtolong(argv[0], 0, &conv_ok);
		if(!conv_ok)
			return EINVAL;
	}

	next_sector = sector + 1;

	if(argc < 2) {
		count = 1;
	} else {
		count = argtolong(argv[1], 0, &conv_ok);
		if(!conv_ok)
			return EINVAL;
	}

	if(count > sector*512/TERM_DATA_BUFFER_SIZE)
		count = sector*512/TERM_DATA_BUFFER_SIZE;

	debug_printf("rc=%u\n", (WORD)disk_read(0, data_buffer, sector, count));

	return 0;
}

//Write buffer
int term_cmd_bw(int argc, const char *argv[]) {
	long sector, count;
	bool_e conv_ok;

	if(argc < 1)
		return EINVAL;

	sector = argtolong(argv[0], 0, &conv_ok);
	if(!conv_ok)
		return EINVAL;

	if(argc < 2) {
		count = 1;
	} else {
		count = argtolong(argv[1], 0, &conv_ok);
		if(!conv_ok)
			return EINVAL;
	}

	if(count > sector*512/TERM_DATA_BUFFER_SIZE)
		count = sector*512/TERM_DATA_BUFFER_SIZE;

	debug_printf("rc=%u\n", (WORD)disk_write(0, data_buffer, sector, count));

	return 0;
}

//Fill buffer
int term_cmd_bf(int argc, const char *argv[]) {
	long value;
	bool_e conv_ok;

	if(argc < 1)
		return EINVAL;

	value = argtolong(argv[0], 0, &conv_ok);
	if(!conv_ok)
		return EINVAL;

	memset(data_buffer, (BYTE)value, sizeof(TERM_DATA_BUFFER_SIZE));

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
		put_rc(res);
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

	res = scan_files(argv[0]);
	if (res) {
		put_rc(res);
		return EIO;
	}
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

	put_rc(f_rename(argv[0], argv[1]));

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
		put_rc(res);
		return ENOENT;
	}

	put_rc(f_truncate(&file));

	f_close(&file);

	return 0;
}

//mkdir
int term_cmd_fk(int argc, const char *argv[]) {
	if(argc < 1)
		return EINVAL;

	put_rc(f_mkdir(argv[0]));

	return 0;
}

//chmod
int term_cmd_fa(int argc, const char *argv[]) {
	long value, mask;
	bool_e conv_ok;

	if(argc < 3)
		return EINVAL;


	mask = argtolong(argv[0], 0, &conv_ok);
	if(!conv_ok)
		return EINVAL;

	value = argtolong(argv[1], 0, &conv_ok);
	if(!conv_ok)
		return EINVAL;

	put_rc(f_chmod(argv[2], value, mask));

	return 0;
}

//Change timestamp
int term_cmd_ft(int argc, const char *argv[]) {
//	if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
//	Finfo.fdate = ((p1 - 1980) << 9) | ((p2 & 15) << 5) | (p3 & 31);
//	if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
//	Finfo.ftime = ((p1 & 31) << 11) | ((p1 & 63) << 5) | ((p1 >> 1) & 31);
//	put_rc(f_utime(ptr, &Finfo));

	return 0;
}

//format drive
int term_cmd_fm(int argc, const char *argv[]) {
//	if (!xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
//	debug_printf("The drive 0 will be formatted. If you are really sure, use the command fmY instead of fm\n");
//	if (*ptr == 'Y')
//		put_rc(f_mkfs(0, (BYTE)p2, (WORD)p3));

	return 0;
}

//Move robot to X, Y position
int term_cmd_goto(int argc, const char *argv[]) {
	long x, y;
	bool_e conv_ok;

	if(argc < 2)
		return EINVAL;

	x = argtolong(argv[0], 0, &conv_ok);
	if(!conv_ok)
		return EINVAL;

	y = argtolong(argv[1], 0, &conv_ok);
	if(!conv_ok)
		return EINVAL;


	if(x >= 200 && x <= 1800 && y >= 200 && y <= 2800)
		ASSER_push_goto(x, y, FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);
	else
		return EINVAL;

	return 0;
}

//list files
int term_cmd_ls(int argc, const char *argv[]) {
	BYTE res;
	DIR dir;
	FILINFO Finfo;
	long dir_count, file_count, total_size;
	FATFS *fs;
	const char *target;

	if(argc < 1)
		target = "";
	else
		target = argv[0];

	res = f_opendir(&dir, target);
	if (res) {
		put_rc(res);
		return ENOENT;
	}

	for(;;) {
//#if _USE_LFN
//		Finfo.lfname = Lfname;
//		Finfo.lfsize = sizeof(Lfname);
//#endif
		res = f_readdir(&dir, &Finfo);
		if ((res != FR_OK) || !Finfo.fname[0]) break;
		if (Finfo.fattrib & AM_DIR) {
			dir_count++;
		} else {
			file_count++;
			total_size += Finfo.fsize;
		}
		debug_printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s",
				(Finfo.fattrib & AM_DIR) ? 'D' : '-',
				(Finfo.fattrib & AM_RDO) ? 'R' : '-',
				(Finfo.fattrib & AM_HID) ? 'H' : '-',
				(Finfo.fattrib & AM_SYS) ? 'S' : '-',
				(Finfo.fattrib & AM_ARC) ? 'A' : '-',
				(Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
				(Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
				Finfo.fsize, &(Finfo.fname[0]));
//#if _USE_LFN
//		debug_printf("  %s\n", Lfname);
//#else
		UART1_putc('\n');
//#endif
	}
	debug_printf("%4lu File(s), %10lu bytes total\n%4lu Dir(s)", file_count, total_size, dir_count);
	if (f_getfree(target, (DWORD*)&total_size, &fs) == FR_OK)
		debug_printf(", %10lu bytes free\n", total_size * fs->csize * 512);
	else
		debug_printf("\n");

	return 0;
}

//remove file
int term_cmd_rm(int argc, const char *argv[]) {
	BYTE res;

	if(argc < 1)
		return EINVAL;

	debug_printf("Trying to delete %s\n", argv[0]);
	res = f_unlink(argv[0]);
	if(res)
	{
		debug_printf("Can't delete %s : ", argv[0]);
		put_rc(res);
		return ENOENT;
	}
	else
		debug_printf("%s deleted \n", argv[0]);

	return 0;
}

//Software reset
int term_cmd_reset(int argc, const char *argv[]) {
	NVIC_SystemReset();

	return 0;
}

