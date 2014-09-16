/*
 *	Club Robot ESEO 2010 - 2011
 *	CkeckNorris
 *
 *	Fichier : sick.h
 *	Package : Carte Principale
 *	Description : Traitement des données des capteurs SICK (télémètres LASER DT50)
 *	Auteur : Ronan
 *	Version 20110304
 */

#include "QS/QS_all.h"
#include "config_use.h"

#ifndef SICK_H
	#define SICK_H

	#ifdef USE_SICK

	#include "Geometry.h"

	#if (defined(USE_SICK_DT50_LEFT) || defined(USE_SICK_DT50_RIGHT) || defined(USE_SICK_DT50_TOP)) && !defined(USE_SCHEDULED_POSITION_REQUEST)
		#error "Attention : pour utiliser la detection des elements de jeu, il faut que la carte propulsion envoie des deltas en translation et/ou rotation. Si vous avez un probleme avec ça, allez voir Ronan."
	#endif

	/* Informations des DT50 sur le robot */
	#define X_DT50_LEFT 				-57
	#define Y_DT50_LEFT 				160
	#define ANGLE_DT50_LEFT				PI4096/2

	#define X_DT50_RIGHT 				57
	#define Y_DT50_RIGHT 				-160
	#define ANGLE_DT50_RIGHT			-PI4096/2

	#define X_DT50_TOP	 				55
	#define Y_DT50_TOP	 				160
	#define ANGLE_DT50_TOP				PI4096/2

	typedef enum
	{
		DT50_LEFT,
		DT50_RIGHT,
		DT50_TOP,		// utilisé pour la detection des lingots
		NB_DISTANCE_SENSOR
	}sick_distance_sensor_e;

	/* Fonction d'initialisation */
	void SICK_init();

	/* Mise à jour ... */
	void SICK_update();

	/* Cette fonction est appelée en IT sur réception d'un message can de la carte propulsion envoyé
	   à chaque fois que le robot à bougé d'une certaine distance ou angle */
	void SICK_update_points();

	/* Accesseur sur la distance du capteur passé en parametre */
	Uint16 SICK_get_distance(sick_distance_sensor_e sensor);


	/*accesseur qui retourn le point scané par le capteur passé en paramètre*/
	GEOMETRY_point_t SICK_get_point_position(sick_distance_sensor_e sensor);


	#ifdef SICK_C

		#include "maths_home.h"
		#include "elements.h"
		#include "QS/QS_adc.h"

		#ifdef DEBUG_SICK
			#define sick_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define sick_printf(...)	(void)0
		#endif

		#define MIN_ADC						187
		#define MAX_ADC						905
		#define NO_DISTANCE_ADC				905
		#define DT50_RESPONSE_TIME 			20 		// 20 ms (cf. datasheet)

		#define NB_GOOD_POINTS_MAX 			8 					// Cette valeurs est pour l'instant prise au hasard.
																// Elle est définie en fonction de la distance/angle
																// parcourue entre deux appels consécutifs de SICK_update()
		#define NB_MIN_POINTS 				6

		#define MIN_DISTANCE_UNDER_POINTS 	225 				// Attention valeur au carré ! (correspond à 15 mm)
		#define DELTA_MAX_UNDER_CENTERS_CALCULATED 100			// Attention valeur au carré ! (correspond à 15 mm)
		#define	POINTS_SPREAD_DISTANCE ((ELEMENTS_DIAMETER-30)*(ELEMENTS_DIAMETER-30)) 	// (170 * 170) mm (A voir en fonction du delta_translation/delta_rotation)
		#define NB_CIRCLES_INTERSECTIONS_MAX (NB_GOOD_POINTS_MAX*(NB_GOOD_POINTS_MAX-1) >> 1) 	// N(N-1)/2

		#define BORDER_DELTA 				50 		// 50 mm

		typedef struct
		{
			adc_id_e pinADC;
			Uint16 min_adc_value, max_adc_value;
			time32_t response_time;
			Sint16 x;
			Sint16 y;
			Sint16 angle;
			game_element_e type_of_element_detected;
		}distance_sensor_config_t;

		typedef struct
		{
			distance_sensor_config_t const* config;
			GEOMETRY_point_t point;
			bool_e point_updated;
			GEOMETRY_point_t good_points[NB_GOOD_POINTS_MAX];
			Uint8 nb_points;
			Uint16 distance;
			bool_e locked;
		}distance_sensor_t;

		/* Cette fonction s'occupe de transférer le point du repère du robot dans le repère
		   du terrain.
		   Elle renvoie {-1,-1} si la valeur adc n'est pas dans la plage définie */
		static GEOMETRY_point_t SICK_get_point(sick_distance_sensor_e sick_sensor);

	#endif /* def SICK_C */

	#endif /* USE_SICK */
#endif /* ndef SICK_H */
