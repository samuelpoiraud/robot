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
 *
 *	SPI2_ON_DMA					: Activation du DMA sur l'SPI 2
 */


 #ifndef QS_SPI_H
	#define QS_SPI_H
	#include "QS_all.h"

	#include "stm32f4xx_spi.h"

	typedef enum{
		SPI_DATA_SIZE_8_BIT,
		SPI_DATA_SIZE_16_BIT
	}spi_data_size_e;

	void SPI_init(void);

	// @ref SPI_BaudRate_Prescaler
	void SPI_setBaudRate(SPI_TypeDef* SPIx, uint16_t SPI_BaudRatePrescaler);

	// @ref SPI_Clock_Phase
	void SPI_setCPHA(SPI_TypeDef* SPIx, uint16_t SPI_CPHA);

	void SPI_setDataSize(SPI_TypeDef* SPIx, spi_data_size_e spi_data_size);

	Uint16 SPI_exchange(SPI_TypeDef* SPIx, Uint16 c);
	void SPI_write(SPI_TypeDef* SPIx, Uint16 msg);
	Uint16 SPI_read(SPI_TypeDef* SPIx);

	#ifdef USE_SPI1
		Uint8 SPI1_exchange(Uint8 c);
		void SPI1_write(Uint8 msg);
		Uint8 SPI1_read();
		#define SPI1_put_byte SPI1_write
	#endif /* def USE_SPI1 */

	#ifdef USE_SPI2
		Uint16 SPI2_exchange(Uint16 c);
		void SPI2_write(Uint16 msg);
		Uint16 SPI2_read();
		#define SPI2_put_byte SPI2_write

		void SPI2_DMA_send16BitLoop(Uint16 msg, Uint32 count);
		void SPI2_DMA_send16BitArray(Uint16 *data, Uint32 count);
	#endif /* def USE_SPI2 */

#endif /* ndef QS_SPI_H */
