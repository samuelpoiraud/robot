/*
 *	Club Robot ESEO 2014 - 2015
 *	Holly & Wood
 *
 *	Fichier : QS_can_verbose.h
 *	Package : Qualité Soft
 *	Description : Verbosité des messages CAN sur uart
 *	Auteur : Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100111
 */

/** ----------------  Defines possibles  --------------------
 *  USE_RPM_SENSOR				: Activation du module RPM SENSOR
 *	RPM_SENSOR_NB_SENSOR		: Nombre de RPM SENSOR à utiliser
 *
 * ----------------  Choses à savoir  --------------------
 *	USE_EXTERNAL_IT				: Define obligatoire
 */

#ifndef QS_RPM_SENSOR_H
	#define	QS_RPM_SENSOR_H

	#include "QS_all.h"

	#ifdef USE_RPM_SENSOR

		#include "QS_external_it.h"

		typedef enum{
			RPM_SENSOR_GPIO_A = EXTIT_GpioA,
			RPM_SENSOR_GPIO_B = EXTIT_GpioB,
			RPM_SENSOR_GPIO_C = EXTIT_GpioC,
			RPM_SENSOR_GPIO_D = EXTIT_GpioD,
			RPM_SENSOR_GPIO_E = EXTIT_GpioE,
			RPM_SENSOR_GPIO_F = EXTIT_GpioF,
			RPM_SENSOR_GPIO_G = EXTIT_GpioG
		}RPM_SENSOR_port_e;

		typedef enum{
			RPM_SENSOR_PIN_0 = EXTIT_PIN_0,
			RPM_SENSOR_PIN_1 = EXTIT_PIN_1,
			RPM_SENSOR_PIN_2 = EXTIT_PIN_2,
			RPM_SENSOR_PIN_3 = EXTIT_PIN_3,
			RPM_SENSOR_PIN_4 = EXTIT_PIN_4,
			RPM_SENSOR_PIN_5 = EXTIT_PIN_5,
			RPM_SENSOR_PIN_6 = EXTIT_PIN_6,
			RPM_SENSOR_PIN_7 = EXTIT_PIN_7,
			RPM_SENSOR_PIN_8 = EXTIT_PIN_8,
			RPM_SENSOR_PIN_9 = EXTIT_PIN_9,
			RPM_SENSOR_PIN_10 = EXTIT_PIN_10,
			RPM_SENSOR_PIN_11 = EXTIT_PIN_11,
			RPM_SENSOR_PIN_12 = EXTIT_PIN_12,
			RPM_SENSOR_PIN_13 = EXTIT_PIN_13,
			RPM_SENSOR_PIN_14 = EXTIT_PIN_14,
			RPM_SENSOR_PIN_15 = EXTIT_PIN_15
		}RPM_SENSOR_pin_e;

		typedef enum{
			RPM_SENSOR_Edge_Rising,
			RPM_SENSOR_Edge_Falling,
			RPM_SENSOR_Edge_Both
		}RPM_SENSOR_edge_e;

		typedef Uint8 RPM_SENSOR_id_t;
		#define RPM_SENSOR_ID_FAULT			0xFF

		void RPM_SENSOR_init();
		void RPM_SENSOR_process_it();

		RPM_SENSOR_id_t RPM_SENSOR_addSensor(RPM_SENSOR_port_e port, RPM_SENSOR_pin_e pin, RPM_SENSOR_edge_e edge, Uint8 nb_tick_per_rev);
		Uint16 RPM_SENSOR_getSpeed(RPM_SENSOR_id_t id);

	#endif

#endif	/* def QS_RPM_SENSOR_H*/
