#ifndef MACRO_TYPES_H
#define MACRO_TYPES_H


#include <stdio.h>
#include <stdlib.h>


 /* Type de base pour le dsPIC */
	typedef unsigned char Uint8;
	typedef signed char Sint8;
	typedef unsigned short Uint16;		//A VERIFIER POUR UNE ARCHI 32 BITS ???
	typedef signed short Sint16;		//A VERIFIER POUR UNE ARCHI 32 BITS ???
	typedef unsigned int Uint32;		//A VERIFIER POUR UNE ARCHI 32 BITS ???
	typedef signed int Sint32;			//A VERIFIER POUR UNE ARCHI 32 BITS ??? 
	/* Type pour les SID du protocole CAN */
	typedef Uint16	Uint11;

 /* Structures */
	typedef struct 
	{
		Uint11 sid;
		Uint8 data[8];
		Uint8 size;
	}CAN_msg_t;
	#define SOH 0x01
	#define EOT 0x04
	
	typedef enum
	{
		FALSE = 0,
		TRUE
	}bool_e;
 /* Macros */
 	#define BEACON_ADVERSARY_POSITION_IR_ARROUND_AERA	0x252
 	#define SET_COLOR 									0x3
 	#define CALIBRAGE									0x10B
 	#define ASSER_PUSH									0x155

	#define ABS(x) (((x)>=0)?(x):(-(x)))
	#define MIN(a, b)				(((a) > (b)) ? (b) : (a))
	#define MAX(a, b)				(((a) > (b)) ? (a) : (b))

	#define HIGHINT(x)				(((x) >> 8) & 0xFF)
	#define LOWINT(x)				((x) & 0xFF)
	#define U16FROMU8(high,low)		((((Uint16)(high))<<8)|(Uint16)(low))
	#define BITS_ON(var, mask)		((var) |= (mask))
	/* ~0 est le complement à 1 de 0, donc pour 16 bits OxFFFF) */
	/* ~0 ^ mask permet d'etre indépendant de la taille (en bits) de ~mask */
	#define BITS_OFF(var, mask)		((var) &= ~0 ^ (mask))
	#define BIT_SET(var, bitno)		((var) |= (1 << (bitno)))
	#define BIT_CLR(var, bitno)		((var) &= ~(1 << (bitno)))
	#define BIT_TEST(data, bitno)	(((data) >> (bitno)) & 0x01)

#endif //def MACRO_TYPES_H

