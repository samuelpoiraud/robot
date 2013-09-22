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

#define SUPERVISION_C
#include "Supervision.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_can_over_xbee.h"
#include "SD/SD.h"
#include "Selftest.h"
#include "Eeprom_can_msg.h"
#include "RTC.h"
#include "Buffer.h"


//@pre : QS_WHO_I_AM doit �tre found.
//@pre : le CAN doit �tre initialis�...
void Supervision_init(void)
{
	SELFTEST_init();
	RTC_init();
	BUFFER_init();

	EEPROM_HOLD = 1;
	EEPROM_CS = 1;
	#ifdef EEPROM_CAN_MSG_ENABLE
		EEPROM_CAN_MSG_init();
	#endif
	//#define FLUSH_EEPROM
	#ifdef FLUSH_EEPROM
		EEPROM_CAN_MSG_flush_eeprom();	//Activer ceci permet de vider la m�moire EEPROM lors de l'init (attention, cela peut prendre plusieurs secondes !)
		debug_printf("EEPROM_flushed\n");
	#endif

	#ifdef USE_LCD
		init_LCD_interface();
	#endif


	#ifdef USE_XBEE
		if(QS_WHO_AM_I_get() == TINY)
			CAN_over_XBee_init(ROBOT_1, ROBOT_2);	//Tiny
		else
			CAN_over_XBee_init(ROBOT_2, ROBOT_1);	//Krusty
	#endif
}

volatile Uint8 flag_1sec = 0;
void Supervision_process_1sec(void)
{
	flag_1sec++;
}

void Supervision_process_main(void)
{	
	static bool_e first_second_elapsed = FALSE;
//	print_all_msg(); //d�sol�...
//	EEPROM_CAN_MSG_verbose_all_match_header();


	if(flag_1sec)
	{
		flag_1sec--;
		//Chaque seconde.
		#ifdef USE_XBEE
			CAN_over_XBee_every_second();
		#endif
		SELFTEST_process_1sec();
		//Au bout de la premi�re seconde.
		if(!first_second_elapsed)
		{
			first_second_elapsed = TRUE;
			RTC_print_time();	//Si on rajoute pas ce d�lai d'une seconde, la RTC n'est pas pr�te quand on vient la lire.
			SD_init();
			//A partir de maintenant, on peut loguer sur la carte SD...
			SD_printf("Hello, I am %s\n", ((QS_WHO_AM_I_get() == TINY)?"TINY":"KRUSTY"));
		}
	}

	/* Gestion du selftest */
	SELFTEST_update(NULL);

	/* Test fiabilit� des balises */
	SELFTEST_balise_update();

	#ifdef USE_XBEE
		if(PORT_SWITCH_XBEE)
			CAN_over_XBee_process_main();
	#endif


	/* Mise � jour des informations affich�es � l'�cran */
	#ifdef USE_LCD
		LCD_Update();
	#endif

	SD_process_main();

}



void Supervision_update_led_beacon(CAN_msg_t * can_msg)
{
	switch(can_msg->sid)
	{
		case BEACON_ADVERSARY_POSITION_IR:
			if(global.env.match_started == TRUE)
				//Enregistrement du type d'erreur
				error_counters_update(can_msg);
			//Si le message d'erreur n'est pas nul autrement dit si il y a une erreur quelconque
			if(can_msg->data[0] || can_msg->data[4])
				led_ir_update(BEACON_ERROR);
			else if(can_msg->data[3] < 102 || can_msg->data[7] < 102) //Distance IR en cm
				led_ir_update(BEACON_NEAR);
			else
				led_ir_update(BEACON_FAR);
			break;

		case BEACON_ADVERSARY_POSITION_US:
			if(global.env.match_started == TRUE)
				//Enregistrement du type d'erreur
				error_counters_update(can_msg);
			//Si le message d'erreur n'est pas nul autrement dit si il y a une erreur quelconque
			if(can_msg->data[0] || can_msg->data[4])
				led_us_update(BEACON_ERROR);
			else if(can_msg->data[1] < 4 || can_msg->data[5] < 4) //Distance US en mm on test seulement le poids fort autrement dit 1024mm
				led_us_update(BEACON_NEAR);
			else
				led_us_update(BEACON_FAR);
			break;
		default:
			break;
	}
}








