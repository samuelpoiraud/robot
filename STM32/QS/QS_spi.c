/*
 *	Club Robot ESEO 2010 - 2011
 *	???
 *
 *	Fichier : QS_spi.c
 *	Package : Qualité Soft
 *	Description : Fonction SPI
 *	Auteur : Ronan
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100929
 */
 
#define QS_SPI_C
#include "QS_spi.h"
#include "QS_ports.h"
#include "stm32f4xx_spi.h"

#define SPI1_SPI_HANDLE SPI1
#define SPI1_SPI_CLOCK  RCC_APB1Periph_SPI1

#define SPI2_SPI_HANDLE SPI2
#define SPI2_SPI_CLOCK  RCC_APB1Periph_SPI2

#if defined(USE_SPI1) || defined(USE_SPI2)

void SPI_init(void) 
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	SPI_InitTypeDef SPI_InitStructure;

	PORTS_spi_init();

	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	#ifdef USE_SPI1
		RCC_APB1PeriphClockCmd(SPI1_SPI_CLOCK, ENABLE);
		SPI_Init(SPI1_SPI_HANDLE, &SPI_InitStructure);
		SPI_Cmd(SPI1_SPI_HANDLE, ENABLE);
	#endif /* def USE_SPI1 */
	
	#ifdef USE_SPI2
		RCC_APB1PeriphClockCmd(SPI2_SPI_CLOCK, ENABLE);
		SPI_Init(SPI2_SPI_HANDLE, &SPI_InitStructure);
		SPI_Cmd(SPI2_SPI_HANDLE, ENABLE);
	#endif /* def USE_SPI2 */

}

#endif  /* defined(USE_SPI1) || defined(USE_SPI2) */

#ifdef USE_SPI1

Uint8 SPI1_exchange(Uint8 c)
{
	while(SPI_I2S_GetFlagStatus(SPI1_SPI_HANDLE, SPI_I2S_FLAG_TXE) == RESET);

	if(SPI_I2S_GetFlagStatus(SPI1_SPI_HANDLE, SPI_I2S_FLAG_RXNE) == SET)
	{
		debug_printf("SPI1: buffer non vide \n");
		SPI_I2S_ReceiveData(SPI1_SPI_HANDLE);
	}

	SPI_I2S_SendData(SPI1_SPI_HANDLE, c);
	while(SPI_I2S_GetFlagStatus(SPI1_SPI_HANDLE, SPI_I2S_FLAG_RXNE) == RESET);


	//Test si erreur
	if(SPI_I2S_GetFlagStatus(SPI1_SPI_HANDLE, SPI_I2S_FLAG_OVR) || SPI_I2S_GetFlagStatus(SPI1_SPI_HANDLE, SPI_FLAG_MODF))
	{
		debug_printf("SPI1: collision\n");
		SPI_I2S_ClearFlag(SPI1_SPI_HANDLE, SPI_I2S_FLAG_OVR);
		SPI_I2S_ClearFlag(SPI1_SPI_HANDLE, SPI_FLAG_MODF);
	}

	return SPI_I2S_ReceiveData(SPI1_SPI_HANDLE);
}

void SPI1_write(Uint8 msg)
{
	SPI1_exchange(msg);
}

Uint8 SPI1_read()
{
	return SPI1_exchange(0x00);
}

#endif

#ifdef USE_SPI2

Uint8 SPI2_exchange(Uint8 c)
{
	while(SPI_I2S_GetFlagStatus(SPI2_SPI_HANDLE, SPI_I2S_FLAG_TXE) == RESET);

	if(SPI_I2S_GetFlagStatus(SPI2_SPI_HANDLE, SPI_I2S_FLAG_RXNE) == SET)
	{
		debug_printf("SPI2: buffer non vide \n");
		SPI_I2S_ReceiveData(SPI2_SPI_HANDLE);
	}

	SPI_I2S_SendData(SPI2_SPI_HANDLE, c);
	while(SPI_I2S_GetFlagStatus(SPI2_SPI_HANDLE, SPI_I2S_FLAG_RXNE) == RESET);
	

	//Test si erreur
	if(SPI_I2S_GetFlagStatus(SPI2_SPI_HANDLE, SPI_I2S_FLAG_OVR) || SPI_I2S_GetFlagStatus(SPI2_SPI_HANDLE, SPI_FLAG_MODF))
	{
		debug_printf("SPI2: collision\n");
		SPI_I2S_ClearFlag(SPI2_SPI_HANDLE, SPI_I2S_FLAG_OVR);
		SPI_I2S_ClearFlag(SPI2_SPI_HANDLE, SPI_FLAG_MODF);
	}

	return SPI_I2S_ReceiveData(SPI2_SPI_HANDLE);
}

void SPI2_write(Uint8 msg) 
{
	SPI2_exchange(msg);
}

Uint8 SPI2_read() 
{
	return SPI2_exchange(0x00);
}

#endif /* defined(USE_SPI2) */

