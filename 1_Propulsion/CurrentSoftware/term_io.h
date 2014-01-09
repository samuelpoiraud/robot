#ifndef TERM_IO_H_
#define TERM_IO_H_

#include "usb_host/Class/MSC/fat_fs/integer.h"
#include "QS/QS_all.h"


int xatoi (char**, long*);
void xitoa (long, int, int);
void put_dump (const BYTE*, Uint32 ofs, int cnt);
char * get_command (void);
int get_line_r (char*, int len, int *idx);

void char_from_user(char c);

#endif /* TERM_IO_H_ */
