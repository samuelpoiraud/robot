/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : scan_laser.h
 *  Package : Propulsion
 *  Description : Scan avec scan_laser
 *  Auteur :
 *  Version 201001
 */

#ifndef SCAN_LASER_H
#define SCAN_LASER_H

void SCAN_init();
void SCAN_process_it();
void SCAN_PROCESS(CAN_msg_t *msg);

#endif // SCAN_LASER_H
