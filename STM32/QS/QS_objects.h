
#ifndef QS_OBJECTS_SCAN_H
#define  QS_OBJECTS_SCAN_H

#include "QS_all.h"
#include "QS_maths.h"

#define NB_MAX_POLYGONS (20)
#define NB_MAX_SUMMITS (10)

#define NB_MAX_CIRCLES (20)

#define OBJECT_MARGIN (20)

typedef enum{
	OBJECT_ABSENT,
	OBJECT_PRESENT,
	OBJECT_NO_DATA
}OBJECTS_state_e;

typedef struct{
	GEOMETRY_point_t summits[NB_MAX_SUMMITS];
	Uint8 nb_summits;
}OBJECTS_polygon_t;

typedef struct{
	OBJECTS_polygon_t polygons[NB_MAX_CIRCLES];
	Uint8 nb_polygons;
	OBJECTS_state_e state[NB_MAX_CIRCLES];
}OBJECTS_polygons_t;

typedef struct{
	GEOMETRY_circle_t circles[NB_MAX_POLYGONS];
	Uint8 nb_circles;
	OBJECTS_state_e state[NB_MAX_POLYGONS];
}OBJECTS_circles_t;

typedef enum{
	ROCKET_BLUE,
	ROCKET_YELLOW,
	ROCKET_POLYC_BS,
	ROCKET_POLYC_YS,
	MODULE_POLYC1_BS,
	MODULE_POLYC2_BS,
	MODULE_POLYC3_BS,
	MODULE_POLYC1_YS,
	MODULE_POLYC2_YS,
	MODULE_POLYC3_YS,
	MODULE_BLUE_N,
	MODULE_BLUE_S,
	MODULE_YELLOW_N,
	MODULE_YELLOW_S,
	BIG_CRATER_BS,
	BIG_CRATER_YS,
	SMALL_CRATER_NORTH_BS,
	SMALL_CRATER_SOUTH_BS,
	SMALL_CRATER_NORTH_YS,
	SMALL_CRATER_SOUTH_YS,
	NB_CIRCLES
}OBJECTS_circle_name_e;

typedef enum{
	DEPOSE_BLUE,
	DEPOSE_YELLOW,
	DEPOSE_MIDDLE_BS,
	DEPOSE_MIDDLE_M,
	DEPOSE_MIDDLE_YS,
	NB_POLYGONS
}OBJECTS_polygon_name_e;



void OBJECTS_init();


#endif /*  QS_OBJECTS_SCAN_H */

