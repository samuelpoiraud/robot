/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : main.c
 *	Package : Actionneur
 *	Description : Ordonnanceur de la carte actionneur
 *  Auteurs : Aurélien
 *  Version 20110225
 */

#include "QS/QS_all.h"
#include "QS/QS_sys.h"
#include "QS/QS_buttons.h"
#include "QS/QS_ports.h"
#include "QS/QS_adc.h"
#include "QS/QS_uart.h"
#include "QS/QS_pwm.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_ax12.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_servo.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_ax12.h"
#include "QS/QS_CapteurCouleurCW.h"
#include "QS/QS_can_verbose.h"
#include "QS/QS_IHM.h"
#include "QS/QS_systick.h"
#include "terminal/term_io.h"
#include "queue.h"
#include "clock.h"
#include "ActManager.h"
#include "Can_msg_processing.h"

#ifdef I_AM_ROBOT_BIG

#else

#endif


static void MAIN_onButton0();
static void MAIN_onButton1();
static void MAIN_onButton2();
static void MAIN_onButton3();
static void MAIN_onButton4();
static void MAIN_onButton5();
static void MAIN_onButton0LongPush();
static void MAIN_onButton1LongPush();
static void MAIN_onButton2LongPush();
static void MAIN_onButton3LongPush();
static void MAIN_onButton4LongPush();
static void MAIN_onButton5LongPush();

static void MAIN_global_var_init();
static void MAIN_sensor_test();

int main (void)
{
	CAN_msg_t msg;
	/*-------------------------------------
		Démarrage
	-------------------------------------*/

	//Initialisation du système
	SYS_init();				// Init système
	MAIN_global_var_init();	// Init variable globale
	SYSTICK_init((time32_t*)&(global.absolute_time));

	#ifndef FDP_2016
		PORTS_init();	// Config des ports
	#else
		if(PORTS_secure_init() == FALSE){
			error_printf("Blocage car le code ne démarre pas sur le bon slot !\n");
			while(1);
		}
	#endif

	PORTS_pwm_init();

	GPIO_SetBits(LED_RUN);
	GPIO_ResetBits(LED_USER);
	GPIO_SetBits(LED_CAN);

	// Initialisation des périphériques
	CAN_process_init();
	UART_init();
	TIMER_init();
	CLOCK_init();
	QUEUE_init();
	BUTTONS_init();

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//Détermine le robot sur lequel est branchée la carte.
	debug_printf("--- Hello, I'm ACT (%s) ---\n", QS_WHO_AM_I_get_name());

	#if defined(I_AM_ROBOT_BIG)
		#define ROBOT_CODE_NAME	"Holly"
		if(QS_WHO_AM_I_get() != BIG_ROBOT)
	#elif defined(I_AM_ROBOT_SMALL)
		#define ROBOT_CODE_NAME	"Wood"
		if(QS_WHO_AM_I_get() != SMALL_ROBOT)
	#endif	// Pour changer le code du robot aller dans : "config/config_global.h"
		{
			debug_printf("ATTENTION ! Vous avez programmé la carte actionneur de %s avec le code de %s\n", QS_WHO_AM_I_get_name(), ROBOT_CODE_NAME);
			msg.sid = STRAT_ACT_SELFTEST_DONE;
			msg.data.strat_act_selftest_done.error_code[0] = SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME;
			msg.size = 1;
			CAN_send(&msg);
			GPIO_SetBits(LED_ERROR);
			GPIO_SetBits(LED_RUN);
			GPIO_SetBits(LED_ORANGE);
			while(1);	//On plante le code ici.
		}

	#undef ROBOT_CODE_NAME

	//Init actioneurs
	ACTMGR_init();
	TERMINAL_init();

	IHM_define_act_button(BP_0_IHM, &MAIN_onButton0, &MAIN_onButton0LongPush);
	IHM_define_act_button(BP_1_IHM, &MAIN_onButton1, &MAIN_onButton1LongPush);
	IHM_define_act_button(BP_2_IHM, &MAIN_onButton2, &MAIN_onButton2LongPush);
	IHM_define_act_button(BP_3_IHM, &MAIN_onButton3, &MAIN_onButton3LongPush);
	IHM_define_act_button(BP_4_IHM, &MAIN_onButton4, &MAIN_onButton4LongPush);
	IHM_define_act_button(BP_5_IHM, &MAIN_onButton5, &MAIN_onButton5LongPush);


	debug_printf("---   ACT Ready    ---\n");

	// Demande des états initiaux des switchs
	CAN_send_sid(IHM_GET_SWITCH);

	while(1)
	{
		/*-------------------------------------
			Gestion des DELs, boutons, etc
		-------------------------------------*/

		toggle_led(LED_USER);

		QUEUE_run();
		BUTTONS_update();
		MAIN_sensor_test();

		#ifdef I_AM_ROBOT_BIG

		#else

		#endif

		/*-------------------------------------
			Réception CAN et exécution
		-------------------------------------*/
		while(CAN_data_ready()){
			// Réception et acquittement
			toggle_led(LED_CAN);
			msg = CAN_get_next_msg();
			CAN_process_msg(&msg);		// Traitement du message pour donner les consignes à la machine d'état
			#ifdef CAN_VERBOSE_MODE
				QS_CAN_VERBOSE_can_msg_print(&msg, VERB_INPUT_MSG);
			#endif
		}

		#ifdef USE_UART
			while(UART1_data_ready()){
				TERMINAL_uart_checker(UART1_get_next_msg());
			}
		#endif

	}//Endloop
	return 0;
}


#ifdef I_AM_ROBOT_BIG
static void MAIN_onButton0() {

	 static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = ACT_FISH_BRUSH_BACK;


	if(state == 0){
		msg.data.act_msg.order = ACT_FISH_BRUSH_BACK_OPEN;
	}else if(state == 1){
		msg.data.act_msg.order = ACT_FISH_BRUSH_BACK_CLOSE;
	}

	CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton0LongPush() {}

static void MAIN_onButton1() {}

static void MAIN_onButton1LongPush() {}

static void MAIN_onButton2() {}

static void MAIN_onButton2LongPush() {}

static void MAIN_onButton3() {}

static void MAIN_onButton3LongPush() {}

static void MAIN_onButton4() {}

static void MAIN_onButton4LongPush() {}

static void MAIN_onButton5() {}

static void MAIN_onButton5LongPush() {}


#else // ROBOT_SMALL

static void MAIN_onButton0(){
	/* EXEMPLE
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 2;
	msg.sid = ACT_POMPE_WOOD;
	msg.data.act_msg.act_data.act_optionnal_data[0] = 100;


	if(state == 0){
		msg.data.act_msg.order = ACT_POMPE_WOOD_NORMAL;
	}else if(state == 1){
		msg.data.act_msg.order = ACT_POMPE_WOOD_REVERSE;
	}else if(state == 2){
		msg.data.act_msg.order = ACT_POMPE_WOOD_STOP;
	}

	CAN_process_msg(&msg);
	state = (state == 2)? 0 : state + 1;*/
}
static void MAIN_onButton0LongPush(){}

static void MAIN_onButton1(){}

static void MAIN_onButton1LongPush(){}

static void MAIN_onButton2(){}

static void MAIN_onButton2LongPush(){}

static void MAIN_onButton3(){}

static void MAIN_onButton3LongPush(){}

static void MAIN_onButton4(){}

static void MAIN_onButton4LongPush(){}

static void MAIN_onButton5(){}

static void MAIN_onButton5LongPush(){}

#endif // ROBOT_BIG et ROBOT_SMALL

static void MAIN_global_var_init(){
	// Initialisation de la variable global
	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	global.flags.alim = FALSE;
	global.alim_value = 0;
	global.absolute_time = 0;
}

static void MAIN_sensor_test(){
	//static bool_e led_on = FALSE;
#ifdef I_AM_ROBOT_BIG
	/* EXEMPLE
	 if(CUP_NIPPER_FDC || ELEVATOR_FDC){
		if(led_on == FALSE){
			IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, ON});
			led_on = TRUE;
		}
	}else if(led_on == TRUE){
		IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, OFF});
		led_on = FALSE;
	}*/
#else

#endif
}
