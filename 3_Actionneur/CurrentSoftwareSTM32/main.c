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

#include "Krusty/KActManager.h"
#include "Tiny/TActManager.h"

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

	LED_RUN = 1;
	LED_USER = 0;

	UART_init();
	TIMER_init();
	CLOCK_init();
	QUEUE_init();
	BUTTONS_init();

	LED_CAN = 1;
	//#ifdef USE_CAN
		CAN_init();
	//#endif

	// Gestion du CAN
	#ifdef USE_CAN
		CAN_msg_t msg;
	#endif

#if defined(I_AM_ROBOT_KRUSTY)
	debug_printf("--- Hello, I'm ACT (Krusty) ---\n");
#elif defined(I_AM_ROBOT_TINY)
	debug_printf("--- Hello, I'm ACT (Tiny) ---\n");
#else
	debug_printf("--- Hello, I'm ACT ---\n");
#endif

	//Init actioneurs
	ACTMGR_init();

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
			debug_printf("SWITCH_RG0: %s | %s\n", (SWITCH_RG0)? "[Plate]" : " Plate ",
												  (SWITCH_RG0)? " Lift " : "[Lift]");
		}
		if(lastSwitchState[1] != SWITCH_RG1) {
			lastSwitchState[1] = SWITCH_RG1;
			debug_printf("SWITCH_RG1: %s | %s\n", (SWITCH_RG1)? "[DCMotor]" : " DCMotor ",
												  (SWITCH_RG1)? " Plier-AX12 " : "[Plier-AX12]");
		}

		LED_USER = !LED_USER;
		LED_USER2 = BUTTON1_PORT || BUTTON2_PORT || BUTTON3_PORT || BUTTON4_PORT;

		QUEUE_run();
		BUTTONS_update();

		/*-------------------------------------
			Réception CAN et exécution
		-------------------------------------*/
		#ifdef USE_CAN
			while(CAN_data_ready()){
				// Réception et acquittement
				LED_CAN = !LED_CAN;
				//debug_printf("Boucle CAN \r\n");
				msg = CAN_get_next_msg();
				CAN_process_msg(&msg);		// Traitement du message pour donner les consignes à la machine d'état
			}
		#endif
	}//Endloop
	return 0;
}

#ifdef I_AM_ROBOT_KRUSTY
static void MAIN_onButton1() {
#ifdef USE_CAN
/*	CAN_msg_t msg;

	msg.size = 1;
	if(SWITCH_RG0) {
		msg.sid = ACT_PLATE;
		if(SWITCH_RG1) {
			debug_printf("Main: Plate rotate vertically\n");
			msg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
		} else {
			debug_printf("Main: Plate plier close\n");
			msg.data[0] = ACT_PLATE_PLIER_CLOSE;
		}
		CAN_process_msg(&msg);
	} else {
		if(SWITCH_RG1) {
			debug_printf("Main: Lift left go up\n");
			msg.data[0] = ACT_LIFT_GO_UP;
		} else {
			debug_printf("Main: Lift left plier close\n");
			msg.data[0] = ACT_LIFT_PLIER_CLOSE;
		}
		msg.sid = ACT_LIFT_RIGHT;
		CAN_process_msg(&msg);
		msg.sid = ACT_LIFT_LEFT;
		CAN_process_msg(&msg);
	}*/
	CAN_msg_t msg;

	msg.size = 1;

	msg.sid = ACT_FRUIT_MOUTH;

	debug_printf("Main: ACT_FRUIT_MOUTH_OPEN\n");
	msg.data[0] = ACT_FRUIT_MOUTH_OPEN;

	CAN_process_msg(&msg);


#endif
}

static void MAIN_onButton2() {
#ifdef USE_CAN
/*	CAN_msg_t msg;

	msg.size = 1;
	if(SWITCH_RG0) {
		msg.sid = ACT_PLATE;
		if(SWITCH_RG1) {
			debug_printf("Main: Plate rotate middle\n");
			msg.data[0] = ACT_PLATE_ROTATE_PREPARE;
		} else {
			debug_printf("Main: Plate plier stop asser\n");
			msg.data[0] = ACT_PLATE_PLIER_STOP;
		}
		CAN_process_msg(&msg);
	} else {
		if(SWITCH_RG1) {
			debug_printf("Main: Lift left go middle\n");
			msg.data[0] = ACT_LIFT_GO_MID;
		} else {
			debug_printf("Main: Lift left plier stop asser\n");
			msg.data[0] = ACT_LIFT_PLIER_STOP;
		}
		msg.sid = ACT_LIFT_RIGHT;
		CAN_process_msg(&msg);
		msg.sid = ACT_LIFT_LEFT;
		CAN_process_msg(&msg);
	}
*/
	CAN_msg_t msg;

	msg.size = 1;

	msg.sid = ACT_FRUIT_MOUTH;

	debug_printf("Main: ACT_FRUIT_MOUTH_CLOSED\n");
	msg.data[0] = ACT_FRUIT_MOUTH_CLOSE;

	CAN_process_msg(&msg);


#endif
}

static void MAIN_onButton3() {
#ifdef USE_CAN
	CAN_msg_t msg;

		msg.size = 1;

		msg.sid = ACT_LANCELAUNCHER;


		msg.data[0] = ACT_LANCELAUNCHER_RUN;

		CAN_process_msg(&msg);
		debug_printf("Main: ACT_LANCELAUNCHER_RUN\n");

	/*if(SWITCH_RG0) {
		msg.sid = ACT_PLATE;
		if(SWITCH_RG1) {
			debug_printf("Main: Plate rotate horizontally\n");
			msg.data[0] = ACT_PLATE_ROTATE_HORIZONTALLY;
		} else {
			debug_printf("Main: Plate plier open\n");
			msg.data[0] = ACT_PLATE_PLIER_OPEN;
		}
		CAN_process_msg(&msg);
	} else {
		msg.sid = ACT_LIFT_RIGHT;
		if(SWITCH_RG1) {
			debug_printf("Main: Lift left go down\n");
			msg.data[0] = ACT_LIFT_GO_DOWN;
		} else {
			debug_printf("Main: Lift left plier open\n");
			msg.data[0] = ACT_LIFT_PLIER_OPEN;
		}
		msg.sid = ACT_LIFT_RIGHT;
		CAN_process_msg(&msg);
		msg.sid = ACT_LIFT_LEFT;
		CAN_process_msg(&msg);
	}
*/
#endif
}

static void MAIN_onButton4() {

	/*Uint8 i;
	CAN_msg_t msg;
	static Uint16 w;

	debug_printf("Plate & Lift asser off\n");

	msg.sid = ACT_PLATE;
	msg.data[0] = ACT_PLATE_ROTATE_STOP;
	msg.size = 1;
	CAN_process_msg(&msg);
	msg.sid = ACT_LIFT_RIGHT ;
	msg.data[0] = ACT_LIFT_STOP;
	msg.size = 1;
	CAN_process_msg(&msg);

	msg.sid = ACT_BALLSORTER;
	msg.data[0] = ACT_BALLSORTER_TAKE_NEXT_CHERRY;
	msg.data[1] = LOWINT(6300);
	msg.data[2] = ((HIGHINT(6300) & 0x7F) | ((Uint16)((w % 3) != 2) << 7));
	msg.size = 3;
	w++;
	CAN_process_msg(&msg);

	debug_printf("Sensor vals:\n");
	debug_printf("- Plate potar val:      %d\n", ADC_getValue(PLATE_ROTATION_POTAR_ADC_ID));
	debug_printf("- Lift left potar val:  %d\n", ADC_getValue(LIFT_LEFT_TRANSLATION_POTAR_ADC_ID));
	debug_printf("- Lift right potar val: %d\n", ADC_getValue(LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID));
	debug_printf("- Capteur cerise Red(Q1): %d, White(Q2): %d, Blue(Q3): %d\n", BALLSORTER_SENSOR_PIN_RED_CHERRY, BALLSORTER_SENSOR_PIN_WHITE_CHERRY, BALLSORTER_SENSOR_PIN_BLUE_CHERRY);

	for(i=0; i<7; i++)
		debug_printf("-  AX12[%d] val: %u\n", i, AX12_get_position(i));
	debug_printf("\n");*/
}

#else // Tiny

#include "Tiny/THammer.h" //pour HAMMER_get_pos()

static void MAIN_onButton1() {
#ifdef USE_CAN
	CAN_msg_t msg;

//	msg.sid = ACT_HAMMER;
//	msg.data[0] = ACT_HAMMER_MOVE_TO;
//	msg.data[1] = LOWINT(0);
//	msg.data[2] = HIGHINT(0);
//	msg.size = 3;

	msg.sid = ACT_BALLINFLATER;
	msg.data[0] = ACT_BALLINFLATER_START;
	msg.data[1] = 10;
	msg.size = 2;

	CAN_process_msg(&msg);
#endif
}

static void MAIN_onButton2() {
#ifdef USE_CAN
	CAN_msg_t msg;

//	msg.sid = ACT_HAMMER;
//	msg.data[0] = ACT_HAMMER_MOVE_TO;
//	msg.data[1] = LOWINT(10);
//	msg.data[2] = HIGHINT(0);
//	msg.size = 3;

	msg.sid = ACT_BALLINFLATER;
	msg.data[0] = ACT_BALLINFLATER_STOP;
	msg.size = 1;

	CAN_process_msg(&msg);
#endif
}

static void MAIN_onButton3() {
#ifdef USE_CAN
	CAN_msg_t msg;

	msg.sid = ACT_HAMMER;
	msg.data[0] = ACT_HAMMER_BLOW_CANDLE;
	msg.data[1] = BLUE;
	msg.size = 2;

	CAN_process_msg(&msg);

#endif
}

static void MAIN_onButton4() {
	Uint8 i;
	CAN_msg_t msg;

	msg.sid = ACT_HAMMER;
	msg.data[0] = ACT_HAMMER_STOP;
	msg.size = 1;
	CAN_process_msg(&msg);

	debug_printf("- Hammer pos en degres: %d, potar val: %d\n", HAMMER_get_pos(), ADC_getValue(HAMMER_SENSOR_ADC_ID));
	debug_printf("- CW[x] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_X));
	debug_printf("- CW[y] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_Y));
	debug_printf("- CW[Y] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_Z));
	debug_printf("- CW digital:\n"
				 " CH0: %d\n"
				 " CH1: %d\n"
				 " CH2: %d\n"
				 " CH3: %d\n",
			CW_is_color_detected(CANDLECOLOR_CW_ID, 0),
			CW_is_color_detected(CANDLECOLOR_CW_ID, 1),
			CW_is_color_detected(CANDLECOLOR_CW_ID, 2),
			CW_is_color_detected(CANDLECOLOR_CW_ID, 3));

	for(i=0; i<7; i++)
		debug_printf("-  AX12[%d] val: %u\n", i, AX12_get_position(i));
	debug_printf("\n");
}
#endif // Tiny ou Krusty
