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

#include "QS_spi.h"
#include "QS_ports.h"
#include "QS_outputlog.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"

#define SPI1_SPI_HANDLE SPI1
#define SPI1_SPI_CLOCK  RCC_APB2Periph_SPI1

#define SPI2_SPI_HANDLE SPI2
#define SPI2_SPI_CLOCK  RCC_APB1Periph_SPI2

#if defined(USE_SPI1) || defined(USE_SPI2)

#define SPI2_DMA_STREAM	DMA1_Stream4
#define SPI2_DMA_CHAN	DMA_Channel_0

#ifdef USE_SPI2
	static Uint8 SPI2_InitDMA_send16BitLoop(Uint16 value, Uint16 count);
	static Uint8 SPI2_InitDMA_send16bitArray(Uint16 *data, Uint16 count);
	static void SPI2_waitEndDMA(void);
#endif

static volatile SPI_InitTypeDef SPI_InitStructure[2];
static volatile bool_e initialized = FALSE;

void SPI_init(void)
{
	if(initialized)
		return;
	initialized = TRUE;

	PORTS_spi_init();

	SPI_InitStructure[0].SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure[0].SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure[0].SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure[0].SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure[0].SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure[0].SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure[0].SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure[0].SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure[0].SPI_CRCPolynomial = 7;

	SPI_InitStructure[1].SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure[1].SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure[1].SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure[1].SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure[1].SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure[1].SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure[1].SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure[1].SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure[1].SPI_CRCPolynomial = 7;

	#ifdef SPI2_ON_DMA
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	#endif

	#ifdef USE_SPI1
		RCC_APB2PeriphClockCmd(SPI1_SPI_CLOCK, ENABLE);
		SPI_Init(SPI1_SPI_HANDLE, (SPI_InitTypeDef*)&(SPI_InitStructure[0]));
		SPI_Cmd(SPI1_SPI_HANDLE, ENABLE);
	#endif /* def USE_SPI1 */

	#ifdef USE_SPI2
		RCC_APB1PeriphClockCmd(SPI2_SPI_CLOCK, ENABLE);
		SPI_Init(SPI2_SPI_HANDLE, (SPI_InitTypeDef*)&(SPI_InitStructure[1]));
		SPI_Cmd(SPI2_SPI_HANDLE, ENABLE);
	#endif /* def USE_SPI2 */

}

void SPI_setBaudRate(SPI_TypeDef* SPIx, uint16_t SPI_BaudRatePrescaler){
	SPI_Cmd(SPIx, DISABLE);
	SPI_DeInit(SPIx);

	switch((Uint32)SPIx){
		case (Uint32)SPI1:
			SPI_InitStructure[0].SPI_BaudRatePrescaler = SPI_BaudRatePrescaler;
			SPI_Init(SPIx, (SPI_InitTypeDef*)&(SPI_InitStructure[0]));
			break;

		case (Uint32)SPI2:
			SPI_InitStructure[1].SPI_BaudRatePrescaler = SPI_BaudRatePrescaler;
			SPI_Init(SPIx, (SPI_InitTypeDef*)&(SPI_InitStructure[1]));
			break;

		default :
			error_printf("SPI_setBaudRate SPIx valeur impossible\n");
			break;
	}
	SPI_Cmd(SPIx, ENABLE);
}

void SPI_setDataSize(SPI_TypeDef* SPIx, spi_data_size_e spi_data_size){
	assert(IS_SPI_ALL_PERIPH(SPIx));

	switch(spi_data_size){
		case SPI_DATA_SIZE_8_BIT:
			SPI_DataSizeConfig(SPIx, SPI_DataSize_8b);
			break;

		case SPI_DATA_SIZE_16_BIT:
			SPI_DataSizeConfig(SPIx, SPI_DataSize_16b);
			break;

		default:
			error_printf("SPI_setDataSize argument erroné : %d\n", spi_data_size);
			break;
	}
}

Uint16 SPI_exchange(SPI_TypeDef* SPIx, Uint16 c){
	assert(IS_SPI_ALL_PERIPH(SPIx));
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return 0;
	}

	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

	if(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == SET)
	{
		debug_printf("SPI : buffer non vide \n");
		SPI_I2S_ReceiveData(SPIx);
	}

	SPI_I2S_SendData(SPIx, c);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);


	//Test si erreur
	if(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_OVR) || SPI_I2S_GetFlagStatus(SPIx, SPI_FLAG_MODF))
	{
		debug_printf("SPI : collision\n");
		SPI_I2S_ClearFlag(SPIx, SPI_I2S_FLAG_OVR);
		SPI_I2S_ClearFlag(SPIx, SPI_FLAG_MODF);
	}

	return SPI_I2S_ReceiveData(SPIx);
}

void SPI_write(SPI_TypeDef* SPIx, Uint16 msg){
	assert(IS_SPI_ALL_PERIPH(SPIx));

	SPI_exchange(SPIx, msg);
}

Uint16 SPI_read(SPI_TypeDef* SPIx){
	assert(IS_SPI_ALL_PERIPH(SPIx));

	return SPI_exchange(SPIx, 0x00);
}


#endif  /* defined(USE_SPI1) || defined(USE_SPI2) */

#ifdef USE_SPI1

Uint8 SPI1_exchange(Uint8 c)
{
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return 0;
	}

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
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return;
	}

	SPI1_exchange(msg);
}

Uint8 SPI1_read()
{
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return 0;
	}

	return SPI1_exchange(0x00);
}

#endif

#ifdef USE_SPI2

Uint16 SPI2_exchange(Uint16 c)
{
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return 0;
	}

	while(SPI_I2S_GetFlagStatus(SPI2_SPI_HANDLE, SPI_I2S_FLAG_TXE) == RESET);

	if(SPI_I2S_GetFlagStatus(SPI2_SPI_HANDLE, SPI_I2S_FLAG_RXNE) == SET)
	{
		//debug_printf("SPI2: buffer non vide \n");
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

void SPI2_write(Uint16 msg)
{
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return;
	}

	SPI2_exchange(msg);
}

Uint16 SPI2_read()
{
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return 0;
	}

	return  SPI2_exchange(0x00);
}

void SPI2_DMA_send16BitLoop(Uint16 data, Uint32 count){
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return;
	}
	Uint32 index = 0;

	count++;	// Correction condition de fin DMA

	do{
		SPI2_InitDMA_send16BitLoop(data, (count - index > 0xFFFF) ? 0xFFFF : count - index);
		SPI2_waitEndDMA();
		index += 0xFFFF;
	}while(count > index);
}

void SPI2_DMA_send16BitArray(Uint16 *data, Uint32 count){
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return;
	}
	Uint32 index = 0;

	count++;	// Correction condition de fin DMA

	do{
		SPI2_InitDMA_send16bitArray(data + index, (count - index > 0xFFFF) ? 0xFFFF : count - index);
		SPI2_waitEndDMA();
		index += 0xFFFF;
	}while(count > index);
}

static void SPI2_waitEndDMA(void){
	while(SPI2_DMA_STREAM->NDTR);
	DMA_DeInit(SPI2_DMA_STREAM);
}

static Uint8 SPI2_InitDMA_send16BitLoop(Uint16 value, Uint16 count){

	static Uint16 dummy;

	dummy = value;

	DMA_InitTypeDef DMA_InitStructure;

	DMA_StructInit(&DMA_InitStructure);

	DMA_InitStructure.DMA_Channel = SPI2_DMA_CHAN;

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_InitStructure.DMA_Memory0BaseAddr = (Uint32)&dummy;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;

	DMA_InitStructure.DMA_BufferSize = count;

	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;

	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;

	/* Deinit first TX stream */
	DMA_ClearFlag(SPI2_DMA_STREAM, DMA_FLAG_TCIF0 | DMA_FLAG_HTIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_FEIF0);

	/* Init TX stream */
	DMA_Init(SPI2_DMA_STREAM, &DMA_InitStructure);

	/* Enable TX stream */
	SPI2_DMA_STREAM->CR |= DMA_SxCR_EN;

	/* Enable SPI TX DMA */
	SPI2->CR2 |= SPI_CR2_TXDMAEN;

	/* Return OK */
	return 1;
}

static Uint8 SPI2_InitDMA_send16bitArray(Uint16 *data, Uint16 count){

	DMA_InitTypeDef DMA_InitStructure;

	DMA_StructInit(&DMA_InitStructure);

	DMA_InitStructure.DMA_Channel = SPI2_DMA_CHAN;

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_InitStructure.DMA_Memory0BaseAddr = (Uint32)data;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;

	DMA_InitStructure.DMA_BufferSize = count;

	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;

	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;

	/* Deinit first TX stream */
	DMA_ClearFlag(SPI2_DMA_STREAM, DMA_FLAG_TCIF0 | DMA_FLAG_HTIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_FEIF0);

	/* Init TX stream */
	DMA_Init(SPI2_DMA_STREAM, &DMA_InitStructure);

	/* Enable TX stream */
	SPI2_DMA_STREAM->CR |= DMA_SxCR_EN;

	/* Enable SPI TX DMA */
	SPI2->CR2 |= SPI_CR2_TXDMAEN;

	/* Return OK */
	return 1;
}

#endif /* defined(USE_SPI2) */

