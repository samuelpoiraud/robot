/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Test_main.c
 *	Package : Test CAN reception
 *	Description : Fonction receptrice pour test CAN
 *	Auteur : Jacen
 *	Version 20090502
 */

#define TEST_MAIN_C
#include "Test_main.h"
#ifdef TEST_MODE
int main (void)
	{
		#define NUMERO_RECEPTEUR	3
		CAN_msg_t Can_msg;
		int i =0;
		int erreur = 0;
		PORTS_init();
		CAN_init();
		UART_init();
//		TIMER_init();
		
		A_LED=1;
		debug_printf("Recepteur OK\n");

		while (1)
		{	
			if(global.flags.canrx)
			{
				Can_msg =CAN_get_next_msg();
				if (Can_msg.sid==1)
				{
					i=1;erreur=0;
				}
				if (Can_msg.data[0]=='M')
				{
					i++;
					A_LED = !A_LED;
					debug_printf("recu : Ox%x, count = Ox%x\n", Can_msg.sid, i);
					if (Can_msg.sid != (i+erreur))
					{
						debug_printf ("ERREUR : différence de comptage detectee !\n");
						erreur = Can_msg.sid - i;
					}
					Can_msg.sid+=NUMERO_RECEPTEUR;
					Can_msg.data[0]='S';
					CAN_send(&Can_msg);
				}
			}

		}
		return 0;
	}


#endif /* def TEST_MODE */
