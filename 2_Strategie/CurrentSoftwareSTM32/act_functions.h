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

#ifndef ACT_FUNCTIONS_H
#define ACT_FUNCTIONS_H

#include "QS/QS_all.h"
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
bool_e ACT_push_order_with_timeout(ACT_sid_e sid,  ACT_order_e order, Uint16 timeout);
bool_e ACT_push_order_with_param(ACT_sid_e sid,  ACT_order_e order, Uint16 param);


// -------------------------------- Fonctions de pilotage haut niveau des actionneurs (avec machine à état intégré)

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

bool_e ACT_config(Uint16 sid, Uint8 sub_act, act_config_e cmd, Uint16 value);


#endif /* ndef ACT_FUNCTIONS_H */
