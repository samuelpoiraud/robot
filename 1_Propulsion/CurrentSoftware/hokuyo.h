/*
 * hokuyo.h
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */

#ifndef HOKUYO_H_
#define HOKUYO_H_
//#define TRIANGULATION

#include "QS/QS_all.h"

#ifdef USE_HOKUYO


	#define HOKUYO_MAX_FOES	16

	typedef struct{
		Sint32 dist;
		int teta;
		Sint32 coordX;
		Sint32 coordY;
	}HOKUYO_adversary_position;

	void HOKUYO_init(void);
	void HOKUYO_process_main(void);
	void HOKUYO_process_it(Uint8 ms);
	bool_e HOKUYO_is_working_well(void);
	void refresh_adversaries(void);

	#ifdef TRIANGULATION
	typedef struct{       //position des balises
		Sint32 x;
		Sint32 y;
		Uint32 teta;
		Uint32 weight;
	}position;
	void tri_points();
	void Hokuyo_validPointsAndBeacons();
	//void dectect_centre_beacons();


	#endif

#endif

#endif /* HOKUYO_H_ */
