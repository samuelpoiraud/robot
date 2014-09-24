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

#include "QS/QS_IHM.h"
#include "QS/QS_types.h"

void SWITCHS_init();

// Envois un message CAN pour les boutons qui communiquent avec l'extérieure
void SWITCHS_send_msg(switch_ihm_e switch_id);
void SWITCHS_update();
bool_e SWITCHS_get(switch_ihm_e switch_id);
void SWITCHS_answer(Uint8* tab, Uint8 size);
void SWITCHS_send_all();

#endif /* ndef SWITCH_H */
