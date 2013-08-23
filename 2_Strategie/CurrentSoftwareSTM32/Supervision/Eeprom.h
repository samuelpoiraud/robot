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
	#include "QS/QS_spi.h"
	
	void EEPROM_Write(Uint32 Address, Uint8 * Data, Uint8 size);
	void EEPROM_Read(Uint32 Address, Uint8 * Data, Uint8 size);
	bool_e Memory_write_granted(void);
	bool_e Memory_busy(void);
	void EEPROM_init(void);
	
	#define STATUS_MASK_WIP	0b00000001
	#define STATUS_MASK_WEL	0b00000010
	#define STATUS_MASK_BP0	0b00000100
	#define STATUS_MASK_BP1	0b00001000

	#define EEPROM_SIZE_OCTETS	(Uint32)(131072)
#endif /* ndef EEPROM_H */
