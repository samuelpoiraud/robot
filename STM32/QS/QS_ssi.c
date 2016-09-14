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

#include "QS_ssi.h"
#include "QS_ports.h"
#include "QS_outputlog.h"
#include "stm32f4xx_spi.h"

#ifdef USE_SSI

	#ifdef USE_SPI2
		#error "On ne peut pas utiliser SPI2 et SSI en même temps"
	#endif

	#ifndef SSI_NB_BIT
		#error "Veuillez configurer le nombre de bit que vous voulez lire en SSI"
	#endif

	bool_e initialized = FALSE;

	void SSI_init(void){
		if(initialized)
			return;
		initialized = TRUE;

		PORTS_ssi_init();

		SPI_InitTypeDef SPI_InitStructure;

		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
		SPI_InitStructure.SPI_CRCPolynomial = 7;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		SPI_Init(SPI2, &SPI_InitStructure);
		SPI_Cmd(SPI2, ENABLE);
	}

	Uint64 SSI_read(){
		if(!initialized){
			error_printf("SSI non initialisé ! Appeller SSI_init\n");
			return 0;
		}

		Uint64 result = 0;
		Uint8 i;
		bool_e first_time = TRUE;
		for(i=0; i<AROUND_UP((SSI_NB_BIT+1)/16.); i++){

			result <<= 16;

			while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

			if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == SET)
			{
				debug_printf("SPI2: buffer non vide \n");
				SPI_I2S_ReceiveData(SPI2);
			}

			SPI_I2S_SendData(SPI2, 0x0000);
			while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);


			//Test si erreur
			if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_OVR) || SPI_I2S_GetFlagStatus(SPI2, SPI_FLAG_MODF))
			{
				debug_printf("SPI2: collision\n");
				SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_OVR);
				SPI_I2S_ClearFlag(SPI2, SPI_FLAG_MODF);
			}

			result |= SPI_I2S_ReceiveData(SPI2);

			if(first_time){
				result &= 0x7FFF;
				first_time = FALSE;
			}
		}

		result >>= 16 - (SSI_NB_BIT+1)%16;
		return result;
	}

#endif
