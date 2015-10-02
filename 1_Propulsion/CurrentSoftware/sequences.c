#define _SEQUENCES_C

#include "sequences.h"
#include "roadmap.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_maths.h"
#include "odometry.h"
#include "warner.h"
#include "copilot.h"

//border_mode peut �tre BORDER_MODE_WITH_UPDATE_POSITION ou BORDER_MODE
void SEQUENCES_rush_in_the_wall(Sint16 angle, way_e way, PROP_speed_e rush_speed, acknowledge_e acquittement, Sint32 far_point_x, Sint32 far_point_y, border_mode_e border_mode, corrector_e corrector)
{
	Sint16 cos_a, sin_a;
	//on va vers l'angle demand�.
	ROADMAP_add_order(TRAJECTORY_ROTATION, 0,0, angle, PROP_ABSOLUTE, PROP_END_OF_BUFFER, ANY_WAY, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

	if(angle < 0)
		angle += 2*PI4096;

	COS_SIN_16384_get((Sint32)(angle)*4, &cos_a, &sin_a);
	cos_a /= 4;
	sin_a /= 4;


	if(way==BACKWARD)//RECULER
	{
		cos_a = -cos_a;
		sin_a = -sin_a;
	}

	//le point obtenu cos / sin est situ� � 4096 mm de notre position, et droit devant nous !
	if(far_point_x || far_point_y)
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, far_point_x, far_point_y, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, way, border_mode, PROP_NO_MULTIPOINT, rush_speed, acquittement, corrector, AVOID_DISABLED);
	else
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, cos_a, sin_a, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, way, border_mode, PROP_NO_MULTIPOINT, rush_speed, acquittement, corrector, AVOID_DISABLED);
}


void SEQUENCES_calibrate(void)
{
#ifndef SIMULATION_VIRTUAL_PERFECT_ROBOT	// Pour ne pas avoir de warning
	Sint16 x, y, teta;
#endif
	color_e color;
	color = ODOMETRY_get_color();
#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		if(color == BOT_COLOR)
			ODOMETRY_set(BIG_BOT_COLOR_CALIBRATION_X, BIG_BOT_COLOR_CALIBRATION_Y, BIG_BOT_COLOR_CALIBRATION_TETA);
		else
			ODOMETRY_set(BIG_TOP_COLOR_CALIBRATION_X, BIG_TOP_COLOR_CALIBRATION_Y, BIG_TOP_COLOR_CALIBRATION_TETA);
		COPILOT_reset_absolute_destination();
	}else{
		if(color == BOT_COLOR)
			ODOMETRY_set(SMALL_BOT_COLOR_CALIBRATION_X, SMALL_BOT_COLOR_CALIBRATION_Y, SMALL_BOT_COLOR_CALIBRATION_TETA);
		else
			ODOMETRY_set(SMALL_TOP_COLOR_CALIBRATION_X, SMALL_TOP_COLOR_CALIBRATION_Y, SMALL_TOP_COLOR_CALIBRATION_TETA);
		COPILOT_reset_absolute_destination();
	}
	WARNER_inform(WARNING_CALIBRATION_FINISHED, NO_ERROR);
#else
	if(QS_WHO_AM_I_get() == BIG_ROBOT)
	{
/*
		//Eloignement de la bordure
		if(color == BOT_COLOR)
			y = 150;
		else
			y = -150;

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y,0, RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, -(y*7)/10,0, RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);


		if(color == BOT_COLOR){
			y = -4096;
			teta = -PI4096/2;
		}else{
			y = 4096;
			teta = PI4096/2;
		}

		//Calage en Y
		SEQUENCES_rush_in_the_wall(teta, FORWARD, 16, NO_ACKNOWLEDGE, 0, y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		//Eloignement de la bordure
		if(color == BOT_COLOR)
			y = 65;
		else
			y = -65;

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y,0, RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, -350, 0,0, RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		//Calage en X
		SEQUENCES_rush_in_the_wall(PI4096, FORWARD, 16, NO_ACKNOWLEDGE, -4096, 0, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		//Eloignement de la bordure pour calage en Y
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 350, 0, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);


		//Position finale PIERRE
		if(color == BOT_COLOR){
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, BIG_BOT_COLOR_CALIBRATION_X, BIG_BOT_COLOR_CALIBRATION_Y,0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
			ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0,BIG_BOT_COLOR_CALIBRATION_TETA, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE, AVOID_DISABLED);
		}else{
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, BIG_TOP_COLOR_CALIBRATION_X, BIG_TOP_COLOR_CALIBRATION_Y,0, NOT_RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
			ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0,BIG_TOP_COLOR_CALIBRATION_TETA, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT,SLOW, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE, AVOID_DISABLED);
		}

	*/
		if(color == BOT_COLOR)
			ODOMETRY_set(BIG_BOT_COLOR_CALIBRATION_X, BIG_BOT_COLOR_CALIBRATION_Y,BIG_BOT_COLOR_CALIBRATION_TETA);
		else
			ODOMETRY_set(BIG_TOP_COLOR_CALIBRATION_X, BIG_TOP_COLOR_CALIBRATION_Y,BIG_TOP_COLOR_CALIBRATION_TETA);
		ROADMAP_add_order(TRAJECTORY_NONE,0,0,0,PROP_ABSOLUTE,PROP_END_OF_BUFFER,ANY_WAY,NOT_BORDER_MODE,PROP_NO_MULTIPOINT,SLOW,ACKNOWLEDGE_CALIBRATION,CORRECTOR_ENABLE,AVOID_DISABLED);

	}else{// SMALL ROBOT

		//J'avance pour sortir de la serviette
		y = (color==BOT_COLOR)? 300:(-300);
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, PROP_RELATIVE, PROP_END_OF_BUFFER,FORWARD, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		//On s'approche de la bordure o� il y le bac � poissons
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 750, 0, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, FORWARD, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		//Calage en X
		SEQUENCES_rush_in_the_wall(0, FORWARD, 16, NO_ACKNOWLEDGE, 4096, 0,  BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		//Eloignement de la bordure pour calage en Y
		if(color==BOT_COLOR)
			x = -(SMALL_BOT_COLOR_CALIBRATION_X - SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE);
		else
			x = -(SMALL_TOP_COLOR_CALIBRATION_X - SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE);
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, x, 0, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, ANY_WAY, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		//On s'approche de la bordure rapidement...
		y = (color==BOT_COLOR)?(-150):150;
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, BACKWARD, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		//Calage en X
		if(color==BOT_COLOR){
			teta = SMALL_BOT_COLOR_CALIBRATION_TETA;
			y = -4096;
		}else{
			teta = SMALL_TOP_COLOR_CALIBRATION_TETA;
			y = 4096;
		}

		SEQUENCES_rush_in_the_wall(teta, BACKWARD, 16, NO_ACKNOWLEDGE, 0, y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		if(color == BOT_COLOR){
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, SMALL_BOT_COLOR_CALIBRATION_X, SMALL_BOT_COLOR_CALIBRATION_Y,PI4096/2, PROP_ABSOLUTE, PROP_END_OF_BUFFER, FORWARD, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE, AVOID_DISABLED);
		}else{
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, SMALL_TOP_COLOR_CALIBRATION_X, SMALL_TOP_COLOR_CALIBRATION_Y,-PI4096/2, PROP_ABSOLUTE, PROP_END_OF_BUFFER, FORWARD, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE, AVOID_DISABLED);
		}
	}
#endif

}
void SEQUENCES_selftest(void)
{
	Sint16 cos_a, sin_a;
	Sint32 x, y;
	COS_SIN_16384_get((Sint32)(global.position.teta)*4, &cos_a, &sin_a);
	cos_a /= 4;	//->[mm*4096]
	sin_a /= 4;	//->[mm*4096]

	//Calcul du point qui est 10cm devant nous...  (en relatif, donc on calcule simplement la part de x et de y pour une distance de 10cm)...
	x = (Sint32)cos_a;
	x = (x*100)/4096;
	y = (Sint32)sin_a;
	y = (y*100)/4096;

	//Seul le premier mouvement est consid�r� comme le selftest (il peut r�ussir ou �chouer...)
	//Le second mouvement permet de revenir � la position pr�c�dente.
	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, -x, -y, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, ANY_WAY, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, ACKNOWLEDGE_SELFTEST, CORRECTOR_ENABLE, AVOID_DISABLED);
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, x, y, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, ANY_WAY, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	}else{
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, x, y, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, ANY_WAY, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, ACKNOWLEDGE_SELFTEST, CORRECTOR_ENABLE, AVOID_DISABLED);
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, -x, -y, 0, PROP_RELATIVE, PROP_END_OF_BUFFER, ANY_WAY, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	}
}

void SEQUENCES_trajectory_for_test_coefs(void)
{
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1000, 2000, 0, PROP_ABSOLUTE, PROP_END_OF_BUFFER, FORWARD, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1200, 1800, 0, PROP_ABSOLUTE, PROP_END_OF_BUFFER, BACKWARD, NOT_BORDER_MODE, PROP_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1100, 1250, 0, PROP_ABSOLUTE, PROP_END_OF_BUFFER, BACKWARD, NOT_BORDER_MODE, PROP_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 800, 750, 0, PROP_ABSOLUTE, PROP_END_OF_BUFFER, BACKWARD, NOT_BORDER_MODE, PROP_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1000, 500, 0, PROP_ABSOLUTE, PROP_END_OF_BUFFER, BACKWARD, NOT_BORDER_MODE, PROP_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_ROTATION, 0,0, PI4096/2, PROP_ABSOLUTE, PROP_END_OF_BUFFER, ANY_WAY, NOT_BORDER_MODE, PROP_NO_MULTIPOINT, FAST, ACKNOWLEDGE_TRAJECTORY_FOR_TEST_COEFS, CORRECTOR_ENABLE, AVOID_DISABLED);
}
