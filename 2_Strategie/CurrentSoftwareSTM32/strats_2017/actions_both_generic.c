/*
 *	Club Robot ESEO 2015 - 2016
 *	Black & Pearl
 *
 *	Fichier : actions_both_2016.c
 *	Package : Carte S²/strats_2016
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Arnaud
 *	Version 2016
 */


#include "actions_both_generic.h"
#include "../QS/QS_can_over_xbee.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_measure.h"
#include "../QS/QS_IHM.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_outputlog.h"
#include "../propulsion/movement.h"
#include "../propulsion/astar.h"
#include "../propulsion/prop_functions.h"
#include "../Supervision/SD/SD.h"
#include "../Supervision/LCD_interface.h"
#include "../Supervision/Selftest.h"
#include "../Supervision/Buzzer.h"
#include "../utils/actionChecker.h"
#include "../environment.h"
#include "../elements.h"

// Strat Rotation
#define ROT_WAY_CLOCK						0
#define ROT_WAY_TRIGO						1
#define ROT_DEFAULT_SPEED					SLOW
#define ROT_ODOMETRIE_PLAGE_ROTATION		2
#define ROT_NB_TOUR_ODO_ROTATION			10
#define ROT_BEGIN_NB_TOUR					2
#define ROT_CALAGE_WAY						FORWARD
#define ROT_ADVANCE_WAY						BACKWARD
#define ROT_DISTANCE						-400
#define ROT_WAY								ROT_WAY_CLOCK

// Strat Translation
#define TRANS_WAY_FORWARD					0
#define TRANS_WAY_BACKWARD					1
#define TRANS_DEFAULT_SPEED					SLOW
#define TRANS_ODOMETRIE_PLAGE_TRANSLATION	0
#define TRANS_CALAGE_WAY					TRANS_WAY_FORWARD
#define TRANS_AREA_LENGTG					3000

// Strat Symétrie
#define SYM_WAY_FORWARD						0
#define SYM_WAY_BACKWARD					1
#define SYM_DEFAULT_SPEED					SLOW
#define SYM_ODOMETRIE_PLAGE_SYMETRIE		1
#define SYM_CALAGE_WAY						SYM_WAY_BACKWARD
#define SYM_AREA_LENGTH						500
#define SYM_AREA_WIDTH						600
#define SYM_OFFSET_WALL						350

/* ----------------------------------------------------------------------------- */
/* 							Fonctions de réglage odométrique		             */
/* ----------------------------------------------------------------------------- */

error_e func_go_to_home(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_GO_TO_START_ZONE,
							IDLE,
							PATH,
							DONE,
							ERROR
							);
	switch (state) {
		case IDLE:
			debug_printf("state IDLE in GO_TO_HOME\n");
			state= PATH;
			break;

		case PATH:
			state = ASTAR_try_going(1000,COLOR_Y(550),PATH,DONE,ERROR,SLOW,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			//state = PATHFIND_try_going(COLOR_NODE(A1),PATH,DONE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT );
			break;

		case DONE:
			debug_printf("GO_TO_HOME return END_OK\n");
			return END_OK;
			break;

		case ERROR:
			return NOT_HANDLED;
			break;
	}
	return IN_PROGRESS;
}

void strat_stop_robot(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_STOP_ROBOT,
			IDLE,
			STOP,
			END,
			FAIL
	);

	switch(state){
		case IDLE:
			state = STOP;
			break;

		case STOP:
			state = try_stop(state, END, FAIL);
			break;

		case END:
			break;

		case FAIL:
			break;
	}
}

void strat_tourne_en_rond(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_TOURNE_EN_ROND,
		IDLE,
		POS_DEPART,
		POS_POINT,
		TOUR,
		FIN,
		DONE,
		ERROR
	);

	displacement_t tour[6] = {
		{{650,600},SLOW},
		{{700,2250},SLOW},
		{{960,2600},SLOW},
		{{1500,2300},SLOW},
		{{1500,750},SLOW},
		{{1200,450},SLOW}
	};

//	CAN_msg_t msg;

	static Uint16 i = 0;
	Uint16 nbTour = 1;

	switch(state){
	case IDLE:
//		msg.sid=PROP_SET_POSITION;
//		msg.data[0]=500 >> 8;
//		msg.data[1]=500 & 0xFF;
//		msg.data[2]=120 >> 8;
//		msg.data[3]=120 & 0xFF;
//		msg.data[4]=PI4096/2 >> 8;
//		msg.data[5]=PI4096/2 & 0xFF;
//		msg.size = 6;
//		CAN_send(&msg);

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(global.pos.x,200,POS_DEPART,POS_POINT,ERROR,SLOW,(QS_WHO_AM_I_get_name() == BIG_ROBOT)?FORWARD:BACKWARD,NO_AVOIDANCE, END_AT_BRAKE);
		break;
	case POS_POINT:
		state = try_going(650,600,POS_POINT,TOUR,ERROR,SLOW,(QS_WHO_AM_I_get_name() == BIG_ROBOT)?FORWARD:BACKWARD,NO_AVOIDANCE, END_AT_BRAKE);
		break;
	case TOUR:
		state = try_going_multipoint(tour,6,TOUR,FIN,ERROR,(QS_WHO_AM_I_get_name() == BIG_ROBOT)?FORWARD:BACKWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
		break;
	case FIN:
		i++;

		if(i >= nbTour)
			state = DONE;
		else
			state = POS_POINT;

		break;
	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}

}

void strat_reglage_odo_rotation(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_REGLAGE_ODO_ROTATION,
		IDLE,
		WAIT_COEF,
		INIT_CALAGE,
		INIT_VAR,
		AVANCER,
		BEGIN_PROCESS,
		TOUR0,
		TOUR1,
		TOUR2,
		CALAGE1,
		CALAGE2,
		COMPARE_N_CORRECT,
		REPORT,
		ERROR
	);

	static Sint16 nb_tour_compteur, actual_nb_tour;
	static Uint32 coefOdoRotation;
	static position_t positionCalage;

	switch(state){
		case IDLE:
			SELFTEST_set_warning_bat_display_state(FALSE);
			PROP_set_position(0, 0, 0);
			CAN_send_sid(DEBUG_PROPULSION_GET_COEFS);
			actual_nb_tour = ROT_BEGIN_NB_TOUR;
			state = WAIT_COEF;
			break;

		case WAIT_COEF:
			if(global.debug.propulsion_coefs_updated & (1 << ODOMETRY_COEF_ROTATION)){
				coefOdoRotation = global.debug.propulsion_coefs[ODOMETRY_COEF_ROTATION];
				state = INIT_CALAGE;
			}
			break;

		case INIT_CALAGE:
			state = try_rushInTheWall(0, state, INIT_VAR, INIT_VAR, ROT_CALAGE_WAY, TRUE, 0, 0);
			if(ON_LEAVE()){
				PROP_set_position(0, 0, 0);
			}
			break;

		case INIT_VAR:
			nb_tour_compteur = 0;
			state = AVANCER;
			break;

		case AVANCER:
			state = try_going(ROT_DISTANCE, 0, state, BEGIN_PROCESS, ERROR, ROT_DEFAULT_SPEED, ROT_ADVANCE_WAY, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;


		case BEGIN_PROCESS:
			if(nb_tour_compteur < actual_nb_tour){
				state = TOUR0;
			}else{
				state = CALAGE1;
			}
			break;


		case TOUR0:
#if ROT_WAY == ROT_WAY_CLOCK
			state = try_go_angle(-2*PI4096/3, state, TOUR1, ERROR, ROT_DEFAULT_SPEED, CLOCKWISE, END_AT_BRAKE);
#else
			state = try_go_angle(2*PI4096/3, state, TOUR1, ERROR, ROT_DEFAULT_SPEED, TRIGOWISE, END_AT_BRAKE);
#endif
			break;


		case TOUR1:
#if ROT_WAY == ROT_WAY_CLOCK
			state = try_go_angle(2*PI4096/3, state, TOUR2, ERROR, ROT_DEFAULT_SPEED, CLOCKWISE, END_AT_BRAKE);
#else
			state = try_go_angle(-2*PI4096/3, state, TOUR2, ERROR, ROT_DEFAULT_SPEED, TRIGOWISE, END_AT_BRAKE);
#endif
			break;

		case TOUR2:
			state = try_go_angle(0, state, BEGIN_PROCESS, ERROR, ROT_DEFAULT_SPEED, ANY_WAY, END_AT_BRAKE);
			if(ON_LEAVE()){
				nb_tour_compteur++;
			}
			break;

		case CALAGE1:
			state = try_rushInTheWall(0, state, CALAGE2, CALAGE2, ROT_CALAGE_WAY, TRUE, 0, 0);
			break;

		case CALAGE2:
			state = try_rushInTheWall(0, state, COMPARE_N_CORRECT, COMPARE_N_CORRECT, ROT_CALAGE_WAY, FALSE, 0, 0);
			if(ON_LEAVE()){
				positionCalage = global.pos;
				PROP_set_position(0, 0, 0);
			}
			break;

		case COMPARE_N_CORRECT:
			// compare l'angle reçu après calage par rapport au ZERO
			//correction de la nouvelle odométrie
			// Envoi du nouveau coefficient à la propulsion
			//Renouvel le process autant de fois que nécessaire

			//Si il est bien réglé l'angle interne du robot devrait etre de zero, sinon l'angle interne du robot s'est décalé quand le robot a approché le mur

			if(absolute(positionCalage.angle) <= ROT_ODOMETRIE_PLAGE_ROTATION){
				error_printf("Step odométrie rotation finit : 0x%lx   | angle : %d \n", coefOdoRotation, positionCalage.angle);
				LCD_printf(3, TRUE, TRUE, "V|%d|0x%lx", positionCalage.angle, coefOdoRotation);

				if(actual_nb_tour == ROT_NB_TOUR_ODO_ROTATION){
					state = REPORT;
				}else{
					if(actual_nb_tour <= 3)
						actual_nb_tour++;
					else
						actual_nb_tour+=2;

					if(actual_nb_tour > ROT_NB_TOUR_ODO_ROTATION)
						actual_nb_tour = ROT_NB_TOUR_ODO_ROTATION;

					state = INIT_VAR;
				}


			}else{//Recommencer procédure en modifiant les valeurs
				debug_printf("Echec calage angle : %d \n", positionCalage.angle);

#if ROT_WAY == ROT_WAY_CLOCK
				if(positionCalage.angle > 0){

#else
				if(positionCalage.angle < 0){

#endif
					if(absolute(positionCalage.angle) > 100)
						coefOdoRotation += 50/actual_nb_tour;
					else if(absolute(positionCalage.angle) > 25)
						coefOdoRotation += 25/actual_nb_tour;
					else if(absolute(positionCalage.angle) > 10)
						coefOdoRotation += 10/actual_nb_tour;
					else
						coefOdoRotation += 1;

					LCD_printf(3, TRUE, TRUE, "+ | %d | 0x%lx", positionCalage.angle, coefOdoRotation);
					debug_printf("Augmentation du coef 0x%lx\n", coefOdoRotation);

				}
				else{

					if(absolute(positionCalage.angle) > 100)
						coefOdoRotation -= 50/actual_nb_tour;
					else if(absolute(positionCalage.angle) > 25)
						coefOdoRotation -= 25/actual_nb_tour;
					else if(absolute(positionCalage.angle) > 10)
						coefOdoRotation -= 10/actual_nb_tour;
					else
						coefOdoRotation -= 1;

					LCD_printf(3, TRUE, TRUE, "- | %d | 0x%lx", positionCalage.angle, coefOdoRotation);
					debug_printf("Diminution du coef 0x%lx\n", coefOdoRotation);
				}

				CAN_msg_t msg;
				msg.sid = DEBUG_PROPULSION_SET_COEF;
				msg.size = SIZE_DEBUG_PROPULSION_SET_COEF;
				msg.data.debug_propulsion_set_coef.id = ODOMETRY_COEF_ROTATION;
				msg.data.debug_propulsion_set_coef.value = coefOdoRotation;
				CAN_send(&msg);

				state = INIT_VAR;
			}

			break;

		case REPORT:{
			static bool_e repeat_result;
			if(entrance){
				LCD_printf(3, TRUE, TRUE, "F | %d | 0x%lx", positionCalage.angle, coefOdoRotation);
				WATCHDOG_create_flag(1000, &repeat_result);
			}
			if(repeat_result){
				debug_printf("Odométrie rotation finit : 0x%lx\n", coefOdoRotation);
				WATCHDOG_create_flag(1000, &repeat_result);
			}
			}break;

		case ERROR:
			if(entrance)
				debug_printf("Réglage odométrie rotation erreur\n");
			break;
	}

}

void strat_reglage_odo_translation(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_REGLAGE_ODO_TRANSLATION,
		IDLE,
		WAIT_COEF,
		INIT_CALAGE,
		INIT_VAR,
		AVANCER0,
		AVANCER1,
		CALAGE1,
		CALAGE2,
		COMPARE_N_CORRECT,
		REPORT,
		ERROR
	);

	static Uint32 coefOdoTranslation;
	static position_t positionCalage;
	static Sint32 comparation_value;

	switch(state){
		case IDLE:
			SELFTEST_set_warning_bat_display_state(FALSE);
			PROP_set_position(0, 0, 0);
			CAN_send_sid(DEBUG_PROPULSION_GET_COEFS);
			state = WAIT_COEF;
			break;

		case WAIT_COEF:
			if(global.debug.propulsion_coefs_updated & (1 << ODOMETRY_COEF_TRANSLATION)){
				coefOdoTranslation = global.debug.propulsion_coefs[ODOMETRY_COEF_TRANSLATION];
				state = INIT_CALAGE;
			}
			break;

		case INIT_CALAGE:
#if TRANS_CALAGE_WAY == TRANS_WAY_FORWARD
			state = try_rushInTheWall(0, state, INIT_VAR, INIT_VAR, FORWARD, TRUE, 0, 0);
#else
			state = try_rushInTheWall(0, state, INIT_VAR, INIT_VAR, BACKWARD, TRUE, 0, 0);
#endif
			if(ON_LEAVE()){
#if TRANS_CALAGE_WAY == TRANS_WAY_FORWARD
				if(I_AM_BIG())
					PROP_set_position(-BIG_CALIBRATION_FORWARD_BORDER_DISTANCE, 0, 0);
				else
					PROP_set_position(-SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE, 0, 0);

#else
				if(I_AM_BIG())
					PROP_set_position(BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE, 0, 0);
				else
					PROP_set_position(SMALL_CALIBRATION_BACKWARD_BORDER_DISTANCE, 0, 0);
#endif
			}
			break;

		case INIT_VAR:
			state = AVANCER0;
			break;

		case AVANCER0:
#if TRANS_CALAGE_WAY == TRANS_WAY_FORWARD
			state = try_going(-TRANS_AREA_LENGTG/2, 0, state, AVANCER1, ERROR, TRANS_DEFAULT_SPEED, BACKWARD, NO_AVOIDANCE, END_AT_BRAKE);
#else
			state = try_going(TRANS_AREA_LENGTG/2, 0, state, AVANCER1, ERROR, TRANS_DEFAULT_SPEED, FORWARD, NO_AVOIDANCE, END_AT_BRAKE);
#endif
			break;

		case AVANCER1:
#if TRANS_CALAGE_WAY == TRANS_WAY_FORWARD
			state = try_going(-(TRANS_AREA_LENGTG-500), 0, state, CALAGE1, ERROR, TRANS_DEFAULT_SPEED, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
#else
			state = try_going((TRANS_AREA_LENGTG-500), 0, state, CALAGE1, ERROR, TRANS_DEFAULT_SPEED, FORWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
#endif
			break;

		case CALAGE1:
#if TRANS_CALAGE_WAY == TRANS_WAY_FORWARD
			state = try_rushInTheWall(PI4096, state, CALAGE2, CALAGE2, FORWARD, TRUE, 0, 0);
#else
			state = try_rushInTheWall(PI4096, state, CALAGE2, CALAGE2, BACKWARD, TRUE, 0, 0);
#endif
			break;

		case CALAGE2:
#if TRANS_CALAGE_WAY == TRANS_WAY_FORWARD
			state = try_rushInTheWall(PI4096, state, COMPARE_N_CORRECT, COMPARE_N_CORRECT, FORWARD, FALSE, 0, 0);
#else
			state = try_rushInTheWall(PI4096, state, COMPARE_N_CORRECT, COMPARE_N_CORRECT, BACKWARD, FALSE, 0, 0);
#endif
			if(ON_LEAVE()){
				positionCalage = global.pos;
#if TRANS_CALAGE_WAY == TRANS_WAY_FORWARD
				if(I_AM_BIG())
					PROP_set_position(-BIG_CALIBRATION_FORWARD_BORDER_DISTANCE, 0, 0);
				else
					PROP_set_position(-SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE, 0, 0);

#else
				if(I_AM_BIG())
					PROP_set_position(BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE, 0, 0);
				else
					PROP_set_position(SMALL_CALIBRATION_BACKWARD_BORDER_DISTANCE, 0, 0);
#endif
			}
			break;

		case COMPARE_N_CORRECT:

#if TRANS_CALAGE_WAY == TRANS_WAY_FORWARD
			if(I_AM_BIG())
				comparation_value = -TRANS_AREA_LENGTG - (positionCalage.x - BIG_CALIBRATION_FORWARD_BORDER_DISTANCE);
			else
				comparation_value = -TRANS_AREA_LENGTG - (positionCalage.x - SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE);

#else
			if(I_AM_BIG())
				comparation_value = TRANS_AREA_LENGTG - (positionCalage.x + BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE);
			else
				comparation_value = TRANS_AREA_LENGTG - (positionCalage.x + SMALL_CALIBRATION_BACKWARD_BORDER_DISTANCE);
#endif

			if(absolute(comparation_value) <= TRANS_ODOMETRIE_PLAGE_TRANSLATION){
				debug_printf("Odométrie en translation bien calibrée\n");
				state = REPORT;
			}else{
				if(comparation_value > TRANS_ODOMETRIE_PLAGE_TRANSLATION){

					if(absolute(comparation_value) > 100)
						coefOdoTranslation -= 25;
					else if(absolute(comparation_value) > 25)
						coefOdoTranslation -= 10;
					else if(absolute(comparation_value) > 10)
						coefOdoTranslation -= 5;
					else
						coefOdoTranslation -= 1;

					debug_printf("Il n'est pas allé assez loin\n");
					LCD_printf(3, TRUE, TRUE, "- | %ld | 0x%lx", comparation_value, coefOdoTranslation);

				}else if(comparation_value < -TRANS_ODOMETRIE_PLAGE_TRANSLATION){

					if(absolute(comparation_value) > 100)
						coefOdoTranslation += 25;
					else if(absolute(comparation_value) > 25)
						coefOdoTranslation += 10;
					else if(absolute(comparation_value) > 10)
						coefOdoTranslation += 5;
					else
						coefOdoTranslation += 1;

					debug_printf("Il est trop loin\n");
					LCD_printf(3, TRUE, TRUE, "+ | %ld | 0x%lx", comparation_value, coefOdoTranslation);
				}

				debug_printf("Nouveau coefficient 0x%lx\n",coefOdoTranslation);

				CAN_msg_t msg;
				msg.sid = DEBUG_PROPULSION_SET_COEF;
				msg.size = SIZE_DEBUG_PROPULSION_SET_COEF;
				msg.data.debug_propulsion_set_coef.id = ODOMETRY_COEF_TRANSLATION;
				msg.data.debug_propulsion_set_coef.value = coefOdoTranslation;
				CAN_send(&msg);
				state = INIT_VAR;
			}
			break;

		case REPORT:{
			static bool_e repeat_result;
			if(entrance){
				LCD_printf(3, TRUE, TRUE, "F | %ld | 0x%lx", comparation_value, coefOdoTranslation);
				WATCHDOG_create_flag(1000, &repeat_result);
			}
			if(repeat_result){
				debug_printf("Odométrie translation finit : 0x%lx\n", coefOdoTranslation);
				WATCHDOG_create_flag(1000, &repeat_result);
			}
			}break;

		case ERROR:
			break;
	}
}

void strat_reglage_odo_symetrie(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_REGLAGE_ODO_SYMETRIE,
		IDLE,
		WAIT_COEF,
		INIT_CALAGE,
		INIT_VAR,
		AVANCER_APRES_CALAGE,
		AVANCER1,
		AVANCER2,
		AVANCER3,
		AVANCER4,
		AVANCER5,
		CALAGE1,
		CALAGE2,
		COMPARE_N_CORRECT,
		REPORT,
		ERROR
	);

	static Uint32 coefOdoSymetrie;
	static position_t positionCalage;
	static Sint32 comparation_value;

	switch(state){

		case IDLE:
			SELFTEST_set_warning_bat_display_state(FALSE);
			PROP_set_position(0, 0, 0);
			CAN_send_sid(DEBUG_PROPULSION_GET_COEFS);
			state = WAIT_COEF;
			break;

		case WAIT_COEF:
			if(global.debug.propulsion_coefs_updated & (1 << ODOMETRY_COEF_SYM)){
				coefOdoSymetrie = global.debug.propulsion_coefs[ODOMETRY_COEF_SYM];
				state = INIT_CALAGE;
			}
			break;

		case INIT_CALAGE:
			state = try_rushInTheWall(PI4096, state, INIT_VAR, INIT_VAR, BACKWARD, TRUE, 0, 0);
			if(ON_LEAVE()){
				PROP_set_position(0, 0, 0);
			}
			break;

		case INIT_VAR:
			state = AVANCER_APRES_CALAGE;
			break;

		case AVANCER_APRES_CALAGE:
			state = try_going(SYM_OFFSET_WALL, 0, state, AVANCER1, AVANCER1, SYM_DEFAULT_SPEED, FORWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case AVANCER1:
			state = try_going(SYM_OFFSET_WALL, -SYM_AREA_LENGTH, state, AVANCER2, AVANCER2, SYM_DEFAULT_SPEED, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case AVANCER2:
			state = try_going(SYM_OFFSET_WALL + SYM_AREA_WIDTH, -SYM_AREA_LENGTH, state, AVANCER3, AVANCER3, SYM_DEFAULT_SPEED, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case AVANCER3:
			state = try_going(SYM_OFFSET_WALL + SYM_AREA_WIDTH, SYM_AREA_LENGTH, state, AVANCER4, AVANCER4, SYM_DEFAULT_SPEED, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case AVANCER4:
			state = try_going(SYM_OFFSET_WALL, SYM_AREA_LENGTH, state, AVANCER5, AVANCER5, SYM_DEFAULT_SPEED, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case AVANCER5:
			state = try_going(SYM_OFFSET_WALL, 0, state, CALAGE1, CALAGE1, SYM_DEFAULT_SPEED, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case CALAGE1:
			state = try_rushInTheWall(PI4096, state, CALAGE2, CALAGE2, BACKWARD, TRUE, 0, 0);
			break;

		case CALAGE2:
			state = try_rushInTheWall(PI4096, state, COMPARE_N_CORRECT, COMPARE_N_CORRECT, BACKWARD, FALSE, 0, 0);
			if(ON_LEAVE()){
				positionCalage = global.pos;
				PROP_set_position(0, 0, 0);
			}
			break;

		case COMPARE_N_CORRECT:
			comparation_value = positionCalage.angle;
			if(absolute(comparation_value) <=  SYM_ODOMETRIE_PLAGE_SYMETRIE){
				debug_printf("Odométrie en symétrie bien calibrée\n");
				state = REPORT;
			}else{
				if(comparation_value > SYM_ODOMETRIE_PLAGE_SYMETRIE){

					if(absolute(comparation_value) > 100)
						coefOdoSymetrie -= 10;
					else if(absolute(comparation_value) > 25)
						coefOdoSymetrie -= 5;
					else if(absolute(comparation_value) > 10)
						coefOdoSymetrie -= 2;
					else
						coefOdoSymetrie -= 1;

					debug_printf("Il est trop loin du bord %ld\n", comparation_value);
					LCD_printf(3, TRUE, TRUE, "- | %ld | 0x%lx", comparation_value, coefOdoSymetrie);

				}else if(comparation_value < -SYM_ODOMETRIE_PLAGE_SYMETRIE){

					if(absolute(comparation_value) > 100)
						coefOdoSymetrie += 10;
					else if(absolute(comparation_value) > 25)
						coefOdoSymetrie += 5;
					else if(absolute(comparation_value) > 10)
						coefOdoSymetrie += 2;
					else
						coefOdoSymetrie += 1;

					debug_printf("Il est trop proche du bord %ld\n", comparation_value);
					LCD_printf(3, TRUE, TRUE, "+ | %ld | 0x%lx", comparation_value, coefOdoSymetrie);
				}

				debug_printf("Nouveau coefficient 0x%lx\n",coefOdoSymetrie);

				CAN_msg_t msg;
				msg.sid = DEBUG_PROPULSION_SET_COEF;
				msg.size = SIZE_DEBUG_PROPULSION_SET_COEF;
				msg.data.debug_propulsion_set_coef.id = ODOMETRY_COEF_SYM;
				msg.data.debug_propulsion_set_coef.value = coefOdoSymetrie;
				CAN_send(&msg);

				state = INIT_VAR;
			}
			break;

		case REPORT:{
			static bool_e repeat_result;
			if(entrance){
				LCD_printf(3, TRUE, TRUE, "F | %ld | 0x%lx", comparation_value, coefOdoSymetrie);
				WATCHDOG_create_flag(1000, &repeat_result);
			}
			if(repeat_result){
				debug_printf("Odométrie symétrie finit : 0x%lx\n", coefOdoSymetrie);
				WATCHDOG_create_flag(1000, &repeat_result);
			}
			}break;

		case ERROR:
			if(entrance)
				debug_printf("Réglage symétrie rotation erreur\n");
			break;
	}
}


typedef enum
{
	KPT_COMPUTE = 0,
	KDT_COMPUTE,
	KPR_COMPUTE,
	KDR_COMPUTE,
	COEF_NB
}coefs_e;
//COEFS par défaut pour les robots...
const Uint32 default_coefs_small_robot[COEF_NB] =	{		34,		12,		130,		288	};
const Uint32 default_coefs_big_robot[COEF_NB] =		{		32,		0x36,	0xA0,		0x800	};
static coefs_e current_coef;

void send_coef(coefs_e coef, Uint32 value)
{
	CAN_msg_t msg;
	debug_printf("send coef : ");

	msg.sid = DEBUG_PROPULSION_SET_COEF;
	msg.size = SIZE_DEBUG_PROPULSION_SET_COEF;
	switch(coef)
	{
		case KPT_COMPUTE:	msg.data.debug_propulsion_set_coef.id = CORRECTOR_COEF_KP_TRANSLATION;	debug_printf("Kp translation");	break;
		case KDT_COMPUTE:	msg.data.debug_propulsion_set_coef.id = CORRECTOR_COEF_KD_TRANSLATION;	debug_printf("Kd translation");	break;
		case KPR_COMPUTE:	msg.data.debug_propulsion_set_coef.id = CORRECTOR_COEF_KP_ROTATION;	debug_printf("Kp rotation");	break;
		case KDR_COMPUTE:	msg.data.debug_propulsion_set_coef.id = CORRECTOR_COEF_KD_ROTATION;	debug_printf("Kd rotation");	break;
		default:				break;
	}
	msg.data.debug_propulsion_set_coef.value = value;
	debug_printf(" = %ld\n",value);
	CAN_send(&msg);
}

void send_default_coefs(void)
{
	Uint8 i;
	const Uint32 * default_coefs;
	debug_printf("SEND DEFAULT COEFS...\n");
	if(QS_WHO_AM_I_get() == SMALL_ROBOT)
		default_coefs = default_coefs_small_robot;
	else
		default_coefs = default_coefs_big_robot;
	for(i=0;i<COEF_NB;i++)		//On charge les coefs par défaut.
		send_coef(i, default_coefs[i]);
}

#define PRECISION	16	//Puissance de 2... plus c'est élevé, plus on cherchera un coef précis...

//Précondition : la première trajectoire (coefs par défaut) doit fonctionner sans FAIL !!
bool_e strat_reglage_prop_compute_coefs(time32_t duration)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_REGLAGE_COMPUTE_COEFS,
				INIT,
				FIRST_MEASURE_LAUNCH,
				FIRST_MEASURE_SAVE_DURATION,
				LOOKING_FOR_MAXIMUM,
				LOOKING_FOR_MAXIMUM_COMPARE_DURATION,
				LOOKING_FOR_MINIMUM,
				LOOKING_FOR_MINIMUM_COMPARE_DURATION,
				LOOKING_FOR_BEST,
				LOOKING_FOR_BEST_ADD_OFFSET,
				LOOKING_FOR_BEST_SUB_OFFSET,
				LOOKING_FOR_BEST_ANALYSE,
				NEXT_COEF
				);
	static Uint32 best_coefs[COEF_NB];
	static Uint32 multiply_value, divide_value, offset, current_value;
	static time32_t best_duration;
	Uint8 i;
	bool_e update_coef_and_return_true = FALSE;

	debug_printf("# %ld %ld %ld %ld : %ld\n",	((current_coef==0)?current_value:best_coefs[0]),
											((current_coef==1)?current_value:best_coefs[1]),
											((current_coef==2)?current_value:best_coefs[2]),
											((current_coef==3)?current_value:best_coefs[3]),
											duration);

	switch(state)
	{
		case INIT:
			for(i=0;i<COEF_NB;i++)		//On charge les coefs par défaut.
			{
				if(QS_WHO_AM_I_get() == SMALL_ROBOT)
					best_coefs[i] = default_coefs_small_robot[i];
				else
					best_coefs[i] = default_coefs_big_robot[i];
			}
			current_coef = KPT_COMPUTE;	//Premier coef à éprouver
			state = FIRST_MEASURE_LAUNCH;
			break;
		case FIRST_MEASURE_LAUNCH:
			current_value = best_coefs[current_coef];
			multiply_value = 1;
			divide_value = 1;
			update_coef_and_return_true = TRUE;
			state = FIRST_MEASURE_SAVE_DURATION;
			break;
		case FIRST_MEASURE_SAVE_DURATION:
			best_duration = duration;
			state = LOOKING_FOR_MAXIMUM;
			break;
		case LOOKING_FOR_MAXIMUM:
			multiply_value *=2;
			current_value = best_coefs[current_coef] * multiply_value;
			update_coef_and_return_true = TRUE;
			if(multiply_value < 4096)
				state = LOOKING_FOR_MAXIMUM_COMPARE_DURATION;
			else
				state = LOOKING_FOR_MINIMUM;
			break;
		case LOOKING_FOR_MAXIMUM_COMPARE_DURATION:
			if(duration < best_duration)	//On obtient mieux, on continue de multiplier
			{
				best_coefs[current_coef] = current_value;
				best_duration = duration;
				state = LOOKING_FOR_MAXIMUM;
			}
			else							//On a trouvé le maximum..
				state = LOOKING_FOR_MINIMUM;
			break;
		case LOOKING_FOR_MINIMUM:
			divide_value *=2;
			current_value = best_coefs[current_coef] / divide_value;
			update_coef_and_return_true = TRUE;
			if(current_value != 0)
				state = LOOKING_FOR_MINIMUM_COMPARE_DURATION;
			else
				state = LOOKING_FOR_BEST;
			break;
		case LOOKING_FOR_MINIMUM_COMPARE_DURATION:
			if(duration < best_duration)	//On obtient mieux, on continue de diviser
			{
				best_coefs[current_coef] = current_value;
				best_duration = duration;
				state = LOOKING_FOR_MINIMUM;
			}
			else							//On a trouvé le maximum..
				state = LOOKING_FOR_BEST;
			break;
		case LOOKING_FOR_BEST:
			//On a une "meilleure valeur", à x2 ou /2 prêt... go pour la dichotomie...
			multiply_value = PRECISION;
			state = LOOKING_FOR_BEST_ADD_OFFSET;

			break;
		case LOOKING_FOR_BEST_ADD_OFFSET:
			multiply_value /= 2;
			if(multiply_value)
			{
				offset = (best_coefs[current_coef] * multiply_value)/PRECISION;
				current_value = best_coefs[current_coef] + offset;	//On essaye d'ajouter l'offset
				update_coef_and_return_true = TRUE;
				state = LOOKING_FOR_BEST_SUB_OFFSET;
			}
			else
				state = NEXT_COEF;
			break;
		case LOOKING_FOR_BEST_SUB_OFFSET:
			if(duration < best_duration)	//C'était mieux en ajoutant l'offset
			{
				best_coefs[current_coef] = current_value;
				best_duration = duration;
				state = LOOKING_FOR_BEST_ADD_OFFSET;
			}
			else
			{
				current_value = best_coefs[current_coef] - offset;	//On essaye d'enlever l'offset
				update_coef_and_return_true = TRUE;
				state = LOOKING_FOR_BEST_ANALYSE;
			}
			break;
		case LOOKING_FOR_BEST_ANALYSE:
			if(duration < best_duration)	//C'était mieux en retirant l'offset
			{
				best_coefs[current_coef] = current_value;
				best_duration = duration;
			}
			state = LOOKING_FOR_BEST_ADD_OFFSET;
			break;
		case NEXT_COEF:
			debug_printf("_____________________\n\tBest coef for ");
			switch(current_coef)
			{
				case KPT_COMPUTE:	debug_printf("Kp translation");	break;
				case KDT_COMPUTE:	debug_printf("Kd translation");	break;
				case KPR_COMPUTE:	debug_printf("Kp rotation");	break;
				case KDR_COMPUTE:	debug_printf("Kd rotation");	break;
				default:											break;
			}
			debug_printf("is %ld",best_coefs[current_coef]);
			current_coef = (current_coef < COEF_NB - 1)?(current_coef+1):0;
			state = FIRST_MEASURE_LAUNCH;
			break;
		default:
			break;
	}
	if(update_coef_and_return_true)
	{
		send_coef(current_coef, current_value);
		return TRUE;
	}
	return FALSE;
}


void strat_reglage_prop(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_COEF_PROP,
			INIT,
			COMPUTE_COEFS,
			WAIT_TRAJECTORY,
			TRAJECTORY_TRANSLATION,
			TRAJECTORY_CURVE,
			ROTATION,
			FAILED,
			ERROR_STATE,
			PRINT_RESULT,
			ROTATION_FAILED,
			DONE);

	static time32_t duration = 0;

	switch(state)
	{
		case INIT:
			debug_printf("Lancement de la stratégie de réglage des coefs.\n);");
			debug_printf("Assurez vous que le robot est à 50cm de la bordure derrière lui, qu'il a 2m devant lui et 80cm de chaque coté...\n");
			//Send mode BEST_EFFORT
			CAN_send_sid(DEBUG_ENABLE_MODE_BEST_EFFORT);
			PROP_set_position(1000,500,PI4096/2);
			send_default_coefs();
			state = COMPUTE_COEFS;
			break;
		case COMPUTE_COEFS:
			if(strat_reglage_prop_compute_coefs(duration))
				state = TRAJECTORY_TRANSLATION;
			break;
		case TRAJECTORY_TRANSLATION:
			CAN_send_sid(DEBUG_DO_TRAJECTORY_FOR_TEST_COEFS);
			state = WAIT_TRAJECTORY;
			//state = try_going_multipoint(displacements, 1, TRAJECTORY_TRANSLATION, TRAJECTORY_CURVE, FAILED, FORWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;
		case TRAJECTORY_CURVE:
			//state = try_going_multipoint(&displacements[1], DISPLACEMENTS_NB-1, TRAJECTORY_CURVE, ROTATION, FAILED, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;
		case ROTATION:
			//state = try_go_angle(PI4096/2, ROTATION, PRINT_RESULT, FAILED, FAST);
			break;
		case WAIT_TRAJECTORY:
			if(global.debug.duration_trajectory_for_test_coefs)
			{
				duration = global.debug.duration_trajectory_for_test_coefs;
				state = PRINT_RESULT;
			}
			if(global.prop.error)
				state = FAILED;
			//TODO gestion d'un timeout sur cette action...
			break;
		case PRINT_RESULT:
			debug_printf("END : t=%ldms\n", duration);
			state = COMPUTE_COEFS;
			break;
		case FAILED:
			if(entrance)
			{
				debug_printf("FAILED : come back home...\n");
				duration = 100000;	//fausse durée de fail : 100 secondes.
				send_coef(current_coef, ((QS_WHO_AM_I_get()==SMALL_ROBOT)?default_coefs_small_robot[current_coef]:default_coefs_big_robot[current_coef]));
			}
			state = try_going(1000,500, FAILED, ROTATION_FAILED, ERROR_STATE, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;
		case ROTATION_FAILED:
			state = try_go_angle(PI4096/2, state, COMPUTE_COEFS, FAILED, FAST, ANY_WAY, END_AT_LAST_POINT);
			break;
		case ERROR_STATE:
			if(entrance)
				debug_printf("Fin de la stratégie, échec du déplacement\n");
			break;
		default:
			break;
	}

}

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */


/*
 * Cette sub_action se rend au point (x,y) OU au noeud le plus proche si le point(x,y) est à moins de 5cm manhattan d'un noeud.
 * Ensuite, l'action marque une attente d'une seconde avant de rendre la main.
 * Si elle est appelée ou rappelée, alors qu'on est sur ce point, elle marque simplement une attente d'une seconde.
 *
 * Cas d'exemple : sub_wait(1000,COLOR_Y(2000), 1500);								//Va passer 1,5s en {1000, COLOR_Y(2000)}
 * Autre exemple intéressant : sub_wait(global.pos.x, global.pos.y, 1000);	//Marque une attente où que tu sois
 *
 * Duration en [ms]
 */
error_e sub_wait(Sint16 x, Sint16 y, time32_t duration)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_BOTH_WAIT,
			INIT,
			GET_IN_WITH_ASTAR,
			WAIT,
			ERROR,
			DONE
		);
	static time32_t local_time;

	switch(state)
	{
		case INIT:
			if(is_in_square(x-50, x+50, y-50, y+50,(GEOMETRY_point_t){global.pos.x, global.pos.y}))	//Je suis sur le point (5cm près)
				state = WAIT;
			else
				state = GET_IN_WITH_ASTAR;
			break;

		case GET_IN_WITH_ASTAR:
			state = ASTAR_try_going(x, y, state, WAIT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case WAIT:
			if(entrance)
			{
				local_time = global.match_time;
			}
			if(global.match_time > local_time + duration)
				state = DONE;
			break;

		case ERROR:
			state = INIT;
			return NOT_HANDLED;
			break;

		case DONE:
			state = INIT;
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}

#define MAX_CORRECT_POS_AT_RUSH					100			//Correction max en mm.
#define MAX_ANGLE_WHEN_CORRECT_POS_AT_RUSH		PI4096/12	//Il faut être à moins de 5° pour accepter une correction de position !
// Vérification de l'angle non actif !!!
error_e action_recalage_x(way_e sens, Sint16 angle, Sint16 wanted_x, Sint16 wanted_teta, bool_e get_out, Sint16* delta_correction_x, bool_e set_pos_at_rush, bool_e set_teta_at_rush){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_RECALAGE_X,
		INIT,
		RUSH_WALL,
		COMPUTE,
		GET_OUT,
		WAIT_FOR_EXIT,
		DONE
	);

	static bool_e success = FALSE;
	static time32_t local_time;
	static GEOMETRY_point_t escape_point;

	switch(state){
		case INIT :
			escape_point = (GEOMETRY_point_t){global.pos.x, global.pos.y};
			success = FALSE;
			state = RUSH_WALL;
			break;

		case RUSH_WALL :
			state = try_rushInTheWall(angle, state, COMPUTE, COMPUTE, sens, TRUE, 0, 0);
			break;

		case COMPUTE :
#ifdef ROBOT_VIRTUEL_PARFAIT
#warning "Désactivation des capteurs pour permettre des recalages en simu"
			*delta_correction_x = global.pos.x - wanted_x;
			PROP_set_position(wanted_x, global.pos.y, global.pos.angle);
			state = (get_out == TRUE)? GET_OUT : DONE;
			success = TRUE;
#else
			if(
				  (
					(absolute(global.pos.x - wanted_x) <= MAX_CORRECT_POS_AT_RUSH)
					|| (escape_point.x > global.pos.x && global.pos.x < wanted_x) // Si on se cale avec un x descendant et qu'on est moins haut que la position voulue c'est bon
					|| (escape_point.x < global.pos.x && global.pos.x > wanted_x) // Si on se cale avec un x ascendant et qu'on est plus haut que la position voulue c'est bon
				  )
				&& (
					(QS_WHO_AM_I_get() == SMALL_ROBOT && sens == FORWARD && RECALAGE_AV_G && RECALAGE_AV_D)
					|| (QS_WHO_AM_I_get() == SMALL_ROBOT && sens == BACKWARD && RECALAGE_AR_G && RECALAGE_AR_D)
					|| (QS_WHO_AM_I_get() == BIG_ROBOT && sens == FORWARD && RECALAGE_AV_G && RECALAGE_AV_D)
					|| (QS_WHO_AM_I_get() == BIG_ROBOT && sens == BACKWARD && RECALAGE_AR_G && RECALAGE_AR_D)
				   )
				)
			{
				success = TRUE;
				if(set_pos_at_rush){
					if(delta_correction_x != NULL)
						*delta_correction_x = global.pos.x - wanted_x;
					PROP_set_position(wanted_x, global.pos.y, (set_teta_at_rush)?wanted_teta:global.pos.angle);
					info_printf("Correction acceptée : x=%d->%d | y=%d | teta=%d\n", global.pos.x, wanted_x, global.pos.y, (set_teta_at_rush)?wanted_teta:global.pos.angle);
				}else
					info_printf("Calage réussie sans correction\n");
			}
			else
			{
				if(absolute(global.pos.x - wanted_x) > MAX_CORRECT_POS_AT_RUSH){
					info_printf("Correction Refusée (raison : Différence en x trop grande -> %d)\n", absolute(global.pos.x - wanted_x));
				}else if((QS_WHO_AM_I_get() == SMALL_ROBOT && sens == FORWARD && !RECALAGE_AV_G && !RECALAGE_AV_D)
						 || (QS_WHO_AM_I_get() == BIG_ROBOT && sens == BACKWARD && !RECALAGE_AR_G && !RECALAGE_AR_D)){
					info_printf("Correction Refusée (raison : Aucune détection de bordure pour les deux capteurs)\n");
				}else{
					if(QS_WHO_AM_I_get() == SMALL_ROBOT && sens == FORWARD){
						info_printf("Correction Refusée (raison : Aucune détection de bordure pour le capteur avant %s)\n", (RECALAGE_AV_G)?"droit":"gauche");
					}else if(QS_WHO_AM_I_get() == SMALL_ROBOT && sens == BACKWARD){
						info_printf("Correction Refusée (raison : Aucune détection de bordure pour le capteur arrière %s)\n", (RECALAGE_AR_G)?"droit":"gauche");
					}if(QS_WHO_AM_I_get() == BIG_ROBOT && sens == BACKWARD){
						info_printf("Correction Refusée (raison : Aucune détection de bordure pour le capteur arrière %s)\n", (RECALAGE_AR_G)?"droit":"gauche");
					}else if(QS_WHO_AM_I_get() == BIG_ROBOT && sens == FORWARD){
						info_printf("Correction Refusée (raison : Aucune détection de bordure pour le capteur avant %s)\n", (RECALAGE_AV_G)?"droit":"gauche");
					}else{
						info_printf("Correction Refusée (raison : Aucun capteur présent dans ce sens de recalage)\n");
					}
				}
				if(delta_correction_x != NULL)
					*delta_correction_x = 0;
				//BUZZER_play(500, DEFAULT_NOTE, 2);
			}
			state = (get_out == TRUE)? GET_OUT : DONE;
#endif
		break;

	case GET_OUT :
		state = try_going(escape_point.x, escape_point.y, state, DONE, WAIT_FOR_EXIT, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case WAIT_FOR_EXIT:		//On a pas d'autre choix que d'attendre et de réessayer périodiquement.
		if(entrance)
			local_time = global.match_time;

		if(global.match_time - local_time > 1500)
			state = GET_OUT;
		break;

	case DONE :
		state = INIT;
		if(success)
			return END_OK;
		else
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}


//set_teta_at_rush indique si on met l'angle à jour... n'est possible QUE SI on met y également à jour !
error_e action_recalage_y(way_e sens, Sint16 angle, Sint16 wanted_y, Sint16 wanted_teta, bool_e get_out, Sint16 * delta_correction_y, bool_e set_pos_at_rush, bool_e set_teta_at_rush){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_RECALAGE_Y,
		IDLE,
		RUSH_WALL,
		SECOND_RUSH_WALL_WITHOUT_ASSER_IN_ROTATION,
		COMPUTE,
		GET_OUT,
		WAIT_FOR_EXIT,
		DONE
	);

	static bool_e success = FALSE;
	static time32_t local_time;
	static GEOMETRY_point_t escape_point;

	switch(state){
		case IDLE :
			escape_point = (GEOMETRY_point_t){global.pos.x, global.pos.y};
			success = FALSE;
			state = RUSH_WALL;
			break;

		case RUSH_WALL :
			state = try_rushInTheWall(angle, state, SECOND_RUSH_WALL_WITHOUT_ASSER_IN_ROTATION, SECOND_RUSH_WALL_WITHOUT_ASSER_IN_ROTATION, sens, TRUE, 0, 0);
			break;
		case SECOND_RUSH_WALL_WITHOUT_ASSER_IN_ROTATION:
			state = try_rushInTheWall(angle, state, COMPUTE, COMPUTE, sens, FALSE, 0, 0);
			break;
		case COMPUTE :
#ifdef ROBOT_VIRTUEL_PARFAIT
#warning "Désactivation des capteurs pour permettre des recalages en simu"
			if(delta_correction_y != NULL)
				*delta_correction_y = global.pos.y - wanted_y;
			PROP_set_position(global.pos.x, wanted_y, global.pos.angle);
			state = (get_out == TRUE)? GET_OUT : DONE;
			success = TRUE;
#else

			if(
					(
					  (absolute(global.pos.y - wanted_y) <= MAX_CORRECT_POS_AT_RUSH)
					  || (escape_point.y > global.pos.y && global.pos.y < wanted_y) // Si on se cale avec un y descendant et qu'on est moins haut que la position voulue c'est bon
					  || (escape_point.y < global.pos.y && global.pos.y > wanted_y) // Si on se cale avec un y ascendant et qu'on est plus haut que la position voulue c'est bon
					)
				&& (
					(QS_WHO_AM_I_get() == SMALL_ROBOT && sens == FORWARD && RECALAGE_AV_G && RECALAGE_AV_D)
					|| (QS_WHO_AM_I_get() == SMALL_ROBOT && sens == BACKWARD)
					|| (QS_WHO_AM_I_get() == BIG_ROBOT && sens == FORWARD && RECALAGE_AV_G && RECALAGE_AV_D)
					|| (QS_WHO_AM_I_get() == BIG_ROBOT && sens == BACKWARD)
				   )
				)
			{

				success = TRUE;
				if(set_pos_at_rush){
					if(delta_correction_y != NULL)
						*delta_correction_y = global.pos.y - wanted_y;
					PROP_set_position(global.pos.x, wanted_y, (set_teta_at_rush)?wanted_teta:global.pos.angle);
					info_printf("Correction acceptée : x=%d | y=%d->%d | teta=%d\n", global.pos.x, global.pos.y, wanted_y, (set_teta_at_rush)?wanted_teta:global.pos.angle);
				}
			}
			else
			{
				if(absolute(global.pos.y - wanted_y) > MAX_CORRECT_POS_AT_RUSH){
					info_printf("Correction Refusée (raison : Différence en y trop grande -> %d)\n", absolute(global.pos.y - wanted_y));
				}else if((QS_WHO_AM_I_get() == SMALL_ROBOT && sens == FORWARD && RECALAGE_AV_G && RECALAGE_AV_D)
						 || (QS_WHO_AM_I_get() == BIG_ROBOT && sens == BACKWARD && RECALAGE_AR_G && RECALAGE_AR_D)){
					info_printf("Correction Refusée (raison : Aucune détection de bordure pour les deux capteurs)\n");
				}else{
					if(QS_WHO_AM_I_get() == SMALL_ROBOT && sens == FORWARD){
						info_printf("Correction Refusée (raison : Aucune détection de bordure pour le capteur avant %s)\n", (RECALAGE_AV_G)?"droit":"gauche");
					}else if(QS_WHO_AM_I_get() == SMALL_ROBOT && sens == BACKWARD){
						info_printf("Correction Refusée (raison : Aucune détection de bordure pour le capteur arrière %s)\n", (RECALAGE_AR_G)?"droit":"gauche");
					}if(QS_WHO_AM_I_get() == BIG_ROBOT && sens == BACKWARD){
						info_printf("Correction Refusée (raison : Aucune détection de bordure pour le capteur arrière %s)\n", (RECALAGE_AR_G)?"droit":"gauche");
					}else if(QS_WHO_AM_I_get() == BIG_ROBOT && sens == FORWARD){
						info_printf("Correction Refusée (raison : Aucune détection de bordure pour le capteur avant %s)\n", (RECALAGE_AV_G)?"droit":"gauche");
					}else{
						info_printf("Correction Refusée (raison : Aucun capteur présent dans ce sens de recalage)\n");
					}
				}
				success = FALSE;
				if(delta_correction_y != NULL)
					*delta_correction_y = 0;
				BUZZER_play(500, DEFAULT_NOTE, 2);
			}
			state = (get_out == TRUE)? GET_OUT : DONE;
#endif
			break;

		case GET_OUT :
			state = try_going(escape_point.x, escape_point.y, GET_OUT, DONE, WAIT_FOR_EXIT, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case WAIT_FOR_EXIT:		//On a pas d'autre choix que d'attendre et de réessayer périodiquement.
			if(entrance)
				local_time = global.match_time;

			if(global.match_time - local_time > 1500)
				state = GET_OUT;
			break;

		case DONE :
			state = IDLE;
			if(success)
				return END_OK;
			else
				return NOT_HANDLED;
	}
	return IN_PROGRESS;
}



void xbee_send_my_position(void)
{
	CAN_msg_t msg;
	msg.sid = XBEE_MY_POSITION_IS;
	msg.data.xbee_my_position_is.robot_id = BIG_ROBOT;
	msg.data.xbee_my_position_is.x = global.pos.x;
	msg.data.xbee_my_position_is.y = global.pos.y;
	msg.size = SIZE_XBEE_MY_POSITION_IS;
	CANMsgToXbee(&msg, FALSE);
}
