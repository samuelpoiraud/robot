/**
 * \file fix_beacon.h
 * \brief Dialogue avec la balise fixe / récupération des infos d'observations de l'adversaire
 * \author Nirgal
 * \version 201403
 * \date 19 mars 2014
 *
 */

#ifndef FIX_BEACON_H_
#define FIX_BEACON_H_


#include "QS/QS_all.h"
#include "QS/QS_CANmsgList.h"


typedef struct
{
	zone_event_t events;
	time32_t min_detection_time;
	time32_t presence_duration;
	bool_e someone_is_here;
	Uint16 specific_param_x;
	Uint16 specific_param_y;
	bool_e updated;					//Flag levé lors d'une mise à jour (appelée par environnment...), et baissé dans le main.. Donc un seul passage de boucle possible.
}zone_t;

zone_t * FIX_BEACON_get_pzone(zone_e i);



void FIX_BEACON_init(void);

//Attention à appeler cette fonction après le any_match (les flags d'updated y sont resetés)
void FIX_BEACON_clean(void);

//Sub-action demandant une info sur une zone
//Il FAUT envoyer NULL dans le paramètre msg !!!
error_e FIX_BEACON_get_infos(zone_e zone, CAN_msg_t * msg);

//Demande d'activation d'évènement de surveillance sur une zone.
//Les évènement peuvent se cumuler... 		exemple : FIX_BEACON_zone_enable(ZONE_FREQSQUO, EVENT_GET_IN | EVENT_GET_OUT);
void FIX_BEACON_zone_enable(zone_e zone, zone_event_t events);

//Désactive les évènements de surveillance d'une zone
void FIX_BEACON_zone_disable(zone_e zone);


//Traitement des messages CAN dont le SID est : STRAT_ZONE_INFOS
void FIX_BEACON_process_msg(CAN_msg_t * msg);

#endif // FIX_BEACON_H_
