#include "term_commands.h"
#include "term_io.h"
#include "Libraries/fat_sd/ff.h"
#include "Libraries/fat_sd/diskio.h"
#include <string.h>

volatile UINT Timer;
BYTE Buff[4*1024] __attribute__ ((aligned (4)));
/*
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

int term_cmd_md(const char *parameters) {
	const char* ptr = parameters;
	long p1, p2;

	if (!xatoi(&ptr, &p1)) return 0;
	if (!xatoi(&ptr, &p2)) p2 = 128;
	for (ptr=(char*)p1; p2 >= 16; ptr += 16, p2 -= 16)
		put_dump((BYTE*)ptr, (UINT)ptr, 16);
	if (p2) put_dump((BYTE*)ptr, (UINT)ptr, p2);

	return 0;
}

int term_cmd_cp(const char *parameters) {
	const char* ptr = parameters;
	char *ptr2;
	long p1;
	BYTE res;
	FIL File1, File2;
	UINT s1, s2, blen = sizeof(Buff);

	ptr++;
	while (*ptr == ' ') ptr++;
	ptr2 = strchr(ptr, ' ');
	if (!ptr2) return 0;
	*ptr2++ = 0;
	while (*ptr2 == ' ') ptr2++;
	debug_printf("Opening \"%s\"", ptr);
	res = f_open(&File1, ptr, FA_OPEN_EXISTING | FA_READ);
	debug_printf("\n");
	if (res) {
		put_rc(res);
		return 0;
	}
	debug_printf("Creating \"%s\"", ptr2);
	res = f_open(&File2, ptr2, FA_CREATE_ALWAYS | FA_WRITE);
	debug_printf("\n");
	if (res) {
		put_rc(res);
		f_close(&File1);
		return 0;
	}
	debug_printf("Copying file...");
	Timer = 0;
	p1 = 0;
	for (;;) {
		res = f_read(&File1, Buff, blen, &s1);
		if (res || s1 == 0) {
			debug_printf("EOF\n");
			return 0;   // error or eof
		}
		res = f_write(&File2, Buff, s1, &s2);
		p1 += s2;
		if (res || s2 < s1) {
			debug_printf("disk full\n");
			return 0;   // error or disk full
		}
	}
	debug_printf("%lu bytes copied with %lu kB/sec.\n", p1, p1 / Timer);
	f_close(&File1);
	f_close(&File2);

	return 0;
}

int term_cmd_cat(const char *parameters) {
	const char* ptr = parameters;
	long p1;
	BYTE res;
	FIL File1;
	UINT s1, blen = sizeof(Buff);

	while (*ptr == ' ') ptr++;
	res = f_open(&File1, ptr, FA_OPEN_EXISTING | FA_READ);
	if (res)
	{
		put_rc(res);
		return 0;
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
			return 0;
		}
		if (res) {
			put_rc(res);
			return 0;   // error or eof
		}
		fwrite(Buff, 1, s1, stdout);
//		for(i=0;i<s1;i++)
//			UART1_putc(Buff[i]);
	}
	f_close(&File1);
}

int term_cmd_clean(const char *parameters) {
	BYTE res;

	debug_printf("Cleaning all match files - Cela peut prendre plusieurs minutes\n");
	debug_printf("Si vous ne vouliez pas supprimer les matchs enregistrés, RESETTEZ VITE LA CARTE STRATEGIE !!!\n");

	Sint16 match;
	char path[12];
	for(match = 9999; match>=0; match--)
	{
		sprintf(path, "%04d.MCH", match);
		res = f_unlink(path);
		if(res == RES_OK)
		{
			debug_printf("\n%s deleted",path);
		}
		if(match == (match/100)*100)
			debug_printf(".");
	}
	res = f_unlink("index.inf");
	if(res == RES_OK)
	{
		debug_printf("\nindex.inf deleted\n");
	}
	debug_printf("Vous pouvez maintenant reseter la carte Stratégie (si vous lancez un match maintenant, il ne sera pas enregistré).");
}

int term_cmd_dd(const char *parameters) {
	if (!xatoi(&ptr, &p2)) p2 = sect;
	res = disk_read(0, Buff, p2, 1);
	if (res) { debug_printf("rc=%d\n", (WORD)res); break; }
	sect = p2 + 1;
	debug_printf("Sector:%lu\n", p2);
	for (ptr=(char*)Buff, ofs = 0; ofs < 0x200; ptr+=16, ofs+=16)
		put_dump((BYTE*)ptr, ofs, 16);
}

int term_cmd_di(const char *parameters) {
	debug_printf("rc=%d\n", (WORD)disk_initialize(0));
}

int term_cmd_ds(const char *parameters) {
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
}

int term_cmd_bd(const char *parameters) {
	if (!xatoi(&ptr, &p1)) break;
	for (ptr=(char*)&Buff[p1], ofs = p1, cnt = 32; cnt; cnt--, ptr+=16, ofs+=16)
		put_dump((BYTE*)ptr, ofs, 16);
}

int term_cmd_br(const char *parameters) {
	if (!xatoi(&ptr, &p2)) break;
	if (!xatoi(&ptr, &p3)) p3 = 1;
	debug_printf("rc=%u\n", (WORD)disk_read(0, Buff, p2, p3));
}

int term_cmd_bw(const char *parameters) {
	if (!xatoi(&ptr, &p2)) break;
	if (!xatoi(&ptr, &p3)) p3 = 1;
	debug_printf("rc=%u\n", (WORD)disk_write(0, Buff, p2, p3));
}

int term_cmd_bf(const char *parameters) {
	if (!xatoi(&ptr, &p1)) break;
	memset(Buff, (BYTE)p1, sizeof(Buff));
}

int term_cmd_fi(const char *parameters) {
	put_rc(f_mount(0, &Fatfs[0]));
}

int term_cmd_fs(const char *parameters) {
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
}

int term_cmd_fo(const char *parameters) {
	if (!xatoi(&ptr, &p1)) break;
	while (*ptr == ' ') ptr++;
	put_rc(f_open(&File1, ptr, (BYTE)p1));
}

int term_cmd_fc(const char *parameters) {
	put_rc(f_close(&File1));
}

int term_cmd_fe(const char *parameters) {
	if (!xatoi(&ptr, &p1)) break;
	res = f_lseek(&File1, p1);
	put_rc(res);
	if (res == FR_OK)
		debug_printf("fptr=%lu(0x%lX)\n", File1.fptr, File1.fptr);
}

int term_cmd_fd(const char *parameters) {
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
}

int term_cmd_fr(const char *parameters) {
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
}

int term_cmd_fw(const char *parameters) {
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
}

int term_cmd_fn(const char *parameters) {
	while (*ptr == ' ') ptr++;
	ptr2 = strchr(ptr, ' ');
	if (!ptr2) break;
	*ptr2++ = 0;
	while (*ptr2 == ' ') ptr2++;
	put_rc(f_rename(ptr, ptr2));
}

int term_cmd_fv(const char *parameters) {
	put_rc(f_truncate(&File1));
}

int term_cmd_fk(const char *parameters) {
	while (*ptr == ' ') ptr++;
	put_rc(f_mkdir(ptr));
}

int term_cmd_fa(const char *parameters) {
	if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
	while (*ptr == ' ') ptr++;
	put_rc(f_chmod(ptr, p1, p2));
}

int term_cmd_ft(const char *parameters) {
	if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
	Finfo.fdate = ((p1 - 1980) << 9) | ((p2 & 15) << 5) | (p3 & 31);
	if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
	Finfo.ftime = ((p1 & 31) << 11) | ((p1 & 63) << 5) | ((p1 >> 1) & 31);
	put_rc(f_utime(ptr, &Finfo));
}

int term_cmd_fm(const char *parameters) {
	if (!xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
	debug_printf("The drive 0 will be formatted. If you are really sure, use the command fmY instead of fm\n");
	if (*ptr == 'Y')
		put_rc(f_mkfs(0, (BYTE)p2, (WORD)p3));
}

int term_cmd_fz(const char *parameters) {
	if (xatoi(&ptr, &p1) && p1 >= 1 && (size_t)p1 <= sizeof(Buff))
		blen = p1;
	debug_printf("blen=%u\n", blen);
}

int term_cmd_goto(const char *parameters) {
	if (!xatoi(&ptr, &p1)) break;
	if (!xatoi(&ptr, &p2)) break;

	if(p1 >= 200 && p1 <= 1800 && p2 >= 200 && p2 <= 2800)
		ASSER_push_goto(p1, p2, FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);
}

int term_cmd_ls(const char *parameters) {
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
}

int term_cmd_rm(const char *parameters) {
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

int term_cmd_reset(const char *parameters) {
	NVIC_SystemReset();
}
*/
