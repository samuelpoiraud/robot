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

volatile static Uint32 absolute_time = 0;
volatile static Uint32 hokuyo_update_time = 0;

#define BEACON_MAX_FOES		2
#define FOE_DATA_LIFETIME	250		//[ms] Durée de vie des données envoyées par la propulsion et par la balise

adversary_t adversaries[HOKUYO_MAX_FOES + BEACON_MAX_FOES];	//Ce tableau se construit progressivement, quand on a toutes les données, on peut les traiter et renseigner le tableau des positions adverses.
volatile Uint8 hokuyo_objects_number = 0;	//Nombre d'objets hokuyo

void DETECTION_init(void)
{
	Uint8 i;
	static bool_e initialized = FALSE;
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
	if(absolute_time > next_beacon_activate_msg)
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
	assert(foe_id < BEACON_MAX_FOES);

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




// Doit être appelé en FIN d'IT... (les flags updated y sont baissés)
void DETECTION_process_it(void)
{
	Uint8 i;
	absolute_time += PERIODE_IT_ASSER;

	for(i = 0; i < HOKUYO_MAX_FOES + BEACON_MAX_FOES; i++)
	{
		adversaries[i].updated = FALSE;
		//on peut descendre ce flag ici grace au fait que la fonction qui remplit le tableau "adversaries" est non préemptible.
		//(et donc l'IT qui suivra et appelera ce DETECTION_process_it aura pu voir les données cohérentes et complètes).

		if((adversaries[i].enable) && (absolute_time - adversaries[i].update_time > FOE_DATA_LIFETIME))
			adversaries[i].enable = FALSE;
	}

}


/*
 * Déclaration des détections adverses :
 * 	- Via un message CAN (sinon, msg = NULL)
 * 	- Via une structure HOKUYO_adversary_position et adv_nb (sinon adv = NULL)
 */
void DETECTION_new_adversary_position(CAN_msg_t * msg, HOKUYO_adversary_position * adv, Uint8 adv_nb)
{
	Uint8 i;
	Sint16 cosinus, sinus, our_cos, our_sin;

	//Section critique. le tableau adversaries ne peut pas être consulté en IT pendant sa modification ici..
	//Pour éviter cette section critique qui est "un peu" longue... il faudrait simplement mémoriser en tâche de fond msg et adv... et les traiter en IT.
	TIMER1_disableInt();

	if(msg != NULL)
	{
		if(msg->sid == BROADCAST_BEACON_ADVERSARY_POSITION_IR)
		{
			for(i = 0; i<BEACON_MAX_FOES; i++)
			{
				//Extraction des données du msg.
				adversaries[HOKUYO_MAX_FOES+i].fiability_error = msg->data[0+4*i];
				adversaries[HOKUYO_MAX_FOES+i].angle = (Sint16)(U16FROMU8(msg->data[1+4*i],msg->data[2+4*i]));
				adversaries[HOKUYO_MAX_FOES+i].dist = (Uint16)(msg->data[3+4*i])*12;	//*12 : normalement, c'est *10, mais on corrige ici la précision de la distance.

				//enable ou pas ?
				adversaries[HOKUYO_MAX_FOES+i].enable = FALSE;
				if	(absolute_time - hokuyo_update_time > FOE_DATA_LIFETIME
						|| 	(		adversaries[HOKUYO_MAX_FOES+i].angle > PI4096/2-PI4096/3		//Angle entre 30° et 150° --> angle mort hokuyo + marge de 15° de chaque coté.
								&& 	adversaries[HOKUYO_MAX_FOES+i].angle < PI4096/2+PI4096/3 )
					)
				{
					if((adversaries[HOKUYO_MAX_FOES+i].fiability_error & ~SIGNAL_INSUFFISANT & ~TACHE_TROP_GRANDE) == AUCUNE_ERREUR)	//Si je n'ai pas d'autre erreur que SIGNAL_INSUFFISANT... c'est bon
					{
						adversaries[HOKUYO_MAX_FOES+i].enable = TRUE;
						adversaries[HOKUYO_MAX_FOES+i].updated = TRUE;
					}
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

				//Mise à jour de l'update_time
				adversaries[HOKUYO_MAX_FOES+i].update_time = absolute_time;
			}
		}
	}
	if(adv != NULL)
	{
		hokuyo_objects_number = adv_nb;
		hokuyo_update_time = absolute_time;
		if(adv_nb > 0)
		{
			for(i = 0; i < adv_nb ; i++)
			{
				adversaries[i].enable = TRUE;
				adversaries[i].updated = TRUE;
				adversaries[i].update_time = absolute_time;
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

	TIMER1_enableInt();
	//Fin de la section critique
}


adversary_t * DETECTION_get_adversaries(Uint8 * size)
{
	*size = BEACON_MAX_FOES + HOKUYO_MAX_FOES;
	return adversaries;
}

