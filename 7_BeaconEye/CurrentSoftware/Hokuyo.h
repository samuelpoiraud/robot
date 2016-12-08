/*
 * hokuyo.h
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */


#include "QS/QS_all.h"


#ifndef HOKUYO_H_
#define HOKUYO_H_


	typedef struct{
		Sint32 dist;
		int teta;
		Sint32 coordX;
		Sint32 coordY;
		Sint32 coordX_prec;
		Sint32 coordY_prec;
		Uint8 accuracy;
	}HOKUYO_adversary_position;


	#define BEACON_COORDX 1000

	void HOKUYO_init(void);
	void HOKUYO_process_main(void);
	void HOKUYO_process_it(Uint8 ms);
	bool_e HOKUYO_is_working_well(void);
	HOKUYO_adversary_position * HOKUYO_get_valid_points(void);
	Uint16 HOKUYO_get_nb_valid_points(void);
	HOKUYO_adversary_position * HOKUYO_get_adversary_position(Uint8 i);
	Uint8 HOKUYO_get_adversaries_number(void);


#endif /* HOKUYO_H_ */






