/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_qei_on_it.c
 *	Package : Qualite Software
 *	Description :	Utilise deux IT externes pour décoder les
					signaux d'un codeur incrémental
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#include "QS_qei_on_it.h"

#ifdef USE_QEI_ON_IT

#include "QS_extern_it.h"

/*variable globale pour le pilote codeur sur IT */
static Sint16 m_QEI_ON_IT_count;


// Fonction d'IT utilisées
/*#if QEI_ON_IT_QA == 0
		#define QEI_ON_IT_QA_IT			_INT0Interrupt
		#define QEI_ON_IT_QA_PIN		PORTFbits.RF6
		#define QEI_ON_IT_QA_EDGE		INTCON2bits.INT0EP
		#define QEI_ON_IT_QA_EN			_INT0IE
		#define QEI_ON_IT_QA_ACK		_INT0IF
#elif QEI_ON_IT_QA == 1
		#define QEI_ON_IT_QA_IT			_INT1Interrupt
		#define QEI_ON_IT_QA_PIN		PORTEbits.RE8
		#define QEI_ON_IT_QA_EDGE		INTCON2bits.INT1EP
		#define QEI_ON_IT_QA_EN			_INT1IE
		#define QEI_ON_IT_QA_ACK		_INT1IF
#elif QEI_ON_IT_QA == 2
		#define QEI_ON_IT_QA_IT			_INT2Interrupt
		#define QEI_ON_IT_QA_PIN		PORTEbits.RE9
		#define QEI_ON_IT_QA_EDGE		INTCON2bits.INT2EP
		#define QEI_ON_IT_QA_EN			_INT2IE
		#define QEI_ON_IT_QA_ACK		_INT2IF
#elif QEI_ON_IT_QA == 3
		#define QEI_ON_IT_QA_IT			_INT3Interrupt
		#define QEI_ON_IT_QA_PIN		PORTAbits.RA14
		#define QEI_ON_IT_QA_EDGE		INTCON2bits.INT3EP
		#define QEI_ON_IT_QA_EN			_INT3IE
		#define QEI_ON_IT_QA_ACK		_INT3IF
#elif QEI_ON_IT_QA == 4
		#define QEI_ON_IT_QA_IT			_INT4Interrupt
		#define QEI_ON_IT_QA_PIN		PORTAbits.RA15
		#define QEI_ON_IT_QA_EDGE		INTCON2bits.INT4EP
		#define QEI_ON_IT_QA_EN			_INT4IE
		#define QEI_ON_IT_QA_ACK		_INT4IF
#else
	#error "QEI_ON_IT_QA doit etre 0 1 2 3 ou 4"
#endif *//* QEI_ON_IT_QA == n */

/*#if QEI_ON_IT_QB == 0
		#define QEI_ON_IT_QB_IT			_INT0Interrupt
		#define QEI_ON_IT_QB_PIN		PORTFbits.RF6
		#define QEI_ON_IT_QB_EDGE		INTCON2bits.INT0EP
		#define QEI_ON_IT_QB_EN			_INT0IE
		#define QEI_ON_IT_QB_ACK		_INT0IF
#elif QEI_ON_IT_QB == 1
		#define QEI_ON_IT_QB_IT			_INT1Interrupt
		#define QEI_ON_IT_QB_PIN		PORTEbits.RE8
		#define QEI_ON_IT_QB_EDGE		INTCON2bits.INT1EP
		#define QEI_ON_IT_QB_EN			_INT1IE
		#define QEI_ON_IT_QB_ACK		_INT1IF
#elif QEI_ON_IT_QB == 2
		#define QEI_ON_IT_QB_IT			_INT2Interrupt
		#define QEI_ON_IT_QB_PIN		PORTEbits.RE9
		#define QEI_ON_IT_QB_EDGE		INTCON2bits.INT2EP
		#define QEI_ON_IT_QB_EN			_INT2IE
		#define QEI_ON_IT_QB_ACK		_INT2IF
#elif QEI_ON_IT_QB == 3
		#define QEI_ON_IT_QB_IT			_INT3Interrupt
		#define QEI_ON_IT_QB_PIN		PORTAbits.RA14
		#define QEI_ON_IT_QB_EDGE		INTCON2bits.INT3EP
		#define QEI_ON_IT_QB_EN			_INT3IE
		#define QEI_ON_IT_QB_ACK		_INT3IF
#elif QEI_ON_IT_QB == 3
		#define QEI_ON_IT_QB_IT			_INT4Interrupt
		#define QEI_ON_IT_QB_PIN		PORTAbits.RA15
		#define QEI_ON_IT_QB_EDGE		INTCON2bits.INT4EP
		#define QEI_ON_IT_QB_EN			_INT4IE
		#define QEI_ON_IT_QB_ACK		_INT4IF
#else
	#error "QEI_ON_IT_QB doit etre 0 1 2 3 ou 4"
#endif *//* QEI_ON_IT_QB == n */



/*-------------------------------------
	Initialisation de l'interface
-------------------------------------*/
void QEI_ON_IT_init() 
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	m_QEI_ON_IT_count=0; //RàZ compteur
	
	//activation des IT
//	EXTERN_IT_enable(QEI_ON_IT_QA, RISING_EDGE);
//	EXTERN_IT_enable(QEI_ON_IT_QB, RISING_EDGE);
}

/*-------------------------------------
	Recupération de la valeur du codeur
-------------------------------------*/
Sint16 QEI_ON_IT_get_count()
{
	return m_QEI_ON_IT_count;
}	

/*-------------------------------------
	Ecriture de la valeur du codeur
-------------------------------------*/
void QEI_ON_IT_set_count(Sint16 count)
{
	m_QEI_ON_IT_count=count;
}	

/*-------------------------------------
	Routines d'interruption
-------------------------------------*/
/*void _ISR QEI_ON_IT_QA_IT()
{
	QEI_ON_IT_QA_ACK = 0;	//EXTERN_IT_acknowledge(QEI_ON_IT_QA);

	if(QEI_ON_IT_QA_PIN)
		QEI_ON_IT_QA_EDGE = FALLING_EDGE;	//EXTERN_IT_enable(QEI_ON_IT_QA, FALLING_EDGE);
	else
		QEI_ON_IT_QA_EDGE = RISING_EDGE;//EXTERN_IT_enable(QEI_ON_IT_QA, RISING_EDGE);
	//QEI_ON_IT_QA_EN = 1;
	

	if( QEI_ON_IT_QA_PIN ^ QEI_ON_IT_QB_PIN)
		m_QEI_ON_IT_count++;
	else
		m_QEI_ON_IT_count--;		
}

void _ISR QEI_ON_IT_QB_IT()
{
	QEI_ON_IT_QB_ACK = 0;	//EXTERN_IT_acknowledge(QEI_ON_IT_QB);

	if(QEI_ON_IT_QB_PIN)
		QEI_ON_IT_QB_EDGE = FALLING_EDGE;	//EXTERN_IT_enable(QEI_ON_IT_QB, FALLING_EDGE);
	else
		QEI_ON_IT_QB_EDGE = RISING_EDGE;	//	EXTERN_IT_enable(QEI_ON_IT_QB, RISING_EDGE);

	if( QEI_ON_IT_QA_PIN ^ QEI_ON_IT_QB_PIN)
		m_QEI_ON_IT_count--;
	else
		m_QEI_ON_IT_count++;		
}
*/	
#endif /* def USE_QEI_ON_IT */
