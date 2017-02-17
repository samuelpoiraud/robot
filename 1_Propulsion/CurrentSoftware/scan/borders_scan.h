#ifndef BORDERS_SCAN_H
#define BORDERS_SCAN_H

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

scan_zone_e BORDERS_SCAN_treatment(GEOMETRY_point_t pos_mesure);

bool_e calculeZonePublic(scan_zone_e zone);

#endif /* BORDERS_SCAN_H */
