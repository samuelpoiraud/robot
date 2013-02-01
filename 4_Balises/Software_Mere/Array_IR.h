/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Array_IR.h
 *	Package : Balises
 *	Description : tableaux et fonctions permettant le calcul de la position
 *   				du robot adverse selon les angles mesurés
 *	Auteur : Nirgal
 *	Version 201212
 */
 
 #ifndef ARRAY_IR_H
	#define ARRAY_IR_H
	#include "QS\QS_all.h"

	#define BAD (16383)	//Valeurs non cohérentes...
	
	Sint16 find_x(Sint16 a, Sint16 b);
	Sint16 find_y(Sint16 a, Sint16 b);

#endif /* ndef ARRAY_IR_H */
