/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : QS_maths.h
 *  Package : Qualité Soft
 *  Description : Regroupement de toutes les fonctions mathématiques utiles
 *  Auteur : Arnaud
 *  Version 20130518
 */

#include "QS_all.h"

#ifndef QS_MATHS_H
	#define QS_MATHS_H

	#include "math.h"

	#define SQUARE(x)		((x)*(x))

	Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

	#ifdef FAST_COS_SIN
		void COS_SIN_4096_get(Sint16 teta,Sint16 * cos, Sint16 * sin);
		void COS_SIN_16384_get(Sint32 teta, Sint16 * cos, Sint16 * sin);
	#endif

	double cos4096(Sint16 angle);
	double sin4096(Sint16 angle);
	Sint16 atan4096(double tangent);

	typedef struct {
		Sint16 x;
		Sint16 y;
	} GEOMETRY_point_t;

	typedef struct {
		Sint16 x;
		Sint16 y;
	} GEOMETRY_vector_t;

	typedef struct {
		GEOMETRY_point_t a;
		GEOMETRY_point_t b;
	} GEOMETRY_segment_t;

	typedef struct {
		GEOMETRY_point_t c;
		Uint16 r;
	} GEOMETRY_circle_t;

	Uint16 GEOMETRY_distance(GEOMETRY_point_t a, GEOMETRY_point_t b);
	Uint32 GEOMETRY_squared_distance(GEOMETRY_point_t a, GEOMETRY_point_t b);
	bool_e GEOMETRY_segments_intersects(GEOMETRY_segment_t s1,GEOMETRY_segment_t s2);
	Sint16 GEOMETRY_viewing_angle(Sint16 start_x, Sint16 start_y, Sint16 destination_x, Sint16 destination_y);
	Sint16 GEOMETRY_modulo_angle(Sint16 angle);
	GEOMETRY_point_t GEOMETRY_segment_middle(GEOMETRY_segment_t s);
	GEOMETRY_circle_t GEOMETRY_circle_from_diameter(GEOMETRY_segment_t diameter);
	GEOMETRY_segment_t GEOMETRY_circle_intersections(GEOMETRY_circle_t c0, GEOMETRY_circle_t c1);

	bool_e is_in_square(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2, GEOMETRY_point_t current);
	bool_e i_am_in_square(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2);
	bool_e is_in_circle(GEOMETRY_point_t current, GEOMETRY_circle_t circle);

	#define PI16384				51472
	#define HALF_PI16384		25736
	#define QUATER_PI16384		12868
	#define THREE_HALF_PI16384	77208
	#define	TWO_PI16384			102944
	#define PI_22				(13176795)		/*Valeur de PI<<22 */
	#define TWO_PI22			(26353589)

	#define TWO_PI4096			(25736)
	#define HALF_PI4096			(6434)
	#define THREE_HALF_PI4096	(19302)

#endif /* ndef QS_MATHS_H */
