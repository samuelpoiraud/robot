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
		void SPI1_put_byte(Uint8 msg);
		bool_e SPI1_data_ready();
		Uint8 SPI1_get_next_msg();
	#endif /* def USE_SPI1 */
	
	#ifdef USE_SPI2
		void SPI2_write(Uint8 msg);
		Uint8 SPI2_read();
		void SPI2_put_byte(Uint8 msg);
		bool_e SPI2_data_ready();
		Uint8 SPI2_get_next_msg();
	#endif /* def USE_SPI2 */
	
#endif /* ndef QS_SPI_H */
