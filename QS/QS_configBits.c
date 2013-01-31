/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi-Tech', PACMAN
 *
 *  Fichier : QS_configBits.c
 *  Package : Qualité Soft
 *  Description : Configuration interne du PIC
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#include "QS_all.h"

/* Bits de configuration du dsPIC .
A besoin de se trouver apres le header du pic
et seulement une fois dans le projet */
#ifdef FREQ_10MHZ
	_FOSC(CSW_FSCM_OFF & XT_PLL4) /* XT PLL x4 & Failsafe clock off */
#elif defined(FREQ_INTERNAL_CLK)
	_FOSC(CSW_FSCM_OFF & FRC_PLL4) // oscillateur interne 7.74MHz, pour debug au robot
#elif defined(FREQ_20MHZ)
	_FOSC(CSW_FSCM_OFF & XT_PLL8) /* XT PLL x8 & Failsafe clock off */
#elif defined(FREQ_40MHZ)
	_FOSC(CSW_FSCM_OFF & XT_PLL16) /* XT PLL x16 & Failsafe clock off */
#else
	#error pas d horloge selectionnee
#endif /* defined FREQ_xxMHZ */
_FWDT(WDT_OFF) /* WatchDog off */
_FBORPOR(PBOR_OFF & PWRT_16 & MCLR_EN) /* MCLR activé & demarrage retardé de 16ms */
_FGS(CODE_PROT_OFF) /* Disable code protection */

__asm__(".global HEAPSIZE");
__asm__(".equiv HEAPSIZE,0x100");
