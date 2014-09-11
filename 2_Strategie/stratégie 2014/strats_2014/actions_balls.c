/*
 * actions_ball.c
 *
 *  Created on: 18 mai 2014
 *      Author: Nirgal
 */

#include "actions_balls.h"
#include "../prop_functions.h"
#include "../act_functions.h"
#include "../QS/QS_types.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../Pathfind.h"
#include "../high_level_strat.h"

#define OFFSET_BALL_LAUNCHER	20		//Distance entre le centre du robot et le centre du ball launcher
#define ELOIGNEMENT_SHOOT_BALL 520
#define SPEED_LANCE_LAUNCHER 110
#define TIME_BEGINNING_NO_AVOIDANCE 5000

static bool_e lance_ball = FALSE;
static bool_e launcher_ball_adversary = FALSE;  // Si il doit lancer une balle sur le mammouth ennemis

error_e sub_lance_launcher(bool_e lanceAll, color_e mammouth)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_LANCE_LAUNCHER,
		IDLE,
		GET_IN,
		POS_BEGINNING,
		POS_LAUNCH,
		ERROR,
		GET_OUT_WITH_ERROR,
		GOBACK_FIRST_POINT,
		GOBACK_LAST_POINT,
		DONE,
		RETURN_NOT_HANDLED
	);


	static displacement_t dplt[4]; // Deplacement
	static way_e sensRobot;
	static Uint16 posShoot; // Position à laquelle, on va tirer les balles
	static Uint16 sensShoot; // Vrai si il va de rouge vers jaune sinon faux
	Uint16 zone_access_posShoot_x1, zone_access_posShoot_x2, zone_access_posShoot_y1, zone_access_posShoot_y2;
	Uint16 zone_access_firstPoint_y1, zone_access_firstPoint_y2;
	Uint16 middle_mammouth;
	avoidance_type_e avoidance;
	//Si on est en début de match (<5 sec) et que Guy ne nous a pas indiqué qu'il était bloqué au Nord, l'évitement est désactivé...
	avoidance = (global.env.match_time > TIME_BEGINNING_NO_AVOIDANCE || global.env.guy_is_bloqued_in_north)?NO_DODGE_AND_WAIT:NO_AVOIDANCE;

	switch(state){
		case IDLE:
			dplt[0].point.x = ELOIGNEMENT_SHOOT_BALL + 90;	//600 : pour pouvoir tourner sur place lorsqu'on est sur le premier point !
			dplt[1].point.x = ELOIGNEMENT_SHOOT_BALL;
			dplt[2].point.x = ELOIGNEMENT_SHOOT_BALL;
			dplt[3].point.x = ELOIGNEMENT_SHOOT_BALL + 90;	//600 : pour pouvoir tourner sur place lorsqu'on est sur le dernier point !
			dplt[0].speed = FAST;
			dplt[1].speed = SPEED_LANCE_LAUNCHER;
			dplt[2].speed = SPEED_LANCE_LAUNCHER;
			dplt[3].speed = SPEED_LANCE_LAUNCHER;

			middle_mammouth = (mammouth == RED)?750:2250;

			if(global.env.pos.y < middle_mammouth)	//Par rapport au mammouth visé, je suis plutôt coté rouge
			{
				sensRobot = FORWARD;
				sensShoot = TRUE;
				if(mammouth == RED)		//3 points, sens rouge vers jaune, pour le mammouth rouge
				{
					if(mammouth == global.env.color)
						posShoot = 720 - OFFSET_BALL_LAUNCHER; // Depart Rouge
					else
						posShoot = 740 - OFFSET_BALL_LAUNCHER;

					zone_access_posShoot_x1 = 300;
					zone_access_posShoot_x2 = 650;
					zone_access_posShoot_y1 = 0;
					zone_access_posShoot_y2 = 450;
					zone_access_firstPoint_y1 = 0;
					zone_access_firstPoint_y2 = 750;
					dplt[0].point.y = 500;
					dplt[1].point.y = posShoot - 150;
					dplt[2].point.y = posShoot + 200;
					dplt[3].point.y = 1100;
				}
				else					//3 points, sens rouge vers jaune, pour le mammouth jaune
				{
					if(mammouth == global.env.color)
						posShoot = 2200 - OFFSET_BALL_LAUNCHER;
					else
						posShoot = 2300 - OFFSET_BALL_LAUNCHER;

					zone_access_posShoot_x1 = 400;
					zone_access_posShoot_x2 = 600;
					zone_access_posShoot_y1 = 1500;
					zone_access_posShoot_y2 = 2000;
					zone_access_firstPoint_y1 = 1800;
					zone_access_firstPoint_y2 = 2300;
					dplt[0].point.y = 1900;
					dplt[1].point.y = posShoot - 150;
					dplt[2].point.y = posShoot + 200;
					dplt[3].point.y = 2500;
				}
			}
			else							//Par rapport au mammouth visé, je suis plutôt coté jaune
			{
				sensRobot = BACKWARD;
				sensShoot = FALSE;
				if(mammouth == RED)		//3 points, sens jaune vers rouge, pour le mammouth rouge
				{
					if(mammouth == global.env.color)
						posShoot = 740 + OFFSET_BALL_LAUNCHER;
					else
						posShoot = 800 + OFFSET_BALL_LAUNCHER;

					zone_access_posShoot_x1 = 400;
					zone_access_posShoot_x2 = 600;
					zone_access_posShoot_y1 = 3000-1500;
					zone_access_posShoot_y2 = 3000-2000;
					zone_access_firstPoint_y1 = 3000-1800;
					zone_access_firstPoint_y2 = 3000-2300;
					dplt[0].point.y = 1100;
					dplt[1].point.y = posShoot + 150;
					dplt[2].point.y = posShoot - 200;
					dplt[3].point.y = 500;
				}
				else					//3 points, sens jaune vers rouge, pour le mammouth jaune
				{
					if(mammouth == global.env.color)
						posShoot = 2280 + OFFSET_BALL_LAUNCHER; // Depart Jaune
					else
						posShoot = 2280 + OFFSET_BALL_LAUNCHER;

					zone_access_posShoot_x1 = 300;
					zone_access_posShoot_x2 = 650;
					zone_access_posShoot_y1 = 3000-0;
					zone_access_posShoot_y2 = 3000-450;
					zone_access_firstPoint_y1 = 3000-0;
					zone_access_firstPoint_y2 = 3000-750;
					dplt[0].point.y = 2500;
					dplt[1].point.y = posShoot + 150;
					dplt[2].point.y = posShoot - 200;
					dplt[3].point.y = 1900;
				}
			}


			//Plusieurs cas possibles :
			//1- on peut se rendre au premier point sans s'arrêter pour faire la trajectoire de lancé
			//2- on peut se rendre au premier point en s'y arrêtant puis faire la trajectoire de lancé
			//3- on ne peut pas s'y rendre directement -> pathfind
			if(est_dans_carre(zone_access_posShoot_x1, zone_access_posShoot_x2, zone_access_posShoot_y1, zone_access_posShoot_y2, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{	//1- on peut se rendre directement au point de lancé
					state = POS_LAUNCH;

					// Si un adversaire se trouve dans la zone du premier point, nous allons au premier point de tire
//					if(foe_in_zone(FALSE,dplt[0].point.x,dplt[0].point.y)){

//					}
			}
			else if(est_dans_carre(400, 1800, zone_access_firstPoint_y1, zone_access_firstPoint_y2, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{	//2- on peut se rendre au premier point en s'y arrêtant puis faire la trajectoire de lancé
				state = POS_BEGINNING;
			}
			else
			{	//3- on ne peut pas s'y rendre directement -> pathfind
				state = GET_IN;
			}


			break;

		case GET_IN:
			state = PATHFIND_try_going(PATHFIND_closestNode(dplt[0].point.x,dplt[0].point.y, 0x00),
					GET_IN, POS_LAUNCH, RETURN_NOT_HANDLED, ANY_WAY, FAST,DODGE_AND_WAIT, END_AT_LAST_POINT);
			//On considère que le point du pathfind est satisfaisant pour rejoindre directement ensuite le point de lancé
			break;

		case POS_BEGINNING:
			state = try_going(dplt[0].point.x,dplt[0].point.y,POS_BEGINNING,POS_LAUNCH,ERROR,FAST,sensRobot,avoidance);
			break;

		case POS_LAUNCH:
			if(entrance){
				PROP_WARNER_arm_y(posShoot);
				// Trop prêt et tourne à angle droit devant le bac sinon et fini par taper
				dplt[1].point.x = ELOIGNEMENT_SHOOT_BALL + 40;
			}

			state = try_going_multipoint(&dplt[1], 3, POS_LAUNCH, DONE , ERROR, sensRobot, avoidance, END_AT_BREAK);
			//state = try_going(dplt[1].point.x,dplt[1].point.y,POS_LAUNCH,POS_SHOOT,ERROR,FAST,sensRobot,NO_DODGE_AND_WAIT);

			if(global.env.prop.reach_y)		//Peu probable... mais par sécurité (si on était super lent, on va peut etre recevoir le warner avant le point de freinage
			{
				// Si nous avons echoué le tire de l'autre balle sur le mammouth adverse, onrevient tirer la balle sur notre mammouth
				if(get_sub_act_enable(SUB_LANCE_ADV_FAIL) && !get_sub_act_done(SUB_LANCE_ADV_FAIL)){
					ACT_lance_launcher_run(ACT_Lance_1_BALL,sensShoot);
					set_sub_act_done(SUB_LANCE_ADV_FAIL,TRUE);
					launcher_ball_adversary = TRUE;
				}else{
					ACT_lance_launcher_run((lanceAll)?ACT_Lance_ALL:((mammouth==global.env.color)?ACT_Lance_5_BALL:ACT_Lance_1_BALL),sensShoot);
					set_sub_act_done((mammouth == global.env.color)?SUB_LANCE:SUB_LANCE_ADV,TRUE);	//On lance, donc l'action est faite...

					if(mammouth == global.env.color)
						lance_ball = TRUE;
					else
						launcher_ball_adversary = TRUE;
				}
			}
			break;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case GET_OUT_WITH_ERROR :
			//On recherche le point de sortie le plus proche.
			if(global.env.pos.x > 530)	//Le point actuel est acceptable comme sortie
				state = RETURN_NOT_HANDLED;
			else
			{
				if(absolute(global.env.pos.y - 1500) < 300)	//Je suis a moins de 300 de l'axe de symétrie du terrain
					state = RETURN_NOT_HANDLED;		//Je peux rendre la main
				else
					state = GOBACK_FIRST_POINT;		//Je suis trop proche du bac pour pouvoir rendre la main
			}
			break;
		case GOBACK_FIRST_POINT:
			state = try_going(dplt[0].point.x,dplt[0].point.y,GOBACK_FIRST_POINT,RETURN_NOT_HANDLED,GOBACK_LAST_POINT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GOBACK_LAST_POINT:
			state = try_going(dplt[2].point.x,dplt[2].point.y,GOBACK_LAST_POINT,RETURN_NOT_HANDLED,GOBACK_FIRST_POINT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case DONE:
			state = IDLE;

			return END_OK;
			break;

		case RETURN_NOT_HANDLED :
			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}
