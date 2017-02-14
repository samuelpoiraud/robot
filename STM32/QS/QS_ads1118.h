/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : QS_ads1118.h
 *  Package : Qualité Soft
 *  Description : Gestion de l'ADC externe
 *  Auteur : Arnaud
 */

/** ----------------  Defines possibles  --------------------
 *	Aucun
 */

#ifndef ADS_1118_H
	#define ADS_1118_H

	#include "QS_all.h"

#if 0

	#ifdef USE_ADS_1118

		#include "QS_ports.h"

		typedef enum{
			ADS1118_CONFIG_SINGLE_SHOT_OFF		= 0,			// No effect
			ADS1118_CONFIG_SINGLE_SHOT_START	= 1				// Start a single conversion (when in power-down state)
		}ADS1118_CONFIG_singleShotStart;						// [RW]	| reset -> 0b0

		typedef enum{
			ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN0__AINN_IS_AIN1 	= 0b000,
			ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN0__AINN_IS_AIN3 	= 0b001,
			ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN1__AINN_IS_AIN3 	= 0b010,
			ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN2__AINN_IS_AIN3 	= 0b011,
			ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN0__AINN_IS_GND 	= 0b100,
			ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN1__AINN_IS_GND 	= 0b101,
			ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN2__AINN_IS_GND 	= 0b110,
			ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN3__AINN_IS_GND 	= 0b111,
		}ADS1118_CONFIG_inputMultiplexer;						// [RW]	| reset -> 0b000

		typedef enum{
			ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_6_144	= 0b000,
			ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_4_096	= 0b001,
			ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_2_048	= 0b010,
			ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_1_024	= 0b011,
			ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_0_512	= 0b100,
			ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_0_256	= 0b101,
			ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_0_256	= 0b110,
			ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_0_256	= 0b111,
		}ADS1118_CONFIG_gainAmplifier;							// [RW] | reset -> 0b010

		typedef enum{
			ADS1118_CONFIG_MODE_CONTINUOUS_CONVERSION	= 0,
			ADS1118_CONFIG_MODE_SINGLE_SHOT_CONVERSION	= 1
		}ADS1118_CONFIG_operatingMode;							// [RW] | reset -> 0b1

		typedef enum{
			ADS1118_CONFIG_DATA_RATE_008_SPS	= 0b000,
			ADS1118_CONFIG_DATA_RATE_016_SPS	= 0b001,
			ADS1118_CONFIG_DATA_RATE_032_SPS	= 0b010,
			ADS1118_CONFIG_DATA_RATE_064_SPS	= 0b011,
			ADS1118_CONFIG_DATA_RATE_128_SPS	= 0b100,
			ADS1118_CONFIG_DATA_RATE_250_SPS	= 0b101,
			ADS1118_CONFIG_DATA_RATE_475_SPS	= 0b110,
			ADS1118_CONFIG_DATA_RATE_860_SPS	= 0b111,
		}ADS1118_CONFIG_dataRate;								// [RW] | reset -> 0b100

		typedef enum{
			ADS1118_CONFIG_SENSOR_MODE_ADC			= 0,
			ADS1118_CONFIG_SENSOR_MODE_TEMPERATURE	= 1
		}ADS1118_CONFIG_temperatureSensorMode;					// [RW] | reset -> 0b0

		typedef enum{
			ADS1118_CONFIG_PULLUP_DISABLE	= 0,				// Pull up resistor disabled on DOUT | /DRDY pin
			ADS1118_CONFIG_PULLUP_ENABLE	= 1					// Pull up resistor enabled on DOUT | /DRDY pin
		}ADS1118_CONFIG_pullup;									// [RW] | reset -> 0b1

		typedef struct{
			GPIO_TypeDef* GPIOx;
			uint16_t GPIO_Pin;
			SPI_TypeDef* SPI_handle;
		}ADS1118_sensorConfig_t;

		void ADS1118_init(void);

		bool_e ADS1118_setConfig(ADS1118_sensorConfig_t sensorConfig,
								ADS1118_CONFIG_singleShotStart singleShotStart,
								ADS1118_CONFIG_inputMultiplexer inputMultiplexer,
								ADS1118_CONFIG_gainAmplifier gainAmplifier,
								ADS1118_CONFIG_operatingMode operatingMode,
								ADS1118_CONFIG_dataRate dataRate,
								ADS1118_CONFIG_temperatureSensorMode temperatureSensorMode,
								ADS1118_CONFIG_pullup pullup);

		bool_e ADS1118_getConfig(ADS1118_sensorConfig_t sensorConfig,
								ADS1118_CONFIG_singleShotStart * singleShotStart,
								ADS1118_CONFIG_inputMultiplexer * inputMultiplexer,
								ADS1118_CONFIG_gainAmplifier * gainAmplifier,
								ADS1118_CONFIG_operatingMode * operatingMode,
								ADS1118_CONFIG_dataRate * dataRate,
								ADS1118_CONFIG_temperatureSensorMode * temperatureSensorMode,
								ADS1118_CONFIG_pullup * pullup);

		bool_e ADS1118_getValue(Uint16 * value);

	#endif

#endif

#endif /* ADS_1118_H */
