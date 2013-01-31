/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Tableau.h
 *	Package : Balises
 *	Description : tableaux et fonctions permettant le calcul de la position
 *   				du robot adverse selon les angles mesurés
 *	Auteur : Nirgal
 *	Version 20090210
 */
 
 #ifndef TABLEAU_H
	#define TABLEAU_H
	#include "QS\QS_all.h"

	#define BAD (16383)	//Valeurs non cohérentes...
	
	Sint16 find_x(Sint16 a, Sint16 b);
	Sint16 find_y(Sint16 a, Sint16 b);
//	#define ACTIVER_TABLEAU_C
	#ifdef TABLEAU_C
	#ifdef ACTIVER_TABLEAU_C
		//variables privées
		const Sint16 __attribute__ ((space(psv))) tableau_x [64][128];
		const Sint16 __attribute__ ((space(psv))) tableau_y [64][128];
		Sint16 retour;
	#endif
	#endif /*def TABLEAU_C*/
	
	
	
	#ifdef TABLEAU_C








	#endif /* def TABLEAU_C */
#endif /* ndef TABLEAU_H */
