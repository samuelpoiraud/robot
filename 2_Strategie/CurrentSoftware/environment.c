/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, CHOMP
 *
 *	Fichier : environment.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de supervision de l'environnement
 *	Auteur : Jacen
 *	Version 20100420
 */

#define ENVIRONMENT_C

#include "environment.h"
#include "act_functions.h"

void ENV_update()
{
	CAN_msg_t incoming_msg;

	/* RAZ des drapeaux temporaires pour la prochaine itération */
	ENV_clean();

	/* Récuperation de l'évolution de l'environnement
	renseignee par les messages CAN */
	while (CAN_data_ready())
	{
		LED_CAN=!LED_CAN;
		incoming_msg = CAN_get_next_msg();
		CAN_update(&incoming_msg);
	}
	
	/* Récupération des données des télémètres*/
	TELEMETER_update();
	
	/* Récupération des données des boutons */
	BUTTON_update();
	
	/* Mise à jour de l'info de position de l'adversaire */
	/* à faire après récupération des infos des capteurs */
	DETECTION_update();
	

	/* Traitement des données des capteurs Sick (télémètres LASER)
	obtenues lors de la réception d'un message CAN d'avancement/de rotation de la propulsion */
	SICK_update();
}

/* met à jour l'environnement en fonction du message CAN reçu */
void CAN_update (CAN_msg_t* incoming_msg)
{
//****************************** Messages carte supervision *************************/	
	switch (incoming_msg->sid)
	{
		case SUPER_ASK_CONFIG:
			global.env.config_updated=TRUE;
			global.env.wanted_config.strategie = incoming_msg->data[0];
			global.env.wanted_config.evitement = incoming_msg->data[2];
			global.env.wanted_config.balise = incoming_msg->data[3];
			/* gestion de la couleur */
			global.env.wanted_color=incoming_msg->data[1];

			if(global.env.wanted_color != global.env.color)
				global.env.color_updated = TRUE;
			break;
		case SUPER_ASK_STRAT_SELFTEST:
				
			break;
			
//****************************** Messages carte propulsion/asser *************************/	
		case CARTE_P_TRAJ_FINIE:
			global.env.asser.fini = TRUE;
			ENV_pos_update(incoming_msg);
			break;
		case CARTE_P_ASSER_ERREUR:
			global.env.asser.erreur = TRUE;
			global.env.asser.vitesse_translation_erreur = 
				((Sint32)U16FROMU8(incoming_msg->data[1],incoming_msg->data[0]) << 16) 
					+ U16FROMU8(incoming_msg->data[3],incoming_msg->data[2]);
			global.env.asser.vitesse_rotation_erreur = 
				((Sint32)U16FROMU8(incoming_msg->data[5],incoming_msg->data[4]) << 16) 
					+ U16FROMU8(incoming_msg->data[7],incoming_msg->data[6]);
			break;
		case CARTE_P_ROBOT_CALIBRE:
			global.env.asser.calibrated = TRUE;
			break;
		case BROADCAST_POSITION_ROBOT:
			/* Si c'est un message de changement de point en mode multipoints */
			if(incoming_msg->data[6] & RAISON_CHANGE_POINT_MULTI)
			{
				global.env.asser.change_point = TRUE;	
			}
			/* Si c'est un message de freinage en mode multipoints */
			if(incoming_msg->data[6] & RAISON_FREINE_MULTI)
			{
				global.env.asser.freine = TRUE;
				global.env.asser.freine_multi_point = TRUE;	
			}
			ENV_pos_update(incoming_msg);
		break;

//****************************** Messages de la carte actionneur *************************/
		case ACT_RESULT:
			ACT_process_result(incoming_msg);
			break;

		case ACT_BALLSORTER_RESULT:
			global.env.color_ball = incoming_msg->data[0];
			break;

/************************************ Récupération des données de la balise *******************************/
		case BEACON_ADVERSARY_POSITION_IR:
			ENV_pos_foe_update(incoming_msg);
			break;
		case BEACON_ADVERSARY_POSITION_US:
			ENV_pos_foe_update(incoming_msg);
			break;
/************************************* Récupération des envois de l'autre robot ***************************/
		case STRAT_ELTS_UPDATE:
			break;
		default:
			break;
	}
	return;
}

/* Cette fonction permet d'éxécuter un traitement rapide dans l'interruption suivant
   le message CAN reçu et renvoie si le message doit être placé dans le buffer */
bool_e CAN_fast_update(CAN_msg_t* msg)
{
	switch (msg->sid)
	{
		case BROADCAST_POSITION_ROBOT:
			/* Si c'est un message de type delta translation ou rotation */ 
			if(msg->data[6] & (RAISON_TRANSLATION | RAISON_ROTATION))
			{
				ENV_pos_update(msg);
				SICK_update_points();
				return FALSE;
			}
			return TRUE;	
			
		default:
			return TRUE;		
	}	
}

/* met a jour la position a partir d'un message asser la délivrant */
void ENV_pos_update (CAN_msg_t* msg)
{
	global.env.pos.x = U16FROMU8(msg->data[0],msg->data[1]);
	global.env.pos.y = U16FROMU8(msg->data[2],msg->data[3]);
	global.env.pos.angle = U16FROMU8(msg->data[4],msg->data[5]);
	global.env.pos.cosAngle = cos4096(global.env.pos.angle);
	global.env.pos.sinAngle = sin4096(global.env.pos.angle);

	//debug_printf("\n Pos update :  (%lf : %lf)\n",global.env.pos.cosAngle,global.env.pos.sinAngle);
	global.env.pos.updated = TRUE;
}

/* met a jour la position de l'adversaire à partir des balises */
void ENV_pos_foe_update (CAN_msg_t* msg)
{
	if(msg->sid==BEACON_ADVERSARY_POSITION_IR)
	{
		if((msg->data[0] & 0xFE) == AUCUNE_ERREUR)	//Si l'octet de fiabilité vaut SIGNAL_INSUFFISANT, on le laisse passer quand même
		{
			global.env.sensor[BEACON_IR_FOE_1].angle = U16FROMU8(msg->data[1],msg->data[2]);
			/* Pour gérer l'inversion de la balise */
			//global.env.sensor[BEACON_IR_FOE_1].angle += (global.env.sensor[BEACON_IR_FOE_1].angle > 0)?-PI4096:PI4096;
			global.env.sensor[BEACON_IR_FOE_1].distance = (Uint16)(msg->data[3])*10;
			global.env.sensor[BEACON_IR_FOE_1].update_time = global.env.match_time;
			global.env.sensor[BEACON_IR_FOE_1].updated = TRUE;
			//debug_printf("Distance Foe 1 = %d", global.env.sensor[BEACON_IR_FOE_1].distance);
		}
		if((msg->data[4] & 0xFE) == AUCUNE_ERREUR)
		{
			global.env.sensor[BEACON_IR_FOE_2].angle = U16FROMU8(msg->data[5],msg->data[6]);
			/* Pour gérer l'inversion de la balise */
			//global.env.sensor[BEACON_IR_FOE_2].angle += (global.env.sensor[BEACON_IR_FOE_2].angle > 0)?-PI4096:PI4096;
			global.env.sensor[BEACON_IR_FOE_2].distance = (Uint16)(msg->data[7])*10;
			global.env.sensor[BEACON_IR_FOE_2].update_time = global.env.match_time;
			global.env.sensor[BEACON_IR_FOE_2].updated = TRUE;
			//debug_printf("DIstance FOe 2 = %d", global.env.sensor[BEACON_IR_FOE_2].distance);
		}
	}
	else if(msg->sid==BEACON_ADVERSARY_POSITION_US)
	{		
		if(msg->data[0]==AUCUNE_ERREUR) 
		{
			global.env.sensor[BEACON_US_FOE_1].distance = U16FROMU8(msg->data[1],msg->data[2]);//*10;
			global.env.sensor[BEACON_US_FOE_1].update_time = global.env.match_time;
			global.env.sensor[BEACON_US_FOE_1].updated = TRUE;
			//debug_printf("Incoming beacon US foe 1 = %d\n",  global.env.sensor[BEACON_US_FOE_1].distance);
		}
		if(msg->data[4]==AUCUNE_ERREUR) 
		{
			global.env.sensor[BEACON_US_FOE_2].distance = U16FROMU8(msg->data[5],msg->data[6]);//*10;
			global.env.sensor[BEACON_US_FOE_2].update_time = global.env.match_time;
			global.env.sensor[BEACON_US_FOE_2].updated = TRUE;
			//debug_printf("Incoming beacon US foe 2 = %d\n",  global.env.sensor[BEACON_IR_FOE_2].distance);
		}
	}
}

/* baisse les drapeaux d'environnement pour préparer la prochaine MaJ */
void ENV_clean ()
{
	STACKS_clear_timeouts();
	DETECTION_clear_updates();
	if(global.env.color == global.env.wanted_color)
		global.env.color_updated = FALSE;
	global.env.config_updated = FALSE;
	global.env.asser.fini = FALSE;
	global.env.asser.erreur = FALSE;
	global.env.asser.freine = FALSE;
	global.env.asser.freine_multi_point = FALSE;
	global.env.asser.change_point = FALSE;
	global.env.pos.updated = FALSE;
	global.env.foe[FOE_1].updated = FALSE;
	global.env.foe[FOE_2].updated = FALSE;
	global.env.ask_asser_calibration = FALSE;
	
	/*global.env.act[BROOM_LEFT].ready == TRUE;
	global.env.act[BROOM_RIGHT].ready == TRUE;
	global.env.act[F].ready == TRUE;
	global.env.act[BROOM_LEFT].closed == TRUE;
	global.env.act[BROOM_RIGHT].closed == TRUE;
	global.env.act[F].closed == TRUE;
	global.env.act[BROOM_LEFT].opened == TRUE;
	global.env.act[BROOM_RIGHT].opened == TRUE;
	global.env.act[F].opened == TRUE;
	global.env.act[BROOM_LEFT].failure == TRUE;
	global.env.act[BROOM_RIGHT].failure == TRUE;
	global.env.act[F].failure == TRUE;*/
}


/* initialise les variables d'environnement */
void ENV_init()
{
	CAN_init();
	CAN_set_direct_treatment_function(CAN_fast_update);
//	SICK_init();
	BUTTON_init();
	TELEMETER_init();
	DETECTION_init();

	ENV_clean();
	global.env.config=(config_t){/*strategie*/0, /*evitement*/TRUE, /*balise*/TRUE};
	global.env.wanted_color=BLUE;
	global.env.color = COLOR_INIT_VALUE; //update -> color = wanted + dispatch
	global.env.color_updated = TRUE;
	global.env.match_started = FALSE;
	global.env.match_over = FALSE;
	global.env.foe[FOE_1].update_time = 0;
	global.env.foe[FOE_2].update_time = 0;
	global.env.match_time = 0;
	global.env.pos.dist = 0;
	global.env.ask_start = FALSE;
	global.env.asser.change_point = FALSE;
	global.env.asser.calibrated = FALSE;

	//Initialisation des elemnts du terrain
	int i;
	for(i=0; i<4; i++){
		global.env.map_elements[i] = TODO;
	}
	for(i=14; i<40; i++){
		global.env.map_elements[i] = TODO;
	}
	//Attention la suite n'est pas definitive et necessitera un changement en fonction des positions de calage
	global.env.map_elements[4] = NON;
	global.env.map_elements[5] = NON;
	global.env.map_elements[6] = TODO;
	global.env.map_elements[7] = TODO;
	global.env.map_elements[8] = TODO;
	for(i=9; i<14;i++){
		global.env.map_elements[i] = NON;
	}
}

/* envoie un message CAN BROADCAST_COULEUR à jour */
void ENV_set_color(color_e color)
{
	/* changer la couleur */
	global.env.color = color;
	/* changer les LEDs de couleur */
	if(global.env.color!=BLUE)
	{
		BLUE_LEDS=0;
		RED_LEDS=1;
	}
	else
	{
		RED_LEDS=0;
		BLUE_LEDS=1;
	}
	/* indiquer au monde la nouvelle couleur */
	CAN_msg_t msg;
	msg.sid=BROADCAST_COULEUR;
	msg.data[0]=global.env.color;
	msg.size=1;
	CAN_send(&msg);
}

/* envoie la config actuelle sur le CAN (pour la super) */
void ENV_dispatch_config()
{
	CAN_msg_t msg;
	msg.sid = SUPER_CONFIG_IS;
	msg.data[0] = global.env.config.strategie;
	msg.data[2] = global.env.config.evitement;
	msg.data[3] = global.env.config.balise;
	msg.size=4;
	/* gestion couleur */
	msg.data[1] = (global.env.color==COLOR_INIT_VALUE)?0:global.env.color;
	CAN_send(&msg);
}

bool_e ENV_game_zone_filter(Sint16 x, Sint16 y, Uint16 delta)
{
	/* Délimitation du terrain */
	if(x < delta || y < delta || x > GAME_ZONE_SIZE_X - delta || y > GAME_ZONE_SIZE_Y - delta
	|| (x > 875 - delta && x < 1125 + delta  && y > 975 - delta && y < 2025 + delta) // Pour supprimer la zone centrale (totem + palmier)
	|| (x > 1250 - delta && (y < 340 + delta || y > 2660 - delta))) //Pour supprimer les cales
	{
		return FALSE;
	}
	return TRUE;
}	
