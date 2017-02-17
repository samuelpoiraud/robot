/**
 * \file zone.c
 * \brief Définition, initialisation et teste de Zone
 * \author Amaury.Gaudin Edouard.Thyebaut
 * \version 1
 * \date 4 décembre 2013
 *
 * Fonctions de créations des 14 zones importantes du plateau de jeux ainsi que l'initialisation de ces zones
 *  Puis le teste de la présence ou non d'un robot dans ces zones avec les informations à retourner en fonction des zones
 */
#include "zone.h"
#include "QS_hokuyo/hokuyo.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"
#include "Secretary.h"


	static volatile time32_t absolute_time = 0;
	static zones_t zones[ZONE_NUMBER];
	static bool_e datas_updated = FALSE;


	static void ZONE_detection(void);
	static void ZONE_analysis(void);
	static void ZONE_verbose(zone_e i);
	static void ZONE_init_fields(zone_e i, Uint16 x_min, Uint16 x_max, Uint16 y_min, Uint16 y_max, time32_t min_detection_time, specific_analyse_mode_e specific_analyse_mode);




//Fonction appelée au début du match, pour nettoyer toutes les précédentes observations...
void ZONE_init(void)
{
	zone_e i;
	//				 Identifiant de la zone						coordonnées			time_ms, Positionnement spécifique
//	ZONE_init_fields(ZONE_FRESQUO, 								1,	28,	110,189,	1500,	FRESCO_LOCATOR); 					//fresque

	ZONE_new_color(BOT_COLOR);	//Couleur par défaut...

	//Vérification que toutes les zones sont initialisées
	for(i=0;i<ZONE_NUMBER;i++)
		if(zones[i].initialized == FALSE)
			debug_printf("WARNING : ZONE %d is not initialized !!!!!!!\n",i);
}


void ZONE_process_main(void)
{
	if(datas_updated)
	{
		datas_updated = FALSE;
		ZONE_detection();
		ZONE_analysis();
	}
}

//Activations par défaut en fonction de notre couleur...
void ZONE_new_color(color_e c){
	//Attention, on désactive TOUTES les activations précédemment activées !
	ZONE_disable_all();

//	ZONE_enable(BIG_ROBOT, ZONE_CENTRAL_QUATER_HEART_RED_MAMMOTH, 		EVENT_GET_IN | EVENT_TIME_IN | EVENT_GET_OUT);	//Foyer central

}

void ZONE_process_it_1ms(void)
{
	absolute_time++;
}

void ZONE_enable(robot_id_e robot_id, zone_e i, zone_event_t events)
{
	if(robot_id == BIG_ROBOT)
	{
		zones[i].events_for_big = events;
		zones[i].enable |= ZONE_ENABLE_FOR_BIG;
	}
	if(robot_id == SMALL_ROBOT)
	{
		zones[i].events_for_small = events;
		zones[i].enable |= ZONE_ENABLE_FOR_SMALL;
	}
	zones[i].events = zones[i].events_for_big | zones[i].events_for_small;
}

void ZONE_disable(robot_id_e robot_id, zone_e i)
{
	if(robot_id == BIG_ROBOT)
	{
		zones[i].events_for_big = EVENT_NO_EVENT;
		zones[i].enable &= ~ZONE_ENABLE_FOR_BIG;
	}
	if(robot_id == SMALL_ROBOT)
	{
		zones[i].events_for_small = EVENT_NO_EVENT;
		zones[i].enable &= ~ZONE_ENABLE_FOR_SMALL;
	}
	if(zones[i].enable != ZONE_NOT_ENABLED)
		debug_printf("WARNING : l'un des robots a désactivé une zone que l'autre avait aussi activé. Attention, elle reste activé pour l'autre.. !!!\n");
	zones[i].events = zones[i].events_for_big | zones[i].events_for_small;
}

void ZONE_disable_all(void)
{
	Uint8 i;
	for(i=0;i<ZONE_NUMBER;i++)
	{
		zones[i].events_for_big = EVENT_NO_EVENT;
		zones[i].events_for_big = EVENT_NO_EVENT;
		zones[i].enable = ZONE_NOT_ENABLED;
	}
}

void ZONE_set_datas_updated(void)
{
	datas_updated = TRUE;
}

static void ZONE_init_fields(zone_e i, Uint16 x_min, Uint16 x_max, Uint16 y_min, Uint16 y_max, time32_t min_detection_time, specific_analyse_mode_e specific_analyse_mode){
	zones_t * z;
	z = &zones[i];

	z->x_min = x_min;
	z->x_max = x_max;
	z->y_min = y_min;
	z->y_max = y_max;
	z->enable = ZONE_NOT_ENABLED;
	z->specific_analyse_mode = specific_analyse_mode;
	z->events = EVENT_NO_EVENT;
	z->min_detection_time = min_detection_time;
	z->someone_is_here = FALSE;
	z->input_time = 0;
	z->presence_duration = 0;
	z->busy_state = NOBODY_IN_ZONE;
	if(specific_analyse_mode == FRESCO_LOCATOR)
		z->specific_analyse_param_x = 500;
	else
		z->specific_analyse_param_x = 0;
	z->specific_analyse_param_y = 0;
	z->alert_was_send = FALSE;
	z->initialized = TRUE;
}

void ZONE_clean_all_detections(void)
{
	Uint8 i;
	zones_t * z;
	for(i=0;i<ZONE_NUMBER;i++)
	{
		z = &zones[i];
		z->busy_state = NOBODY_IN_ZONE;
		z->someone_is_here = FALSE;
		z->alert_was_send = FALSE;
		z->presence_duration = 0;
		z->input_time = 0;
		z->specific_analyse_param_x = 0;
		z->specific_analyse_param_y = 0;
		if(z->specific_analyse_mode == FRESCO_LOCATOR)
			z->specific_analyse_param_x = 500;
		else
			z->specific_analyse_param_x = 0;
		z->specific_analyse_param_y = 0;
	}
}


zones_t * ZONE_get_pzone(zone_e i)
{
	assert(i<ZONE_NUMBER);
	return &zones[i];
}




#define CENTER_HEART_X	1100//[mm] Centre du foyer central en X
#define CENTER_HEART_Y	1500//[mm] Centre du foyer central en Y

/*static void ZONE_specific_analyse(HOKUYO_adversary_position * a, zones_t * z)
{
	//Analyse spécifique de ce que fait l'objet dans la zone...
	//PRECONDITION : on sait que l'objet *pa est dans la zone *pz
	//Analyse des zones activées et occupées
	switch (z->specific_analyse_mode)
	{
		case NORMAL_MODE:
			break;
		case FRESCO_LOCATOR:
			//On sauvegarde la coordonnée du point le plus faible en X...
			//Cette coordonnée sera envoyée lorsque l'objet quittera la zone
			if(a->coordX < z->specific_analyse_param_x)
			{
				z->specific_analyse_param_x = a->coordX;
				z->specific_analyse_param_y = a->coordY;
			}
			break;
		case DISPOSE_ON_HEART_POSITION_LOCATOR:
			//On recherche la coordonnée la plus proche du centre (on suppose que le robot pose là où il est le plus prêt du centre !)
			if(		CALCULATOR_squared_distance(a->coordX, a->coordY, CENTER_HEART_X, CENTER_HEART_Y) 		<
					CALCULATOR_squared_distance(z->specific_analyse_param_x, z->specific_analyse_param_y, CENTER_HEART_X, CENTER_HEART_Y))
			{
				z->specific_analyse_param_x = a->coordX;
				z->specific_analyse_param_y = a->coordY;
			}
			break;
		default :
			break;
	}

}

bool_e ZONE_object_is_in_zone(HOKUYO_adversary_position * a, zones_t * z)
{
	if(	a->coordX/10>=z->x_min &&
		a->coordX/10<=z->x_max &&
		a->coordY/10>=z->y_min &&
		a->coordY/10<=z->y_max )
		return TRUE;
	return FALSE;
}


void ZONE_detection(void)
{
	zone_e i;
	Uint8 j;
	for (i=0;i<ZONE_NUMBER;i++) 							//Regarde toutes les zones
	{
		zones[i].someone_is_here=FALSE;		 			//On suppose qu'il n'y a personne dans la zone activée
		if (zones[i].enable != ZONE_NOT_ENABLED)			//La zones est activée
		{
			for(j=0;j<HOKUYO_get_adversaries_number();j++) 	//Pour chaque objet
			{
				if(ZONE_object_is_in_zone(HOKUYO_get_adversary_position(j), &zones[i]))
				{
					zones[i].someone_is_here=TRUE;          //La zone est occupée par un des robots
					if(zones[i].events & EVENT_SPECIAL)
						ZONE_specific_analyse(HOKUYO_get_adversary_position(j),&zones[i]);		//Analyse fine des coordonnées de l'objet dans la zone.
					//On continue de la boucle (si plusieurs objets sont dans la zone, l'analyse des coordonnées doit se faire pour chaque objets).
				}
			}
		}
	}
}
*/

static void ZONE_analysis(void)
{
	zone_e i;
	zones_t * z;

	for (i=0;i<ZONE_NUMBER;i++)
	{
		z = &zones[i];	//Pointeur vers la zone traitée.

		switch(z->busy_state)
		{
			case NOBODY_IN_ZONE:
				if(z->someone_is_here)
				{
					z->busy_state = SOMEONE_IS_IN;
					z->input_time = absolute_time;
					if(z->events & EVENT_GET_IN)
						SECRETARY_set_event(i, EVENT_GET_IN);
					ZONE_verbose(i);
				}
				break;
			case SOMEONE_IS_IN:							//Un ou des objets sont	 	actuellement dans la zone
				if(!z->someone_is_here)
					z->busy_state = SOMEONE_GET_OUT;
				else
				{
					z->presence_duration = absolute_time - z->input_time;
					//L'évènement de temps de présence n'est valable que si min_detectection_time est non nul...
					if(z->min_detection_time != 0 && z->presence_duration > z->min_detection_time)	//Depuis qu'il est entré, il a passé min_detection_time dans la zone
					{
						z->busy_state = SOMEONE_IS_IN_SINCE_MIN_DETECTION_TIME;
						ZONE_verbose(i);
						if(z->events & EVENT_TIME_IN)
							SECRETARY_set_event(i, EVENT_TIME_IN);
					}
				}
				break;
			case SOMEONE_IS_IN_SINCE_MIN_DETECTION_TIME:	//Un ou des objets sont 	actuellement dans la zone depuis au moins le temps mini de détection
				if(!z->someone_is_here)
					z->busy_state = SOMEONE_GET_OUT;
				break;
			case SOMEONE_GET_OUT:						//Un ou des objets sortent 	actuellement de la zone
				ZONE_verbose(i);
				if(z->events & EVENT_GET_OUT)
					SECRETARY_set_event(i, EVENT_GET_OUT);
				//Dans les modes spécifiques, on envoit l'info mesurée lors de la sortie...donc ici...
				z->busy_state = NOBODY_IN_ZONE;
				break;
			default:
				z->busy_state = NOBODY_IN_ZONE;
				break;
		}
	}
}

#define TEXTE_SIZE	40
static void ZONE_verbose(zone_e i)
{
	zones_t * z;
	char  str[TEXTE_SIZE];
	Uint8 index = 0;
	UNUSED_VAR(index);
	z = &zones[i];	//Pointeur vers la zone traitée.
	switch(z->busy_state)
	{
		case SOMEONE_IS_IN:
			index = sprintf(str,"IN  :");
			break;
		case SOMEONE_IS_IN_SINCE_MIN_DETECTION_TIME:
			index = sprintf(str,"TIME:");
			break;
		case SOMEONE_GET_OUT:
			index = sprintf(str,"OUT :");
			break;
		default:
			break;
	}

	switch (i){
//		case ZONE_FRESQUO:								sprintf(str+index,"FRESQUO");		break;
		default:																			break;
	}
	debug_printf("%s\n",str);
}





