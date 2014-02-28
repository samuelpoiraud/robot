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
#include "../QS/QS_outputlog.h"
#include "SD/SD.h"
#include "Selftest.h"
#include "Eeprom_can_msg.h"
#include "RTC.h"
#include "Buffer.h"
#include "config_use.h"
#include "config_pin.h"
#include "LCD_interface.h"
#include "Buzzer.h"
#include "Synchro_balises.h"

//@pre : QS_WHO_I_AM doit �tre found.
//@pre : le CAN doit �tre initialis�...
void Supervision_init(void)
{
	SELFTEST_init();
	RTC_init();
	BUFFER_init();
	SYNCHRO_init();

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
		if(QS_WHO_AM_I_get() == SMALL_ROBOT)
			CAN_over_XBee_init(SMALL_ROBOT_MODULE, BIG_ROBOT_MODULE);
		else
			CAN_over_XBee_init(BIG_ROBOT_MODULE, SMALL_ROBOT_MODULE);
	#endif
}
volatile Uint8 flag_1sec = 0;
volatile Uint8 flag_xbee_msg = 0;	//Flag qui indique qu'un envoi de message sur XBee doit �tre fait..
#define PERIOD_XBEE_MSG		100		//[ms]
void Supervision_process_1ms(void)
{
	static Uint16 t_ms = 0;
	static Uint16 t_xbee = 50;
	t_ms++;
	if(t_ms >= 1000)
	{
		t_ms = 0;
		flag_1sec++;
	}

	t_xbee++;
	if(t_xbee >= PERIOD_XBEE_MSG)
	{
		t_xbee = 0;
		flag_xbee_msg++;
	}
}

void SUPERVISION_send_pos_over_xbee(void)
{
	CAN_msg_t msg;
	msg.sid = XBEE_MY_POSITION_IS;
	msg.data[0] = HIGHINT(global.env.pos.x);
	msg.data[1] = LOWINT(global.env.pos.x);
	msg.data[2] = HIGHINT(global.env.pos.y);
	msg.data[3] = LOWINT(global.env.pos.y);
	msg.data[4] = QS_WHO_AM_I_get();
	msg.size = 5;

	CANMsgToXbee(&msg, TRUE);	//Envoi en BROADCAST...aux modules PING�s

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
		//Au bout de la premi�re seconde.
		if(!first_second_elapsed)
		{
			//Si on rajoute pas ce d�lai d'une seconde, la RTC n'est pas pr�te quand on vient la lire.
			first_second_elapsed = TRUE;
			SD_init();
			//A partir de maintenant, on peut loguer sur la carte SD...
			debug_printf("--- Hello, I'm STRAT (%s) ---\n", QS_WHO_AM_I_get_name());
			RTC_print_time();
		}
	}

	if(flag_xbee_msg)
	{
		flag_xbee_msg = 0;
		#ifdef USE_XBEE
			#warning "d�sactiv� pour l'instant, tant que pas test�... ->"
			if(0)	//if(PORT_SWITCH_XBEE)
				SUPERVISION_send_pos_over_xbee();	//Envoi p�riodique de notre position en XBee (cet envoi ne fonctionnera que lorsque le module est initialis�, et qu'aupr�s des modules PING�s)
		#endif
	}
	/* Gestion du selftest */
	SELFTEST_process_main();

	SYNCHRO_process_main();


	#ifdef USE_XBEE
		if(PORT_SWITCH_XBEE)
			CAN_over_XBee_process_main();
	#endif


	/* Mise � jour des informations affich�es � l'�cran */
	#ifdef USE_LCD
		LCD_Update();
	#endif

	SD_process_main();

	BUZZER_process_main();
}











