/*
 *  Club Robot ESEO 2006 - 2010
 *  Game Hoover, Phoboss, Archi-Tech', PACMAN
 *
 *  Fichier : QS_macro.h
 *  Package : Qualité Soft
 *  Description : Définition de macros pour tout code du robot
 *  Auteur : Axel Voitier (et un peu savon aussi)
 *	Révision 2008-2009 : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100415
 */

#ifndef QS_MACRO_H
	#define QS_MACRO_H

	#define HIGHINT(x)				(((x) >> 8) & 0xFF)
	#define LOWINT(x)				((x) & 0xFF)
	#define U16FROMU8(high,low)		((((Uint16)(high))<<8)|(Uint16)(low))
	#define U32FROMU16(high,low)		((((Uint32)(high))<<16)|(Uint32)(low))
	#define U32FROMU8(higher,high,low,lower)		((((Uint32)(higher))<<24)|(((Uint32)(high))<<16)|(((Uint32)(low))<<8)|(Uint32)(lower))
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
	#define absolute(x)					(((x) >= 0) ? (x) : (-(x)))
	#ifdef VERBOSE_MODE

		#define debug_printf(...)	printf(__VA_ARGS__)

		#define assert(condition) \
			if(!(condition)) {printf("assert failed file " __FILE__ " line %d : %s", __LINE__ , #condition ); NVIC_SystemReset();}
	#else
		#define debug_printf(...) (void)0
		#define assert(condition) (void)0
	#endif
	#define PI4096	12868

	//Gestion des gros tableau, il faut changer PSVPAG avec PSV_adjust(tableau) pour être compatible avec d'autre micropro (genre stm32)
	#define _LARGEARRAY
	#define PSV_getCurrent() 0
	#define PSV_setCurrent(newpsvpag) (newpsvpag = newpsvpag)
	#define PSV_adjust(array) (void)0






#endif /* ndef QS_MACRO_H */
