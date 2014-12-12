#include "act_avoidance.h"
#include "avoidance.h"

#define LOG_PREFIX "act_avoid: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_can.h"

static offset_avoid_s offset_avoid[ACT_AVOID_NB][ACT_AVOID_NB_MAX_CMD] = {{{0}}};
static offset_avoid_s total_offset_avoid = {0};

static void init_new_offset(act_avoid_e act_avoid_id, Uint8 cmd, Uint8 act_cmd, Uint16 Xleft, Uint16 Xright, Uint16 Yfront, Uint16 Yback);
static void send_total_offset_avoid();
static void refresh_total_offset_avoid();

void ACT_AVOIDANCE_init(){
	Uint8 i, j;

	// Initialisation de tout les offsets à FALSE pour être sûr de ne pas imposer un offset non controlé
	for(i=0;i<ACT_AVOID_NB;i++){
		for(j=0;j<ACT_AVOID_NB_MAX_CMD;j++)
			offset_avoid[i][j].init = FALSE;
	}


	// Listing de tout les offsets d'évitement de tout les actionneurs

	//ACT_AVOID_TORCH_LOCKER
	if(QS_WHO_AM_I_get() == BIG_ROBOT){ // Seulement sur le gros robot
		//init_new_offset(ACT_AVOID_TORCH_LOCKER, ACT_AVOID_TORCH_Locker_Lock, ACT_TORCH_Locker_Lock,		0, 0, 60, 0);
		//init_new_offset(ACT_AVOID_TORCH_LOCKER, ACT_AVOID_TORCH_Locker_Unlock, ACT_TORCH_Locker_Unlock, 0, 0, 50, 0);
		init_new_offset(ACT_AVOID_POP_COLLECT_LEFT, ACT_AVOID_POP_COLLECT_LEFT_Open, ACT_pop_collect_left_open, 60, 0, 0, 60);
		init_new_offset(ACT_AVOID_POP_COLLECT_LEFT, ACT_AVOID_POP_COLLECT_LEFT_Mid, ACT_pop_collect_left_mid, 30, 0, 0, 30);
		init_new_offset(ACT_AVOID_POP_COLLECT_LEFT, ACT_AVOID_POP_COLLECT_RIGHT_Open, ACT_pop_collect_right_open, 0, 60, 0, 60);
		init_new_offset(ACT_AVOID_POP_COLLECT_LEFT, ACT_AVOID_POP_COLLECT_RIGHT_Mid, ACT_pop_collect_right_mid, 0, 30, 0, 30);
		init_new_offset(ACT_AVOID_POP_DROP_LEFT, ACT_AVOID_POP_DROP_LEFT_Open, ACT_pop_drop_left_open, 0, 0, 0, 60);
		init_new_offset(ACT_AVOID_POP_DROP_LEFT, ACT_AVOID_POP_DROP_RIGHT_Open, ACT_pop_drop_right_open, 0, 0, 0, 60);
		init_new_offset(ACT_AVOID_BACK_SPOT_RIGHT, ACT_AVOID_BACK_SPOT_RIGHT_Open, ACT_back_spot_right_open, 0, 40, 0, 0);
		init_new_offset(ACT_AVOID_BACK_SPOT_LEFT, ACT_AVOID_BACK_SPOT_LEFT_Open, ACT_back_spot_left_open, 40, 0, 0, 0);
	}
}


static void init_new_offset(act_avoid_e act_avoid_id, Uint8 cmd, Uint8 act_cmd, Uint16 Xleft, Uint16 Xright, Uint16 Yfront, Uint16 Yback){
	if(cmd >= ACT_AVOID_NB_MAX_CMD){
		debug_printf("Error : tentative d'initialisation d'évitement actionneur -> cmd %d inconnue\n", cmd);
		return;
	}else if(act_avoid_id >= ACT_AVOID_NB){
		debug_printf("Error : tentative d'initialisation d'évitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
		return;
	}

	offset_avoid[act_avoid_id][cmd].Xleft = Xleft;
	offset_avoid[act_avoid_id][cmd].Xright = Xright;
	offset_avoid[act_avoid_id][cmd].Yfront = Yfront;
	offset_avoid[act_avoid_id][cmd].Yback = Yback;
	offset_avoid[act_avoid_id][cmd].init = TRUE;
	offset_avoid[act_avoid_id][cmd].active = FALSE;
	offset_avoid[act_avoid_id][cmd].act_cmd = act_cmd;
}

void ACT_AVOIDANCE_new_action(act_avoid_e act_avoid_id, Uint8 cmd, bool_e state){
	if(cmd >= ACT_AVOID_NB_MAX_CMD){
		debug_printf("Error : tentative d'activation d'évitement actionneur -> cmd %d inconnue\n", cmd);
		return;
	}else if(act_avoid_id >= ACT_AVOID_NB){
		debug_printf("Error : tentative d'activation d'évitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
		return;
	}

	offset_avoid[act_avoid_id][cmd].active = state;
	refresh_total_offset_avoid();
}

void ACT_AVOIDANCE_reset_actionneur(act_avoid_e act_avoid_id){
	if(act_avoid_id >= ACT_AVOID_NB){
		debug_printf("Error : tentative de reset d'évitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
		return;
	}
	Uint8 i;
	for(i=0;i<ACT_AVOID_NB_MAX_CMD;i++)
		offset_avoid[act_avoid_id][i].active = FALSE;
}

static void refresh_total_offset_avoid(){
	Uint8 i, j;

	total_offset_avoid.Xleft = 0;
	total_offset_avoid.Xright = 0;
	total_offset_avoid.Yfront = 0;
	total_offset_avoid.Yback = 0;

	for(i=0;i<ACT_AVOID_NB;i++){
		for(j=0;j<ACT_AVOID_NB_MAX_CMD;j++){
			if(offset_avoid[i][j].active){

				if(offset_avoid[i][j].Xleft > total_offset_avoid.Xleft)
					total_offset_avoid.Xleft = offset_avoid[i][j].Xleft;

				if(offset_avoid[i][j].Xright > total_offset_avoid.Xright)
					total_offset_avoid.Xright = offset_avoid[i][j].Xright;

				if(offset_avoid[i][j].Yfront > total_offset_avoid.Yfront)
					total_offset_avoid.Yfront = offset_avoid[i][j].Yfront;

				if(offset_avoid[i][j].Yback > total_offset_avoid.Yback)
					total_offset_avoid.Yback = offset_avoid[i][j].Yback;
			}
		}
	}

	send_total_offset_avoid();
}

static void send_total_offset_avoid(){
	CAN_msg_t msg;
	msg.sid = PROP_OFFSET_AVOID;
	msg.size = 8;
	msg.data[0] = HIGHINT(total_offset_avoid.Xleft);
	msg.data[1] = LOWINT(total_offset_avoid.Xleft);
	msg.data[2] = HIGHINT(total_offset_avoid.Xright);
	msg.data[3] = LOWINT(total_offset_avoid.Xright);
	msg.data[4] = HIGHINT(total_offset_avoid.Yfront);
	msg.data[5] = LOWINT(total_offset_avoid.Yfront);
	msg.data[6] = HIGHINT(total_offset_avoid.Yback);
	msg.data[7] = LOWINT(total_offset_avoid.Yback);
	CAN_send(&msg);
}

void ACT_AVOIDANCE_new_classic_cmd(act_avoid_e act_avoid_id, Uint8 act_cmd){
	Uint8 i;
	if(act_avoid_id >= ACT_AVOID_NB){
		debug_printf("Error : tentative d'activation d'évitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
		return;
	}

	ACT_AVOIDANCE_reset_actionneur(act_avoid_id);

	for(i=0;i<ACT_AVOID_NB_MAX_CMD;i++){
		if(offset_avoid[act_avoid_id][i].init == TRUE
				&& offset_avoid[act_avoid_id][i].act_cmd == act_cmd){
			ACT_AVOIDANCE_new_action(act_avoid_id, i, TRUE);
			return;
		}
	}


}
