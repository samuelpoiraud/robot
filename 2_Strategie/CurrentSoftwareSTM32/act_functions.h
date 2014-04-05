/*
 *	Club Robot ESEO 2009 - 2010
 *	CHOMP
 *
 *	Fichier : act_functions.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'actionneur
 *	Auteur : Julien et Ronan
 *	Version 20110313
 */



#include "QS/QS_all.h"

#ifndef ACT_FUNCTIONS_H
#define ACT_FUNCTIONS_H

#include "queue.h"
#include "QS/QS_CANmsgList.h"
#include "act_can.h"

//Voir aussi act_can.h et state_machine_helper.h

/*
 * Exemple de code pour faire une action et l'attendre:
 * case MON_ETAT:
 *                    // On demande qu'une seule fois de faire l'action. Les actions du même actionneur peuvent
 *      if(entrance)  // être empilée et seront executées les unes après les autres
 *           ACT_fruit_mouth_goto(ACT_FRUIT_Mid);
 *
 *      //Après la demande, on attend la fin de l'action (ou des actions si plusieurs ont été empilées)
 *      //Pour la liste des actionneurs (comme ACT_QUEUE_Fruit), voir l'enum queue_e
 *      state = check_act_status(ACT_QUEUE_Fruit, MON_ETAT, ETAT_OK, ETAT_ERREUR);
 *
 *      //check_act_status est définie dans state_machine_helper.h/c
 *      break;
 */

/* Fonctions empilables */

////////////////////////////////////////
//////////////// PIERRE ////////////////
////////////////////////////////////////

//ACT_truc_enum = MSG_CAN,
// ...
typedef enum {
	ACT_FRUIT_Verrin_Open = ACT_FRUIT_MOUTH_OPEN,
	ACT_FRUIT_Verrin_Close = ACT_FRUIT_MOUTH_CLOSE,
	ACT_FRUIT_Verrin_Canceled = ACT_FRUIT_MOUTH_CANCELED,
	ACT_FRUIT_Verrin_Stop = ACT_FRUIT_MOUTH_STOP,
	ACT_FRUIT_Labium_Open = ACT_FRUIT_LABIUM_OPEN,
	ACT_FRUIT_Labium_Close = ACT_FRUIT_LABIUM_CLOSE,
	ACT_FRUIT_Labium_Stop = ACT_FRUIT_LABIUM_STOP
} ACT_fruit_mouth_cmd_e;

typedef enum {
	ACT_Lance_1_BALL =  ACT_LANCELAUNCHER_RUN_1_BALL,
	ACT_Lance_5_BALL = ACT_LANCELAUNCHER_RUN_5_BALL,
	ACT_Lance_ALL = ACT_LANCELAUNCHER_RUN_ALL,
	ACT_Lance_Stop = ACT_LANCELAUNCHER_STOP
} ACT_lance_launcher_cmd_e;

typedef enum {
	ACT_Filet_Idle = ACT_FILET_IDLE,
	ACT_Filet_Launched = ACT_FILET_LAUNCHED,
	ACT_Filet_Stop = ACT_FILET_STOP
} ACT_filet_cmd_e;

typedef enum {
	ACT_Small_arm_Idle = ACT_SMALL_ARM_IDLE,
	ACT_Small_arm_Mid = ACT_SMALL_ARM_MID,
	ACT_Small_arm_Deployed = ACT_SMALL_ARM_DEPLOYED,
	ACT_Small_arm_Stop = ACT_SMALL_ARM_STOP
} ACT_small_arm_cmd_e;

typedef enum {
	ACT_Pompe_Normal = ACT_POMPE_NORMAL,
	ACT_Pompe_Reverse = ACT_POMPE_REVERSE,
	ACT_Pompe_Stop = ACT_POMPE_STOP
} ACT_pompe_cmd_e;

bool_e ACT_fruit_mouth_goto(ACT_fruit_mouth_cmd_e cmd);
bool_e ACT_lance_launcher_run(ACT_lance_launcher_cmd_e cmd,Uint16 param);
bool_e ACT_filet_launch(ACT_filet_cmd_e cmd);
bool_e ACT_small_arm_goto(ACT_small_arm_cmd_e cmd);
bool_e ACT_pompe_order(ACT_pompe_cmd_e cmd, Uint8 param);
bool_e ACT_arm_goto(ARM_state_e position);
bool_e ACT_arm_goto_XY(ARM_state_e position, Sint16 x, Sint16 y, Uint8 z);

#endif /* ndef ACT_FUNCTIONS_H */
