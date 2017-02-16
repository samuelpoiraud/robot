/*
 * QS_hokuyo.h
 *
 *  Created on: 8 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_QS_HOKUYO_H_
#define QS_HOKUYO_QS_HOKUYO_H_

#include "../QS/QS_all.h"


/* Hokuyo frame */
#define LINE_FEED						0x0A	// Caractère de retour à la ligne
#define DATA_BLOCK_SIZE					64
/* Timeout */
#define HOKUYO_BUFFER_READ_TIMEOUT		500  	// [ms]
#define HOKUYO_ECHO_READ_TIMEOUT		100  	// [ms]
/* Hokuyo specification */
#define FIRST_MEASUREMENT_POINT			0
#define SENSOR_FRONT_STEP				540
#define LAST_MEASUREMENT_POINT			1080
#define SLIT_DIVISION					1440	// Nombre de pas de mesures pour 360°
#define NB_STEP_MAX						(LAST_MEASUREMENT_POINT - FIRST_MEASUREMENT_POINT)
#define ANGLE_RESOLUTION				(360 * 100 / SLIT_DIVISION) // [°*100]
#define FIRST_STEP_DEG					4500	// [°*100]
#define FRONT_STEP_DEG					18000   // [°*100]
#define LAST_POINT_DEG					31525   // [°*100]
/* Treatement point */
#define TO_CLOSE_DISTANCE				100		// [mm]
#define TO_FAR_DISTANCE					3500	// [mm]
#define FIELD_SIZE_Y 					3000	// [mm]
#define FIELD_SIZE_X 					2000	// [mm]
#define HOKUYO_MARGIN_FIELD_SIDE_IGNORE 50 		// [mm] Marge sur les bords du terrain
#define MARGIN_BEACON					100		// [mm] Marge pour les balises extérieures

#define DEG100_TO_PI4096(x)				((((Sint32)(x))*183)>>8)

typedef struct{
	Sint32 dist;
	int teta;
	Sint32 coordX;
	Sint32 coordY;
	Sint32 coordX_prec;
	Sint32 coordY_prec;
	Uint8 accuracy;
}HOKUYO_adversary_position;

void HOKUYO_init(void);
void HOKUYO_processMain(void);
void HOKUYO_putsCommand(Uint8 tab[], Uint16 length);
void HOKUYO_writeCommand(Uint8 tab[]);
Uint8 HOKUYO_convertChecksum(Uint8 checksum);

#endif /* QS_HOKUYO_QS_HOKUYO_H_ */
