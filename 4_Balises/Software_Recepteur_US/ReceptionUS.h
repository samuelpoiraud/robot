/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : ReceptionUS.h
 *	Package : Balise Récepteur US
 *	Description : 
 *	Auteur : Nirgal
 *	Version 201012
 */

#ifndef RECEPTIONUS_H
	#define RECEPTIONUS_H
	
	#include "QS/QS_all.h"

	
	#define	ERREUR_PAS_DE_SYNCHRO				0b00000001
	#define ERREUR_CONVOLUTION_NON_CONFORME 	0b00000010
	#define	ERREUR_SIGNAL_SATURE				0b00000100
	#define ERREUR_TROP_PROCHE					0b00001000
	#define	ERROR_OBSOLESCENCE					0b10000000

	//#undef abs	//Patch pour pouvoir inclure dsp.h sans craindre une redéfinition de abs(), déjà définit par QS_macro.h !!!
	//#include <dsp.h>

	#define	PORT_SPI_CHIP_SELECT 	PORTEbits.RE1
		
	void ReceptionUS_init(void);
		
	void ReceptionUS_process_main(void);
	
	void ReceptionUS_next_step(void);
	
	void ReceptionUS_traiter_signal(void);
	
	void ReceptionUS_step_init(void);
	
	void ReceptionUS_afficher_signal(Sint16 * signal, Uint16 taille);
	
	void Reception_US_reset_dataready(void);
	
	bool_e Reception_US_get_dataready(void);
	
	Uint8 ReceptionUS_get_fiabilite(void);
	
	Uint16 ReceptionUS_get_distance_cm(void);
	
	
	typedef enum
	{
		ADVERSARY_1 = 0,
		ADVERSARY_2,
		ADVERSARY_NUMBER
	}adversary_e;
	
		
#endif /* ndef RECEPTIONUS_H */
