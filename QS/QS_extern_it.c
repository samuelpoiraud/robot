/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : QS_extern_it.c
 *	Package : Qualite Soft
 *	Description : fonction de manipulation des IT externes du dsPIC30F6010A
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#include "QS_extern_it.h"

/*
Priority, de 0(priorité basse) à 7(priorité haute).
Channel : de INT0 à INT4.
*/
void EXTERN_IT_set_priority(EXTERN_IT_e channel, Uint8 priority)
{
	if(priority > 7)
		priority = 7;
		
	switch(channel)
	{
		case INT0:		
			IPC0bits.INT0IP  = priority;
		break;
		case INT1:
			IPC4bits.INT1IP = priority;			
		break;
		case INT2:
			IPC5bits.INT2IP = priority;			
		break;
		case INT3:
			IPC9bits.INT3IP  = priority;				
		break;
		case INT4:
			IPC9bits.INT4IP = priority;				
		break;
		default :
		break;
	}
}	


void EXTERN_IT_enable(EXTERN_IT_e channel, edge_e edge)
{
	switch(channel)
	{
		case INT0:		
			INTCON2bits.INT0EP = edge;
			_INT0IE = 1;
		break;
		case INT1:
			INTCON2bits.INT1EP = edge;
			_INT1IE = 1;				
		break;
		case INT2:
			INTCON2bits.INT2EP = edge;
			_INT2IE = 1;			
		break;
		case INT3:
			INTCON2bits.INT3EP = edge;
			_INT3IE = 1;				
		break;
		case INT4:
			INTCON2bits.INT4EP = edge;
			_INT4IE = 1;				
		break;
		default :
		break;
	}	
}	
	
void EXTERN_IT_disable(EXTERN_IT_e channel)
{
switch(channel)
	{
		case INT0:		
			_INT0IE = 0;
			_INT0IF = 0;
		break;
		case INT1:
			_INT1IE = 0;				
			_INT1IF = 0;
		break;
		case INT2:
			_INT2IE = 0;
			_INT2IF = 0;			
		break;
		case INT3:
			_INT3IE = 0;
			_INT3IF = 0;				
		break;
		case INT4:
			_INT4IE = 0;
			_INT4IF = 0;				
		break;
		default :
		break;
	}	
}	

		
void EXTERN_IT_acknowledge(EXTERN_IT_e channel)
{
	switch(channel)
	{
		case INT0:		
			_INT0IF = 0;
		break;
		case INT1:
			_INT1IF = 0;				
		break;
		case INT2:
			_INT2IF = 0;			
		break;
		case INT3:
			_INT3IF = 0;				
		break;
		case INT4:
			_INT4IF = 0;				
		break;
		default :
		break;
	}	
}	
