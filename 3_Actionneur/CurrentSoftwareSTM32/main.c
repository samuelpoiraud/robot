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
#include "terminal/term_io.h"
#include "queue.h"
#include "clock.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_servo.h"

#ifdef I_AM_ROBOT_BIG
	#include "Pierre/TestServo.h"
#endif

#include "ActManager.h"

//Information pour le bouton 4
#include "QS/QS_ax12.h"
#include "QS/QS_CapteurCouleurCW.h"

#ifdef USE_CAN
	#include "QS/QS_can.h"
#endif
	#include "Can_msg_processing.h"
	#include "QS/QS_CANmsgList.h"
#include "config_pin.h"

static void MAIN_onButton0();
static void MAIN_onButton1();
static void MAIN_onButton2();
static void MAIN_onButton3();
static void MAIN_onButton4();

int main (void)
{
	Sint8 lastSwitchState[2] = {-1, -1};
	/*-------------------------------------
		Démarrage
	-------------------------------------*/

	//initialisations
	SYS_init();
	PORTS_init();

	global.match_started = FALSE;
	global.match_over = FALSE;
	global.alim = FALSE;
	global.alim_value = 0;
	global.absolute_time = 0;

	GPIO_SetBits(LED_RUN);
	GPIO_ResetBits(LED_USER);

	UART_init();
	TIMER_init();
	CLOCK_init();
	QUEUE_init();
	BUTTONS_init();
	AX12_init();

	GPIO_SetBits(LED_CAN);
	#ifdef USE_CAN
		CAN_init();
	#endif

	// Gestion du CAN
	#ifdef USE_CAN
		CAN_msg_t msg;
	#endif

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

	debug_printf("\n-------\nWaiting for other boards ready\n-------\n");

	GPIO_SetBits(I_AM_READY);

	time32_t begin_waiting_time = global.absolute_time;
	bool_e FDP_init = FALSE;
	CAN_msg_t waiting_msg;
	while(global.absolute_time - begin_waiting_time < 2000 && !FDP_init){
		while (CAN_data_ready())
		{
			waiting_msg = CAN_get_next_msg();
			if(waiting_msg.sid == BROADCAST_FDP_READY)
				FDP_init = TRUE;
		}
	}

	if(FDP_init)
		debug_printf("Démarrage synchronisé\n");
	else
		debug_printf("Démarrage non syncrhonisé via timeout\n");



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
		#ifdef USE_CAN
			while(CAN_data_ready()){
				// Réception et acquittement
				toggle_led(LED_CAN);
				//debug_printf("Boucle CAN \n");
				msg = CAN_get_next_msg();
				CAN_process_msg(&msg);		// Traitement du message pour donner les consignes à la machine d'état
			}
		#endif

		#ifdef USE_UART
			while(UART1_data_ready()){
				uart_checker(UART1_get_next_msg());
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
	msg.sid = ACT_TEST_SERVO;

	if(state == 0)
		msg.data[0] = ACT_TEST_SERVO_IDLE;
	else if(state == 1)
		msg.data[0] = ACT_TEST_SERVO_STATE_1;
	else if(state == 2)
		msg.data[0] = ACT_TEST_SERVO_STATE_2;

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

#else // ROBOT_SMALL

static void MAIN_onButton0() {
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
