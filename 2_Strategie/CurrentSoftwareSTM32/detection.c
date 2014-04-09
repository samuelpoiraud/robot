/*
 *	Club Robot ESEO 2009 - 2010
 *	PACMAN
 *
 *	Fichier : detection.c
 *	Package : Carte Principale
 *	Description : Traitement des informations pour détection
 *	Auteur : Jacen (Modifié par Ronan)
 *	Version 20110417
 */

#define DETECTION_C
#include "detection.h"
#include "QS/QS_CANmsgList.h"
#include "can_utils.h"
#include "QS/QS_outputlog.h"
#include "config_use.h"
#include "config_debug.h"
#include "environment.h"
#include "maths_home.h"

typedef struct
{
	Sint16 angle;
	Sint16 dist;
	Sint16 x;
	Sint16 y;
	bool_e enable;					//cet objet est activé.
	bool_e enable_for_avoidance;	//Objet activé pour l'évitement
	bool_e enable_for_zoning;		//Objet acivé pour le zoning
	time32_t update_time;
}adversary_t;


typedef enum
{
	DETECTION_REASON_PROCESS_MAIN = 0,		//Process main...
	DETECTION_REASON_DATAS_RECEIVED_FROM_PROPULSION,
	DETECTION_REASON_DATAS_RECEIVED_FROM_BEACON_IR
}detection_reason_e;		//Raison de l'appel à la fonction DETECTION_compute

static void DETECTION_compute(detection_reason_e reason);

#define FOE_DATA_LIFETIME	250		//[ms] Durée de vie des données envoyées par la propulsion
volatile adversary_t hokuyo_objects[MAX_NB_FOES];	//Ce tableau se construit progressivement, quand on a toutes les données, on peut les traiter et renseigner le tableau des positions adverses.
volatile Uint8 hokuyo_objects_number = 0;	//Nombre d'objets hokuyo

volatile adversary_t beacon_ir_objects[2];

void DETECTION_init(void)
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	hokuyo_objects_number = 0;

}	

/*	mise à jour de l'information de détection avec le contenu
	courant de l'environnement */
void DETECTION_update(void)
{
	//Necessaire pour des match infini de test, on reactive les balises toutes les 90sec
	static time32_t next_beacon_activate_msg = 1000;	//Prochain instant d'envoi de message.
	if(!global.env.match_over && global.env.absolute_time > next_beacon_activate_msg)
	{
		next_beacon_activate_msg += 90000;
		CAN_send_sid(BEACON_ENABLE_PERIODIC_SENDING);
	}

	DETECTION_compute(DETECTION_REASON_PROCESS_MAIN);
}

//Cette fonction utilise les données accumulées... selon un algo très sophistiqué... et détermine les positions adverses.
static void DETECTION_compute(detection_reason_e reason)
{
	Uint8 i,j, j_min;
	Sint16 dist_min;
	bool_e objects_chosen[MAX_NB_FOES];
	static time32_t data_from_propulsion_update_time = 0;

	#warning "Algo non testé..."
	//On se contente de choisir les NB_FOES objets hokuyo les plus proches observés et de les enregistrer dans le tableau d'adversaires.
	//Si pas de données venant de la propulsion (hokuyo ou adversaire virtuel parfait) -> on prend les données IR.


	switch(reason)
	{
		case DETECTION_REASON_PROCESS_MAIN:
			for(i=0;i<MAX_NB_FOES;i++)
			{
				if((global.env.foe[i].obsolete == DETECTION_IS_RECENT) && (global.env.absolute_time - global.env.foe[i].update_time > FOE_DATA_LIFETIME))
					global.env.foe[i].obsolete = DETECTION_IS_GETTING_OBSOLETE;
				else if(global.env.foe[i].obsolete == DETECTION_IS_GETTING_OBSOLETE)
					global.env.foe[i].obsolete = DETECTION_IS_OBSOLETE;
			}
			break;
		case DETECTION_REASON_DATAS_RECEIVED_FROM_BEACON_IR:
			//Si je n'ai pas de données en provenance de la propulsion depuis un moment... j'utilise les données de la BEACON_IR.
			if	(global.env.absolute_time - data_from_propulsion_update_time > FOE_DATA_LIFETIME)
			{
				for(i=0;i<2;i++)
				{
					if(beacon_ir_objects[i].enable)	//Si les données sont cohérentes... (signal vu...)
					{
						global.env.foe[i].x 			= beacon_ir_objects[i].x;
						global.env.foe[i].y 			= beacon_ir_objects[i].y;
						global.env.foe[i].angle 		= beacon_ir_objects[i].angle;
						global.env.foe[i].dist 			= beacon_ir_objects[i].dist;
						global.env.foe[i].update_time 	= beacon_ir_objects[i].update_time;
						global.env.foe[i].updated 		= TRUE;
						global.env.foe[i].obsolete 		= DETECTION_IS_RECENT;
						global.env.foe[i].from 			= DETECTION_FROM_BEACON_IR;
					}
				}
			}
			break;
		case DETECTION_REASON_DATAS_RECEIVED_FROM_PROPULSION:		//Cette source d'info est prioritaire...
			data_from_propulsion_update_time = global.env.absolute_time;
			//debug_printf("Compute :");
			for(j = 0; j < hokuyo_objects_number; j++)
				objects_chosen[j] = FALSE;			//init, aucun des objets n'est choisi

			for(i = 0 ; i < MAX_NB_FOES ; i++)	//Pour chaque case du tableau d'adversaires qu'on doit remplir
			{
				dist_min = 0x7FFF;
				j_min = 0xFF;		//On suppose qu'il n'y a pas d'objet hokuyo.
				for(j = 0; j < hokuyo_objects_number; j++)	//Pour tout les objets hokuyos recus
				{
					if(hokuyo_objects[i].enable && objects_chosen[j] == FALSE && dist_min > hokuyo_objects[j].dist)	//Pour tout objet restant (activé, non choisi)
					{
						j_min = j;
						dist_min = hokuyo_objects[j].dist;		//On cherche la distance mini parmi les objet restant
					}
				}
				if(j_min != 0xFF)									//Si on a trouvé un objet
				{
					objects_chosen[j_min] = TRUE;					//On "consomme" cet objet
					global.env.foe[i].x 			= hokuyo_objects[j_min].x;	//On enregistre cet objet à la case i.
					global.env.foe[i].y 			= hokuyo_objects[j_min].y;
					global.env.foe[i].angle 		= hokuyo_objects[j_min].angle;
					global.env.foe[i].dist 			= hokuyo_objects[j_min].dist;
					global.env.foe[i].update_time 	= hokuyo_objects[j_min].update_time;
					global.env.foe[i].updated 		= TRUE;
					global.env.foe[i].obsolete 		= DETECTION_IS_RECENT;
					global.env.foe[i].from 			= DETECTION_FROM_PROPULSION;
					//debug_printf("%d:x=%4d\ty=%4d\ta=%5d\td=%4d\t|", i, hokuyo_objects[j_min].x, hokuyo_objects[j_min].y, hokuyo_objects[j_min].angle, hokuyo_objects[j_min].dist);
				}
				else
					global.env.foe[i].updated = FALSE;				//Plus d'objet dispo... on vide la case i.

				//TODO le tableau de foe devrait plutot contenir d'autres types d'infos utiles..... revoir leur type..
			}
			break;
		default:
			break;
	}




	//S'il y a plus d'objets hokuyo que d'adversaires possibles dans notre évitement, on choisit les plus proches.
	//debug_printf("\n");
}


/* Message can recu avec des infos concernant les adversaires... */
void DETECTION_pos_foe_update (CAN_msg_t* msg)
{
	//bool_e slashn;
	Uint8 fiability;
	Uint8 adversary_nb, i;
	Sint16 cosinus, sinus;
	switch(msg->sid)
	{
		case STRAT_ADVERSARIES_POSITION:
			adversary_nb = msg->data[0] & (~IT_IS_THE_LAST_ADVERSARY);
			if(adversary_nb < MAX_NB_FOES)
			{
				fiability = msg->data[6];
				if(fiability)
				{
					hokuyo_objects[adversary_nb].enable = TRUE;
					hokuyo_objects[adversary_nb].update_time = global.env.absolute_time;
				}
				else
					hokuyo_objects[adversary_nb].enable = FALSE;
				if(fiability & ADVERSARY_DETECTION_FIABILITY_X)
					hokuyo_objects[adversary_nb].x = ((Sint16)msg->data[1])*20;
				if(fiability & ADVERSARY_DETECTION_FIABILITY_Y)
					hokuyo_objects[adversary_nb].y = ((Sint16)msg->data[2])*20;
				if(fiability)
				{
					if(ADVERSARY_DETECTION_FIABILITY_TETA)
						hokuyo_objects[adversary_nb].angle = (Sint16)(U16FROMU8(msg->data[3],msg->data[4]));
					else	//je dois calculer moi-même l'angle de vue relatif de l'adversaire
					{
						hokuyo_objects[adversary_nb].angle = GEOMETRY_viewing_angle(global.env.pos.x, global.env.pos.y,hokuyo_objects[adversary_nb].x, hokuyo_objects[adversary_nb].y);
						hokuyo_objects[adversary_nb].angle = GEOMETRY_modulo_angle(hokuyo_objects[adversary_nb].angle - global.env.pos.angle);
					}
					if(ADVERSARY_DETECTION_FIABILITY_DISTANCE)
						hokuyo_objects[adversary_nb].dist = ((Sint16)msg->data[5])*20;
					else	//je dois calculer moi-même la distance de l'adversaire
						hokuyo_objects[adversary_nb].dist = GEOMETRY_distance(	(GEOMETRY_point_t){global.env.pos.x, global.env.pos.y},
																				(GEOMETRY_point_t){hokuyo_objects[adversary_nb].x, hokuyo_objects[adversary_nb].y}
																				);
				}

			}
			if(msg->data[0] & IT_IS_THE_LAST_ADVERSARY)
			{
				if(adversary_nb == 0 && !fiability)
					hokuyo_objects_number = 0;				//On a des données, qui nous disent qu'aucun adversaire n'est vu...
				else
					hokuyo_objects_number = adversary_nb + 1;
				DETECTION_compute(DETECTION_REASON_DATAS_RECEIVED_FROM_PROPULSION);
			}
			break;
		case BEACON_ADVERSARY_POSITION_IR:

			for(i = 0; i<2; i++)
			{
				beacon_ir_objects[i].angle = U16FROMU8(msg->data[1+4*i],msg->data[2+4*i]);
				beacon_ir_objects[i].dist = (Uint16)(msg->data[3+4*i])*10;
				beacon_ir_objects[i].update_time = global.env.absolute_time;
				COS_SIN_4096_get(beacon_ir_objects[i].angle, &cosinus, &sinus);
				beacon_ir_objects[i].x = global.env.pos.x + (beacon_ir_objects[i].dist * (cosinus * global.env.pos.cosAngle - sinus * global.env.pos.sinAngle));
				beacon_ir_objects[i].y = global.env.pos.y + (beacon_ir_objects[i].dist * (cosinus * global.env.pos.sinAngle + sinus * global.env.pos.cosAngle));

				if((msg->data[0+4*i] & ~SIGNAL_INSUFFISANT) == AUCUNE_ERREUR)	//Si je n'ai pas d'autre erreur que SIGNAL_INSUFFISANT... c'est bon
					beacon_ir_objects[i].enable = TRUE;
				else
					beacon_ir_objects[i].enable = FALSE;
			}
			if(beacon_ir_objects[0].enable || beacon_ir_objects[1].enable)
				DETECTION_compute(DETECTION_REASON_DATAS_RECEIVED_FROM_BEACON_IR);	//Si l'un des deux adversaires à été vu... on prévient l'algo COMPUTE.




		/*	slashn = FALSE;
			if((msg->data[0] & 0xFE) == AUCUNE_ERREUR)	//Si l'octet de fiabilité vaut SIGNAL_INSUFFISANT, on le laisse passer quand même
			{
				//slashn = TRUE;
				global.env.sensor[BEACON_IR_FOE_1].angle = U16FROMU8(msg->data[1],msg->data[2]);
				// Pour gérer l'inversion de la balise
				//global.env.sensor[BEACON_IR_FOE_1].angle += (global.env.sensor[BEACON_IR_FOE_1].angle > 0)?-PI4096:PI4096;
				global.env.sensor[BEACON_IR_FOE_1].distance = (Uint16)(msg->data[3])*10;
				global.env.sensor[BEACON_IR_FOE_1].update_time = global.env.match_time;
				global.env.sensor[BEACON_IR_FOE_1].updated = TRUE;
				//debug_printf("IR1=%dmm", global.env.sensor[BEACON_IR_FOE_1].distance);
				//debug_printf("|%d", ((Sint16)((((Sint32)(global.env.sensor[BEACON_IR_FOE_1].angle))*180/PI4096))));
			} //else debug_printf("NO IR 1 err %d!\n", msg->data[0]);
			if((msg->data[4] & 0xFE) == AUCUNE_ERREUR)
			{
				//slashn = TRUE;
				global.env.sensor[BEACON_IR_FOE_2].angle = (Sint16)(U16FROMU8(msg->data[5],msg->data[6]));
				// Pour gérer l'inversion de la balise
				//global.env.sensor[BEACON_IR_FOE_2].angle += (global.env.sensor[BEACON_IR_FOE_2].angle > 0)?-PI4096:PI4096;
				global.env.sensor[BEACON_IR_FOE_2].distance = (Uint16)(msg->data[7])*10;
				global.env.sensor[BEACON_IR_FOE_2].update_time = global.env.match_time;
				global.env.sensor[BEACON_IR_FOE_2].updated = TRUE;
				//debug_printf(" IR2=%dmm", global.env.sensor[BEACON_IR_FOE_2].distance);
				//debug_printf("|%d", ((Sint16)((((Sint32)(global.env.sensor[BEACON_IR_FOE_2].angle))*180/PI4096))));
			} //else debug_printf("NO IR 2 err %d!\n", msg->data[4]);
			if(slashn)
				debug_printf("\n");
				*/
			break;
		default:
			break;
	}
}
//void DETECTION_update_foe_position(void)
//{


	/*
 	static bool_e ultrasonic_fiable = TRUE;
	Sint16 beacon_foe_x, beacon_foe_y;
	bool_e update_dist_by_ir;
	Uint8 foe_id;
	 for (foe_id = 0; foe_id < MAX_NB_FOES; foe_id++)
	{
		if((global.env.match_time - global.env.sensor[BEACON_IR(foe_id)].update_time < MINIMUM_TIME_FOR_BEACON_SYNCRONIZATION) &&
		   (global.env.match_time - global.env.sensor[BEACON_US(foe_id)].update_time < MINIMUM_TIME_FOR_BEACON_SYNCRONIZATION) &&
		   (global.env.match_started == TRUE) &&
		   (ultrasonic_fiable == TRUE))
		{
			if(absolute(global.env.sensor[BEACON_IR(foe_id)].distance - global.env.sensor[BEACON_US(foe_id)].distance) > 1000) {
				ultrasonic_fiable = FALSE;
				CAN_msg_t msg;
				msg.sid = DEBUG_US_NOT_RELIABLE;
				msg.data[0] = HIGHINT(global.env.sensor[BEACON_IR(foe_id)].distance);
				msg.data[1] = LOWINT(global.env.sensor[BEACON_IR(foe_id)].distance);
				msg.data[2] = HIGHINT(global.env.sensor[BEACON_US(foe_id)].distance);
				msg.data[3] = LOWINT(global.env.sensor[BEACON_US(foe_id)].distance);
				msg.data[4] = foe_id;
				msg.size = 4;
				CAN_send(&msg);
			}
		}
#warning "DESACTIVATION MANUELLE DES US !!!"
		ultrasonic_fiable = FALSE;

		if(global.env.sensor[BEACON_IR(foe_id)].updated)
		{
			update_dist_by_ir = FALSE;
			
			if(global.env.match_time - global.env.sensor[BEACON_US(foe_id)].update_time > MAXIMUM_TIME_FOR_BEACON_REFRESH ||
				ultrasonic_fiable == FALSE) //Si la balise US n'a rien reçu depuis 500ms
			{
				global.env.foe[foe_id].dist = global.env.sensor[BEACON_IR(foe_id)].distance; //On met à jour la distance par infrarouge
				update_dist_by_ir = TRUE;
			}
			if(global.env.match_time - global.env.sensor[BEACON_US(foe_id)].update_time < MINIMUM_TIME_FOR_BEACON_SYNCRONIZATION || update_dist_by_ir)
			{
				//L'ancienne distance est conservee
				beacon_foe_x = (global.env.foe[foe_id].dist * cos4096(global.env.sensor[BEACON_IR(foe_id)].angle)) * global.env.pos.cosAngle 
					- (global.env.foe[foe_id].dist * sin4096(global.env.sensor[BEACON_IR(foe_id)].angle)) * global.env.pos.sinAngle + global.env.pos.x;

				beacon_foe_y  = (global.env.foe[foe_id].dist * cos4096(global.env.sensor[BEACON_IR(foe_id)].angle)) * global.env.pos.sinAngle 
					+ (global.env.foe[foe_id].dist * sin4096(global.env.sensor[BEACON_IR(foe_id)].angle)) * global.env.pos.cosAngle + global.env.pos.y;

				if(ENV_game_zone_filter(beacon_foe_x,beacon_foe_y,BORDER_DELTA))
				{
					global.env.foe[foe_id].x = beacon_foe_x;
					global.env.foe[foe_id].y = beacon_foe_y;
					global.env.foe[foe_id].update_time = global.env.match_time;
					global.env.foe[foe_id].updated = TRUE;
				}
			}
			global.env.foe[foe_id].angle = global.env.sensor[BEACON_IR(foe_id)].angle;		
			//detection_printf("IR Foe_%d is x:%d y:%d d:%d a:%d\r\n",foe_id, global.env.foe[foe_id].x, global.env.foe[foe_id].y, global.env.foe[foe_id].dist, ((Sint16)(((Sint32)(global.env.foe[foe_id].angle))*180/PI4096)));
		}

		if(global.env.sensor[BEACON_US(foe_id)].updated && ultrasonic_fiable == TRUE)
		{
			// L'ancien angle est conserve
			if(global.env.match_time - global.env.sensor[BEACON_IR(foe_id)].update_time < MINIMUM_TIME_FOR_BEACON_SYNCRONIZATION)
			{
				beacon_foe_x = (global.env.sensor[BEACON_US(foe_id)].distance * cos4096(global.env.foe[foe_id].angle)) * global.env.pos.cosAngle 
					- (global.env.sensor[BEACON_US(foe_id)].distance * sin4096(global.env.foe[foe_id].angle)) * global.env.pos.sinAngle + global.env.pos.x;

				beacon_foe_y  = (global.env.sensor[BEACON_US(foe_id)].distance * cos4096(global.env.foe[foe_id].angle)) * global.env.pos.sinAngle 
					+ (global.env.sensor[BEACON_US(foe_id)].distance * sin4096(global.env.foe[foe_id].angle)) * global.env.pos.cosAngle + global.env.pos.y;

				if(ENV_game_zone_filter(beacon_foe_x,beacon_foe_y,BORDER_DELTA))
				{
					global.env.foe[foe_id].x = beacon_foe_x;
					global.env.foe[foe_id].y = beacon_foe_y;
					global.env.foe[foe_id].update_time = global.env.match_time;
					global.env.foe[foe_id].updated = TRUE;
				}
			}
			// On mets a jour la distance
			global.env.foe[foe_id].dist = global.env.sensor[BEACON_US(foe_id)].distance;
			//detection_printf("US Foe_%d is x:%d y:%d d:%d a:%d\r\n",foe_id, global.env.foe[foe_id].x, global.env.foe[foe_id].y, global.env.foe[foe_id].dist,((Sint16)(((Sint32)(global.env.foe[foe_id].angle))*180/PI4096)));
		}
	}

bool_e ENV_game_zone_filter(Sint16 x, Sint16 y, Uint16 delta)
{
	// Délimitation du terrain
	if(x < delta || y < delta || x > GAME_ZONE_SIZE_X - delta || y > GAME_ZONE_SIZE_Y - delta
	//|| (x > 875 - delta && x < 1125 + delta  && y > 975 - delta && y < 2025 + delta) // Pour supprimer la zone centrale (totem + palmier)
	|| (x > 1250 - delta && (y < 340 + delta || y > 2660 - delta))) //Pour supprimer les cales
	{
		return FALSE;
	}
	return TRUE;
}

	*/




//}


