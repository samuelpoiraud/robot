/*
 * hokuyo.h
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */

#ifndef HOKUYO_H_
#define HOKUYO_H_
//#define TRIANGULATION
#define NB_MESURES_HOKUYO 4

#include "QS/QS_all.h"

#ifdef USE_HOKUYO


	#define HOKUYO_MAX_FOES	16

	typedef struct{
		Sint32 dist;
		Sint16 teta;
		Sint32 coordX;
		Sint32 coordY;
	}HOKUYO_adversary_position;

	typedef struct{
		Sint16 power_intensity;
		Sint16 teta;
		Sint32 coordX;
		Sint32 coordY;
	}HOKUYO_point_position;

	void HOKUYO_init(void);
	void HOKUYO_process_main(void);
	void HOKUYO_process_it(Uint8 ms);
	bool_e HOKUYO_is_working_well(void);
	void refresh_adversaries(void);

	#ifdef TRIANGULATION
	typedef struct{       //position des balises
		Sint32 x;
		Sint32 y;
		Uint32 teta;
		Uint32 weight;
	}position;
	void tri_points();
	void Hokuyo_validPointsAndBeacons();

	//Fonction de tri des tableaux de mesures (tri à bulles)
	void tri_tableau(HOKUYO_adversary_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon);

	//Fonction permettant d'éliminer les points bizarres
	void tri_mesures(HOKUYO_adversary_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon);

	//Fonction prenant la moyenne des points mesurés pour chaque angle donné (ie tout les 0,25°)
	void moyenne_mesures(HOKUYO_adversary_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon);

	//Fonction réalisant la régression circulaire d'un nuage de points
	HOKUYO_adversary_position regression_circulaire(HOKUYO_adversary_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon);

	//Fonction trouvant le centre des balises
	void find_beacons_centres();
	#endif

#endif

#endif /* HOKUYO_H_ */
