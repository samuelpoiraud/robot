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
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "queue.h"
#include "clock.h"
#include "QS/QS_who_am_i.h"
#include "Pierre/PFilet.h"

#include "BIG/BActManager.h"
#include "SMALL/SActManager.h"

//Information pour le bouton 4
#include "QS/QS_ax12.h"
#include "QS/QS_CapteurCouleurCW.h"

//TODO: SelfTest


//#include "switch.h"

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

extern bool_e EXECUTING_LAUNCH;

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

	LED_RUN = 1;
	LED_USER = 0;

	UART_init();
	TIMER_init();
	CLOCK_init();
	QUEUE_init();
	BUTTONS_init();

	LED_CAN = 1;
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
		#define ROBOT_CODE_NAME	"Pierre"
		if(QS_WHO_AM_I_get() != BIG_ROBOT)
	#elif defined(I_AM_ROBOT_SMALL)
		#define ROBOT_CODE_NAME	"Guy"
		if(QS_WHO_AM_I_get() != SMALL_ROBOT)
	#endif	// Pour changer le code du robot aller dans : "config/config_global.h"
			debug_printf("ATTENTION ! Vous avez programmé la carte actionneur de %s avec le code de %s\n", QS_WHO_AM_I_get_name(), ROBOT_CODE_NAME);

	#undef ROBOT_CODE_NAME

	//Init actioneurs
	ACTMGR_init();

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

		LED_USER = !LED_USER;
		LED_USER2 = BUTTON1_PORT || BUTTON2_PORT || BUTTON3_PORT || BUTTON4_PORT;

		if(EXECUTING_LAUNCH)
			debug_printf("message passé\n");

		QUEUE_run();
		BUTTONS_update();

		#ifdef I_AM_ROBOT_BIG
		if(global.match_started == FALSE && global.match_over == FALSE) // Si le match n'a pas commencé et n'est pas terminé on gère le réarmement automatique du filet et l'avertissement
				;//FILET_process_main();
		#endif

		/*-------------------------------------
			Réception CAN et exécution
		-------------------------------------*/
		#ifdef USE_CAN
			while(CAN_data_ready()){
				// Réception et acquittement
				LED_CAN = !LED_CAN;
				//debug_printf("Boucle CAN \n");
				msg = CAN_get_next_msg();
				CAN_process_msg(&msg);		// Traitement du message pour donner les consignes à la machine d'état
			}
		#endif
	}//Endloop
	return 0;
}

static void MAIN_onButton0() {
	CAN_msg_t msg;
	msg.size = 2;
	msg.sid = ACT_ARM;
	msg.data[0] = ACT_ARM_GOTO;


	static int truc = 0;
	truc = !truc;

	msg.data[0] = ACT_ARM_PRINT_POS;
	CAN_process_msg(&msg);
}


#ifdef I_AM_ROBOT_BIG
static void MAIN_onButton1() {
	CAN_msg_t msg;
	msg.size = 2;
	msg.sid = ACT_ARM;
	msg.data[0] = ACT_ARM_GOTO;
	msg.data[1] = ACT_ARM_POS_OPEN;
	CAN_process_msg(&msg);
}

static void MAIN_onButton2() {
	CAN_msg_t msg;
	msg.size = 2;
	msg.sid = ACT_ARM;
	msg.data[0] = ACT_ARM_GOTO;
	msg.data[1] = ACT_ARM_POS_MID;
	CAN_process_msg(&msg);
}

static void MAIN_onButton3() {
	CAN_msg_t msg;
	msg.size = 2;
	msg.sid = ACT_ARM;
	msg.data[0] = ACT_ARM_GOTO;
	msg.data[1] = ACT_ARM_POS_PARKED;
	CAN_process_msg(&msg);
}

static void MAIN_onButton4() {
	//FILET_BOUTON_process();
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = ACT_ARM;
	msg.data[0] = ACT_ARM_INIT;
	CAN_process_msg(&msg);
}

#else // ROBOT_SMALL

static void MAIN_onButton1() {}

static void MAIN_onButton2() {}

static void MAIN_onButton3() {}

static void MAIN_onButton4() {}
#endif // ROBOT_BIG et ROBOT_SMALL
