#ifndef ACT_AVOIDANCE_H
#define ACT_AVOIDANCE_H

#include "QS/QS_all.h"

#include "act_functions.h"

typedef enum{
	// Lister les actionneurs modifiants l'évitement du robot
	//ACT_AVOID_POP_COLLECT_LEFT,
	ACT_AVOID_FISH_UNSTICK_ARM,
	ACT_AVOID_BLACK_SAND_CIRCLE,
	ACT_AVOID_BOTTOM_DUNE_LEFT,
	ACT_AVOID_MIDDLE_DUNE_LEFT,
	ACT_AVOID_CONE_DUNE,
	ACT_AVOID_DUNIX_LEFT,
	ACT_AVOID_DUNIX_RIGHT,
	ACT_AVOID_SAND_LOCKER_LEFT,
	ACT_AVOID_SAND_LOCKER_RIGHT,



	ACT_AVOID_NB //= 1  //Enlever le = 1
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

typedef enum {
	ACT_AVOID_FISH_UNSTICK_ARM_Open,
	ACT_AVOID_FISH_UNSTICK_ARM_Mid,
	ACT_AVOID_FISH_UNSTICK_ARM_Close
} ACT_AVOID_FISH_UNSTICK_ARM_CMD;

typedef enum {
	ACT_AVOID_BLACK_SAND_CIRCLE_Open
} ACT_AVOID_BLACK_SAND_CIRCLE_CMD;

typedef enum {
	ACT_AVOID_BOTTOM_DUNE_LEFT_Open
} ACT_AVOID_BOTTOM_DUNE_LEFT_CMD;

typedef enum {
	ACT_AVOID_MIDDLE_DUNE_LEFT_Open
} ACT_AVOID_MIDDLE_DUNE_LEFT_CMD;

typedef enum {
	ACT_AVOID_CONE_DUNE_Open
} ACT_AVOID_CONE_DUNE_CMD;

typedef enum {
	ACT_AVOID_DUNIX_LEFT_Open
} ACT_AVOID_DUNIX_LEFT_CMD;

typedef enum {
	ACT_AVOID_DUNIX_RIGHT_Open
} ACT_AVOID_DUNIX_RIGHT_CMD;

typedef enum {
	ACT_AVOID_SAND_LOCKER_LEFT_Open
} ACT_AVOID_SAND_LOCKER_LEFT_CMD;

typedef enum {
	ACT_AVOID_SAND_LOCKER_RIGHT_Open
} ACT_AVOID_SAND_LOCKER_RIGHT_CMD;




void ACT_AVOIDANCE_init();
void ACT_AVOIDANCE_new_action(act_avoid_e act_avoid_id, Uint8 cmd, bool_e state);
void ACT_AVOIDANCE_reset_actionneur(act_avoid_e act_avoid_id);
void ACT_AVOIDANCE_new_classic_cmd(act_avoid_e act_avoid_id, Uint8 act_cmd);

#endif // ACT_AVOIDANCE_H
