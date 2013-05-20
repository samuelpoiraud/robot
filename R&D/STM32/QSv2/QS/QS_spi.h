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
 
 #ifndef QS_SPI_H
	#define QS_SPI_H
	#include "QS_all.h"
		
	void SPI_init(void);
	
	#ifdef USE_SPI1
		void SPI1_write(Uint8 msg);
		Uint8 SPI1_read();
	#endif /* def USE_SPI1 */
	
	#ifdef USE_SPI2
		void SPI2_write(Uint8 msg);
		Uint8 SPI2_read();
	#endif /* def USE_SPI2 */
	
#endif /* ndef QS_SPI_H */
