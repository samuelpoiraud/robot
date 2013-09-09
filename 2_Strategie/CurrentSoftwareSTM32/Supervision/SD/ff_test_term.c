/*----------------------------------------------------------------------*/
/* FAT file system test terminal for FatFs R0.07a  (C)ChaN, 2008        */
/* modified by Martin Thomas 4/2009 for the STM32 example               */
/* updated and modified by Samuel Poiraud 9/2013						*/
/*----------------------------------------------------------------------*/

#include <string.h> /* memset et al*/
#include "../../QS/QS_all.h"
#include "../../QS/QS_uart.h"
#include "Libraries/fat_sd/integer.h"
#include "Libraries/fat_sd/diskio.h"
#include "Libraries/fat_sd/ff.h"
#include "term_io.h"
#include "ff_test_term.h"


DWORD acc_size;				/* Work register for fs command */
WORD acc_files, acc_dirs;
FILINFO Finfo;
#if _USE_LFN
char Lfname[512];
#endif



FATFS Fatfs[_DRIVES];		/* File system object for each logical drive */
FIL File1, File2;			/* File objects */
DIR Dir;					/* Directory object */
BYTE Buff[4*1024] __attribute__ ((aligned (4))) ;		/* Working buffer */

volatile UINT Timer;		/* Performance timer (1kHz increment) */

/* called by a timer interrupt-handler every 1ms */
void ff_test_term_timerproc (void)
{
	Timer++;
}

static FRESULT scan_files (char* path)
{
	DIR dirs;
	FRESULT res;
	BYTE i;
	char *fn;


	if ((res = f_opendir(&dirs, path)) == FR_OK) {
		i = strlen(path);
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
#if _USE_LFN
				fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
				fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR) {
				acc_dirs++;
				*(path+i) = '/'; strcpy(path+i+1, fn);
				res = scan_files(path);
				*(path+i) = '\0';
				if (res != FR_OK) break;
			} else {
//				debug_printf("%s/%s\n", path, fn);
				acc_files++;
				acc_size += Finfo.fsize;
			}
		}
	}

	return res;
}



static void put_rc (FRESULT rc)
{
	const char *p;
	static const char str[] =
		"OK\0" "NOT_READY\0" "NO_FILE\0" "FR_NO_PATH\0" "INVALID_NAME\0" "INVALID_DRIVE\0"
		"DENIED\0" "EXIST\0" "RW_ERROR\0" "WRITE_PROTECTED\0" "NOT_ENABLED\0"
		"NO_FILESYSTEM\0" "INVALID_OBJECT\0" "MKFS_ABORTED\0";
	FRESULT i;

	for (p = str, i = 0; i != rc && *p; i++) {
		while(*p++);
	}
	debug_printf("rc=%u FR_%s\n", (UINT)rc, p);
}


//Show some help content...
void print_help(void)
{
	printf("\nHelp content - Terminal for easy access to the SD card content.\n");
	printf("\n   ::  TERMINAL_COMMANDS  ::\n");
	printf("exit                     - exit the terminal\n");
	printf("help                     - show this command list\n");
	printf("\n   ::  DISK_COMMANDS  ::\n");
	printf("di                       - Initialize disk 0\n");
	printf("fi                       - Force initialized the logical drive\n");
	printf("ds                       - Show disk status\n");
	printf("\n   ::  FILE_COMMANDS  ::\n");
	printf("ls [<path>]              - Directory listing\n");
	printf("cat <path>               - print the file content\n");
	printf("cp <src_name> <dst_name> - Copy file\n");
	printf("rm <name>                - Delete a file or dir (=Unlink)\n");
	printf("\n   ::  MEMORY_COMMANDS  ::\n");
	printf("md <address> [<count>]   - Dump memory\n");
	printf("dd [<lba>]               - Dump sector\n");

}


bool_e execute_command(char * ptr)
{
	static char *ptr2;
	long p1, p2, p3;
	BYTE res, b1;
	WORD w1;
	UINT s1, s2, cnt, blen = sizeof(Buff);
	Uint32 i;
	DWORD ofs = 0, sect = 0;
	FATFS *fs;				/* Pointer to file system object */

	switch (*ptr++)
	{
		case 'm' :
			switch (*ptr++) {
			case 'd' :	/* md <address> [<count>] - Dump memory */
				if (!xatoi(&ptr, &p1)) break;
				if (!xatoi(&ptr, &p2)) p2 = 128;
				for (ptr=(char*)p1; p2 >= 16; ptr += 16, p2 -= 16)
					put_dump((BYTE*)ptr, (UINT)ptr, 16);
				if (p2) put_dump((BYTE*)ptr, (UINT)ptr, p2);
				break;
			default:
				debug_printf("Unknow command\n");
				break;
			}
			break;

		case 'c':
				if(*ptr == 'p')	/* cp <src_name> <dst_name> - Copy file */
				{
					ptr++;
					while (*ptr == ' ') ptr++;
					ptr2 = strchr(ptr, ' ');
					if (!ptr2) break;
					*ptr2++ = 0;
					while (*ptr2 == ' ') ptr2++;
					debug_printf("Opening \"%s\"", ptr);
					res = f_open(&File1, ptr, FA_OPEN_EXISTING | FA_READ);
					debug_printf("\n");
					if (res) {
						put_rc(res);
						break;
					}
					debug_printf("Creating \"%s\"", ptr2);
					res = f_open(&File2, ptr2, FA_CREATE_ALWAYS | FA_WRITE);
					debug_printf("\n");
					if (res) {
						put_rc(res);
						f_close(&File1);
						break;
					}
					debug_printf("Copying file...");
					Timer = 0;
					p1 = 0;
					for (;;) {
						res = f_read(&File1, Buff, blen, &s1);
						if (res || s1 == 0) {
							debug_printf("EOF\n");
							break;   /* error or eof */
						}
						res = f_write(&File2, Buff, s1, &s2);
						p1 += s2;
						if (res || s2 < s1) {
							debug_printf("disk full\n");
							break;   /* error or disk full */
						}
					}
					debug_printf("%lu bytes copied with %lu kB/sec.\n", p1, p1 / Timer);
					f_close(&File1);
					f_close(&File2);
				}
				else if(*ptr == 'a')
				{
					ptr++;
					if(*ptr++ == 't')		// cat <path> - print the file content
					{
						while (*ptr == ' ') ptr++;
						res = f_open(&File1, ptr, FA_OPEN_EXISTING | FA_READ);
						if (res)
						{
							put_rc(res);
							break;
						}

						debug_printf("Reading file...%s\n",ptr);
						Timer = 0;
						p1 = 0;
						while(1)
						{
							res = f_read(&File1, Buff, blen-1, &s1);
							if(s1 == 0)
							{
								debug_printf("\n...End of file %s\n",ptr);
								break;
							}
							if (res) {
								put_rc(res);
								break;   /* error or eof */
							}
							for(i=0;i<s1;i++)
								UART1_putc(Buff[i]);
						}
						f_close(&File1);
					}
				}
				break;

		case 'd' :
			switch (*ptr++) {
			case 'd' :	/* dd [<lba>] - Dump sector */
				if (!xatoi(&ptr, &p2)) p2 = sect;
				res = disk_read(0, Buff, p2, 1);
				if (res) { debug_printf("rc=%d\n", (WORD)res); break; }
				sect = p2 + 1;
				debug_printf("Sector:%lu\n", p2);
				for (ptr=(char*)Buff, ofs = 0; ofs < 0x200; ptr+=16, ofs+=16)
					put_dump((BYTE*)ptr, ofs, 16);
				break;

			case 'i' :	/* di - Initialize disk */
				debug_printf("rc=%d\n", (WORD)disk_initialize(0));
				break;

			case 's' :	/* ds - Show disk status */
				Buff[0]=2;
				if (disk_ioctl(0, CTRL_POWER, Buff) == RES_OK )
					{ debug_printf("Power is %s\n", Buff[1] ? "ON" : "OFF"); }
				if (disk_ioctl(0, GET_SECTOR_COUNT, &p2) == RES_OK)
					{ debug_printf("Drive size: %lu sectors\n", p2); }
				if (disk_ioctl(0, GET_SECTOR_SIZE, &w1) == RES_OK)
					{ debug_printf("Sector size: %u\n", w1); }
				if (disk_ioctl(0, GET_BLOCK_SIZE, &p2) == RES_OK)
					{ debug_printf("Erase block size: %lu sectors\n", p2); }
				if (disk_ioctl(0, MMC_GET_TYPE, &b1) == RES_OK)
					{ debug_printf("MMC/SDC type: %u\n", b1); }
				if (disk_ioctl(0, MMC_GET_CSD, Buff) == RES_OK)
					{ debug_printf("CSD:\n"); put_dump(Buff, 0, 16); }
				if (disk_ioctl(0, MMC_GET_CID, Buff) == RES_OK)
					{ debug_printf("CID:\n"); put_dump(Buff, 0, 16); }
				if (disk_ioctl(0, MMC_GET_OCR, Buff) == RES_OK)
					{ debug_printf("OCR:\n"); put_dump(Buff, 0, 4); }
				if (disk_ioctl(0, MMC_GET_SDSTAT, Buff) == RES_OK) {
					debug_printf("SD Status:\n");
					for (s1 = 0; s1 < 64; s1 += 16) put_dump(Buff+s1, s1, 16);
				}
				break;
			default:
				debug_printf("Unknow command\n");
				break;
			}
			break;

		case 'b' :
			switch (*ptr++) {
			case 'd' :	/* bd <addr> - Dump R/W buffer */
				if (!xatoi(&ptr, &p1)) break;
				for (ptr=(char*)&Buff[p1], ofs = p1, cnt = 32; cnt; cnt--, ptr+=16, ofs+=16)
					put_dump((BYTE*)ptr, ofs, 16);
				break;

			case 'e' :	/* be <addr> [<data>] ... - Edit R/W buffer */
				/*if (!xatoi(&ptr, &p1)) break;
				if (xatoi(&ptr, &p2)) {
					do {
						Buff[p1++] = (BYTE)p2;
					} while (xatoi(&ptr, &p2));
					break;
				}
				for (;;) {
					debug_printf("%04X %02X-", (WORD)(p1), (WORD)Buff[p1]);
					get_line(linebuf, sizeof(linebuf));
					ptr = linebuf;
					if (*ptr == '.') break;
					if (*ptr < ' ') { p1++; continue; }
					if (xatoi(&ptr, &p2))
						Buff[p1++] = (BYTE)p2;
					else
						debug_printf("???\n");
				}
				break;
				*/
				debug_printf("Desimplemented function\n");
				break;
			case 'r' :	/* br <lba> [<num>] - Read disk into R/W buffer */
				if (!xatoi(&ptr, &p2)) break;
				if (!xatoi(&ptr, &p3)) p3 = 1;
				debug_printf("rc=%u\n", (WORD)disk_read(0, Buff, p2, p3));
				break;

			case 'w' :	/* bw <lba> [<num>] - Write R/W buffer into disk */
				if (!xatoi(&ptr, &p2)) break;
				if (!xatoi(&ptr, &p3)) p3 = 1;
				debug_printf("rc=%u\n", (WORD)disk_write(0, Buff, p2, p3));
				break;

			case 'f' :	/* bf <val> - Fill working buffer */
				if (!xatoi(&ptr, &p1)) break;
				memset(Buff, (BYTE)p1, sizeof(Buff));
				break;
			default:
				debug_printf("Unknow command\n");
				break;
			}
			break;

		case 'f' :
			switch (*ptr++) {

			case 'i' :	/* fi - Force initialized the logical drive */
				put_rc(f_mount(0, &Fatfs[0]));
				break;

			case 's' :	/* fs - Show logical drive status */
				res = f_getfree("", (DWORD*)&p2, &fs);
				if (res) { put_rc(res); break; }
				debug_printf("FAT type = %u (%s)\nBytes/Cluster = %lu\nNumber of FATs = %u\n"
						"Root DIR entries = %u\nSectors/FAT = %lu\nNumber of clusters = %lu\n"
						"FAT start (lba) = %lu\nDIR start (lba,clustor) = %lu\nData start (lba) = %lu\n\n",
						(WORD)fs->fs_type,
						(fs->fs_type==FS_FAT12) ? "FAT12" : (fs->fs_type==FS_FAT16) ? "FAT16" : "FAT32",
						(DWORD)fs->csize * 512, (WORD)fs->n_fats,
						fs->n_rootdir, fs->sects_fat, (DWORD)fs->max_clust - 2,
						fs->fatbase, fs->dirbase, fs->database
				);
				acc_size = acc_files = acc_dirs = 0;
#if _USE_LFN
				Finfo.lfname = Lfname;
				Finfo.lfsize = sizeof(Lfname);
#endif
				res = scan_files(ptr);
				if (res) { put_rc(res); break; }
				debug_printf("%u files, %lu bytes.\n%u folders.\n"
						"%lu KB total disk space.\n%lu KB available.\n",
						acc_files, acc_size, acc_dirs,
						(fs->max_clust - 2) * (fs->csize / 2), p2 * (fs->csize / 2)
				);
				break;



			case 'o' :	/* fo <mode> <file> - Open a file */
				if (!xatoi(&ptr, &p1)) break;
				while (*ptr == ' ') ptr++;
				put_rc(f_open(&File1, ptr, (BYTE)p1));
				break;

			case 'c' :	/* fc - Close a file */
				put_rc(f_close(&File1));
				break;

			case 'e' :	/* fe - Seek file pointer */
				if (!xatoi(&ptr, &p1)) break;
				res = f_lseek(&File1, p1);
				put_rc(res);
				if (res == FR_OK)
					debug_printf("fptr=%lu(0x%lX)\n", File1.fptr, File1.fptr);
				break;

			case 'd' :	/* fd <len> - read and dump file from current fp */
				if (!xatoi(&ptr, &p1)) break;
				ofs = File1.fptr;
				while (p1) {
					if ((UINT)p1 >= 16) { cnt = 16; p1 -= 16; }
					else 				{ cnt = p1; p1 = 0; }
					res = f_read(&File1, Buff, cnt, &cnt);
					if (res != FR_OK) { put_rc(res); break; }
					if (!cnt) break;
					put_dump(Buff, ofs, cnt);
					ofs += 16;
				}
				break;

			case 'r' :	/* fr <len> - read file */
				if (!xatoi(&ptr, &p1)) break;
				p2 = 0;
				Timer = 0;
				while (p1) {
					if ((UINT)p1 >= blen) {
						cnt = blen; p1 -= blen;
					} else {
						cnt = p1; p1 = 0;
					}
					res = f_read(&File1, Buff, cnt, &s2);
					if (res != FR_OK) { put_rc(res); break; }
					p2 += s2;
					if (cnt != s2) break;
				}
				debug_printf("%lu bytes read with %lu kB/sec.\n", p2, p2 / Timer);
				break;

			case 'w' :	/* fw <len> <val> - write file */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
				memset(Buff, (BYTE)p2, blen);
				p2 = 0;
				Timer = 0;
				while (p1) {
					if ((UINT)p1 >= blen) {
						cnt = blen; p1 -= blen;
					} else {
						cnt = p1; p1 = 0;
					}
					res = f_write(&File1, Buff, cnt, &s2);
					if (res != FR_OK) { put_rc(res); break; }
					p2 += s2;
					if (cnt != s2) break;
				}
				debug_printf("%lu bytes written with %lu kB/sec.\n", p2, p2 / Timer);
				break;

			case 'n' :	/* fn <old_name> <new_name> - Change file/dir name */
				while (*ptr == ' ') ptr++;
				ptr2 = strchr(ptr, ' ');
				if (!ptr2) break;
				*ptr2++ = 0;
				while (*ptr2 == ' ') ptr2++;
				put_rc(f_rename(ptr, ptr2));
				break;



			case 'v' :	/* fv - Truncate file */
				put_rc(f_truncate(&File1));
				break;

			case 'k' :	/* fk <name> - Create a directory */
				while (*ptr == ' ') ptr++;
				put_rc(f_mkdir(ptr));
				break;

			case 'a' :	/* fa <atrr> <mask> <name> - Change file/dir attribute */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
				while (*ptr == ' ') ptr++;
				put_rc(f_chmod(ptr, p1, p2));
				break;

			case 't' :	/* ft <year> <month> <day> <hour> <min> <sec> <name> - Change timestamp */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				Finfo.fdate = ((p1 - 1980) << 9) | ((p2 & 15) << 5) | (p3 & 31);
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				Finfo.ftime = ((p1 & 31) << 11) | ((p1 & 63) << 5) | ((p1 >> 1) & 31);
				put_rc(f_utime(ptr, &Finfo));
				break;


#if _USE_MKFS
			case 'm' :	/* fm <partition rule> <cluster size> - Create file system */
				if (!xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				debug_printf("The drive 0 will be formatted. If you are really sure, use the command fmY instead of fm\n");
				if (*ptr == 'Y')
					put_rc(f_mkfs(0, (BYTE)p2, (WORD)p3));
				break;
#endif
			case 'z' :	/* fz [<rw size>] - Change R/W length for fr/fw/fx command */
				if (xatoi(&ptr, &p1) && p1 >= 1 && (size_t)p1 <= sizeof(Buff))
					blen = p1;
				debug_printf("blen=%u\n", blen);
				break;
			default:
				debug_printf("Unknow command\n");
				break;
			}
			break;

		case 'h':
			if(*ptr++ != 'e')
				break;
			if(*ptr++ != 'l')
				break;
			if(*ptr++ != 'p')
				break;
			print_help();
			break;
		case 'l' :	/* ls [<path>] - Directory listing */
			if(*ptr++ != 's')
				break;
			while (*ptr == ' ') ptr++;
			res = f_opendir(&Dir, ptr);
			if (res) { put_rc(res); break; }
			p1 = s1 = s2 = 0;
			for(;;) {
#if _USE_LFN
				Finfo.lfname = Lfname;
				Finfo.lfsize = sizeof(Lfname);
#endif
				res = f_readdir(&Dir, &Finfo);
				if ((res != FR_OK) || !Finfo.fname[0]) break;
				if (Finfo.fattrib & AM_DIR) {
					s2++;
				} else {
					s1++; p1 += Finfo.fsize;
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
#if _USE_LFN
				debug_printf("  %s\n", Lfname);
#else
				xputc('\n');
#endif
			}
			debug_printf("%4u File(s),%10lu bytes total\n%4u Dir(s)", s1, p1, s2);
			if (f_getfree(ptr, (DWORD*)&p1, &fs) == FR_OK)
				debug_printf(", %10lu bytes free\n", p1 * fs->csize * 512);
			break;

		case 't' :	/* t [<year> <mon> <mday> <hour> <min> <sec>] */
			/*if (xatoi(&ptr, &p1)) {
				rtc.year = (WORD)p1;
				xatoi(&ptr, &p1); rtc.month = (BYTE)p1;
				xatoi(&ptr, &p1); rtc.mday = (BYTE)p1;
				xatoi(&ptr, &p1); rtc.hour = (BYTE)p1;
				xatoi(&ptr, &p1); rtc.min = (BYTE)p1;
				if (!xatoi(&ptr, &p1)) break;
				rtc.sec = (BYTE)p1;
				rtc_settime(&rtc);
			}
			rtc_gettime(&rtc);
			debug_printf("%u/%u/%u %02u:%02u:%02u\n", rtc.year, rtc.month, rtc.mday, rtc.hour, rtc.min, rtc.sec);
			*/
			debug_printf("Rtc unimplemented\n");
			break;

		case 'p' : /* p [<0|1>] - Card Power state, Card Power Control off/on */
			/*if (xatoi(&ptr, &p1)) {
				if (!p1) {
					f_sync(&File1);
				}
				Buff[0] = p1;
				disk_ioctl(0, CTRL_POWER, Buff);
			}
			Buff[0] = 2;
			if (disk_ioctl(0, CTRL_POWER, Buff) == RES_OK ) {
				debug_printf("Card Power is %s\n", Buff[1] ? "ON" : "OFF");
			}
			*/
			debug_printf("Unknow command\n");
			break;
		case 'r':
			if(*ptr++ == 'm')	/* rm <name> - Delete a file or dir (=Unlink)*/
			{
				while (*ptr == ' ') ptr++;
				debug_printf("Trying to delete %s\n",ptr);
				res = f_unlink(ptr);
				if(res)
				{
					debug_printf("Can't delete %s : ",ptr);
					put_rc(res);
				}
				else
					debug_printf("%s deleted \n",ptr);
			}
			break;
		case 'e' :	//exit...
			if(*ptr++ != 'x')
				break;
			if(*ptr++ != 'i')
				break;
			if(*ptr++ != 't')
				break;
			return FALSE;
			break;
		default:
			debug_printf("Unknow command\n");
			break;
	}
	return TRUE;
}

//Return false si une sortie du terminal est demandée
bool_e terminal_process_main (void)
{
	char * ptr;
	bool_e ret;

	typedef enum
	{
		INIT = 0,
		SEND_PROMPT,
		WAIT_COMMAND,
		EXECUTE_COMMAND
	}state_e;
	static state_e state = INIT;

	ret = TRUE;	//On garde la main
	switch (state)
	{
		case INIT:
			debug_printf("FatFs module terminal :: type \"help\" for command list\n");
			state = SEND_PROMPT;
			break;
		case SEND_PROMPT:
			debug_printf(">");
			state = WAIT_COMMAND;
			break;
		case WAIT_COMMAND:
			ptr = get_command();
			if(ptr)
			{
				ret = execute_command(ptr);
				state = SEND_PROMPT;
			}
			break;
		default:
			state = INIT;
			break;
	}
	if(ret == FALSE)
		state = INIT;	//Une sortie du terminal est demandée...
	return ret;
}

