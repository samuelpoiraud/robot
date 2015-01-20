/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : main.c
 *	Package : Carte Principale
 *	Description : Ordonnancement de la carte Principale
 *	Auteur : Jacen, modifi� par Gonsevi
 *	Version 2012/01/14
 */

#define MAIN_C
#include "main.h"
	#include "QS/QS_ports.h"
	#include "QS/QS_uart.h"
	#include "queue.h"
	#include "Stacks.h"
	#include "environment.h"
	#include "brain.h"
	#include "clock.h"
	#include "zone_mutex.h"
	#include "QS/QS_who_am_i.h"
	#include "QS/QS_outputlog.h"
	#include "queue.h"
	#include "QS/QS_buttons.h"
	#include "Supervision/Supervision.h"
	#include "act_functions.h"
	#ifdef STM32F40XX
		#include "QS/QS_sys.h"
		#include "stm32f4xx.h"
		#include "stm32f4xx_gpio.h"
		#include "stm32f4xx_rcc.h"
	#endif
#include "QS/QS_IHM.h"
#include "act_avoidance.h"


void test_bp_switchs(void);
void test_leds(void);
void pull_bp_and_switch(void);
void RCON_read(void);

void process_measure_loop_duration(void)
{
	static time32_t previous_time = 0;
	if(global.env.match_time > 0)
	{
		//D�commenter ce printf pour afficher la mesure de la dur�e de la boucle.
		//ATTENTION, cet affichage est hors de la mesure... (donc elle impacte le code hors de cette mesure).
		if(global.env.match_time-previous_time > 5)
			debug_printf("L%d\n",(Uint16)(global.env.match_time-previous_time));
		previous_time = global.env.match_time;
	}
}



void tests(void)
{
	//test_bp_switchs();
	//test_leds();
}

int main (void)
{
	volatile Uint16 i, j;
	// Commandes pour EVE
	#ifdef USE_QSx86
		// Initialisation des variables utiles
		EVE_manager_card_init();
	#endif // USE_QSx86
	SYS_init();		// Init syst�me
	PORTS_init();	// Config des ports

	// Activation des pulls UP des capteurs de recalage
	PORTS_set_pull(GPIOC, GPIO_Pin_1, GPIO_PuPd_UP);
	PORTS_set_pull(GPIOC, GPIO_Pin_2, GPIO_PuPd_UP);

	#ifdef MODE_SIMULATION
		pull_bp_and_switch();
	#endif

	UART_init();
	//RCON_read();

	#ifndef FDP_2014
		IHM_init();
	#endif

	ENV_init();	//Pour �tre r�ceptif aux �ventuels messages CAN envoy�s tr�s t�t...

	GPIO_SetBits(LED_RUN);

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//D�termine le robot sur lequel est branch�e la carte.
	debug_printf("------- Hello, I'm STRAT (%s) -------\n", QS_WHO_AM_I_get_name());

	STACKS_init();
	QUEUE_init();

	// Si le d�marrage de la carte n'est pas cons�cutif � l'arriv�e de l'alimentation
	//		On reset les autres cartes
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == RESET)
		CAN_send_sid(BROADCAST_RESET);

	CLOCK_init();
	debug_printf("\n-------\nWaiting for other boards ready\n-------\n");
	//retard pour attendre l'initialisation des autres cartes
	// voir si on peut faire mieux
	for(j=0;j<100;j++)
	{
		for(i=1;i;i++);
		if(j%10 == 0x00)
			debug_printf(".");
	}
	debug_printf("\n");

	Supervision_init();
	BRAIN_init();
	ACT_AVOIDANCE_init();
	ELEMENTS_init();

#ifndef FDP_2014
	// Demande des �tats initiaux des switchs
	CAN_send_sid(IHM_GET_SWITCH);
#endif

	while(1)
	{
		toggle_led(LED_RUN);

		// Commandes pour EVE
		#ifdef USE_QSx86
			EVE_manager_card();
		#endif // USE_QSx86
		/* mise � jour de l'environnement */
		ENV_update();

		/* Code concernant la supervision */
		Supervision_process_main();

		/* Execution des routines de supervision des piles
		actionneurs */
		STACKS_run();
		QUEUE_run();

		any_match();

		process_measure_loop_duration();
	}
	return 0;
}

void pull_bp_and_switch(void)
{
	//En mode simulation, on indique une valeur par d�faut des boutons et des switchs..... par l'ajout d'un pull up ou pull down.
	//PORTS_set_pull(GPIOA, GPIO_Pin_7, GPIO_PuPd_DOWN);	//Verbose : Verbose = 0/ Bin = 1
	//La pr�sence de laccelermetre empeche le PD d'etre prepond�rent sur la broche
	PORTS_set_pull(GPIOA, GPIO_Pin_1, GPIO_PuPd_UP);	//Switch debug : can+debug on = 1 / Rien(sauf certains printf) = 0
	PORTS_set_pull(GPIOB, GPIO_Pin_4, GPIO_PuPd_DOWN);	//XBee OFF
	PORTS_set_pull(GPIOB, GPIO_Pin_5, GPIO_PuPd_DOWN);	//Save OFF
	PORTS_set_pull(GPIOB, GPIO_Pin_0, GPIO_PuPd_DOWN);	//Mesure du 24V
	PORTS_set_pull(GPIOC, GPIO_Pin_13, GPIO_PuPd_UP);	//Who Am I : gros robot
	PORTS_set_pull(GPIOD, GPIO_Pin_6, GPIO_PuPd_UP);	//Color (1)
	PORTS_set_pull(GPIOD, GPIO_Pin_7, GPIO_PuPd_UP);	//Biroute OFF (1)
	PORTS_set_pull(GPIOE, GPIO_Pin_7, GPIO_PuPd_UP);	//Sw LCD
	PORTS_set_pull(GPIOE, GPIO_Pin_8, GPIO_PuPd_UP);	//Sw evit
	PORTS_set_pull(GPIOE, GPIO_Pin_9, GPIO_PuPd_UP);	//Sw Strat
	PORTS_set_pull(GPIOE, GPIO_Pin_10, GPIO_PuPd_UP);	//Sw Strat
	PORTS_set_pull(GPIOE, GPIO_Pin_11, GPIO_PuPd_UP);	//Sw Strat
	PORTS_set_pull(GPIOE, GPIO_Pin_12, GPIO_PuPd_UP);	//BP Selftest
	PORTS_set_pull(GPIOE, GPIO_Pin_13, GPIO_PuPd_UP);	//BP Menu lcd ok
	PORTS_set_pull(GPIOE, GPIO_Pin_14, GPIO_PuPd_UP);	//BP Menu lcd +
	PORTS_set_pull(GPIOE, GPIO_Pin_15, GPIO_PuPd_UP);	//BP Menu lcd -
	PORTS_set_pull(GPIOE, GPIO_Pin_7, GPIO_PuPd_DOWN);	//XBee OFF
}


void big_delay(void)
{
	volatile Uint32 i;
	for(i=0;i<5000000;i++);
}

void test_leds(void){
	GPIO_ResetBits(LED_RUN);
	while(1)
	{
		toggle_led(LED_RUN);				big_delay();
		toggle_led(BLUE_LEDS);				big_delay();
		toggle_led(GREEN_LEDS);				big_delay();
		toggle_led(RED_LEDS);				big_delay();
		toggle_led(LED_ERROR);				big_delay();
		toggle_led(LED_SELFTEST);			big_delay();
		toggle_led(LED_CAN);				big_delay();
		toggle_led(LED_UART);				big_delay();
		toggle_led(LED_USER);				big_delay();
		toggle_led(LED_BEACON_IR_GREEN);	big_delay();
		toggle_led(LED_BEACON_IR_RED);		big_delay();
		//toggle_led(LED_BEACON_US_GREEN);	big_delay();
		//toggle_led(LED_BEACON_US_RED);	big_delay();
	}
}


void test_bp_switchs(void)
{
	debug_printf("Test des Entr�es BP et Switch\n");
	Uint8 sw_debug=2, sw_verbose=2, sw_xbee=2, sw_save=2, sw_color=2, sw_lcd=2, sw_evit=2, sw_strat1=2, sw_strat2=2, sw_strat3=2, port_robot_id=2, biroute=2, bp_run_match=2;

#ifdef FDP_2014
	Uint8 bp_selftest=2, bp_calibration=2, bp_menu_m=2, bp_menu_p=2, bp_print_match=2;
#endif

	while(1)
	{
		if(IHM_switchs_get(SWITCH_DEBUG) 	!= sw_debug		)	{	sw_debug 		= IHM_switchs_get(SWITCH_DEBUG);	debug_printf("sw_debug = %s\n"		, (sw_debug		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_VERBOSE) 	!= sw_verbose	)	{	sw_verbose 		= IHM_switchs_get(SWITCH_VERBOSE);	debug_printf("sw_verbose = %s\n"	, (sw_verbose	)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_XBEE) 	!= sw_xbee		)	{	sw_xbee 		= IHM_switchs_get(SWITCH_XBEE);		debug_printf("sw_xbee = %s\n"		, (sw_xbee		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_SAVE) 	!= sw_save		)	{	sw_save 		= IHM_switchs_get(SWITCH_SAVE);		debug_printf("sw_save = %s\n"		, (sw_save		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_COLOR) 	!= sw_color		)	{	sw_color 		= IHM_switchs_get(SWITCH_COLOR);	debug_printf("sw_color = %s\n"		, (sw_color		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_LCD) 		!= sw_lcd		)	{	sw_lcd	 		= IHM_switchs_get(SWITCH_LCD);		debug_printf("sw_lcd = %s\n"		, (sw_lcd		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_EVIT) 	!= sw_evit		)	{	sw_evit 		= IHM_switchs_get(SWITCH_EVIT);		debug_printf("sw_evit = %s\n"		, (sw_evit		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_STRAT_1) 	!= sw_strat1	)	{	sw_strat1 		= IHM_switchs_get(SWITCH_STRAT_1);	debug_printf("sw_strat1 = %s\n"		, (sw_strat1	)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_STRAT_2) 	!= sw_strat2	)	{	sw_strat2 		= IHM_switchs_get(SWITCH_STRAT_2);	debug_printf("sw_strat2 = %s\n"		, (sw_strat2	)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_STRAT_3) 	!= sw_strat3	)	{	sw_strat3 		= IHM_switchs_get(SWITCH_STRAT_3);	debug_printf("sw_strat3 = %s\n"		, (sw_strat3	)?"ON":"OFF");	 }
		if(PORT_ROBOT_ID 	!= port_robot_id)	{	port_robot_id  	= PORT_ROBOT_ID;	debug_printf("port_robot_id = %s\n"	, (port_robot_id)?"ON":"OFF");	 }
		if(IHM_switchs_get(BIROUTE) 		!= biroute		)	{	biroute 	   	= IHM_switchs_get(BIROUTE);			debug_printf("biroute = %s\n"		, (biroute		)?"ON":"OFF");	 }
		if(BUTTON0_PORT 	!= bp_run_match	)	{	bp_run_match   	= BUTTON0_PORT;		debug_printf("bp_run_match = %s\n"	, (bp_run_match	)?"ON":"OFF");	 }
#ifdef FDP_2014
		if(BUTTON1_PORT 	!= bp_selftest	)	{	bp_selftest    	= BUTTON1_PORT;		debug_printf("bp_selftest = %s\n"	, (bp_selftest	)?"ON":"OFF");	 }
		if(BUTTON2_PORT 	!= bp_calibration)	{	bp_calibration 	= BUTTON2_PORT;		debug_printf("bp_calibration = %s\n", (bp_calibration)?"ON":"OFF");	 }
		if(BUTTON3_PORT 	!= bp_menu_p	)	{	bp_menu_p 		= BUTTON3_PORT;		debug_printf("bp_menu_p = %s\n"		, (bp_menu_p	)?"ON":"OFF");	 }
		if(BUTTON4_PORT 	!= bp_menu_m	)	{	bp_menu_m 		= BUTTON4_PORT;		debug_printf("bp_menu_m = %s\n"		, (bp_menu_m	)?"ON":"OFF");	 }
		if(BUTTON5_PORT 	!= bp_print_match)	{	bp_print_match 	= BUTTON5_PORT;		debug_printf("bp_print_match = %s\n", (bp_print_match)?"ON":"OFF");	 }
#endif
	}
}

void RCON_read(void)
{
#if defined(STM32F40XX)
	debug_printf("STM32F4xx reset source :\n");
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST))
		debug_printf("- Low power management\n");
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
		debug_printf("- Window watchdog time-out\n");
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
		debug_printf("- Independent watchdog time-out\n");
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST))
		debug_printf("- Software reset\n");
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST))
		debug_printf("- POR\n");
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST))
		debug_printf("- Pin NRST\n");
	if(RCC_GetFlagStatus(RCC_FLAG_BORRST))
		debug_printf("- POR or BOR\n");
	RCC_ClearFlag();
#endif
}

