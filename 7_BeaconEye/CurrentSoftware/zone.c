/**
 * \file zone.c
 * \brief Définition, initialisation et teste de Zone
 * \author Amaury.Gaudin Edouard.Thyebaut
 * \version 1
 * \date 4 décembre 2013
 *
 * Fonctions de créations des 14 zones importantes du plateau de jeux ainsi que l'initialisation de ces zones
 *  Puis le teste de la présence ou non d'un robot dans ces zones avec les informations à retourner en fonction des zones
 */

#include "zone.h"
#include "QS_hokuyo/hokuyo.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"
#include "Secretary.h"
#include "environment.h"
#include "QS/QS_maths.h"
#include "IHM/terminal.h"

static ZONE_zone_t zones[ZONE_NUMBER];


static void ZONE_initFields(ZONE_zoneId_e id, ZONE_rectangle_t rectangle, bool_e isDependingOnColor, time32_t thresholdDetectionTime);
static void ZONE_reset(ZONE_zoneId_e id);
static void ZONE_verbose(ZONE_zoneId_e id);
static bool_e ZONE_objectIsInZone(ZONE_zoneId_e id, GEOMETRY_point_t objectPosition);
static Uint8* ZONE_getZoneName(ZONE_zoneId_e id);
static bool_e ZONE_objectIsInZone(ZONE_zoneId_e id, GEOMETRY_point_t objectPosition);


void ZONE_init(void) {
	UNUSED_VAR(ZONE_initFields);
	UNUSED_VAR(ZONE_reset);
	UNUSED_VAR(ZONE_verbose);
	UNUSED_VAR(ZONE_getZoneName);
	UNUSED_VAR(ZONE_objectIsInZone);
}

void ZONE_processMain(void) {

}

void ZONE_enable(ZONE_zoneId_e id, robot_id_e robot_id, ZONE_event_t events) {
	ZONE_zone_t *z = ZONE_getZone(id);

	if(robot_id == BIG_ROBOT) {
		z->eventsBig = events;
		z->enable |= ZONE_ENABLE_FOR_BIG;
	}
	if(robot_id == SMALL_ROBOT) {
		z->eventsSmall = events;
		z->enable |= ZONE_ENABLE_FOR_SMALL;
	}
	z->events = zones[id].eventsBig | z->eventsSmall;
}

void ZONE_disable(ZONE_zoneId_e id, robot_id_e robot_id) {
	ZONE_zone_t *z = ZONE_getZone(id);

	if(robot_id == BIG_ROBOT) {
		z->eventsBig = EVENT_NO_EVENT;
		z->enable &= ~ZONE_ENABLE_FOR_BIG;
	}
	if(robot_id == SMALL_ROBOT)	{
		z->eventsSmall = EVENT_NO_EVENT;
		z->enable &= ~ZONE_ENABLE_FOR_SMALL;
	}
	z->events = z->eventsBig | z->eventsSmall;
}

void ZONE_disableAll(void) {
	Uint8 i;
	for(i = 0; i < ZONE_NUMBER; i++) {
		zones[i].eventsBig = EVENT_NO_EVENT;
		zones[i].eventsSmall = EVENT_NO_EVENT;
		zones[i].enable = ZONE_NOT_ENABLED;
	}
}

ZONE_zone_t* ZONE_getZone(ZONE_zoneId_e id) {
	assert(id < ZONE_NUMBER);
	return &zones[id];
}

static void ZONE_initFields(ZONE_zoneId_e id, ZONE_rectangle_t rectangle, bool_e isDependingOnColor, time32_t thresholdDetectionTime) {
	ZONE_zone_t *z = ZONE_getZone(id);

	z->rectangle = rectangle;
	z->isDependingOnColor = isDependingOnColor;

	z->enable = ZONE_NOT_ENABLED;

	z->events = EVENT_NO_EVENT;
	z->eventsBig = EVENT_NO_EVENT;
	z->eventsSmall = EVENT_NO_EVENT;

	z->inputTime = 0;
	z->presenceTime = 0;
	z->thresholdDetectionTime = thresholdDetectionTime;

	z->alertSent = FALSE;

	z->isInitialized = TRUE;
}

static void ZONE_reset(ZONE_zoneId_e id) {
	ZONE_zone_t *z = ZONE_getZone(id);

	z->enable = ZONE_NOT_ENABLED;

	z->events = EVENT_NO_EVENT;
	z->eventsBig = EVENT_NO_EVENT;
	z->eventsSmall = EVENT_NO_EVENT;

	z->inputTime = 0;
	z->presenceTime = 0;

	z->alertSent = FALSE;
}

static void ZONE_verbose(ZONE_zoneId_e id) {

}

static Uint8* ZONE_getZoneName(ZONE_zoneId_e id) {
	switch(id) {
		default:
			return (Uint8*)"UNKNOW_ZONE";
	}
}

static bool_e ZONE_objectIsInZone(ZONE_zoneId_e id, GEOMETRY_point_t objectPosition) {
	ZONE_zone_t *z = ZONE_getZone(id);
	ZONE_rectangle_t rec = z->rectangle;

	if(z->isDependingOnColor) {
		return is_in_square( rec.origin.x, rec.origin.x + rec.width,
				    		 COLOR_Y(rec.origin.y), COLOR_Y(rec.origin.y + rec.height),
							 objectPosition);
	} else {
		return is_in_square( rec.origin.x, rec.origin.x + rec.width,
						     rec.origin.y, rec.origin.y + rec.height,
							 objectPosition);
	}
}

#if 0
void ZONE_detection(void) {
	ZONE_zoneId_e id;
	Uint8 j;

	for(id = 0; id < ZONE_NUMBER; id++) {
		ZONE_zone_t *z = ZONE_getZone(id);

		/* On suppose que la zone n'est pas occupée */
		z->isSomeone = FALSE;

		if (z->enable != ZONE_NOT_ENABLED) {
			for(j = 0; j < HOKUYO_get_adversaries_number(); j++) {
				if(ZONE_object_is_in_zone(HOKUYO_get_adversary_position(j), &zones[i]))
				{
					zones[i].someone_is_here=TRUE;          //La zone est occupée par un des robots
					if(zones[i].events & EVENT_SPECIAL)
						ZONE_specific_analyse(HOKUYO_get_adversary_position(j),&zones[i]);		//Analyse fine des coordonnées de l'objet dans la zone.
					//On continue de la boucle (si plusieurs objets sont dans la zone, l'analyse des coordonnées doit se faire pour chaque objets).
				}
			}
		}
	}
}


static void ZONE_analysis(void)
{
	zone_e i;
	zones_t * z;

	for (i=0;i<ZONE_NUMBER;i++)
	{
		z = &zones[i];	//Pointeur vers la zone traitée.

		switch(z->busy_state)
		{
			case NOBODY_IN_ZONE:
				if(z->someone_is_here)
				{
					z->busy_state = SOMEONE_IS_IN;
					z->input_time = absolute_time;
					if(z->events & EVENT_GET_IN)
						SECRETARY_set_event(i, EVENT_GET_IN);
					ZONE_verbose(i);
				}
				break;
			case SOMEONE_IS_IN:							//Un ou des objets sont	 	actuellement dans la zone
				if(!z->someone_is_here)
					z->busy_state = SOMEONE_GET_OUT;
				else
				{
					z->presence_duration = absolute_time - z->input_time;
					//L'évènement de temps de présence n'est valable que si min_detectection_time est non nul...
					if(z->min_detection_time != 0 && z->presence_duration > z->min_detection_time)	//Depuis qu'il est entré, il a passé min_detection_time dans la zone
					{
						z->busy_state = SOMEONE_IS_IN_SINCE_MIN_DETECTION_TIME;
						ZONE_verbose(i);
						if(z->events & EVENT_TIME_IN)
							SECRETARY_set_event(i, EVENT_TIME_IN);
					}
				}
				break;
			case SOMEONE_IS_IN_SINCE_MIN_DETECTION_TIME:	//Un ou des objets sont 	actuellement dans la zone depuis au moins le temps mini de détection
				if(!z->someone_is_here)
					z->busy_state = SOMEONE_GET_OUT;
				break;
			case SOMEONE_GET_OUT:						//Un ou des objets sortent 	actuellement de la zone
				ZONE_verbose(i);
				if(z->events & EVENT_GET_OUT)
					SECRETARY_set_event(i, EVENT_GET_OUT);
				//Dans les modes spécifiques, on envoit l'info mesurée lors de la sortie...donc ici...
				z->busy_state = NOBODY_IN_ZONE;
				break;
			default:
				z->busy_state = NOBODY_IN_ZONE;
				break;
		}
	}
}

#define TEXTE_SIZE	40
static void ZONE_verbose(zone_e i)
{
	zones_t * z;
	char  str[TEXTE_SIZE];
	Uint8 index = 0;
	UNUSED_VAR(index);
	z = &zones[i];	//Pointeur vers la zone traitée.
	switch(z->busy_state)
	{
		case SOMEONE_IS_IN:
			index = sprintf(str,"IN  :");
			break;
		case SOMEONE_IS_IN_SINCE_MIN_DETECTION_TIME:
			index = sprintf(str,"TIME:");
			break;
		case SOMEONE_GET_OUT:
			index = sprintf(str,"OUT :");
			break;
		default:
			break;
	}

	switch (i){
//		case ZONE_FRESQUO:								sprintf(str+index,"FRESQUO");		break;
		default:																			break;
	}
	debug_printf("%s\n",str);
}

#endif



