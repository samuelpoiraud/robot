/*
 *	Club Robot ESEO 2012-2015
 *
 *	Fichier : motor.h
 *	Package : Balise Emettrice
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit être le plus constante possible.
 *	Auteur : Nirgal(PIC)/Arnaud(STM32)
 *	Version 201203
 */

#ifndef SYNCHRO_BALISES_H
	#define SYNCHRO_BALISES_H

	#include "QS/QS_all.h"
	#include "QS/QS_rf.h"

	#define PERIODE_FLASH		50		//Période de répétition des flashs [nombre de step]	//Période du flash en µs = PERIODE_FLASH * DUREE_STEP
	#define NO_FLASH_TIME		4		//[nb de step] 8ms ou on emet rien au début et à la fin, pour avoir donc 8 ms entre la fin de l'émission d'un balise et le début de l'autre

	void SYNCRF_init();
	bool_e SYNCRF_is_synchronized(void);
	void SYNCRF_process_main();
	void SyncRF_process_it();
	void SYNCRF_sendRequest();

	bool_e RF_CAN_data_ready();
	CAN_msg_t RF_CAN_get_next_msg();


#endif // SYNCHRO_BALISES_H

