#define _SEQUENCES_C

#include "sequences.h"
#include "roadmap.h"
#include "QS/QS_macro.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_who_am_i.h"
#include "cos_sin.h"
#include "odometry.h"


//border_mode peut �tre BORDER_MODE_WITH_UPDATE_POSITION ou BORDER_MODE
void SEQUENCES_rush_in_the_wall(Sint16 angle, way_e way, acknowledge_e acquittement, Sint32 far_point_x, Sint32 far_point_y, border_mode_e border_mode, corrector_e corrector)
{
	Sint16 cos_a, sin_a;
	//on va vers l'angle demand�.
	ROADMAP_add_order(TRAJECTORY_ROTATION, 0,0, angle, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

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
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, far_point_x, far_point_y, 0, RELATIVE, NOT_NOW, way, border_mode, NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, acquittement, corrector);
	else
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, cos_a, sin_a, 0, RELATIVE, NOT_NOW, way, border_mode, NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, acquittement, corrector);
}

// Calibration de PIERRE ET GUY
void SEQUENCES_calibrate()
{
	Sint16 x, y, teta;
	color_e color;
	color = ODOMETRY_get_color();

	if(QS_WHO_AM_I_get() == PIERRE){
		if(color == RED)
			y = 100;
		else
			y = -100;

		//Eloignement de la bordure
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		if(color == RED){
			y = -4096;
			teta = PI4096/2;
		}else{
			y = 4096;
			teta = -PI4096/2;
		}

		//Calage en Y
		SEQUENCES_rush_in_the_wall(teta, BACKWARD, NO_ACKNOWLEDGE, 0, y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		if(color == RED)
			y = 80;
		else
			y = -80;

		//Eloignement de la bordure pour calage en X
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		//Calage en X
		x = -4096;
		teta = 0;
		SEQUENCES_rush_in_the_wall(teta, BACKWARD, NO_ACKNOWLEDGE, x, 0, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		//Fait avancer du bord
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 80, 0,0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		//Position finale PIERRE
		if(color == RED){
			ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0,PI4096, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, BIG_RED_CALIBRATION_X, BIG_RED_CALIBRATION_Y,0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
			ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0,BIG_RED_CALIBRATION_TETA, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
		}else{
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, BIG_YELLOW_CALIBRATION_X, BIG_YELLOW_CALIBRATION_Y,0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
			ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0,BIG_YELLOW_CALIBRATION_TETA, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT,SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
		}
	}else{// GUY
		if(color == RED)
			y = 100;
		else
			y = -100;

		//Eloignement de la bordure
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		if(color == RED){
			y = -4096;
			teta = -PI4096/2;
		}else{
			y = 4096;
			teta = PI4096/2;
		}

		//Calage en Y
		SEQUENCES_rush_in_the_wall(teta, FORWARD, NO_ACKNOWLEDGE, 0, y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		if(color == RED)
			y = 320;
		else
			y = -320;

		//Eloignement de la bordure pour calage en X
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 800,0, 0, RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		//Calage en X
		x = 4096;
		teta = 0;
		SEQUENCES_rush_in_the_wall(teta, FORWARD, NO_ACKNOWLEDGE, x, 0, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		//Fait avancer du bord
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, -1000, 0,0, RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		if(color == RED){
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, SMALL_RED_CALIBRATION_X, SMALL_RED_CALIBRATION_Y,0, NOT_RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
			ROADMAP_add_order(TRAJECTORY_ROTATION,0,0,SMALL_RED_CALIBRATION_TETA, NOT_RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
		}else{
			ROADMAP_add_order(TRAJECTORY_TRANSLATION, SMALL_YELLOW_CALIBRATION_X, SMALL_YELLOW_CALIBRATION_Y,0, NOT_RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
			ROADMAP_add_order(TRAJECTORY_ROTATION,0,0,SMALL_YELLOW_CALIBRATION_TETA, NOT_RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
		}
	}

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
	ROADMAP_add_order(TRAJECTORY_TRANSLATION, x, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, ACKNOWLEDGE_SELFTEST, CORRECTOR_ENABLE);
	ROADMAP_add_order(TRAJECTORY_TRANSLATION, -x, -y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

}

/*   TINY ET KRUSTY
void SEQUENCES_calibrate(way_e way, calibration_square_e calibration_square)
{
	Sint16 x, y, teta;
	color_e color;
	color = ODOMETRY_get_color();

	if(color == BLUE)
		y = -550;
	else
		y = 550;

	//Eloignement de la bordure
	ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

	//Calage en X
	teta = 0;
	x = -4096;
	SEQUENCES_rush_in_the_wall(teta, BACKWARD, NO_ACKNOWLEDGE, x, 0, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

	// 1er carr�: 180(petit uniquement) 2e carr� : 460 3e carr� : 860 4e carr� 1260 5e carr� 1610
	switch (calibration_square)
	{
		case ASSER_CALIBRATION_SQUARE_0:
			x = 250;
		break;
		case ASSER_CALIBRATION_SQUARE_1 :
			x = 600;
		break;
		case ASSER_CALIBRATION_SQUARE_2 :
			x = 1000;
		break;
		case ASSER_CALIBRATION_SQUARE_3 :
			x = 1400;
		break;
		default :
			x = 600;
		break;
	}
	//On retranche du x voulu la coordonn�e x qui sera atteinte lors du callage car on est oblig�s de faire un move relative !
	if(QS_WHO_AM_I_get()==TINY)
		x -=  TINY_CALIBRATION_BACKWARD_BORDER_DISTANCE;
	else
		x -= KRUSTY_CALIBRATION_BACKWARD_BORDER_DISTANCE;

	//On avance jusqu'au X souhait�
	ROADMAP_add_order(TRAJECTORY_TRANSLATION, x, 0, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

	if(color == BLUE)
	{
		y = 4096;
		if(way == FORWARD)
			teta = PI4096/2;
		else
			teta = -PI4096/2;
	}
	else
	{
		y = -4096;
		if(way == FORWARD)
			teta = -PI4096/2;
		else
			teta = PI4096/2;
	}

	//Calibrage en Y -> dans le sens demand� !
	SEQUENCES_rush_in_the_wall(teta, way, NO_ACKNOWLEDGE, 0, y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

	//Jle fait avancer de 0 centimetres (coll� au mur : obligation de match) xD.
	//x_or_y = (color == BLUE)? -0:0;

	//Trajectoire sans effet simplement pour g�n�rer l'acquittement de la calibration
	ROADMAP_add_order(TRAJECTORY_STOP, 0, 0, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);



	//finalement on ne le fait pas tourner car on est coll� au mur
	//ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0,PI4096, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);

}*/


void SEQUENCES_trajectory_for_test_coefs(void)
{
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1000, 2000, 0, NOT_RELATIVE, NOT_NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1200, 1800, 0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1100, 1250, 0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 800, 750, 0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
	ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 1000, 500, 0, NOT_RELATIVE, NOT_NOW, BACKWARD, NOT_BORDER_MODE, MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);
	ROADMAP_add_order(TRAJECTORY_ROTATION, 0,0, PI4096/2, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_TRAJECTORY_FOR_TEST_COEFS, CORRECTOR_ENABLE);
}


/*
void SEQUENCES_CALIBRATE(way_e way)
{
	//ELOIGNEMENT BORDURES/////////////////////
	if(global.position.x > 1900)
		ROADMAP_add_order(TRANSLATION, 1800, global.position.y, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);

	if(global.position.x <218)
		ROADMAP_add_order(TRANSLATION, 500, global.position.y, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);

	if(global.position.y > 2785)
		ROADMAP_add_order(TRANSLATION, global.position.x, 2500, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
	if(global.position.y <210)
		ROADMAP_add_order(TRANSLATION, global.position.x, 700, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);

	/////////////////////////////////////////////////
	////EVITEMENT CENTRE...
	if( global.couleur == VERT)
	{
		if(global.position.y > 1200)
		{
			if(global.position.x < 1050)
				ROADMAP_add_order(TRANSLATION, 500, 1200, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
			else
				ROADMAP_add_order(TRANSLATION, 1500, 1200, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
		}
		ROADMAP_add_order(TRANSLATION, 500, 2500, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);

	}
	else
	{
		if(global.position.y < 1800)
		{
			if(global.position.x < 1050)
				ROADMAP_add_order(TRANSLATION, 500, 1800, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
			else
				ROADMAP_add_order(TRANSLATION, 1500, 1800, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);

		}
		ROADMAP_add_order(TRANSLATION, 500, 2500, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
	}

	calage_MAJ_position(way);
}

*/
