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
#include "QS/QS_buttons.h"
#include "QS/QS_ports.h"
#include "QS/QS_adc.h"
#include "QS/QS_uart.h"
#include "QS/QS_timer.h"

#include "Krusty/KActManager.h"
#include "Tiny/TActManager.h"

//Debug position ax12, à enlever
#include "QS/QS_ax12.h"

//#include "switch.h"

#ifdef USE_CAN
	#include "QS/QS_can.h"
	#include "Can_msg_processing.h"
	#include "QS/QS_CANmsgList.h"
#endif

void RCON_read();
void _ISR _MathError();
void _ISR _StackError();
void _ISR _AddressError();
void _ISR _OscillatorFail();

int main (void)
{
	/*-------------------------------------
		Démarrage
	-------------------------------------*/
	static Uint8 button1_pos = 0;
	static Uint8 button2_pos = 0;
	static Uint8 button3_pos = 0;
	static Uint8 button4_pos = 0;
		
	//initialisations
	PORTS_init();
	LED_RUN = 1;
	UART_init();
	TIMER_init();
	BUTTONS_init();
	QUEUE_init();
	LED_CAN = 1;
	#ifdef USE_CAN
		CAN_init();
	#endif

	// Gestion du CAN
	#ifdef USE_CAN
		CAN_msg_t msg;
	#endif
	
	LED_RUN = 0;
	LED_USER = 1;

	//init actioneurs
	ACTMGR_init();
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

		if(!button1_pos && BUTTON1_PORT)
		{
			//msg.sid = ACT_BALLLAUNCHER;
			//msg.data[0] = ACT_BALLLAUNCHER_ACTIVATE;

			/*msg.sid = ACT_BALLINFLATER;
			msg.data[0] = ACT_BALLINFLATER_STOP;
			msg.data[1] = LOWINT(6000);
			msg.data[2] = HIGHINT(6000);
			msg.size = 3;
			CAN_process_msg(&msg);*/

			msg.sid = ACT_PLATE;
			msg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
			msg.size = 1;
			CAN_process_msg(&msg);

			LED_USER2 = BUTTON1_PORT;
			button1_pos = BUTTON1_PORT;
		}
		else if (button1_pos && !BUTTON1_PORT)
		{
			LED_USER2 = BUTTON1_PORT;
			button1_pos = BUTTON1_PORT;
		}	

		if(!button2_pos && BUTTON2_PORT)
		{
			msg.sid = ACT_PLATE;
			msg.data[0] = ACT_PLATE_ROTATE_PREPARE;
			msg.size = 1;
			CAN_process_msg(&msg);
			LED_USER2 = BUTTON2_PORT;
			button2_pos = BUTTON2_PORT;
		}
		else if (button2_pos && !BUTTON2_PORT)
		{
			LED_USER2 = BUTTON2_PORT;
			button2_pos = BUTTON2_PORT;
		}	

		if(!button3_pos && BUTTON3_PORT)
		{
			/*msg.sid = ACT_BALLINFLATER;
			msg.data[0] = ACT_BALLINFLATER_START;
			msg.data[1] = 3;  //secondes
			msg.size = 2;
			CAN_process_msg(&msg);*/

			msg.sid = ACT_PLATE;
			msg.data[0] = ACT_PLATE_ROTATE_HORIZONTALLY;
			msg.size = 1;
			CAN_process_msg(&msg);

			LED_USER2 = BUTTON3_PORT;
			button3_pos = BUTTON3_PORT;
		}
		else if (button3_pos && !BUTTON3_PORT)
		{
			LED_USER2 = BUTTON3_PORT;
			button3_pos = BUTTON3_PORT;
		}	

		//Affichage des valeurs des capteurs
		if(!button4_pos && BUTTON4_PORT)
		{
			Uint8 i;
			debug_printf("Sensor vals:\n");
#ifdef I_AM_ROBOT_KRUSTY
			debug_printf("- Plate potar val:      %d\n", ADC_getValue(PLATE_ROTATION_POTAR_ADC_ID));
			debug_printf("- Lift left potar val:  %d\n", ADC_getValue(LIFT_LEFT_TRANSLATION_POTAR_ADC_ID));
			debug_printf("- Lift right potar val: %d\n", ADC_getValue(LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID));
			debug_printf("- Capteur cerise: %d\n", BALLSORTER_SENSOR_PIN);

			for(i=0; i<7; i++)
				debug_printf("-  AX12[%d] val: %u\n", i, AX12_get_position(i));
#endif
#ifdef I_AM_ROBOT_TINY
			debug_printf("- Hammer potar val: %d\n", ADC_getValue(HAMMER_SENSOR_ADC_ID));
			debug_printf("- CW[x] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_x));
			debug_printf("- CW[y] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_y));
			debug_printf("- CW[Y] val: %d\n", ADC_getValue(CANDLECOLOR_CW_PIN_ADC_Y));

			for(i=0; i<7; i++)
				debug_printf("-  AX12[%d] val: %u\n", i, AX12_get_position(i));
#endif
			debug_printf("\n");

			LED_USER2 = BUTTON4_PORT;
			button4_pos = BUTTON4_PORT;
		}
		else if (button4_pos && !BUTTON4_PORT)
		{
			LED_USER2 = BUTTON4_PORT;
			button4_pos = BUTTON4_PORT;
		}	
		
		QUEUE_run();
		
		//Scrutation avec les sharps pour savoir si un pion a été aimanté
//		TELEMETER_pawn_detection();
		
		//Scrutation des changement d'états des microrupteurs des pinces
//		SWITCH_microswitch_scan();

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
