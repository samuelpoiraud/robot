/*
 * hokuyo.h
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */

#ifndef HOKUYO_H_
#define HOKUYO_H_

	#define HOKUYO_OFFSET 4500 //45 degrés
	#define HOKUYO_ANGLE_ROBOT_TERRAIN 0
	#define HOKUYO_DETECTION_MARGE 130
	#define HOKUYO_EVITEMENT_MIN 150
	#define HOKUYO_BORDS_TERRAIN 80
	#define ROBOT_COORDX 130
	#define ROBOT_COORDY 180

	typedef struct{
		Sint32 dist;
		int teta;
		Sint32 coordX;
		Sint32 coordY;
	}HOKUYO_data;

	typedef struct{
			Sint32 coordX;
			Sint32 coordY;
	}HOKUYO_ennemy;


	HOKUYO_data hokuyo_sushi[1080];
	HOKUYO_ennemy hokuyo_ennemi[1080];

	void HOKUYO_init(void);
	void HOKUYO_process_main(void);



#endif /* HOKUYO_H_ */
