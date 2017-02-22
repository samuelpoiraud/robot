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
#include "QS/QS_hokuyo/hokuyo.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"
#include "Secretary.h"
#include "environment.h"
#include "QS/QS_maths.h"
#include "IHM/terminal.h"
#include "LCD/low layer/ssd2119.h"
#include "IHM/view.h"
#include "LCD/image/terrain.h"

#define COLOR_EMPTY_ZONE				SSD2119_COLOR_GREEN
#define COLOR_SOMEONE_IN_ZONE			SSD2119_COLOR_YELLOW
#define COLOR_SOMEONE_IN_ZONE_TIME		SSD2119_COLOR_RED
#define ZONE_COLOR_Y(id, y)				((ZONE_getZone(id)->isDependingOnColor && global.current_color == BOT_COLOR) ? (y) : (GAME_ZONE_SIZE_Y - (y)))

static ZONE_zone_t zones[ZONE_NUMBER];
static bool_e isVerboseActivate = TRUE;
static bool_e needUpdate = FALSE;


static void ZONE_initFields(ZONE_zoneId_e id, ZONE_rectangle_t rectangle, bool_e isDependingOnColor, time32_t thresholdDetectionTime);
static void ZONE_reset(ZONE_zoneId_e id);
static void ZONE_verbose(ZONE_zoneId_e id);
static bool_e ZONE_objectIsInZone(ZONE_zoneId_e id, GEOMETRY_point_t objectPosition);
static char* ZONE_getZoneName(ZONE_zoneId_e id);
static bool_e ZONE_objectIsInZone(ZONE_zoneId_e id, GEOMETRY_point_t objectPosition);
static void ZONE_detection(void);
static void ZONE_analysis(void);
static char* ZONE_busyStateToString(ZONE_zoneId_e id);
static void ZONE_drawZone();
static void ZONE_eraseZone();
static void ZONE_drawRectangle(ZONE_rectangle_t rec, bool_e reverse, bool_e erase, Uint16 color);


void ZONE_init(void) {
	UNUSED_VAR(ZONE_initFields);
	UNUSED_VAR(ZONE_reset);
	UNUSED_VAR(ZONE_verbose);
	UNUSED_VAR(ZONE_getZoneName);
	UNUSED_VAR(ZONE_objectIsInZone);

	ZONE_initFields(ZONE_MIDDLE, (ZONE_rectangle_t) {500, 500, 1000, 1000}, TRUE, 2000);
	ZONE_enable(ZONE_MIDDLE, BIG_ROBOT, EVENT_GET_IN);
	ZONE_initFields(ZONE_BOTTOM, (ZONE_rectangle_t) {1250, 250, 1750, 750}, FALSE, 0);
	ZONE_enable(ZONE_BOTTOM, BIG_ROBOT, EVENT_GET_IN);

}

void ZONE_processMain(void) {
	if(needUpdate) {
		ZONE_detection();
		ZONE_analysis();
		ZONE_drawZone();
		needUpdate = FALSE;
	}
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

void ZONE_requestUpdate() {
	needUpdate = TRUE;
}

void ZONE_colorChange() {
	ZONE_eraseZone();
	ZONE_drawZone();
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
	if(isVerboseActivate) {
		TERMINAL_printf("ZONE : %s (%d) -> %s", ZONE_getZoneName(id), id, ZONE_busyStateToString(id));
	}
}

static char* ZONE_busyStateToString(ZONE_zoneId_e id) {
	ZONE_zone_t *z = ZONE_getZone(id);

	switch(z->busyState) {
		case BUSY_STATE_NOBODY:
			return "Vide";
		case BUSY_STATE_SOMEONE:
			return "Occupe";
		case BUSY_STATE_SOMEONE_WITH_MIN_TIME:
			return "Occupe avec depassement";
		case BUSY_STATE_GET_OUT:
			return "Sortie";
		default:
			return "UNKNOWN BUSY STATE";
	}
}

static char* ZONE_getZoneName(ZONE_zoneId_e id) {
	switch(id) {
		default:
			return "UNKNOWN ZONE";
	}
}

static bool_e ZONE_objectIsInZone(ZONE_zoneId_e id, GEOMETRY_point_t objectPosition) {
	ZONE_zone_t *z = ZONE_getZone(id);
	ZONE_rectangle_t rec = z->rectangle;

	if(z->isDependingOnColor) {
		if(global.current_color == TOP_COLOR) {
			debug_printf("%d %d | %d %d -> %d %d", rec.x0, GAME_ZONE_SIZE_Y - rec.y0, rec.x1, GAME_ZONE_SIZE_Y - rec.y1, objectPosition.x, objectPosition.y);
			return is_in_square(rec.x0, rec.x1, GAME_ZONE_SIZE_Y - rec.y0, GAME_ZONE_SIZE_Y - rec.y1, objectPosition);
		}
	}

	debug_printf("%d %d | %d %d -> %d %d", rec.x0, rec.y0, rec.x1, rec.y1, objectPosition.x, objectPosition.y);
	return is_in_square(rec.x0, rec.x1, rec.y0, rec.y1, objectPosition);
}

static void ZONE_detection(void) {
	ZONE_zoneId_e id;
	Uint8 j;

	for(id = 0; id < ZONE_NUMBER; id++) {
		ZONE_zone_t *z = ZONE_getZone(id);

		/* On suppose que la zone n'est pas occupée */
		z->isSomeone = FALSE;

		if (z->enable != ZONE_NOT_ENABLED) {
			for(j = 0; j < HOKUYO_getAdversariesNumber(); j++) {
				if(ZONE_objectIsInZone(id, (GEOMETRY_point_t) {HOKUYO_getAdversaryPosition(j)->coordX, HOKUYO_getAdversaryPosition(j)->coordY})) {
					z->isSomeone = TRUE;
				}
			}
		}
	}
}

static void ZONE_analysis(void) {
	ZONE_zoneId_e id;

	for (id = 0; id < ZONE_NUMBER;id++) {

		ZONE_zone_t *z = ZONE_getZone(id);

		switch(z->busyState) {

			case BUSY_STATE_NOBODY:
				if(z->isSomeone) {
					z->busyState = BUSY_STATE_SOMEONE;
					z->inputTime = global.absolute_time;
					if(z->events & EVENT_GET_IN) {
						//SECRETARY_set_event(i, EVENT_GET_IN);
					}
					ZONE_verbose(id);
				}
				break;

			case BUSY_STATE_SOMEONE:
				if(!z->isSomeone) {
					z->busyState = BUSY_STATE_GET_OUT;
				} else {
					z->presenceTime = global.absolute_time - z->inputTime;

					/* Le seuil de détection est actif s'il est différent de 0 */
					if(z->thresholdDetectionTime != 0 && z->presenceTime > z->thresholdDetectionTime) {
						z->busyState = BUSY_STATE_SOMEONE_WITH_MIN_TIME;
						ZONE_verbose(id);
						if(z->events & EVENT_TIME_IN) {
							//SECRETARY_set_event(id, EVENT_TIME_IN);
						}
					}
				}
				break;

			case BUSY_STATE_SOMEONE_WITH_MIN_TIME:
				if(!z->isSomeone) {
					z->busyState = BUSY_STATE_GET_OUT;
				}
				break;

			case BUSY_STATE_GET_OUT:
				ZONE_verbose(id);
				if(z->events & EVENT_GET_OUT) {
					//SECRETARY_set_event(id, EVENT_GET_OUT);
				}
				z->busyState = BUSY_STATE_NOBODY;
				break;

			default:
				break;
		}
	}
}

static void ZONE_drawZone() {
	ZONE_zoneId_e id;

	for (id = 0; id < ZONE_NUMBER;id++) {

		ZONE_zone_t *z = ZONE_getZone(id);
		ZONE_rectangle_t rec = z->rectangle;
		Uint16 recColor = COLOR_EMPTY_ZONE;

		switch(z->busyState) {

			case BUSY_STATE_NOBODY:
				recColor = COLOR_EMPTY_ZONE;
				break;

			case BUSY_STATE_SOMEONE:
				recColor = COLOR_SOMEONE_IN_ZONE;
				break;

			case BUSY_STATE_SOMEONE_WITH_MIN_TIME:
				recColor = COLOR_SOMEONE_IN_ZONE_TIME;
				break;

			case BUSY_STATE_GET_OUT:
				recColor = COLOR_EMPTY_ZONE;
				break;

			default:
				break;
		}

		ZONE_drawRectangle(rec, (z->isDependingOnColor && global.current_color == TOP_COLOR) ? TRUE : FALSE , FALSE, recColor);

	}
}

static void ZONE_eraseZone() {
	ZONE_zoneId_e id;

	for (id = 0; id < ZONE_NUMBER;id++) {
		ZONE_zone_t *z = ZONE_getZone(id);
		ZONE_rectangle_t rec = z->rectangle;
		bool_e reverse = FALSE;

		if(z->isDependingOnColor) {
			if(global.current_color == BOT_COLOR) {
				reverse = TRUE;
			} else {
				reverse = FALSE;
			}
		} else {
			reverse = FALSE;
		}

		ZONE_drawRectangle(rec, reverse, TRUE, 0x0000);
	}
}

static void ZONE_drawRectangle(ZONE_rectangle_t rec, bool_e reverse, bool_e erase, Uint16 color) {
	if(!reverse) {
		VIEW_drawRectangle( FIELD_TO_LCD(rec.y0), FIELD_TO_LCD(rec.x0),
							FIELD_TO_LCD(rec.y1), FIELD_TO_LCD(rec.x1),
							erase, color, &terrain);
	} else {
		VIEW_drawRectangle( FIELD_TO_LCD(GAME_ZONE_SIZE_Y - rec.y1), FIELD_TO_LCD(rec.x0),
							FIELD_TO_LCD(GAME_ZONE_SIZE_Y - rec.y0), FIELD_TO_LCD(rec.x1),
							erase, color, &terrain);
	}
}
