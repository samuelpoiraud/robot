/*-----------------------------------------------------------------------*/
/* MMC/SDSC/SDHC (in SPI mode) control module for STM32 Version 1.1.6    */
/* (C) Martin Thomas, 2010 - based on the AVR MMC module (C)ChaN, 2007   */
/*-----------------------------------------------------------------------*/

/* Copyright (c) 2010, Martin Thomas, ChaN
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
   * Neither the name of the copyright holders nor the names of
	 contributors may be used to endorse or promote products derived
	 from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */


#include "../../../../stm32f4xx/stm32f4xx.h"
#include "../../../../stm32f4xx/stm32f4xx_spi.h"
#include "../../../../stm32f4xx/stm32f4xx_dma.h"
#include "../../../../stm32f4xx/stm32f4xx_rcc.h"
#include "../../../../QS/QS_all.h"
#include "ffconf.h"
#include "diskio.h"
#include "config_pin.h"

// demo uses a command line option to define this (see Makefile):
//#define STM32_SD_USE_DMA


#ifdef STM32_SD_USE_DMA
// #warning "Information only: using DMA"
#pragma message "*** Using DMA ***"

Uint8 dma_send_buffer[515] __attribute__ ((aligned(4)));  //Utilisé par le controleur DMA quand on veut écrire un secteur (pendant ce temps le CPU continue d'executer d'autre instructions)
//515 = 512 (sector size) + 2 CRC reads + 1 response read

#endif

/* set to 1 to provide a disk_ioctrl function even if not needed by the FatFs */
#define STM32_SD_DISK_IOCTRL_FORCE      0


#define CARD_SUPPLY_SWITCHABLE   0
 //#define GPIO_PWR                 GPIOB
 //#define RCC_APB2Periph_GPIO_PWR  RCC_APB2Periph_GPIOD
 //#define GPIO_Pin_PWR             GPIO_Pin_10
 //#define GPIO_Mode_PWR            GPIO_Mode_Out_OD /* pull-up resistor at power FET */
 #define SOCKET_WP_CONNECTED      0
 #define SOCKET_CP_CONNECTED      0
 #define SPI_SD                   SPI2
 #define GPIO_CS                  SD_CS
 #define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOB
 #define DMA_Stream_SendBuffer    DMA2_Stream1
 #define DMA_Stream_SPI_SD_RX     DMA1_Stream3   //voir datasheet, page 218
 #define DMA_Stream_SPI_SD_TX     DMA1_Stream4
 #define DMA_FLAG_TC_SendBuffer   DMA_FLAG_TCIF1
 #define DMA_FLAG_SPI_SD_TC_RX    DMA_FLAG_TCIF3
 #define DMA_FLAG_SPI_SD_TC_TX    DMA_FLAG_TCIF4
 #define GPIO_SPI_SD              GPIOB
 #define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_13
 #define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_14
 #define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_15
 #define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB2PeriphClockCmd
 #define RCC_APBPeriph_SPI_SD     RCC_APB2Periph_SPI2
 #define SPI_BaudRatePrescaler_SPI_SD  SPI_BaudRatePrescaler_4




/* Definitions for MMC/SDC command */
#define CMD_GO_IDLE_STATE	(0x40+0)	/* GO_IDLE_STATE */
#define CMD_SEND_OP_COND_MMC	(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD_SEND_OP_COND_SDC	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD_SEND_IF_COND	(0x40+8)	/* SEND_IF_COND */
#define CMD_SEND_CSD	(0x40+9)	/* SEND_CSD */
#define CMD_SEND_CID	(0x40+10)	/* SEND_CID */
#define CMD_STOP_TRANSMISSION	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD_SD_STATUS_SDC	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD_SET_BLOCKLEN	(0x40+16)	/* SET_BLOCKLEN */
#define CMD_READ_SINGLE_BLOCK	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD_READ_MULTIPLE_BLOCK	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD_SET_BLOCK_COUNT_MMC	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD_SET_WR_BLK_ERASE_COUNT_SDC	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD_WRITE_BLOCK	(0x40+24)	/* WRITE_BLOCK */
#define CMD_WRITE_MULTIPLE_BLOCK	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMDAPP_CMD	(0x40+55)	/* APP_CMD */
#define CMD_READ_OCR	(0x40+58)	/* READ_OCR */


/* Card-Select Controls  (Platform dependent) */
#define SELECT()        SD_CS=0    /* MMC CS = L */
#define DESELECT()      SD_CS=1     /* MMC CS = H */

/* Manley EK-STM32F board does not offer socket contacts -> dummy values: */
#define SOCKPORT	1			/* Socket contact port */
#define SOCKWP		0			/* Write protect switch (PB5) */
#define SOCKINS		0			/* Card detect switch (PB4) */

#if (_MAX_SS != 512) || (_FS_READONLY == 0) || (STM32_SD_DISK_IOCTRL_FORCE == 1)
#define STM32_SD_DISK_IOCTRL   1
#else
#define STM32_SD_DISK_IOCTRL   0
#endif

/*--------------------------------------------------------------------------

   Module Private Functions and Variables

---------------------------------------------------------------------------*/

static const Uint32 socket_state_mask_cp = (1 << 0);
static const Uint32 socket_state_mask_wp = (1 << 1);

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

static volatile Uint32 Timer1, Timer2;	/* 100Hz decrement timers */

static
BYTE CardType;			/* Card type flags */

enum speed_setting { INTERFACE_SLOW, INTERFACE_FAST };

static void interface_speed( enum speed_setting speed )
{
	Uint32 tmp;

	tmp = SPI_SD->CR1;
	if ( speed == INTERFACE_SLOW ) {
		/* Set slow clock (100k-400k) */
		tmp = ( tmp | SPI_BaudRatePrescaler_256 );
	} else {
		/* Set fast clock (depends on the CSD) */
		tmp = ( tmp & ~SPI_BaudRatePrescaler_256 ) | SPI_BaudRatePrescaler_SPI_SD;
	}
	SPI_SD->CR1 = tmp;
}

#if SOCKET_WP_CONNECTED
	/* Socket's Write-Protection Pin: high = write-protected, low = writable */

	static void socket_wp_init(void)
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		/* Configure I/O for write-protect */
		RCC_APB2PeriphClockCmd(RCC_APBxPeriph_GPIO_WP, ENABLE);
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_WP;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_WP;
		GPIO_Init(GPIO_WP, &GPIO_InitStructure);
	}

	static Uint32 socket_is_write_protected(void)
	{
		return ( GPIO_ReadInputData(GPIO_WP) & GPIO_Pin_WP ) ? socket_state_mask_wp : 0;
	}
#else
	static inline Uint32 socket_is_write_protected(void)
	{
		return 0; /* fake not protected */
	}
#endif /* SOCKET_WP_CONNECTED */


#if SOCKET_CP_CONNECTED
	/* Socket's Card-Present Pin: high = socket empty, low = card inserted */

	static void socket_cp_init(void)
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		/* Configure I/O for card-present */
		RCC_APB2PeriphClockCmd(RCC_APBxPeriph_GPIO_CP, ENABLE);
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_CP;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_CP;
		GPIO_Init(GPIO_CP, &GPIO_InitStructure);
	}

	static inline Uint32 socket_is_empty(void)
	{
		return ( GPIO_ReadInputData(GPIO_CP) & GPIO_Pin_CP ) ? socket_state_mask_cp : FALSE;
	}

#else
	static inline Uint32 socket_is_empty(void)
	{
		return 0; /* fake inserted */
	}
#endif /* SOCKET_CP_CONNECTED */


#if CARD_SUPPLY_SWITCHABLE

#else



#if (STM32_SD_DISK_IOCTRL == 1)
static int chk_power(void)
{
	return 1; /* fake powered */
}
#endif

#endif /* CARD_SUPPLY_SWITCHABLE */


/*-----------------------------------------------------------------------*/
/* Transmit/Receive a byte to MMC via SPI  (Platform dependent)          */
/*-----------------------------------------------------------------------*/
static BYTE stm32_spi_rw( BYTE out )
{
	/* Loop while DR register in not empty */
	/// not needed: while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE) == RESET) { ; }

#ifdef STM32_SD_USE_DMA
	//On attent la fin d'une eventuelle transaction utilisant DMA
	while (DMA_GetCmdStatus(DMA_Stream_SPI_SD_RX) == ENABLE && DMA_GetFlagStatus(DMA_Stream_SPI_SD_RX, DMA_FLAG_SPI_SD_TC_RX) == RESET) { ; }
#endif

	/* Send byte through the SPI peripheral */
	SPI_I2S_SendData(SPI_SD, out);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI_SD);
}



/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

#define xmit_spi(dat)  stm32_spi_rw(dat)

/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static
BYTE rcvr_spi (void)
{
	return stm32_spi_rw(0xff);
}

/* Alternative macro to receive data fast */
#define rcvr_spi_m(dst)  *(dst)=stm32_spi_rw(0xff)



/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
BYTE wait_ready (void)
{
	BYTE res;

	Timer2 = 50;	/* Wait for ready in timeout of 500ms */
	rcvr_spi();
	do
		res = rcvr_spi();
	while ((res != 0xFF) && Timer2);

	return res;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void release_spi (void)
{
	DESELECT();
	rcvr_spi();
}

#ifdef STM32_SD_USE_DMA
/*-----------------------------------------------------------------------*/
/* Transmit/Receive Block using DMA (Platform dependent. STM32 here)     */
/*-----------------------------------------------------------------------*/

static void stm32_dma_init() {
	static Uint8 rw_workbyte; //DOIT ETRE STATIC pour que la variable reste en mémoire après avoir quitter la fonction (le DMA va l'utiliser)
	static bool_e initialized = FALSE;
	if(initialized)
		return;

	initialized = TRUE;

	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA_Stream_SPI_SD_RX);
	DMA_DeInit(DMA_Stream_SPI_SD_TX);

	/* shared DMA configuration values */
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (Uint32)(&(SPI_SD->DR));
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_BufferSize = 515; //512 sector size + 2 CRC (dummy read) + 1 response (read, ignored)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;


	// La lecture par DMA est considérée comme inutile ... car il faut attendre la fin de toute façon

#if _FS_READONLY == 0
	//discard  RX data
	DMA_InitStructure.DMA_Memory0BaseAddr = (Uint32)rw_workbyte;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_Init(DMA_Stream_SPI_SD_RX, &DMA_InitStructure);

	//send data from buffer (512 bytes)
	DMA_InitStructure.DMA_Memory0BaseAddr = (Uint32)dma_send_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Init(DMA_Stream_SPI_SD_TX, &DMA_InitStructure);
#endif

	dma_send_buffer[512] = 0xFF; //CRC LSB read
	dma_send_buffer[513] = 0xFF; //CRC MSB read
	dma_send_buffer[514] = 0xFF; //response read
}


static
void stm32_dma_transfer(
	BOOL receive,		/* FALSE for buff->SPI, TRUE for SPI->buff               */
	const BYTE *buff,	/* receive TRUE  : 512 byte data block to be transmitted
						   receive FALSE : Data buffer to store received data    */
	UINT btr 			/* receive TRUE  : Byte count (must be multiple of 2)
						   receive FALSE : Byte count (must be 512)              */
)
{

	//On ne veut faire du DMA que en transmission
	assert(receive != TRUE);

	assert(btr == 512);

	stm32_dma_init();

	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA_Stream_SendBuffer);

	//Buffer copy DMA, histoire d'aller encore plus vite ...
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (Uint32)(buff);
	DMA_InitStructure.DMA_Memory0BaseAddr = (Uint32)dma_send_buffer;

	//check buffer alignement
	switch((int)buff % 4) {
		case 0:
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
			break;

		case 1:
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
			break;

		case 2:
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
			break;

		case 3:
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
			break;
	}

	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_BufferSize = 512;  //sector size, last 3 bytes are set in stm32_dma_init()
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA_Stream_SendBuffer, &DMA_InitStructure);
	DMA_Cmd(DMA_Stream_SendBuffer, ENABLE);

	//On attend la fin de la copie. Une copie par CPU est plus lente (2 fois plus, mais c'est surement négligeable devant la vitesse de la carte SD (< 1ms pour 800 bytes: http://www.embedds.com/using-direct-memory-access-dma-in-stm23-projects/, mais fréquence inconnue)
	while (DMA_GetCmdStatus(DMA_Stream_SendBuffer) == ENABLE && DMA_GetFlagStatus(DMA_Stream_SendBuffer, DMA_FLAG_TC_SendBuffer) == RESET) { ; }
	DMA_ClearFlag(DMA_Stream_SendBuffer, DMA_FLAG_TC_SendBuffer);
//	DMA_Cmd(DMA_Stream_SendBuffer, DISABLE); //Utile ? Normalement cleared by hardware à la fin du transfert (page 230)

	//On attend que le DMA ait fini d'envoyer les données précédentes.
	while (DMA_GetCmdStatus(DMA_Stream_SPI_SD_RX) == ENABLE && DMA_GetFlagStatus(DMA_Stream_SPI_SD_RX, DMA_FLAG_SPI_SD_TC_RX) == RESET) { ; }

	DMA_ClearFlag(DMA_Stream_SPI_SD_RX, DMA_FLAG_SPI_SD_TC_RX);
	DMA_ClearFlag(DMA_Stream_SPI_SD_TX, DMA_FLAG_SPI_SD_TC_TX);

	/* Enable DMA RX Channel */
	DMA_Cmd(DMA_Stream_SPI_SD_RX, ENABLE);
	/* Enable DMA TX Channel */
	DMA_Cmd(DMA_Stream_SPI_SD_TX, ENABLE);

	/* Enable SPI TX/RX request */
	SPI_I2S_DMACmd(SPI_SD, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

//	/* Wait until DMA1_Channel 3 Transfer Complete */
//	/// not needed:
//	/* Wait until DMA1_Channel 2 Receive Complete */
//	while (DMA_GetFlagStatus(DMA_Stream_SPI_SD_RX, DMA_FLAG_SPI_SD_TC_RX) == RESET) { ; }
//	// same w/o function-call:
//	// while ( ( ( DMA1->ISR ) & DMA_FLAG_SPI_SD_TC_RX ) == RESET ) { ; }

//	/* Disable DMA RX Channel */
//	DMA_Cmd(DMA_Stream_SPI_SD_RX, DISABLE);
//	/* Disable DMA TX Channel */
//	DMA_Cmd(DMA_Stream_SPI_SD_TX, DISABLE);

//	/* Disable SPI RX/TX request */
//	SPI_I2S_DMACmd(SPI_SD, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}
#endif /* STM32_SD_USE_DMA */


/*-----------------------------------------------------------------------*/
/* Power Control and interface-initialization (Platform dependent)       */
/*-----------------------------------------------------------------------*/
#include "../../../../QS/QS_spi.h"
#include "../../../../QS/QS_ports.h"



static
void power_on (void)
{

	//PORTS_spi_init();
	DESELECT();	//CS high.

	/*
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	volatile BYTE dummyread;
	// Enable GPIO clock for CS
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CS, ENABLE);
	// Enable SPI clock, SPI1: APB2, SPI2: APB1
	RCC_APBPeriphClockCmd_SPI_SD(RCC_APBPeriph_SPI_SD, ENABLE);

	card_power(1);
	socket_cp_init();
	socket_wp_init();

	for (Timer1 = 25; Timer1; );	// Wait for 250ms

	// Configure I/O for Flash Chip select
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_CS;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_CS, &GPIO_InitStructure);

	// De-select the Card: Chip Select high
	DESELECT();

	// Configure SPI pins: SCK and MOSI with default alternate function (not re-mapped) push-pull
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);
	// Configure MISO as Input with internal pull-up
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_MISO;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);


	// drain SPI
	while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE) == RESET) { ; }
	dummyread = SPI_I2S_ReceiveData(SPI_SD);
*/
#ifdef STM32_SD_USE_DMA
	/* enable DMA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
#endif
}

static
void power_off (void)
{
	/*GPIO_InitTypeDef GPIO_InitStructure;

	if (!(Stat & STA_NOINIT)) {
		SELECT();
		wait_ready();
		release_spi();
	}

	SPI_I2S_DeInit(SPI_SD);
	SPI_Cmd(SPI_SD, DISABLE);
	RCC_APBPeriphClockCmd_SPI_SD(RCC_APBPeriph_SPI_SD, DISABLE);

	// All SPI-Pins to input with weak internal pull-downs
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MISO | GPIO_Pin_SPI_SD_MOSI;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);

	card_power(0);
*/
	Stat |= STA_NOINIT;		/* Set STA_NOINIT */
}


/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
BOOL rcvr_datablock (
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
)
{
	BYTE token;


	Timer1 = 10;
	do {							/* Wait for data packet in timeout of 100ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && Timer1);
	if(token != 0xFE) return FALSE;	/* If not valid data token, return with error */

	//Jamais de transfert DMA en lecture, on veut le buffer maintenant
//#ifdef STM32_SD_USE_DMA
//	stm32_dma_transfer( TRUE, buff, btr );
//#else
	do {							/* Receive the data block into buffer */
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
	} while (btr -= 4);
//#endif /* STM32_SD_USE_DMA */

	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	return TRUE;					/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _FS_READONLY == 0
static
BOOL xmit_datablock (
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data/Stop token */
)
{
#ifndef STM32_SD_USE_DMA
	BYTE resp;
	BYTE wc;
#endif

	if (wait_ready() != 0xFF) return FALSE;

	xmit_spi(token);					/* transmit data token */
	if (token != 0xFD) {	/* Is data token */

#ifdef STM32_SD_USE_DMA
		stm32_dma_transfer( FALSE, buff, 512 );
		//Gestion checksum + response par DMA. Donc on refuse jamais ici (dangereux ?)
#else
		wc = 0;
		do {							/* transmit the 512 byte data block to MMC */
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (--wc);

		xmit_spi(0xFF);					/* CRC (Dummy) */
		xmit_spi(0xFF);
		resp = rcvr_spi();				/* Receive data response */
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return FALSE;
#endif /* STM32_SD_USE_DMA */
	}

	return TRUE;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	Uint32 arg		/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMDAPP_CMD, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	DESELECT();
	SELECT();
	if (wait_ready() != 0xFF) {
		return 0xFF;
	}

	/* Send command packet */
	xmit_spi(cmd);						/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD_GO_IDLE_STATE) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD_SEND_IF_COND) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD_STOP_TRANSMISSION) rcvr_spi();		/* Skip a stuff byte when stop reading */

	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{
	BYTE n, cmd, ty, ocr[4];

	if (drv) return STA_NOINIT;			/* Supports only single drive */
	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

	power_on();							/* Force socket power on and initialize interface */
	interface_speed(INTERFACE_SLOW);
	for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD_GO_IDLE_STATE, 0) == 1) {			/* Enter Idle state */
		Timer1 = 100;						/* Initialization timeout of 1000 milliseconds */
		if (send_cmd(CMD_SEND_IF_COND, 0x1AA) == 1) {	/* SDHC */
			for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();		/* Get trailing return value of R7 response */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at VDD range of 2.7-3.6V */
				while (Timer1 && send_cmd(ACMD_SEND_OP_COND_SDC, 1UL << 30));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (Timer1 && send_cmd(CMD_READ_OCR, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}
		} else {							/* SDSC or MMC */
			if (send_cmd(ACMD_SEND_OP_COND_SDC, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD_SEND_OP_COND_SDC;	/* SDSC */
			} else {
				ty = CT_MMC; cmd = CMD_SEND_OP_COND_MMC;	/* MMC */
			}
			while (Timer1 && send_cmd(cmd, 0));			/* Wait for leaving idle state */
			if (!Timer1 || send_cmd(CMD_SET_BLOCKLEN, 512) != 0)	/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	release_spi();

	if (ty) {			/* Initialization succeeded */
		Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
		interface_speed(INTERFACE_FAST);
	} else {			/* Initialization failed */
		power_off();
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block read */
		if (send_cmd(CMD_READ_SINGLE_BLOCK, sector) == 0)	{ /* READ_SINGLE_BLOCK */
			if (rcvr_datablock(buff, 512)) {
				count = 0;
			}
		}
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD_READ_MULTIPLE_BLOCK, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) {
					break;
				}
				buff += 512;
			} while (--count);
			send_cmd(CMD_STOP_TRANSMISSION, 0);				/* STOP_TRANSMISSION */
		}
	}
	release_spi();

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _FS_READONLY == 0

DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD_WRITE_BLOCK, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD_SET_WR_BLK_ERASE_COUNT_SDC, count);
		if (send_cmd(CMD_WRITE_MULTIPLE_BLOCK, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	release_spi();

	return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY == 0 */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if (STM32_SD_DISK_IOCTRL == 1)
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	WORD csize;

	if (drv) return RES_PARERR;

	res = RES_ERROR;

	if (ctrl == CTRL_POWER) {
		switch (*ptr) {
		case 0:		/* Sub control code == 0 (POWER_OFF) */
			if (chk_power())
				power_off();		/* Power off */
			res = RES_OK;
			break;
		case 1:		/* Sub control code == 1 (POWER_ON) */
			power_on();				/* Power on */
			res = RES_OK;
			break;
		case 2:		/* Sub control code == 2 (POWER_GET) */
			*(ptr+1) = (BYTE)chk_power();
			res = RES_OK;
			break;
		default :
			res = RES_PARERR;
		}
	}
	else {
		if (Stat & STA_NOINIT) return RES_NOTRDY;

		switch (ctrl) {
		case CTRL_SYNC :		/* Make sure that no pending write process */
			SELECT();
			if (wait_ready() == 0xFF)
				res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			if ((send_cmd(CMD_SEND_CSD, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDC version 2.00 */
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(Uint32*)buff = (Uint32)csize << 10;
				} else {					/* SDC version 1.XX or MMC*/
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(Uint32*)buff = (Uint32)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			if (CardType & CT_SD2) {	/* SDC version 2.00 */
				if (send_cmd(ACMD_SD_STATUS_SDC, 0) == 0) {	/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16)) {				/* Read partial block */
						for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
						*(Uint32*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {					/* SDC version 1.XX or MMC */
				if ((send_cmd(CMD_SEND_CSD, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
					if (CardType & CT_SD1) {	/* SDC version 1.XX */
						*(Uint32*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {					/* MMC */
						*(Uint32*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
			if (send_cmd(CMD_SEND_CSD, 0) == 0		/* READ_CSD */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
			if (send_cmd(CMD_SEND_CID, 0) == 0		/* READ_CID */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD_READ_OCR, 0) == 0) {	/* READ_OCR */
				for (n = 4; n; n--) *ptr++ = rcvr_spi();
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
			if (send_cmd(ACMD_SD_STATUS_SDC, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(ptr, 64))
					res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
		}

		release_spi();
	}

	return res;
}
#endif /* _USE_IOCTL != 0 */


/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void disk_timerproc (void)
{
	static Uint32 pv;
	Uint32 ns;
	Uint32 n;
	DSTATUS s;


	n = Timer1;                /* 100Hz decrement timers */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;

	ns = pv;
	pv = socket_is_empty() | socket_is_write_protected();	/* Sample socket switch */

	if (ns == pv) {                         /* Have contacts stabled? */
		s = Stat;

		if (pv & socket_state_mask_wp)      /* WP is H (write protected) */
			s |= STA_PROTECT;
		else                                /* WP is L (write enabled) */
			s &= ~STA_PROTECT;

		if (pv & socket_state_mask_cp)      /* INS = H (Socket empty) */
			s |= (STA_NODISK | STA_NOINIT);
		else                                /* INS = L (Card inserted) */
			s &= ~STA_NODISK;

		Stat = s;
	}
}

