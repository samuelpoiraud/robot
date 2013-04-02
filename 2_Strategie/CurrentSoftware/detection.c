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

void DETECTION_init(void)
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
		
	Uint8 i;
	for(i=0; i<SENSOR_NB;i++)
	{
		global.env.sensor[i].angle=0;
		global.env.sensor[i].distance=2600;
		global.env.sensor[i].foeX=0;
		global.env.sensor[i].foeY=0;
		global.env.sensor[i].update_time=0;
		global.env.sensor[i].updated = FALSE;
	}
	for (i=0; i<NB_FOES; i++)
	{
		global.env.foe[i].dist = 31337; 
		global.env.foe[i].angle = PI4096/2;
		global.env.foe[i].update_time = 0;
		global.env.foe[i].updated = FALSE;
	}
}	

/*	mise à jour de l'information de détection avec le contenu
	courant de l'environnement */
void DETECTION_update(void)
{
	Uint8 i;
	bool_e must_update=FALSE;
	
	/* on teste si il y a une nouvelle information pour mettre à jour la position de l'adversaire */
	for(i=0; i< SENSOR_NB; i++)
	{
		if(global.env.sensor[i].updated)
		{
			must_update = TRUE;
		}
	}
	
	if(must_update)
	{
		if (global.env.config.balise)
		{
			//Mise à jour grâce aux balises et télémètres
			DETECTION_update_foe_by_beacon();
			//DETECTION_update_foe_by_telemeter();
		}
		else
		{
			//Mise à jour seulement grâce aux telemetres.
			//DETECTION_update_foe_only_by_telemeter();
		}
		
		for (i=0; i < NB_FOES; i++)
		{
			if(global.env.foe[i].updated)
		#warning attention: A désactiver en match pour eviter de flooder
				detection_printf("\r\nFoe_%d is x:%d y:%d dist:%d angle:%d\r\n", i, global.env.foe[i].x, global.env.foe[i].y, global.env.foe[i].dist, global.env.foe[i].angle);
				CAN_send_foe_pos();	
		}			
	}	
}

void DETECTION_update_foe_only_by_telemeter()
{
	Uint8 i;
	foe_e foe_id = FOE_1;
	
	for(i=0; i<TELEMETER_NUMBER; i++)
	{
		if(global.env.sensor[i].updated)
		{
			global.env.foe[foe_id].dist = global.env.sensor[i].distance+300; //300 pour aller du centre de notre robot au centre du robot adverse
			global.env.foe[foe_id].angle = global.env.sensor[i].angle;
			global.env.foe[foe_id].x = global.env.sensor[i].foeX;
			global.env.foe[foe_id].y = global.env.sensor[i].foeY;
			global.env.foe[foe_id].update_time = global.env.match_time;
			global.env.foe[foe_id].updated = TRUE;
			detection_printf("\r\nFoe_%d telemeter is x:%d y:%d dist:%d angle:%d\r\n", foe_id, global.env.foe[foe_id].x, global.env.foe[foe_id].y, global.env.foe[foe_id].dist, global.env.foe[foe_id].angle);
			foe_id++;
		}
		if (foe_id == NB_FOES) return; //Sécurité, vue l'orientation des dt10, on ne peut recevoir 2 informations pour le meme adversaire
	}	
}

void DETECTION_update_foe_by_telemeter()
{
	Uint8 i, j;
	GEOMETRY_point_t pos_telemeter_adversary;//Position de l'adversaire renvoyé par le telemetre
	GEOMETRY_point_t pos_adversary[NB_FOES] = {{global.env.foe[FOE_1].x,global.env.foe[FOE_1].y},//Dernière position connue de l'adversaire 1
											 {global.env.foe[FOE_2].x,global.env.foe[FOE_2].y}};//Dernière position connue de l'adversaire 2
	bool_e processed;//Permet de savoir si l'information a bien ete traitée
	
	for(i=0; i<TELEMETER_NUMBER; i++)
	{
		if(global.env.sensor[i].updated)
		{
			processed = FALSE;
			pos_telemeter_adversary.x = global.env.sensor[i].foeX;
			pos_telemeter_adversary.y = global.env.sensor[i].foeY;
			
			for (j=0; j<NB_FOES; j++)//On regarde si on repère un des deux robots sur le terrain
			{
				if (GEOMETRY_distance (pos_adversary[j], pos_telemeter_adversary) < 250 //On regarde si on récupère le bon robot 
				&& global.env.sensor[BEACON_IR(j)].updated // des infos de la balise a infrarouges 
				&& global.env.sensor[BEACON_IR(j)].distance < BEACON_FAR_THRESHOLD) // la balise a infrarouges donne un adversaire proche
				{
					global.env.foe[j].dist = global.env.sensor[i].distance+300; //300 pour aller du centre de notre robot au centre du robot adverse
					global.env.foe[j].angle = global.env.sensor[BEACON_IR(j)].angle;
					global.env.foe[j].x = global.env.sensor[i].foeX;
					global.env.foe[j].y = global.env.sensor[i].foeY;
					global.env.foe[j].update_time = global.env.match_time;
					global.env.foe[j].updated = TRUE;
					detection_printf("\r\nFoe_%d telemeter is x:%d y:%d dist:%d angle:%d\r\n", j, global.env.foe[j].x, global.env.foe[j].y, global.env.foe[j].dist, global.env.foe[j].angle);
					processed = TRUE; //Informations de position exploitées
				}
			}

			for (j=0; j<NB_FOES; j++)
			{
				if(!processed && //Non traité
				global.env.match_time - global.env.sensor[BEACON_IR(j)].update_time > MAXIMUM_TIME_FOR_BEACON_REFRESH)//Non mis à jour depuis 500ms
				{
					global.env.foe[j].dist = global.env.sensor[i].distance+300; //300 pour aller du centre de notre robot au centre du robot adverse
					global.env.foe[j].angle = global.env.sensor[i].angle;
					global.env.foe[j].x = global.env.sensor[i].foeX;
					global.env.foe[j].y = global.env.sensor[i].foeY;
					global.env.foe[j].update_time = global.env.match_time;
					global.env.foe[j].updated = TRUE;
					detection_printf("\r\nFoe_%d telemeter is x:%d y:%d dist:%d angle:%d\r\n", j, global.env.foe[j].x, global.env.foe[j].y, global.env.foe[j].dist, global.env.foe[j].angle);
					processed = TRUE;
				}
			}
		}
	}
}

void DETECTION_update_foe_by_beacon()
{
	Sint16 beacon_foe_x, beacon_foe_y;
	bool_e update_dist_by_ir;
	Uint8 foe_id;
	
	for (foe_id = 0; foe_id < NB_FOES; foe_id++)
	{
		if(global.env.sensor[BEACON_IR(foe_id)].updated)
		{
			update_dist_by_ir = FALSE;
			
			if(global.env.match_time - global.env.sensor[BEACON_US(foe_id)].update_time > MAXIMUM_TIME_FOR_BEACON_REFRESH) //Si la balise US n'a rien reçu depuis 500ms
			{
				global.env.foe[foe_id].dist = global.env.sensor[BEACON_IR(foe_id)].distance; //On met à jour la distance par infrarouge
				update_dist_by_ir = TRUE;
			}
			if(global.env.match_time - global.env.sensor[BEACON_US(foe_id)].update_time < MINIMUM_TIME_FOR_BEACON_SYNCRONIZATION || update_dist_by_ir)
			{
				/* L'ancienne distance est conservee */
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
			detection_printf("IR Foe_%d is x:%d y:%d dist:%d angle:%d\r\n",foe_id, global.env.foe[foe_id].x, global.env.foe[foe_id].y, global.env.foe[foe_id].dist, global.env.foe[foe_id].angle);	
		}
		
		if(global.env.sensor[BEACON_US(foe_id)].updated)
		{
			/* L'ancien angle est conserve */
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
			/* On mets a jour la distance */
			global.env.foe[foe_id].dist = global.env.sensor[BEACON_US(foe_id)].distance;
			detection_printf("US Foe_%d is x:%d y:%d dist:%d angle:%d\r\n",foe_id, global.env.foe[foe_id].x, global.env.foe[foe_id].y, global.env.foe[foe_id].dist, global.env.foe[foe_id].angle);	
		}
	}
}

void DETECTION_clear_updates()
{
	Uint8 i;
	for(i=0; i<SENSOR_NB;i++)
		global.env.sensor[i].updated=FALSE;
}	


/*fonction qui retourne un type enum qui permet de savoir on détect un lingot et ça position dans le totem
et egalement si il n'y a plus de lingot dans le totem*/
detection_totem_e DETECTION_bullion_update()
{

	GEOMETRY_point_t point ;
	point = SICK_get_point_position(DT50_TOP);
	if(point.x>NORTH_TOTEM_X_MIN && point.x<NORTH_TOTEM_X_MAX)
	{
		if(point.y>FIRST_TOTEM_Y_MIN && point.y<FIRST_TOTEM_Y_MAX)
			return FIRST_TOTEM_NORTH;
		if(point.y>SECOND_TOTEM_Y_MIN && point.y<SECOND_TOTEM_Y_MAX)
			return SECOND_TOTEM_NORTH;
	}
	else if(point.x>SOUTH_TOTEM_X_MIN && point.x<SOUTH_TOTEM_X_MAX)
	{
		if(point.y>FIRST_TOTEM_Y_MIN && point.y<FIRST_TOTEM_Y_MAX)
			return FIRST_TOTEM_SOUTH;
		if(point.y>SECOND_TOTEM_Y_MIN && point.y<SECOND_TOTEM_Y_MAX)
			return SECOND_TOTEM_SOUTH;
	}
	else if(point.x>TOTEM_NORTH_CENTER_X_MIN && point.x<TOTEM_NORTH_CENTER_X_MAX)
	{
		if(point.y>FIRST_TOTEM_CENTER_Y_MIN && point.y<FIRST_TOTEM_CENTER_Y_MAX)
			return FIRST_TOTEM_NORTH_EMPTY;
		if(point.y>SECOND_TOTEM_CENTER_Y_MIN && point.y<SECOND_TOTEM_CENTER_Y_MAX)
			return SECOND_TOTEM_NORTH_EMPTY;
	}
	else if(point.x>TOTEM_SOUTH_CENTER_X_MIN && point.x<TOTEM_SOUTH_CENTER_X_MAX)
	{
		if(point.y>FIRST_TOTEM_CENTER_Y_MIN && point.y<FIRST_TOTEM_CENTER_Y_MAX)
			return FIRST_TOTEM_SOUTH_EMPTY;
		if(point.y>SECOND_TOTEM_CENTER_Y_MIN && point.y<SECOND_TOTEM_CENTER_Y_MAX)
			return SECOND_TOTEM_SOUTH_EMPTY;
	}
	return NO_DETECTION;
}	

#include "QS/QS_CANmsgList.h"
//envoi de la position du robot adverse sur le CAN
static void CAN_send_foe_pos()
{
	#ifdef USE_FOE_POS_CAN_DEBUG
	static time32_t previous_time=0;
	
	Uint8 view = 0;
	detection_sensor_e i;
	CAN_msg_t msg;
	
	if (global.env.match_time - previous_time > 250)
	{
		previous_time = global.env.match_time;

		for (i=0; i < SENSOR_NB; i++)
		{
			view |= (global.env.sensor[i].updated)<<i;
		}	

		for (i=0; i < NB_FOES; i++)
		{
			msg.sid = DEBUG_FOE_POS;
			msg.data[0]= view;
			msg.data[1]= HIGHINT(global.env.foe[i].angle);
			msg.data[2]= LOWINT(global.env.foe[i].angle);
			msg.data[3]= global.env.foe[i].dist/10;
			msg.data[4]= HIGHINT(global.env.foe[i].x);
			msg.data[5]= LOWINT(global.env.foe[i].x);
			msg.data[6]= HIGHINT(global.env.foe[i].y);
			msg.data[7]= LOWINT(global.env.foe[i].y);
			msg.size=8;
			CAN_send(&msg);
		}
	}
	#endif /* def USE_FOE_POS_CAN_DEBUG */
}
