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

#ifndef QS_RPM_SENSOR_H
	#define	QS_RPM_SENSOR_H

	#include "QS_all.h"
	#include "QS_external_it.h"

	#define RPM_SENSOR_NB_SENSOR		1

	typedef Uint8 RPM_SENSOR_id_t;
	#define RPM_SENSOR_ID_FAULT			0xFF

	void RPM_SENSOR_init();

	RPM_SENSOR_id_t RPM_SENSOR_addSensor(EXTERNALIT_port_e port, Uint8 pin, EXTERNALIT_edge_e edge, Uint8 nb_tick_per_rev);

	Uint16 RPM_SENSOR_getSpeed(RPM_SENSOR_id_t id);

	void RPM_SENSOR_process_it();

#endif	/* def QS_RPM_SENSOR_H*/
