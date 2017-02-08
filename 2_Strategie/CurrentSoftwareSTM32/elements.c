#include "elements.h"
#include "environment.h"
#include "QS/QS_outputlog.h"

#define LOG_PREFIX "element: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ELEMENTS
#include "QS/QS_outputlog.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can_over_xbee.h"
#include "QS/QS_watchdog.h"
#include "utils/actionChecker.h"
#include "QS/QS_stateMachineHelper.h"
#include "utils/generic_functions.h"


#define MAX_MODULE_DROP			6

typedef struct{
	Uint8 nbDrop;
	moduleType_e dropTable[MAX_MODULE_DROP];
}moduleDropInfo_s;

typedef struct{
	bool_e sending;
	bool_e receiving;
	time32_t lastUpdate;
	bool_e flag;
}hardflag_s;

#define HARDFLAGS_NB  (F_ELEMENTS_HRDFLAGS_END - F_ELEMENTS_HARDFLAGS_START - 1)
#define HARDFLAGS_PERIOD  	(1000)
#define HARDFLAGS_TIMEOUT  	(HARDFLAGS_PERIOD + 200)

static volatile bool_e elements_flags[F_ELEMENTS_FLAGS_NB];
static volatile moduleDropInfo_s moduleDropInfo[NB_MODULE_LOCATION] = {0};
static volatile hardflag_s elements_hardflags[HARDFLAGS_NB];


const module_zone_characteristics_s module_zone[NB_MODULE_LOCATION] = {
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6},		// MODULE_DROP_MIDDLE
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6},		// MODULE_DROP_NORTH_CENTER
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=4},		// MODULE_DROP_SOUTH_CENTER
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6},		// MODULE_DROP_NORTH
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=4}		// MODULE_DROP_SOUTH
};

const module_zone_characteristics_s central = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6};

const module_zone_characteristics_s our_diagonal = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6};

const module_zone_characteristics_s our_side = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=4};

const module_zone_characteristics_s adv_diagonal = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6};

const module_zone_characteristics_s adv_side = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=4};

static void ELEMENTS_receive_hardflags();
static void ELEMENTS_send_hardflags();
static void ELEMENTS_send_hardflags_to_xbee();

void ELEMENTS_init(){
	Uint8 i;

	for(i=0;i<F_ELEMENTS_FLAGS_NB;i++)
	{
		elements_flags[i] = FALSE;
	}

	for(i=0;i<F_ELEMENTS_FLAGS_NB;i++)
	{
		elements_hardflags[i].sending = FALSE;
		elements_hardflags[i].receiving = FALSE;
		elements_hardflags[i].lastUpdate = global.absolute_time;
	}
}

void ELEMENTS_process_main(){
	#ifdef USE_HARDFLAGS
		ELEMENTS_send_hardflags();
		ELEMENTS_receive_hardflags();
	#endif
}

bool_e ELEMENTS_get_flag(elements_flags_e flag_id)
{
	assert(flag_id < F_ELEMENTS_FLAGS_NB);
	return elements_flags[flag_id];
}

void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state)
{
	assert(flag_id < F_ELEMENTS_FLAGS_NB);
	elements_flags[flag_id] = new_state;

#ifdef USE_SYNC_ELEMENTS
	if(flag_id < F_ELEMENTS_FLAGS_END_SYNCH)
	{
		CAN_msg_t msg;
		msg.sid = XBEE_SYNC_ELEMENTS_FLAGS;
		msg.size = SIZE_XBEE_SYNC_ELEMENTS_FLAGS;
		msg.data.xbee_sync_elements_flags.flagId = flag_id;
		msg.data.xbee_sync_elements_flags.flag = new_state;
		CANMsgToXbee(&msg,FALSE);
	}
#endif

#ifdef USE_HARDFLAGS
	if(flag_id > F_ELEMENTS_HARDFLAGS_START && flag_id < F_ELEMENTS_HRDFLAGS_END)
	{
		elements_hardflags[flag_id - F_ELEMENTS_HARDFLAGS_START - 1].sending = new_state;
	}
#endif
}


//################################## SYNCHRONISATION #################################
#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg)
{
	if(msg->data.xbee_sync_elements_flags.flagId < F_ELEMENTS_FLAGS_END_SYNCH){
		//debug_printf("\n\n\n\nReception message Xbee\n\n\n\n");
		elements_flags[msg->data.xbee_sync_elements_flags.flagId] = msg->data.xbee_sync_elements_flags.flag;
	}
}
#endif

#define	TIMEOUT_ANSWER	200

error_e ELEMENTS_check_communication(CAN_msg_t * msg)
{
#ifdef USE_SYNC_ELEMENTS
	CREATE_MAE(INIT,
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
			CANMsgToXbee(&request,FALSE);
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
					ELEMENTS_set_flag(F_COMMUNICATION_AVAILABLE, TRUE);
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
			ELEMENTS_set_flag(F_COMMUNICATION_AVAILABLE, FALSE);
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
	#endif
	return IN_PROGRESS;

}

//################################## HARDFLAGS #################################
static void ELEMENTS_send_hardflags_to_xbee(){
	CAN_msg_t request;
	request.sid = XBEE_ELEMENTS_HARDFLAGS;
	Uint8 i, j;

	// Send flags by XBee
	i = 0;
	while(i < HARDFLAGS_NB){
		j = 0;
		while(j < 8){
			if(elements_hardflags[i].sending) {
				request.data.xbee_elements_hardflags.flagId[j] = i;
				j++;
			}
			i++;
		}
		if(j > 0){
			request.size = j;
			CANMsgToXbee(&request,FALSE);
		}
	}
}

static void ELEMENTS_send_hardflags()
{
	CREATE_MAE(SEND_REQUEST,
				WAIT_FOR_NEXT_REQUEST
				);
	static time32_t timeLastSending = 0;

	switch(state)
	{
		case SEND_REQUEST:
			ELEMENTS_send_hardflags_to_xbee();
			timeLastSending = global.absolute_time;
			state = WAIT_FOR_NEXT_REQUEST;
			break;
		case WAIT_FOR_NEXT_REQUEST:
			if(global.absolute_time > timeLastSending + HARDFLAGS_PERIOD){
				state = SEND_REQUEST;
			}
			break;
		default:
			RESET_MAE();
			break;
	}
}

void ELEMENTS_receive_hardflags_from_xbee(CAN_msg_t * msg)
{
	Uint8 i;
	Uint8 index = 0;
	if(msg->sid == XBEE_ELEMENTS_HARDFLAGS) {
		for(i = 0; i < msg->size; i++){
			index = msg->data.xbee_elements_hardflags.flagId[i];
			elements_hardflags[index].receiving = TRUE;
			elements_hardflags[i].lastUpdate = global.absolute_time;
			if(elements_hardflags[index].sending){
				error_printf("ERROR Hardflag : This hardflag %d could not be used by both robots\n", index);
			}
		}
	}
}

static void ELEMENTS_receive_hardflags()
{
	Uint8 i;
	for(i = 0; i < HARDFLAGS_NB; i++){
		if(elements_hardflags[i].receiving){
			if(!elements_hardflags[i].flag){
				elements_hardflags[i].flag = TRUE;
				ELEMENTS_set_flag(F_ELEMENTS_HARDFLAGS_START + 1 + i, TRUE);
			}
			elements_hardflags[i].receiving = FALSE;
		}else if(elements_hardflags[i].lastUpdate > HARDFLAGS_TIMEOUT){
			if(elements_hardflags[i].flag){
				elements_hardflags[i].flag = FALSE;
				ELEMENTS_set_flag(F_ELEMENTS_HARDFLAGS_START + 1 + i, FALSE);
			}
		}
	}
}




bool_e modulePlaceIsEmpty(Uint8 place, moduleDropLocation_e location){
	assert(place < MAX_MODULE_DROP);
	assert(location < NB_MODULE_LOCATION);
	return (moduleDropInfo[location].dropTable[place] == MODULE_EMPTY);
}

Uint8 getNbDrop(moduleDropLocation_e location){
	return moduleDropInfo[location].nbDrop;
}

moduleType_e getModuleType(Uint8 place, moduleDropLocation_e location){
	assert(place < MAX_MODULE_DROP);
	assert(location < NB_MODULE_LOCATION);
	return moduleDropInfo[location].dropTable[place];
}

void addModule(moduleType_e type, moduleDropLocation_e location){
	Uint8 i;
	for(i=0; i<MAX_MODULE_DROP-1; i++){
		moduleDropInfo[location].dropTable[i+1] = moduleDropInfo[location].dropTable[i];
	}
	moduleDropInfo[location].dropTable[0] = type;
}
