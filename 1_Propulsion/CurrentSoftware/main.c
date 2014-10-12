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
#include "QS/QS_watchdog.h"
#include "QS/QS_can.h"
#include "hokuyo.h"
#include "gyroscope.h"
#include "detection.h"
#include "clock.h"

#if defined (STM32F40XX)
	#include "QS/QS_sys.h"
#endif
#if defined (LCD_TOUCH)
	#include "LCDTouch/stm32f4_discovery_lcd.h"
	#include "LCDTouch/LCDTouch_Display.h"
	#include "LCDTouch/LCD.h"
	#include "LCDTouch/zone.h"
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
void SWITCHS_update(void);
volatile Uint8 t_ms = 0;
volatile bool_e flag_calibration_asked = FALSE;
volatile bool_e flag_selftest_asked = FALSE;

void calibration_button_action(void)
{
	flag_calibration_asked = TRUE;
}

void blue_button_action(void)
{
	flag_selftest_asked = TRUE;
}


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
	//> ceci est utile pour le stockage d'un tableau de boeuf dans la m�moire programme
	CORCONbits.PSV=1;
#endif
	SECRETARY_init();	//Pour recevoir tout les messages CAN envoy�s tr�s t�t...
	volatile Uint32 i;
	for(i=0;i<1000000;i++);	//tempo (env 50ms) pour un bon fonctionnement de l'UART lorsqu'on branche les cartes. Sinon, les premiers printf ne sont pas envoy�s -> ????
	LED_RUN = 0;
	UART_init();

	RCON_read();

	//Doit se faire AVANT ODOMETRY_init() !!!

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//D�termine le robot sur lequel est branch�e la carte.
	debug_printf("--- Hello, I'm PROP (%s) ---\n", QS_WHO_AM_I_get_name());


	ODOMETRY_init();
	WATCHDOG_init();
	SUPERVISOR_init();
	COPILOT_init();
	PILOT_init();
	global.mode_best_effort_enable = FALSE;
	global.match_started = FALSE;
	global.match_over = FALSE;
	global.absolute_time = 0;

	ROADMAP_init();
	WARNER_init();

	JOYSTICK_init();
	DEBUG_init();
	BUTTONS_init();
	BUTTONS_define_actions(BUTTON0,&blue_button_action, &calibration_button_action, 1);
	BUTTONS_define_actions(BUTTON1,&calibration_button_action, NULL, 1);

	DETECTION_init();
	#ifdef USE_HOKUYO
		HOKUYO_init();
	#endif

	#ifdef USE_GYROSCOPE
		GYRO_init();
	#endif

	#if defined (LCD_TOUCH)
		LCD_init();
	#endif

	CLOCK_init();
/*
	time32_t begin_waiting_time = global.absolute_time;
	CAN_msg_t waiting_msg;
	while(global.absolute_time - begin_waiting_time < 300){
		while (CAN_data_ready())
		{
			LED_CAN=!LED_CAN;
			waiting_msg = CAN_get_next_msg();
			if(waiting_msg.sid == BROADCAST_RESET){
				NVIC_SystemReset();
			}
		}
	}
	CAN_direct_send(BROADCAST_I_AM_READY, 1, (Uint8[]){I_AM_READY_PROP});
	display(I_AM_READY_PROP);
	debug_printf("--- Hello, I'm PROP (%s) ---\n", QS_WHO_AM_I_get_name());
	debug_printf("\nAsser Ready !\n");
	debug_printf("\nWaiting others board\n");
	begin_waiting_time = global.absolute_time;
	bool_e FDP_init = FALSE;
	while(global.absolute_time - begin_waiting_time < 300 && !FDP_init){
		while (CAN_data_ready())
		{
			LED_CAN=!LED_CAN;
			waiting_msg = CAN_get_next_msg();
			if(waiting_msg.sid == BROADCAST_FDP_READY)
				FDP_init = TRUE;
		}
	}*/


	IT_init();
	/*
	R�capitulatif des priorit�s des ITs :
	-> 7 : Codeurs QEI_on_it
	-> 6 : CAN
	-> 5 : timer1 (5ms)
	-> 4 : UART
	-> 3 : timer2 (100ms)
	-> 0 : t�che de fond
	*/


}

int main (void)
{
	initialisation();
	LED_RUN = 1;

	//Routines de tests UART et CAN
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

	#ifdef USE_GYROSCOPE
		WATCHDOG_create(1000, &GYRO_test, TRUE);
	#endif

	while(1)
	{
		#ifdef USE_QSx86
			// Update pour EVE
			EVE_manager_card();
		#endif	/* USE_QSx86 */

		DEBUG_process_main();

		if(t_ms > 20)	//Pour �viter les rebonds
		{
			t_ms = 0;
			BUTTONS_update();			//Gestion des boutons
			SWITCHS_update();			//Surveillance des switchs
		}


		SECRETARY_process_main();	//Communication avec l'ext�rieur. (R�ception des messages)

		WARNER_process_main();		//Communication avec l'ext�rieur. (Envois des messages)

#if defined (LCD_TOUCH)
		LCD_process_main();
#endif

		#ifdef USE_HOKUYO
			HOKUYO_process_main();
		#endif
		DETECTION_process_main();

		if(flag_calibration_asked)
		{
			flag_calibration_asked = FALSE;
			debug_printf("BP calibration pressed\n");
			SEQUENCES_calibrate();
		}

		if(flag_selftest_asked)
		{
			flag_selftest_asked = FALSE;
			debug_printf("BP Selftest pressed\n");
			SEQUENCES_selftest();
		}

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

void SWITCHS_update(void)
{
	static bool_e previous_switch_left = FALSE;
	static bool_e previous_switch_middle = FALSE;
	static bool_e previous_switch_right = FALSE;
	bool_e current_switch_left;
	bool_e current_switch_middle;
	bool_e current_switch_right;

	current_switch_left 	= SWITCH_LEFT;
	current_switch_middle 	= SWITCH_MIDDLE;
	current_switch_right 	= SWITCH_RIGHT;

	if(current_switch_left 		&& !previous_switch_left) 	debug_printf("SW left enabled\n");
	if(current_switch_middle 	&& !previous_switch_middle) debug_printf("SW middle enabled\n");
	if(current_switch_right 	&& !previous_switch_right)	debug_printf("SW right enabled\n");

	if(!current_switch_left 	&& previous_switch_left) 	debug_printf("SW left disabled\n");
	if(!current_switch_middle 	&& previous_switch_middle) 	debug_printf("SW middle disabled\n");
	if(!current_switch_right 	&& previous_switch_right)	debug_printf("SW right disabled\n");

	previous_switch_left 	= current_switch_left;
	previous_switch_middle 	= current_switch_middle;
	previous_switch_right 	= current_switch_right;

	//TODO quelles actions associer � ces switchs......?
}

void MAIN_process_it(Uint8 ms)
{
	t_ms += ms;
}
