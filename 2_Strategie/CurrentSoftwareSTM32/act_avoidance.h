#ifndef ACT_AVOIDANCE_H
#define ACT_AVOIDANCE_H

#include "QS/QS_all.h"
#include "avoidance.h"
#include "act_functions.h"


typedef struct{
	Uint16 Xleft;
	Uint16 Xright;
	Uint16 Yfront;
	Uint16 Yback;
	bool_e init;
	bool_e active;
	Uint16 act_cmd;
}offset_avoid_s;


// Listing des ordres des différents actionneurs modifiants l'évitement du robot

#define ACT_AVOID_NB_MAX_CMD		5


typedef enum {
	ACT_AVOID_FISH_UNSTICK_ARM_Open,
	ACT_AVOID_FISH_UNSTICK_ARM_Mid,
	ACT_AVOID_FISH_UNSTICK_ARM_Close
} ACT_AVOID_FISH_UNSTICK_ARM_CMD;

typedef enum {
	ACT_AVOID_FISH_MAGNETIC_ARM_Open,
	ACT_AVOID_FISH_MAGNETIC_ARM_Middle
}ACT_AVOID_FISH_UNSTICK_ARM_CMD;

typedef enum {
	ACT_AVOID_BLACK_SAND_CIRCLE_Open
} ACT_AVOID_BLACK_SAND_CIRCLE_CMD;

typedef enum {
	ACT_AVOID_BOTTOM_DUNE_Lock
} ACT_AVOID_BOTTOM_DUNE_CMD;

typedef enum {
	ACT_AVOID_MIDDLE_DUNE_Lock
} ACT_AVOID_MIDDLE_DUNE_CMD;

typedef enum {
	ACT_AVOID_CONE_DUNE_Lock
} ACT_AVOID_CONE_DUNE_CMD;

typedef enum {
	ACT_AVOID_DUNIX_LEFT_Open
} ACT_AVOID_DUNIX_LEFT_CMD;

typedef enum {
	ACT_AVOID_DUNIX_RIGHT_Open
} ACT_AVOID_DUNIX_RIGHT_CMD;

typedef enum {
	ACT_AVOID_SAND_LOCKER_LEFT_Open,
	ACT_AVOID_SAND_LOCKER_LEFT_Lock_Block,
	ACT_AVOID_SAND_LOCKER_LEFT_Mid
} ACT_AVOID_SAND_LOCKER_LEFT_CMD;

typedef enum {
	ACT_AVOID_SAND_LOCKER_RIGHT_Open,
	ACT_AVOID_SAND_LOCKER_RIGHT_Lock_Block,
	ACT_AVOID_SAND_LOCKER_RIGHT_Mid
} ACT_AVOID_SAND_LOCKER_RIGHT_CMD;

typedef enum {
	ACT_AVOID_MOSFET_ACT_ALL_Normal
} ACT_AVOID_MOSFET_ACT_ALL_CMD;

typedef enum {
	ACT_AVOID_MOSFET_STRAT_0_Normal
} ACT_AVOID_MOSFET_STRAT_0_CMD;

typedef enum {
	ACT_AVOID_MOSFET_STRAT_1_Normal
} ACT_AVOID_MOSFET_STRAT_1_CMD;

typedef enum {
	ACT_AVOID_MOSFET_STRAT_2_Normal
} ACT_AVOID_MOSFET_STRAT_2_CMD;

typedef enum {
	ACT_AVOID_MOSFET_STRAT_3_Normal
} ACT_AVOID_MOSFET_STRAT_3_CMD;

typedef enum {
	ACT_AVOID_MOSFET_STRAT_4_Normal
} ACT_AVOID_MOSFET_STRAT_4_CMD;



//Pearl
typedef enum {
	ACT_AVOID_PEARL_SAND_CIRCLE_Open
} ACT_AVOID_PEARL_SAND_CIRCLE_CMD;

typedef enum {
	ACT_AVOID_LEFT_ARM_Unlock,
	ACT_AVOID_LEFT_ARM_Lock
} ACT_AVOID_LEFT_ARM_CMD;

typedef enum {
	ACT_AVOID_RIGHT_ARM_Unlock,
	ACT_AVOID_RIGHT_ARM_Lock
} ACT_AVOID_RIGHT_ARM_CMD;

typedef enum {
	ACT_AVOID_MOSFET_ACT_0_Normal
} ACT_AVOID_MOSFET_ACT_0_CMD;

typedef enum {
	ACT_AVOID_MOSFET_ACT_1_Normal
} ACT_AVOID_MOSFET_ACT_1_CMD;

typedef enum {
	ACT_AVOID_POMPE_FRONT_LEFT_Normal,
	ACT_AVOID_POMPE_FRONT_LEFT_Reverse
}ACT_AVOID_POMPE_FRONT_LEFT_CMD;

typedef enum {
	ACT_AVOID_POMPE_FRONT_RIGHT_Normal,
	ACT_AVOID_POMPE_FRONT_RIGHT_Reverse
}ACT_AVOID_POMPE_FRONT_RIGHT_CMD;

void ACT_AVOIDANCE_init();
void ACT_AVOIDANCE_new_action(queue_id_e act_avoid_id, Uint8 cmd, bool_e state);
void ACT_AVOIDANCE_reset_actionneur(queue_id_e act_avoid_id);
void ACT_AVOIDANCE_new_classic_cmd(queue_id_e act_avoid_id, Uint8 act_cmd);

#endif // ACT_AVOIDANCE_H
