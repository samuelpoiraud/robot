/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Test_main.c
 *	Package : Projet Standard
 *	Description : fonction principale d'exemple pour le projet 
 *				standard construit par la QS pour exemple, permettant
 *				l'appel des fonctions de test
 *	Auteur : Jacen
 *	Version 20090502
 */

#define TEST_MAIN_C
#include "Test_main.h"
#ifdef TEST_MODE
int main (void)
	{
		CAN_msg_t msg;
		bool_e display=FALSE;
		PORTS_init();
		UART_init();
		TIMER_init();
		CAN_init();
		
		global.receiver1_error_count=0;
		global.receiver3_error_count=0;
		
		TIMER1_run(250);
		//A_LED=1;
		debug_printf("Debut d'emission\n");
		while (1)
		{
			if (global.flags.canrx)
			{
				msg = CAN_get_next_msg();
				if (msg.sid == global.current_sid+1)
					global.receiver1_error_count--;
				if (msg.sid == global.current_sid+3)
					global.receiver3_error_count--;
				if(display)
					debug_printf("Error count : 1:%d 3:%d\n", 
						global.receiver1_error_count,
						global.receiver3_error_count);
				display = !display;
			}	
		}
		return 0;
	}

void _ISR _T1Interrupt()
{
	Uint8 i;
	static Uint11 sid=1;
	static CAN_msg_t msg={0x42, "MASTER ", 8};
	A_LED=!A_LED;
	global.current_sid=sid;
	msg.sid=sid++;
	CAN_send(&msg);
	global.receiver1_error_count++;
	global.receiver3_error_count++;
	debug_printf("a envoye %d\n", sid-1);
	if (sid>= 0x7F0)
	{
		sid=1;
		msg.size--;
		if(!msg.size)
			msg.size=8;
		//debug_printf("Fin d'emission\n");
		//TIMER1_stop();
	}
	else
	{
		// modifier les data. avec un peu de chance ce bidouillage permet
		// d'obtenir toutes les valeurs
		msg.data[1]+=sid;
		for(i=2;i<8;i++)
		{
			msg.data[i]+= msg.data[i-1];
		}	
	}
	IFS0bits.T1IF=0;
}


#endif /* def TEST_MODE */
