/*
 *	Club Robot ESEO 2009 - 2010 - 2012 - 2015
 *
 *
 *	Fichier : secretary.c
 *	Package : Balises Receptrice
 *	Description : Gestion du CAN sur la carte Balise Recepteur
 *	Auteur : Nirgal(PIC) / Arnaud(STM32)
 *	Version 201203
 */

#include "secretary.h"
#include "QS/QS_can.h"
#include "brain.h"
#include "Synchro.h"
#include "eyes.h"
#include "QS/QS_can_over_uart.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_rf.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can_verbose.h"
#include "leds.h"

volatile Uint32 periodic_sending_enabled;	//activation de l'envoi périodique pour les X prochaines [ms]



bool_e SECRETARY_msg_processing_direct_treatment_function(CAN_msg_t* msg);
void SECRETARY_process_msg(CAN_msg_t * msg);
void SECRETARY_send_adversary_location(void);
void SECRETARY_selftest(void);
void SECRETARY_send_pong(void);
void SECRETARY_couleur(CAN_msg_t *msg);
void SECRETARY_warning_battery(CAN_msg_t *msg);


void SECRETARY_init(void)
{
	#ifdef USE_CAN
		CAN_init();
		CAN_set_direct_treatment_function(SECRETARY_msg_processing_direct_treatment_function);
	#endif

	global.flags.start_of_match = FALSE;
	global.flags.end_of_match = FALSE;
	global.flags.flag_100ms = FALSE;
	periodic_sending_enabled = 0;
}

volatile Uint8 t_adversary1_is_seen = 0;
volatile Uint8 t_adversary2_is_seen = 0;
volatile Uint8 t_synchro_received = 0;
volatile bool_e flag_new_datas_available = FALSE;
volatile bool_e warner_battery_foe_1 = FALSE;
volatile bool_e warner_battery_foe_2 = FALSE;

#define ADVERSARY_SEEN_TIMEOUT		20 //[100 ms]	//2 secondes
#define SYNCHRO_RECEIVED_TIMEOUT	20 //[100 ms]	//2 secondes

void SECRETARY_process_it_100ms(void)
{
	volatile adversary_location_t * p_adversary_location;

	global.flags.flag_100ms = TRUE;
	periodic_sending_enabled = (periodic_sending_enabled>100)?periodic_sending_enabled-100:0;

	p_adversary_location = BRAIN_get_adversary_location();
	if(p_adversary_location[ADVERSARY_1].error == AUCUNE_ERREUR || p_adversary_location[ADVERSARY_1].error == SIGNAL_INSUFFISANT)
		t_adversary1_is_seen = ADVERSARY_SEEN_TIMEOUT;
	if(p_adversary_location[ADVERSARY_2].error == AUCUNE_ERREUR || p_adversary_location[ADVERSARY_2].error == SIGNAL_INSUFFISANT)
		t_adversary2_is_seen = ADVERSARY_SEEN_TIMEOUT;
	if(SYNCHRO_is_synchronized())
		t_synchro_received = SYNCHRO_RECEIVED_TIMEOUT;

	if(t_adversary1_is_seen)
		t_adversary1_is_seen--;
	if(t_adversary2_is_seen)
		t_adversary2_is_seen--;
	if(t_synchro_received)
		t_synchro_received--;
}

void SECRETARY_set_new_datas_available(void)
{
	flag_new_datas_available = TRUE;
}

bool_e SECRETARY_msg_processing_direct_treatment_function(CAN_msg_t* msg)
{
	bool_e bret = FALSE;
	switch(msg->sid)
	{
		//Un appel explicite à ce message est nécessaire pour déclencher l'envoi périodique.
		case BEACON_ENABLE_PERIODIC_SENDING:
			periodic_sending_enabled = (Uint32)(100000);	//Activation pour 100 prochaines secondes
		break;
		case BROADCAST_STOP_ALL:
			global.flags.end_of_match = TRUE;
			break;
		case BEACON_DISABLE_PERIODIC_SENDING:
			periodic_sending_enabled = 0;
		break;
		case BEACON_DO_SELFTEST:	//no break;
		case BEACON_PING:
			bret = TRUE;	//on laisse passer le message... qui sera traité dans le main !
		break;
		default:
			bret = TRUE;    //on laisse passer tous les autres messages
		break;
	}

	return bret;	//Inutile de mettre les messages CAN en buffer...
	//Ce code permet d'ignorer totalement l'ensemble des messages BROADCAST qui nous arrivent...
	//Et ainsi de gagner du temps en enregistrant pas bettement ces messages inutiles dans le buffer...
}


void SECRETARY_process_msg(CAN_msg_t * msg)
{
	SECRETARY_msg_processing_direct_treatment_function(msg);
	switch(msg->sid)
	{
		case BROADCAST_RESET:
			NVIC_SystemReset();
		break;
		case BROADCAST_START:
			global.flags.start_of_match = TRUE;
		break;
		case BEACON_DO_SELFTEST:
			SECRETARY_selftest();
		break;
		case BEACON_PING:
			SECRETARY_send_pong();
		break;
		case BROADCAST_COULEUR:
			SECRETARY_couleur(msg);
		break;
		case STRAT_PROP_FOE_DETECTED:
			LEDS_set_infos_evitement(msg);
		break;
		case  BEACON_WARNING_LOW_BATTERY:
			SECRETARY_warning_battery(msg);
		break;
		default:
		break;
	}
}


void SECRETARY_process_main(void)
{

	/*-------------------------------------
		Réception CAN et exécution
	-------------------------------------*/

	CAN_msg_t msg;

	#ifdef USE_RF
		while(RF_can_data_ready()){
			msg = RF_can_get_next_msg();
			SECRETARY_process_msg(&msg);		// Traitement du message pour donner les consignes à la machine d'état
			#ifdef CAN_VERBOSE_MODE
				QS_CAN_VERBOSE_can_msg_print(&msg, VERB_INPUT_MSG);
			#endif
		}
	#endif

	while(CAN_data_ready()){
		msg = CAN_get_next_msg();
		toggle_led(LED_CAN);
		SECRETARY_process_msg(&msg);		// Traitement du message pour donner les consignes à la machine d'état
		#ifdef CAN_VERBOSE_MODE
			QS_CAN_VERBOSE_can_msg_print(&msg, VERB_INPUT_MSG);
		#endif
	}

	if(flag_new_datas_available && periodic_sending_enabled != 0)
	{
		flag_new_datas_available = FALSE;
		SECRETARY_send_adversary_location();
	}
}



void SECRETARY_send_adversary_location(void)
{
	CAN_msg_t msg;
	volatile adversary_location_t * p_adversary_location;
	p_adversary_location = BRAIN_get_adversary_location();

	msg.sid = BROADCAST_BEACON_ADVERSARY_POSITION_IR;
	msg.size = SIZE_BROADCAST_BEACON_ADVERSARY_POSITION_IR;

	msg.data.broadcast_beacon_adversary_position_ir.adv[0].error = p_adversary_location[ADVERSARY_1].error;	//Si !=0 c'est qu'il y a une erreur ! (et on sait laquelle dans le debug msg can !)
	msg.data.broadcast_beacon_adversary_position_ir.adv[0].angle = p_adversary_location[ADVERSARY_1].angle;
	if(p_adversary_location[ADVERSARY_1].error == AUCUNE_ERREUR || p_adversary_location[ADVERSARY_1].error == SIGNAL_INSUFFISANT)
		msg.data.broadcast_beacon_adversary_position_ir.adv[0].dist = (Uint8)p_adversary_location[ADVERSARY_1].distance;
	else
		msg.data.broadcast_beacon_adversary_position_ir.adv[0].dist = 0xFF;

	msg.data.broadcast_beacon_adversary_position_ir.adv[1].error = p_adversary_location[ADVERSARY_2].error;	//Si !=0 c'est qu'il y a une erreur ! (et on sait laquelle dans le debug msg can !)
	msg.data.broadcast_beacon_adversary_position_ir.adv[1].angle = p_adversary_location[ADVERSARY_2].angle;
	if(p_adversary_location[ADVERSARY_2].error == AUCUNE_ERREUR || p_adversary_location[ADVERSARY_2].error == SIGNAL_INSUFFISANT)
		msg.data.broadcast_beacon_adversary_position_ir.adv[1].dist = (Uint8)p_adversary_location[ADVERSARY_2].distance;
	else
		msg.data.broadcast_beacon_adversary_position_ir.adv[1].dist = 0xFF;

	//debug_printf(" Adv1 dist=%d angle=%d error=%d         Adv2 dist=%d angle=%d error=%d\n", msg.data.broadcast_beacon_adversary_position_ir.adv[0].dist, msg.data.broadcast_beacon_adversary_position_ir.adv[0].angle, msg.data.broadcast_beacon_adversary_position_ir.adv[0].error,msg.data.broadcast_beacon_adversary_position_ir.adv[1].dist, msg.data.broadcast_beacon_adversary_position_ir.adv[1].angle, msg.data.broadcast_beacon_adversary_position_ir.adv[1].error);
	#ifdef USE_CAN
		CAN_send(&msg);
	#endif
	#ifdef CAN_SEND_OVER_UART1
		#warning "envois des messages can dupliqués sur UART"
		CANmsgToU1tx(&msg);
	#endif
	#ifdef VERBOSE_CAN_OVER_UART1
		debug_printf("sid=%x|e=%x|a=%d°\t|d=%d\n",message.sid, global.localisation_erreur, /*(Sint16)global.adverse_angle, */(Sint16)(((Sint32)global.adverse_angle*180)/PI4096),(Uint16)global.adverse_distance/10);
	#endif
}



void SECRETARY_selftest(void)
{
	CAN_msg_t msg;
	Uint8 i;
	msg.sid = STRAT_BEACON_SELFTEST_DONE;
	i = 0;

	if(t_adversary1_is_seen == 0)
		msg.data.strat_beacon_selftest_done.error_code[i++] = SELFTEST_BEACON_ADV1_NOT_SEEN;   //L'adversaire 1 n'a pas été vu depuis plus de 2 secondes

	if(t_adversary2_is_seen == 0)
		msg.data.strat_beacon_selftest_done.error_code[i++] = SELFTEST_BEACON_ADV2_NOT_SEEN;	//L'adversaire 2 n'a pas été vu depuis plus de 2 secondes

	if(warner_battery_foe_1)
		msg.data.strat_beacon_selftest_done.error_code[i++] = SELFTEST_BEACON_ADV1_BATTERY_LOW;  //La batterie de la balise sur l'dversaire 1 est faible

	if(warner_battery_foe_2)
		msg.data.strat_beacon_selftest_done.error_code[i++] = SELFTEST_BEACON_ADV2_BATTERY_LOW;  //La batterie de la balise sur l'dversaire 2 est faible

	if(t_synchro_received == 0)
		msg.data.strat_beacon_selftest_done.error_code[i++] = SELFTEST_BEACON_SYNCHRO_NOT_RECEIVED;	//Nous n'avons pas reçu de synchro depuis 2 secondes

	msg.data.strat_beacon_selftest_done.error_code[i++] = SELFTEST_NO_ERROR;
	msg.size = i;

#ifdef USE_CAN
	CAN_send(&msg);
#endif
#ifdef CAN_SEND_OVER_UART1
	CANmsgToU1tx(&msg);
#endif
#ifdef VERBOSE_CAN_OVER_UART1
	if(t_adversary1_is_seen == 0)
		debug_printf("SELFTEST ERROR : SELFTEST_BEACON_ADV1_NOT_SEEN\n");
	if(t_adversary2_is_seen == 0)
		debug_printf("SELFTEST ERROR : SELFTEST_BEACON_ADV2_NOT_SEEN\n");
	if(t_synchro_received == 0)
		debug_printf("SELFTEST ERROR : SELFTEST_BEACON_SYNCHRO_NOT_RECEIVED\n");
#endif
}

void SECRETARY_send_pong(void)
{
	CAN_msg_t msg;
	msg.sid = STRAT_BEACON_PONG;
	msg.size = 0;
	#ifdef USE_CAN
		CAN_send(&msg);
	#endif
	#ifdef CAN_SEND_OVER_UART1
		CANmsgToU1tx(&msg);
	#endif
	#ifdef VERBOSE_CAN_OVER_UART1
		debug_printf("pong\n");
	#endif
}

void SECRETARY_couleur(CAN_msg_t *msg){
	if(msg->sid == BROADCAST_COULEUR){
		global.color = msg->data.broadcast_couleur.color;
		if(msg->data.broadcast_couleur.color == BOT_COLOR)
			LEDS_set_team_color(LEDS_BLUE);
		else
			LEDS_set_team_color(LEDS_GREEN);
	}
}

void SECRETARY_warning_battery(CAN_msg_t *msg){
	if(msg->data.beacon_warning_low_battery.module == RF_FOE1){
		warner_battery_foe_1 = TRUE;
	}else if(msg->data.beacon_warning_low_battery.module == RF_FOE2){
		warner_battery_foe_2 = TRUE;
	}
}
