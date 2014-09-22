/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : switch.h
 *	Package : Carte IHM
 *	Description : Contrôle les switchs
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "QS/QS_buttons.h"

typedef void(*switch_action_t)(void);

typedef enum
{
	SW_COLOR=0,
	SW_LCD,
	SW_INT_NUMBER
}switch_int_id_e;

void SWITCHS_init();

// Définit les fonctions associé pour les switchs internes
void SWITCHS_INT_define_actions(switch_int_id_e switch_id,switch_action_t changement);

// Envois un message CAN pour les boutons qui communiquent avec l'extérieure
void SWITCHS_send_msg(switch_ihm_e switch_id);
void SWITCHS_update();
void SWITCHS_VERBOSE(void);

#endif /* ndef SWITCH_H */
