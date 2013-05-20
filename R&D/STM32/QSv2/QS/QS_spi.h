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
		Uint8 SPI1_exchange(Uint8 c);
		void SPI1_write(Uint8 msg);
		Uint8 SPI1_read();
		#define SPI1_put_byte SPI1_write
	#endif /* def USE_SPI1 */
	
	#ifdef USE_SPI2
		Uint8 SPI2_exchange(Uint8 c);
		void SPI2_write(Uint8 msg);
		Uint8 SPI2_read();
		#define SPI2_put_byte SPI2_write
	#endif /* def USE_SPI2 */
	
#endif /* ndef QS_SPI_H */
