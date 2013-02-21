/* 
 * File:   Hammer.h
 * Author: cha
 *
 * Created on December 4, 2012, 6:17 AM
 */
#include "queue.h"
#include "QS/QS_ax12.h"
#include "QS/QS_all.h"

#ifndef HAMMER_H
#define	HAMMER_H



typedef enum
{
	HAMMER_UP=0,
	HAMMER_DOWN,
	HAMMER_TIDY

}HAMMER_pos_e;

#define HAMMER_VAL_UP 150
#define HAMMER_VAL_DOWN 90
#define HAMMER_VAL_TIDY 180


void HAMMER_init();
void HAMMER_goPose(queue_id_t queue_id,bool_e init);



#endif	/* HAMMER_H */

