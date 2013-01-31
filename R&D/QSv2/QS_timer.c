/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi-Tech'
 *
 *  Fichier : QS_timer.c
 *  Package : Qualité Soft
 *  Description : Gestion et configuration des timer
 *  Auteur : Jacen
 *  Version 20090808
 */

#include "QS_all.h"
#include "QS_isr.h"
#include <timer.h>

#define TIMER_NB 5

/*	A utiliser avec ConfigIntTimern, de 0 (peu prioritaire) à 7
 *	(le plus prioritaire)
 */
#define TIMER_INT_PRIOR(priority)	(0xFFF8|(priority))


#ifdef FREQ_10MHZ
	#define TIMER_PRESCALER_MS	T1_PS_1_64			/* Prescaler 1:64 */
	#define PULSE_PER_MS	156.25
	#define TIMER_PRESCALER_US	T1_PS_1_1			/* Prescaler 1:1 */
	#define PULSE_PER_US	10
#elif defined (FREQ_20MHZ)
	#define TIMER_PRESCALER_MS	T1_PS_1_128			/* Prescaler 1:128 */
	#define PULSE_PER_MS	156.25
	#define TIMER_PRESCALER_US	T1_PS_1_2			/* Prescaler 1:2 */
	#define PULSE_PER_US	10
#else //40MHz
	#define TIMER_PRESCALER_MS	T1_PS_1_256			/* Prescaler 1:256 */
	#define PULSE_PER_MS	156.25
	#define TIMER_PRESCALER_US	T1_PS_1_4			/* Prescaler 1:4 */
	#define PULSE_PER_US	10
#endif /* def FREQ_XXMHZ */

typedef void(*timer_close_fun_t)();
typedef void(*timer_open_fun_t)(unsigned int config, unsigned int period);
typedef void(*timer_write_fun_t)(unsigned int value);
typedef unsigned int (*timer_read_fun_t)(void);
typedef void (*timer_config_int_fun_t)(unsigned int config);

typedef struct
{
	timer_open_fun_t open;
	timer_read_fun_t read;
	timer_write_fun_t write;
	timer_close_fun_t close;
	isr_fun_t isr;
	acquaintance_mode_e acquaintance_mode;
	timer_config_int_fun_t config_int;
}timer_t;

static struct
{
	double pulse_per_ms;
	double pulse_per_us;
}TIMERS_config;
static timer_t Timer[TIMER_NB];

void TIMER_init()
{
	Uint8 i;
	static bool_e initialized = FALSE;
	if (!initialized)
	{
		TIMERS_config.pulse_per_ms=PULSE_PER_MS;
		TIMERS_config.pulse_per_us=PULSE_PER_US;

		Timer[0].open = &OpenTimer1;
		Timer[0].read = &ReadTimer1;
		Timer[0].write = &WriteTimer1;
		Timer[0].close = &CloseTimer1;
		Timer[0].isr = 0;
		Timer[0].acquaintance_mode=0;
		Timer[0].config_int = ConfigIntTimer1;

		Timer[1].open = &OpenTimer2;
		Timer[1].read = &ReadTimer2;
		Timer[1].write = &WriteTimer2;
		Timer[1].close = &CloseTimer2;
		Timer[1].isr = 0;
		Timer[1].acquaintance_mode=0;
		Timer[1].config_int = ConfigIntTimer2;

		Timer[2].open = &OpenTimer3;
		Timer[2].read = &ReadTimer3;
		Timer[2].write = &WriteTimer3;
		Timer[2].close = &CloseTimer3;
		Timer[2].isr = 0;
		Timer[2].acquaintance_mode=0;
		Timer[2].config_int = ConfigIntTimer3;

		Timer[3].open = &OpenTimer4;
		Timer[3].read = &ReadTimer4;
		Timer[3].write = &WriteTimer4;
		Timer[3].close = &CloseTimer4;
		Timer[3].isr = 0;
		Timer[3].acquaintance_mode=0;
		Timer[3].config_int = ConfigIntTimer4;

		Timer[4].open = &OpenTimer5;
		Timer[4].read = &ReadTimer5;
		Timer[4].write = &WriteTimer5;
		Timer[4].close = &CloseTimer5;
		Timer[4].isr = 0;
		Timer[4].acquaintance_mode=0;
		Timer[4].config_int = ConfigIntTimer5;

		/*
		 *	les interruptions sont activées au lancement du timer
		 * 	on ne fait ici qu'assurer que les IT sont désactivées. 
		 *	Le choix de priorité effectué ici sera effacé au moment
		 *	du lancement du timer.
		 */
		for(i=0; i<TIMER_NB; i++)
		{
			Timer[i].config_int(TIMER_INT_PRIOR(0)&T1_INT_OFF);
		}

		initialized = TRUE;
	}	
}

void TIMER_run(	Uint8 timerID, 
				Uint8 period /* en millisecondes */,
				isr_fun_t isr,
				acquaintance_mode_e acquaintance_mode)
{
	assert(timerID < TIMER_NB);
	Timer[timerID].open(
			T1_ON &					/* Timer1 ON */ 
			T1_IDLE_CON &			/* operate during sleep */
			T1_GATE_OFF &			/* Timer Gate time accumulation disabled */
			TIMER_PRESCALER_MS &	/* Prescaler adapté, cf debut du fichier */
			T2_32BIT_MODE_OFF &		/* Timer 2 and Timer 3 form are 2 16 bit Timers */
			T1_SYNC_EXT_OFF &		/* Do not synch external clk input */
			T1_SOURCE_INT,			/* Internal clock source */
			PULSE_PER_MS * period);	/* periode en ms */
	Timer[timerID].isr = isr;
	Timer[timerID].acquaintance_mode=acquaintance_mode;
	Timer[timerID].config_int(TIMER_INT_PRIOR(6-timerID)&(isr?T1_INT_ON:T1_INT_OFF));
}	

void TIMER_run_us(	Uint8 timerID, 
				Uint16 period /* en microsecondes */,
				isr_fun_t isr,
				acquaintance_mode_e acquaintance_mode)
{
	assert(timerID < TIMER_NB);
	assert(period < (65535/PULSE_PER_US));
	Timer[timerID].open(
			T1_ON &					/* Timer1 ON */ 
			T1_IDLE_CON &			/* operate during sleep */
			T1_GATE_OFF &			/* Timer Gate time accumulation disabled */
			TIMER_PRESCALER_US &	/* Prescaler adapté, cf debut du fichier */
			T2_32BIT_MODE_OFF &		/* Timer 2 and Timer 3 form are 2 16 bit Timers */
			T1_SYNC_EXT_OFF &		/* Do not synch external clk input */
			T1_SOURCE_INT,			/* Internal clock source */
			PULSE_PER_US * period);	/* periode en us */
	Timer[timerID].isr = isr;
	Timer[timerID].acquaintance_mode=acquaintance_mode;
	Timer[timerID].config_int(TIMER_INT_PRIOR(6-timerID)&(isr?T1_INT_ON:T1_INT_OFF));
}

void TIMER_stop(Uint8 timerID)
{	
	assert(timerID < TIMER_NB);
	Timer[timerID].close();
}

Uint8 TIMER_read(Uint8 timerID)
{	
	assert(timerID < TIMER_NB);
	return (Timer[timerID].read())/PULSE_PER_MS;
}

Uint16 TIMER_read_us(Uint8 timerID)
{	
	assert(timerID < TIMER_NB);
	return (Timer[timerID].read())/PULSE_PER_US;
}

void _ISR _T1Interrupt()
{
	assert(Timer[0].isr);
	if(BIT_TEST(Timer[0].acquaintance_mode, 0))
		IFS0bits.T1IF = 0;
	Timer[0].isr();
	if(BIT_TEST(Timer[0].acquaintance_mode, 1))
		IFS0bits.T1IF = 0;
}

void _ISR _T2Interrupt()
{
	assert(Timer[1].isr);
	if(BIT_TEST(Timer[1].acquaintance_mode, 0))
		IFS0bits.T2IF = 0;
	Timer[1].isr();
	if(BIT_TEST(Timer[1].acquaintance_mode, 1))
		IFS0bits.T2IF = 0;
}
void _ISR _T3Interrupt()
{
	assert(Timer[2].isr);
	if(BIT_TEST(Timer[2].acquaintance_mode, 0))
		IFS0bits.T3IF = 0;
	Timer[2].isr();
	if(BIT_TEST(Timer[2].acquaintance_mode, 1))
		IFS0bits.T3IF = 0;
}
void _ISR _T4Interrupt()
{
	assert(Timer[3].isr);
	if(BIT_TEST(Timer[3].acquaintance_mode, 0))
		IFS1bits.T4IF = 0;
	Timer[3].isr();
	if(BIT_TEST(Timer[3].acquaintance_mode, 1))
		IFS1bits.T4IF = 0;
}

void _ISR _T5Interrupt()
{
	assert(Timer[4].isr);
	if(BIT_TEST(Timer[4].acquaintance_mode, 0))
		IFS1bits.T5IF = 0;
	Timer[4].isr();
	if(BIT_TEST(Timer[4].acquaintance_mode, 1))
		IFS1bits.T5IF = 0;
}

//Code utilisant les timers 4 et 5 ensemble pour créer un timer 32 bis.
//
//void TIMER4_stop(void)
//{	
//	/* Il s'agit en fait de la combinaison des Timers 4 et 5 */
//	DisableIntT4;
//	WriteTimer45(0);
//	CloseTimer45();
//}
//
//void TIMER4_run(Uint16 period /* en millisecondes */)
//{
//	/* Il s'agit en fait de la combinaison des Timers 4 et 5 */
//	OpenTimer4(
//			T4_ON &					/* Timer4 ON */ 
//			T4_IDLE_CON &			/* operate during sleep */
//			T4_GATE_OFF &			/* Timer Gate time accumulation disabled */
//			T4_PS_1_256 &			/* Prescaler 1:256 */
//			T4_SOURCE_INT &			/* Internal clock source */
//			T4_32BIT_MODE_ON,		/* Timer 4 and Timer 5 form a 32 bit Timer */
//			PULSE_PER_MS * period);	/* periode en ms */
//	EnableIntT4;
//}
//
