#ifndef TERM_IO_H_
#define TERM_IO_H_

#include "Libraries/fat_sd/integer.h"
#include "../../QS/QS_all.h"


int xatoi (const char **, long*);
void xitoa (long, int, int);
void put_dump (const BYTE*, Uint32 ofs, int cnt);
char * get_command (void);
int get_line_r (char*, int len, int *idx);

void SD_char_from_user(char c);

#endif /* TERM_IO_H_ */
