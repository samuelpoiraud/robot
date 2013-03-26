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
#include "QS/QS_buttons.h"
#include "QS/QS_ports.h"
#include "QS/QS_adc.h"
#include "QS/QS_uart.h"
#include "QS/QS_timer.h"

#include "Krusty/KActManager.h"
#include "Tiny/TActManager.h"

//Debug position ax12, � enlever
#include "QS/QS_ax12.h"

//#include "switch.h"

#ifdef USE_CAN
	#include "QS/QS_can.h"
	#include "Can_msg_processing.h"
	#include "QS/QS_CANmsgList.h"
#endif

static void MAIN_onButton1();
static void MAIN_onButton2();
static void MAIN_onButton3();
static void MAIN_onButton4();

void RCON_read();
void _ISR _MathError();
void _ISR _StackError();
void _ISR _AddressError();
void _ISR _OscillatorFail();

int main (void)
{
	Sint8 lastSwitchState[2] = {-1, -1};
	/*-------------------------------------
		D�marrage
	-------------------------------------*/
		
	//initialisations
	PORTS_init();
	LED_RUN = 1;
	LED_USER = 0;
	UART_init();
	TIMER_init();
	BUTTONS_init();
	QUEUE_init();
	//init actioneurs
	ACTMGR_init();

	CLOCK_init();

	BUTTONS_define_actions(BUTTON1, &MAIN_onButton1, NULL, 0);
	BUTTONS_define_actions(BUTTON2, &MAIN_onButton2, NULL, 0);
	BUTTONS_define_actions(BUTTON3, &MAIN_onButton3, NULL, 0);
	BUTTONS_define_actions(BUTTON4, &MAIN_onButton4, NULL, 0);

	LED_CAN = 1;
	#ifdef USE_CAN
		CAN_init();
	#endif

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

	RCON_read();
	
	
	while(1)
	{
		/*-------------------------------------
			Gestion des DELs, boutons, etc 
		-------------------------------------*/
		
		//Switch choix de l'actionneur test� par les boutons: affichage de l'�tat s'il a chang�
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
			R�ception CAN et ex�cution
		-------------------------------------*/
		#ifdef USE_CAN
			while(CAN_data_ready()){
				// R�ception et acquittement
				LED_CAN = !LED_CAN;
				//debug_printf("Boucle CAN \r\n");
				msg = CAN_get_next_msg();
				CAN_process_msg(&msg);		// Traitement du message pour donner les consignes � la machine d'�tat
			}
		#endif
	}//Endloop
	return 0;
}

#ifdef I_AM_ROBOT_KRUSTY
static void MAIN_onButton1() {
#ifdef USE_CAN
	CAN_msg_t msg;

	if(SWITCH_RG0) {
		msg.sid = ACT_PLATE;
		if(SWITCH_RG1) {
			debug_printf("Main: Plate rotate vertically\n");
			msg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
		} else {
			debug_printf("Main: Plate plier close\n");
			msg.data[0] = ACT_PLATE_PLIER_CLOSE;
		}
		msg.size = 1;
	} else {
		msg.sid = ACT_LIFT_LEFT;
		if(SWITCH_RG1) {
			debug_printf("Main: Lift left go up\n");
			msg.data[0] = ACT_LIFT_GO_UP;
		} else {
			debug_printf("Main: Lift left plier close\n");
			msg.data[0] = ACT_LIFT_PLIER_CLOSE;
		}
		msg.size = 1;
	}

	CAN_process_msg(&msg);
#endif
}

static void MAIN_onButton2() {
#ifdef USE_CAN
	CAN_msg_t msg;

	if(SWITCH_RG0) {
		msg.sid = ACT_PLATE;
		if(SWITCH_RG1) {
			debug_printf("Main: Plate rotate middle\n");
			msg.data[0] = ACT_PLATE_ROTATE_PREPARE;
		} else {
			debug_printf("Main: Plate plier stop asser\n");
			msg.data[0] = ACT_PLATE_PLIER_STOP;
		}
		msg.size = 1;
	} else {
		msg.sid = ACT_LIFT_LEFT;
		if(SWITCH_RG1) {
			debug_printf("Main: Lift left go middle\n");
			msg.data[0] = ACT_LIFT_GO_MID;
		} else {
			debug_printf("Main: Lift left plier stop asser\n");
			msg.data[0] = ACT_LIFT_PLIER_STOP;
		}
		msg.size = 1;
	}

	CAN_process_msg(&msg);
#endif
}

static void MAIN_onButton3() {
#ifdef USE_CAN
	CAN_msg_t msg;

	if(SWITCH_RG0) {
		msg.sid = ACT_PLATE;
		if(SWITCH_RG1) {
			debug_printf("Main: Plate rotate horizontally\n");
			msg.data[0] = ACT_PLATE_ROTATE_HORIZONTALLY;
		} else {
			debug_printf("Main: Plate plier open\n");
			msg.data[0] = ACT_PLATE_PLIER_OPEN;
		}
		msg.size = 1;
	} else {
		msg.sid = ACT_LIFT_LEFT;
		if(SWITCH_RG1) {
			debug_printf("Main: Lift left go down\n");
			msg.data[0] = ACT_LIFT_GO_DOWN;
		} else {
			debug_printf("Main: Lift left plier open\n");
			msg.data[0] = ACT_LIFT_PLIER_OPEN;
		}
		msg.size = 1;
	}

	CAN_process_msg(&msg);
#endif
}

static void MAIN_onButton4() {
	Uint8 i;
	CAN_msg_t msg;
	
	debug_printf("Plate & Lift asser off\n");

	msg.sid = ACT_PLATE;
	msg.data[0] = ACT_PLATE_ROTATE_STOP;
	msg.size = 1;
	CAN_process_msg(&msg);
	msg.sid = ACT_LIFT_LEFT;
	msg.data[0] = ACT_LIFT_STOP;
	msg.size = 1;
	CAN_process_msg(&msg);

	debug_printf("Sensor vals:\n");
	debug_printf("- Plate potar val:      %d\n", ADC_getValue(PLATE_ROTATION_POTAR_ADC_ID));
	debug_printf("- Lift left potar val:  %d\n", ADC_getValue(LIFT_LEFT_TRANSLATION_POTAR_ADC_ID));
	debug_printf("- Lift right potar val: %d\n", ADC_getValue(LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID));
	debug_printf("- Capteur cerise: %d\n", BALLSORTER_SENSOR_PIN);

	for(i=0; i<7; i++)
		debug_printf("-  AX12[%d] val: %u\n", i, AX12_get_position(i));
	debug_printf("\n");
}

#else // Tiny

static void MAIN_onButton1() {
#ifdef USE_CAN
	CAN_msg_t msg;

	msg.sid = ACT_HAMMER;
	msg.data[0] = ACT_HAMMER_MOVE_TO;
	msg.data[1] = LOWINT(0);
	msg.data[2] = HIGHINT(0);
	msg.size = 3;

	CAN_process_msg(&msg);
#endif
}

static void MAIN_onButton2() {
#ifdef USE_CAN
	CAN_msg_t msg;

	msg.sid = ACT_HAMMER;
	msg.data[0] = ACT_HAMMER_MOVE_TO;
	msg.data[1] = LOWINT(10);
	msg.data[2] = HIGHINT(0);
	msg.size = 3;

	CAN_process_msg(&msg);
#endif
}

static void MAIN_onButton3() {
#ifdef USE_CAN
	CAN_msg_t msg;

	msg.sid = ACT_HAMMER;
	msg.data[0] = ACT_HAMMER_MOVE_TO;
	msg.data[1] = LOWINT(90);
	msg.data[2] = HIGHINT(0);
	msg.size = 3;

	CAN_process_msg(&msg);
#endif
}

static void MAIN_onButton4() {
	Uint8 i;
	debug_printf("- Hammer potar val: %d\n", ADC_getValue(HAMMER_SENSOR_ADC_ID));
	debug_printf("- CW[x] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_x));
	debug_printf("- CW[y] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_y));
	debug_printf("- CW[Y] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_Y));

	for(i=0; i<7; i++)
		debug_printf("-  AX12[%d] val: %u\n", i, AX12_get_position(i));
	debug_printf("\n");
}
#endif


void RCON_read()
{
	debug_printf("dsPIC30F reset source :\n");
	if(RCON & 0x8000)
		debug_printf("- Trap conflict event\n");
	if(RCON & 0x4000)
		debug_printf("- Illegal opcode or uninitialized W register access\n");
	if(RCON & 0x80)
		debug_printf("- MCLR Reset\n");
	if(RCON & 0x40)
		debug_printf("- RESET instruction\n");
	if(RCON & 0x10)
		debug_printf("- WDT time-out\n");
	if(RCON & 0x8)
		debug_printf("- PWRSAV #SLEEP instruction\n");
	if(RCON & 0x4)
		debug_printf("- PWRSAV #IDLE instruction\n");
	if(RCON & 0x2)
		debug_printf("- POR, BOR\n");
	if(RCON & 0x1)
		debug_printf("- POR\n");
	RCON=0;
}
/* Trap pour debug reset */
void _ISR _MathError()
{
  _MATHERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Math\n");
  while(1) Nop();
}

void _ISR _StackError()
{
  _STKERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Stack\n");
  while(1) Nop();
}

void _ISR _AddressError()
{
  _ADDRERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Address\n");
  while(1) Nop();
}

void _ISR _OscillatorFail()
{
  _OSCFAIL = 0;
  LED_ERROR = 1;
  debug_printf("Trap OscFail\n");
  while(1) Nop();
}
