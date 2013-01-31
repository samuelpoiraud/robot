/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_qei_on_it.c
 *	Package : Qualite Software
 *	Description :	Utilise deux IT externes pour décoder les
					signaux d'un codeur incrémental
 *	Auteur : Jacen
 *	Version 20091029
 */

#define QS_QEI_ON_IT_C
#include "../QS/QS_qei_on_it.h"

#ifdef USE_QEI_ON_IT

/*-------------------------------------
	Initialisation de l'interface
-------------------------------------*/
void QEI_ON_IT_init() 
{
	global.QEI_ON_IT_count=0; //RàZ compteur
	
	#ifdef CLUB_JUNIOR
		global.correction_erreur_bizarre = 0;
	#endif
	
	//activation des IT
	EXTERN_IT_enable(QEI_ON_IT_QA, RISING_EDGE);
	EXTERN_IT_enable(QEI_ON_IT_QB, RISING_EDGE);
}

/*-------------------------------------
	Recupération de la valeur du codeur
-------------------------------------*/
Sint16 QEI_ON_IT_get_count()
{
	return global.QEI_ON_IT_count - (global.correction_erreur_bizarre / 3);
}	

/*-------------------------------------
	Routines d'interruption
-------------------------------------*/
void _ISR QEI_ON_IT_QA_IT()
{
	EXTERN_IT_acknowledge(QEI_ON_IT_QA);

	if(QEI_ON_IT_QA_PIN)
		EXTERN_IT_enable(QEI_ON_IT_QA, FALLING_EDGE);
	else
		EXTERN_IT_enable(QEI_ON_IT_QA, RISING_EDGE);
	

	if( QEI_ON_IT_QA_PIN ^ QEI_ON_IT_QB_PIN)
		global.QEI_ON_IT_count++;
	else
	{
		global.QEI_ON_IT_count--;		
		#ifdef CLUB_JUNIOR
			global.correction_erreur_bizarre++;
		#endif
	}	
}

void _ISR QEI_ON_IT_QB_IT()
{
	EXTERN_IT_acknowledge(QEI_ON_IT_QB);

	if(QEI_ON_IT_QA_PIN)
		EXTERN_IT_enable(QEI_ON_IT_QB, FALLING_EDGE);
	else
		EXTERN_IT_enable(QEI_ON_IT_QB, RISING_EDGE);

	if( QEI_ON_IT_QA_PIN ^ QEI_ON_IT_QB_PIN)
	{
		global.QEI_ON_IT_count--;
		#ifdef CLUB_JUNIOR
			global.correction_erreur_bizarre++;
		#endif
	}	
	else
		global.QEI_ON_IT_count++;		
}
	
#endif /* def USE_QEI_ON_IT */
