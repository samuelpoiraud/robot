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

#if 0

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

static bool_e ADS1118_writeSPI();
static bool_e ADS1118_readSPI();

void ADS1118_init(void){
	SPI_init();
}

bool_e ADS1118_setConfig(ADS1118_sensorConfig_t sensorConfig,
								ADS1118_CONFIG_singleShotStart singleShotStart,
								ADS1118_CONFIG_inputMultiplexer inputMultiplexer,
								ADS1118_CONFIG_gainAmplifier gainAmplifier,
								ADS1118_CONFIG_operatingMode operatingMode,
								ADS1118_CONFIG_dataRate dataRate,
								ADS1118_CONFIG_temperatureSensorMode temperatureSensorMode,
								ADS1118_CONFIG_pullup pullup){


}

bool_e ADS1118_getConfig(ADS1118_sensorConfig_t sensorConfig,
						ADS1118_CONFIG_singleShotStart * singleShotStart,
						ADS1118_CONFIG_inputMultiplexer * inputMultiplexer,
						ADS1118_CONFIG_gainAmplifier * gainAmplifier,
						ADS1118_CONFIG_operatingMode * operatingMode,
						ADS1118_CONFIG_dataRate * dataRate,
						ADS1118_CONFIG_temperatureSensorMode * temperatureSensorMode,
						ADS1118_CONFIG_pullup * pullup){


}

bool_e ADS1118_getValue(Uint16 * value){

}

static bool_e ADS1118_writeSPI(){

}

static bool_e ADS1118_readSPI(){

}

#endif
