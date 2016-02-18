/*
 *	Club Robot ESEO 2012-2015
 *
 *	Fichier : motor.h
 *	Package : Balise Emettrice
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit �tre le plus constante possible.
 *	Auteur : Nirgal(PIC)/Arnaud(STM32)
 *	Version 201203
 */

#ifndef SYNCHRO_BALISES_H
	#define SYNCHRO_BALISES_H

	#include "QS/QS_all.h"
	#include "QS/QS_rf.h"

	#define PERIODE_FLASH		50		//P�riode de r�p�tition des flashs [nombre de step]	//P�riode du flash en �s = PERIODE_FLASH * DUREE_STEP
	#define NO_FLASH_TIME		4		//[nb de step] 8ms ou on emet rien au d�but et � la fin, pour avoir donc 8 ms entre la fin de l'�mission d'un balise et le d�but de l'autre

	void SYNCRF_init();
	bool_e SYNCRF_is_synchronized(void);
	void SYNCRF_process_main();
	void SyncRF_process_it();
	void SYNCRF_sendRequest();

	bool_e RF_CAN_data_ready();
	CAN_msg_t RF_CAN_get_next_msg();


#endif // SYNCHRO_BALISES_H

