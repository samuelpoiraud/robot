/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : main.c
 *	Package : EVE
 *	Description : Ordonnancement d'EVE
 *	Auteur : Julien Franchineau
 *	Version 20111201
 */

#define MAIN_C
#include "main.h"

// tests
#include <unistd.h>
void fucking_test()
{
	printf("FUCKING_TEST\n");
	Uint16 error, bal_ref;
	char* memory_reference;
	EVE_time_t time = 0,old_time = 0;
	int check = 1;
//	EVE_UART_msg_t* buffer;

	printf("In the thread\n");

	error = EVE_create_and_bound_global_clock_memory(&memory_reference);
	if(error != END_SUCCESS)
	{
		// On a une erreur donc on ne peut pas aller plus loin
		printf("Fail create clock memory\n");
	}

	bal_ref = EVE_create_msg_queue(BAL_STRATEGIE_TO_IHM_UART);
	if(bal_ref == -1)
	{
		// Erreur ^^
		printf("Fail create message queue for test\n");
	}

	while(1)
	{
		time = EVE_get_global_clock(memory_reference);
		if(time%1000 == 0 && check == 1)
		{
			printf("%dms pass\n",(int)time);
			check = 0;
			old_time = time;
		}
		if(old_time!=time)
		{
			//printf("Time : %d\n",time);
			old_time = time;
			check = 1;
		}

		/*if((error = EVE_check_messages_numbers(bal_ref))>0)
		{
			if(EVE_read_new_msg_UART(bal_ref,buffer) != END_SUCCESS)
				printf("\nSome fail happened\n");
			else
			{
				printf("Strat : %s",buffer->data);
			}
		}*/
	}

	error = EVE_unbound_global_clock_memory(memory_reference);
	if(error != END_SUCCESS)
		printf("Fail unbound clock\n");

	printf("End fucking_test\n");
	while(1){};
}
// end of tests


int main (int argc, char* argv[])
{
	// Initialisation des variables globales
	GLOBAL_VARS_init();


	// Exécution du gestionnaire des threads
	MANAGER_main();

	//while(1);
	//fucking_test();

	return 0;
}

