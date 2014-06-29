/*
 * hokuyo.h
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */

#ifndef HOKUYO_H_
#define HOKUYO_H_


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

#endif

#endif /* HOKUYO_H_ */
