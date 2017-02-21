/**
 * \file zone.h
 * \brief Définition d'enum,de structure et de fonction
 * \author Amaury.Gaudin Edouard.Thyebaut
 * \version 1
 * \date 27 novembre 2013
 */

#ifndef ZONE_H_
#define ZONE_H_
#include "stm32f4xx.h"
#include "QS/QS_all.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_maths.h"

/* Evènement associé à une zone */
typedef Uint8 ZONE_enable_t;
#define ZONE_NOT_ENABLED				0x00
#define ZONE_ENABLE_FOR_BIG				0x01
#define ZONE_ENABLE_FOR_SMALL 			0x02
#define ZONE_ENABLE_FOR_BIG_AND_SMALL 	(ZONE_ENABLE_FOR_BIG | ZONE_ENABLE_FOR_SMALL)

typedef struct {
	GEOMETRY_point_t origin;			/* Origine du rectangle */
	Uint16 width;						/* Largueur du rectangle */
	Uint16 height;						/* Hauteur du rectangle */
} ZONE_rectangle_t;

typedef enum {
	BUSY_STATE_NOBODY = 0,				/* Aucun objet dans la zone */
	BUSY_STATE_SOMEONE,					/* Un objet est dans la zone*/
	BUSY_STATE_SOMEONE_WITH_MIN_TIME,	/* Un objet est dans la zone depuis un temps défini */
	BUSY_STATE_GET_OUT					/* L'objet sort de la zone */
} ZONE_busyState_e;

typedef struct {
	ZONE_rectangle_t rectangle;			/* Rectangle définissant la zone */
	bool_e isDependingOnColor;			/* Indique si la zone dépend de la couleur, dans ce cas on prendra le symétrique si l'on change de couleur */

	ZONE_enable_t enable;				/* Activation ou non de la zone et pour quel robot */

	ZONE_event_t events;				/* Evènements que l'on souhaite observés sur la zone */
	ZONE_event_t eventsBig;				/* Evènements que l'on souhaite observés sur la zone pour le gros robot */
	ZONE_event_t eventsSmall;			/* Evènements que l'on souhaite observés sur la zone pour le petit robot */

	ZONE_busyState_e busyState;			/* Etat d'occupation de la zone */
	bool_e isSomeone;					/* Variable intermédiaire pour gérer l'état d'occupation */

	time32_t inputTime;					/* Date à laquelle l'objet est rentré dans la zone */
	time32_t presenceTime;				/* Temps de présence de l'objet dans la zone depuis son entrée */
	time32_t thresholdDetectionTime;	/* Seuil de déclenchement de présence dans la zone pour l'évènement "BUSY_STATE_SOMEONE_WITH_MIN_TIME" */

	bool_e alertSent;					/* Indique si l'alerte a été envoyé */

	bool_e isInitialized;				/* Indique si la zone est initialisée */
} ZONE_zone_t;

void ZONE_init(void);
void ZONE_processMain(void);
void ZONE_enable(ZONE_zoneId_e id, robot_id_e robot_id, ZONE_event_t events);
void ZONE_disable(ZONE_zoneId_e id, robot_id_e robot_id);
void ZONE_disableAll(void);
ZONE_zone_t* ZONE_getZone(ZONE_zoneId_e id);



#endif /* ZONE_H_ */
