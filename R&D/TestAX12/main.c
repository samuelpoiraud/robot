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

#define MAIN_C
#include "main.h"
#include "QS/QS_ax12_v2.h"

void onButton1Pushed();
void onButton2Pushed();
void onButton3Pushed();
void onButton4Pushed();

int main (void)
{
	//initialisations
	PORTS_init();
	UART_init();
	TIMER_init();
	AX12_init();
        BUTTONS_init();

	LED_CAN = 0;
	LED_RUN = 0;
	LED_USER = 1;
	

	debug_printf("Hello, I'm TestAX12 (ACT)\n");

	AX12_instruction_write8(0, AX12_TORQUE_ENABLE, 0x0000);
	AX12_instruction_write16(0, AX12_CW_ANGLE_LIMIT_L, 0x0000);
	AX12_instruction_write16(0, AX12_CCW_ANGLE_LIMIT_L, 500);
	AX12_instruction_wait();

	debug_printf("AX12 ready, you can move the output now\n");
	TIMER2_run(100);
        BUTTONS_define_actions(BUTTON1, &onButton1Pushed, NULL, 0);
        BUTTONS_define_actions(BUTTON2, &onButton2Pushed, NULL, 0);
        BUTTONS_define_actions(BUTTON3, &onButton3Pushed, NULL, 0);
		BUTTONS_define_actions(BUTTON4, &onButton4Pushed, NULL, 0);
	while(1)
	{
            BUTTONS_update();
	}//Endloop

	return 0;
}

void onButton1Pushed() {
    static bool_e already_in_function = FALSE;
	LED_USER = !LED_USER;
    if(already_in_function)
        return;
    already_in_function = TRUE;
    AX12_instruction_write16(0, AX12_GOAL_SPEED_L, 100);
    already_in_function = FALSE;
}

void onButton2Pushed() {
    static bool_e already_in_function = FALSE;
	LED_USER = !LED_USER;
    if(already_in_function)
        return;
    already_in_function = TRUE;
    AX12_instruction_write16(0, AX12_GOAL_POSITION_L, 200);
    
    already_in_function = FALSE;
}

void onButton3Pushed() {
    static bool_e already_in_function = FALSE;
	LED_USER = !LED_USER;
    if(already_in_function)
        return;
    already_in_function = TRUE;
    //AX12_instruction_write16(0, AX12_GOAL_SPEED_L, 250);
  	AX12_instruction_write16(0, AX12_GOAL_SPEED_L, 1023);
  	
    already_in_function = FALSE;
}

void onButton4Pushed() {
    static bool_e already_in_function = FALSE;
	LED_USER = !LED_USER;
    if(already_in_function)
        return;
    already_in_function = TRUE;
    //AX12_instruction_write16(0, AX12_GOAL_SPEED_L, 250);
  	AX12_instruction_write16(0, AX12_GOAL_POSITION_L, 270);
  //AX12_instruction_wait();
/*	while(AX12_get_position(0)<490)	
		AX12_instruction_wait();
  	AX12_instruction_write16(0, AX12_GOAL_POSITION_L, 100);
*/	AX12_instruction_wait();
    already_in_function = FALSE;
}



void _ISR _T2Interrupt() {
	static bool_e flag = FALSE;
	static Uint16 counter = 0;


	counter++;

	if(counter == 10) {
            debug_printf("AX12 is at position %d\n", AX12_get_position(0));
		
            counter = 0;
	}
	IFS0bits.T2IF = 0;
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
