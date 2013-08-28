/*
 *	Club Robot ESEO 2010 - 2011 - 2013
 *	???
 *
 *	Fichier : eeprom.c
 *	Package : Supervision
 *	Description : Gestion d'une mémoire externe sur SPI
 *	Auteur : Ronan / Nirgal
 *	Version 201308
 */

#define READ 0x03 	/* Read data from memory array beginning at selected address */
#define WRITE 0x02 	/* Write data to memory array beginning at selected address */
#define WEN 0x06 	/* Set the write enable latch (enable write operations) */
#define WRDI 0x04 	/* Reset the write enable latch (disable write operations) */
#define RDSR 0x05 	/* Read STATUS register */
#define WRSR 0x01 	/* Write STATUS register */
#define PE 0x42 	/* Page Erase - erase one page in memory array */
#define SE 0xD8 	/* Sector Erase - erase one sector in memory array */
#define CE 0xC7 	/* Chip Erase - erase all sectors in memory array */
#define RDID 0xAB 	/* Release from Deep power-down and read electronic signature */
#define DPD 0xB9 	/* Deep Power-Down mode */
	#define STATUS_MASK_WIP	0b00000001
	#define STATUS_MASK_WEL	0b00000010
	#define STATUS_MASK_BP0	0b00000100
	#define STATUS_MASK_BP1	0b00001000
#define EEPROM_C
#include "Eeprom.h"


static void WriteEnable(void);

void delay_50ns(void)
{
	static volatile Uint8 delay;
	//Wait >50ns
	delay = 100;
	while(delay--);
}

void EEPROM_init(void)
{
	EEPROM_HOLD = 1;
	EEPROM_WP = 1;
	SPI_init();

	/*Test SPI...
	while(1)
	{
		volatile Uint32 i;
		Uint8 data;
		for(i=0;i<500000;i++);
		SPI2_write(0x52);
		for(i=0;i<500000;i++);
		EEPROM_CS = 0;
		for(i=0;i<500000;i++);
		EEPROM_CS = 1;
	}
	*/
}


//Attention, il est interdit de demander une écriture à cheval sur 2 pages...
//(les pages font 256 octets... )
//TODO : renommer en EEPROM_write() et EEPROM_read()
void EEPROM_Write(Uint32 Address, Uint8 * Data, Uint8 size)
{
  Uint16 i;
  while(Memory_busy()) debug_printf(".");
  WriteEnable();                      // Write Enable prior to Write
  while(!Memory_write_granted())		  // Check for WEL bit set
  {
	  WriteEnable();                      // Write Enable prior to Write
	  debug_printf("EEPROM Write FAILED - try again\n");
  }

	EEPROM_CS = 0;                             // Select Device
	delay_50ns();
	SPI2_write(WRITE);               // Send Write OpCode
	SPI2_write((Uint8)(Address >> 16));   // Send High Address Byte (seven "do t care" bits)
	SPI2_write((Uint8)(Address >> 8));	// Send Address Byte
	SPI2_write((Uint8)(Address));     	// Send Low Address Byte

	for(i=0;i<size;i++)
		SPI2_write(Data[i]);                // Write Byte to device
	delay_50ns();
	EEPROM_CS = 1;                             // Deselect Device
	 // while(Memory_busy());               // Wait for Write to Complete

	//debug_printf("config %d", SPI2CON);
	/*debug_printf("EEPROM wrote %lx : ", Address);
  	for(i=0;i<size;i++)
  		debug_printf("%02x ",Data[i]);
  	debug_printf("\n");

  	EEPROM_Read(Address, read_data, size);
  	*/
}

//Attention, il est interdit de demander une écriture à cheval sur 2 pages...
//(les pages font 256 octets... )
void EEPROM_Read(Uint32 Address, Uint8 * Data, Uint8 size)
{
	Uint16 i;
	while(Memory_busy()) debug_printf(".");
  	EEPROM_CS = 0;                             // Select Device
  	delay_50ns();
	SPI2_write(READ);                // Send Read OpCode
 	SPI2_write((Uint8)(Address >> 16));   // Send High Address Byte (seven "don t care" bits)
	SPI2_write((Uint8)(Address >> 8));	// Send Address Byte
	SPI2_write((Uint8)(Address));     	// Send Low Address Byte

  	for(i=0;i<size;i++)
  		Data[i]=SPI2_read();
  	delay_50ns();
  	EEPROM_CS = 1;                             // Deselect Device
    /*debug_printf("EEPROM read %lx : ", Address);
    for(i=0;i<size;i++)
  	  debug_printf("%02x ",Data[i]);
    debug_printf("\n");
    */
}

static void WriteEnable(void) {
    EEPROM_CS = 0;                           //Select Device
   	SPI2_write(WEN);               //Write Enable OpCode
    EEPROM_CS = 1;                           //Deselect Device
}

bool_e Memory_busy(void) {
	Uint8 status;
    EEPROM_CS = 0;                           //Select Device
    SPI2_write(RDSR);              //Read Status Register OpCode
    status=SPI2_read();         //Read Status Register
    EEPROM_CS = 1;                           //Deselect Device
    return (status & STATUS_MASK_WIP)?TRUE:FALSE;
}

//Renvoi vrai si la mémoire n'est pas protégée en écriture...
bool_e Memory_write_granted(void) 
{
	Uint8 status;
    EEPROM_CS = 0;                           //Select Device
    SPI2_write(RDSR);              //Read Status Register OpCode
 	status=SPI2_read();         //Read Status Register
    EEPROM_CS = 1;                           //Deselect Device
    return (status & STATUS_MASK_WEL)?TRUE:FALSE;
}
