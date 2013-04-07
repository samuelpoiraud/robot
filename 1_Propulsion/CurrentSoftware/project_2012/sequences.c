#define _SEQUENCES_C

#include "sequences.h"
#include "roadmap.h"
#include "QS/QS_macro.h"
#include "QS/QS_CANmsgList.h"
#include "cos_sin.h"
#include "odometry.h"

//border_mode peut être BORDER_MODE_WITH_UPDATE_POSITION ou BORDER_MODE
void SEQUENCES_rush_in_the_wall(Sint16 angle, way_e way, acknowledge_e acquittement, Sint32 far_point_x, Sint32 far_point_y, border_mode_e border_mode, corrector_e corrector)
{
	Sint16 cos_a, sin_a;
	//on va vers l'angle demandé.
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

	//le point obtenu cos / sin est situé à 4096 mm de notre position, et droit devant nous !
	if(far_point_x || far_point_y)
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, far_point_x, far_point_y, 0, RELATIVE, NOT_NOW, way, border_mode, NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, acquittement, corrector);
	else
		ROADMAP_add_order(TRAJECTORY_TRANSLATION, cos_a, sin_a, 0, RELATIVE, NOT_NOW, way, border_mode, NO_MULTIPOINT, SLOW_TRANSLATION_AND_FAST_ROTATION, acquittement, corrector);
}

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
	
	// 1er carré: 180(petit uniquement) 2e carré : 460 3e carré : 860 4e carré 1260 5e carré 1610
	switch (calibration_square)
	{
		case ASSER_CALIBRATION_SQUARE_0:
			x = 180;
		break;
		case ASSER_CALIBRATION_SQUARE_1 :
			x = 460;
		break;
		case ASSER_CALIBRATION_SQUARE_2 :
			x = 860;
		break;
		case ASSER_CALIBRATION_SQUARE_3 :
			x = 1260;
		break;
		default :
			x = 460;
		break;
	}
	
	//On avance jusqu'au X souhaité
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
	   	
	//Calibrage en Y -> dans le sens demandé !
	SEQUENCES_rush_in_the_wall(teta, way, NO_ACKNOWLEDGE, 0, y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

	//Jle fait avancer de 0 centimetres (collé au mur : obligation de match) xD.
	//x_or_y = (color == BLUE)? -0:0;
	// ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, x_or_y, 0, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);
	//finalement on ne le fait pas tourner car on est collé au mur
	//ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0,PI4096, RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);

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
