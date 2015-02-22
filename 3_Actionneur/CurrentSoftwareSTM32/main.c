/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : main.c
 *	Package : Actionneur
 *	Description : Ordonnanceur de la carte actionneur
 *  Auteurs : Aur�lien
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
	#include "Holly/Ascenseur/elevator.h"
#else

#endif


static void MAIN_onButton0();
static void MAIN_onButton1();
static void MAIN_onButton2();
static void MAIN_onButton3();
static void MAIN_onButton4();
static void MAIN_global_var_init();

int main (void)
{
	Sint8 lastSwitchState[2] = {-1, -1};
	CAN_msg_t msg;
	/*-------------------------------------
		D�marrage
	-------------------------------------*/

	//Initialisation du syst�me
	SYS_init();				// Init syst�me
	SYSTICK_init((time32_t*)&(global.absolute_time));
	PORTS_init();			// Config des ports
	PORTS_pwm_init();
	MAIN_global_var_init();	// Init variable globale

	GPIO_SetBits(LED_RUN);
	GPIO_ResetBits(LED_USER);
	GPIO_SetBits(LED_CAN);

	// Initialisation des p�riph�riques
	CAN_process_init();
	UART_init();
	TIMER_init();
	CLOCK_init();
	QUEUE_init();
	BUTTONS_init();

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//D�termine le robot sur lequel est branch�e la carte.
	debug_printf("--- Hello, I'm ACT (%s) ---\n", QS_WHO_AM_I_get_name());

	#if defined(I_AM_ROBOT_BIG)
		#define ROBOT_CODE_NAME	"Holly"
		if(QS_WHO_AM_I_get() != BIG_ROBOT)
	#elif defined(I_AM_ROBOT_SMALL)
		#define ROBOT_CODE_NAME	"Wood"
		if(QS_WHO_AM_I_get() != SMALL_ROBOT)
	#endif	// Pour changer le code du robot aller dans : "config/config_global.h"
		{
			debug_printf("ATTENTION ! Vous avez programm� la carte actionneur de %s avec le code de %s\n", QS_WHO_AM_I_get_name(), ROBOT_CODE_NAME);
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
	TERMINAL_init();

	BUTTONS_define_actions(BUTTON0, &MAIN_onButton0, NULL, 1);
	BUTTONS_define_actions(BUTTON1, &MAIN_onButton1, NULL, 1);
	BUTTONS_define_actions(BUTTON2, &MAIN_onButton2, NULL, 1);
	BUTTONS_define_actions(BUTTON3, &MAIN_onButton3, NULL, 1);
	BUTTONS_define_actions(BUTTON4, &MAIN_onButton4, NULL, 1);

	debug_printf("---   ACT Ready    ---\n");

	// Demande des �tats initiaux des switchs
	CAN_send_sid(IHM_GET_SWITCH);

	while(1)
	{
		/*-------------------------------------
			Gestion des DELs, boutons, etc
		-------------------------------------*/

		//Switch choix de l'actionneur test� par les boutons: affichage de l'�tat s'il a chang�
		if(lastSwitchState[0] != SWITCH_RG0) {
			lastSwitchState[0] = SWITCH_RG0;
		}
		if(lastSwitchState[1] != SWITCH_RG1) {
			lastSwitchState[1] = SWITCH_RG1;
		}

		toggle_led(LED_USER);

		QUEUE_run();
		BUTTONS_update();

		#ifdef I_AM_ROBOT_BIG
			ELEVATOR_state_machine();
		#else

		#endif

		/*-------------------------------------
			R�ception CAN et ex�cution
		-------------------------------------*/
		while(CAN_data_ready()){
			// R�ception et acquittement
			toggle_led(LED_CAN);
			msg = CAN_get_next_msg();
			CAN_process_msg(&msg);		// Traitement du message pour donner les consignes � la machine d'�tat
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
	msg.sid = ACT_PINCEMI_LEFT;

	if(state == 0)
		msg.data[0] = ACT_PINCEMI_LEFT_CLOSE;
	else if(state == 1)
		msg.data[0] = ACT_PINCEMI_LEFT_OPEN;

	CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton1() {
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = ACT_PINCEMI_RIGHT;

	if(state == 0)
		msg.data[0] = ACT_PINCEMI_RIGHT_CLOSE;
	else if(state == 1)
		msg.data[0] = ACT_PINCEMI_RIGHT_OPEN;

	CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton2() {
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = ACT_STOCK_LEFT;

	if(state == 0)
		msg.data[0] = ACT_STOCK_LEFT_CLOSE;
	else if(state == 1)
		msg.data[0] = ACT_STOCK_LEFT_OPEN;

	CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton3() {
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = ACT_STOCK_RIGHT;

	if(state == 0)
		msg.data[0] = ACT_STOCK_RIGHT_CLOSE;
	else if(state == 1)
		msg.data[0] = ACT_STOCK_RIGHT_OPEN;

	CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
}

static void MAIN_onButton4() {
	static Uint8 state = 0;
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = ACT_CUP_NIPPER;

	if(state == 0)
		msg.data[0] = ACT_CUP_NIPPER_CLOSE;
	else if(state == 1)
		msg.data[0] = ACT_CUP_NIPPER_OPEN;

	CAN_process_msg(&msg);
	state = (state == 1)? 0 : state + 1;
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
