#ifndef ACT_AVOIDANCE_H
#define ACT_AVOIDANCE_H

#include "../QS/QS_all.h"
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

// BIG robot
typedef enum {
	ACT_AVOID_CYLINDER_PUSHER_LEFT_Out
} ACT_AVOID_CYLINDER_PUSHER_LEFT_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_PUSHER_RIGHT_Out
} ACT_AVOID_CYLINDER_PUSHER_RIGHT_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_SLIDER_LEFT_Out,
	ACT_AVOID_CYLINDER_SLIDER_LEFT_Almost_out,
	ACT_AVOID_CYLINDER_SLIDER_LEFT_Almost_out_with_cylinder
} ACT_AVOID_CYLINDER_SLIDER_LEFT_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_SLIDER_RIGHT_Out,
	ACT_AVOID_CYLINDER_SLIDER_RIGHT_Almost_out,
	ACT_AVOID_CYLINDER_SLIDER_RIGHT_Almost_out_with_cylinder
} ACT_AVOID_CYLINDER_SLIDER_RIGHT_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_ARM_LEFT_Out
} ACT_AVOID_CYLINDER_ARM_LEFT_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_ARM_RIGHT_Out
} ACT_AVOID_CYLINDER_ARM_RIGHT_CMD;

typedef enum {
	ACT_AVOID_ORE_GUN_Down
} ACT_AVOID_ORE_GUN_CMD;

typedef enum {
	ACT_AVOID_ORE_ROLLER_ARM_Out
} ACT_AVOID_ORE_ROLLER_ARM_CMD;

typedef enum {
	ACT_AVOID_ORE_WALL_Out
} ACT_AVOID_ORE_WALL_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_BALANCER_LEFT_Out
}ACT_AVOID_CYLINDER_BALANCER_LEFT_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_BALANCER_RIGHT_Out
}ACT_AVOID_CYLINDER_BALANCER_RIGHT_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_DISPOSE_LEFT_Take,
	ACT_AVOID_CYLINDER_DISPOSE_LEFT_Raise,
	ACT_AVOID_CYLINDER_DISPOSE_LEFT_Dispose
}ACT_AVOID_CYLINDER_DISPOSE_LEFT_CMD;

typedef enum {
	ACT_AVOID_CYLINDER_DISPOSE_RIGHT_Take,
	ACT_AVOID_CYLINDER_DISPOSE_RIGHT_Raise,
	ACT_AVOID_CYLINDER_DISPOSE_RIGHT_Dispose
}ACT_AVOID_CYLINDER_DISPOSE_RIGHT_CMD;



// SMALL robot
typedef enum {
	ACT_AVOID_SMALL_CYLINDER_SLIDER_Out,
	ACT_AVOID_SMALL_CYLINDER_SLIDER_Almost_out,
	ACT_AVOID_SMALL_CYLINDER_SLIDER_Almost_out_with_cylinder
} ACT_AVOID_SMALL_CYLINDER_SLIDER_CMD;

typedef enum {
	ACT_AVOID_SMALL_CYLINDER_ARM_Out
} ACT_AVOID_SMALL_CYLINDER_ARM_CMD;

typedef enum {
	ACT_AVOID_SMALL_CYLINDER_DISPOSE_Take,
	ACT_AVOID_SMALL_CYLINDER_DISPOSE_Raise,
	ACT_AVOID_SMALL_CYLINDER_DISPOSE_Dispose
}ACT_AVOID_SMALL_CYLINDER_DISPOSE_CMD;

void ACT_AVOIDANCE_init();
void ACT_AVOIDANCE_new_action(queue_id_e act_avoid_id, Uint8 cmd, bool_e state);
void ACT_AVOIDANCE_reset_actionneur(queue_id_e act_avoid_id);
void ACT_AVOIDANCE_new_classic_cmd(queue_id_e act_avoid_id, Uint8 act_cmd);

#endif // ACT_AVOIDANCE_H
