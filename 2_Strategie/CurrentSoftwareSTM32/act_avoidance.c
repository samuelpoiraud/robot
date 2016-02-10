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

	if(QS_WHO_AM_I_get() == BIG_ROBOT){ // Seulement sur le gros robot
		//----------------------------------------------------------------------------------------------------------left-----right----front---back
//EXEMPLE:init_new_offset(ACT_AVOID_POP_COLLECT_LEFT,ACT_AVOID_POP_COLLECT_LEFT_Open,ACT_POP_COLLECT_LEFT_OPEN,			60,		0,		0,		60);
		init_new_offset(ACT_AVOID_FISH_UNSTICK_ARM,ACT_AVOID_FISH_UNSTICK_ARM_Open,ACT_FISH_UNSTICK_ARM_OPEN,			0,		210,	0,		0);
		init_new_offset(ACT_AVOID_FISH_UNSTICK_ARM,ACT_AVOID_FISH_UNSTICK_ARM_Mid,ACT_FISH_UNSTICK_ARM_MIDDLE,			0,		150,	0,		0);
		init_new_offset(ACT_AVOID_FISH_UNSTICK_ARM,ACT_AVOID_FISH_UNSTICK_ARM_Close,ACT_FISH_UNSTICK_ARM_CLOSE_FISH,	0,		100,	0,		0);

		init_new_offset(ACT_AVOID_BLACK_SAND_CIRCLE,ACT_AVOID_BLACK_SAND_CIRCLE_Open,ACT_BLACK_SAND_CIRCLE_OPEN,		0,		0,		0,		0);

		init_new_offset(ACT_AVOID_BOTTOM_DUNE_LEFT,ACT_AVOID_BOTTOM_DUNE_LEFT_Open,ACT_BOTTOM_DUNE_LEFT_OPEN,			0,		0,		0,		0);

		init_new_offset(ACT_AVOID_MIDDLE_DUNE_LEFT,ACT_AVOID_MIDDLE_DUNE_LEFT_Open,ACT_MIDDLE_DUNE_LEFT_OPEN,			0,		0,		0,		0);

		init_new_offset(ACT_AVOID_CONE_DUNE,ACT_AVOID_CONE_DUNE_Open,ACT_CONE_DUNE_OPEN,								0,		0,		0,		0);

		init_new_offset(ACT_AVOID_DUNIX_LEFT,ACT_AVOID_DUNIX_LEFT_Open,ACT_DUNIX_LEFT_OPEN,								0,		0,		0,		0);

		init_new_offset(ACT_AVOID_DUNIX_RIGHT,ACT_AVOID_DUNIX_RIGHT_Open,ACT_DUNIX_RIGHT_OPEN,							0,		0,		0,		0);

		init_new_offset(ACT_AVOID_SAND_LOCKER_LEFT,ACT_AVOID_SAND_LOCKER_LEFT_Open,ACT_SAND_LOCKER_LEFT_OPEN,			0,		0,		0,		0);

		init_new_offset(ACT_AVOID_SAND_LOCKER_RIGHT,ACT_AVOID_SAND_LOCKER_RIGHT_Open,ACT_SAND_LOCKER_RIGHT_OPEN,		0,		0,		0,		0);






	}else{ //Seulement sur le petit robot

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
	debug_printf("New total avoid offset : L:%d R:%d F:%d B:%d\n", total_offset_avoid.Xleft, total_offset_avoid.Xright, total_offset_avoid.Yfront, total_offset_avoid.Yback);
	send_total_offset_avoid();
}

static void send_total_offset_avoid(){
	CAN_msg_t msg;
	msg.sid = PROP_OFFSET_AVOID;
	msg.size = SIZE_PROP_OFFSET_AVOID;
	msg.data.prop_offset_avoid.x_left = total_offset_avoid.Xleft;
	msg.data.prop_offset_avoid.x_right = total_offset_avoid.Xright;
	msg.data.prop_offset_avoid.y_front = total_offset_avoid.Yfront;
	msg.data.prop_offset_avoid.y_back = total_offset_avoid.Yback;
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
	refresh_total_offset_avoid();
	debug_printf("Act : %d reset avoidance\n", act_avoid_id);
}
