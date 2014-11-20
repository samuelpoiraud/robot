/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : led.h
 *	Package : Carte IHM
 *	Description : Contrôle les leds
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#ifndef LED_H
#define LED_H

#include "QS/QS_all.h"

#include "QS/QS_IHM.h"

void LEDS_init();
void LEDS_process_it(void);
void LEDS_get_msg(CAN_msg_t *msg);

#endif /* ndef LED_H */
