/*
 *	Club Robot ESEO 2014 - 2015
 *	Holly & Wood
 *
 *	Fichier : QS_can_verbose.c
 *	Package : Qualité Soft
 *	Description : Verbosité des messages CAN sur uart
 *	Auteur : Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100111
 */

#include "QS_rpm_sensor.h"

#ifdef USE_RPM_SENSOR

	#include "QS_outputlog.h"

	#ifndef USE_EXTERNAL_IT
		#error "Activation USE_EXTERNAL_IT requise pour le fonctionnement de RPM_SENSOR"
	#endif

	typedef struct{
		bool_e used;
		Uint16 speed;
		time32_t durationTick;
		time32_t lastTimeTick;
		Uint8 nb_tick_per_rev;
	}RPM_SENSOR_info_t;

	static volatile bool_e initialized = FALSE;

	static volatile RPM_SENSOR_info_t info[RPM_SENSOR_NB_SENSOR] = {0};

	static void RPM_SENSOR_sensorIt(Uint8 id);

	void RPM_SENSOR_init(){
		if(initialized)
			return;
		initialized = TRUE;

		RPM_SENSOR_id_t i;
		for(i=0; i<RPM_SENSOR_NB_SENSOR; i++){
			info[i].used = FALSE;
			info[i].speed = 0;
		}

		EXTERNALIT_init();
	}

	RPM_SENSOR_id_t RPM_SENSOR_addSensor(RPM_SENSOR_port_e port, RPM_SENSOR_pin_e pin, RPM_SENSOR_edge_e edge, Uint8 nb_tick_per_rev){

		bool_e found = FALSE;
		RPM_SENSOR_id_t i, idFound;
		for(i=0; i<RPM_SENSOR_NB_SENSOR; i++){
			if(info[i].used == FALSE){
				found = TRUE;
				idFound = i;
				break;
			}
		}

		if(found){
			info[idFound].used = TRUE;
			info[idFound].nb_tick_per_rev = nb_tick_per_rev;
			EXTERNALIT_configureWithId(port, pin, edge, RPM_SENSOR_sensorIt, idFound);
			EXTERNALIT_set_it_enabled(port, pin, TRUE);
			return idFound;
		}else{
			error_printf("Ajout de RPM_SENSOR impossible nombre maximum atteint\n");
			return RPM_SENSOR_ID_FAULT;
		}
	}

	Uint16 RPM_SENSOR_getSpeed(RPM_SENSOR_id_t id){
		if(id >= RPM_SENSOR_NB_SENSOR || info[id].used == FALSE){
			error_printf("Lecture de vitesse impossible RPM_SENSOR_id non valide\n");
			return 0;
		}

		return info[id].speed;
	}

	static void RPM_SENSOR_sensorIt(Uint8 id){
		if(global.absolute_time - info[id].lastTimeTick != 0)
			info[id].speed = (1/((float)info[id].nb_tick_per_rev * (global.absolute_time - info[id].lastTimeTick)))*1000*60;

		info[id].durationTick = global.absolute_time - info[id].lastTimeTick;
		info[id].lastTimeTick = global.absolute_time;
	}

	void RPM_SENSOR_process_it(){

		RPM_SENSOR_id_t i;
		for(i=0; i<RPM_SENSOR_NB_SENSOR; i++){
			if(info[i].used == TRUE){
				if(global.absolute_time > info[i].lastTimeTick + info[i].durationTick){
					info[i].speed = (1/((float)info[i].nb_tick_per_rev*(global.absolute_time - info[i].lastTimeTick)))*1000*60;
				}
			}
		}
	}

#endif
