/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : TEST_SERVO.h
 *	Package : Carte actionneur
 *	Description : Gestion du TEST_SERVO
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : BIG
 */

#if 0

#ifndef TEST_SERVO_H
#define	TEST_SERVO_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_BIG

#include "../queue.h"

void TEST_SERVO_init();

void TEST_SERVO_reset_config();

bool_e TEST_SERVO_CAN_process_msg(CAN_msg_t* msg);

void TEST_SERVO_run_command(queue_id_t queueId, bool_e init);

void TEST_SERVO_init_pos();

void TEST_SERVO_stop();

#endif  /* I_AM_ROBOT_BIG */
#endif	/* TEST_SERVO_H */

#endif
