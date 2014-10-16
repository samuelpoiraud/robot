/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP
 *
 *	Fichier : avoidance.c
 *	Package : Carte Principale
 *	Description : Actions relatives au déplacement avec evitement
 *	Auteur : Jacen
 *	Version 20110426
 */

#include "avoidance.h"
#include "can_utils.h"
#include "QS/QS_maths.h"
#include "QS/QS_who_am_i.h"
#include "prop_functions.h"
#include "config_use.h"
#include "QS/QS_outputlog.h"
#include "config/config_pin.h"
#include "config/config_debug.h"
#include "Supervision/Buzzer.h"
#include "Supervision/SD/SD.h"
#include "math.h"
#include "state_machine_helper.h"
#include "QS/QS_maths.h"
#include "QS/QS_IHM.h"

#define WAIT_TIME_DETECTION			1000	//[ms] temps pendant lequel on attend que l'adversaire s'en aille. Ensuite, on abandonne la trajectoire.
#define FOE_IS_LEFT_TIME			250		//[ms] temps depuis lequel l'adversaire doit être parti pour que l'on reprenne notre trajectoire.
#define EXTRACTION_DISTANCE			300

static error_e AVOIDANCE_watch_prop_stack();
static error_e extraction_of_foe(PROP_speed_e speed);
static error_e goto_extract_with_avoidance(const displacement_t displacements);
static error_e wait_move_and_scan_foe2(avoidance_type_e avoidance_type);
static error_e wait_move_and_wait_foe();


static Uint16 wait_timeout = WAIT_TIME_DETECTION;
static bool_e prop_detected_foe = FALSE;

#ifdef DEBUG_AVOIDANCE
	#define avoidance_printf(format, ...)	debug_printf("t=%lums " format, global.env.match_time, ##__VA_ARGS__)
#else
	#define avoidance_printf(...)	(void)0
#endif

//------------------------------------------------------------------- Machines à états de déplacements

//Action qui gere un déplacement et renvoi le state rentré en arg. Ne s'arrète qu'à la fin que si aucun autre déplacement n'est demandé.
Uint8 try_going(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition)
{
	error_e sub_action;
	//sub_action = goto_pos_with_scan_foe((displacement_t[]){{{x, y},FAST}},1,way,avoidance);
	sub_action = goto_pos_curve_with_avoidance((displacement_t[]){{{x, y},speed}}, NULL, 1, way, avoidance, end_condition);
	switch(sub_action){
		case IN_PROGRESS:
			return in_progress;
			break;
		case FOE_IN_PATH:
		case NOT_HANDLED:
			return fail_state;
			break;
		case END_OK:
		case END_WITH_TIMEOUT:
		default:
			return success_state;
			break;
	}
	return in_progress;
}

//Action qui gere un déplacement et renvoi le state rentré en arg. Cette fonction permet le multipoint.
Uint8 try_going_multipoint(const displacement_t displacements[], Uint8 nb_displacements, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition)
{
	error_e sub_action;
	sub_action = goto_pos_curve_with_avoidance(displacements, NULL, nb_displacements, way, avoidance, end_condition);
	switch(sub_action){
		case IN_PROGRESS:
			return in_progress;
			break;
		case FOE_IN_PATH:
		case NOT_HANDLED:
		case END_WITH_TIMEOUT:
			return fail_state;
			break;
		case END_OK:
		default:
			return success_state;
			break;
	}
	return in_progress;
}

//Action qui gere un changement d'angle et renvoi le state rentré en arg.
Uint8 try_go_angle(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed)
{
	enum state_e
	{
		EMPILE,
		WAIT,
		DONE
	};
	static enum state_e state = EMPILE;
	Uint8 ret;
	static bool_e timeout;
	ret = in_progress;
	switch(state)
	{
		case EMPILE:
			PROP_push_goangle(angle, speed, TRUE);
			state = WAIT;
			break;
		case WAIT:
			if(STACKS_wait_end_auto_pull(PROP, &timeout)){
				state = DONE;
			}
			break;
		case DONE:
			state = EMPILE;
			ret = (timeout)? fail_state : success_state;
			break;
		default:
			state = EMPILE;
			ret = fail_state;
			break;
	}
	return ret;
}

// Fait avancer le robot vers un points jusqu'a ce qu'il rencontre un enemie ou un obstacle
Uint8 try_rush(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance){
	CREATE_MAE_WITH_VERBOSE(0x00,
			IDLE,
			FAST_ROTATE,
			INIT_COEF,
			GO,
			ERROR,
			DONE
		);

	error_e sub_action;
	static Sint16 angle;

	switch(state){
		case IDLE :
			state = FAST_ROTATE;
			break;

		case FAST_ROTATE:
			if(entrance){
				angle = atan2(y-global.env.pos.y, x-global.env.pos.x)*4096;
				if(angle > 0)
					angle -= (way == BACKWARD)? PI4096 : 0;
				else
					angle += (way == BACKWARD)? PI4096 : 0;
			}
			state = try_go_angle(angle, FAST_ROTATE, INIT_COEF, ERROR, FAST);
			break;

		case INIT_COEF:
			PROP_set_threshold_error_translation(50,FALSE);
			state = GO;
			break;

		case GO :
			sub_action = goto_pos_curve_with_avoidance((displacement_t[]){{{x, y},30}}, NULL, 1, way, avoidance, END_AT_LAST_POINT);
			switch(sub_action){
				case IN_PROGRESS:
					state = GO;
					break;
				case FOE_IN_PATH:
					state = ERROR;
					break;
				case NOT_HANDLED:
				case END_OK:
				case END_WITH_TIMEOUT:
				default:
					state = DONE;
					break;
			}
			break;

		case ERROR :
			PROP_set_threshold_error_translation(0,TRUE);
			state = IDLE;
			return fail_state;

		case DONE :
			PROP_set_threshold_error_translation(0,TRUE);
			state = IDLE;
			return success_state;
	}
	return in_progress;
}


//Action qui gere un déplacement en avance ou arrière et renvoi le state rentré en arg.
Uint8 try_advance(Uint16 dist, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition)
{
	static GEOMETRY_point_t point;
	enum state_e{
		COMPUTE,
		GO
	};

	Sint16 cos,sin;
	static enum state_e state = COMPUTE;
	Uint8 return_state = in_progress;

	switch(state){
		case COMPUTE :
			COS_SIN_4096_get((way == FORWARD)? global.env.pos.angle:global.env.pos.angle+PI4096, &cos, &sin);

			point.x = (Sint32)cos*dist/4096 + global.env.pos.x;
			point.y = (Sint32)sin*dist/4096 + global.env.pos.y;
			state = GO;
			break;

		case GO :
			return_state = try_going(point.x, point.y, in_progress, success_state, fail_state, speed, way, avoidance, end_condition);
			if(return_state == success_state || return_state == fail_state)
				state = COMPUTE;
			break;
	}
	return return_state;
}

//Action qui gere l'arret du robot et renvoi le state rentré en arg.
Uint8 try_stop(Uint8 in_progress, Uint8 success_state, Uint8 fail_state)
{
	typedef enum{
		STOP,
		WAIT_AND_CHECK
	}state_e;
	static state_e state = STOP;

	error_e subaction;

	switch(state){
		case STOP :
			PROP_push_stop();
			state = WAIT_AND_CHECK;
			break;

		case WAIT_AND_CHECK :
			subaction = AVOIDANCE_watch_prop_stack();
			switch (subaction) {
				case END_OK:
					debug_printf("PROP_STOP effectué\n");
					state = STOP;
					return success_state;
					break;

				case IN_PROGRESS:
					break;

				case END_WITH_TIMEOUT:
					debug_printf("PROP_STOP effectué avec TIMEOUT\n");
					state = STOP;
					return fail_state;
					break;

				case NOT_HANDLED:
					debug_printf("PROP_STOP erreur\n");
					state = STOP;
					return fail_state;
					break;

				default:
					debug_printf("PROP_STOP erreur\n");
					state = STOP;
					return fail_state;
					break;
			}
			break;
	}
	return in_progress;
}


//------------------------------------------------------------------- Fonctions relatives à l'évitement

//Version simplifiée de wait_move_and_scan_foe. on esquive pas la cible ici. (pas de dodge)
/*
 * Fonction qui regarde si l'adversaire est devant nous pendant un mouvement, et on l'évite si nécessaire
 * Elle doit être appelée à la place de STACKS_wait_end_auto_pull (c'est géré dans cette fonction)
 *
 * pre : Etre sur le terrain
 * post : Pile PROP vidée
 * param : nombre de mouvements chargés dans la pile
 *
 * return IN_PROGRESS : En cours
 * return END_OK : Terminé
 * return NOT_HANDLED : Action impossible, ou timeout normal
 * return END_WITH_TIMEOUT : Adversaire rencontré, mais on est arrivé à destination
 */
static error_e wait_move_and_scan_foe2(avoidance_type_e avoidance_type) {
	enum state_e
	{
		INITIALIZATION = 0,
		NO_FOE,
		WAIT_STOP,
		WAIT_FOE
	};
	static enum state_e state = INITIALIZATION;

	static time32_t avoidance_timeout_time = 0;
	static time32_t last_match_time;
	static bool_e debug_foe_forced = FALSE;
	static time32_t no_foe_count;
	time32_t current_match_time = global.env.match_time;

	bool_e timeout;
	error_e ret = IN_PROGRESS;

	//Si pas d'évitement, on fait pas d'évitement
	if(avoidance_type == NO_AVOIDANCE)
	{
		error_e prop_stack_state = AVOIDANCE_watch_prop_stack();
		switch(prop_stack_state)
		{
			case IN_PROGRESS:
				break;

			case END_OK:
			case END_WITH_TIMEOUT:
			case NOT_HANDLED:
				avoidance_printf("wait_move_and_scan_foe: end state = %d\n", prop_stack_state);
				ret = prop_stack_state;
				break;

			default: //Ne devrait jamais arriver, AVOIDANCE_watch_prop_stack ne doit pas retourner FOE_IN_PATH car elle ne gère pas d'evitement
				avoidance_printf("wait_move_and_scan_foe: DEFAULT prop_stack_state = %d!!\n", prop_stack_state);
				ret = NOT_HANDLED;
				break;
		}
	}
	else
	{
		switch(state)
		{
			case INITIALIZATION:
				// initialisation des variables statiques
				avoidance_timeout_time = 0;
				debug_foe_forced = FALSE;

				avoidance_printf("wait_move_and_scan_foe: initialized\n");
				state = NO_FOE;
				break;

			case NO_FOE:

				if(global.env.debug_force_foe)	//Evitement manuel forcé !
				{
					STACKS_flush(PROP);
					debug_foe_reason(FORCED_BY_USER, 0, 0);
					PROP_push_stop();
					state = WAIT_STOP;
					global.env.debug_force_foe = FALSE;
					debug_foe_forced = TRUE;	//Nous allons juste attendre le stop.. et puis on retournera un FOE_IN_PATH.
				}
				else
				{
					//Si on effectue un translation, c'est qu'on est en direction du point voulu (si le point était sur notre gauche, on aura fait une rotation au préalable)
					//Necessaire pour que l'angle de detection de l'adversaire soit valide (car sinon on ne pointe pas forcément vers notre point d'arrivé ...)
					//On considère ici que si la prop faire une translation, le robot pointe vers le point d'arrivée
		//			if((global.env.prop.current_trajectory != TRAJECTORY_TRANSLATION && global.env.prop.current_trajectory != TRAJECTORY_AUTOMATIC_CURVE) &&
		//				(is_in_path[FOE_1] || is_in_path[FOE_2]))
		//				avoidance_printf("Not in translation but foe in path\n");

					if(global.env.prop.is_in_translation && foe_in_path(TRUE))	//Si un adversaire est sur le chemin
					{	//On ne peut pas inclure le test du type de trajectoire dans le foe_in_path car ce foe_in_path sert également à l'arrêt, une fois qu'on a vu l'adversaire.
						//debug_foe_reason(foe, global.env.foe[foe].angle, global.env.foe[foe].dist);
						//debug_printf("IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
						BUZZER_play(20, DEFAULT_NOTE, 3);
						switch(avoidance_type)
						{
							case NORMAL_WAIT:
							case NO_DODGE_AND_WAIT:
							case DODGE_AND_WAIT:
								avoidance_printf("wait_move_and_scan_foe: foe detected, waiting\n");

								// adversaire détecté ! on s'arrête !
								STACKS_push(PROP, &wait_forever, FALSE);
								PROP_push_stop();
								// un adversaire est détecté devant nous, on attend de s'arreter avant de voir s'il est toujours la
								state = WAIT_STOP;
								break;
							case DODGE_AND_NO_WAIT:
							case NO_DODGE_AND_NO_WAIT:
								avoidance_printf("wait_move_and_scan_foe: foe detected\n");
								STACKS_flush(PROP);
								PROP_push_stop();
								state = WAIT_STOP;
								break;

							case NO_AVOIDANCE: //Géré au début de la fonction
							default:
								state = INITIALIZATION;
								break;
						}
					}
					else
					{
						// aucun adversaire n'est détecté, on fait notre mouvement normalement
						// on regarde si la pile s'est vidée
						error_e prop_stack_state = AVOIDANCE_watch_prop_stack();
						switch(prop_stack_state)
						{
							case END_OK:
							case END_WITH_TIMEOUT:
							case NOT_HANDLED:
								avoidance_printf("wait_move_and_scan_foe: end no foe state = %d\n", prop_stack_state);
								ret = prop_stack_state;
								break;
							case IN_PROGRESS:
								break;
							default:
								break;
						}
					}
				}
				break;

			case WAIT_STOP:
				//Quand on s'est arreté, on regarde si l'adversaire est toujours devant nous avant de redémarrer
				if(STACKS_wait_end_auto_pull(PROP, &timeout))
				{
					if(debug_foe_forced)
					{			//L'evitement a été forcé pour debuggage, on sort direct
						avoidance_printf("wait_move_and_scan_foe: forced foe detection, returning FOE_IN_PATH\n");
						debug_foe_forced = FALSE;
						ret = FOE_IN_PATH;
					}
					else
					{
						switch(avoidance_type) {
							case NORMAL_WAIT:
							case NO_DODGE_AND_WAIT:
							case DODGE_AND_WAIT:
								no_foe_count = 0;
								state = WAIT_FOE;
								break;

							case DODGE_AND_NO_WAIT:
							case NO_DODGE_AND_NO_WAIT:
								ret = FOE_IN_PATH;
								break;
							case NO_AVOIDANCE: //Géré au début de la fonction
							default:
								state = INITIALIZATION;
								break;
						}
					}
				}
				break;

			case WAIT_FOE:
				//debug_printf("Test 2: IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
				// on va attendre que l'ennemi sorte de notre chemin
				//On ne regarde pas ici si le robot pointe vers le point d'arrivée car il a été arreté en pleine translation vers ce point
				avoidance_timeout_time += current_match_time - last_match_time;
				if(avoidance_timeout_time >= wait_timeout)
				{
					avoidance_printf("wait_move_and_scan_foe: timeout avec ennemi sur path\n");
					ret = FOE_IN_PATH;
				}
				else if(foe_in_path(FALSE))	//Si on vient de recevoir un update de sa position et qu'il est toujours devant nous...
				{
					BUZZER_play(20, DEFAULT_NOTE, 1);
					no_foe_count = 0;	//On reset le compteur de no_foe.
				}
				else
				{
					no_foe_count += current_match_time - last_match_time;
					if(no_foe_count >= FOE_IS_LEFT_TIME) 	//L'adversaire est parti depuis FOE_IS_LEFT_TIME ms
					{
						avoidance_printf("wait_move_and_scan_foe: no more foe, continuing\n");
						STACKS_pull(PROP);	// on vire le wait_forever et on lance l'action suivante
						state = NO_FOE;	// adversaire n'est plus dans notre chemin, on reprend le mouvement normal
					}
				}
				break;
			default:
				debug_printf("PROBLEME, on est dans le default !\n");
				ret = NOT_HANDLED;
				break;
		}
	}
	last_match_time = current_match_time;
	if(ret != IN_PROGRESS)
	{
		STACKS_flush(PROP);	//TIMEOUT -> ON VIDE LE BUFFER PROPREMENT.
		state = INITIALIZATION;
	}
	return ret;
}

//Version simplifiée de wait_move_and_scan_foe. on esquive pas la cible ici. (pas de dodge)
static error_e wait_move_and_wait_foe() {


	error_e ret = IN_PROGRESS;

	// check fin de trajectoire
	error_e prop_stack_state = AVOIDANCE_watch_prop_stack();
	switch(prop_stack_state)
	{
		case IN_PROGRESS:
			break;

		case END_OK:
		case END_WITH_TIMEOUT:
		case NOT_HANDLED:
			avoidance_printf("wait_move_and_wait_foe: end state = %d\n", prop_stack_state);
			ret = prop_stack_state;
			break;

		default: //Ne devrait jamais arriver, AVOIDANCE_watch_prop_stack ne doit pas retourner FOE_IN_PATH car elle ne gère pas d'evitement
			avoidance_printf("wait_move_and_wait_foe: DEFAULT prop_stack_state = %d!!\n", prop_stack_state);
			ret = NOT_HANDLED;
			break;
	}

	// check adversaire détecté
	if(prop_detected_foe){
		STACKS_flush(PROP);
		ret = FOE_IN_PATH;
	}

	return ret;
}


void AVOIDANCE_forced_foe_dected(){
	CAN_msg_t msg;
	msg.sid = PROP_DEBUG_FORCED_FOE;
	msg.size = 0;
	CAN_send(&msg);
}


void AVOIDANCE_set_timeout(Uint16 msec) {
	wait_timeout = msec;
}

/*
 * Fonction qui réalise un PROP_push_goto tout simple avec la gestion de l'évitement (en courbe)
 *
 * pre : Etre sur le terrain
 * post : Robot aux coordonnées voulues
 *
 * param displacements : deplacements de la trajectoire
 * param nb_displacement : nombre de deplacements de la trajectoire
 * param way : sens de déplacement
 * end_condition : doit on finir quand on freine sur le dernier point ou quand on y est ?
 *
 * return IN_PROGRESS : En cours
 * return END_OK : Terminé
 * return NOT_HANDLED : Action impossible
 * return END_WITH_TIMEOUT : Timeout
 * return FOE_IN_PATH : un adversaire nous bloque
 */
error_e goto_pos_curve_with_avoidance(const displacement_t displacements[], const displacement_curve_t displacements_curve[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, PROP_end_condition_e end_condition)
{
	enum state_e
	{
		CHECK_SCAN_FOE = 0,
		LOAD_MOVE,
		WAIT_MOVE_AND_SCAN_FOE,
		EXTRACT,
		DONE
	};
	#ifdef USE_PROP_AVOIDANCE
		static enum state_e state = LOAD_MOVE;
	#else
		static enum state_e state = CHECK_SCAN_FOE;
	#endif

	static bool_e timeout = FALSE;
	static error_e sub_action;

	Uint8 i;

	//Si nouveau déplacement et qu'aucun point n'est donné, on a rien a faire
	#ifdef USE_PROP_AVOIDANCE
		if(state == LOAD_MOVE && nb_displacements == 0)
			return END_OK;
	#else
		if(state == CHECK_SCAN_FOE && nb_displacements == 0)
			return END_OK;
	#endif

	switch(state)
	{
		case CHECK_SCAN_FOE :
			if(avoidance_type != NO_AVOIDANCE && foe_in_zone(TRUE, displacements[0].point.x, displacements[0].point.y, FALSE)){
				avoidance_printf("goto_pos_with_scan_foe NOT HANDLED because foe in target zone");
				state = CHECK_SCAN_FOE;
				return FOE_IN_PATH;
			}else
				state = LOAD_MOVE;
			break;


		case LOAD_MOVE:
			timeout = FALSE;
			clear_prop_detected_foe();
			global.env.destination = displacements[nb_displacements-1].point;
			for(i=nb_displacements-1;i>=1;i--)
			{
				if(displacements)
					PROP_push_goto_multi_point(displacements[i].point.x, displacements[i].point.y, displacements[i].speed, way, PROP_CURVES, avoidance_type, END_OF_BUFFER, end_condition, FALSE);
				else if(displacements_curve)
					PROP_push_goto_multi_point(displacements_curve[i].point.x, displacements_curve[i].point.y, displacements_curve[i].speed, way, displacements_curve[i].curve?PROP_CURVES:0, avoidance_type, END_OF_BUFFER, end_condition, FALSE);
			}
			if(displacements)
				PROP_push_goto_multi_point(displacements[0].point.x, displacements[0].point.y, displacements[0].speed, way, PROP_CURVES, avoidance_type, END_OF_BUFFER, end_condition, TRUE);
			else if(displacements_curve)
				PROP_push_goto_multi_point(displacements_curve[0].point.x, displacements_curve[0].point.y, displacements_curve[0].speed, way, displacements_curve[0].curve?PROP_CURVES:0, avoidance_type, END_OF_BUFFER, end_condition, TRUE);

			avoidance_printf("goto_pos_with_scan_foe : load_move\n");
			if(displacements || displacements_curve)
				state = WAIT_MOVE_AND_SCAN_FOE;
			else
			{
				#ifdef USE_PROP_AVOIDANCE
					state = LOAD_MOVE;
				#else
					state = CHECK_SCAN_FOE;
				#endif
				return NOT_HANDLED;
			}
			break;

		case WAIT_MOVE_AND_SCAN_FOE:
			#ifdef USE_PROP_AVOIDANCE
				sub_action = wait_move_and_wait_foe();
			#else
				sub_action = wait_move_and_scan_foe2(avoidance_type);
			#endif

			switch(sub_action)
			{
				case END_OK:
					avoidance_printf("wait_move_and_scan_foe -- fini\n");
					state = DONE;
					break;

				case END_WITH_TIMEOUT:
					timeout = TRUE;
					avoidance_printf("wait_move_and_scan_foe -- timeout\n");
					SD_printf("TIMEOUT on WAIT_MOVE_AND_SCAN_FOE\n");
					state = DONE;
					break;

				case NOT_HANDLED:
					avoidance_printf("wait_move_and_scan_foe -- probleme\n");
					SD_printf("ERROR on WAIT_MOVE_AND_SCAN_FOE\n");
					wait_timeout = WAIT_TIME_DETECTION;
					#ifdef USE_PROP_AVOIDANCE
						state = LOAD_MOVE;
					#else
						state = CHECK_SCAN_FOE;
					#endif
					return NOT_HANDLED;
					break;

				case FOE_IN_PATH:
					avoidance_printf("wait_move_and_scan_foe -- foe in path\n");
					SD_printf("FOE_IN_PATH on WAIT_MOVE_AND_SCAN_FOE\n");
					wait_timeout = WAIT_TIME_DETECTION;
					if(avoidance_type == DODGE_AND_WAIT || avoidance_type == DODGE_AND_NO_WAIT)
						state = EXTRACT;
					else
					{
						#ifdef USE_PROP_AVOIDANCE
							state = LOAD_MOVE;
						#else
							state = CHECK_SCAN_FOE;
						#endif
						return FOE_IN_PATH;			//Pas d'extraction demandée... on retourne tel quel FOE_IN_PATH !
					}
					break;

				case IN_PROGRESS:
					break;

				default:
					#ifdef USE_PROP_AVOIDANCE
						state = LOAD_MOVE;
					#else
						state = CHECK_SCAN_FOE;
					#endif
					return NOT_HANDLED;
					break;
			}
			break;

		case EXTRACT:
			sub_action = extraction_of_foe(FAST);//SLOW_TRANSLATION_AND_FAST_ROTATION);
			switch(sub_action)
			{
				case END_OK:
					#ifdef USE_PROP_AVOIDANCE
						state = LOAD_MOVE;
					#else
						state = CHECK_SCAN_FOE;
					#endif
					return FOE_IN_PATH;
					break;

				case END_WITH_TIMEOUT:
					timeout = TRUE;
					state = DONE;
					break;

				case NOT_HANDLED:
					#ifdef USE_PROP_AVOIDANCE
						state = LOAD_MOVE;
					#else
						state = CHECK_SCAN_FOE;
					#endif
					return NOT_HANDLED;
					break;

				case FOE_IN_PATH:
					#ifdef USE_PROP_AVOIDANCE
						state = LOAD_MOVE;
					#else
						state = CHECK_SCAN_FOE;
					#endif
					return FOE_IN_PATH;
					break;

				case IN_PROGRESS:
					break;

				default:
					#ifdef USE_PROP_AVOIDANCE
						state = LOAD_MOVE;
					#else
						state = CHECK_SCAN_FOE;
					#endif
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			wait_timeout = WAIT_TIME_DETECTION;
			#ifdef USE_PROP_AVOIDANCE
				state = LOAD_MOVE;
			#else
				state = CHECK_SCAN_FOE;
			#endif
			return timeout?END_WITH_TIMEOUT:END_OK;
			break;

		default:
			debug_printf("Cas Default state, panique !!!\n");
			#ifdef USE_PROP_AVOIDANCE
				state = LOAD_MOVE;
			#else
				state = CHECK_SCAN_FOE;
			#endif
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}



/* ----------------------------------------------------------------------------- */
/* 		Fonctions de scrutation de la position de l'adversaire		   */
/* ----------------------------------------------------------------------------- */

/* Fonction qui regarde si le robot est dans notre chemin */
//Pas en static pour tests
/*static*/
bool_e foe_in_path(bool_e verbose)
{
	bool_e in_path;
	Sint16 cosinus, sinus;
	Sint32 relative_foe_x;
	Sint32 relative_foe_y;
	Uint8 i;
	way_e move_way;
	Uint32 breaking_acceleration;
	Uint32 current_speed;
	Uint32 break_distance;
	Uint32 respect_distance;
	Sint32 avoidance_rectangle_min_x;
	Sint32 avoidance_rectangle_max_x;
	Sint32 avoidance_rectangle_width_y;

	move_way = global.env.prop.current_way;	//TODO cracra.. a nettoyer ultérieurement.

	in_path = FALSE;	//On suppose que pas d'adversaire dans le chemin

	if (!IHM_switchs_get(SWITCH_EVIT))
	{	//évitement désactivé par l'interrupteur
		static bool_e already_printed_debug_no_evitement = FALSE;
		if(already_printed_debug_no_evitement == FALSE) {
			already_printed_debug_no_evitement = TRUE;
			debug_printf("\n\nEVIT disabled by the switch. This message is printed only once.\n");
		}
		return FALSE;
	}

	/*	On définit un "rectangle d'évitement" comme la somme :
	 * 		- du rectangle que le robot va recouvrir s'il décide de freiner maintenant.
	 *  	- du rectangle de "respect" qui nous sépare de l'adversaire lorsqu'on se sera arreté
	 *  On calcule la position relative des robots adverses pour savoir s'ils se trouvent dans ce rectangle
	 *
	 */

	breaking_acceleration = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_ACCELERATION_NORMAL:BIG_ROBOT_ACCELERATION_NORMAL;
	current_speed = (Uint32)(absolute(global.env.pos.translation_speed)*1);
	break_distance = current_speed*current_speed/(2*breaking_acceleration);	//distance que l'on va parcourir si l'on décide de freiner maintenant.
	respect_distance = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_RESPECT_DIST_MIN:BIG_ROBOT_RESPECT_DIST_MIN;	//Distance à laquelle on souhaite s'arrêter

	if(move_way == FORWARD || move_way == ANY_WAY)	//On avance
		avoidance_rectangle_max_x = break_distance + respect_distance;
	else
		avoidance_rectangle_max_x = 0;

	if(move_way == BACKWARD || move_way == ANY_WAY)	//On recule
		avoidance_rectangle_min_x = -(break_distance + respect_distance);
	else
		avoidance_rectangle_min_x = 0;

	avoidance_rectangle_width_y = FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH);
	//avoidance_printf("\n%d[%ld>%ld][%ld>%ld]\n", current_speed,avoidance_rectangle_min_x,avoidance_rectangle_max_x,-avoidance_rectangle_width_y/2,avoidance_rectangle_width_y/2);

	for (i=0; i<MAX_NB_FOES; i++)
	{
		if (global.env.foe[i].enable)
		{
			COS_SIN_4096_get(global.env.foe[i].angle, &cosinus, &sinus);
			relative_foe_x = (((Sint32)(cosinus)) * global.env.foe[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]
			relative_foe_y = (((Sint32)(sinus))   * global.env.foe[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]

			if(		relative_foe_y > -avoidance_rectangle_width_y/2 	&& 	relative_foe_y < avoidance_rectangle_width_y/2
				&& 	relative_foe_x > avoidance_rectangle_min_x 		&& 	relative_foe_x < avoidance_rectangle_max_x)
				{
					in_path = TRUE;	//On est dans le rectangle d'évitement !!!
					if(verbose)
						SD_printf("FOE[%d]_IN_PATH|%c|d:%d|a:%d|rel_x%ld|rel_y%ld   RECT:[%ld>%ld][%ld>%ld]\n", i, (global.env.foe[i].from == DETECTION_FROM_BEACON_IR)?'B':'H',global.env.foe[i].dist, global.env.foe[i].angle, relative_foe_x, relative_foe_y,avoidance_rectangle_min_x,avoidance_rectangle_max_x,-avoidance_rectangle_width_y/2,avoidance_rectangle_width_y/2);
				}
		}
	}
	return in_path;
}

// Retourne si un adversaire est dans la chemin entre nous et la position
bool_e foe_in_zone(bool_e verbose, Sint16 x, Sint16 y, bool_e check_on_all_traject){
	bool_e inZone;
	Uint8 i;
	Sint32 a, b, c; // avec a, b et c les coefficients de la droite entre nous et la cible
	Sint32 NCx, NCy, NAx, NAy;

	a = y - global.env.pos.y;
	b = -(x - global.env.pos.x);
	c = -(Sint32)global.env.pos.x*y + (Sint32)global.env.pos.y*x;

	if(global.env.pos.x == x && global.env.pos.y == y)
		return FALSE;


	inZone = FALSE;	//On suppose que pas d'adversaire dans le chemin

	for (i=0; i<MAX_NB_FOES; i++)
	{
		if (global.env.foe[i].enable){
			// d(D, A) < L
			// D : droite que le robot empreinte pour aller au point
			// A : Point adversaire
			// L : Largeur du robot maximum * 2
			if(verbose)
				debug_printf("Nous x:%d y:%d / ad x:%d y:%d / destination x:%d y:%d /\n ", global.env.pos.x, global.env.pos.y, global.env.foe[i].x, global.env.foe[i].y,x,y);

			if((QS_WHO_AM_I_get() == BIG_ROBOT && absolute((Sint32)a*global.env.foe[i].x + (Sint32)b*global.env.foe[i].y + c) / (Sint32)sqrt((Sint32)a*a + (Sint32)b*b) < MARGE_COULOIR_EVITEMENT_STATIC_BIG_ROBOT)
					|| (QS_WHO_AM_I_get() == SMALL_ROBOT && absolute((Sint32)a*global.env.foe[i].x + (Sint32)b*global.env.foe[i].y + c) / (Sint32)sqrt((Sint32)a*a + (Sint32)b*b) < MARGE_COULOIR_EVITEMENT_STATIC_SMALL_ROBOT)){
				// /NC./NA ¤ [0,NC*d]
				// /NC : Vecteur entre nous et le point cible
				// /NA : Vecteur entre nous et l'adversaire
				// NC : Distance entre nous et le point cible
				// d : Distance d'évitement de l'adversaire (longueur couloir)

				NCx = x - global.env.pos.x;
				NCy = y - global.env.pos.y;

				NAx = global.env.foe[i].x - global.env.pos.x;
				NAy = global.env.foe[i].y - global.env.pos.y;


				if((NCx*NAx + NCy*NAy) >= (Sint32)dist_point_to_point(global.env.pos.x, global.env.pos.y, x, y)*100
						&& (
							(!check_on_all_traject &&(NCx*NAx + NCy*NAy) < (Sint32)dist_point_to_point(global.env.pos.x, global.env.pos.y, x, y)*DISTANCE_EVITEMENT_STATIC)
							||
							(check_on_all_traject &&(NCx*NAx + NCy*NAy) < SQUARE((Sint32)dist_point_to_point(global.env.pos.x, global.env.pos.y, x, y))))){
					inZone = TRUE;
					if(verbose)
						debug_printf("DETECTED\n");
				}else{
					if(verbose)
						debug_printf("NO\n");
				}
			}else{
				if(verbose)
					debug_printf("NO\n");
			}
		}
	}
	return inZone;
}

//Retourne si un adversaire est dans le carré dont deux coins 1 et 2 sont passés en paramètres
bool_e foe_in_square(bool_e verbose, Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2)
{
	Uint8 i;
	for (i=0; i<MAX_NB_FOES; i++)
	{
		if (global.env.foe[i].enable)
		{
			if(is_in_square(x1,x2,y1,y2,(GEOMETRY_point_t){global.env.foe[i].x,global.env.foe[i].y}))
			{
				if(verbose)
					SD_printf("FOE %d[%d;%d] found in zone x[%d->%d] y[%d->%d]\n",i,global.env.foe[i].x,global.env.foe[i].y,x1,x2,y1,y2);
				return TRUE;
			}
		}
	}
	SD_printf("NO FOE found in zone x[%d->%d] y[%d->%d]\n",x1,x2,y1,y2);
	return FALSE;
}

//Un point est-t-il une position permettant les rotations sans tapper dans un élément de jeu.
bool_e is_possible_point_for_rotation(GEOMETRY_point_t * p)
{
	Uint8 widthRobot;
	widthRobot =  (QS_WHO_AM_I_get() == BIG_ROBOT)? BIG_ROBOT_WIDTH/2 : SMALL_ROBOT_WIDTH/2;
	widthRobot += 100;	//Marge !

	if(!is_in_square(0+(widthRobot), 2000-(widthRobot), 0+(widthRobot), 3000-(widthRobot), *p))			// Terrain
		return FALSE;
	if(is_in_circle(*p,(GEOMETRY_circle_t){(GEOMETRY_point_t){1050, 1500}, 150+widthRobot}))				// Foyer centre
		return FALSE;
	if(is_in_circle(*p, (GEOMETRY_circle_t){(GEOMETRY_point_t){2000, 0}, 125+widthRobot}))				// Foyer droite
		return FALSE;
	if(is_in_circle(*p, (GEOMETRY_circle_t){(GEOMETRY_point_t){2000, 3000}, 125+widthRobot}))			// Foyer gauche
		return FALSE;
	if(is_in_square(0-(widthRobot), 300+(widthRobot), 400-(widthRobot), 1100+(widthRobot), *p))			// Bac à fruit jaune
		return FALSE;
	if(is_in_square(0-(widthRobot), 300+(widthRobot), 1900-(widthRobot), 2600+(widthRobot), *p))			// Bac à fruit rouge
		return FALSE;
	if(is_in_circle(*p, (GEOMETRY_circle_t){(GEOMETRY_point_t){1300, 0}, 150+widthRobot}))				// Arbre rouge 1
		return FALSE;
	if(is_in_circle(*p, (GEOMETRY_circle_t){(GEOMETRY_point_t){2000, 700}, 150+widthRobot}))				// Arbre rouge 2
		return FALSE;
	if(is_in_circle(*p, (GEOMETRY_circle_t){(GEOMETRY_point_t){2000, 2300}, 150+widthRobot}))			// Arbre jaune 1
		return FALSE;
	if(is_in_circle(*p, (GEOMETRY_circle_t){(GEOMETRY_point_t){1300, 3000}, 150+widthRobot}))			// Arbre jaune 2
		return FALSE;

	return  TRUE;
}

/*	Trouve une extraction lorsqu'un ou plusieurs ennemi(s) qui nous pose(nt) problème */
static error_e extraction_of_foe(PROP_speed_e speed){
	CREATE_MAE_WITH_VERBOSE(SM_ID_EXTRACTION_OF_FOE,
		IDLE,
		COMPUTE,
		GO_POINT,
		TURN_TRIGO,
		TURN_HORAIRE,
		WAIT,
		DONE,
		ERROR
	);
	static Uint8 remaining_try;
	static Uint8 sens = TURN_TRIGO;							//Si il arrive pas à trouver un point au bout de 3 coups tourne sur lui-même pour permettre à l'hokuyo de voir partout
	static Uint8 bestPoint;									//Indice du meilleur point dans le tableau
	Uint32 bestPoint_distance2_with_nearest_foe;		//Distance entre le meilleur point et l'adversaire qui en est le plus proche
	Uint32 adversary_to_close_distance;						//Distance minimale exigée entre un adversaire et un point candidat
	static GEOMETRY_point_t pointEx[12];					//Incroyable mais VRAI, idée de GRAVOUILLE d'utliser un tableau au lieu de calcul d'angle
	Uint8 i,foe;
	Sint16 cos,sin;
	Uint32 distance2_between_point_and_foe;					//Distance au carré entre le point courant et l'adversaire courant
	Uint32 distance2_between_point_and_foe_min;				//Distance au carré entre le point courant l'adversaire le plus proche trouvé
	bool_e can_i_go_to_point = FALSE;
	bool_e i_can_turn_in_my_position = FALSE;

	switch(state){
		case IDLE:
			remaining_try = 3;
			state = COMPUTE;
			break;
		case COMPUTE:

			// Algo de recherche du meilleur point....
			// Parmi les 12 points candidats (tout les 30°, à EXTRACTION_DISTANCE [mm] de notre robot)
			//	On recherche le point le plus éloigné des adversaires.
			//	C'est à dire, le point ayant son adversaire le plus proche...... le plus loin de lui.
			//	Ainsi, si un seul adversaire est le plus proche de tout les points candidats (plutot probable)... Le meilleur point est celui diamétralement opposé à l'adversaire en question.
			//	Si par contre certains points sont plus proches d'un autre adversaire... ils peuvent ne pas être choisis pour cette raison
			//  Si aucun point n'est accessible (car trop proche bordure, élément fixe de jeu, ou adversaire)... L'algo échoue et patiente 1 seconde avant de retenter.
			// 	On tente 3 fois avant d'abandonner...
			remaining_try--;

			adversary_to_close_distance = (QS_WHO_AM_I_get() == BIG_ROBOT)? 500 : 400;
			i_can_turn_in_my_position = is_possible_point_for_rotation(&((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}));

			bestPoint = 0xFF;
			bestPoint_distance2_with_nearest_foe = 0;

			for(i = 0; i < 12; i++)	//Pour chaque point parmi les 12...
			{
				//Si je peux tourner là où je suis, ou que les points que je vais calculer sont pile devant ou pile derrière... à 30° près... alors je calcule le point i
				if(i_can_turn_in_my_position || i <= 1 || (i >=5 && i<=7) || i>=11)
				{
					//Calcul des coordonnées du point.
					COS_SIN_4096_get((PI4096*30*i + global.env.pos.angle)/180,&cos,&sin);
					pointEx[i].x = ((Sint32)(cos)*EXTRACTION_DISTANCE)/4096 + global.env.pos.x;
					pointEx[i].y = ((Sint32)(sin)*EXTRACTION_DISTANCE)/4096 + global.env.pos.y;

					if(is_possible_point_for_rotation(&pointEx[i]))	//Si le point est "acceptable" (loin d'un élément fixe ou d'une bordure...)
					{
						distance2_between_point_and_foe_min = 0xFFFFFFFF;
						//On recherche la distance minimale entre le point 'i' et l'adversaire le plus proche.
						for(foe = 0; foe < MAX_NB_FOES; foe++)		//Pour tout les adversaires obsersés
						{
							if(global.env.foe[foe].enable){
								distance2_between_point_and_foe = (pointEx[i].x-global.env.foe[foe].x)*(pointEx[i].x-global.env.foe[foe].x) + (pointEx[i].y-global.env.foe[foe].y)*(pointEx[i].y-global.env.foe[foe].y);
								if(distance2_between_point_and_foe < distance2_between_point_and_foe_min){	//Si l'adversaire en cours est plus proche du point que les autres, on le prend en compte.
									distance2_between_point_and_foe_min = distance2_between_point_and_foe;
								}
							}
						}

						//On recherche maintenant le point ayant "la distance avec son adversaire le plus proche", la plus GRANDE possible...
						if(distance2_between_point_and_foe_min > bestPoint_distance2_with_nearest_foe)
						{	//Si le point 'i' est plus loin des adversaires que les autres points calculés... il est le meilleur point candidat
							bestPoint_distance2_with_nearest_foe = distance2_between_point_and_foe_min;
							bestPoint = i;
						}
					}
				}
			}


			if(bestPoint != 0xFF)
			{
				can_i_go_to_point = TRUE; // Si le point faux, on le mettra a faux

				//Pour gerer un cas d'erreur, si nous somme trop prêt d'un adversaire, un point derrière peut être supprimé alors que l'adversaire est devant nous
				for(foe = 0; foe < MAX_NB_FOES; foe++)
				{// Si nous sommes encercle par deux ennemis, on peut pas se permettre de comparer seulement avec l'ennemis le plus proche du point
					if(global.env.foe[foe].enable)
					{
						distance2_between_point_and_foe = (pointEx[bestPoint].x-global.env.foe[foe].x)*(pointEx[bestPoint].x-global.env.foe[foe].x) + (pointEx[bestPoint].y-global.env.foe[foe].y)*(pointEx[bestPoint].y-global.env.foe[foe].y);

						if(distance2_between_point_and_foe < adversary_to_close_distance*adversary_to_close_distance)
						{ // Si le point est pres de l'adveraire, on regarde où il se situe par rapport à nous et l'adversaire
							//Calcul du point sur le bord du robot en direction de l'adversaire de façon à offrir un point de plus pour la sortie si il est encerclée
							Uint16 norm = GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y},(GEOMETRY_point_t){global.env.foe[foe].x,global.env.foe[foe].y});

							float coefx = (global.env.foe[foe].x-global.env.pos.x)/(norm*1.);
							float coefy = (global.env.foe[foe].y-global.env.pos.y)/(norm*1.);

							GEOMETRY_point_t center; // Le centre d'où nous allons faire le produit scalaire
							Uint8 widthRobot;
							widthRobot =  (QS_WHO_AM_I_get() == BIG_ROBOT)? BIG_ROBOT_WIDTH/2 : SMALL_ROBOT_WIDTH/2;

							center.x = global.env.pos.x + widthRobot*coefx;
							center.y = global.env.pos.y + widthRobot*coefy;

							Sint32 vecAdX = global.env.foe[foe].x-center.x;
							Sint32 vecAdY = global.env.foe[foe].y-center.y;
							Sint32 vecPointX = pointEx[bestPoint].x-center.x;
							Sint32 vecPointY = pointEx[bestPoint].y-center.y;

							if(vecAdX*vecPointX + vecAdY*vecPointY > 0)
							{ // Si le produit scalaire des deux vecteurs est positif nous pouvons pas aller au point car le point se situe entre nous et l'adversaire
								can_i_go_to_point = FALSE;
								break;
							}
						}
					}
				}
			}



			//Si on a trouvé un point et qu'il est suffisamment loin des adversaires.... Champomy !!!
			if(can_i_go_to_point)
				state = GO_POINT;
			else
				state = WAIT;

			break;

		case GO_POINT:
			state = check_sub_action_result(goto_extract_with_avoidance((displacement_t){(GEOMETRY_point_t){pointEx[bestPoint].x,pointEx[bestPoint].y}, speed}), GO_POINT, DONE, WAIT);
			break;

		case WAIT:{ // Etat d'attente si il n'a pas trouvé de chemin dans l'immédiat
			static time32_t begin_time;

			if(entrance)
			{
				//BUZZER_play(500, NOTE_LA, 10);
				begin_time = global.env.match_time;
			}

			if(global.env.match_time > begin_time + 1000)
			{
				if(remaining_try)
					state = COMPUTE;
				else if(is_possible_point_for_rotation(&((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}))){
					state = sens;
				}else
					state = COMPUTE;
			}

			}break;

		case TURN_TRIGO:
			remaining_try = 3;
			state = try_go_angle(global.env.pos.angle + PI4096/2,TURN_TRIGO,COMPUTE,TURN_HORAIRE,FAST);

			if(state == TURN_HORAIRE)
				sens = TURN_HORAIRE;
			break;

		case TURN_HORAIRE:
			remaining_try = 3;
			state = try_go_angle(global.env.pos.angle - PI4096/2,TURN_HORAIRE,COMPUTE,TURN_TRIGO,FAST);

			if(state == TURN_TRIGO)
				sens = TURN_TRIGO;
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			state = IDLE;
			return NOT_HANDLED;
			break;
		default:
			break;
	}

	return IN_PROGRESS;
}

/* Fonction qui réalise un PROP_push_goto spécifique à l'extration du robot avec la gestion de l'évitement */

static error_e goto_extract_with_avoidance(const displacement_t displacements)
{
	enum state_e
	{
		CHECK_SCAN_FOE = 0,
		LOAD_MOVE,
		WAIT_MOVE_AND_SCAN_FOE,
		DONE
	};
	#ifdef USE_PROP_AVOIDANCE
		static enum state_e state = LOAD_MOVE;
	#else
		static enum state_e state = CHECK_SCAN_FOE;
	#endif

	static bool_e timeout = FALSE;
	error_e prop_stack_state;

	switch(state)
	{
		case CHECK_SCAN_FOE :
			if(foe_in_zone(TRUE, displacements.point.x, displacements.point.y, FALSE)){
				avoidance_printf("goto_extract_with_avoidance NOT HANDLED because foe in target zone");
				state = CHECK_SCAN_FOE;
				return NOT_HANDLED;
			}else
				state = LOAD_MOVE;
			break;

		case LOAD_MOVE:
			clear_prop_detected_foe();
			global.env.destination = displacements.point;
			PROP_push_goto_multi_point(displacements.point.x, displacements.point.y, displacements.speed, ANY_WAY, PROP_CURVES, AVOID_ENABLED, END_OF_BUFFER, END_AT_LAST_POINT, TRUE);
			avoidance_printf("goto_extract_with_avoidance : load_move\n");
			state = WAIT_MOVE_AND_SCAN_FOE;
			break;

		case WAIT_MOVE_AND_SCAN_FOE:

#ifdef USE_PROP_AVOIDANCE
			prop_stack_state = AVOIDANCE_watch_prop_stack();
			switch(prop_stack_state)
			{
				case END_OK:
					avoidance_printf("goto_extract_with_avoidance -- fini\n");
					state = DONE;
					break;

				case END_WITH_TIMEOUT:
					timeout = TRUE;
					avoidance_printf("goto_extract_with_avoidance -- timeout\n");
					SD_printf("TIMEOUT on goto_extract_with_avoidance");
					state = DONE;
					break;

				case NOT_HANDLED:
					avoidance_printf("goto_extract_with_avoidance -- probleme\n");
					SD_printf("ERROR on goto_extract_with_avoidance");
					state = LOAD_MOVE;
					return NOT_HANDLED;
					break;

				case IN_PROGRESS:
					break;

				default:
					state = LOAD_MOVE;
					return NOT_HANDLED;
					break;
			}

			// check adversaire détecté
			if(prop_detected_foe){
				STACKS_flush(PROP);
				state = LOAD_MOVE;
				return FOE_IN_PATH;
			}
#else
			if(global.env.debug_force_foe)	//Evitement manuel forcé !
			{
				STACKS_flush(PROP);
				debug_foe_reason(FORCED_BY_USER, 0, 0);
				PROP_push_stop();
				global.env.debug_force_foe = FALSE;
				state = CHECK_SCAN_FOE;
				return FOE_IN_PATH;
			}
			else
			{
				//Si on effectue un translation, c'est qu'on est en direction du point voulu (si le point était sur notre gauche, on aura fait une rotation au préalable)
				//Necessaire pour que l'angle de detection de l'adversaire soit valide (car sinon on ne pointe pas forcément vers notre point d'arrivé ...)
				//On considère ici que si la prop faire une translation, le robot pointe vers le point d'arrivée
	//			if((global.env.prop.current_trajectory != TRAJECTORY_TRANSLATION && global.env.prop.current_trajectory != TRAJECTORY_AUTOMATIC_CURVE) &&
	//				(is_in_path[FOE_1] || is_in_path[FOE_2]))
	//				avoidance_printf("Not in translation but foe in path\n");

				if(global.env.prop.is_in_translation && foe_in_path(TRUE))	//Si un adversaire est sur le chemin
				{	//On ne peut pas inclure le test du type de trajectoire dans le foe_in_path car ce foe_in_path sert également à l'arrêt, une fois qu'on a vu l'adversaire.
					//debug_foe_reason(foe, global.env.foe[foe].angle, global.env.foe[foe].dist);
					//debug_printf("IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
					BUZZER_play(20, DEFAULT_NOTE, 3);
					avoidance_printf("goto_extract_with_avoidance: foe detected\n");
					STACKS_flush(PROP);
					PROP_push_stop();
					state = CHECK_SCAN_FOE;
					return FOE_IN_PATH;
				}
				else
				{
					// aucun adversaire n'est détecté, on fait notre mouvement normalement
					// on regarde si la pile s'est vidée
					prop_stack_state = AVOIDANCE_watch_prop_stack();
					switch(prop_stack_state)
					{
						case END_OK:
							avoidance_printf("goto_extract_with_avoidance -- fini\n");
							state = DONE;
							break;

						case END_WITH_TIMEOUT:
							timeout = TRUE;
							avoidance_printf("goto_extract_with_avoidance -- timeout\n");
							SD_printf("TIMEOUT on goto_extract_with_avoidance");
							state = DONE;
							break;

						case NOT_HANDLED:
							avoidance_printf("goto_extract_with_avoidance -- probleme\n");
							SD_printf("ERROR on goto_extract_with_avoidance");
							state = CHECK_SCAN_FOE;
							return NOT_HANDLED;
							break;

						case IN_PROGRESS:
							break;

						default:
							state = CHECK_SCAN_FOE;
							return NOT_HANDLED;
							break;
					}
				}
			}
#endif
			break;

		case DONE:
			#ifdef USE_PROP_AVOIDANCE
				state = LOAD_MOVE;
			#else
				state = CHECK_SCAN_FOE;
			#endif
			return timeout?END_WITH_TIMEOUT:END_OK;
			break;

		default:
			debug_printf("Cas Default state, panique !!!\n");
			#ifdef USE_PROP_AVOIDANCE
				state = LOAD_MOVE;
			#else
				state = CHECK_SCAN_FOE;
			#endif
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}

// Vérifie adversaire dans NORTH_US, NORTH_FOE, SOUTH_US, SOUTH_FOE
foe_pos_e AVOIDANCE_where_is_foe(Uint8 foe_id)
{
	assert(foe_id < MAX_NB_FOES);

	if(global.env.foe[foe_id].x > 1000)
	{
		// Partie SUD
		if(COLOR_Y(global.env.foe[foe_id].y) > 1500)
		{
			return SOUTH_FOE;
		}
		else
		{
			return SOUTH_US;
		}
	}
	else
	{
		if(COLOR_Y(global.env.foe[foe_id].y) > 1500)
		{
			return NORTH_FOE;
		}
		else
		{
			return NORTH_US;
		}
	}
}

void debug_foe_reason(foe_origin_e origin, Sint16 angle, Sint16 distance){
	CAN_msg_t msg_to_send;
	msg_to_send.sid = DEBUG_FOE_REASON;
	msg_to_send.size = 5;
	msg_to_send.data[0] = origin;
	msg_to_send.data[1] = angle >> 8;
	msg_to_send.data[2] = angle & 0xFF;
	msg_to_send.data[3] = distance >> 8;
	msg_to_send.data[4] = distance & 0xFF;
	CAN_send(&msg_to_send);
}


void clear_prop_detected_foe(){
	prop_detected_foe = FALSE;
}

void set_prop_detected_foe(CAN_msg_t *msg){
	prop_detected_foe = TRUE;
	SD_printf("PROP FOE found in zone x[%d] y[%d] %s\n", U16FROMU8(msg->data[0], msg->data[1]), U16FROMU8(msg->data[2], msg->data[3]), (msg->data[4])?"with timeout":"");
}

//------------------------------------------------------------------- Fonctions autress

/*
 * Surveille l'execution de la pile PROP. Renvoie vrai si toutes les fonctions sont finies
 * Rattrappe le robot quand il part en erreur.
 * Inscrit dans le booléen got_timeout si un timeout a été levé
 */
static error_e AVOIDANCE_watch_prop_stack ()
{
	bool_e timeout = FALSE, action_end;

	action_end = STACKS_wait_end(PROP,&timeout);

	if(timeout){ // S'il y a timeout que la l'ensemble des trajectoires soient finit ou pas, on vide la stack puis on déclare le timeout
		STACKS_flush(PROP);
		return END_WITH_TIMEOUT;
	}else if(action_end) // Si l'ensemble des trajectoires sont finit, on le déclare
		return END_OK;
	else if (global.env.prop.erreur){
		STACKS_flush(PROP);
		return NOT_HANDLED;
	}

	return IN_PROGRESS;
}

/* Action qui update la position */
error_e ACTION_update_position()
{
	enum state_e {
		SEND_CAN_MSG,
		WAIT_RECEPTION
	};
	static enum state_e state;

	switch (state)
	{
		case SEND_CAN_MSG :
			if (!global.env.pos.updated)
			{
				CAN_send_sid(PROP_TELL_POSITION);
				state = WAIT_RECEPTION;
				return IN_PROGRESS;
			}
			else
			{
				return END_OK;
			}
			break;

		case WAIT_RECEPTION :
			if (global.env.pos.updated)
			{
				state = SEND_CAN_MSG;
				return END_OK;
			}
			break;
	}
	return IN_PROGRESS;
}

/* Action qui arrête le robot, met la position à jour */
error_e ACTION_prop_stop()
{
	enum state_e {
		SEND_CAN_MSG,
		WAIT_RECEPTION
	};
	static enum state_e state;

	static time32_t initial_time;

	switch (state)
	{
		case SEND_CAN_MSG :
			initial_time = global.env.match_time;
			STACKS_flush(PROP);
			CAN_send_sid(PROP_STOP);
			state = WAIT_RECEPTION;
			break;

		case WAIT_RECEPTION :
			if (global.env.prop.ended || (global.env.match_time-initial_time > (1000/*ms*/)))
			{
				state = SEND_CAN_MSG;
				return END_OK;
			}
			break;
	}
	return IN_PROGRESS;
}

