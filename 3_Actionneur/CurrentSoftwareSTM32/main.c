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
#include "terminal/term_io.h"
#include "queue.h"
#include "clock.h"
#include "ActManager.h"
#include "Can_msg_processing.h"
#include "QS/QS_IHM.h"

#ifdef I_AM_ROBOT_BIG
	#include "Pierre/TestServo.h"
#endif


static void MAIN_onButton0();
static void MAIN_onButton1();
static void MAIN_onButton2();
static void MAIN_onButton3();
static void MAIN_onButton4();
static void MAIN_global_var_init();

static void test() {
	debug_printf("pass direct cddcd\n");
}


static void longpush() {
	debug_printf("pass longpush cddcd\n");
}

int main (void)
{
	Sint8 lastSwitchState[2] = {-1, -1};
	CAN_msg_t msg;
	/*-------------------------------------
		Démarrage
	-------------------------------------*/

	//Initialisation du système
	SYS_init();				// Init système
	PORTS_init();			// Config des ports
	MAIN_global_var_init();	// Init variable globale

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
			msg.data[0] = SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME;
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
	#ifdef I_AM_ROBOT_BIG
	   TEST_SERVO_init();
	#endif

	BUTTONS_define_actions(BUTTON0, &MAIN_onButton0, NULL, 1);
	BUTTONS_define_actions(BUTTON1, &MAIN_onButton1, NULL, 1);
	BUTTONS_define_actions(BUTTON2, &MAIN_onButton2, NULL, 1);
	BUTTONS_define_actions(BUTTON3, &MAIN_onButton3, NULL, 1);
	BUTTONS_define_actions(BUTTON4, &MAIN_onButton4, NULL, 1);

	debug_printf("---   ACT Ready    ---\n");


	IHM_define_act_button(BP_3_IHM,&test,&longpush);
	IHM_define_act_button(BP_4_IHM,&test,&longpush);

	while(1)
	{
		/*-------------------------------------
			Gestion des DELs, boutons, etc
		-------------------------------------*/

		//Switch choix de l'actionneur testé par les boutons: affichage de l'état s'il a changé
		if(lastSwitchState[0] != SWITCH_RG0) {
			lastSwitchState[0] = SWITCH_RG0;
		}
		if(lastSwitchState[1] != SWITCH_RG1) {
			lastSwitchState[1] = SWITCH_RG1;
		}

		toggle_led(LED_USER);

		QUEUE_run();
		BUTTONS_update();

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
				uart_checker(UART1_get_next_msg());
			}
		#endif

	}//Endloop
	return 0;
}


#define SERVO 0
#ifdef I_AM_ROBOT_BIG
static void MAIN_onButton0() {

	CAN_msg_t msg;
	msg.sid = IHM_SET_LED;
	msg.data[0] = ON << 5 | LED_0_IHM;
	msg.data[1] = BLINK_1HZ << 5 | LED_1_IHM;
	msg.data[2] = ON << 5 | LED_2_IHM;
	msg.data[3] = BLINK_1HZ << 5 | LED_3_IHM;
	msg.data[4] = ON << 5 | LED_4_IHM;
	msg.data[5] = BLINK_1HZ << 5 | LED_5_IHM;
	msg.data[6] = ON << 5 | LED_UP_IHM;
	msg.data[7] = LED_COLOR_MAGENTA << 5 | LED_COLOR_IHM;
	msg.size = 8;
	CAN_send(&msg);

	msg.sid = IHM_GET_SWITCH;
	msg.data[0] = SWITCH0_IHM;
	msg.data[1] = SWITCH6_IHM;
	msg.data[2] = SWITCH5_IHM;
	msg.data[3] = BLINK_1HZ << 5 | LED_3_IHM;
	msg.data[4] = ON << 5 | LED_4_IHM;
	msg.data[5] = BLINK_1HZ << 5 | LED_5_IHM;
	msg.data[6] = ON << 5 | LED_UP_IHM;
	msg.data[7] = LED_COLOR_MAGENTA << 5 | LED_COLOR_IHM;
	msg.size = 3;
	CAN_send(&msg);


//	SERVO_set_speed(SERVO_VERY_SLOW,SERVO);

//	static Uint8 state = 0;
//	if(state == 0)
//		SERVO_set_cmd(0,SERVO);
//	else if(state == 1)
//		SERVO_set_cmd(500,SERVO);
//	else if(state == 2)
//		SERVO_set_cmd(1000,SERVO);

//	state = (state == 2)? 0 : state + 1;
}

static void MAIN_onButton1() {
}

static void MAIN_onButton2() {
}

static void MAIN_onButton3() {
}

static void MAIN_onButton4() {
}

#else // ROBOT_SMALL

static void MAIN_onButton0() {
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = ACT_PINCE_GAUCHE;

	if(state == 0)
		msg.data[0] = ACT_PINCE_GAUCHE_IDLE	;
	else if(state == 1)
		msg.data[0] = ACT_PINCE_GAUCHE_CLOSED;
	else if(state == 2)
		msg.data[0] = ACT_PINCE_GAUCHE_OPEN;

	CAN_process_msg(&msg);
	state = (state == 2)? 0 : state + 1;
}

static void MAIN_onButton1() {
}

static void MAIN_onButton2() {
}

static void MAIN_onButton3() {
}

static void MAIN_onButton4() {
}
#endif // ROBOT_BIG et ROBOT_SMALL

static void MAIN_global_var_init(){
	// Initialisation de la variable global
	global.match_started = FALSE;
	global.match_over = FALSE;
	global.alim = FALSE;
	global.alim_value = 0;
	global.absolute_time = 0;
}
