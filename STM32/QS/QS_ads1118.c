/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : QS_ads1118.c
 *  Package : Qualité Soft
 *  Description : Gestion de l'ADC externe
 *  Auteur : Arnaud
 */

#include "QS_ads1118.h"
#include "QS_spi.h"

#ifdef USE_ADS_1118

	typedef union{
		Uint16 raw;
		struct{

			ADS1118_CONFIG_singleShotStart singleShotStart				:1;
			ADS1118_CONFIG_inputMultiplexer inputMultiplexer			:3;
			ADS1118_CONFIG_gainAmplifier gainAmplifier					:3;
			ADS1118_CONFIG_operatingMode operatingMode					:1;
			ADS1118_CONFIG_dataRate dataRate							:3;
			ADS1118_CONFIG_temperatureSensorMode temperatureSensorMode	:1;
			ADS1118_CONFIG_pullup pullup								:1;

			enum{
				ADS1118_CONFIG_SECURE_WRITE		= 0b01				// Toujours mettre le mode SECURE_WRITE sinon l'écriture du registre ne se fera pas
			}secureWrite						:2;					// [RW] | reset -> 0b01

			enum{
				ADS1118_CONFIG_RESERVED			= 1					// Toujours mettre la valeur 1 dans le registre réservé
			}reservedReg						:1;					// [R] | reset -> 0b1
		};

	}ADS1118_config_reg_s;

	void ADS1118_init(void){
		SPI_init();
	}

	bool_e ADS1118_setConfig(ADS1118_sensorConfig_t * sensorConfig,
									ADS1118_CONFIG_singleShotStart singleShotStart,
									ADS1118_CONFIG_inputMultiplexer inputMultiplexer,
									ADS1118_CONFIG_gainAmplifier gainAmplifier,
									ADS1118_CONFIG_operatingMode operatingMode,
									ADS1118_CONFIG_dataRate dataRate,
									ADS1118_CONFIG_temperatureSensorMode temperatureSensorMode,
									ADS1118_CONFIG_pullup pullup){

		assert(sensorConfig != NULL);

		ADS1118_config_reg_s configReg;

		configReg.singleShotStart = singleShotStart;
		configReg.inputMultiplexer = inputMultiplexer;
		configReg.gainAmplifier = gainAmplifier;
		configReg.operatingMode = operatingMode;
		configReg.dataRate = dataRate;
		configReg.temperatureSensorMode = temperatureSensorMode;
		configReg.pullup;

		GPIO_SetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);

		if(ADS1118_isAvailable(sensorConfig))
			return FALSE;

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_16_BIT);

		SPI_read(sensorConfig->SPI_handle);

		SPI_write(sensorConfig->SPI_handle, configReg.raw);

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_8_BIT);

		GPIO_ResetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);

		return TRUE;
	}

	bool_e ADS1118_getConfig(ADS1118_sensorConfig_t * sensorConfig,
							ADS1118_CONFIG_singleShotStart * singleShotStart,
							ADS1118_CONFIG_inputMultiplexer * inputMultiplexer,
							ADS1118_CONFIG_gainAmplifier * gainAmplifier,
							ADS1118_CONFIG_operatingMode * operatingMode,
							ADS1118_CONFIG_dataRate * dataRate,
							ADS1118_CONFIG_temperatureSensorMode * temperatureSensorMode,
							ADS1118_CONFIG_pullup * pullup){

		assert(sensorConfig != NULL);

		ADS1118_config_reg_s configReg;

		GPIO_SetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);

		if(ADS1118_isAvailable(sensorConfig))
			return FALSE;

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_16_BIT);

		SPI_read(sensorConfig->SPI_handle);

		configReg.raw = SPI_read(sensorConfig->SPI_handle);

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_8_BIT);

		GPIO_ResetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);

		if(singleShotStart != NULL)
			*singleShotStart = configReg.singleShotStart;

		if(inputMultiplexer != NULL)
			*inputMultiplexer = configReg.inputMultiplexer;

		if(gainAmplifier != NULL)
			*gainAmplifier = configReg.gainAmplifier;

		if(operatingMode != NULL)
			*operatingMode = configReg.operatingMode;

		if(dataRate != NULL)
			*dataRate = configReg.dataRate;

		if(temperatureSensorMode != NULL)
			*temperatureSensorMode = configReg.temperatureSensorMode;

		if(pullup != NULL)
			*pullup = configReg.pullup;

		return TRUE;
	}

	bool_e ADS1118_getValue(ADS1118_sensorConfig_t * sensorConfig, Uint16 * value){
		assert(sensorConfig != NULL);
		assert(value != NULL);

		GPIO_SetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);

		if(ADS1118_isAvailable(sensorConfig))
			return FALSE;

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_16_BIT);

		*value = SPI_read(sensorConfig->SPI_handle);

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_8_BIT);

		GPIO_ResetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);

		return TRUE;
	}

	bool_e ADS1118_isAvailable(ADS1118_sensorConfig_t * sensorConfig){
		assert(sensorConfig != NULL);
		if(GPIO_ReadInputDataBit(sensorConfig->GPIO_din.GPIOx, sensorConfig->GPIO_din.GPIO_Pin))
			return FALSE;
		else
			return TRUE;
	}

#endif
