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
 *	USE_SPI1					: Activation de l'SPI 1
 *	USE_SPI2					: Activation de l'SPI 2
 */


 #ifndef QS_SPI_H
	#define QS_SPI_H
	#include "QS_all.h"

	typedef enum{
		SPI_DATA_SIZE_8_BIT,
		SPI_DATA_SIZE_16_BIT
	}spi_data_size_e;

	void SPI_init(void);

	#ifdef USE_SPI1
		Uint8 SPI1_exchange(Uint8 c);
		void SPI1_write(Uint8 msg);
		Uint8 SPI1_read();
		#define SPI1_put_byte SPI1_write
		void SPI1_setDataSize(spi_data_size_e spi_data_size);
	#endif /* def USE_SPI1 */

	#ifdef USE_SPI2
		Uint16 SPI2_exchange(Uint16 c);
		void SPI2_write(Uint16 msg);
		Uint16 SPI2_read();
		#define SPI2_put_byte SPI2_write
		void SPI2_setDataSize(spi_data_size_e spi_data_size);
	#endif /* def USE_SPI2 */

#endif /* ndef QS_SPI_H */
