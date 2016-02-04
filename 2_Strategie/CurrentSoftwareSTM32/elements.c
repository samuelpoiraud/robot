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

#define FISHS_PASSAGES 2
static Uint8 fishs_passage = 0;

void ELEMENTS_init(){
	Uint8 i;

	for(i=0;i<ELEMENTS_FLAGS_NB;i++)
	{
		elements_flags[i] = FALSE;
	}
	fishs_passage = 0;
}

void COQUILLAGE_init(){
	ELEMENTS_set_flag(COQUILLAGE_HOME_FOUR, TRUE);
	ELEMENTS_set_flag(COQUILLAGE_HOME_FIVE, TRUE);

	ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_THREE, TRUE);

	if(SWITCH_COQUILLAGE_1){
		ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_THREE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_FOUR, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_SIX, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_TWO, TRUE);
	}

	if(SWITCH_COQUILLAGE_2){
		ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_THREE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_FOUR, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_FIVE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_SIX, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_ROCK_TWO, TRUE);
	}

	if(SWITCH_COQUILLAGE_3){
		ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_FIVE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_FOUR, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_ROCK_TWO, TRUE);
	}

	if(SWITCH_COQUILLAGE_4){
		ELEMENTS_set_flag(COQUILLAGE_HOME_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_NEUTRAL_FIVE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_FOUR, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_ROCK_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_ROCK_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_ROCK_THREE, TRUE);
	}

	if(SWITCH_COQUILLAGE_5){
		ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_ROCK_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_ONE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_HOME_THREE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_FOUR, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_FIVE, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_ROCK_TWO, TRUE);
		ELEMENTS_set_flag(COQUILLAGE_AWAY_ROCK_THREE, TRUE);
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
	if(msg->data.xbee_sync_elements_flags.flagId < ELEMENTS_FLAGS_END_SYNCH)
		elements_flags[msg->data.xbee_sync_elements_flags.flagId] = msg->data.xbee_sync_elements_flags.flag;
}
#endif

void ELEMENTS_inc_fishs_passage(){
	fishs_passage++;
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
