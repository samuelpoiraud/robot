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


//@pre : QS_WHO_I_AM doit être found.
//@pre : le CAN doit être initialisé...
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
	SD_init();

	#ifdef USE_XBEE
		if(QS_WHO_AM_I_get() == TINY)
			CAN_over_XBee_init(ROBOT_1, ROBOT_2);	//Tiny
		else
			CAN_over_XBee_init(ROBOT_2, ROBOT_1);	//Krusty
	#endif
	//#define FLUSH_EEPROM
	#ifdef FLUSH_EEPROM
		EEPROM_CAN_MSG_flush_eeprom();	//Activer ceci permet de vider la mémoire EEPROM lors de l'init (attention, cela peut prendre plusieurs secondes !)
		debug_printf("EEPROM_flushed\n");
	#endif

}

void Supervision_process_main(void)
{	
	static bool_e RTC_time_printed = FALSE;
	static time32_t absolute_time = 0;
//	print_all_msg(); //désolé...
//	EEPROM_CAN_MSG_verbose_all_match_header();


	if(global.env.absolute_time/1000 > absolute_time)
	{
		absolute_time = global.env.absolute_time;
		//Chaque seconde.
		#ifdef USE_XBEE
			CAN_over_XBee_every_second();
		#endif
		SELFTEST_process_1sec();
		//AU bout de la première seconde.
		if(!RTC_time_printed)
		{
			RTC_time_printed = TRUE;
			RTC_print_time();	//Si on rajoute pas ce délai d'une seconde, la RTC n'est pas prête quand on vient la lire.
		}
	}

	/* Gestion du selftest */
	SELFTEST_update(NULL);

	/* Test fiabilité des balises */
	SELFTEST_balise_update();

	#ifdef USE_XBEE
		if(PORT_SWITCH_XBEE)
			CAN_over_XBee_process_main();
	#endif

	SD_process_main();

}



void Supervision_update_led_beacon(CAN_msg_t * can_msg)
{
	switch(can_msg->sid)
	{
		case BEACON_ADVERSARY_POSITION_IR:
			if(global.env.match_started == TRUE)
				//Enregistrement dutype d'erreur
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
				//Enregistrement dutype d'erreur
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








