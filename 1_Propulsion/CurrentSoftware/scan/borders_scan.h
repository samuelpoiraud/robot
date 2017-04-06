#ifndef BORDERS_SCAN_H
	#define BORDERS_SCAN_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_maths.h"


	#ifdef SCAN_BORDURE

		#include "scan.h"

		void BORDERS_SCAN_init();

		typedef enum{
			BLUE_NORTH,
			MIDDLE_NORTH,
			YELLOW_NORTH,
			BLUE_START,
			BLUE_CORNER,
			YELLOW_START,
			YELLOW_CORNER,
			BLUE_SOUTH,
			YELLOW_SOUTH,
			OTHER_ZONE
		}scan_zone_e;

		typedef struct{
			GEOMETRY_point_t scan_point;
			Uint16 scan_time;//en nombre d'it
		}scan_point_time_t;

		void BORDERS_SCAN_process_main();


		scan_zone_e BORDERS_SCAN_treatment(GEOMETRY_point_t pos_mesure);

		bool_e BORDERS_SCAN_calculeZonePublic(scan_zone_e zone);

		void BORDERS_SCAN_afficheDebug();

	#endif

#endif /* BORDERS_SCAN_H */
