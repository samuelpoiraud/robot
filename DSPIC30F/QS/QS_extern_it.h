/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : QS_extern_it.h
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des "IT Externe" dsPIC30F6010A
 *					(INT1, INT2, INT3, INT4)
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20091023
 */

#ifndef QS_EXTERN_IT_H
	#define QS_EXTERN_IT_H
	#include "QS_all.h"

	typedef enum
	{
		INT0 = 0,
		INT1 = 1,
		INT2 = 2,
		INT3 = 3,
		INT4 = 4
	} EXTERN_IT_e;

	typedef enum
	{
		RISING_EDGE, FALLING_EDGE
	} edge_e;

	void EXTERN_IT_set_priority(EXTERN_IT_e channel, Uint8 priority);

	void EXTERN_IT_enable(EXTERN_IT_e channel, edge_e edge);
	//Front : 1 pour front descendant, 0 pour front montant
	//Channel : INT0, INT1, INT2, INT3 ou INT4

	void EXTERN_IT_disable(EXTERN_IT_e channel);

	void EXTERN_IT_acknowledge(EXTERN_IT_e channel);

	/*Fonctions appelées sur Interruptions...
	INT0 : void _ISR _INT0Interrupt(void)
	INT1 : void _ISR _INT1Interrupt(void)
	INT2 : void _ISR _INT2Interrupt(void)
	INT3 : void _ISR _INT3Interrupt(void)
	INT4 : void _ISR _INT4Interrupt(void)
	*/
#endif /* ndef QS_EXTERN_IT_H */
