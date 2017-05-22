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
#include "QS/QS_hokuyo/hokuyo.h"
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
#include "gyroscope.h"
#include "detection.h"
#include "detection_choc.h"
#include "scan/scan.h"
#include "scan/borders_scan.h"
#include "scan/rotation_scan.h"


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

	// Initialisation du système
	SYS_init();				// Init système

	#ifdef DISABLE_SECURE_GPIO_INIT
		PORTS_init();	// Config des ports
	#else
		if(PORTS_secure_init() == FALSE){
			error_printf("Blocage car le code ne démarre pas sur le bon slot !\n");
			while(1);
		}
	#endif

	MAIN_global_var_init();	// Init variable globale
	SYSTICK_init((time32_t*)&(global.absolute_time));

	GPIO_SetBits(LED_RUN);

	SECRETARY_init();	//Pour recevoir tout les messages CAN envoyés très tôt...
	GPIO_ResetBits(LED_RUN);
	UART_init();

	RCC_read();

	//Doit se faire AVANT ODOMETRY_init() !!!

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//Détermine le robot sur lequel est branchée la carte.
	debug_printf("--- Hello, I'm PROP (%s) ---\n", QS_WHO_AM_I_get_name());
	TIMER_init();
	ODOMETRY_init();
	WATCHDOG_init();
	SUPERVISOR_init();
	COPILOT_init();
	PILOT_init();
	ROADMAP_init();
	WARNER_init();
	JOYSTICK_init();
	DEBUG_init();
	ADC_init();
	BUTTONS_init();

	IHM_init(&global.flags.match_started);
	IHM_define_act_button(BP_CALIBRATION_IHM,&SEQUENCES_calibrate,NULL);
#ifdef MODE_PRINT_FIRST_TRAJ
	IHM_define_act_button(BP_PRINTMATCH_IHM,&DEBUG_display,NULL);
#endif

	//BUTTONS_define_actions(BUTTON0, &SCAN_onPower, NULL, 1);

	DETECTION_init();
	#ifdef USE_HOKUYO
		HOKUYO_init();
	#endif

	#ifdef SCAN
		SCAN_init();
	#endif

	#ifdef SCAN_OBJETS
		OBJECTS_SCAN_init();
	#endif

	#ifdef SCAN_ROTATION
		SCAN_CORNER_init();
	#endif

	#ifdef USE_GYROSCOPE
		GYRO_init();
	#endif

	#ifdef DETECTION_CHOC
		DETECTION_CHOC_init();
	#endif

	IT_init();

	// Demande des états initiaux des switchs
	CAN_send_sid(IHM_GET_SWITCH);

	GPIO_SetBits(I_AM_READY);

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

		if(t_ms > 20)	//Pour éviter les rebonds
		{
			t_ms = 0;
			BUTTONS_update();			//Gestion des boutons
			//debug_printf("value = %d\n", ADC_getValue(ADC_SENSOR_FISHS));
		}

		SECRETARY_process_main();	//Communication avec l'extérieur. (Réception des messages)

		WARNER_process_main();		//Communication avec l'extérieur. (Envois des messages)

		#ifdef USE_HOKUYO
			HOKUYO_processMain();
		#endif

		DETECTION_process_main();

		MAIN_sensor_test();

		#ifdef DETECTION_CHOC
			DETECTION_CHOC_process_main();
		#endif

		OUTPUTLOG_process_main();

		#ifdef SCAN
			SCAN_process_main();
		#endif

		#ifdef SCAN_BORDURE
			BORDERS_SCAN_process_main();
		#endif

		#ifdef SCAN_ROTATION
			SCAN_CORNER_process_main();
		#endif

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
	global.absolute_time = 0;
	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	global.flags.alim = FALSE;
	global.flags.powerAvailable = FALSE;
}

static void MAIN_sensor_test(){
	static bool_e led_on = FALSE;
	if(QS_WHO_AM_I_get() == SMALL_ROBOT){
		if(ADC_getValue(ADC_SENSOR_LASER_LEFT) > 5 || ADC_getValue(ADC_SENSOR_LASER_RIGHT) < 50){
			if(led_on == FALSE){
				IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, ON});
				led_on = TRUE;
			}
		}else if(led_on == TRUE){
			IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, OFF});
			led_on = FALSE;
		}
	}
}
