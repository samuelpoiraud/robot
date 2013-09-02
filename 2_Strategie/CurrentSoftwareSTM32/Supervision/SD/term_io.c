#include <stdarg.h>
#include "term_io.h"
#include "../../QS/QS_all.h"
#include "../../QS/QS_uart.h"

int xatoi (char **str, long *res)
{
	DWORD val;
	BYTE c, radix, s = 0;


	while ((c = **str) == ' ') (*str)++;
	if (c == '-') {
		s = 1;
		c = *(++(*str));
	}
	if (c == '0') {
		c = *(++(*str));
		if (c <= ' ') {
			*res = 0; return 1;
		}
		if (c == 'x') {
			radix = 16;
			c = *(++(*str));
		} else {
			if (c == 'b') {
				radix = 2;
				c = *(++(*str));
			} else {
				if ((c >= '0')&&(c <= '9'))
					radix = 8;
				else
					return 0;
			}
		}
	} else {
		if ((c < '1')||(c > '9'))
			return 0;
		radix = 10;
	}
	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;
		}
		if (c >= radix) return 0;
		val = val * radix + c;
		c = *(++(*str));
	}
	if (s) val = -val;
	*res = val;
	return 1;
}


void xitoa (long val, int radix, int len)
{
	BYTE c, r, sgn = 0, pad = ' ';
	BYTE s[20], i = 0;
	DWORD v;


	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	if (len > 20) return;
	do {
		c = (BYTE)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	if (sgn) s[i++] = sgn;
	while (i < len)
		s[i++] = pad;
	do
		UART1_putc(s[--i]);
	while (i);
}


void put_dump (const BYTE *buff, DWORD ofs, int cnt)
{
	BYTE n;


	debug_printf("%08lX ", ofs);
	for(n = 0; n < cnt; n++)
		debug_printf(" %02X", buff[n]);
	UART1_putc(' ');
	for(n = 0; n < cnt; n++) {
		if ((buff[n] < 0x20)||(buff[n] >= 0x7F))
			UART1_putc('.');
		else
			UART1_putc(buff[n]);
	}
	UART1_putc('\r');
	UART1_putc('\n');
}


#define BUFFER_COMMAND_SIZE 120
volatile Uint8 buffer_command[BUFFER_COMMAND_SIZE];
volatile Uint8 index = 0;
volatile bool_e command_available = TRUE;

void SD_char_from_user(char c)
{
	switch(c)
	{
		case '\r':
			command_available = TRUE;
			if(index>0)
				buffer_command[index] = 0;
			index = 0;
			//printf("\n");	//Echo
			break;
		case '\b':
			if(index)
			{
				index--;
				printf("\b \b");
			}
			break;
		case '\n':
			break;
		default:
			if(c >= ' ')	//Caractère "normal..."
				if(index < BUFFER_COMMAND_SIZE - 1)
				{
					buffer_command[index] = c;
					index++;	//Sinon on incrémente plus...
				}
				//printf("%c",c);	//Echo
			break;
	}
}

char * get_command (void)
{
	if(command_available)
	{
		command_available = FALSE;
		return buffer_command;
	}
	else
		return 0;
}


