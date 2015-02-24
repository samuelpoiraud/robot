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

/*typedef enum {
	ACT_TORCH_Locker_Lock = ACT_TORCH_LOCKER_LOCK,
	ACT_TORCH_Locker_Unlock = ACT_TORCH_LOCKER_UNLOCK,
	ACT_TORCH_Locker_Inside = ACT_TORCH_LOCKER_INSIDE,
	ACT_TORCH_Locker_Stop = ACT_TORCH_LOCKER_STOP
} ACT_torch_locker_cmd_e;*/

typedef enum {
	ACT_pop_collect_left_closed = ACT_POP_COLLECT_LEFT_CLOSED,
	ACT_pop_collect_left_open = ACT_POP_COLLECT_LEFT_OPEN,
	ACT_pop_collect_left_mid = ACT_POP_COLLECT_LEFT_MID,
	ACT_pop_collect_left_stop = ACT_POP_COLLECT_LEFT_STOP
} ACT_pop_collect_left_cmd_e;

typedef enum {
	ACT_pop_collect_right_closed = ACT_POP_COLLECT_RIGHT_CLOSED,
	ACT_pop_collect_right_open = ACT_POP_COLLECT_RIGHT_OPEN,
	ACT_pop_collect_right_mid = ACT_POP_COLLECT_RIGHT_MID,
	ACT_pop_collect_right_stop = ACT_POP_COLLECT_RIGHT_STOP
} ACT_pop_collect_right_cmd_e;

typedef enum {
	ACT_pop_drop_left_closed = ACT_POP_DROP_LEFT_CLOSED,
	ACT_pop_drop_left_open = ACT_POP_DROP_LEFT_OPEN,
	ACT_pop_drop_left_stop = ACT_POP_DROP_LEFT_STOP
} ACT_pop_drop_left_cmd_e;

typedef enum {
	ACT_pop_drop_right_closed = ACT_POP_DROP_RIGHT_CLOSED,
	ACT_pop_drop_right_open = ACT_POP_DROP_RIGHT_OPEN,
	ACT_pop_drop_right_stop = ACT_POP_DROP_RIGHT_STOP
} ACT_pop_drop_right_cmd_e;

typedef enum {
	ACT_back_spot_right_open = ACT_BACK_SPOT_RIGHT_OPEN,
	ACT_back_spot_right_closed = ACT_BACK_SPOT_RIGHT_CLOSED,
	ACT_back_spot_right_stop = ACT_BACK_SPOT_RIGHT_STOP
} ACT_back_spot_right_cmd_e;

typedef enum {
	ACT_back_spot_left_open = ACT_BACK_SPOT_LEFT_OPEN,
	ACT_back_spot_left_closed = ACT_BACK_SPOT_LEFT_CLOSED,
	ACT_back_spot_left_stop = ACT_BACK_SPOT_LEFT_STOP
} ACT_back_spot_left_cmd_e;

typedef enum {
	ACT_spot_pompe_right_normal = ACT_SPOT_POMPE_RIGHT_NORMAL,
	ACT_spot_pompe_right_reverse = ACT_SPOT_POMPE_RIGHT_REVERSE,
	ACT_spot_pompe_right_stop = ACT_SPOT_POMPE_RIGHT_STOP
} ACT_spot_pompe_right_cmd_e;

typedef enum {
	ACT_spot_pompe_left_normal = ACT_SPOT_POMPE_LEFT_NORMAL,
	ACT_spot_pompe_left_reverse = ACT_SPOT_POMPE_LEFT_REVERSE,
	ACT_spot_pompe_left_stop = ACT_SPOT_POMPE_LEFT_STOP
} ACT_spot_pompe_left_cmd_e;

typedef enum {
	ACT_carpet_launcher_right_idle = ACT_CARPET_LAUNCHER_RIGHT_IDLE,
	ACT_carpet_launcher_right_launch = ACT_CARPET_LAUNCHER_RIGHT_LAUNCH,
	ACT_carpet_launcher_right_stop = ACT_CARPET_LAUNCHER_RIGHT_STOP
} ACT_carpet_launcher_right_cmd_e;

typedef enum {
	ACT_carpet_launcher_left_idle = ACT_CARPET_LAUNCHER_LEFT_IDLE,
	ACT_carpet_launcher_left_launch = ACT_CARPET_LAUNCHER_LEFT_LAUNCH,
	ACT_carpet_launcher_left_stop = ACT_CARPET_LAUNCHER_LEFT_STOP
} ACT_carpet_launcher_left_cmd_e;

////////////////////////////////////////
//////////////// WOOD //////////////////
////////////////////////////////////////
typedef enum {
	ACT_pince_gauche_closed = ACT_PINCE_GAUCHE_CLOSED,
	ACT_pince_gauche_open = ACT_PINCE_GAUCHE_OPEN,
	ACT_pince_gauche_mid_pos = ACT_PINCE_GAUCHE_MID_POS,
	ACT_pince_gauche_stop = ACT_PINCE_GAUCHE_STOP
} ACT_pince_gauche_cmd_e;
typedef enum {
	ACT_pince_droite_closed = ACT_PINCE_DROITE_CLOSED,
	ACT_pince_droite_mid_pos = ACT_PINCE_DROITE_MID_POS,
	ACT_pince_droite_open = ACT_PINCE_DROITE_OPEN,
	ACT_pince_droite_stop = ACT_PINCE_DROITE_STOP
} ACT_pince_droite_cmd_e;
typedef enum {
	ACT_pince_devant_closed = ACT_PINCE_DEVANT_CLOSED,
	ACT_pince_devant_open = ACT_PINCE_DEVANT_OPEN,
	ACT_pince_devant_mid_pos = ACT_PINCE_DEVANT_MID_POS,
	ACT_pince_devant_stop = ACT_PINCE_DEVANT_STOP
} ACT_pince_devant_cmd_e;
typedef enum {
	ACT_clap_closed = ACT_CLAP_CLOSED,
	ACT_clap_open = ACT_CLAP_OPEN,
	ACT_clap_stop = ACT_CLAP_STOP
} ACT_clap_cmd_e;

typedef enum {
	ACT_pop_drop_left_Wood_closed = ACT_POP_DROP_LEFT_WOOD_CLOSED,
	ACT_pop_drop_left_Wood_open = ACT_POP_DROP_LEFT_WOOD_OPEN,
	ACT_pop_drop_left_Wood_stop = ACT_POP_DROP_LEFT_WOOD_STOP
}ACT_pop_drop_left_Wood_cmd_e;

////////////////////////////////////////
//////////////// COMMON ////////////////
////////////////////////////////////////



// -------------------------------- Fonctions de pilotage des actionneurs

// Holly
//bool_e ACT_torch_locker(ACT_torch_locker_cmd_e cmd);
bool_e ACT_pop_collect_left(ACT_pop_collect_left_cmd_e cmd);
bool_e ACT_pop_collect_right(ACT_pop_collect_right_cmd_e cmd);

bool_e ACT_pop_drop_left(ACT_pop_drop_left_cmd_e cmd);
bool_e ACT_pop_drop_right(ACT_pop_drop_right_cmd_e cmd);

bool_e ACT_back_spot_right(ACT_back_spot_right_cmd_e cmd);
bool_e ACT_back_spot_left(ACT_back_spot_left_cmd_e cmd);

bool_e ACT_spot_pompe_right(ACT_spot_pompe_right_cmd_e cmd);
bool_e ACT_spot_pompe_left(ACT_spot_pompe_left_cmd_e cmd);

bool_e ACT_carpet_launcher_right(ACT_carpet_launcher_right_cmd_e cmd);
bool_e ACT_carpet_launcher_left(ACT_carpet_launcher_left_cmd_e cmd);

// Wood
bool_e ACT_pince_gauche(ACT_pince_gauche_cmd_e cmd);
bool_e ACT_pince_droite(ACT_pince_droite_cmd_e cmd);
bool_e ACT_pince_devant(ACT_pince_devant_cmd_e cmd);
bool_e ACT_clap(ACT_clap_cmd_e cmd);
// Common


// -------------------------------- Fonctions de pilotage haut niveau des actionneurs (avec machine à état intégré)

// Holly

// Wood

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
