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

// -------------------------------- Enumeration des différents états de chaques actionneurs
// exemple :
//  ACT_truc_enum = MSG_CAN,
//  ...

////////////////////////////////////////
//////////////// HOLLY /////////////////
////////////////////////////////////////

typedef enum {
	ACT_TORCH_Locker_Lock = ACT_TORCH_LOCKER_LOCK,
	ACT_TORCH_Locker_Unlock = ACT_TORCH_LOCKER_UNLOCK,
	ACT_TORCH_Locker_Inside = ACT_TORCH_LOCKER_INSIDE,
	ACT_TORCH_Locker_Stop = ACT_TORCH_LOCKER_STOP
} ACT_torch_locker_cmd_e;

////////////////////////////////////////
//////////////// WOOD //////////////////
////////////////////////////////////////

typedef enum {
	ACT_Small_arm_Idle = ACT_SMALL_ARM_IDLE,
	ACT_Small_arm_Mid = ACT_SMALL_ARM_MID,
	ACT_Small_arm_Deployed = ACT_SMALL_ARM_DEPLOYED,
	ACT_Small_arm_Stop = ACT_SMALL_ARM_STOP
} ACT_small_arm_cmd_e;

////////////////////////////////////////
//////////////// COMMON ////////////////
////////////////////////////////////////



// -------------------------------- Fonctions de pilotage des actionneurs

// Holly
bool_e ACT_torch_locker(ACT_torch_locker_cmd_e cmd);

// Wood
bool_e ACT_small_arm_goto(ACT_small_arm_cmd_e cmd);

// Common


// -------------------------------- Fonctions de pilotage haut niveau des actionneurs (avec machine à état intégré)

// Holly

// Wood
error_e ACT_small_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

// Common


// -------------------------------- Fonctions de configuration des actionneurs
/*
 * Entrer le sid de l'actionneur à paramètrer (Liste des sid dans QS_CANmsgList.h)
 * Puis :
 *	AX12_SPEED_CONFIG
	   // wheel mode (0 à 100)
	   // position mode (0 à 500)
	AX12_TORQUE_CONFIG
	   // (0 à 100)
  */
bool_e ACT_config(Uint16 sid, Uint8 cmd, Uint16 value);

#endif /* ndef ACT_FUNCTIONS_H */
