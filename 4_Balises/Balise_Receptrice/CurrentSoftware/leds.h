/*
 *	Club Robot 2015
 *
 *	Fichier : leds.h
 *	Package : Balise receptrice
 *	Description : Gestion des leds
 *	Auteur : Valentin
 */

#ifndef LEDS_H
	#define LEDS_H

#include "QS/QS_all.h"
#include "QS/QS_types.h"
#include "brain.h"
#include "QS/QS_CANmsgList.h"

typedef enum{
	LEDS_RED,
	LEDS_BLUE,
	LEDS_GREEN,
	NB_LEDS_COLOR
}Led_balise_color_e;

void LEDS_init();
void LEDS_mux_process_it();
void LEDS_manager_process_it();

//Gestion de la couleur de l'équipe
void LEDS_set_team_color(Led_balise_color_e color);
void LEDS_set_infos_evitement(CAN_msg_t * msg);
void LEDS_rf_sync();

#endif /*LEDS_H*/
