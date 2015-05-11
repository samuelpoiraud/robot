#ifndef ACT_AVOIDANCE_H
#define ACT_AVOIDANCE_H

#include "QS/QS_all.h"

#include "act_functions.h"

typedef enum{
	//ACT_AVOID_TORCH_LOCKER,
	// Lister les actionneurs modifiants l'évitement du robot
	ACT_AVOID_POP_COLLECT_LEFT,
	ACT_AVOID_POP_COLLECT_RIGHT,
	ACT_AVOID_POP_DROP_LEFT,
	ACT_AVOID_POP_DROP_RIGHT,
	ACT_AVOID_BACK_SPOT_RIGHT,
	ACT_AVOID_BACK_SPOT_LEFT,
	ACT_AVOID_CUP_NIPPER,
	ACT_AVOID_CLAP_HOLLY,
	ACT_AVOID_PINCE_DROITE,
	ACT_AVOID_PINCE_GAUCHE,
	ACT_AVOID_CLAP_WOOD,
	ACT_AVOID_CARPET_RIGHT,
	ACT_AVOID_CARPET_LEFT,
	ACT_AVOID_NB
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


typedef enum {
	ACT_AVOID_POP_COLLECT_LEFT_Open,
	ACT_AVOID_POP_COLLECT_LEFT_Mid
} ACT_AVOID_POP_COLLECT_LEFT_CMD;

typedef enum {
	ACT_AVOID_POP_COLLECT_RIGHT_Open,
	ACT_AVOID_POP_COLLECT_RIGHT_Mid
} ACT_AVOID_POP_COLLECT_RIGHT_CMD;

typedef enum {
	ACT_AVOID_POP_DROP_LEFT_Open
} ACT_AVOID_POP_DROP_LEFT_CMD;

typedef enum {
	ACT_AVOID_POP_DROP_RIGHT_Open
} ACT_AVOID_POP_DROP_RIGHT_CMD;

typedef enum {
	ACT_AVOID_BACK_SPOT_RIGHT_Open
} ACT_AVOID_BACK_SPOT_RIGHT_CMD;

typedef enum {
	ACT_AVOID_BACK_SPOT_LEFT_Open
} ACT_AVOID_BACK_SPOT_LEFT_CMD;

typedef enum {
	ACT_AVOID_CUP_NIPPER_Open,
	ACT_AVOID_CUP_NIPPER_Lock
} ACT_AVOID_CUP_NIPPER_CMD;

typedef enum {
	ACT_AVOID_CLAP_HOLLY_Left,
	ACT_AVOID_CLAP_HOLLY_Right,
	ACT_AVOID_CLAP_HOLLY_Left_Mid,
	ACT_AVOID_CLAP_HOLLY_Right_Mid
} ACT_AVOID_CLAP_HOLLY_CMD;

typedef enum {
	ACT_AVOID_PINCE_GAUCHE_Open,
	ACT_AVOID_PINCE_GAUCHE_Close
} ACT_AVOID_PINCE_GAUCHE_CMD;

typedef enum {
	ACT_AVOID_CLAP_WOOD_Open
} ACT_AVOID_CLAP_WOOD_CMD;

typedef enum {
	ACT_AVOID_PINCE_DROITE_Open,
	ACT_AVOID_PINCE_DROITE_Close
} ACT_AVOID_PINCE_DROITE_CMD;

typedef enum {
	ACT_AVOID_CARPET_LEFT_Launch
} ACT_AVOID_CARPET_LEFT_CMD;

typedef enum {
	ACT_AVOID_CARPET_RIGHT_Launch
} ACT_AVOID_CARPET_RIGHT_CMD;

void ACT_AVOIDANCE_init();
void ACT_AVOIDANCE_new_action(act_avoid_e act_avoid_id, Uint8 cmd, bool_e state);
void ACT_AVOIDANCE_reset_actionneur(act_avoid_e act_avoid_id);
void ACT_AVOIDANCE_new_classic_cmd(act_avoid_e act_avoid_id, Uint8 act_cmd);

#endif // ACT_AVOIDANCE_H
