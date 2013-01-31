/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Brain_US.h
 *	Package : Projet Balise Mère
 *	Description : Rétentions des infos reçues par les balises US et calculs des positions adverses.
 *	Auteur : Nirgal
 *	Version 201208
 */
#ifndef BRAIN_US
#define BRAIN_US 
	#include "QS/QS_all.h"
	#include "main.h"
	
	void BRAIN_US_init(void);
 	
 	void BRAIN_US_add_datas(beacon_id_e beacon_id, adversary_e adversary, Sint16 distance, Uint8 fiability); 

	void BRAIN_US_timeout_update(Uint16 ms);
	
	void BRAIN_US_process_main(void);
	
	
#endif //def BRAIN_US


