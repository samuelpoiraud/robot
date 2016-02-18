#include "elements.h"
#include "environment.h"

#define LOG_PREFIX "element: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ELEMENTS
#include "QS/QS_outputlog.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can_over_xbee.h"
#include "../QS/QS_watchdog.h"
#include "state_machine_helper.h"
#include "Generic_functions.h"

volatile bool_e elements_flags[ELEMENTS_FLAGS_NB];
volatile COQUILLAGES_t coquillages[COQUILLAGE_NB];
volatile COQUILLAGES_config_e coquillages_config = NO_COQUILLAGES_CONFIG;

#define FISHS_PASSAGES 2
static Uint8 fishs_passage = 0;
static Uint8 seashell_depose = 0;

void ELEMENTS_init(){
	Uint8 i;

	for(i=0;i<ELEMENTS_FLAGS_NB;i++)
	{
		elements_flags[i] = FALSE;
	}
	fishs_passage = 0;

}

void COQUILLAGES_init(){

	coquillages[COQUILLAGE_AWAY_ONE].pos.x = 1250;
	coquillages[COQUILLAGE_AWAY_ONE].pos.y = 2800;

	coquillages[COQUILLAGE_AWAY_TWO].pos.x = 1550;
	coquillages[COQUILLAGE_AWAY_TWO].pos.y = 2800;

	coquillages[COQUILLAGE_AWAY_THREE].pos.x = 1250;
	coquillages[COQUILLAGE_AWAY_THREE].pos.y = 2400;

	coquillages[COQUILLAGE_AWAY_FOUR].pos.x = 1550;
	coquillages[COQUILLAGE_AWAY_FOUR].pos.y = 2400;

	coquillages[COQUILLAGE_AWAY_FIVE].pos.x = 1850;
	coquillages[COQUILLAGE_AWAY_FIVE].pos.y = 2400;

	coquillages[COQUILLAGE_AWAY_ROCK_ONE].pos.x = 1800;
	coquillages[COQUILLAGE_AWAY_ROCK_ONE].pos.y = 2950;

	coquillages[COQUILLAGE_AWAY_ROCK_TWO].pos.x = 1950;
	coquillages[COQUILLAGE_AWAY_ROCK_TWO].pos.y = 2950;

	coquillages[COQUILLAGE_AWAY_ROCK_THREE].pos.x = 1950;
	coquillages[COQUILLAGE_AWAY_ROCK_THREE].pos.y = 2800;

	coquillages[COQUILLAGE_NEUTRAL_ONE].pos.x = 1450;
	coquillages[COQUILLAGE_NEUTRAL_ONE].pos.y = 2100;

	coquillages[COQUILLAGE_NEUTRAL_TWO].pos.x = 1650;
	coquillages[COQUILLAGE_NEUTRAL_TWO].pos.y = 1800;

	coquillages[COQUILLAGE_NEUTRAL_THREE].pos.x = 1550;
	coquillages[COQUILLAGE_NEUTRAL_THREE].pos.y = 1500;

	coquillages[COQUILLAGE_NEUTRAL_FOUR].pos.x = 1850;
	coquillages[COQUILLAGE_NEUTRAL_FOUR].pos.y = 1500;

	coquillages[COQUILLAGE_NEUTRAL_FIVE].pos.x = 1650;
	coquillages[COQUILLAGE_NEUTRAL_FIVE].pos.y = 1200;

	coquillages[COQUILLAGE_NEUTRAL_SIX].pos.x = 1450;
	coquillages[COQUILLAGE_NEUTRAL_SIX].pos.y = 900;

	coquillages[COQUILLAGE_HOME_ONE].pos.x = 1250;
	coquillages[COQUILLAGE_HOME_ONE].pos.y = 600;

	coquillages[COQUILLAGE_HOME_TWO].pos.x = 1550;
	coquillages[COQUILLAGE_HOME_TWO].pos.y = 600;

	coquillages[COQUILLAGE_HOME_THREE].pos.x = 1850;
	coquillages[COQUILLAGE_HOME_THREE].pos.y = 600;

	coquillages[COQUILLAGE_HOME_FOUR].pos.x = 1250;
	coquillages[COQUILLAGE_HOME_FOUR].pos.y = 200;

	coquillages[COQUILLAGE_HOME_FIVE].pos.x = 1550;
	coquillages[COQUILLAGE_HOME_FIVE].pos.y = 200;

	coquillages[COQUILLAGE_HOME_ROCK_ONE].pos.x = 1950;
	coquillages[COQUILLAGE_HOME_ROCK_ONE].pos.y = 200;

	coquillages[COQUILLAGE_HOME_ROCK_TWO].pos.x = 1800;
	coquillages[COQUILLAGE_HOME_ROCK_TWO].pos.y = 50;

	coquillages[COQUILLAGE_HOME_ROCK_THREE].pos.x = 1950;
	coquillages[COQUILLAGE_HOME_ROCK_THREE].pos.y = 50;


	//Gestion des différentes configurations

	if(COQUILLAGES_is_config(CONFIG_COQUILLAGES_1)){
		coquillages[COQUILLAGE_HOME_ROCK_ONE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_TWO].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_THREE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ONE].property = NO_ELEMENT;
		coquillages[COQUILLAGE_HOME_TWO].property = NO_ELEMENT;
		coquillages[COQUILLAGE_HOME_THREE].property = NO_ELEMENT;
		coquillages[COQUILLAGE_HOME_FOUR].property = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_FIVE].property = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_TWO].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_THREE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FOUR].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FIVE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_SIX].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ONE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_TWO].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_THREE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FOUR].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FIVE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_THREE].property  = ADV_ELEMENT;
	}
	else if(COQUILLAGES_is_config(CONFIG_COQUILLAGES_2)){
		coquillages[COQUILLAGE_HOME_ROCK_ONE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_TWO].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_THREE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_ONE].property = NO_ELEMENT;
		coquillages[COQUILLAGE_HOME_TWO].property = NO_ELEMENT;
		coquillages[COQUILLAGE_HOME_THREE].property = NO_ELEMENT;
		coquillages[COQUILLAGE_HOME_FOUR].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_FIVE].property = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_THREE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FOUR].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FIVE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_SIX].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_TWO].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_THREE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FOUR].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FIVE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_TWO].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_THREE].property  = ADV_ELEMENT;
	}
	else if(COQUILLAGES_is_config(CONFIG_COQUILLAGES_3)){
		coquillages[COQUILLAGE_HOME_ROCK_ONE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_TWO].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_THREE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_ONE].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_TWO].property = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_THREE].property = NO_ELEMENT;
		coquillages[COQUILLAGE_HOME_FOUR].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_FIVE].property = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_ONE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_THREE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FOUR].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FIVE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_SIX].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_TWO].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_THREE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FOUR].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FIVE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_TWO].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_THREE].property  = ADV_ELEMENT;
	}
	else if(COQUILLAGES_is_config(CONFIG_COQUILLAGES_4)){
		coquillages[COQUILLAGE_HOME_ROCK_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_THREE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_ONE].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_TWO].property = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_THREE].property = NO_ELEMENT;
		coquillages[COQUILLAGE_HOME_FOUR].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_FIVE].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_ONE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_TWO].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_THREE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FOUR].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FIVE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_SIX].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_THREE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FOUR].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FIVE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_ONE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_TWO].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_THREE].property  = OUR_ELEMENT;
	}
	else if(COQUILLAGES_is_config(CONFIG_COQUILLAGES_5)){
		coquillages[COQUILLAGE_HOME_ROCK_ONE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_TWO].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_THREE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_ONE].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_TWO].property = ADV_ELEMENT;
		coquillages[COQUILLAGE_HOME_THREE].property = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_HOME_FOUR].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_FIVE].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_ONE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_TWO].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_THREE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FOUR].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FIVE].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_SIX].property  = NO_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_THREE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FOUR].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FIVE].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_TWO].property  = NEUTRAL_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_THREE].property  = NEUTRAL_ELEMENT;
	}else{
		coquillages[COQUILLAGE_HOME_ROCK_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_HOME_ROCK_THREE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_ONE].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_TWO].property = ADV_ELEMENT;
		coquillages[COQUILLAGE_HOME_THREE].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_FOUR].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_HOME_FIVE].property = OUR_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_THREE].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FOUR].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_FIVE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_NEUTRAL_SIX].property  = OUR_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_THREE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FOUR].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_FIVE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_ONE].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_TWO].property  = ADV_ELEMENT;
		coquillages[COQUILLAGE_AWAY_ROCK_THREE].property  = ADV_ELEMENT;
	}
}

void ELEMENTS_process_main(){
	ELEMENTS_check_configuration_coquillages();
}

ELEMENTS_property_e COQUILLAGE_get_property(Uint8 id){
	return coquillages[id].property;
}

//Le coquillage est à nous et est présent au début du match
bool_e COQUILLAGE_is_mine(Uint8 id){
	if(coquillages[id].property == OUR_ELEMENT || coquillages[id].property == NEUTRAL_ELEMENT)
		return TRUE;
	else
		return FALSE;
}

//Le coquillage est à nous et est présent au moement de l'appel de la fonction
bool_e COQUILLAGE_is_present(Uint8 id){
	if(coquillages[id].property == OUR_ELEMENT || coquillages[id].property == NEUTRAL_ELEMENT)
		return TRUE;
	else
		return FALSE;
}

Uint8 COQUILLAGES_get_config(){
	return coquillages_config;
}

bool_e COQUILLAGES_is_config(COQUILLAGES_config_e user_config){
	return (bool_e)(coquillages_config == user_config);
}

#define TIMEOUT_CONFIG_COQUILLAGES      500

void ELEMENTS_check_configuration_coquillages()
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_ASK_CONFIG_COQUILLAGES,
				INIT,
				SEND_REQUEST,
				WAIT_FOR_ANSWER,
				TIMEOUT,
				END);
	static bool_e watchdog_flag = FALSE;
	static watchdog_id_t watchdog_id = 0;
	static Uint8 nb_ask = 0;

	switch(state)
	{
		case INIT:
			nb_ask = 0;
			Uint8 nb_switchs_enable = 0;
			if(IHM_switchs_get(SWITCH_COQUILLAGE_1)){
				nb_switchs_enable++;
				coquillages_config = CONFIG_COQUILLAGES_1;
			}
			if(IHM_switchs_get(SWITCH_COQUILLAGE_2)){
				nb_switchs_enable++;
				coquillages_config = CONFIG_COQUILLAGES_2;
			}
			if(IHM_switchs_get(SWITCH_COQUILLAGE_3)){
				nb_switchs_enable++;
				coquillages_config = CONFIG_COQUILLAGES_3;
			}
			if(IHM_switchs_get(SWITCH_COQUILLAGE_4)){
				nb_switchs_enable++;
				coquillages_config = CONFIG_COQUILLAGES_4;
			}
			if(IHM_switchs_get(SWITCH_COQUILLAGE_5)){
				nb_switchs_enable++;
				coquillages_config = CONFIG_COQUILLAGES_5;
			}
			if(nb_switchs_enable > 1)
				state = END;
			else
				state = SEND_REQUEST;
			break;
		case SEND_REQUEST:{
			CAN_msg_t request;
			request.sid = XBEE_ASK_CONFIG_COQUILLAGES;
			request.size = 0;
			CANMsgToXBeeDestination(&request,(QS_WHO_AM_I_get()==BIG_ROBOT)?SMALL_ROBOT_MODULE:BIG_ROBOT_MODULE);
			state = WAIT_FOR_ANSWER;
			break;}
		case WAIT_FOR_ANSWER:
			if(entrance)
			{
				watchdog_id = WATCHDOG_create_flag( TIMEOUT_CONFIG_COQUILLAGES,&watchdog_flag);
			}
			if(watchdog_flag)
			{
				state = TIMEOUT;
			}
			else if(coquillages_config != 0)
			{
				state = END;
				WATCHDOG_stop(watchdog_id);
			}
			break;

		case TIMEOUT:
			nb_ask++;
			if(nb_ask < 3)
				state = SEND_REQUEST;
			else
				state = END;
			break;
		case END:
			COQUILLAGES_init();
			break;
		default:
			RESET_MAE();
			break;
	}
}

void ELEMENTS_send_config_coquillages(CAN_msg_t *msg){
	if(msg->sid == XBEE_ASK_CONFIG_COQUILLAGES){
		CAN_msg_t msg_to_send;
		msg_to_send.sid = XBEE_SEND_CONFIG_COQUILLAGES;
		msg_to_send.size = SIZE_XBEE_SEND_CONFIG_COQUILLAGES;
		msg_to_send.data.xbee_send_config_coquillages.config = coquillages_config;
		CANMsgToXBeeDestination(&msg_to_send, (QS_WHO_AM_I_get()==BIG_ROBOT)?SMALL_ROBOT_MODULE:BIG_ROBOT_MODULE);
	}
}



bool_e ELEMENTS_get_flag(elements_flags_e flag_id)
{
	assert(flag_id < ELEMENTS_FLAGS_NB);
	return elements_flags[flag_id];
}

void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state)
{
	assert(flag_id < ELEMENTS_FLAGS_NB);
	elements_flags[flag_id] = new_state;

#ifdef USE_SYNC_ELEMENTS
	if(flag_id < ELEMENTS_FLAGS_END_SYNCH)
	{
		CAN_msg_t msg;
		msg.sid = XBEE_SYNC_ELEMENTS_FLAGS;
		msg.size = SIZE_XBEE_SYNC_ELEMENTS_FLAGS;
		msg.data.xbee_sync_elements_flags.flagId = flag_id;
		msg.data.xbee_sync_elements_flags.flag = new_state;
		if(QS_WHO_AM_I_get()==BIG_ROBOT){
			CANMsgToXBeeDestination(&msg,SMALL_ROBOT_MODULE);
		}else{
			CANMsgToXBeeDestination(&msg,BIG_ROBOT_MODULE);
		}
	}
#endif

}

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg)
{
	if(msg->data.xbee_sync_elements_flags.flagId < ELEMENTS_FLAGS_END_SYNCH){
		debug_printf("\n\n\n\nReception message Xbee\n\n\n\n");
		elements_flags[msg->data.xbee_sync_elements_flags.flagId] = msg->data.xbee_sync_elements_flags.flag;
	}
}
#endif

void ELEMENTS_inc_fishs_passage(){
	fishs_passage++;
}

void ELEMENTS_inc_seashell_depose(){
	seashell_depose++;
}
Uint8 ELEMENTS_get_seashell_depose(){
	return seashell_depose;
}

Uint8 ELEMENTS_get_fishs_passage(){
	return fishs_passage;
}

bool_e ELEMENTS_fishs_passage_completed(){
	return (fishs_passage >= FISHS_PASSAGES);
}


#define	TIMEOUT_ANSWER	200

error_e ELEMENTS_check_communication(CAN_msg_t * msg)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_CHECK_COMMUNICATION,
				INIT,
				SEND_REQUEST,
				WAIT_FOR_ANSWER,
				WAIT_TIMEOUT,
				TIMEOUT,
				ANSWER_RECEIVED,
				END);
	static bool_e watchdog_flag = FALSE;
	static watchdog_id_t watchdog_id = 0;

	switch(state)
	{
		case INIT:
			if(msg == NULL)	//On s'autorise à continuer seulement si ce n'est pas la réception d'un message qui nous active...
				state = SEND_REQUEST;
			break;
		case SEND_REQUEST:{
			CAN_msg_t request;
			request.sid = XBEE_COMMUNICATION_AVAILABLE;
			request.size = 0;
			CANMsgToXBeeDestination(&request,(QS_WHO_AM_I_get()==BIG_ROBOT)?SMALL_ROBOT_MODULE:BIG_ROBOT_MODULE);
			state = WAIT_FOR_ANSWER;
			break;}
		case WAIT_FOR_ANSWER:
			if(entrance)
			{
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_ANSWER,&watchdog_flag);
			}
			if(watchdog_flag)
			{
				state = WAIT_TIMEOUT;
			}
			else if(msg)
			{
				if(msg->sid == XBEE_COMMUNICATION_RESPONSE)
				{
					ELEMENTS_set_flag(COMMUNICATION_AVAILABLE, TRUE);
					state = ANSWER_RECEIVED;
					WATCHDOG_stop(watchdog_id);
				}
			}
			break;
		case ANSWER_RECEIVED:
			state = END;
			break;

		case WAIT_TIMEOUT:
			state = wait_time(1000, WAIT_TIMEOUT,TIMEOUT);
			break;
		case TIMEOUT:
			RESET_MAE();
			ELEMENTS_set_flag(COMMUNICATION_AVAILABLE, FALSE);
			return END_WITH_TIMEOUT;
			break;
		case END:
			RESET_MAE();
			return END_OK;
			break;
		default:
			RESET_MAE();
			break;
	}
	return IN_PROGRESS;
}

