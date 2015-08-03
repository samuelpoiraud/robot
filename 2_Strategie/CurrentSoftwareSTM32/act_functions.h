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

typedef struct{
	ACT_sid_e sid;
	queue_id_e queue_id;
	char * name;
}act_link_SID_Queue_s;

extern const act_link_SID_Queue_s act_link_SID_Queue[];
Uint8 ACT_search_link_SID_Queue(ACT_sid_e sid);

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


bool_e ACT_push_order(ACT_sid_e sid,  ACT_order_e order);
bool_e ACT_push_order_with_param(ACT_sid_e sid,  ACT_order_e order, Uint16 param);


// -------------------------------- Fonctions de pilotage haut niveau des actionneurs (avec machine à état intégré)

// Holly

typedef enum{
	// Order unitaire
	ACT_MAE_SPOTIX_OPEN_ALL,
	ACT_MAE_SPOTIX_OPEN_GREAT,
	ACT_MAE_SPOTIX_OPEN_NIPPER,
	ACT_MAE_SPOTIX_TAKE_WITH_PRESENCE,
	ACT_MAE_SPOTIX_TAKE_WITHOUT_PRESENCE,
	ACT_MAE_SPOTIX_TAKE_BALL,
	ACT_MAE_SPOTIX_GO_DOWN,
	ACT_MAE_SPOTIX_GO_UP,
	ACT_MAE_SPOTIX_GO_DISPOSE,
	ACT_MAE_SPOTIX_GO_MID,
	ACT_MAE_SPOTIX_GO_MID_LOW,
	ACT_MAE_SPOTIX_RELEASE_STOCK,
	ACT_MAE_SPOTIX_UNLOCK_STOCK,
	ACT_MAE_SPOTIX_LOCK_STOCK,

	// Ordre composé
	ACT_MAE_SPOTIX_STOCK_AND_STAY_WITH_PRESENCE,
	ACT_MAE_SPOTIX_STOCK_AND_GO_DOWN_WITH_PRESENCE,
	ACT_MAE_SPOTIX_STOCK_AND_STAY_WITHOUT_PRESENCE,
	ACT_MAE_SPOTIX_STOCK_AND_GO_DOWN_WITHOUT_PRESENCE,
	ACT_MAE_SPOTIX_RELEASE_NIPPER_AND_GO_DOWN,
	ACT_MAE_SPOTIX_GO_MID_IN_OPENING_STOCK,
	ACT_MAE_SPOTIX_ULU
}ACT_MAE_holly_spotix_e;

typedef enum{
	ACT_MAE_SPOTIX_LEFT,
	ACT_MAE_SPOTIX_RIGHT,
	ACT_MAE_SPOTIX_BOTH
}ACT_MAE_holly_spotix_side_e;

typedef enum{
	ACT_MAE_CUP_TAKE,
	ACT_MAE_CUP_RELEASE,
	ACT_MAE_CUP_UP,
	ACT_MAE_CUP_MID,
	ACT_MAE_CUP_DOWN
}ACT_MAE_holly_cup_e;

typedef Sint16 spotix_order_id_t;
typedef Sint16 cup_order_id_t;

// MAE Standart
error_e ACT_MAE_holly_cup_bloquing(ACT_MAE_holly_cup_e order);
error_e ACT_MAE_holly_spotix_bloquing(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who);

// Envoyé un ordre sans attente de retour
cup_order_id_t ACT_MAE_holly_cup_do_order(ACT_MAE_holly_cup_e order);
spotix_order_id_t ACT_MAE_holly_spotix_do_order(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who);

// Fonction pour avoir les retours de l'ordre en background
bool_e ACT_MAE_holly_spotix_wait_end_order(spotix_order_id_t id);
void ACT_MAE_holly_spotix_process_main();
error_e ACT_holly_spotix_get_last_error();

bool_e ACT_MAE_holly_cup_wait_end_order(cup_order_id_t id);
void ACT_MAE_holly_cup_process_main();
error_e ACT_holly_cup_get_last_error();

// Wood



// Common
/**
 * @brief ACT_get_sensor
 * @arg act_sensor_id : le capteur voulu
 * @return :	END_OK				-> Le capteur détecte une présence
 *				END_WITH_TIMEOUT	-> pas de réponse actionneur
 *				NOT_HANDLED			-> Le capteur ne détecte aucune présence
 *				IN_PROGRESS			-> demande en cours
 */
error_e ACT_get_sensor(act_sensor_id_e act_sensor_id);

void ACT_sensor_answer(CAN_msg_t* msg);

// -------------------------------- Fonctions de configuration des actionneurs

bool_e ACT_config(Uint16 sid, Uint8 sub_act, Uint8 cmd, Uint16 value);

#endif /* ndef ACT_FUNCTIONS_H */
