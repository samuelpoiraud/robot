#define _SEQUENCES_C

#include "sequences.h"
#include "roadmap.h"
#include "QS/QS_macro.h"
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
	ROADMAP_add_order(TRAJECTORY_ROTATION, 0,0, angle, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

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
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, far_point_x, far_point_y, 0, RELATIVE, NOT_NOW, way, border_mode, NO_MULTIPOINT, rush_speed, acquittement, corrector, AVOID_DISABLED);
	else
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, cos_a, sin_a, 0, RELATIVE, NOT_NOW, way, border_mode, NO_MULTIPOINT, rush_speed, acquittement, corrector, AVOID_DISABLED);
}

// Calibration de PIERRE ET GUY
void SEQUENCES_calibrate()
{
	Sint16 x, y, teta;
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
	if(QS_WHO_AM_I_get() == BIG_ROBOT){

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
	}else{// GUY
		if(color == BOT_COLOR)
			y = 50;
		else
			y = -50;

		//Eloignement de la bordure
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		if(color == BOT_COLOR){
			y = -4096;
			teta = -PI4096/2;
		}else{
			y = 4096;
			teta = PI4096/2;
		}

		//Calage en Y
		SEQUENCES_rush_in_the_wall(teta, FORWARD, 16, NO_ACKNOWLEDGE, 0, y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		if(color == BOT_COLOR)
			y = 320;
		else
			y = -320;

		//Eloignement de la bordure pour calage en X
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 870,0, 0, RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		//Calage en X
		x = 4096;
		teta = 0;
		SEQUENCES_rush_in_the_wall(teta, FORWARD, 16, NO_ACKNOWLEDGE, x, 0, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		//Fait avancer du bord
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, -1000, 0,0, RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

		if(color == BOT_COLOR){
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, SMALL_BOT_COLOR_CALIBRATION_X, SMALL_BOT_COLOR_CALIBRATION_Y,0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE, AVOID_DISABLED);
			//ROADMAP_add_order(TRAJECTORY_ROTATION,0,0,SMALL_BOT_COLOR_CALIBRATION_TETA, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);
		}else{
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, SMALL_TOP_COLOR_CALIBRATION_X, SMALL_TOP_COLOR_CALIBRATION_Y,0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE, AVOID_DISABLED);
			//ROADMAP_add_order(TRAJECTORY_ROTATION,0,0,SMALL_TOP_COLOR_CALIBRATION_TETA, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);
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
	ROADMAP_add_order(TRAJECTORY_TRANSLATION, x, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, ACKNOWLEDGE_SELFTEST, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_TRANSLATION, -x, -y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);

}

void SEQUENCES_trajectory_for_test_coefs(void)
{
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1000, 2000, 0, NOT_RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1200, 1800, 0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1100, 1250, 0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 800, 750, 0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1000, 500, 0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED);
	ROADMAP_add_order(TRAJECTORY_ROTATION, 0,0, PI4096/2, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_TRAJECTORY_FOR_TEST_COEFS, CORRECTOR_ENABLE, AVOID_DISABLED);
}
