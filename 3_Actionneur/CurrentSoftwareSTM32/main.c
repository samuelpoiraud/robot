/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : main.c
 *	Package : Actionneur
 *	Description : Ordonnanceur de la carte actionneur
 *  Auteurs : Aur√©lien
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
#include "QS/QS_mosfet.h"
#include "terminal/term_io.h"
#include "queue.h"
#include "clock.h"
#include "ActManager.h"
#include "Can_msg_processing.h"
#include "mosfetBoard.h"
#include "colorSensor.h"

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
		D√©marrage
	-------------------------------------*/

	//Initialisation du systeme
	SYS_init();				// Init syst√®me
	MAIN_global_var_init();	// Init variable globale
	SYSTICK_init((time32_t*)&(global.absolute_time));

	#ifdef DISABLE_SECURE_GPIO_INIT
		PORTS_init();	// Config des ports
	#else
		if(PORTS_secure_init() == FALSE){
			error_printf("Blocage car le code ne dÈmarre pas sur le bon slot !\n");
			while(1);
		}
	#endif

	PWM_init();
	PORTS_pwm_init();

	GPIO_SetBits(LED_RUN);
	GPIO_ResetBits(LED_USER);
	GPIO_SetBits(LED_CAN);

	// Initialisation des peripheriques
	CAN_process_init();
	UART_init();
	TIMER_init();
	CLOCK_init();
	QUEUE_init();
	BUTTONS_init();
	IHM_init(&global.flags.match_started);

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//DÈtermine le robot sur lequel est branchÈe la carte.
	debug_printf("--- Hello, I'm ACT (%s) ---\n", QS_WHO_AM_I_get_name());

	#if defined(I_AM_ROBOT_BIG)
		#define ROBOT_CODE_NAME	"Harry"
		if(QS_WHO_AM_I_get() != BIG_ROBOT)
	#elif defined(I_AM_ROBOT_SMALL)
		#define ROBOT_CODE_NAME	"Anne"
		if(QS_WHO_AM_I_get() != SMALL_ROBOT)
	#endif	// Pour changer le code du robot aller dans : "config/config_global.h"
		{
			debug_printf("ATTENTION ! Vous avez programm√© la carte actionneur de %s avec le code de %s\n", QS_WHO_AM_I_get_name(), ROBOT_CODE_NAME);
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

	#ifdef USE_MOSTFET_BOARD
		MOSFET_BOARD_init();
	#endif

	#ifdef USE_I2C_COLOR_SENSOR
		COLOR_SENSOR_init();
	#endif

	IHM_define_act_button(BP_0_IHM, &MAIN_onButton0, &MAIN_onButton0LongPush);
	IHM_define_act_button(BP_1_IHM, &MAIN_onButton1, &MAIN_onButton1LongPush);
	IHM_define_act_button(BP_2_IHM, &MAIN_onButton2, &MAIN_onButton2LongPush);
	IHM_define_act_button(BP_3_IHM, &MAIN_onButton3, &MAIN_onButton3LongPush);
	IHM_define_act_button(BP_4_IHM, &MAIN_onButton4, &MAIN_onButton4LongPush);
	IHM_define_act_button(BP_5_IHM, &MAIN_onButton5, &MAIN_onButton5LongPush);

	debug_printf("---   ACT Ready    ---\n");

	// Demande des Ètats initiaux des switchs
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
			R√©ception CAN et ex√©cution
		-------------------------------------*/
		while(CAN_data_ready()){
			// R√©ception et acquittement
			toggle_led(LED_CAN);
			msg = CAN_get_next_msg();
			CAN_process_msg(&msg);		// Traitement du message pour donner les consignes √  la machine d'√©tat
			#ifdef CAN_VERBOSE_MODE
				QS_CAN_VERBOSE_can_msg_print(&msg, VERB_INPUT_MSG);
			#endif
			//ACTMNG_control_act();

		}

		#ifdef USE_UART1
			while(UART1_data_ready()){
				TERMINAL_uart_checker(UART1_get_next_msg());
			}
		#endif


		#if defined(USE_MOSFETS_MODULE) && defined(USE_MOSFET_MULTI)
			MOSFET_do_order_multi(NULL);
		#endif

		#ifdef USE_MOSTFET_BOARD
			MOSFET_BOARD_processMain();
		#endif

		#ifdef USE_I2C_COLOR_SENSOR
			COLOR_SENSOR_processMain();
		#endif

		OUTPUTLOG_process_main();

	}//Endloop
	return 0;
}


#ifdef I_AM_ROBOT_BIG
// prise de cylindre droite
static void MAIN_onButton0() {
	static Uint8 state = 0;
	CAN_msg_t msg1;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_ORE_ROLLER_ARM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_ARM_OUT;
	}else if(state == 1){
		msg1.sid = ACT_ORE_WALL;
		msg1.data.act_msg.order = ACT_ORE_WALL_OUT;
	}else if(state == 2){
	msg1.sid = ACT_ORE_ROLLER_FOAM;
	msg1.data.act_msg.order = ACT_ORE_ROLLER_FOAM_RUN;
	msg1.data.act_msg.act_data.act_order.act_optionnal_data[0] = (Uint16)(510);
	}else if(state == 3){
		msg1.sid = ACT_ORE_ROLLER_FOAM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_FOAM_STOP;
	}else if(state == 4){
		msg1.sid = ACT_ORE_WALL;
		msg1.data.act_msg.order = ACT_ORE_WALL_IN;
	}else if(state == 5){
		msg1.sid = ACT_ORE_ROLLER_ARM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_ARM_IN;
	}

	if(msg1.sid != 0)
		CAN_process_msg(&msg1);
	state = (state == 5)? 0 : state + 1;
}

// depose balle tire
static void MAIN_onButton0LongPush(){
	static Uint8 state = 0;
	CAN_msg_t msg1;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_ORE_GUN;
		msg1.data.act_msg.order = ACT_ORE_GUN_DOWN;
	}else if(state == 1){
		msg1.sid = ACT_TURBINE;
		msg1.data.act_msg.order = ACT_TURBINE_NORMAL;
	}else if(state == 2){
		msg1.sid = ACT_ORE_TRIHOLE;
		msg1.data.act_msg.order = ACT_ORE_TRIHOLE_RUN;
		msg1.data.act_msg.act_data.act_order.act_optionnal_data[0] = 10;
	}else if(state == 3){
		msg1.sid = ACT_ORE_ROLLER_ARM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_ARM_DEPOSE;
	}else if(state == 4){
		msg1.sid = ACT_ORE_ROLLER_FOAM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_FOAM_RUN;
		msg1.data.act_msg.act_data.act_order.act_optionnal_data[0] = (Uint16)(-510);
	}else if(state == 5){
		msg1.sid = ACT_ORE_TRIHOLE;
		msg1.data.act_msg.order = ACT_ORE_TRIHOLE_STOP;
	}else if(state == 6){
		msg1.sid = ACT_TURBINE;
		msg1.data.act_msg.order = ACT_TURBINE_STOP;
	}else if(state == 7){
		msg1.sid = ACT_ORE_GUN;
		msg1.data.act_msg.order = ACT_ORE_GUN_UP;
	}

	if(msg1.sid != 0)
		CAN_process_msg(&msg1);
	state = (state == 5)? 0 : state + 1;
}

// prise de cylindre gauche
static void MAIN_onButton1() {
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_CYLINDER_ELEVATOR_LEFT;
		msg.data.act_msg.order = ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER;
	}else if(state == 1){
		msg.sid = ACT_POMPE_SLIDER_LEFT;
		msg.data.act_msg.order = ACT_POMPE_NORMAL;
	}else if(state == 2){
		msg.sid = ACT_CYLINDER_SLIDER_LEFT;
		msg.data.act_msg.order = ACT_CYLINDER_SLIDER_LEFT_OUT;
	}else if(state == 3){
		msg.sid = ACT_CYLINDER_ELEVATOR_LEFT;
		msg.data.act_msg.order = ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM;
	}else if(state == 4){
		msg.sid = ACT_POMPE_ELEVATOR_LEFT;
		msg.data.act_msg.order = ACT_POMPE_NORMAL;
	}else if(state == 5){
		msg.sid = ACT_CYLINDER_SLIDER_LEFT;
		msg.data.act_msg.order = ACT_CYLINDER_SLIDER_LEFT_IN;
	}else if(state == 6){
		msg.sid = ACT_POMPE_SLIDER_LEFT;
		msg.data.act_msg.order = ACT_POMPE_STOP;
	}else if(state == 7){
		msg.sid = ACT_CYLINDER_ELEVATOR_LEFT;
		msg.data.act_msg.order = ACT_CYLINDER_ELEVATOR_LEFT_TOP;
	}else if(state == 8){
		msg.sid = ACT_CYLINDER_SLOPE_LEFT;
		msg.data.act_msg.order = ACT_CYLINDER_SLOPE_LEFT_UP;
	}else if(state == 9){
		msg.sid = ACT_POMPE_ELEVATOR_LEFT;
		msg.data.act_msg.order = ACT_POMPE_STOP;
	}else if(state == 10){
		msg.sid = ACT_CYLINDER_ELEVATOR_LEFT;
		msg.data.act_msg.order = ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM;
	}else if(state == 11){
		msg.sid = ACT_CYLINDER_SLOPE_LEFT;
		msg.data.act_msg.order = ACT_CYLINDER_SLOPE_LEFT_DOWN;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 11)? 0 : state + 1;
}

static void MAIN_onButton1LongPush() {}

// depose cylindre droite
static void MAIN_onButton2() {
	static Uint8 state = 0;
	CAN_msg_t msg1;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_CYLINDER_ARM_RIGHT ;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_RIGHT_PREPARE_TO_TAKE;
	}else if(state == 1){
		msg1.sid = ACT_CYLINDER_DISPOSE_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_DISPOSE_RIGHT_TAKE;
	}else if(state == 1){
		msg1.sid = ACT_CYLINDER_BALANCER_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_BALANCER_RIGHT_IN;
	}else if(state == 2){
		msg1.sid = ACT_CYLINDER_BALANCER_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_BALANCER_RIGHT_OUT;
	}else if(state == 3){
		msg1.sid = ACT_CYLINDER_ARM_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_RIGHT_TAKE;
	}else if(state == 4){
		msg1.sid = ACT_POMPE_DISPOSE_RIGHT;
		msg1.data.act_msg.order = ACT_POMPE_NORMAL;
	}else if(state == 5){
		msg1.sid = ACT_CYLINDER_DISPOSE_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_DISPOSE_RIGHT_RAISE;
	}else if(state == 6){
		msg1.sid = ACT_CYLINDER_ARM_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_RIGHT_DISPOSE;
	}else if(state == 7){
		msg1.sid = ACT_CYLINDER_DISPOSE_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE;
	}else if(state == 8){
		msg1.sid = ACT_POMPE_DISPOSE_RIGHT;
		msg1.data.act_msg.order = ACT_POMPE_STOP;
	}else if(state == 9){
		msg1.sid = ACT_CYLINDER_DISPOSE_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_DISPOSE_RIGHT_TAKE;
	}else if(state == 10){
		msg1.sid = ACT_CYLINDER_ARM_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_RIGHT_TAKE;
	}else if(state == 11){
		msg1.sid = ACT_CYLINDER_ARM_RIGHT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_RIGHT_IN;
	}
	if(msg1.sid != 0)
		CAN_process_msg(&msg1);
	state = (state == 11)? 0 : state + 1;
}

static void MAIN_onButton2LongPush() {}

// depose cylindre gauche
static void MAIN_onButton3() {
	static Uint8 state = 0;
	CAN_msg_t msg1;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_CYLINDER_ARM_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_LEFT_PREPARE_TO_TAKE;
	}else if(state == 1){
		msg1.sid = ACT_CYLINDER_DISPOSE_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_DISPOSE_LEFT_TAKE;
	}else if(state == 1){
		msg1.sid = ACT_CYLINDER_BALANCER_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_BALANCER_LEFT_IN;
	}else if(state == 2){
		msg1.sid = ACT_CYLINDER_BALANCER_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_BALANCER_LEFT_OUT;
	}else if(state == 3){
		msg1.sid = ACT_CYLINDER_ARM_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_LEFT_TAKE;
	}else if(state == 4){
		msg1.sid = ACT_POMPE_DISPOSE_LEFT;
		msg1.data.act_msg.order = ACT_POMPE_NORMAL;
	}else if(state == 5){
		msg1.sid = ACT_CYLINDER_DISPOSE_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_DISPOSE_LEFT_RAISE;
	}else if(state == 6){
		msg1.sid = ACT_CYLINDER_ARM_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_LEFT_DISPOSE;
	}else if(state == 7){
		msg1.sid = ACT_CYLINDER_DISPOSE_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_DISPOSE_LEFT_DISPOSE;
	}else if(state == 8){
		msg1.sid = ACT_POMPE_DISPOSE_LEFT;
		msg1.data.act_msg.order = ACT_POMPE_STOP;
	}else if(state == 9){
		msg1.sid = ACT_CYLINDER_DISPOSE_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_DISPOSE_LEFT_TAKE;
	}else if(state == 10){
		msg1.sid = ACT_CYLINDER_ARM_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_LEFT_TAKE;
	}else if(state == 11){
		msg1.sid = ACT_CYLINDER_ARM_LEFT;
		msg1.data.act_msg.order = ACT_CYLINDER_ARM_LEFT_IN;
	}
	if(msg1.sid != 0)
		CAN_process_msg(&msg1);
	state = (state == 11)? 0 : state + 1;
}

static void MAIN_onButton3LongPush() {}

// prise de balle
static void MAIN_onButton4(){
	static Uint8 state = 0;
	CAN_msg_t msg1;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_ORE_ROLLER_ARM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_ARM_OUT;
	}else if(state == 1){
		msg1.sid = ACT_ORE_WALL;
		msg1.data.act_msg.order = ACT_ORE_WALL_OUT;
	}else if(state == 2){
		msg1.sid = ACT_ORE_ROLLER_FOAM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_FOAM_RUN;
		msg1.data.act_msg.act_data.act_order.act_optionnal_data[0] = ACT_ORE_ROLLER_FOAM_SPEED_RUN;
	}else if(state == 3){
		msg1.sid = ACT_ORE_ROLLER_FOAM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_FOAM_STOP;
	}else if(state == 4){
		msg1.sid = ACT_ORE_WALL;
		msg1.data.act_msg.order = ACT_ORE_WALL_IN;
	}else if(state == 5){
		msg1.sid = ACT_ORE_ROLLER_ARM;
		msg1.data.act_msg.order = ACT_ORE_ROLLER_ARM_IN;
	}

	if(msg1.sid != 0)
		CAN_process_msg(&msg1);
	state = (state == 5)? 0 : state + 1;
}

// depose balle tire
static void MAIN_onButton4LongPush(){
static Uint8 state = 0;
	CAN_msg_t msg1;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_ORE_GUN;
		msg1.data.act_msg.order = ACT_ORE_GUN_DOWN;
	}else if(state == 1){
		msg1.sid = ACT_TURBINE;
		msg1.data.act_msg.order = ACT_TURBINE_NORMAL;
	}else if(state == 2){
		msg1.sid = ACT_ORE_TRIHOLE;
		msg1.data.act_msg.order = ACT_ORE_TRIHOLE_RUN;
		msg1.data.act_msg.act_data.act_order.act_optionnal_data[0] = ACT_ORE_TRIHOLE_SPEED_RUN;
	}else if(state == 3){
		msg1.sid = ACT_ORE_TRIHOLE;
		msg1.data.act_msg.order = ACT_ORE_TRIHOLE_STOP;
	}else if(state == 4){
		msg1.sid = ACT_TURBINE;
		msg1.data.act_msg.order = ACT_TURBINE_STOP;
	}else if(state == 5){
		msg1.sid = ACT_ORE_GUN;
		msg1.data.act_msg.order = ACT_ORE_GUN_UP;
	}

	if(msg1.sid != 0)
		CAN_process_msg(&msg1);
	state = (state == 5)? 0 : state + 1;
}

//roue en position passage cratere
static void MAIN_onButton5() {
	static Uint8 state = 0;
	CAN_msg_t msg1, msg2, msg3, msg4;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;
	msg2.size = 8;
	msg2.data.act_msg.act_data.act_order.run_now = TRUE;
	msg3.size = 8;
	msg3.data.act_msg.act_data.act_order.run_now = TRUE;
	msg4.size = 8;
	msg4.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_BEARING_BALL_WHEEL;
		msg1.data.act_msg.order = ACT_BEARING_BALL_WHEEL_DOWN;
	}else if(state == 1){
		msg1.sid = ACT_BIG_BALL_FRONT_LEFT ;
		msg1.data.act_msg.order = ACT_BIG_BALL_FRONT_LEFT_UP;

		msg2.sid = ACT_BIG_BALL_FRONT_RIGHT ;
		msg2.data.act_msg.order = ACT_BIG_BALL_FRONT_RIGHT_UP;

		msg3.sid = ACT_BIG_BALL_BACK_LEFT ;
		msg3.data.act_msg.order = ACT_BIG_BALL_BACK_LEFT_UP;

		msg4.sid = ACT_BIG_BALL_BACK_RIGHT;
		msg4.data.act_msg.order = ACT_BIG_BALL_BACK_RIGHT_UP;
	}else if(state == 2){
		msg1.sid = ACT_BIG_BALL_FRONT_LEFT ;
		msg1.data.act_msg.order = ACT_BIG_BALL_FRONT_LEFT_DOWN;

		msg2.sid = ACT_BIG_BALL_FRONT_RIGHT ;
		msg2.data.act_msg.order = ACT_BIG_BALL_FRONT_RIGHT_DOWN;

		msg3.sid = ACT_BIG_BALL_BACK_LEFT ;
		msg3.data.act_msg.order = ACT_BIG_BALL_BACK_LEFT_DOWN;

		msg4.sid = ACT_BIG_BALL_BACK_RIGHT;
		msg4.data.act_msg.order = ACT_BIG_BALL_BACK_RIGHT_DOWN;
	}else if(state == 3){
		msg1.sid = ACT_BEARING_BALL_WHEEL;
		msg1.data.act_msg.order = ACT_BEARING_BALL_WHEEL_UP;
	}

	if(msg1.sid != 0)
		CAN_process_msg(&msg1);
	if(msg2.sid != 0)
		CAN_process_msg(&msg2);
	if(msg3.sid != 0)
		CAN_process_msg(&msg3);
	if(msg4.sid != 0)
		CAN_process_msg(&msg4);
	state = (state == 3)? 0 : state + 1;
}

//lancer la fusee
static void MAIN_onButton5LongPush() {
	static Uint8 state = 0;
	CAN_msg_t msg1;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_ROCKET;
		msg1.data.act_msg.order = ACT_ROCKET_PREPARE_LAUNCH;
	}else if(state == 1){
		msg1.sid = ACT_ROCKET;
		msg1.data.act_msg.order = ACT_ROCKET_LAUNCH;
	}

	CAN_process_msg(&msg1);
	state = (state == 1)? 0 : state + 1;
}



#else // ROBOT_SMALL

static void MAIN_onButton0(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_CYLINDER_SLIDER;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLIDER_OUT;
	}else if(state == 1){
		msg.sid = ACT_SMALL_CYLINDER_SLIDER;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLIDER_ALMOST_OUT;
	}else if(state == 2){
		msg.sid = ACT_SMALL_CYLINDER_SLIDER;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLIDER_ALMOST_OUT_WITH_CYLINDER;
	}else if(state == 3){
		msg.sid = ACT_SMALL_CYLINDER_SLIDER;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLIDER_IN;
	}else if(state == 4){
		msg.sid = ACT_SMALL_CYLINDER_ELEVATOR;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_ELEVATOR_LOCK_WITH_CYLINDER;
	}else if(state == 5){
		msg.sid = ACT_SMALL_CYLINDER_ELEVATOR;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE;
	}else if(state == 6){
		msg.sid = ACT_SMALL_CYLINDER_ELEVATOR;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_ELEVATOR_TOP;
	}else if(state == 7){
		msg.sid = ACT_SMALL_CYLINDER_ELEVATOR;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM;
	}else if(state == 8){
		msg.sid = ACT_SMALL_CYLINDER_SLIDER;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLIDER_IN;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 8)? 0 : state + 1;
}

static void MAIN_onButton0LongPush(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_POMPE_PRISE;
		msg.data.act_msg.order = ACT_POMPE_NORMAL;
		msg.data.act_msg.act_data.act_order.act_optionnal_data[0] = 100;
	}else if(state == 1){
		msg.sid = ACT_SMALL_POMPE_PRISE;
		msg.data.act_msg.order = ACT_POMPE_REVERSE;
		msg.data.act_msg.act_data.act_order.act_optionnal_data[0] = 100;
	}else if(state == 2){
		msg.sid = ACT_SMALL_POMPE_PRISE;
		msg.data.act_msg.order = ACT_POMPE_STOP;
	}else if(state == 3){
		msg.sid = ACT_SMALL_POMPE_DISPOSE;
		msg.data.act_msg.order = ACT_POMPE_SMALL_ELEVATOR_NORMAL;
		msg.data.act_msg.act_data.act_order.act_optionnal_data[0] = 100;
	}else if(state == 4){
		msg.sid = ACT_SMALL_POMPE_DISPOSE;
		msg.data.act_msg.order = ACT_POMPE_SMALL_ELEVATOR_NORMAL;
		msg.data.act_msg.act_data.act_order.act_optionnal_data[0] = 100;
	}else if(state == 5){
		msg.sid = ACT_SMALL_POMPE_DISPOSE;
		msg.data.act_msg.order = ACT_POMPE_STOP;
	}


	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 5)? 0 : state + 1;
}

static void MAIN_onButton1(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_CYLINDER_SLOPE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLOPE_MID;
	}else if(state == 1){
		msg.sid = ACT_SMALL_CYLINDER_SLOPE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLOPE_ALMOST_UP;
	}else if(state == 2){
		msg.sid = ACT_SMALL_CYLINDER_SLOPE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLOPE_UP;
	}else if(state == 3){
		msg.sid = ACT_SMALL_CYLINDER_SLOPE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLOPE_VERY_UP;
	}else if(state == 4){
		msg.sid = ACT_SMALL_CYLINDER_SLOPE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_SLOPE_DOWN;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 4)? 0 : state + 1;
}

static void MAIN_onButton1LongPush(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_CYLINDER_BALANCER;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_BALANCER_OUT;
	}else if(state == 1){
		msg.sid = ACT_SMALL_CYLINDER_BALANCER;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_BALANCER_IN;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;

}

static void MAIN_onButton2(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_CYLINDER_ARM;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_ARM_PREPARE_TO_TAKE;
	}else if(state == 1){
		msg.sid = ACT_SMALL_CYLINDER_DISPOSE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_DISPOSE_TAKE;
	}else if(state == 2){
		msg.sid = ACT_SMALL_CYLINDER_ARM;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_ARM_TAKE;
	}else if(state == 3){
		msg.sid = ACT_SMALL_CYLINDER_DISPOSE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_DISPOSE_RAISE;
	}else if(state == 4){
		msg.sid = ACT_SMALL_CYLINDER_ARM;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_ARM_DISPOSE;
	}else if(state == 5){
		msg.sid = ACT_SMALL_CYLINDER_DISPOSE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_DISPOSE_DISPOSE;
	}else if(state == 6){
		msg.sid = ACT_SMALL_CYLINDER_DISPOSE;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_DISPOSE_IDLE;
	}else if(state == 7){
		msg.sid = ACT_SMALL_CYLINDER_ARM;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_ARM_IN;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 7)? 0 : state + 1;
}

static void MAIN_onButton2LongPush(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_POMPE_DISPOSE;
		msg.data.act_msg.order = ACT_POMPE_NORMAL;
		msg.data.act_msg.act_data.act_order.act_optionnal_data[0] = 100;
	}else if(state == 1){
		msg.sid = ACT_SMALL_POMPE_DISPOSE;
		msg.data.act_msg.order = ACT_POMPE_REVERSE;
		msg.data.act_msg.act_data.act_order.act_optionnal_data[0] = 100;
	}else if(state == 2){
		msg.sid = ACT_SMALL_POMPE_DISPOSE;
		msg.data.act_msg.order = ACT_POMPE_STOP;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 2)? 0 : state + 1;
}

static void MAIN_onButton3(){
	static Uint8 state = 0;
	CAN_msg_t msg1, msg2, msg3;
	msg1.size = 8;
	msg1.data.act_msg.act_data.act_order.run_now = TRUE;
	msg2.size = 8;
	msg2.data.act_msg.act_data.act_order.run_now = TRUE;
	msg3.size = 8;
	msg3.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg1.sid = ACT_SMALL_BALL_FRONT_LEFT;
		msg1.data.act_msg.order = ACT_SMALL_BALL_FRONT_LEFT_UP;
		msg2.sid = ACT_SMALL_BALL_FRONT_RIGHT;
		msg2.data.act_msg.order = ACT_SMALL_BALL_FRONT_RIGHT_UP;
		msg3.sid = ACT_SMALL_BALL_BACK;
		msg3.data.act_msg.order = ACT_SMALL_BALL_BACK_UP;
	}else if(state == 1){
		msg1.sid = ACT_SMALL_BALL_FRONT_LEFT;
		msg1.data.act_msg.order = ACT_SMALL_BALL_FRONT_LEFT_DOWN;
		msg2.sid = ACT_SMALL_BALL_FRONT_RIGHT;
		msg2.data.act_msg.order = ACT_SMALL_BALL_FRONT_RIGHT_DOWN;
		msg3.sid = ACT_SMALL_BALL_BACK;
		msg3.data.act_msg.order = ACT_SMALL_BALL_BACK_DOWN;
	}

	if(msg1.sid != 0)
		CAN_process_msg(&msg1);
	if(msg2.sid != 0)
		CAN_process_msg(&msg2);
	if(msg3.sid != 0)
		CAN_process_msg(&msg3);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton3LongPush(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_CYLINDER_COLOR;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_COLOR_NORMAL_SPEED;
	}else if(state == 1){
		msg.sid = ACT_SMALL_CYLINDER_COLOR;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_COLOR_ZERO_SPEED;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton4(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_CYLINDER_MULTIFONCTION;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH;
	}else if(state == 1){
		msg.sid = ACT_SMALL_CYLINDER_MULTIFONCTION;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_MULTIFONCTION_OUT;
	}else if(state == 2){
		msg.sid = ACT_SMALL_CYLINDER_MULTIFONCTION;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK;
	}else if(state == 3){
		msg.sid = ACT_SMALL_CYLINDER_MULTIFONCTION;
		msg.data.act_msg.order = ACT_SMALL_CYLINDER_MULTIFONCTION_IN;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 3)? 0 : state + 1;
}

static void MAIN_onButton4LongPush(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_ORE;
		msg.data.act_msg.order = ACT_SMALL_ORE_DOWN;
	}else if(state == 1){
		msg.sid = ACT_SMALL_ORE;
		msg.data.act_msg.order = ACT_SMALL_ORE_UP;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton5(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_MAGIC_ARM;
		msg.data.act_msg.order = ACT_SMALL_MAGIC_ARM_OUT;
	}else if(state == 1){
		msg.sid = ACT_SMALL_MAGIC_ARM;
		msg.data.act_msg.order = ACT_SMALL_MAGIC_ARM_IN;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton5LongPush(){
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 8;
	msg.data.act_msg.act_data.act_order.run_now = TRUE;

	if(state == 0){
		msg.sid = ACT_SMALL_MAGIC_COLOR;
		msg.data.act_msg.order = ACT_SMALL_MAGIC_COLOR_NORMAL_SPEED;
	}else if(state == 1){
		msg.sid = ACT_SMALL_MAGIC_COLOR;
		msg.data.act_msg.order = ACT_SMALL_MAGIC_COLOR_ZERO_SPEED;
	}

	if(msg.sid != 0)
		CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;

}

#endif // ROBOT_BIG et ROBOT_SMALL

static void MAIN_global_var_init(){
	// Initialisation de la variable global
	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	global.flags.power = FALSE;
	global.alim_value = 0;
	global.absolute_time = 0;
}

static void MAIN_sensor_test(){
	static bool_e led_on = FALSE;

	UNUSED_VAR(led_on);

#ifdef I_AM_ROBOT_BIG
	 if(HARRY_SLIDER_RIGHT || HARRY_SLIDER_LEFT){
		if(led_on == FALSE){
			IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, ON});
			led_on = TRUE;
		}
	}else if(led_on == TRUE){
		IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, OFF});
		led_on = FALSE;
	}
#else

#endif
}
