/**
 * \file Secretary.h
 * \brief Interface de communication avec le reste du monde...
 * \author Nirgal
 * \version 201403
 * \date 18 mars 2014
 *
 */

#ifndef SECRETARY_H_
#define SECRETARY_H_
#include "QS/QS_all.h"
#include "QS/QS_CANmsgList.h"

void SECRETARY_init(void);
void SECRETARY_processMain(void);
void SECRETARY_setEvent(ZONE_zoneId_e id, ZONE_event_t event);

#endif //SECRETARY_H_
