/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, CHOMP
 *
 *	Fichier : environment.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de supervision de l'environnement
 *	Auteur : Jacen, Nirgal
 *	Version 20100420, modifs 201304
 */

#define ENVIRONMENT_C

#include "environment.h"
#include "act_functions.h"
#include "can_utils.h"
#include "zone_mutex.h"

void ENV_update()
{
	CAN_msg_t incoming_msg;

	/* RAZ des drapeaux temporaires pour la prochaine itération */
	ENV_clean();
	//ENV_pos_update((CAN_msg_t *)NULL, FALSE);	//On récupère l'éventuel position sauvée en IT...
	
	/* Récuperation de l'évolution de l'environnement
	renseignee par les messages CAN */
	while (CAN_data_ready())
	{
		LED_CAN=!LED_CAN;
		incoming_msg = CAN_get_next_msg();
		CAN_update(&incoming_msg);
	}
	
	/* Récupération des données des télémètres*/
	#ifdef USE_TELEMETER
		TELEMETER_update();
	#endif
	
	/* Récupération des données des boutons */
	BUTTON_update();
	
	/* Mise à jour de l'info de position de l'adversaire */
	/* à faire après récupération des infos des capteurs */
	DETECTION_update();
	

	/* Traitement des données des capteurs Sick (télémètres LASER)
	obtenues lors de la réception d'un message CAN d'avancement/de rotation de la propulsion */
	#ifdef USE_SICK
		SICK_update();
	#endif
}

/* met à jour l'environnement en fonction du message CAN reçu */
void CAN_update (CAN_msg_t* incoming_msg)
{
//****************************** Messages carte supervision *************************/	
	switch (incoming_msg->sid)
	{		
		case BROADCAST_POSITION_ROBOT:	   //Les raisons seront ensuite traitees dans la tache de fond	
		case CARTE_P_ROBOT_FREINE:
		case CARTE_P_ROBOT_CALIBRE:
		case CARTE_P_ASSER_ERREUR:
		case CARTE_P_TRAJ_FINIE:
			ENV_pos_update(incoming_msg);	//Tout ces messages contiennent une position... et d'autres infos communes
		break;
		default:	
		break;
	}

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
			//Remarque : Si USE_SICK... si un broadcast_position est arrivé avec pour seule(s) raison(s) WARNING_ROTATION et WARNING_TRANSLATION, il est traité dès réception par CAN_fast_update, et n'arrive pas ici !
			
			//ATTENTION : Pas de switch car les raisons peuvent être cumulées !!!
			//Les raisons WARNING_TRANSLATION, WARNING_ROTATION, WARNING_NO et WARNING_TIMER ne font rien d'autres que déclencher un ENV_pos_update();

			if(incoming_msg->data[6] & WARNING_REACH_X)		//Nous venons d'atteindre une position en X pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.asser.reach_x = TRUE;
				debug_printf("Rx\n");
			}

			if(incoming_msg->data[6] & WARNING_REACH_Y)	//Nous venons d'atteindre une position en Y pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.asser.reach_y = TRUE;
				debug_printf("Ry\n");
			}
			if(incoming_msg->data[6] & WARNING_REACH_TETA)	//Nous venons d'atteindre une position en Teta pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.asser.reach_teta = TRUE;
				debug_printf("Rt\n");
			}

			break;
		case CARTE_P_ROBOT_FREINE:
			global.env.asser.freine = TRUE;
			
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
		case STRAT_START_MATCH:
			global.env.ask_start = TRUE;
			break;
		case XBEE_PING_FROM_OTHER_STRAT:
			//On recoit un ping, on répond par un PONG.
			//Le lien est établi
			global.env.xbee_is_linked = TRUE;
			CAN_send_sid(XBEE_PONG_OTHER_STRAT);
			break;
		case XBEE_PONG_FROM_OTHER_STRAT:
			//On reçoit un pong, tant mieux, le lien est établi
			global.env.xbee_is_linked = TRUE;
			break;
		case DEBUG_DETECT_FOE:
			global.env.debug_force_foe = TRUE;
			break;

		case XBEE_ZONE_COMMAND_RECV:
			ZONE_CAN_process_msg(incoming_msg);
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
	//Samuel : TODO cette fonction peut génèrer des situations foireuses où la position est mise à jour en pleine mauvaise préemption...
		switch (msg->sid)
		{		
			case BROADCAST_POSITION_ROBOT:	   //Les raisons seront ensuite traitees dans la tache de fond	
			case CARTE_P_ROBOT_FREINE:
			case CARTE_P_ROBOT_CALIBRE:
			case CARTE_P_ASSER_ERREUR:
			case CARTE_P_TRAJ_FINIE:
				//ENV_pos_fast_update(msg, TRUE);
				#ifdef USE_SICK
					SICK_update_points();
                #endif
			break;
			default:
				return TRUE;		
		}
	
		return TRUE;	
	
}
#include <can.h>

/* met a jour la position a partir d'un message asser la délivrant */
void ENV_fast_pos_update (CAN_msg_t* msg, bool_e on_it)
{
	volatile static bool_e new_pos_available = FALSE;
	static position_t new_pos;
	static time32_t new_time;
	static way_e new_way;
	static trajectory_e new_trajectory;
	static SUPERVISOR_error_source_e new_status;
	Uint16 save_int;

	if(on_it == TRUE)
	{
		new_pos.x = U16FROMU8(msg->data[0],msg->data[1]) & 0x1FFF;
		new_pos.y = U16FROMU8(msg->data[2],msg->data[3]) & 0x1FFF;
		new_pos.translation_speed = ((Uint16)(msg->data[0] >> 5))*250;	// [mm/sec]
		new_pos.rotation_speed =	((Uint16)(msg->data[2] >> 5));		// [rad/sec]
		new_pos.angle = U16FROMU8(msg->data[4],msg->data[5]);
		new_pos.cosAngle = cos4096(global.env.pos.angle);
		new_pos.sinAngle = sin4096(global.env.pos.angle);
		new_time = global.env.match_time;
		new_way = (way_e)((msg->data[7] >> 3) & 0x03);
		new_status = (SUPERVISOR_error_source_e)((msg->data[7]) & 0x07);
		new_trajectory = (trajectory_e)((msg->data[7] >> 5) & 0x07);
			/*msg->data[7] : 8 bits  : T T T W W E E E
				TTT : trajectoire actuelle
					TRAJECTORY_TRANSLATION		= 0,
					TRAJECTORY_ROTATION			= 1,
					TRAJECTORY_STOP				= 2,
					TRAJECTORY_AUTOMATIC_CURVE	= 3,
					TRAJECTORY_NONE				= 4
				 WW : Way, sens actuel
					ANY_WAY						= 0,
					BACKWARD					= 1,
					FORWARD						= 2,
				 EEE : Erreur
					SUPERVISOR_INIT				= 0,
					SUPERVISOR_IDLE				= 1,		//Rien à faire
					SUPERVISOR_TRAJECTORY		= 2,		//Trajectoire en cours
					SUPERVISOR_ERROR			= 3,		//Carte en erreur - attente d'un nouvel ordre pour en sortir
					SUPERVISOR_MATCH_ENDED		= 4			//Match terminé
				*/
		new_pos_available = TRUE;
	}
	else		//En tache de fond.
	{
		save_int = _C1IE;
		if(save_int == 1)
			_C1IE = 0;
		if(new_pos_available)
		{
			new_pos_available = FALSE;
			global.env.asser.last_time_pos_updated = new_time;
			global.env.asser.current_way = new_way;
			global.env.asser.current_status = new_status;
			global.env.asser.is_in_translation = (new_trajectory >> 2) & 1;
			global.env.asser.is_in_rotation = (new_trajectory >> 1) & 1;
			global.env.pos = new_pos;
			//debug_printf("\n Pos update :  (%lf : %lf)\n",global.env.pos.cosAngle,global.env.pos.sinAngle);
			global.env.pos.updated = TRUE;
		}
		if(save_int == 1)
			_C1IE = 1;

	}

}

/* mise à jour de la position reçue dans l'un des messages de la propulsion.*/
void ENV_pos_update (CAN_msg_t* msg)
{
	global.env.pos.x = U16FROMU8(msg->data[0],msg->data[1]) & 0x1FFF;
	global.env.pos.y = U16FROMU8(msg->data[2],msg->data[3]) & 0x1FFF;
	global.env.pos.translation_speed = ((Uint16)(msg->data[0] >> 5))*250;	// [mm/sec]
	global.env.pos.rotation_speed =	((Uint16)(msg->data[2] >> 5));		// [rad/sec]
	global.env.pos.angle = U16FROMU8(msg->data[4],msg->data[5]);
	global.env.pos.cosAngle = cos4096(global.env.pos.angle);
	global.env.pos.sinAngle = sin4096(global.env.pos.angle);
	global.env.asser.last_time_pos_updated = global.env.match_time;
	global.env.asser.current_way = (way_e)((msg->data[7] >> 3) & 0x03);
	global.env.asser.current_status = (SUPERVISOR_error_source_e)((msg->data[7]) & 0x07);
	global.env.asser.is_in_translation = (((msg->data[7] >> 5) & 0x07) >> 2) & 1;
	global.env.asser.is_in_rotation = (((msg->data[7] >> 5) & 0x07) >> 1) & 1;

	global.env.pos.updated = TRUE;
			/*msg->data[7] : 8 bits  : T R x W W E E E
				 T : TRUE si robot en translation
				 R : TRUE si robot en rotation
				 x : non utilisé
				 WW : Way, sens actuel
					ANY_WAY						= 0,
					BACKWARD					= 1,
					FORWARD						= 2,
				 EEE : Erreur
					SUPERVISOR_INIT				= 0,
					SUPERVISOR_IDLE				= 1,		//Rien à faire
					SUPERVISOR_TRAJECTORY		= 2,		//Trajectoire en cours
					SUPERVISOR_ERROR			= 3,		//Carte en erreur - attente d'un nouvel ordre pour en sortir
					SUPERVISOR_MATCH_ENDED		= 4			//Match terminé
				*/
}

/* met a jour la position de l'adversaire à partir des balises */
void ENV_pos_foe_update (CAN_msg_t* msg)
{
	bool_e slashn;
	if(msg->sid==BEACON_ADVERSARY_POSITION_IR)
	{
		slashn = FALSE;
		if((msg->data[0] & 0xFE) == AUCUNE_ERREUR)	//Si l'octet de fiabilité vaut SIGNAL_INSUFFISANT, on le laisse passer quand même
		{
			//slashn = TRUE;
			global.env.sensor[BEACON_IR_FOE_1].angle = U16FROMU8(msg->data[1],msg->data[2]);
			/* Pour gérer l'inversion de la balise */
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
			/* Pour gérer l'inversion de la balise */
			//global.env.sensor[BEACON_IR_FOE_2].angle += (global.env.sensor[BEACON_IR_FOE_2].angle > 0)?-PI4096:PI4096;
			global.env.sensor[BEACON_IR_FOE_2].distance = (Uint16)(msg->data[7])*10;
			global.env.sensor[BEACON_IR_FOE_2].update_time = global.env.match_time;
			global.env.sensor[BEACON_IR_FOE_2].updated = TRUE;
			//debug_printf(" IR2=%dmm", global.env.sensor[BEACON_IR_FOE_2].distance);
			//debug_printf("|%d", ((Sint16)((((Sint32)(global.env.sensor[BEACON_IR_FOE_2].angle))*180/PI4096))));
		} //else debug_printf("NO IR 2 err %d!\n", msg->data[4]);
		if(slashn)
			debug_printf("\n");
	}
	else if(msg->sid==BEACON_ADVERSARY_POSITION_US)
	{
		slashn = FALSE;
		if(msg->data[0]==AUCUNE_ERREUR) 
		{
			//slashn = TRUE;
			global.env.sensor[BEACON_US_FOE_1].distance = U16FROMU8(msg->data[1],msg->data[2]);//*10;
			global.env.sensor[BEACON_US_FOE_1].update_time = global.env.match_time;
			global.env.sensor[BEACON_US_FOE_1].updated = TRUE;
			//debug_printf("US1=%dmm", global.env.sensor[BEACON_US_FOE_1].distance);
//		} else if(msg->data[0] == TROP_DE_SIGNAL) {
//			#warning "Fiable ?"
//			global.env.sensor[BEACON_US_FOE_1].distance = 0;
//			global.env.sensor[BEACON_US_FOE_1].update_time = global.env.match_time;
//			global.env.sensor[BEACON_US_FOE_1].updated = TRUE;
//			debug_printf("US1: TROP DE SIGNAL\n");
		} /*else {
			debug_printf("NO US 1 err %d!\n", msg->data[0]);
		}*/
		if(msg->data[4]==AUCUNE_ERREUR) 
		{
			//slashn = TRUE;
			global.env.sensor[BEACON_US_FOE_2].distance = U16FROMU8(msg->data[5],msg->data[6]);//*10;
			global.env.sensor[BEACON_US_FOE_2].update_time = global.env.match_time;
			global.env.sensor[BEACON_US_FOE_2].updated = TRUE;
			//debug_printf(" US2=%dmm", global.env.sensor[BEACON_US_FOE_2].distance);
//		} else if(msg->data[4] == TROP_DE_SIGNAL) {
//			#warning "Fiable ?"
//			global.env.sensor[BEACON_US_FOE_2].distance = 0;
//			global.env.sensor[BEACON_US_FOE_2].update_time = global.env.match_time;
//			global.env.sensor[BEACON_US_FOE_2].updated = TRUE;
//			debug_printf("US2: TROP DE SIGNAL\n");
		} /*else {
			debug_printf("NO US 2 err %d!\n", msg->data[4]);
		}*/
		if(slashn)
			debug_printf("\n");
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
	global.env.asser.reach_x = FALSE;
	global.env.asser.reach_y = FALSE;
	global.env.asser.reach_teta = FALSE;
        //global.env.asser.last_time_pos_updated = 0;
	global.env.pos.updated = FALSE;
	global.env.foe[FOE_1].updated = FALSE;
	global.env.foe[FOE_2].updated = FALSE;
	global.env.ask_asser_calibration = FALSE;
	global.env.debug_force_foe = FALSE;
	
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
	BUTTON_init();
    //    CAN_set_direct_treatment_function(CAN_fast_update);

	#ifdef USE_SICK	
		SICK_init();
	#endif
	#ifdef USE_TELEMETER
		TELEMETER_init();
	#endif
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
	global.env.xbee_is_linked = FALSE;
	global.env.flag_for_ping_xbee = 0;
	global.env.asser.calibrated = FALSE;
	
	global.env.asser.current_way = ANY_WAY;
	global.env.asser.is_in_translation = FALSE;
	global.env.asser.is_in_rotation = FALSE;
	global.env.asser.current_status = NO_ERROR;

	global.env.color_ball = ACT_BALLSORTER_NO_CHERRY;

	//Initialisation des elemnts du terrain
	int i;
	//Init cadeaux
	for(i=GOAL_Cadeau0; i <= GOAL_Cadeau3; i++){
		global.env.map_elements[i] = ELEMENT_TODO;
	}
	//Init Verre + VerreEnnemi + Bougies
	for(i=GOAL_Verres0; i <= GOAL_Etage2Bougie11; i++){
		global.env.map_elements[i] = ELEMENT_TODO;
	}
	//Attention la suite n'est pas definitive et necessitera un changement en fonction des positions de calage
//	global.env.map_elements[4] = ELEMENT_NONE;
//	global.env.map_elements[5] = ELEMENT_NONE;
//	global.env.map_elements[6] = ELEMENT_TODO;
//	global.env.map_elements[7] = ELEMENT_TODO;
//	global.env.map_elements[8] = ELEMENT_TODO;
	for(i = GOAL_Assiette0; i <= GOAL_Assiette4; i++) {
		#warning "Position initiale des robots fixe ou disponible par un DEFINE ? (pour savoir l'état initial des assiettes)"
		if(i != GOAL_Assiette0 && i != GOAL_Assiette2)
			global.env.map_elements[i] = ELEMENT_TODO;
		else global.env.map_elements[i] = ELEMENT_NONE;
	}
	for(i = GOAL_AssietteEnnemi0; i <= GOAL_AssietteEnnemi4; i++){
		global.env.map_elements[i] = ELEMENT_NONE;
	}
}

/* envoie un message CAN BROADCAST_COULEUR à jour */
void ENV_set_color(color_e color)
{
	/* changer la couleur */
	global.env.color = color;
	
	/* indiquer au monde la nouvelle couleur */
	CAN_msg_t msg;
	msg.sid=BROADCAST_COULEUR;
	msg.data[0]=global.env.color;
	msg.size=1;
	CAN_send(&msg);
}


void ENV_XBEE_ping_process(void)
{
	/* changer les LEDs de couleur */
	if(global.env.flag_for_ping_xbee >= 2)	//Toutes les secondes
	{
		global.env.flag_for_ping_xbee = 0;
		if(global.env.xbee_is_linked == FALSE)
		{	
			//Si le lien n'est pas avéré, on ping l'autre carte stratégie	
			CAN_send_sid(XBEE_PING_OTHER_STRAT);	
		}		
	}	
	
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
	//|| (x > 875 - delta && x < 1125 + delta  && y > 975 - delta && y < 2025 + delta) // Pour supprimer la zone centrale (totem + palmier)
	|| (x > 1250 - delta && (y < 340 + delta || y > 2660 - delta))) //Pour supprimer les cales
	{
		return FALSE;
	}
	return TRUE;
}	
