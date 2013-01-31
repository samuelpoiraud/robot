/* 
 * File:   ball_grabber.h
 * Author: vincent
 *
 * Created on November 23, 2012, 11:54 AM
 */
#include "queue.h"
#include "QS/QS_ax12.h"
#include "QS/QS_all.h"

#ifndef BALL_GRABBER_H
#define	BALL_GRABBER_H

typedef enum
{
	BALL_GRABBER_UP=0,
	BALL_GRABBER_DOWN,
	BALL_GRABBER_TIDY

}BALL_GRABBER_pos_e;

#define BALL_GRABBER_VAL_UP 10
#define BALL_GRABBER_VAL_DOWN 14
#define BALL_GRABBER_VAL_TIDY 180


void BALL_GRABBER_init();
void BALL_GRABBER_goPose(queue_id_t queue_id,bool_e init);


#endif	/* BALL_GRABBER_H */

