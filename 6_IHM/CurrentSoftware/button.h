/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : button.h
 *	Package : Carte IHM
 *	Description : Contrôle les boutons, code inspiré de QS_button
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "QS/QS_all.h"

#include "QS/QS_IHM.h"

void BUTTONS_IHM_init();

// Envois un message CAN pour les boutons qui communiquent avec l'extérieure
void BUTTONS_IHM_send_msg(button_ihm_e button_id,bool_e push_long);
void BUTTONS_IHM_update();
void BUTTONS_IHM_process_it(Uint8 ms);

#endif /* ndef BUTTON_H */
