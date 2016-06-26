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
#include "QS_outputlog.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_spi.h"

#define SPI1_SPI_HANDLE SPI1
#define SPI1_SPI_CLOCK  RCC_APB1Periph_SPI1

#define SPI2_SPI_HANDLE SPI2
#define SPI2_SPI_CLOCK  RCC_APB1Periph_SPI2

#if defined(USE_SPI1) || defined(USE_SPI2)

static bool_e initialized = FALSE;
static SPI_HandleTypeDef SPI1_HandleStructure;
static SPI_HandleTypeDef SPI2_HandleStructure;

void SPI_init(void)
{
	if(initialized)
		return;
	initialized = TRUE;

	PORTS_spi_init();

	SPI1_HandleStructure.Instance = SPI1;
	SPI1_HandleStructure.Init.Mode = SPI_MODE_MASTER;
	SPI1_HandleStructure.Init.Direction = SPI_DIRECTION_2LINES;
	SPI1_HandleStructure.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI1_HandleStructure.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI1_HandleStructure.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI1_HandleStructure.Init.CLKPhase = SPI_PHASE_1EDGE;
	SPI1_HandleStructure.Init.NSS = SPI_NSS_SOFT;
	SPI1_HandleStructure.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	SPI1_HandleStructure.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI1_HandleStructure.Init.CRCCalculation = SPI_CRCCALCULATION_ENABLE;
	SPI1_HandleStructure.Init.CRCPolynomial = 7;

	//Copie de l'init dans SPI2
	SPI2_HandleStructure.Instance = SPI2;
	SPI2_HandleStructure.Init = SPI1_HandleStructure.Init;

	#ifdef USE_SPI1
		__HAL_RCC_SPI1_CLK_ENABLE();
		__HAL_SPI_DISABLE(&SPI1_HandleStructure);
		HAL_SPI_Init(&SPI1_HandleStructure);
		__HAL_SPI_ENABLE(&SPI1_HandleStructure);
	#endif /* def USE_SPI1 */

	#ifdef USE_SPI2
		__HAL_RCC_SPI2_CLK_ENABLE();
		__HAL_SPI_DISABLE(&SPI2_HandleStructure);
		HAL_SPI_Init(&SPI2_HandleStructure);
		__HAL_SPI_ENABLE(&SPI2_HandleStructure);
	#endif /* def USE_SPI2 */

}

#endif  /* defined(USE_SPI1) || defined(USE_SPI2) */

//#ifdef USE_SPI1

Uint8 SPI1_exchange(Uint8 c)
{
	Uint8 receiveChar;
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return 0;
	}

	while(__HAL_SPI_GET_FLAG(&SPI1_HandleStructure, SPI_FLAG_TXE) == RESET);

	if(__HAL_SPI_GET_FLAG(&SPI1_HandleStructure, SPI_FLAG_RXNE) == SET)
	{
		debug_printf("SPI1: buffer non vide \n");
		HAL_SPI_Receive(&SPI1_HandleStructure, &receiveChar, 1, 10);
	}

	HAL_SPI_Transmit(&SPI1_HandleStructure, &c, 1, 10);
	while(__HAL_SPI_GET_FLAG(&SPI1_HandleStructure, SPI_FLAG_RXNE) == RESET);


	//Test si erreur
	if(__HAL_SPI_GET_FLAG(&SPI1_HandleStructure, SPI_FLAG_OVR) || __HAL_SPI_GET_FLAG(&SPI1_HandleStructure, SPI_FLAG_MODF))
	{
		debug_printf("SPI1: collision\n");
		__HAL_SPI_CLEAR_OVRFLAG(&SPI1_HandleStructure);
		__HAL_SPI_CLEAR_MODFFLAG(&SPI1_HandleStructure);
	}

	HAL_SPI_Receive(&SPI1_HandleStructure, &receiveChar, 1, 10);
	return receiveChar;
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

Uint8 SPI2_exchange(Uint8 c)
{
	Uint8 receiveChar;
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return 0;
	}

	while(__HAL_SPI_GET_FLAG(&SPI2_HandleStructure, SPI_FLAG_TXE) == RESET);

	if(__HAL_SPI_GET_FLAG(&SPI2_HandleStructure, SPI_FLAG_RXNE) == SET)
	{
		debug_printf("SPI1: buffer non vide \n");
		HAL_SPI_Receive(&SPI2_HandleStructure, &receiveChar, 1, 10);
	}

	HAL_SPI_Transmit(&SPI2_HandleStructure, &c, 1, 10);
	while(__HAL_SPI_GET_FLAG(&SPI2_HandleStructure, SPI_FLAG_RXNE) == RESET);


	//Test si erreur
	if(__HAL_SPI_GET_FLAG(&SPI2_HandleStructure, SPI_FLAG_OVR) || __HAL_SPI_GET_FLAG(&SPI2_HandleStructure, SPI_FLAG_MODF))
	{
		debug_printf("SPI1: collision\n");
		__HAL_SPI_CLEAR_OVRFLAG(&SPI2_HandleStructure);
		__HAL_SPI_CLEAR_MODFFLAG(&SPI2_HandleStructure);
	}

	HAL_SPI_Receive(&SPI2_HandleStructure, &receiveChar, 1, 10);
	return receiveChar;
}

void SPI2_write(Uint8 msg)
{
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return;
	}

	SPI2_exchange(msg);
}

Uint8 SPI2_read()
{
	if(!initialized){
		error_printf("SPI non initialisé ! Appeller SPI_init\n");
		return 0;
	}

	return  SPI2_exchange(0x00);
}

#endif /* defined(USE_SPI2) */

