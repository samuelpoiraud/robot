/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : main.h
 *	Package : Supervision
 *	Description : sequenceur de la supervision
 *	Auteur : Jacen
 *	Version 20100105
 */

#ifndef SUPERVISION_H
	#define SUPERVISION_H
	
	#include "../QS/QS_all.h"



	void Supervision_init(void);
	void Supervision_process_main(void);
	void Supervision_process_1ms(void);
	void Supervision_send_periodically_pos(Uint16 dist, Sint16 angle); // Je ne suis pas sûr des types des parametres (16 bits sur mais signé ou non?)
	
#endif /* ndef SUPERVISION_H */
