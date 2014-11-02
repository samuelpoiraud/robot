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


	#ifdef USE_MATHS_FILTER

	// IMPORTANT IL FAUT UTILISER UN TABLEAU DE TYPE SINT32, sinon ça va merder violement !

	// L'utilisation d'un filtre à temps futur ou passé introduit un déphasage (avance pour temps futur, retard pour temps passé)
	// Pour contrer ce déphasage il est conseillé d'utiliser les deux filtres à la fois.
	// ex :
	//		filter_future_time(values, 30, (float[]){1/2., 1/2.}, 2);
	//		filter_past_time(values, 30, (float[]){1/2., 1/2.}, 2);
	//
	//	Subtilité pour dérivé votre tableau de donnée faite :
	//		filter_future_time(values, 30, (float[]){1, -1}, 2);

	/** Filtre à temps future
		 * @param values	: tableau de donnée à traiter
		 * @param nb_value	: nombre de valeur à traiter dans le tableau
		 * @param factor	: tableau des facteurs
		 * @param nb_factor	: nombre de facteurs
		 *
		 * @brief exemple : filtre moyenneur sur deux valeurs :
		 *						filter_future_time(values, 30, (float[]){1/2., 1/2.}, 2);
		 *						résultat pour la première valeur :	values[0] = 1/2 * values[0] + 1/2 values[1];
		 *						effet de bord : values[29] = values[29];
		 */
		void filter_future_time(Sint32 values[], Uint16 nb_value, float factor[], Uint8 nb_factor);

		/** Filtre à temps passé
			 * @param values	: tableau de donnée à traiter
			 * @param nb_value	: nombre de valeur à traiter dans le tableau
			 * @param factor	: tableau des facteurs
			 * @param nb_factor	: nombre de facteurs
			 *
			 * @brief exemple : filtre moyenneur sur deux valeurs :
			 *						filter_past_time(values, 30, (float[]){1/2., 1/2.}, 2);
			 *						résultat pour la première valeur :	values[29] = 1/2 * values[29] + 1/2 values[28];
			 *						effet de bord : values[0] = values[0];
			 */
		void filter_past_time(Sint32 values[], Uint16 nb_value, float factor[], Uint8 nb_factor);
	#endif

	#ifdef FAST_COS_SIN
		void COS_SIN_4096_get(Sint16 teta,Sint16 * cos, Sint16 * sin);
		void COS_SIN_16384_get(Sint32 teta, Sint16 * cos, Sint16 * sin);
	#endif

	double cos4096(Sint16 angle);
	double sin4096(Sint16 angle);
	double tan4096(int angle);
	Sint16 atan4096(double tangent);
	Sint16 atan2_4096(double y, double x);

	Sint64 puissance(Sint64 value, Uint8 nb);
	float puissance_float(float value, int nb);
	Uint64 factorielle (Uint32 nb);

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
	bool_e is_in_circle(GEOMETRY_point_t current, GEOMETRY_circle_t circle);

	/**
	 * @brief is_in_quadri
	 * @pre Les points doivent être donné pour que le quadrilatère soit connexe (chaques points voit tout les points)
	 * @param points	Le tableau de point du quadrilatère (doit être dans l'ordre)
	 * @param tested_point Le point à tester
	 * @return si le point est dans le quadrilatère
	 */
	bool_e is_in_quadri(GEOMETRY_point_t points[4], GEOMETRY_point_t tested_point);



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
