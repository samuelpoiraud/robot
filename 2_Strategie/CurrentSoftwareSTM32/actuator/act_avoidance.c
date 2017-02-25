#include "act_avoidance.h"


#define LOG_PREFIX "act_avoid: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "../QS/QS_outputlog.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_can.h"

static offset_avoid_s offset_avoid[NB_QUEUE][ACT_AVOID_NB_MAX_CMD] = {{{0}}};
static offset_avoid_s total_offset_avoid = {0};

static void init_new_offset(queue_id_e act_avoid_id, Uint8 cmd, Uint8 act_cmd, Uint16 Xleft, Uint16 Xright, Uint16 Yfront, Uint16 Yback);
static void send_total_offset_avoid();
static void refresh_total_offset_avoid();

void ACT_AVOIDANCE_init(){
	Uint8 i, j;

	UNUSED_VAR(init_new_offset(ACT_QUEUE_Ore_gun, 0, 0, 0, 0, 0, 0));

	// Initialisation de tout les offsets à FALSE pour être sûr de ne pas imposer un offset non controlé
	for(i=0;i<NB_QUEUE;i++){
		for(j=0;j<ACT_AVOID_NB_MAX_CMD;j++)
			offset_avoid[i][j].init = FALSE;
	}


	// Listing de tout les offsets d'évitement de tout les actionneurs

	if(QS_WHO_AM_I_get() == BIG_ROBOT){ // Seulement sur le gros robot
		//---------------------------------------------------------------------------------------------------------------------left-----right----front---back

//EXEMPLE init_new_offset(ACT_QUEUE_Exemple,				ACT_AVOID_EXEMPLE_Open,			ACT_EXEMPLE_OPEN,					0,		190,	0,		0);
//EXEMPLE init_new_offset(ACT_QUEUE_Exemple_arm,			ACT_AVOID_EXEMPLE_Close,		ACT_EXEMPLE_CLOSE					0,		190,	0,		0);

		//TODO 2017 Compléter évitement actionneurs

	}else{ //Seulement sur le petit robot

		//TODO 2017 Compléter évitement actionneurs

	}
}


static void init_new_offset(queue_id_e act_avoid_id, Uint8 cmd, Uint8 act_cmd, Uint16 Xleft, Uint16 Xright, Uint16 Yfront, Uint16 Yback){
	if(cmd >= ACT_AVOID_NB_MAX_CMD){
		debug_printf("Error : tentative d'initialisation d'évitement actionneur -> cmd %d inconnue\n", cmd);
		return;
	}else if(act_avoid_id >= NB_QUEUE){
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

void ACT_AVOIDANCE_new_action(queue_id_e act_avoid_id, Uint8 cmd, bool_e state){
	if(cmd >= ACT_AVOID_NB_MAX_CMD){
		debug_printf("Error : tentative d'activation d'évitement actionneur -> cmd %d inconnue\n", cmd);
		return;
	}else if(act_avoid_id >= NB_QUEUE){
		debug_printf("Error : tentative d'activation d'évitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
		return;
	}

	offset_avoid[act_avoid_id][cmd].active = state;
	refresh_total_offset_avoid();
}

void ACT_AVOIDANCE_reset_actionneur(queue_id_e act_avoid_id){
	if(act_avoid_id >= NB_QUEUE){
		debug_printf("Error : tentative de reset d'évitement actionneur -> act_avoid_id %d inconnue\n", act_avoid_id);
		return;
	}
	Uint8 i;
	for(i=0;i<ACT_AVOID_NB_MAX_CMD;i++)
		offset_avoid[act_avoid_id][i].active = FALSE;
}

static void refresh_total_offset_avoid(){
	Uint8 i, j;

	offset_avoid_s past_offset;

	past_offset.Xleft = total_offset_avoid.Xleft;
	past_offset.Xright = total_offset_avoid.Xright;
	past_offset.Yback = total_offset_avoid.Yback;
	past_offset.Yfront = total_offset_avoid.Yfront;

	total_offset_avoid.Xleft = 0;
	total_offset_avoid.Xright = 0;
	total_offset_avoid.Yfront = 0;
	total_offset_avoid.Yback = 0;

	for(i=0;i<NB_QUEUE;i++){
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

	if(total_offset_avoid.Xleft != past_offset.Xleft
			|| total_offset_avoid.Xright != past_offset.Xright
			|| total_offset_avoid.Yback != past_offset.Yback
			|| total_offset_avoid.Yfront != past_offset.Yfront){
		debug_printf("New total avoid offset : L:%d R:%d F:%d B:%d\n", total_offset_avoid.Xleft, total_offset_avoid.Xright, total_offset_avoid.Yfront, total_offset_avoid.Yback);
		send_total_offset_avoid();
	}
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

void ACT_AVOIDANCE_new_classic_cmd(queue_id_e act_avoid_id, Uint8 act_cmd){
	Uint8 i;
	if(act_avoid_id >= NB_QUEUE){
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
