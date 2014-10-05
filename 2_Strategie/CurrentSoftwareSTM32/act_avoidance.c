#include "act_avoidance.h"
#include "avoidance.h"

#include "config_debug.h"
#define LOG_PREFIX "act_avoid: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_can.h"

static offset_avoid_s offset_avoid[ACT_AVOID_NB][ACT_AVOID_NB_MAX_CMD] = {{{0}}};
static offset_avoid_s total_offset_avoid = {0};

static void init_new_offset(act_avoid_e act_avoid_id, Uint8 cmd, Uint16 Xleft, Uint16 Xright, Uint16 Yfront, Uint16 Yback);
static void send_total_offset_avoid();
static void refresh_total_offset_avoid();

void ACT_AVOIDANCE_init(){
	Uint8 i, j;

	// Initialisation de tout les offsets � FALSE pour �tre s�r de ne pas imposer un offset non control�
	for(i=0;i<ACT_AVOID_NB;i++){
		for(j=0;j<ACT_AVOID_NB_MAX_CMD;j++){
			offset_avoid[i][j].init = FALSE;
			offset_avoid[i][j].active = FALSE;
		}
	}


	// Listing de tout les offsets d'�vitement de tout les actionneurs

	//ACT_AVOID_TORCH_LOCKER
	if(QS_WHO_AM_I_get() == BIG_ROBOT){ // Seulement sur le gros robot
		init_new_offset(ACT_AVOID_TORCH_LOCKER, ACT_AVOID_TORCH_Locker_Lock, 0, 0, 60, 0);
		init_new_offset(ACT_AVOID_TORCH_LOCKER, ACT_AVOID_TORCH_Locker_Unlock, 0, 0, 50, 0);
		init_new_offset(ACT_AVOID_TORCH_LOCKER, ACT_AVOID_TORCH_Locker_Inside, 0, 0, 0, 0);
	}

	//ACT_AVOID_FRUIT_MOUTH
	if(QS_WHO_AM_I_get() == BIG_ROBOT){ // Seulement sur le gros robot
		init_new_offset(ACT_AVOID_FRUIT_MOUTH, ACT_AVOID_FRUIT_MOUTH_Open, 0, 250, 0, 0);
		init_new_offset(ACT_AVOID_FRUIT_MOUTH, ACT_AVOID_FRUIT_MOUTH_Close, 0, 0, 0, 0);
	}

	//ACT_AVOID_SMALL_ARM
	if(QS_WHO_AM_I_get() == SMALL_ROBOT){ // Seulement sur le petit robot
		init_new_offset(ACT_AVOID_SMALL_ARM, ACT_AVOID_SMALL_ARM_Idle, 0, 0, 0, 0);
		init_new_offset(ACT_AVOID_SMALL_ARM, ACT_AVOID_SMALL_ARM_Mid, 20, 0, 0, 0);
		init_new_offset(ACT_AVOID_SMALL_ARM, ACT_AVOID_SMALL_ARM_Deployed, 35, 0, 0, 0);
	}


}


static void init_new_offset(act_avoid_e act_avoid_id, Uint8 cmd, Uint16 Xleft, Uint16 Xright, Uint16 Yfront, Uint16 Yback){
	if(cmd >= ACT_AVOID_NB_MAX_CMD){
		debug_printf("Error : tentative d'initialisation d'�vitement actionneur -> cmd %d inconnue\n", cmd);
		return;
	}else if(act_avoid_id >= ACT_AVOID_NB){
		debug_printf("Error : tentative d'initialisation d'�vitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
		return;
	}


	offset_avoid[act_avoid_id][cmd].Xleft = Xleft;
	offset_avoid[act_avoid_id][cmd].Xright = Xright;
	offset_avoid[act_avoid_id][cmd].Yfront = Yfront;
	offset_avoid[act_avoid_id][cmd].Yback = Yback;
	offset_avoid[act_avoid_id][cmd].init = TRUE;
}

void ACT_AVOIDANCE_new_action(act_avoid_e act_avoid_id, Uint8 cmd, bool_e state){
	if(cmd >= ACT_AVOID_NB_MAX_CMD){
		debug_printf("Error : tentative d'activation d'�vitement actionneur -> cmd %d inconnue\n", cmd);
		return;
	}else if(act_avoid_id >= ACT_AVOID_NB){
		debug_printf("Error : tentative d'activation d'�vitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
		return;
	}

	offset_avoid[act_avoid_id][cmd].active = state;
	refresh_total_offset_avoid();
}

void ACT_AVOIDANCE_reset_actionneur(act_avoid_e act_avoid_id){
	if(act_avoid_id >= ACT_AVOID_NB){
		debug_printf("Error : tentative de reset d'�vitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
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
