/*
 *	Club Robot ESEO 2010 - 2011
 *	???
 *
 *	Fichier : QS_spi.h
 *	Package : Qualité Soft
 *	Description : Fonction SPI
 *	Auteur : Ronan
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100929
 */

/** ----------------  Defines possibles  --------------------
 *	USE_SSI					: Activation du SSI
 *  SSI_NB_BIT				: Nombre de bit à lire entre 1 et 64
 */


 #ifndef QS_SSI_H
	#define QS_SSI_H
	#include "QS_all.h"

	#ifdef USE_SSI
		void SSI_init(void);
		Uint64 SSI_read();
	#endif /* def USE_SSI */

#endif /* ndef QS_SSI_H */
