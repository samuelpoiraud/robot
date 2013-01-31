/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : main.c
 *	Package : Projet Propulsion
 *	Description : fonction principale d'exemple pour le projet 
 *				standard construit par la QS pour exemple, pour
 *				utilisation en Match
 *	Auteur : Jacen
 *	Version 20080924
 */


#include "main.h"
#include "odometry.h"
#include "copilot.h"
#include "pilot.h"
#include "supervisor.h"
#include "warner.h"
#include "it.h"
#include "roadmap.h"
#include "secretary.h"
#include "sequences.h"
#include "debug.h"
#include "joystick.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_buttons.h"
#include "QS/QS_CANmsgList.h"


#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
	extern volatile global_data_storage_t SAVE;
#endif

	
#if defined(VERBOSE_MSG_SEND_OVER_UART) ||				\
	 defined(SIMULATION_VIRTUAL_PERFECT_ROBOT) ||		\
	 defined(MODE_PRINTF_TABLEAU) ||					\
	 defined(MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT) ||	\
	 defined(SUPERVISOR_DISABLE_ERROR_DETECTION) ||		\
	 !defined(ENABLE_CAN)
	#warning "SOYEZ CONSCIENT QUE VOUS NE COMPILEZ PAS EN MODE MATCH..."
#endif

void RCON_read(void);
void button_autocalage_avant(void);
void button_autocalage_arriere(void);

void initialisation(void){
	
	// Config des ports
	PORTS_init();			
	
	LED_RUN = 1;
		
	// Pour l'utilisation de la fenetre psv
	//> ceci est utile pour le stockage d'un tableau de boeuf dans la mémoire programme
	CORCONbits.PSV=1;					
	
	ODOMETRY_init();
	SUPERVISOR_init();
	COPILOT_init();
	PILOT_init();
	SECRETARY_init();
	ROADMAP_init();
	WARNER_init();

	  
	UART_init(); //Si les résistances de tirages uart ne sont pas reliées, le code bloque ici si aucun cable n'y est relié.		
	Uint16 delay;
	for(delay = 1;delay;delay++);	//attente pour que l'UART soit bien prete...
	RCON_read();

		JOYSTICK_init();
	DEBUG_init();
	
	BUTTONS_init();
//	BUTTONS_define_actions(BUTTON3,button_autocalage_avant, NULL, 0);
//	BUTTONS_define_actions(BUTTON1,button_autocalage_arriere, NULL, 0);
	
	#ifdef USE_QSx86
		// Initialisation pour EVE
		EVE_manager_card_init();
	#endif	/* USE_QSx86 */

	
	IT_init();
	/*
	Récapitulatif des priorités des ITs :
	-> 7 : Codeurs QEI_on_it
	-> 6 : CAN
	-> 5 : timer1 (5ms)
	-> 4 : UART
	-> 3 : timer2 (100ms)
	-> 0 : tâche de fond
	*/
}

int main (void)
{
	initialisation();

	//Routines de tests UART et CAN
	debug_printf("\nAsser Ready !\n");
		
	while(1)
	{
		#ifdef USE_QSx86
			// Update pour EVE
			EVE_manager_card();
		#endif	/* USE_QSx86 */
		
		DEBUG_process_main();
		
		BUTTONS_update();			//Gestion des boutons

		SECRETARY_process_main();	//Communication avec l'extérieur. (Réception des messages)
		
		WARNER_process_main();		//Communication avec l'extérieur. (Envois des messages)
		
	}
	return 0;
}
/*
void button_autocalage_avant(void)
{
	SEQUENCES_calibrate(FORWARD);
}

void button_autocalage_arriere(void)
{
	SEQUENCES_calibrate(BACKWARD);
}
*/
void RCON_read(void)
{
	debug_printf("dsPIC30F reset source :\r\n");
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


