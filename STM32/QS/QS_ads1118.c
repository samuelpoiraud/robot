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
#include "QS_outputlog.h"

#ifdef USE_ADS_1118

	typedef union{
		Uint16 raw;
		struct{

			enum{
				ADS1118_CONFIG_RESERVED			= 1					// Toujours mettre la valeur 1 dans le registre réservé
			}reservedReg						:1;					// [R] | reset -> 0b1

			enum{
				ADS1118_CONFIG_SECURE_WRITE		= 0b01				// Toujours mettre le mode SECURE_WRITE sinon l'écriture du registre ne se fera pas
			}secureWrite						:2;					// [RW] | reset -> 0b01

			ADS1118_CONFIG_pullup pullup								:1;
			ADS1118_CONFIG_temperatureSensorMode temperatureSensorMode	:1;
			ADS1118_CONFIG_dataRate dataRate							:3;
			ADS1118_CONFIG_operatingMode operatingMode					:1;
			ADS1118_CONFIG_gainAmplifier gainAmplifier					:3;
			ADS1118_CONFIG_inputMultiplexer inputMultiplexer			:3;
			ADS1118_CONFIG_singleShotStart singleShotStart				:1;
		};

	}ADS1118_config_reg_s;

	void ADS1118_init(ADS1118_sensorConfig_t * sensorConfig){
		assert(sensorConfig != NULL);

		#ifdef USE_ADS_1118_CS_PIN
			GPIO_SetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);
		#endif

		SPI_init();
		SPI_setBaudRate(sensorConfig->SPI_handle, SPI_BaudRatePrescaler_32);
		SPI_setCPHA(sensorConfig->SPI_handle, SPI_CPHA_2Edge);
	}

	bool_e ADS1118_setConfig(ADS1118_sensorConfig_t * sensorConfig,
									ADS1118_CONFIG_singleShotStart singleShotStart,
									ADS1118_CONFIG_inputMultiplexer inputMultiplexer,
									ADS1118_CONFIG_gainAmplifier gainAmplifier,
									ADS1118_CONFIG_operatingMode operatingMode,
									ADS1118_CONFIG_dataRate dataRate,
									ADS1118_CONFIG_temperatureSensorMode temperatureSensorMode,
									ADS1118_CONFIG_pullup pullup,
									bool_e configurationCheck){

		assert(sensorConfig != NULL);

		ADS1118_config_reg_s configReg;

		configReg.singleShotStart = singleShotStart;
		configReg.inputMultiplexer = inputMultiplexer;
		configReg.gainAmplifier = gainAmplifier;
		configReg.operatingMode = operatingMode;
		configReg.dataRate = dataRate;
		configReg.temperatureSensorMode = temperatureSensorMode;
		configReg.pullup = pullup;
		configReg.secureWrite = ADS1118_CONFIG_SECURE_WRITE;
		configReg.reservedReg = ADS1118_CONFIG_RESERVED;

		#ifdef USE_ADS_1118_CS_PIN
			GPIO_ResetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);
		#endif

		/*if(ADS1118_isAvailable(sensorConfig) == FALSE)
			return FALSE;*/

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_16_BIT);

		SPI_write(sensorConfig->SPI_handle, configReg.raw);

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_8_BIT);

		#ifdef USE_ADS_1118_CS_PIN
			GPIO_SetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);
		#endif

		if(configurationCheck){

			ADS1118_CONFIG_singleShotStart singleShotStartRead;
			ADS1118_CONFIG_inputMultiplexer inputMultiplexerRead;
			ADS1118_CONFIG_gainAmplifier gainAmplifierRead;
			ADS1118_CONFIG_operatingMode operatingModeRead;
			ADS1118_CONFIG_dataRate dataRateRead;
			ADS1118_CONFIG_temperatureSensorMode temperatureSensorModeRead;
			ADS1118_CONFIG_pullup pullupRead;

			bool_e res;

			res = ADS1118_getConfig(sensorConfig,
							&singleShotStartRead,
							&inputMultiplexerRead,
							&gainAmplifierRead,
							&operatingModeRead,
							&dataRateRead,
							&temperatureSensorModeRead,
							&pullupRead);

			if(res == FALSE)
				return FALSE;

			if((singleShotStartRead != singleShotStart)
					|| (inputMultiplexerRead != inputMultiplexer)
					|| (gainAmplifierRead != gainAmplifier)
					|| (operatingModeRead != operatingMode)
					|| (dataRateRead != dataRate)
					|| (temperatureSensorModeRead != temperatureSensorMode)
					|| (pullupRead != pullup))
				return FALSE;
		}

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

		#ifdef USE_ADS_1118_CS_PIN
			GPIO_ResetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);
		#endif

		/*if(ADS1118_isAvailable(sensorConfig) == FALSE)
			return FALSE;*/

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_16_BIT);

		SPI_read(sensorConfig->SPI_handle);

		configReg.raw = SPI_read(sensorConfig->SPI_handle);

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_8_BIT);

		#ifdef USE_ADS_1118_CS_PIN
			GPIO_SetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);
		#endif

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

		#ifdef USE_ADS_1118_CS_PIN
			GPIO_ResetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);
		#endif

		/*if(ADS1118_isAvailable(sensorConfig) == FALSE)
			return FALSE;*/

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_16_BIT);

		*value = SPI_read(sensorConfig->SPI_handle);

		SPI_setDataSize(sensorConfig->SPI_handle, SPI_DATA_SIZE_8_BIT);

		#ifdef USE_ADS_1118_CS_PIN
			GPIO_SetBits(sensorConfig->GPIO_cs.GPIOx, sensorConfig->GPIO_cs.GPIO_Pin);
		#endif

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
