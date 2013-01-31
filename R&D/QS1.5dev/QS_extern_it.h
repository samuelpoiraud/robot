/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : QS_extern_it.h
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des "IT Externe" dsPIC30F6010A
 *					(INT1, INT2, INT3, INT4)
 *	Auteur : Jacen
 *	Version 20091023
 */
 
#ifndef QS_EXTERN_IT_H
	#define QS_EXTERN_IT_H
	#include "..\QS\QS_all.h"
	
	typedef enum 
	{
		INT0, INT1, INT2, INT3, INT4
	} EXTERN_IT_e;
	
	typedef enum
	{
		RISING_EDGE, FALLING_EDGE
	} edge_e;	
			
	void EXTERN_IT_enable(EXTERN_IT_e channel, edge_e edge);
	//Front : 1 pour front descendant, 0 pour front montant
	//Channel : INT0, INT1, INT2, INT3 ou INT4
		
	void EXTERN_IT_disable(EXTERN_IT_e channel);
			
	void EXTERN_IT_acknowledge(EXTERN_IT_e channel);
	
	/*Fonctions appelées sur Interruptions...
	INT1 : void _ISR _INT0Interrupt(void)
	INT2 : void _ISR _INT1Interrupt(void)
	INT3 : void _ISR _INT2Interrupt(void)
	INT4 : void _ISR _INT3Interrupt(void)
	*/
#endif /* ndef QS_EXTERN_IT_H */
