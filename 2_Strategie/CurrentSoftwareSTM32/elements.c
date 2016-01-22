#include "elements.h"
#include "environment.h"

#define LOG_PREFIX "element: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ELEMENTS
#include "QS/QS_outputlog.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can_over_xbee.h"

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
#endif

}

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg)
{
	if(msg->data.xbee_sync_elements_flags.flagId < ELEMENTS_FLAGS_NB)
		elements_flags[msg->data.xbee_sync_elements_flags.flagId] = msg->data.xbee_sync_elements_flags.flag;
}
#endif

void ELEMENTS_inc_fishs_passage(){
	fishs_passage++;
}

bool_e ELEMENTS_fishs_passage_completed(){
	return (fishs_passage >= FISHS_PASSAGES);
}
