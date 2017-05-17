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

#include "../QS/QS_all.h"
#include "../QS/QS_CANmsgList.h"
#include "queue.h"
#include "act_can.h"

//Voir aussi act_can.h et state_machine_helper.h

typedef struct{
	ACT_sid_e sid;
	queue_id_e queue_id;
	char * name;
}act_link_SID_Queue_s;

typedef struct{
	bool_e info_received;
	ACT_order_e pos;
	Sint8 torque;
	Uint8 temperature;
	Sint8 load;
}act_config_s;

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

bool_e ACT_push_order_with_all_params(ACT_sid_e sid,  ACT_order_e order, Uint16 param, Uint16 timeout, bool_e run_now);
bool_e ACT_push_order(ACT_sid_e sid,  ACT_order_e order);
bool_e ACT_push_order_with_timeout(ACT_sid_e sid,  ACT_order_e order, Uint16 timeout);
bool_e ACT_push_order_with_param(ACT_sid_e sid,  ACT_order_e order, Uint16 param);
bool_e ACT_push_order_now(ACT_sid_e sid,  ACT_order_e order);

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

bool_e ACT_set_config(Uint16 sid, Uint8 sub_act, act_config_e cmd, Uint16 value);

// -------------------------------- Fonctions permettant de récupérer l'état courant de l'actionneur (uniquement pour AX12 et RX24)

/**
 * @brief ACT_get_current_state
 * @arg sid : le sid de l'actionneur
 * @arg order : l'ordre que l'on veut vérifier
 * @return : END_OK  				-> l'actionneur est dans la position demandé
 * 			 NOT_HANDLED   			-> l'actionneur n'est pas dans la position demandé
 * 			 END_WITH_TIMEOUT		-> pas de réponse actionneur
 * 			 IN_PROGRESS			-> demande en cours
 *
 * Exemple : ACT_get_current_state(MY_ACT, MY_ACT_OPEN_POS)
 * La valeur de retour sera END_OK si l'actionneur MY_ACT est bien dans la position MY_ACT_OPEN_POS.
 * La valeur de retour sera END_OK si l'actionneur MY_ACT est dans une position différente de MY_ACT_OPEN_POS.
 */
error_e ACT_check_position_config(Uint16 sid, ACT_order_e order);
error_e ACT_check_position_config_left(Uint16 sid, ACT_order_e order);
error_e ACT_check_position_config_right(Uint16 sid, ACT_order_e order);

void ACT_get_config_answer(CAN_msg_t* msg);
bool_e ACT_get_config_request(Uint16 sid, act_config_e config);


bool_e ACT_set_warner(Uint16 sid, ACT_order_e pos);
void ACT_warner_answer(CAN_msg_t* msg);
bool_e ACT_get_warner(Uint16 sid);
void ACT_reset_all_warner();

/**
 * @brief ACT_wait_state_vacuostat
 * @arg idVacuostat : correspond à l'ID mosfet associé entre 0 et 7
 * @arg stateToWait : L'état attendu du vacuostat
 * @arg in_progress : Etat en cours
 * @arg sucess : Etat à retourner si l'état du vacuostat est bon
 * @arg fail : Etat à retourner si timeout
*
 * @return : le state rentré en argument
 */
Uint8 ACT_wait_state_vacuostat(act_vacuostat_id idVacuostat, MOSFET_BOARD_CURRENT_MEASURE_state_e stateToWait, Uint8 in_progress, Uint8 sucess, Uint8 fail);

MOSFET_BOARD_CURRENT_MEASURE_state_e ACT_get_state_vacuostat(act_vacuostat_id idVacuostat);

void ACT_receive_vacuostat_msg(CAN_msg_t *msg);

/**
 * @brief ACT_get_turbine_speed
 * @arg speed : pointeur de la variable ou stocker la vitesse [RPM]
 * @arg in_progress : Etat en cours
 * @arg sucess : Etat à retourner si l'état du vacuostat est bon
 * @arg fail : Etat à retourner si timeout
*
 * @return : le state rentré en argument
 */
Uint8 ACT_get_turbine_speed(Uint16 * speed,  Uint8 in_progress, Uint8 sucess, Uint8 fail);

void ACT_set_turbine_speed(Uint16 speed);

void ACT_receive_turbine_msg(CAN_msg_t * msg);

Uint8 ACT_wait_state_color_sensor(COLOR_SENSOR_I2C_color_e color, time32_t timeout, Uint8 in_progress, Uint8 sucess, Uint8 fail);

void ACT_receive_color_sensor_msg(CAN_msg_t *msg);

#endif /* ndef ACT_FUNCTIONS_H */
