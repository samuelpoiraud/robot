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
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include "hokuyo.h"

#if defined (STM32F40XX)
	#include "QS/QS_sys.h"
#endif
#if defined (LCD_TOUCH)
	#include "LCDTouch/stm32f4_discovery_lcd.h"
	#include "LCDTouch/LCDTouch_Display.h"
	#include "LCDTouch/LCD.h"
	#include "LCDTouch/zone.h"
#endif
#ifdef	SCAN_TRIANGLE
	#include "scan_triangle.h"
#endif

#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
	extern volatile global_data_storage_t SAVE;
#endif


#if  defined (LCD_TOUCH)			||					\
	 defined(SIMULATION_VIRTUAL_PERFECT_ROBOT) ||		\
	 defined(MODE_PRINTF_TABLEAU) ||					\
	 defined(MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT) ||	\
	 defined(SUPERVISOR_DISABLE_ERROR_DETECTION) ||		\
	 defined(MODE_REGLAGE_KV) ||						\
	 !defined(ENABLE_CAN)
	#warning "SOYEZ CONSCIENT QUE VOUS NE COMPILEZ PAS EN MODE MATCH..."
#endif




void RCON_read(void);


void initialisation(void)
{

	#ifdef USE_QSx86
		// Initialisation pour EVE
		EVE_manager_card_init();
	#endif	/* USE_QSx86 */
	#if defined(STM32F40XX)
		SYS_init();
	#endif
	// Config des ports
	PORTS_init();

	LED_RUN = 1;

#ifdef __dsPIC30F6010A__
	// Pour l'utilisation de la fenetre psv
	//> ceci est utile pour le stockage d'un tableau de boeuf dans la mémoire programme
	CORCONbits.PSV=1;
#endif

	UART_init(); //Si les résistances de tirages uart ne sont pas reliées, le code bloque ici si aucun cable n'y est relié.

	RCON_read();

	//Doit se faire AVANT ODOMETRY_init() !!!

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//Détermine le robot sur lequel est branchée la carte.
	debug_printf("--- Hello, I'm PROP (%s) ---\n", QS_WHO_AM_I_get_name());

	SECRETARY_init();	//Pour recevoir tout les messages CAN envoyés très tôt...
	ODOMETRY_init();
	SUPERVISOR_init();
	COPILOT_init();
	PILOT_init();

	ROADMAP_init();
	WARNER_init();

	JOYSTICK_init();
	DEBUG_init();
	BUTTONS_init();
	#ifdef USE_HOKUYO
		HOKUYO_init();
	#endif



	#if defined (LCD_TOUCH)
		LCD_init();
	#endif

	#ifdef SCAN_TRIANGLE
		SCAN_TRIANGLE_init();
		// Initialisation des ADC pour les DT10s des scans des triangles
	#endif

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
/*		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1793,894	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1734,918	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1678,947	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1624,982	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1575,1022	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1529,1066	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1489,1115	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1453,1168	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1423,1223	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1398,1282	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1380,1343	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1367,1405	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1361,1468	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1361,1532	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1367,1595	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1380,1657	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1398,1718	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1423,1777	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1453,1832	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1489,1885	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1529,1934	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1575,1978	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1624,2018	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1678,2053	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1734,2082	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1793,2106	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1830,2115	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);


*/

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
		#ifdef SCAN_TRIANGLE
			SCAN_TRIANGLE_calculate();
		#endif
#if defined (LCD_TOUCH)
		LCD_process_main();
#endif

		#ifdef USE_HOKUYO
			HOKUYO_process_main();
		#endif
	}
	return 0;
}


void RCON_read(void)
{
#if defined(STM32F40XX)
	debug_printf("STM32F4xx reset source :\n");
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST))
		debug_printf("- Low power management\n");
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
		debug_printf("- Window watchdog time-out\n");
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
		debug_printf("- Independent watchdog time-out\n");
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST))
		debug_printf("- Software reset\n");
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST))
		debug_printf("- POR\n");
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST))
		debug_printf("- Pin NRST\n");
	if(RCC_GetFlagStatus(RCC_FLAG_BORRST))
		debug_printf("- POR or BOR\n");
	RCC_ClearFlag();
#endif
}




