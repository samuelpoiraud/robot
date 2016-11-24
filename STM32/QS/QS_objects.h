
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
	OBJECTS_polygon_t presence;
	OBJECTS_polygon_t absence;
	OBJECTS_state_e state;
}OBJECTS_polygons_t;

typedef struct{
	GEOMETRY_circle_t presence;
	GEOMETRY_circle_t absence;
	OBJECTS_state_e state;
}OBJECTS_circles_t;

typedef enum{
	OBJ_ROCKET_BLUE,
	OBJ_ROCKET_YELLOW,
	OBJ_ROCKET_POLYC_BS,
	OBJ_ROCKET_POLYC_YS,
	OBJ_MODULE_POLYC1_BS,
	OBJ_MODULE_POLYC2_BS,
	OBJ_MODULE_POLYC3_BS,
	OBJ_MODULE_POLYC1_YS,
	OBJ_MODULE_POLYC2_YS,
	OBJ_MODULE_POLYC3_YS,
	OBJ_MODULE_BLUE_N,
	OBJ_MODULE_BLUE_S,
	OBJ_MODULE_YELLOW_N,
	OBJ_MODULE_YELLOW_S,
	OBJ_BIG_CRATER_BS,
	OBJ_BIG_CRATER_YS,
	OBJ_SMALL_CRATER_NORTH_BS,
	OBJ_SMALL_CRATER_SOUTH_BS,
	OBJ_SMALL_CRATER_NORTH_YS,
	OBJ_SMALL_CRATER_SOUTH_YS,
	OBJ_NB_CIRCLES
}OBJECTS_circle_name_e;

typedef enum{
	OBJ_DEPOSE_BLUE,
	OBJ_DEPOSE_YELLOW,
	OBJ_DEPOSE_MIDDLE_BS,
	OBJ_DEPOSE_MIDDLE_M,
	OBJ_DEPOSE_MIDDLE_YS,
	OBJ_NB_POLYGONS
}OBJECTS_polygon_name_e;



void OBJECTS_init();


#endif /*  QS_OBJECTS_SCAN_H */

