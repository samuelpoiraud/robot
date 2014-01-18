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
#include "../QS/QS_OUTPUTlog.h"
#include "SD/SD.h"
#include "Selftest.h"
#include "Eeprom_can_msg.h"
#include "RTC.h"
#include "Buffer.h"
#include "config_use.h"
#include "config_pin.h"
#include "LCD_interface.h"


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
	//#define FLUSH_EEPROM
	#ifdef FLUSH_EEPROM
		EEPROM_CAN_MSG_flush_eeprom();	//Activer ceci permet de vider la mémoire EEPROM lors de l'init (attention, cela peut prendre plusieurs secondes !)
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
//	print_all_msg(); //désolé...
//	EEPROM_CAN_MSG_verbose_all_match_header();


	if(flag_1sec)
	{
		flag_1sec--;
		//Chaque seconde.
		#ifdef USE_XBEE
			CAN_over_XBee_every_second();
		#endif
		//Au bout de la première seconde.
		if(!first_second_elapsed)
		{
			first_second_elapsed = TRUE;
			RTC_print_time();	//Si on rajoute pas ce délai d'une seconde, la RTC n'est pas prête quand on vient la lire.
			SD_init();
			//A partir de maintenant, on peut loguer sur la carte SD...
			debug_printf("Hello, I am %s\n", ((QS_WHO_AM_I_get() == TINY)?"TINY":"KRUSTY"));
		}
	}

	/* Gestion du selftest */
	SELFTEST_process_main();


	#ifdef USE_XBEE
		if(PORT_SWITCH_XBEE)
			CAN_over_XBee_process_main();
	#endif


	/* Mise à jour des informations affichées à l'écran */
	#ifdef USE_LCD
		LCD_Update();
	#endif

	SD_process_main();

}











