/*
 *	Club Robot ESEO 2009 - 2010
 *	PACMAN
 *
 *	Fichier : detection.c
 *	Package : Carte Propulsion
 *	Description : Traitement des informations pour détection
 *	Auteur : Jacen (Modifié par Ronan)  / Nirgal
 *	Version 201406
 */

#define DETECTION_C
#include "detection.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can.h"
#include "QS/QS_timer.h"
#include "QS/QS_maths.h"
#include "QS/QS_who_am_i.h"
#include "../config/config_pin.h"

volatile static Uint32 hokuyo_update_time = 0;

#define BEACON_MAX_FOES			2
#define FOE_DATA_LIFETIME		250		//[ms] Durée de vie des données envoyées par la propulsion
#define IR_FOE_DATA_LIFETIME	1000	//[ms] Durée de vie des données envoyées par la balise
#define PROTECTION_GAP			40

adversary_t adversaries[HOKUYO_MAX_FOES + BEACON_MAX_FOES];	//Ce tableau se construit progressivement, quand on a toutes les données, on peut les traiter et renseigner le tableau des positions adverses.
volatile Uint8 hokuyo_objects_number = 0;	//Nombre d'objets hokuyo

#define NB_POINTS_BY_ZONE 		(4)
typedef struct{
	GEOMETRY_point_t polygon[NB_POINTS_BY_ZONE];
	bool_e enable;
}detection_zone_s;

typedef enum{
	START_ZONE_BLUE,
	NEXT_TO_START_ZONE_BLUE,
	START_ZONE_YELLOW,
	NEXT_TO_START_ZONE_YELLOW,
	MOONBASE_BLUE_SIDE,
	MOONBASE_MIDDLE_BLUE_SIDE,
	MOONBASE_MIDDLE_YELLOW_SIDE,
	MOONBASE_YELLOW_SIDE,
	FILTER_ZONE_FOR_BIG,
	FILTER_ZONE_FOR_SMALL,
	NB_DETECTION_ZONES
}detection_zone_id_e;

static const GEOMETRY_point_t out_point = {1500, 2500};

detection_zone_s zones_hokuyo[9]={{{(GEOMETRY_point_t){0, 0}, (GEOMETRY_point_t){360, 0}, (GEOMETRY_point_t){360, 710},(GEOMETRY_point_t){0, 710}}, FALSE}, 				// Zone de départ bleu
								 {{(GEOMETRY_point_t){360, 0}, (GEOMETRY_point_t){700, 0}, (GEOMETRY_point_t){700, 710},(GEOMETRY_point_t){360, 710}}, FALSE}, 			// Zone à côté de la zone de départ bleu
								 {{(GEOMETRY_point_t){0, 2290}, (GEOMETRY_point_t){360, 2290}, (GEOMETRY_point_t){360, 3000},(GEOMETRY_point_t){0, 3000}}, FALSE}, 		// Zone de départ jaune
								 {{(GEOMETRY_point_t){360, 2290}, (GEOMETRY_point_t){700, 2290}, (GEOMETRY_point_t){700, 3000},(GEOMETRY_point_t){360, 3000}}, FALSE}, 	// Zone à côté de la zone de départ jaune

								 {{(GEOMETRY_point_t){1350, 850}, (GEOMETRY_point_t){2000, 850}, (GEOMETRY_point_t){2000, 1300},(GEOMETRY_point_t){1850, 1350}}, FALSE}, // Zone de base lunaire côté bleu
								 {{(GEOMETRY_point_t){1350, 850}, (GEOMETRY_point_t){1850, 1350}, (GEOMETRY_point_t){1800, 1500},(GEOMETRY_point_t){1100, 1500}}, FALSE},// Zone de base lunaire milieu côté bleu
								 {{(GEOMETRY_point_t){1350, 2150}, (GEOMETRY_point_t){1850, 1650}, (GEOMETRY_point_t){1800, 1500},(GEOMETRY_point_t){1100, 1500}}, FALSE},// Zone de base lunaire milieu côté jaune
								 {{(GEOMETRY_point_t){1350, 2150}, (GEOMETRY_point_t){2000, 2150}, (GEOMETRY_point_t){2000, 1700},(GEOMETRY_point_t){1850, 1650}}, FALSE}, // Zone de base lunaire côté bleu

								 // Zone filtré pour le gros robot
								 {{(GEOMETRY_point_t){400, 0}, (GEOMETRY_point_t){1150, 0}, (GEOMETRY_point_t){1150, 1000},(GEOMETRY_point_t){400, 1000}}, FALSE},

								 // Zone filtré pour le petit robot
								 {{(GEOMETRY_point_t){1150, 0}, (GEOMETRY_point_t){2000, 0}, (GEOMETRY_point_t){2000, 1000},(GEOMETRY_point_t){1150, 1000}}, FALSE},

								};

detection_zone_s zones_balise[9]={{{(GEOMETRY_point_t){0, 0}, (GEOMETRY_point_t){360, 0}, (GEOMETRY_point_t){360, 710},(GEOMETRY_point_t){0, 710}}, FALSE}, 				// Zone de départ bleu
								 {{(GEOMETRY_point_t){360, 0}, (GEOMETRY_point_t){700, 0}, (GEOMETRY_point_t){700, 710},(GEOMETRY_point_t){360, 710}}, FALSE}, 			// Zone à côté de la zone de départ bleu
								 {{(GEOMETRY_point_t){0, 2290}, (GEOMETRY_point_t){360, 2290}, (GEOMETRY_point_t){360, 3000},(GEOMETRY_point_t){0, 3000}}, FALSE}, 		// Zone de départ jaune
								 {{(GEOMETRY_point_t){360, 2290}, (GEOMETRY_point_t){700, 2290}, (GEOMETRY_point_t){700, 3000},(GEOMETRY_point_t){360, 3000}}, FALSE}, 	// Zone à côté de la zone de départ jaune

								 {{(GEOMETRY_point_t){1350, 850}, (GEOMETRY_point_t){2000, 850}, (GEOMETRY_point_t){2000, 1300},(GEOMETRY_point_t){1850, 1350}}, FALSE}, // Zone de base lunaire côté bleu
								 {{(GEOMETRY_point_t){1350, 850}, (GEOMETRY_point_t){1850, 1350}, (GEOMETRY_point_t){1800, 1500},(GEOMETRY_point_t){1100, 1500}}, FALSE},// Zone de base lunaire milieu côté bleu
								 {{(GEOMETRY_point_t){1350, 2150}, (GEOMETRY_point_t){1850, 1650}, (GEOMETRY_point_t){1800, 1500},(GEOMETRY_point_t){1100, 1500}}, FALSE},// Zone de base lunaire milieu côté jaune
								 {{(GEOMETRY_point_t){1350, 2150}, (GEOMETRY_point_t){2000, 2150}, (GEOMETRY_point_t){2000, 1700},(GEOMETRY_point_t){1850, 1650}}, FALSE}, // Zone de base lunaire côté bleu

								 // Zone filtré pour le gros robot
								 {{(GEOMETRY_point_t){400, 0}, (GEOMETRY_point_t){1150, 0}, (GEOMETRY_point_t){1150, 1000},(GEOMETRY_point_t){400, 1000}}, FALSE},

								 // Zone filtré pour le petit robot
								{{(GEOMETRY_point_t){1150, 0}, (GEOMETRY_point_t){2000, 0}, (GEOMETRY_point_t){2000, 1000},(GEOMETRY_point_t){1150, 1000}}, FALSE},

								};


void DETECTION_init(void)
{
	Uint8 i;
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	hokuyo_objects_number = 0;
	for(i = 0; i < HOKUYO_MAX_FOES+BEACON_MAX_FOES; i++)
	{
		adversaries[i].updated = FALSE;
		adversaries[i].enable = FALSE;
		adversaries[i].update_time = (Uint32)0;
	}
	initialized = TRUE;
}


/*	mise à jour de l'information de détection avec le contenu
	courant de l'environnement */
void DETECTION_process_main(void)
{
	//On reactive les balises toutes les 90sec
	static Uint32 next_beacon_activate_msg = 1000;	//Prochain instant d'envoi de message.
	if(global.absolute_time > next_beacon_activate_msg)
	{
		next_beacon_activate_msg += 90000;
		CAN_send_sid(BEACON_ENABLE_PERIODIC_SENDING);
	}
}

#define BEACON_IR_SIZE_FILTER 3
static Sint16 beacon_ir_distance_filter(bool_e enable, Uint8 foe_id, Sint16 new_distance)
{
	static Uint8 index[BEACON_MAX_FOES];
	static Uint8 nb_datas[BEACON_MAX_FOES];
	static Sint16 previous_distances[BEACON_MAX_FOES][BEACON_IR_SIZE_FILTER];
	Uint8 i;
	Sint32 sum;
	if(foe_id < BEACON_MAX_FOES)
	{
		if(enable)
		{
			assert(index[foe_id] < BEACON_IR_SIZE_FILTER);
			previous_distances[foe_id][index[foe_id]] = new_distance;	//Ajout de la nouvelle distance dans le tableau de filtrage
			if(nb_datas[foe_id] < BEACON_IR_SIZE_FILTER)
				nb_datas[foe_id]++;							//le nombre de données est entre 1 et 3
			index[foe_id] = (index[foe_id]<BEACON_IR_SIZE_FILTER-1)?index[foe_id]+1:0;	//l'index de la prochaine donnée à écrire
			sum = 0;
			for(i=0;i<nb_datas[foe_id];i++)
			{
				sum += previous_distances[foe_id][i];
			}
			assert(nb_datas[foe_id] != 0);	//n'est jamais sensé se produire.
			return (Sint16)(sum/nb_datas[foe_id]);	//on renvoie la moyenne des distances enregistrées dans le tableau
		}
		else
		{
			index[foe_id] = 0;
			nb_datas[foe_id] = 0;
			return new_distance;
		}
	}
	else
		return 0;	//should never happen.
}




// Doit être appelé en FIN d'IT... (les flags updated y sont baissés)
void DETECTION_process_it(void)
{
	Uint8 i;

	for(i = 0; i < HOKUYO_MAX_FOES + BEACON_MAX_FOES; i++)
	{
		adversaries[i].updated = FALSE;
		//on peut descendre ce flag ici grace au fait que la fonction qui remplit le tableau "adversaries" est non préemptible.
		//(et donc l'IT qui suivra et appelera ce DETECTION_process_it aura pu voir les données cohérentes et complètes).

		if(i < HOKUYO_MAX_FOES){
			if((adversaries[i].enable) && (global.absolute_time - adversaries[i].update_time > FOE_DATA_LIFETIME))
				adversaries[i].enable = FALSE;
		}else{
			if((adversaries[i].enable) && (global.absolute_time - adversaries[i].update_time > IR_FOE_DATA_LIFETIME))
				adversaries[i].enable = FALSE;
		}
	}

}


/*
 * Déclaration des détections adverses :
 * 	- Via un message CAN (sinon, msg = NULL)
 * 	- Via une structure HOKUYO_adversary_position et adv_nb (sinon adv = NULL)
 */
void DETECTION_new_adversary_position(CAN_msg_t * msg, HOKUYO_adversary_position * adv, Uint8 adv_nb)
{
	Uint8 i, adversary_nb, fiability;
	Sint16 cosinus, sinus, our_cos, our_sin;
	Uint8 j; // Index pour les zones de detection

	//Section critique. le tableau adversaries ne peut pas être consulté en IT pendant sa modification ici..
	//Pour éviter cette section critique qui est "un peu" longue... il faudrait simplement mémoriser en tâche de fond msg et adv... et les traiter en IT.
	TIMER2_disableInt();

	if(msg != NULL)
	{
		if (msg->sid == BROADCAST_ADVERSARIES_POSITION)
		{
			adversary_nb = msg->data.broadcast_adversaries_position.adversary_number;
			if(adversary_nb < HOKUYO_MAX_FOES)
			{
				fiability = msg->data.broadcast_adversaries_position.fiability;
				if(fiability)
				{
					adversaries[adversary_nb].enable = TRUE;
					adversaries[adversary_nb].update_time = global.absolute_time;
				}
				else
					adversaries[adversary_nb].enable = FALSE;
				if(fiability & ADVERSARY_DETECTION_FIABILITY_X)
					adversaries[adversary_nb].x = ((Sint16)msg->data.broadcast_adversaries_position.x)*20;
				if(fiability & ADVERSARY_DETECTION_FIABILITY_Y)
					adversaries[adversary_nb].y = ((Sint16)msg->data.broadcast_adversaries_position.y)*20;
				if(fiability)
				{
					if(fiability & ADVERSARY_DETECTION_FIABILITY_TETA)
						adversaries[adversary_nb].angle = msg->data.broadcast_adversaries_position.teta;
					else	//je dois calculer moi-même l'angle de vue relatif de l'adversaire
					{
						adversaries[adversary_nb].angle = GEOMETRY_viewing_angle(global.position.x, global.position.y,adversaries[adversary_nb].x, adversaries[adversary_nb].y);
						adversaries[adversary_nb].angle = GEOMETRY_modulo_angle(adversaries[adversary_nb].angle - global.position.teta);
					}
					if(fiability & ADVERSARY_DETECTION_FIABILITY_DISTANCE)
						adversaries[adversary_nb].dist = ((Sint16)msg->data.broadcast_adversaries_position.dist)*20;
					else	//je dois calculer moi-même la distance de l'adversaire
						adversaries[adversary_nb].dist = GEOMETRY_distance(	(GEOMETRY_point_t){global.position.x, global.position.y},
																				(GEOMETRY_point_t){adversaries[adversary_nb].x, adversaries[adversary_nb].y}
																				);
				}
				if(msg->data.broadcast_adversaries_position.last_adversary)
				{
					if(adversary_nb == 0 && !fiability)
						hokuyo_objects_number = 0;				//On a des données, qui nous disent qu'aucun adversaire n'est vu...
					else
						hokuyo_objects_number = adversary_nb + 1;
				}
			}adversary_nb = msg->data.broadcast_adversaries_position.adversary_number;
		}
		else if(msg->sid == BROADCAST_BEACON_ADVERSARY_POSITION_IR)
		{
#ifndef	DISABLED_BALISE_AVOIDANCE
			for(i = 0; i<BEACON_MAX_FOES; i++)
			{

				if((msg->data.broadcast_beacon_adversary_position_ir.adv[i].error & ~TACHE_TROP_GRANDE) == AUCUNE_ERREUR){

					//Extraction des données du msg.
					adversaries[HOKUYO_MAX_FOES+i].fiability_error = msg->data.broadcast_beacon_adversary_position_ir.adv[i].error;

					adversaries[HOKUYO_MAX_FOES+i].angle = GEOMETRY_modulo_angle((Sint16)(msg->data.broadcast_beacon_adversary_position_ir.adv[i].angle));
					adversaries[HOKUYO_MAX_FOES+i].dist = (Uint16)(msg->data.broadcast_beacon_adversary_position_ir.adv[i].dist)*20;

					//enable ou pas ?
					adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
					if	(global.absolute_time - hokuyo_update_time > FOE_DATA_LIFETIME
							|| 	(		adversaries[HOKUYO_MAX_FOES+i].angle < -(PI4096/2-PI4096/3)		//Angle entre 30° et 150° --> angle mort hokuyo + marge de 15° de chaque coté.
									&& 	adversaries[HOKUYO_MAX_FOES+i].angle > -(PI4096/2+PI4096/3) )
						)
					{
						adversaries[HOKUYO_MAX_FOES+i].enable = TRUE;
						adversaries[HOKUYO_MAX_FOES+i].updated = TRUE;
					}

					//Traitement et correction des données
					if(adversaries[HOKUYO_MAX_FOES+i].fiability_error & TACHE_TROP_GRANDE)
						adversaries[HOKUYO_MAX_FOES+i].dist = 250;	//Lorsque l'on reçoit l'erreur TACHE TROP GRANDE, la distance est fausse, mais l'adversaire est probablement très proche. On impose 25cm.

					//filtrage de la distance
					adversaries[HOKUYO_MAX_FOES+i].dist = beacon_ir_distance_filter(adversaries[HOKUYO_MAX_FOES+i].enable,i,adversaries[HOKUYO_MAX_FOES+i].dist);

					//Calcul x et y
					COS_SIN_4096_get(adversaries[HOKUYO_MAX_FOES+i].angle, &cosinus, &sinus);
					COS_SIN_4096_get(global.position.teta, &our_cos, &our_sin);
					adversaries[HOKUYO_MAX_FOES+i].x = global.position.x + (adversaries[HOKUYO_MAX_FOES+i].dist * ((float){((Sint32)(cosinus) * (Sint32)(our_cos) - (Sint32)(sinus) * (Sint32)(our_sin))}/(4096*4096)));
					adversaries[HOKUYO_MAX_FOES+i].y = global.position.y + (adversaries[HOKUYO_MAX_FOES+i].dist * ((float){((Sint32)(cosinus) * (Sint32)(our_sin) + (Sint32)(sinus) * (Sint32)(our_cos))}/(4096*4096)));

					GEOMETRY_point_t pos;
					pos.x = global.position.x;
					pos.y = global.position.y;
					GEOMETRY_point_t adv_pos;
					adv_pos.x = adversaries[HOKUYO_MAX_FOES+i].x;
					adv_pos.y = adversaries[HOKUYO_MAX_FOES+i].y;

					if(is_in_polygon(zones_hokuyo[START_ZONE_BLUE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
						if(is_in_polygon(zones_hokuyo[NEXT_TO_START_ZONE_BLUE].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
					}

					if(is_in_polygon(zones_hokuyo[NEXT_TO_START_ZONE_BLUE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
						if(is_in_polygon(zones_hokuyo[START_ZONE_BLUE].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
					}

					if(is_in_polygon(zones_hokuyo[START_ZONE_YELLOW].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
						if(is_in_polygon(zones_hokuyo[NEXT_TO_START_ZONE_YELLOW].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
					}

					if(is_in_polygon(zones_hokuyo[NEXT_TO_START_ZONE_YELLOW].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
						if(is_in_polygon(zones_hokuyo[START_ZONE_YELLOW].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
					}

					if(is_in_polygon(zones_hokuyo[MOONBASE_BLUE_SIDE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
						if(is_in_polygon(zones_hokuyo[MOONBASE_MIDDLE_BLUE_SIDE].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
					}

					if(is_in_polygon(zones_hokuyo[MOONBASE_MIDDLE_BLUE_SIDE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
						if(is_in_polygon(zones_hokuyo[MOONBASE_BLUE_SIDE].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
						if(is_in_polygon(zones_hokuyo[MOONBASE_MIDDLE_YELLOW_SIDE].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
					}

					if(is_in_polygon(zones_hokuyo[MOONBASE_MIDDLE_YELLOW_SIDE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
						if(is_in_polygon(zones_hokuyo[MOONBASE_MIDDLE_BLUE_SIDE].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
						if(is_in_polygon(zones_hokuyo[MOONBASE_YELLOW_SIDE].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
					}

					if(is_in_polygon(zones_hokuyo[MOONBASE_YELLOW_SIDE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
						if(is_in_polygon(zones_hokuyo[MOONBASE_MIDDLE_YELLOW_SIDE].polygon, NB_POINTS_BY_ZONE, adv_pos, out_point, NULL)){
							adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
							adversaries[HOKUYO_MAX_FOES+i].updated = FALSE;
						}
					}

					//Mise à jour de l'update_time
					adversaries[HOKUYO_MAX_FOES+i].update_time = global.absolute_time;
				}
			}
#endif
		}
	}
	if(adv != NULL)
	{
		hokuyo_objects_number = adv_nb;
		hokuyo_update_time = global.absolute_time;
		if(adv_nb > 0)
		{
			for(i = 0; i < adv_nb ; i++)
			{
				adversaries[i].enable = TRUE;
				adversaries[i].updated = TRUE;
				adversaries[i].update_time = global.absolute_time;
				adversaries[i].x = adv[i].coordX;
				adversaries[i].y = adv[i].coordY;
				adversaries[i].angle = adv[i].teta;
				adversaries[i].dist = adv[i].dist;
			}
			for(i = adv_nb ; i < HOKUYO_MAX_FOES ; i++)
			{
				adversaries[i].enable = FALSE;
			}
		}
	}

	for(i=0;i<HOKUYO_MAX_FOES + BEACON_MAX_FOES;i++)
	{
		if(adversaries[i].enable) //Pour tout les adversaires observés...
		{
			GEOMETRY_point_t pos;
			pos.x=global.position.x;
			pos.y=global.position.y;

			for(j = 0; j < NB_DETECTION_ZONES; j++){
				zones_balise[j].enable = FALSE;
			}

			if(is_in_polygon(zones_balise[START_ZONE_BLUE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
				zones_balise[NEXT_TO_START_ZONE_BLUE].enable = TRUE;
			}
			if(is_in_polygon(zones_balise[NEXT_TO_START_ZONE_BLUE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
				zones_balise[START_ZONE_BLUE].enable = TRUE;
			}
			if(is_in_polygon(zones_balise[START_ZONE_YELLOW].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
				zones_balise[NEXT_TO_START_ZONE_YELLOW].enable = TRUE;
			}
			if(is_in_polygon(zones_balise[NEXT_TO_START_ZONE_YELLOW].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
				zones_balise[START_ZONE_YELLOW].enable = TRUE;
			}

			if(is_in_polygon(zones_balise[MOONBASE_BLUE_SIDE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
				zones_balise[MOONBASE_MIDDLE_BLUE_SIDE].enable = TRUE;
			}
			if(is_in_polygon(zones_balise[MOONBASE_MIDDLE_BLUE_SIDE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
				zones_balise[MOONBASE_BLUE_SIDE].enable = TRUE;
				zones_balise[MOONBASE_MIDDLE_YELLOW_SIDE].enable = TRUE;
			}
			if(is_in_polygon(zones_balise[MOONBASE_MIDDLE_YELLOW_SIDE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
				zones_balise[MOONBASE_YELLOW_SIDE].enable = TRUE;
				zones_balise[MOONBASE_MIDDLE_BLUE_SIDE].enable = TRUE;
			}
			if(is_in_polygon(zones_balise[MOONBASE_YELLOW_SIDE].polygon, NB_POINTS_BY_ZONE, pos, out_point, NULL)){
				zones_balise[MOONBASE_MIDDLE_YELLOW_SIDE].enable = TRUE;
			}

			GEOMETRY_point_t p;
			p.x=adversaries[i].x;
			p.y=adversaries[i].y;
			if(is_in_zone_transparency_balise(p))/*(GEOMETRY_point_t){adversaries[i].x, adversaries[i].y})))*/
				adversaries[i].enable = FALSE; //On désactive cet adversaire... Soit c'est l'autre robot, soit c'est un ennemi dont on est protégé par l'autre robot ou un obstacle fixe.
		}
	}

	/*debug_printf("Adv 1 : ");
	if(adversaries[HOKUYO_MAX_FOES].enable == TRUE){
		debug_printf("\nx : %d\ny : %d\n", adversaries[HOKUYO_MAX_FOES].x, adversaries[HOKUYO_MAX_FOES].y);
		debug_printf("t : %d\nd : %d\n", adversaries[HOKUYO_MAX_FOES].angle*180/PI4096, adversaries[HOKUYO_MAX_FOES].dist);
	}else
		debug_printf("disabled\n");

	debug_printf("Adv 2 : ");
	if(adversaries[HOKUYO_MAX_FOES+1].enable == TRUE){
		debug_printf("\nx : %d\ny : %d\n", adversaries[HOKUYO_MAX_FOES+1].x, adversaries[HOKUYO_MAX_FOES+1].y);
		debug_printf("t : %d\nd : %d\n", adversaries[HOKUYO_MAX_FOES+1].angle*180/PI4096, adversaries[HOKUYO_MAX_FOES+1].dist);
	}else
		debug_printf("disabled\n\n");*/

	TIMER2_enableInt();
	//Fin de la section critique
}


adversary_t * DETECTION_get_adversaries(Uint8 * size)
{
	*size = BEACON_MAX_FOES + HOKUYO_MAX_FOES;
	return adversaries;
}

bool_e is_in_zone_transparency_hokuyo(GEOMETRY_point_t p)
{
	Uint8 i=0;
	for(i = 0; i < NB_DETECTION_ZONES; i++){
		if(zones_hokuyo[i].enable && is_in_polygon(zones_hokuyo[i].polygon, NB_POINTS_BY_ZONE, p, out_point, NULL))
			return TRUE;
	}
	return FALSE;
}

void DETECTON_set_zone_transparency(Uint8 i, bool_e enable) //Pour l'hokuyo
{
	zones_hokuyo[i].enable = enable;
	zones_balise[i].enable = enable;
}

bool_e is_in_zone_transparency_balise(GEOMETRY_point_t p)
{
	Uint8 i=0;
	for(i = 0; i < NB_DETECTION_ZONES; i++){
		if(zones_balise[i].enable && is_in_polygon(zones_balise[i].polygon, NB_POINTS_BY_ZONE, p, out_point, NULL))
			return TRUE;
	}
	return FALSE;
}
