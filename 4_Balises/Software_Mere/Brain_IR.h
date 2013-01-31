/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Brain_IR.h
 *	Package : Projet Balise Mère
 *	Description : Traitement des infos reçues par les balises IR et calculs des positions adverses.
 *	Auteur : Nirgal
 *	Version 201208
 */
#ifndef BRAIN_IR
#define BRAIN_IR 
	#include "QS/QS_all.h"
 	#include "Eyes.h"
	

	void BRAIN_IR_init(void);
 	
 	void BRAIN_IR_task_add(adversary_eyes_buffers_t * adversary_eyes_buffers);
 	
	void BRAIN_IR_process_main(void);
	
	
#endif //def BRAIN_US


