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
