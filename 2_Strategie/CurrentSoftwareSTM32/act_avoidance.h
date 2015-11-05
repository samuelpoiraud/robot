#ifndef ACT_AVOIDANCE_H
#define ACT_AVOIDANCE_H

#include "QS/QS_all.h"

#include "act_functions.h"

typedef enum{
	// Lister les actionneurs modifiants l'évitement du robot
	//ACT_AVOID_POP_COLLECT_LEFT,

	ACT_AVOID_NB = 1  //Enlever le = 1
}act_avoid_e;

typedef struct{
	Uint16 Xleft;
	Uint16 Xright;
	Uint16 Yfront;
	Uint16 Yback;
	bool_e init;
	bool_e active;
	Uint8 act_cmd;
}offset_avoid_s;


// Listing des ordres des différents actionneurs modifiants l'évitement du robot

#define ACT_AVOID_NB_MAX_CMD		5

/*EXEMPLE
typedef enum {
	ACT_AVOID_POP_COLLECT_LEFT_Open,
	ACT_AVOID_POP_COLLECT_LEFT_Mid
} ACT_AVOID_POP_COLLECT_LEFT_CMD;*/


void ACT_AVOIDANCE_init();
void ACT_AVOIDANCE_new_action(act_avoid_e act_avoid_id, Uint8 cmd, bool_e state);
void ACT_AVOIDANCE_reset_actionneur(act_avoid_e act_avoid_id);
void ACT_AVOIDANCE_new_classic_cmd(act_avoid_e act_avoid_id, Uint8 act_cmd);

#endif // ACT_AVOIDANCE_H
