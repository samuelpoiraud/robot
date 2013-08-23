/*
 *	Club Robot ESEO 2008 - 2011
 *	Archi-Tech', PACMAN, ChekNorris
 *
 *	Fichier : main.h
 *	Package : Supervision
 *	Description : sequenceur de la supervision
 *	Auteur : Jacen
 *	Version 20110413
 */

#define MAIN_C
#include "main.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_adc.h"
#ifdef ENABLE_XBEE
	#include "QS/QS_can_over_xbee.h"
#endif

void init(void);
void tests(void);
void SWITCH_update(void);
void alim_watcher_init(void);
bool_e watch_alim(void);
#ifdef TEST_HARD
static bool_e received;
void test_W_UART2();
void test_R_UART2();
#endif

int main (void)
{	
	Uint16 i;
	init();
#ifdef TEST_HARD
	test_W_UART2();
#endif
		
//	print_all_msg(); //désolé...
//	EEPROM_CAN_MSG_verbose_all_match_header();
	i=-1;
#ifdef TEST_HARD
	received  = FALSE;
#endif
	while (1)
	{
		bool_e fast_save = FALSE;

#ifdef TEST_HARD
		test_R_UART2();
#endif

		/* Gestion switchs */
		SWITCH_update();
		
		/* Vérification de la réception d'un message CAN et traitement de ce message */		
		CAN_WATCH_update();

		/* Vérification de la réception d'un message de type CAN sur l'UART1 et l'UART2 */
		CAN_INJECTOR_update();

		fast_save = watch_alim();

	}
	return 0;
}

void alim_watcher_init(){
	ADC_init();
}

bool_e watch_alim(void){
	Sint16 value = ADC_getValue(0);
	//static Sint16 previous_value;

	if(value < 500){
		return TRUE;
	}

	//previous_value = value;
	return FALSE;
}

void init(void)
{
	LED_init();
	PORTS_init();
	UART_init();;
	RCON_read();
	
	TIMER_init();
	BUFFER_init();
	CAN_init();
	CAN_WATCH_init();
	CAN_INJECTOR_init();
	BUTTON_init();
	
	TIMER4_run(250);	//Timer 1 : 250ms.

}

#ifdef TEST_HARD
void test_W_UART2(void)
{
	debug_printf("Test emission UART2:");
}

void debug_msg_received(){
	received = TRUE;
}

void test_R_UART2(){
	if(received == TRUE){
		debug_printf("Message recu!");
		received = FALSE;
	}
}
#endif

void SWITCH_update(void)
{
	#ifdef FDP_2013
		global.config[DEBUG]		= SWITCH_RG1;
		global.config[U1_VERBOSE] 	= SWITCH_RG0;
	#else
		global.config[DEBUG]		= 1;
		global.config[U1_VERBOSE] 	= 1;	
	#endif
}	

void _ISR _T4Interrupt()
{
	static Uint8 time_250ms = 0;
	time_250ms = (time_250ms>=4)?0:time_250ms+1;
	if(!time_250ms)	//1 fois toutes les secondes...
	{
#ifdef TEST_HARD
		static Uint8 LED_LAT = 0x1;
		LATD = LED_LAT<<8;
		if(LED_LAT == 0b100000){
			LED_LAT = 1;
		}else{
			LED_LAT <<=1;
		}
	}
#endif
	beacon_flag_update();
	IFS1bits.T4IF = 0;
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
