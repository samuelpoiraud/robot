/*
 *  Club Robot ESEO 2009 - 2013
 *
 *  Fichier : QS_adc.h
 *  Package : Qualit� Soft
 *  Description : Gestion du convertisseur analogique/num�rique
 *  Auteur : Alexis
 *  Version 20130518
 */

#ifndef QS_ADC_H
	#define QS_ADC_H
	
	#include "QS_types.h"
	
	void ADC_init();
	
	Sint16 ADC_getValue(Uint8 channel);
	/*
	 *	cette fonction permet de r�cup�rer la valeur echantillonn�e par
	 *	le convertisseur analogique numerique (adc). Le channel est le
	 *	numero de la voie choisie parmis les voies selectionn�es,
	 *	numerotees a partir de 0.
	 *	Par exemple, si l'utilisateur a choisi USE_AN4 et USE_AN8,
	 *	il faudra lire ADC_getValue(0) pour lire l'entree analogique 4 et ADC_getValue(1) pour lire l'entree analogique 8.
	 *
	 */

#endif /* ndef QS_ADC_H */
