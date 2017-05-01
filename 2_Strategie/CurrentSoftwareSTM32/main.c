/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : main.c
 *	Package : Carte Principale
 *	Description : Ordonnancement de la carte Principale
 *	Auteur : Jacen, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "main.h"

#ifdef STM32F40XX
	#include "QS/QS_sys.h"
	#include "stm32f4xx.h"
	#include "stm32f4xx_gpio.h"
#endif
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_buttons.h"
#include "QS/QS_IHM.h"
#include "QS/QS_adc.h"
#include "QS/QS_rcc.h"
#include "QS/QS_mosfet.h"
#include "QS/QS_lcd_over_uart.h"
#include "actuator/queue.h"
#include "environment.h"
#include "brain.h"
#include "clock.h"
#include "zone_mutex.h"
#include "actuator/queue.h"
#include "actuator/act_functions.h"
#include "actuator/act_avoidance.h"
#include "Supervision/Supervision.h"
#include "propulsion/astar.h"
#include "elements.h"
#include "avoidance.h"
#include "strats_2017/big/action_big.h"
#include "strats_2017/small/action_small.h"
#include "strats_2017/actions_both_2017.h"

void test_bp_switchs(void);
void test_leds(void);
static void MAIN_sensor_test();


void process_measure_loop_duration(void)
{
	static time32_t previous_time = 0;
	if(global.absolute_time > 0)
	{
		//Décommenter ce printf pour afficher la mesure de la durée de la boucle.
		//ATTENTION, cet affichage est hors de la mesure... (donc elle impacte le code hors de cette mesure).
		if(global.absolute_time - previous_time > 9)
			debug_printf("L%d\n",(Uint16)(global.absolute_time - previous_time));
		previous_time = global.absolute_time;
	}
}

void security_stack(void)
{
	static bool_e initialized = FALSE;
	static bool_e verbosed = FALSE;
	static uint8_t * ptr;
	extern uint8_t _susrstack;
	if(!initialized)
	{
		ptr = &_susrstack;
		*ptr = 0x55;
		initialized = TRUE;
	}
	if(*ptr != 0x55 && !verbosed)
	{
		fatal_printf("Stack overflowed\n");
		verbosed = TRUE;
	}
}

void tests(void)
{
	//test_bp_switchs();
	//test_leds();
}

int main (void)
{
	SYS_init();		// Init système

#ifdef DISABLE_SECURE_GPIO_INIT
	PORTS_init();	// Config des ports
#else
	if(PORTS_secure_init() == FALSE){
		error_printf("Blocage car le code ne démarre pas sur le bon slot !\n");
		while(1);
	}
#endif

	UART_init();
	//RCC_read();

	IHM_init(&global.flags.match_started);
#ifdef MAIN_IR_RCVA
	main_ir_rcva();
#endif
	ENV_init();	//Pour être réceptif aux éventuels messages CAN envoyés très tôt...

	GPIO_SetBits(LED_RUN);

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//Détermine le robot sur lequel est branchée la carte.
	debug_printf("------- Hello, I'm STRAT (%s) -------\n", QS_WHO_AM_I_get_name());

	QUEUE_init();

	ADC_init();

	// Si le démarrage de la carte n'est pas consécutif à l'arrivée de l'alimentation
	//		On reset les autres cartes
	if(RCC_board_just_reboot())
		CAN_send_sid(BROADCAST_RESET);

	CLOCK_init();
	debug_printf("\n-------\nWaiting for other boards ready\n-------\n");
	//retard pour attendre l'initialisation des autres cartes
	// voir si on peut faire mieux
	volatile Uint16 i, j;
	for(j=0;j<100;j++)
	{
		for(i=1;i;i++);
		if(j%10 == 0x00)
			debug_printf(".");
	}
	debug_printf("\n");


	if(sizeof(msg_can_formated_u) > 8){
		printf("Bloquage du main pour un dépassement de taille des données formatées !!!\n");
		display(sizeof(msg_can_formated_u));
		while(1);
	}

	Supervision_init();
	BRAIN_init();
	ACT_AVOIDANCE_init();
	ELEMENTS_init();
	FOE_ANALYSER_init();
	ColorSensor_init();

	// Demande des états initiaux des switchs
	CAN_send_sid(IHM_GET_SWITCH);

	#ifdef USE_LCD_OVER_UART
		LCD_OVER_UART_init();
	#endif

	// ATTENTION à ce moment, la couleur n'a pas encore été initialisée

	while(1)
	{
		toggle_led(LED_RUN);
		security_stack();

		/* mise à jour de l'environnement */
		ENV_update();

		/* Code concernant la supervision */
		Supervision_process_main();

		/* Execution des routines de supervision des piles
		actionneurs */
		QUEUE_run();

		//Machine à état QS_mosfet
		#if defined(USE_MOSFETS_MODULE) && defined(USE_MOSFET_MULTI)
			MOSFET_do_order_multi(NULL);
		#endif

		MAIN_sensor_test();

		FOE_ANALYSER_process_main();

		any_match();

		// Subactions actionneurs parallèles 2017
		if(I_AM_BIG()){
			sub_act_harry_mae_store_modules(MODULE_STOCK_LEFT, FALSE);
			sub_act_harry_mae_store_modules(MODULE_STOCK_RIGHT, FALSE);
			sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_LEFT, FALSE);
			sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, FALSE);
		}else{
			sub_act_anne_mae_store_modules(FALSE);
			sub_act_anne_mae_prepare_modules_for_dispose(FALSE);
		}

		OUTPUTLOG_process_main();

		process_measure_loop_duration();

		if(global.flags.match_started == TRUE && global.flags.match_over == FALSE){
			ZONES_process_main();
		}

	}
	return 0;
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
		toggle_led(LED_ERROR);				big_delay();
		toggle_led(LED_SELFTEST);			big_delay();
		toggle_led(LED_CAN);				big_delay();
		toggle_led(LED_UART);				big_delay();
		toggle_led(LED_USER);				big_delay();
	}
}


void test_bp_switchs(void)
{
	debug_printf("Test des Entrées BP et Switch\n");
	Uint8 sw_debug=2, sw_verbose=2, sw_xbee=2, sw_color=2, sw_lcd=2, sw_evit=2, port_robot_id=2, biroute=2, bp_run_match=2;

	while(1)
	{
		if(IHM_switchs_get(SWITCH_RAW_DATA) != sw_debug		)	{	sw_debug 		= IHM_switchs_get(SWITCH_RAW_DATA);	debug_printf("sw_raw_data = %s\n"		, (sw_debug		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_VERBOSE) 	!= sw_verbose	)	{	sw_verbose 		= IHM_switchs_get(SWITCH_VERBOSE);	debug_printf("sw_verbose = %s\n"	, (sw_verbose	)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_XBEE) 	!= sw_xbee		)	{	sw_xbee 		= IHM_switchs_get(SWITCH_XBEE);		debug_printf("sw_xbee = %s\n"		, (sw_xbee		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_COLOR) 	!= sw_color		)	{	sw_color 		= IHM_switchs_get(SWITCH_COLOR);	debug_printf("sw_color = %s\n"		, (sw_color		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_LCD) 		!= sw_lcd		)	{	sw_lcd	 		= IHM_switchs_get(SWITCH_LCD);		debug_printf("sw_lcd = %s\n"		, (sw_lcd		)?"ON":"OFF");	 }
		if(IHM_switchs_get(SWITCH_EVIT) 	!= sw_evit		)	{	sw_evit 		= IHM_switchs_get(SWITCH_EVIT);		debug_printf("sw_evit = %s\n"		, (sw_evit		)?"ON":"OFF");	 }
		if(IHM_switchs_get(BIROUTE) 		!= biroute		)	{	biroute 	   	= IHM_switchs_get(BIROUTE);			debug_printf("biroute = %s\n"		, (biroute		)?"ON":"OFF");	 }
		if(BUTTON0_PORT 					!= bp_run_match	)	{	bp_run_match   	= BUTTON0_PORT;						debug_printf("bp_run_match = %s\n"	, (bp_run_match	)?"ON":"OFF");	 }
		if(PORT_ROBOT_ID 					!= port_robot_id)	{	port_robot_id  	= PORT_ROBOT_ID;					debug_printf("port_robot_id = %s\n"	, (port_robot_id)?"ON":"OFF");	 }
	}
}

static void MAIN_sensor_test(){
	static bool_e led_on = FALSE;
	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		if(RECALAGE_AR_G || RECALAGE_AR_D /*|| RECALAGE_AV_G || RECALAGE_AV_D*/){
			if(led_on == FALSE){
				IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, ON});
				led_on = TRUE;
			}
		}else if(led_on == TRUE){
			IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, OFF});
			led_on = FALSE;
		}
	}else{
		if(RECALAGE_AV_G || RECALAGE_AV_D ){
			if(led_on == FALSE){
				IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, ON});
				led_on = TRUE;
			}
		}else if(led_on == TRUE){
			IHM_leds_send_msg(1, (led_ihm_t){LED_SENSOR_TEST, OFF});
			led_on = FALSE;
		}
	}
}




#ifdef MAIN_IR_RCVA

#define NB_ECH	10000
void main_ir_rcva(void)
{
	static uint16_t buffer_adc[NB_ECH];

	typedef enum
	{
		INIT = 0,
		WAIT_BP,
		ACQUIRE,
		PRINT
	}state_e;
	static state_e state = INIT;
	uint16_t value;
	static uint16_t previous_value;
	uint32_t i;
	uint32_t index;

	static time32_t time_begin, time_end;

	while(1)
	{
		switch(state)
		{
			case INIT:
				QS_WHO_AM_I_find();	//Détermine le robot sur lequel est branchée la carte.
				ADC_init();
				RTC_init();
				CLOCK_init();	//TEMPORAIRE !!!
				index = 0;
				state = WAIT_BP;
				SD_init();
				PWM_init();
				PWM_set_frequency(450000);
				PWM_run(50,1);

				break;
			case WAIT_BP:
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
				{
					time_begin = global.absolute_time;
					state = ACQUIRE;
				}


				while(UART1_data_ready())
					{
						char_from_user(UART1_get_next_msg());
					}



				SD_process_main();
				break;
			case ACQUIRE:
				value = ADC_getValue(ADC_PHOTOTRANSISTOR);
				if(value && (value != previous_value))
				{
					buffer_adc[index] = value;
					previous_value = value;
					index++;
				}

				if(index>=NB_ECH)
				{
					index = 0;
					time_end = global.absolute_time;
					state = PRINT;
				}

				break;
			case PRINT:
				SD_printf("t:%d | %d\n",time_begin,time_end-time_begin);
				printf("t:%d | %d\n",time_begin,time_end-time_begin);
				for(i = 0; i<NB_ECH; i++)
					SD_printf("%d,",buffer_adc[i]);
				state = WAIT_BP;
				break;
		}
	}

}

#endif
