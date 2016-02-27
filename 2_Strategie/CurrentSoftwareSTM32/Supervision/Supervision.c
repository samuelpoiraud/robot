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
#include "RTC.h"
#include "Buffer.h"
#include "LCD_interface.h"
#include "Buzzer.h"
#include "Synchro_balises.h"
#include "../QS/QS_IHM.h"

//@pre : QS_WHO_I_AM doit être found.
//@pre : le CAN doit être initialisé...
void Supervision_init(void)
{
	SELFTEST_init();
	RTC_init();
	BUFFER_init();
	//SYNCHRO_init();

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
volatile Uint8 flag_xbee_msg = 0;	//Flag qui indique qu'un envoi de message sur XBee doit être fait..
#define PERIOD_XBEE_MSG		500		//[ms]
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
	CAN_over_XBee_process_ms();
}

/*
 * @brief Demande à la prop l'envoie périodique tout les param ms
 */
void Supervision_send_periodically_pos(Uint16 dist, Sint16 angle){
	CAN_msg_t msg;
	msg.sid = PROP_SEND_PERIODICALLY_POSITION;
	msg.size = SIZE_PROP_SEND_PERIODICALLY_POSITION;
	msg.data.prop_send_periodically_position.translation = dist;
	msg.data.prop_send_periodically_position.rotation = angle;
	msg.data.prop_send_periodically_position.period = 0;
	CAN_send(&msg);
}


void SUPERVISION_send_pos_over_xbee(void)
{
	CAN_msg_t msg;
	msg.sid = XBEE_MY_POSITION_IS;
	msg.size = SIZE_XBEE_MY_POSITION_IS;
	msg.data.xbee_my_position_is.x = global.pos.x;
	msg.data.xbee_my_position_is.y = global.pos.y;
	msg.data.xbee_my_position_is.robot_id = QS_WHO_AM_I_get();
	CANMsgToXbee(&msg, TRUE);	//Envoi en BROADCAST...aux modules PINGés
}

void Supervision_process_main(void)
{
	static color_e local_color = -1;
	static bool_e first_second_elapsed = FALSE;
	static bool_e end_of_blink_sent = FALSE;
	static bool_e disable_already_said = FALSE;
	bool_e current_destination_reachable;
	bool_e led_color_initialize = FALSE;

	if(flag_1sec)
	{
		flag_1sec--;
		//Chaque seconde.
		//Au bout de la première seconde.
		if(!first_second_elapsed)
		{
			led_color_initialize = TRUE;
			//Si on rajoute pas ce délai d'une seconde, la RTC n'est pas prête quand on vient la lire.
			first_second_elapsed = TRUE;
			SD_init();

			Supervision_send_periodically_pos(1, PI4096/180); // Tous les milimetres et degrés: ca flood mais on est pas en match donc pas déplacment

			//A partir de maintenant, on peut loguer sur la carte SD...
			RTC_print_time();
		}
	}

	if(flag_xbee_msg)
	{
		flag_xbee_msg = 0;
		#ifdef USE_XBEE
			// "Code désactivé car trop lourd sur le XBee"
			if(0)	//if(IHM_switchs_get(SWITCH_XBEE))
				SUPERVISION_send_pos_over_xbee();	//Envoi périodique de notre position en XBee (cet envoi ne fonctionnera que lorsque le module est initialisé, et qu'auprès des modules PINGés)
		#endif
	}
	/* Gestion du selftest*/
	SELFTEST_process_main();

	//SYNCHRO_process_main();


	#ifdef USE_XBEE
		if(IHM_switchs_get(SWITCH_XBEE))
		{
			disable_already_said = FALSE;
			CAN_over_XBee_process_main();
			current_destination_reachable = XBee_is_destination_reachable();

			if(led_color_initialize || (current_destination_reachable && !end_of_blink_sent))
			{
				if(current_destination_reachable)
				{
					IHM_leds_send_msg(1,(led_ihm_t){LED_COLOR_IHM, ON});
					end_of_blink_sent = TRUE;
				}
				else
					IHM_leds_send_msg(1,(led_ihm_t){LED_COLOR_IHM, SPEED_BLINK_4HZ});
			}
			//Si on a pas de lien XBEE avec l'autre Robot : les leds clignotent.
			//ATTENTION, si l'on désactive après allumage le XBEE sur l'un des robot... l'autre robot qui a eu le temps de dialoguer en XBEE ne clignotera pas !
		}else{
			if(disable_already_said == FALSE){
				disable_already_said = TRUE;
				end_of_blink_sent = FALSE;
				IHM_leds_send_msg(1,(led_ihm_t){LED_COLOR_IHM, SPEED_BLINK_4HZ});
			}
		}
	#endif

	if(global.color != local_color || led_color_initialize)
	{
		IHM_set_led_color((global.color == BOT_COLOR)?LED_COLOR_MAGENTA:LED_COLOR_GREEN);
		local_color = global.color;
	}

	/* Mise à jour des informations affichées à l'écran*/
	#ifdef USE_LCD
		LCD_Update();
	#endif

	SD_process_main();

	BUZZER_process_main();
}











