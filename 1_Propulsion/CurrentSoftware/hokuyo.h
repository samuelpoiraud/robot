/*
 * hokuyo.h
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */

#ifndef HOKUYO_H_
#define HOKUYO_H_


#ifdef USE_HOKUYO



	void HOKUYO_init(void);
	void HOKUYO_process_main(void);
	void HOKUYO_process_it(Uint8 ms);

#endif

#endif /* HOKUYO_H_ */
