/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : QS_adc.h
 *  Package : Qualit� Soft
 *  Description : 	Configuration et utilisation des convertisseurs
					Analogiques/Numeriques.
 *  Auteur : Lugancy, modifi� par Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#ifndef QS_ADC_H
	#define QS_ADC_H
	
	#include "QS_all.h"
	
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
