/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Test_main.c
 *	Package : Carte Balises
 *	Description : fonction principale permettant
 *				l'appel des fonctions de test de la carte balise
 *	Auteur : Christophe
 *	Version 20081209
 */

#define TEST_MAIN_C
#include "main.h"
#include <timer.h>
#include "QS/QS_adc.h"
#include "QS/QS_can.h"
#include "QS/QS_can_over_xbee.h"
#include "QS/QS_buttons.h"
#include "Brain_US.h"
#include "Brain_IR.h"
#include "Secretary.h"
#include "Array_US.h"
#include "Eyes.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"

	void Synchro_master_init(void);
	volatile static Uint16 compteur_secondes = 0;
	volatile static bool_e flag_1sec = FALSE;
	volatile static bool_e flag_50ms = FALSE;
	volatile Uint8 presence_fille1 = 0;
	volatile Uint8 presence_fille2 = 0;
	volatile Uint8 presence_mere = 0;
	volatile Uint8 count_2ms = 0;
	
	

	
	void initialisation (void)
	{

		PORTS_init();
		BUTTONS_init();
		LED_RUN = 1;
		ADC_init();
		TIMER_init();
		UART_init();
		BRAIN_US_init();
		BRAIN_IR_init();
		EYES_init();
		
		#ifdef USE_CAN
			CAN_init();
			BUTTONS_define_actions(BUTTON4, SECRETARY_can_send_beacon_enable_periodic_sending, 0, 0);
			BUTTONS_define_actions(BUTTON3, SECRETARY_can_send_beacon_disable_periodic_sending, 0, 0);
		#endif
	
	//	Beacon_init();
		Synchro_master_init();
		debug_printf("Balise still alive !\n");
	}
	
	#define SEUIL_VBAT_FAIBLE	(Sint16)(450)	// THEORIQUE : Vmesuré = Vbat*47/147*1024/5.   Seuil souhaité = 7.5V => seuil=7.5*47/147=491


	//Envoi un message CAN selon l'encapsulation XBee, sur l'UART2.
	
	void tests(void)
	{
		//ARRAY_US_tests();
		jeu_test_BRAIN_IR_find_ab();
		
	}	


	int main (void)
	{
		Sint16 vbat;
		
		tests();
		initialisation();
		
		
		while(1)
		{
			
			SECRETARY_process_main();
			BUTTONS_update();
			BRAIN_US_process_main();
			BRAIN_IR_process_main();
			
			if(flag_50ms)
			{
				BRAIN_US_timeout_update(50);
				flag_50ms = FALSE;
			}	
			
		
			/*
			if(count_2ms>100)	//Every 100ms.
			{
				count_2ms = 0;
				msg.sid = 0xCAFE;
				msg.size = 0;
				CANMsgToXbee(&msg, (I_AM_MODULE == MODULE_C)?MODULE_2:MODULE_C);	//on cause à l'autre.
			}
			*/
//			process_beacon();
			
			
			if(flag_1sec)
			{
				CAN_over_XBee_every_second();
				flag_1sec = FALSE;
				vbat = ADC_getValue(0);	//Mesure tension batterie...
				if(vbat < SEUIL_VBAT_FAIBLE)
					LED_BAT_FAIBLE = 1;
				else
					LED_BAT_FAIBLE = 0;
			}
	
				
		}
		
		
		while(1)
		{	
			
	
			//S'il est temps d'envoyer les données ou si l'on a recu une donnée à envoyer... on envoi !
			if(global.flags.timer2 == TRUE || global.flag_envoi == TRUE)
				process_envoi();
				
			if(global.flags.timer1 == TRUE)
			{
				global.flags.timer1 = FALSE;
				debug_printf("Timer1, erreur détection\n");	
			}	
			
			//Si une IT s'est déclenchée depuis le dernier passage dans la boucle de main...
//			if(global.flags.it)
//				process_it();


			if(global.flags.timer2 == TRUE)
			{
				global.flags.timer2 = FALSE;
				static Uint8 i, balise, j;
				i = (i + 1) %4;
			/*	if(!i)
					for(balise=0;balise<3;balise++)
						for(j=0;j<NOMBRE_DETECTIONS_PAR_BALISE;j++)	
							debug_printf("b%d\td%d:\t%d\t%d\n",balise,j, detections[balise][j].debut,detections[balise][j].fin);	
			*/
			/*
				if(!i)
					for(balise=0;balise<3;balise++)
						for(j=0;j<NOMBRE_CYCLES_SAUVES;j++)	
							debug_printf("b%d\tc%d:\ti%d\n",balise,j, global.buffer_instants[balise][j]);
			*/				
			}	
		}
		
		return 0;		
	}

		
	#define PERIODE_FLASH					50		//Période de répétition des flashs [nombre de step]	//Période du flash en µs = PERIODE_FLASH * DUREE_STEP
	#define NOMBRE_BALISES_EMETTRICES		2
	#define DUREE_STEP						2000	//Durée d'un step [us]
	#define	NOMBRE_IT_PAR_SECONDES			500
	void Synchro_master_init(void)
	{
		SetPriorityIntT1(7);
		TIMER1_run_us(DUREE_STEP);
	}
		
	void _ISR _T1Interrupt()
	{
		static Uint16 step = 0;
		static Uint8 count_for_flag_50ms = 0;
		bool_e synchro;
		
		synchro = (step == 0)?1:0;
		//Maitre de la synchronisation de toutes les balises.
		SYNCHRO_1 = synchro;	//Signal de synchro ! à 1 pendant le premier step...
		SYNCHRO_2 = synchro;	//Signal de synchro ! à 1 pendant le premier step...
		SYNCHRO_3 = synchro;	//Signal de synchro ! à 1 pendant le premier step...
		if(synchro)
		{
//			LED_RUN = !LED_RUN;
#warning "rétablir led_run sur synchro"
		}
		
		if(step == 0)
			EYES_step(ADVERSARY_1);	
		if(step == PERIODE_FLASH)
			EYES_step(ADVERSARY_2);	
		
		step++;
	
		if(step > PERIODE_FLASH*NOMBRE_BALISES_EMETTRICES - 1)	//Période de 100ms...* nb balises émettrices = 200ms = (5 mesures par seconde !) 
			step = 0;
		
		count_2ms++;
		
		count_for_flag_50ms = (count_for_flag_50ms>=50)?0:(count_for_flag_50ms+1);
		if(!count_for_flag_50ms)
			flag_50ms = TRUE;
		
		compteur_secondes = (compteur_secondes>=NOMBRE_IT_PAR_SECONDES)?0:(compteur_secondes+1);
		if(!compteur_secondes)
			flag_1sec = TRUE;
	
		
		BUTTONS_process_it();
		
		IFS0bits.T1IF	= FALSE;
	}	
	
