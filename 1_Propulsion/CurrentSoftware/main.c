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
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_buttons.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_watchdog.h"
#include "QS/QS_can.h"
#include "QS/QS_sys.h"
#include "QS/QS_systick.h"
#include "QS/QS_adc.h"
#include "QS/QS_IHM.h"
#include "QS/QS_rcc.h"
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
#include "hokuyo.h"
#include "gyroscope.h"
#include "detection.h"
#include "scan_cup.h"

#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
	extern volatile global_data_storage_t SAVE;
#endif



#if	 defined(SIMULATION_VIRTUAL_PERFECT_ROBOT) ||		\
	 defined(MODE_PRINTF_TABLEAU) ||					\
	 defined(MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT) ||	\
	 defined(SUPERVISOR_DISABLE_ERROR_DETECTION) ||		\
	 defined(MODE_REGLAGE_KV)
	#warning "SOYEZ CONSCIENT QUE VOUS NE COMPILEZ PAS EN MODE MATCH..."
#endif




void RCON_read(void);

static void MAIN_global_var_init();
static void MAIN_sensor_test();
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

	// Initialisation du syst�me
	SYS_init();				// Init syst�me

	#ifndef FDP_2016
		PORTS_init();	// Config des ports
	#else
		if(PORTS_secure_init() == FALSE){
			error_printf("Blocage car le code ne d�marre pas sur le bon slot !\n");
			while(1);
		}
	#endif

	MAIN_global_var_init();	// Init variable globale
	SYSTICK_init((time32_t*)&(global.absolute_time));

	GPIO_SetBits(LED_RUN);

	SECRETARY_init();	//Pour recevoir tout les messages CAN envoy�s tr�s t�t...
	GPIO_ResetBits(LED_RUN);
	UART_init();

	RCC_read();

	//Doit se faire AVANT ODOMETRY_init() !!!

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//D�termine le robot sur lequel est branch�e la carte.
	debug_printf("--- Hello, I'm PROP (%s) ---\n", QS_WHO_AM_I_get_name());

	ODOMETRY_init();
	WATCHDOG_init();
	SUPERVISOR_init();
	COPILOT_init();
	PILOT_init();
	ROADMAP_init();
	WARNER_init();
	JOYSTICK_init();
	DEBUG_init();
	BUTTONS_init();
	#ifdef SCAN_CUP
		SCAN_CUP_init();
	#endif
	BUTTONS_define_actions(BUTTON0,&blue_button_action, &calibration_button_action, 1);

	DETECTION_init();
	#ifdef USE_HOKUYO
		HOKUYO_init();
	#endif

	#ifdef USE_GYROSCOPE
		GYRO_init();
	#endif

	IT_init();

	// Demande des �tats initiaux des switchs
	CAN_send_sid(IHM_GET_SWITCH);

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
	GPIO_SetBits(LED_RUN);

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
		//WATCHDOG_create(1000, &GYRO_test, TRUE);
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
		}

		SECRETARY_process_main();	//Communication avec l'ext�rieur. (R�ception des messages)

		WARNER_process_main();		//Communication avec l'ext�rieur. (Envois des messages)

		#ifdef USE_HOKUYO
			HOKUYO_process_main();
		#endif

		DETECTION_process_main();

		MAIN_sensor_test();

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


void MAIN_process_it(Uint8 ms)
{
	t_ms += ms;
}

static void MAIN_global_var_init(){
	// Initialisation de la variable global
	global.debug.mode_best_effort_enable = FALSE;
	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	global.absolute_time = 0;
}

static void MAIN_sensor_test(){
	/*static bool_e led_on = FALSE;
	if(QS_WHO_AM_I_get() == SMALL_ROBOT){
		if(ADC_getValue(SCAN_CUP_SENSOR_LEFT) > 5 || ADC_getValue(SCAN_CUP_SENSOR_LEFT) < 50
				|| ADC_getValue(SCAN_CUP_SENSOR_RIGHT) > 5 || ADC_getValue(SCAN_CUP_SENSOR_RIGHT) < 50){
			if(led_on == FALSE){
				IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, ON});
				led_on = TRUE;
			}
		}else if(led_on == TRUE){
			IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, OFF});
			led_on = FALSE;
		}
	}*/
}
