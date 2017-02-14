#ifndef BORDERS_SCAN_H
#define BORDERS_SCAN_H

#include "scan.h"

void BORDERS_SCAN_init();
GEOMETRY_point_t* BORDERS_SCAN_treatment(GEOMETRY_point_t pos_mesure);

bool_e calculeZonePublic(GEOMETRY_point_t* zone);



#endif /* BORDERS_SCAN_H */
