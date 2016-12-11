#include "it.h"

#include "QS/QS_timer.h"
#include "main.h"
#include "IHM/led.h"
#include "environment.h"
#include "IHM/buzzer.h"

#define IT_TIME		1		// en ms (maximum 65)


void IT_init(void){
	TIMER_init();
	TIMER1_run_us(1000*IT_TIME);			//IT clock vitesse de 1ms
}


//Sur interruption timer 1...
void _ISR _T1Interrupt(){

	LED_processIt(IT_TIME);
	ENVIRONMENT_processIt(IT_TIME);
	BUZZER_processIt(IT_TIME);

	TIMER1_AckIT();
}
