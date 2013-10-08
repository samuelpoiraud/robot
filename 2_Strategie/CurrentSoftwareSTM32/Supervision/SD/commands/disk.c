#include "ls.h"
#include <errno.h>
#include <string.h>
#include "../term_commands_utils.h"
#include "../term_io.h"
#include "../../../QS/QS_all.h"

const char term_cmd_disk_brief[] = "Gère les disques";
const char term_cmd_disk_help[] =
		"Utilisation : disk [num] [commande]\n"
		"\n"
		"num         Numéro du disque (0 le plus souvent)"
		"commande    Une de ces commandes:\n"
		" --dump <sector_num>  affiche un secteur\n"
		" --reinit             reinitialise le controleur du disque\n"
		" --status             affiche le status\n"
		"\n";

static int dump_sector(int disk, int sector);
static int reinit(int disk);
static int show_status(int disk);


int term_cmd_disk(int argc, const char *argv[]) {
	typedef enum {
		CMD_NONE,
		CMD_DUMP,
		CMD_REINIT,
		CMD_STATUS
	} disk_cmd_e;

	disk_cmd_e command = CMD_NONE;
	long sector = 0;
	long disk = 0;
	int i;

	for(i = 0; i < argc; i++) {
		if((i+1) < argc && !strcmp(argv[i], "--dump")) {
			if(argtolong(argv[i+1], 0, &sector)) {
				command = CMD_DUMP;
			}
		} else if(!strcmp(argv[i], "--reinit")) {
			command = CMD_REINIT;
		} else if(!strcmp(argv[i], "--status")) {
			command = CMD_STATUS;
		} else {
			argtolong(argv[i], 0, &disk);
		}
	}

	if(disk < 0)
		return EINVAL;

	debug_printf("Using disk %ld\n", disk);

	switch(command) {
		case CMD_NONE:
			return EINVAL;

		case CMD_DUMP:
			return dump_sector(disk, sector);

		case CMD_REINIT:
			return reinit(disk);

		case CMD_STATUS:
			return show_status(disk);
	}

	return 0;
}

static int dump_sector(int disk, int sector) {
	const BYTE* ptr;
	int res;
	long ofs;

	res = disk_err_to_errno(disk_read(disk, term_common_buffer, sector, 1));
	if (res)
		return res;

	debug_printf("Sector: %u\n", sector);

	for (ptr=(const BYTE*)term_common_buffer, ofs = 0; ofs < 512; ptr+=16, ofs+=16)
		put_dump(ptr, ofs, 16);

	return 0;
}

static int reinit(int disk) {
	return disk_err_to_errno(disk_initialize(disk));
}

static int show_status(int disk) {
	term_common_buffer[0] = 2;
	long data;

	if (disk_ioctl(disk, CTRL_POWER, term_common_buffer) == RES_OK )
		debug_printf("Power is %s\n", term_common_buffer[1] ? "ON" : "OFF");

	data = 0;
	if (disk_ioctl(disk, GET_SECTOR_COUNT, &data) == RES_OK)
		debug_printf("Drive size: %lu sectors\n", data);

	data = 0;
	if (disk_ioctl(disk, GET_SECTOR_SIZE, &data) == RES_OK)
		debug_printf("Sector size: %lu\n", data);

	data = 0;
	if (disk_ioctl(disk, GET_BLOCK_SIZE, &data) == RES_OK)
		debug_printf("Erase block size: %lu sectors\n", data);

	data = 0;
	if (disk_ioctl(disk, MMC_GET_TYPE, &data) == RES_OK)
		debug_printf("MMC/SDC type: %lu\n", data);

	if (disk_ioctl(disk, MMC_GET_CSD, term_common_buffer) == RES_OK){
		debug_printf("CSD:\n");
		put_dump(term_common_buffer, 0, 16);
	}

	if (disk_ioctl(disk, MMC_GET_CID, term_common_buffer) == RES_OK){
		debug_printf("CID:\n");
		put_dump(term_common_buffer, 0, 16);
	}

	if (disk_ioctl(disk, MMC_GET_OCR, term_common_buffer) == RES_OK){
		debug_printf("OCR:\n");
		put_dump(term_common_buffer, 0, 4);
	}

	if (disk_ioctl(disk, MMC_GET_SDSTAT, term_common_buffer) == RES_OK) {
		debug_printf("SD Status:\n");
		for (data = 0; data < 64; data += 16)
			put_dump(term_common_buffer+data, data, 16);
	}

	return 0;
}
