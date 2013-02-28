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
#include "QS/QS_uart.h"
#include "QS/QS_timer.h"

#include "Krusty/KActManager.h"
#include "Tiny/TActManager.h"

#include "Ball_grabber.h"
#include "Hammer.h"

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

	//init actioneur
	ACTMGR_init();
	BALL_GRABBER_init();
	HAMMER_init();
	//inti fin

	debug_printf("Hello, I'm ACT\r\n");
	RCON_read();
	
	while(1)
	{
		/*-------------------------------------
			Gestion des DELs, boutons, etc 
		-------------------------------------*/	
		/*Prepare clamp Front Bottom*/
		//debug_printf("Valeur codeur Arrière : %d\r\n",LIFT_BACK_getPos());
		if(!button1_pos && BUTTON1_PORT)
		{
			//msg.sid = ACT_PREPARE_CLAMP ;
			//msg.data[0] = ACT_BACK;
			//msg.data[1] = ACT_LIFT_BOTTOM;
			//msg.size = 2;
			//CAN_process_msg(&msg);
			LED_USER2 = BUTTON1_PORT;
			button1_pos = BUTTON1_PORT;
		}
		else if (button1_pos && !BUTTON1_PORT)
		{
			LED_USER2 = BUTTON1_PORT;
			button1_pos = BUTTON1_PORT;
		}	
		
		/*Take pawn Front Middle*/
		if(!button2_pos && BUTTON2_PORT)
		{
			//msg.sid = ACT_TAKE_PAWN;
			//msg.data[0] = ACT_BACK;
			//msg.data[1] = ACT_LIFT_MIDDLE;
			//msg.data[2] = ACK_CLAMP_ACTION;
			//msg.size = 3;
			//CAN_process_msg(&msg);
			LED_USER2 = BUTTON2_PORT;
			button2_pos = BUTTON2_PORT;
		}
		else if (button2_pos && !BUTTON2_PORT)
		{
			LED_USER2 = BUTTON2_PORT;
			button2_pos = BUTTON2_PORT;
		}	

		/*File pawn Front Bottom*/
		if(!button3_pos && BUTTON3_PORT)
		{
			//msg.sid = ACT_FILE_PAWN;
			//msg.data[0] = ACT_BACK;
			//msg.data[1] = ACT_LIFT_BOTTOM;
			//msg.data[2] = ACK_LIFT_ACTION;
			//msg.size = 3;
			//CAN_process_msg(&msg);
			//debug_printf("Valeur potar CLAMP_FRONT %d ",CLAMP_FRONT_getPos());
			LED_USER2 = BUTTON3_PORT;
			button3_pos = BUTTON3_PORT;
		}
		else if (button3_pos && !BUTTON3_PORT)
		{
			LED_USER2 = BUTTON3_PORT;
			button3_pos = BUTTON3_PORT;
		}	
		
		if(!button4_pos && BUTTON4_PORT)
		{
			
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
	debug_printf("dsPIC30F reset source :\r\n");
	if(RCON & 0x8000)
		debug_printf("- Trap conflict event\r\n");
	if(RCON & 0x4000)
		debug_printf("- Illegal opcode or uninitialized W register access\r\n");
	if(RCON & 0x80)
		debug_printf("- MCLR Reset\r\n");
	if(RCON & 0x40)
		debug_printf("- RESET instruction\r\n");
	if(RCON & 0x10)
		debug_printf("- WDT time-out\r\n");
	if(RCON & 0x8)
		debug_printf("- PWRSAV #SLEEP instruction\r\n");
	if(RCON & 0x4)
		debug_printf("- PWRSAV #IDLE instruction\r\n");
	if(RCON & 0x2)
		debug_printf("- POR, BOR\r\n");
	if(RCON & 0x1)
		debug_printf("- POR\r\n");
	RCON=0;
}
/* Trap pour debug reset */
void _ISR _MathError()
{
  _MATHERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Math\r\n");
  while(1) Nop();
}

void _ISR _StackError()
{
  _STKERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Stack\r\n");
  while(1) Nop();
}

void _ISR _AddressError()
{
  _ADDRERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Address\r\n");
  while(1) Nop();
}

void _ISR _OscillatorFail()
{
  _OSCFAIL = 0;
  LED_ERROR = 1;
  debug_printf("Trap OscFail\r\n");
  while(1) Nop();
}
