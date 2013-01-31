/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_qei_on_it.h
 *	Package : Qualite Software
 *	Description :	Utilise deux IT externes pour décoder les
					signaux d'un codeur incrémental
 *	Auteur : Jacen
 *	Version 20091029
 */

#ifndef QS_QEI_ON_IT_H
#define QS_QEI_ON_IT_H
#include "../QS/QS_all.h"


#ifdef USE_QEI_ON_IT

	// Initialisation de l'interface
	void QEI_ON_IT_init(); 

	// Recupération de la valeur du codeur
	Sint16 QEI_ON_IT_get_count();

	#ifdef QS_QEI_ON_IT_C

		#include "../QS/QS_extern_it.h"

		// Fonction d'IT utilisées
		#if QEI_ON_IT_QA == 0
				#define QEI_ON_IT_QA_IT			_INT0Interrupt
				#define QEI_ON_IT_QA_PIN		PORTFbits.RF6
		#elif QEI_ON_IT_QA == 1
				#define QEI_ON_IT_QA_IT			_INT1Interrupt
				#define QEI_ON_IT_QA_PIN		PORTEbits.RE8
		#elif QEI_ON_IT_QA == 2
				#define QEI_ON_IT_QA_IT			_INT2Interrupt
				#define QEI_ON_IT_QA_PIN		PORTEbits.RE9
		#elif QEI_ON_IT_QA == 3
				#define QEI_ON_IT_QA_IT			_INT3Interrupt
				#define QEI_ON_IT_QA_PIN		PORTAbits.RA14
		#elif QEI_ON_IT_QA == 4
				#define QEI_ON_IT_QA_IT			_INT4Interrupt
				#define QEI_ON_IT_QA_PIN		PORTAbits.RA15
		#else
			#error "QEI_ON_IT_QA doit etre 0 1 2 3 ou 4"
		#endif /* QEI_ON_IT_QA == n */

		#if QEI_ON_IT_QB == 0
				#define QEI_ON_IT_QB_IT			_INT0Interrupt
				#define QEI_ON_IT_QB_PIN		PORTFbits.RF6
		#elif QEI_ON_IT_QB == 1
				#define QEI_ON_IT_QB_IT			_INT1Interrupt
				#define QEI_ON_IT_QB_PIN		PORTEbits.RE8
		#elif QEI_ON_IT_QB == 2
				#define QEI_ON_IT_QB_IT			_INT2Interrupt
				#define QEI_ON_IT_QB_PIN		PORTEbits.RE9
		#elif QEI_ON_IT_QB == 3
				#define QEI_ON_IT_QB_IT			_INT3Interrupt
				#define QEI_ON_IT_QB_PIN		PORTAbits.RA14
		#elif QEI_ON_IT_QB == 3
				#define QEI_ON_IT_QB_IT			_INT4Interrupt
				#define QEI_ON_IT_QB_PIN		PORTAbits.RA15
		#else
			#error "QEI_ON_IT_QB doit etre 0 1 2 3 ou 4"
		#endif /* QEI_ON_IT_QB == n */
			
	#endif /* def QS_QEI_ON_IT_C */
#endif /* def USE_QEI_ON_IT */	
#endif /* ndef QS_QEI_ON_IT_H */
