/*
 *	Club Robot ESEO 2010 - 2011
 *	???
 *
 *	Fichier : eeprom.h
 *	Package : Supervision
 *	Description : Gestion d'une mémoire externe sur SPI
 *	Auteur : Ronan
 *	Version 20100929
 */

#ifndef EEPROM_H
	#define EEPROM_H
	#include "../QS/QS_all.h"
	#include "../QS/QS_spi.h"
	
	void EEPROM_write(Uint32 Address, Uint8 * Data, Uint8 size);
	void EEPROM_read(Uint32 Address, Uint8 * Data, Uint8 size);
	bool_e Memory_write_granted(void);
	bool_e Memory_busy(void);
	void EEPROM_init(void);
	

	#define EEPROM_SIZE_OCTETS	(Uint32)(131072)
#endif /* ndef EEPROM_H */
