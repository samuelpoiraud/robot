#include "../QS/QS_outputlog.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_can_over_xbee.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_CANmsgList.h"
#include "com_xbee.h"

#define WAIT_TIME_POSITION         800

static ELEMENTS_info_wood_protection infoWood = {FALSE,0,0,FALSE};
static watchdog_id_t watchdog_position_id = 255;
static bool_e create_watchdog_position = FALSE;

void holly_acquitte_flag_position();


void holly_receive_wood_position(CAN_msg_t *msg){
	if(msg->data[0]){
		infoWood.onTheDefensive = TRUE;
		if(!create_watchdog_position){
			watchdog_position_id = WATCHDOG_create(WAIT_TIME_POSITION, &holly_acquitte_flag_position,TRUE);
			create_watchdog_position=TRUE;
		}
	}
	else{
		infoWood.onTheDefensive = FALSE;
	}
	infoWood.x=U16FROMU8(msg->data[1],msg->data[2]);
	infoWood.y=U16FROMU8(msg->data[3],msg->data[4]);
	debug_printf("Position de wood = (%d;%d)\n",infoWood.x,infoWood.y);
}


void holly_acquitte_flag_position(){
	if(infoWood.dataSend)
		infoWood.dataSend = FALSE;
	else
		WATCHDOG_stop(watchdog_position_id);
}

Uint16 get_wood_position_x(){
	return infoWood.x;
}

Uint16 get_wood_position_y(){
	return infoWood.y;
}

bool_e get_wood_state_defensive(){
	return infoWood.onTheDefensive;
}

