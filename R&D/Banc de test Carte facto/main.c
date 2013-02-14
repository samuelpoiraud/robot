/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi-Tech', PACMAN
 *
 *	Fichier : main.h
 *	Package : Supervision
 *	Description : Code de test du hard des cartes
 *	Auteur : Jacen
 *	Version 20100105
 */

#define MAIN_C
#include "main.h"


//TODO: réordonner les LEDs pour que ce soit plus joli

int main (void)
{
	init();

	debug_printf("Carte Test, test dispo:\n"
				 "  Bouton 1: Test ports (sauf Bx) par les LEDs\n"
				 "  Bouton 2: Test entrées analogiques (Bx/ANx), doit être ~= 2.5V\n"
				 "  Bouton 3: Test UART 1 (echo)\n"
				 "  Bouton 4: Test UART 2 (echo)\n"
				 "  Bouton 3 et 4 en même temps (maintenir): Test CAN\n\n");
	
	while(1)
	{
		test_carte_facto_update();
	}
	return 0;	
}

void init()
{
	PORTS_init();
	UART_init();
	PWM_init();
	TIMER_init(); //pour watchdog
	test_carte_facto_init();
}	
