#define _SEQUENCES_C

#include "sequences.h"
#include "roadmap.h"
#include "QS/QS_macro.h"
#include "cos_sin.h"
#include "odometry.h"

//border_mode peut être BORDER_MODE_WITH_UPDATE_POSITION ou BORDER_MODE
void SEQUENCES_rush_in_the_wall(Sint16 angle, way_e way, acknowledge_e acquittement, Sint32 far_point_x, Sint32 far_point_y, border_mode_e border_mode, corrector_e corrector)
{
	Sint16 cos_a, sin_a;
	//on va vers l'angle demandé.
	ROADMAP_add_order(TRAJECTORY_ROTATION, 0,0, angle, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

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

void SEQUENCES_calibrate(way_e way,case_e case_calibrage)
{
	Sint16 x_or_y, teta;
		color_e color;
		color = ODOMETRY_get_color();

	if(color == BLUE)
	{
		ODOMETRY_set(0, 0, 0);

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 170, 0, 0, RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		teta = 0;

		SEQUENCES_rush_in_the_wall(teta, BACKWARD, NO_ACKNOWLEDGE, -4096, 0, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);


                //      1er carré: 180(petit uniquement) 2e carré : 460 3e carré : 860 4e carré 1260 5e carré 1610


                switch (case_calibrage){
                    case case_calibrage_1 :
                        x_or_y = 460;
                        break;
                    case case_calibrage_2 :
                        x_or_y = 860;
                        break;
                    case case_calibrage_3 :
                        x_or_y = 1260;
                        break;
                    default :
                        x_or_y = 460;
                        break;
                }


                ROADMAP_add_order(TRAJECTORY_TRANSLATION, x_or_y, 0, 0, RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		x_or_y = 4096;
		teta = -PI4096/2;
	//	teta = (color == RED)?-teta:teta;
		SEQUENCES_rush_in_the_wall(teta, BACKWARD, NO_ACKNOWLEDGE, 0, x_or_y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		x_or_y = -100;

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, x_or_y, 0, RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0,PI4096, RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);

	}
	else
	{
		ODOMETRY_set(0, 0, (way == FORWARD)?PI4096:0);

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 170, 0, 0, RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		teta = (way == FORWARD)?PI4096:0;

		SEQUENCES_rush_in_the_wall(teta, FORWARD_OR_BACKWARD, NO_ACKNOWLEDGE, -4096, 0, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);
	// 1er carré: 180(petit uniquement) 2e carré : 460 3e carré : 860 4e carré 1260 5e carré 1610

                                switch (case_calibrage){
                    case case_calibrage_1 :
                        x_or_y = 460;
                        break;
                    case case_calibrage_2 :
                        x_or_y = 860;
                        break;
                    case case_calibrage_3 :
                        x_or_y = 1260;
                        break;
                    default :
                        x_or_y = 460;
                        break;
                }

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 460, 0, 0, RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE);

		x_or_y = (color == RED)? 4096:-4096;
		teta = (way == FORWARD)?-PI4096/2:PI4096/2;
		teta = (color == RED)?-teta:teta;
		SEQUENCES_rush_in_the_wall(teta, FORWARD_OR_BACKWARD, NO_ACKNOWLEDGE, 0, x_or_y, BORDER_MODE_WITH_UPDATE_POSITION, CORRECTOR_ENABLE);

		x_or_y = 100;

		ROADMAP_add_order(TRAJECTORY_TRANSLATION, 0, x_or_y, 0, RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_ROTATION, 0, 0, 0, RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_CALIBRATION, CORRECTOR_ENABLE);

	}
}


/*
void SEQUENCES_CALIBRATE(way_e way)
{
	//ELOIGNEMENT BORDURES/////////////////////
	if(global.position.x > 1900)
		ROADMAP_add_order(TRANSLATION, 1800, global.position.y, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
	
	if(global.position.x <218)
		ROADMAP_add_order(TRANSLATION, 500, global.position.y, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
	
	if(global.position.y > 2785)
		ROADMAP_add_order(TRANSLATION, global.position.x, 2500, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
	if(global.position.y <210)
		ROADMAP_add_order(TRANSLATION, global.position.x, 700, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);

	/////////////////////////////////////////////////
	////EVITEMENT CENTRE...
	if( global.couleur == VERT)
	{
		if(global.position.y > 1200)
		{
			if(global.position.x < 1050)
				ROADMAP_add_order(TRANSLATION, 500, 1200, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
			else
				ROADMAP_add_order(TRANSLATION, 1500, 1200, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
		}	
		ROADMAP_add_order(TRANSLATION, 500, 2500, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);

	}
	else
	{
		if(global.position.y < 1800)
		{
			if(global.position.x < 1050)
				ROADMAP_add_order(TRANSLATION, 500, 1800, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
			else
				ROADMAP_add_order(TRANSLATION, 1500, 1800, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);

		}	
		ROADMAP_add_order(TRANSLATION, 500, 2500, 0, NOT_RELATIVE, NOT_NOW, FORWARD_OR_BACKWARD, NOT_BORDER_MODE, NOT_MULTIPOINT, RAPIDE, PAS_ACQUITTER, CORRECTOR_ENABLE);
	}

	calage_MAJ_position(way);
}	

*/
