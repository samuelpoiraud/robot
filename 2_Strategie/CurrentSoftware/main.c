/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : main.c
 *	Package : Carte Principale
 *	Description : Ordonnancement de la carte Principale
 *	Auteur : Jacen, modifi� par Gonsevi
 *	Version 2012/01/14
 */

#define MAIN_C
#include "main.h"
	#include "QS/QS_ports.h"
	#include "QS/QS_uart.h"
	#include "Stacks.h"
	#include "environment.h"
	#include "brain.h"
	#include "clock.h"
	#include "QS/QS_who_am_i.h"
	#include "queue.h"
#include "zone_mutex.h"

void process_measure_loop_duration(void)
{
	static time32_t previous_time = 0;
	if(global.env.match_time > 0)
	{
		//D�commenter ce printf pour afficher la mesure de la dur�e de la boucle.
		//ATTENTION, cet affichage est hors de la mesure... (donc elle impacte le code hors de cette mesure).
		if(global.env.match_time-previous_time > 5)
			debug_printf("L%d\n",(Uint16)(global.env.match_time-previous_time));
		previous_time = global.env.match_time;
	}
}

int main (void)
{

	Uint16 i,j;

	// Commandes pour EVE
	#ifdef USE_QSx86
		// Initialisation des variables utiles
		EVE_manager_card_init();
	#endif // USE_QSx86

	PORTS_init();
	#ifdef VERBOSE_MODE
		UART_init();
	#endif /* def VERBOSE_MODE */
	LED_RUN=1;
	debug_printf("\n-------\nDemarrage CarteP\n-------\n");
	RCON_read(); //permet de voir le type du dernier reset
	
	STACKS_init();
	ENV_init();
	CLOCK_init();
		
	//retard pour attendre l'initialisation des autres cartes
	// voir si on peut faire mieux
	for(j=0;j<40;j++)
		for(i=1;i;i++);


	
	
	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//D�termine le robot sur lequel est branch�e la carte.

	ZONE_init();

	debug_printf("I am %s\n",(QS_WHO_AM_I_get()==TINY)?"TINY":"KRUSTY");
	
	//on passe la config actuelle � la Super
	ENV_dispatch_config();
	
	while(1)
	{
		LED_RUN = !LED_RUN;
		// Commandes pour EVE
		#ifdef USE_QSx86
			EVE_manager_card();
		#endif // USE_QSx86
	
		/* mise � jour de l'environnement */
		ENV_update();

		/* Execution des routines de supervision des piles
		actionneurs */
		STACKS_run();
		QUEUE_run();
		
		
		/* Execution de la routine d'IA */
//		if (global.env.config.strategie < (sizeof(strategies)/sizeof(ia_fun_t)))
//		{
//			any_match(strategies[global.env.config.strategie], MATCH_DURATION);
//		}	
//		else
//		{
//			global.env.config.strategie = 1;
//		}
/*#ifdef FDP_2013	//Pour r�trocompatibilit�...
	#warning "Pensez � cr�er des strat�gies diff�rentes pour Tiny et Krusty... et � les inclure ci-dessous avant de virer ce warning."

		if(QS_WHO_AM_I_get()==TINY)

			any_match(MATCH_DURATION); //TINY
		else
#endif
			//any_match(TEST_STRAT_lever_le_kiki, MATCH_DURATION); //KRUSTY
		    any_match(MATCH_DURATION);
	*/
		any_match(MATCH_DURATION);
//		any_match(0);
//#warning "MATCH INFINI pour test evitement !!!"

		process_measure_loop_duration();
	}
	return 0;
}


void RCON_read()
{
	debug_printf("dsPIC30F reset source :\r\n");
	if(!(RCON & 0xC0DF))
	{
		debug_printf(" - NO SOURCE OF RESET !!!???");
		ASSER_dump_statck();
	}
	if(RCON & 0x8000)
		debug_printf("- Trap conflict event\r\n");
	if(RCON & 0x4000)
		debug_printf("- Illegal opcode or uninitialized W register access\r\n");
	if(RCON & 0x80)
		debug_printf("- MCLR Reset\r\n");
	if(RCON & 0x40)
		debug_printf("- RESET instruction\r\n");
	if(RCON & 0x10)
		debug_printf("- WDT time-out\r\n");
	if(RCON & 0x8)
		debug_printf("- PWRSAV #SLEEP instruction\r\n");
	if(RCON & 0x4)
		debug_printf("- PWRSAV #IDLE instruction\r\n");
	if(RCON & 0x2)
		debug_printf("- POR, BOR\r\n");
	if(RCON & 0x1)
		debug_printf("- POR\r\n");
	RCON=0;
}
/* Trap pour debug reset */
void _ISR _MathError()
{
  _MATHERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Math\r\n");
  while(1) Nop();
}

void _ISR _StackError()
{
  _STKERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Stack\r\n");
  while(1) Nop();
}

void _ISR _AddressError()
{
  _ADDRERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Address\r\n");
  while(1) Nop();
}

void _ISR _OscillatorFail()
{
  _OSCFAIL = 0;
  LED_ERROR = 1;
  debug_printf("Trap OscFail\r\n");
  while(1) Nop();
}
