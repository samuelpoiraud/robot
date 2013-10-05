#ifndef TERM_COMMANDS_UTILS_H
#define TERM_COMMANDS_UTILS_H

#include "Libraries/fat_sd/ff.h"
#include "Libraries/fat_sd/diskio.h"
#include "../../QS/QS_uart.h"

extern unsigned char term_common_buffer[];
extern const int term_common_buffer_size;

int fatfs_err_to_errno(FRESULT err);
int disk_err_to_errno(DRESULT err);
bool_e argtolong(const char *str, int base, long *value);

#endif // TERM_COMMANDS_UTILS_H
