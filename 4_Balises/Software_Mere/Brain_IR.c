/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Brain_IR.c
 *	Package : Projet Balise Mère
 *	Description : Traitement des infos reçues par les balises IR et calculs des positions adverses.
 *	Auteur : Nirgal
 *	Version 201208
 */
	#include "Brain_IR.h"
//	#include "Array_IR.h"
	#include "Secretary.h"
	#include "Eyes.h"
	
/*
O : balises fixes
R : 1 robot adverse

  ----y->				3m
  |		________________________________O  Corner
  x		|							  /	|
  |		|							 /	|
  v		|				    	    / 	|
		O--------------------------R   	|  2m
		Midle					  	\	|
		|							 \	|
		|							  \	|
		________________________________O  Mother

*/

volatile adversary_eyes_buffers_t * p_adversary_eyes_buffers;


void BRAIN_IR_init(void)
{
	p_adversary_eyes_buffers = NULL;
}

	
void BRAIN_IR_task_add(adversary_eyes_buffers_t * adversary_eyes_buffers)
{
	if(p_adversary_eyes_buffers == NULL)	//Si le pointeur qui retient la prochaine struture à traiter est disponible...
		p_adversary_eyes_buffers = adversary_eyes_buffers;
}	

void BRAIN_IR_end_of_cycle(adversary_eyes_buffers_t * adversary_eyes_buffers)
{
	//calcul des durations et des midle !
	detection_t * p_detection;
	Uint16 index_max, index;
	
	//TODO gérer chaque balise avec une variable beacon_id dans une boucle for
	index_max = adversary_eyes_buffers->buffers[BEACON_ID_MOTHER].index;
	for(index = 0; index < index_max; index++)
		debug_printf("%d ",adversary_eyes_buffers->buffers[BEACON_ID_MOTHER].buffer[index]);
	debug_printf("\n");
	
		
/*	beacon_id_e beacon_id;
	Uint8 nb_detections_to_analyse;
	Uint8 index, index_max_duration;
	time16_t max_duration;
	
	
	
	
	return;
	
	
	
	
	
	for(beacon_id = 0; beacon_id <= BEACON_ID_MIDLE ; beacon_id++)
	{
		nb_detections_to_analyse = (beacons_detections[beacon_id].more_than_3_detections)?3:beacons_detections[beacon_id].current_detection_index + 1;
	//	if(beacon_id==0) debug_printf("b%d=%d | ", beacon_id, nb_detections_to_analyse);
		max_duration = 0;
		index_max_duration = 0;
		for(index = 0; index < nb_detections_to_analyse; index++)
		{

			p_detection = &beacons_detections[beacon_id].detections[index];
			p_detection->duration = p_detection->end - p_detection->begin;
		//	if(beacon_id==0) debug_printf("d1:%d->%d=%d  ",p_detection->begin,p_detection->end,p_detection->duration);
			p_detection->begin += (p_detection->duration)/2;	//begin devient maintenant le point milieu de la détection
			if(p_detection->duration > max_duration)
			{
				max_duration = p_detection->duration;
				index_max_duration = index;
			}	
		}
		if(beacon_id==0) debug_printf("d%d=%d\n", index_max_duration, max_duration);	
		beacons_detections[beacon_id].current_detection_index = index_max_duration;
		//le current detection index devient maintenant l'index de la détection principale... qui est alors l'objet de toute notre attention !
		
	}	
	debug_printf("\n");
	*/
}

void BRAIN_IR_process_main(void)
{
	Sint16 a, b;
	Sint16 x, y;
	if(p_adversary_eyes_buffers)
	{
		BRAIN_IR_end_of_cycle((adversary_eyes_buffers_t*)(p_adversary_eyes_buffers));
		/*
		BRAIN_IR_find_ab(p_adversary_eyes_buffers, &a, &b);
		BRAIN_IR_find_xy(a, b, &x, &y);
		SECRETARY_send_....._(p_adversary_eyes_buffers->adversary, x, y);
		*/
		EYES_init_adversary_eyes_buffers((adversary_eyes_buffers_t*)(p_adversary_eyes_buffers));
		//EYES_init_adversary_beacons_detections((adversary_beacons_detections_t *)(p_adversary_eyes_buffers));
		
		
		p_adversary_eyes_buffers = NULL;
	}	
}	
