#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include"mySDLlib.h"


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
	
 /* Macros */
 	#define BEACON_ADVERSARY_POSITION_IR_ARROUND_AERA	0x252
 	#define SET_COLOR 									0x3
 	#define CALIBRAGE									0x10B
 	#define ASSER_PUSH									0x155


 /* Global vars */
	unsigned short run,flag;
	CAN_msg_t msg_to_send;
	int port_serie;

	
 /* Prototypes */
	Uint32 send_position(Uint32 intervalle, void *parameter);
	void can_send(CAN_msg_t * msg, int port_serie);
	void position_adversaire_CAN(SDL_Rect pos, SDL_Event  *event);
	void set_red_CAN(void);
	void set_blue_CAN(void);
	void calibrage_CAN(void);
	void asser_stop_CAN(void);
