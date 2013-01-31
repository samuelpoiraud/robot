/*
 *  Club Robot ESEO 2006 - 2009
 *  Game Hoover, Phoboss, Archi-Tech'
 *
 *  Fichier : QS_macro.h
 *  Package : Qualité Soft
 *  Description : Définition de macros pour tout code du robot
 *  Auteur : Axel Voitier (et un peu savon aussi)
 *	Révision 2008-2009 : Jacen
 *  Version 20081231
 */

#ifndef QS_MACRO_H
	#define QS_MACRO_H

	#define HIGHINT(x)				((x >> 8) & 0xFF)
	#define LOWINT(x)				(x & 0xFF)

	#define BITS_ON(var, mask)		((var) |= (mask))
	/* ~0 est le complement à 1 de 0, donc pour 16 bits OxFFFF) */
	/* ~0 ^ mask permet d'etre indépendant de la taille (en bits) de ~mask */
	#define BITS_OFF(var, mask)		((var) &= ~0 ^ (mask))
	#define BIT_SET(var, bitno)		((var) |= (1 << (bitno)))
	#define BIT_CLR(var, bitno)		((var) &= ~(1 << (bitno)))
	#define BIT_TEST(data, bitno)	(((data) >> (bitno)) & 0x01)

	#define MIN(a, b)				(((a) > (b)) ? (b) : (a))
	#define MAX(a, b)				(((a) > (b)) ? (a) : (b))

	#define nop()					__asm__("nop")
	/* la fonction valeur absolue pour des entiers */
	#define abs(x)					(((x) >= 0) ? (x) : (-(x)))

	#ifdef TEST_MODE
		#define assert(arg)			(void)0
		#define debug_printf(...)	printf(__VA_ARGS__)
	#elif defined(MATCH_MODE)
		#define assert(arg)			(void)0
		#define debug_printf(...)	(void)0
	#endif

	#define PI4096	12868
#endif /* ndef QS_MACRO_H */
