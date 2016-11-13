

#include "QS_objects.h"


OBJECTS_circles_t OBJECTS_circles;
OBJECTS_polygons_t OBJECTS_polygons;


void OBJECTS_init(){

	Uint8 i;

	// Définition des cercles

	OBJECTS_circles.circles[ROCKET_BLUE] = (GEOMETRY_circle_t){{40 + OBJECT_MARGIN, 1150}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[ROCKET_YELLOW] = (GEOMETRY_circle_t){{40 + OBJECT_MARGIN, 1850}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[ROCKET_POLYC_BS] = (GEOMETRY_circle_t){{1350, 40 + OBJECT_MARGIN}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[ROCKET_POLYC_YS] = (GEOMETRY_circle_t){{1350, 2960 - OBJECT_MARGIN}, 40 + OBJECT_MARGIN};

	OBJECTS_circles.circles[MODULE_POLYC1_BS] = (GEOMETRY_circle_t){{600, 1000}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[MODULE_POLYC2_BS] = (GEOMETRY_circle_t){{1100, 500}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[MODULE_POLYC3_BS] = (GEOMETRY_circle_t){{1400, 900}, 40 + OBJECT_MARGIN};

	OBJECTS_circles.circles[MODULE_POLYC1_YS] = (GEOMETRY_circle_t){{600, 2000}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[MODULE_POLYC2_YS] = (GEOMETRY_circle_t){{1100, 2500}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[MODULE_POLYC3_YS] = (GEOMETRY_circle_t){{1400, 2100}, 40 + OBJECT_MARGIN};

	OBJECTS_circles.circles[MODULE_BLUE_N] = (GEOMETRY_circle_t){{600, 200}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[MODULE_BLUE_S] = (GEOMETRY_circle_t){{1850, 800}, 40 + OBJECT_MARGIN};

	OBJECTS_circles.circles[MODULE_YELLOW_N] = (GEOMETRY_circle_t){{600, 2800}, 40 + OBJECT_MARGIN};
	OBJECTS_circles.circles[MODULE_YELLOW_S] = (GEOMETRY_circle_t){{1850, 2200}, 40 + OBJECT_MARGIN};

	OBJECTS_circles.circles[BIG_CRATER_BS] = (GEOMETRY_circle_t){{2000, 0}, 510};
	OBJECTS_circles.circles[BIG_CRATER_YS] = (GEOMETRY_circle_t){{2000, 3000}, 510};

	OBJECTS_circles.circles[SMALL_CRATER_NORTH_BS] = (GEOMETRY_circle_t){{540, 650}, 90};
	OBJECTS_circles.circles[SMALL_CRATER_SOUTH_BS] = (GEOMETRY_circle_t){{1870, 1065}, 90};

	OBJECTS_circles.circles[SMALL_CRATER_NORTH_YS] = (GEOMETRY_circle_t){{540, 2350}, 90};
	OBJECTS_circles.circles[SMALL_CRATER_SOUTH_YS] = (GEOMETRY_circle_t){{1870, 1935}, 90};

	// Définition des polygones

	OBJECTS_polygons.polygons[DEPOSE_BLUE].nb_summits = 4;
	OBJECTS_polygons.polygons[DEPOSE_BLUE].summits[0] = (GEOMETRY_point_t){700, 0};
	OBJECTS_polygons.polygons[DEPOSE_BLUE].summits[1] = (GEOMETRY_point_t){1150, 0};
	OBJECTS_polygons.polygons[DEPOSE_BLUE].summits[2] = (GEOMETRY_point_t){1150, 80 + OBJECT_MARGIN};
	OBJECTS_polygons.polygons[DEPOSE_BLUE].summits[3] = (GEOMETRY_point_t){700, 80 + OBJECT_MARGIN};

	OBJECTS_polygons.polygons[DEPOSE_YELLOW].nb_summits = 4;
	OBJECTS_polygons.polygons[DEPOSE_YELLOW].summits[0] = (GEOMETRY_point_t){700, 3000};
	OBJECTS_polygons.polygons[DEPOSE_YELLOW].summits[1] = (GEOMETRY_point_t){1150, 3000};
	OBJECTS_polygons.polygons[DEPOSE_YELLOW].summits[2] = (GEOMETRY_point_t){1150, 2920 - OBJECT_MARGIN};
	OBJECTS_polygons.polygons[DEPOSE_YELLOW].summits[3] = (GEOMETRY_point_t){700, 2920 - OBJECT_MARGIN};

	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].nb_summits = 4;
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].summits[0] = (GEOMETRY_point_t){1375, 984};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].summits[1] = (GEOMETRY_point_t){1462, 895};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].summits[2] = (GEOMETRY_point_t){1894, 1323};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].summits[3] = (GEOMETRY_point_t){1823, 1394};

	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_M].nb_summits = 4;
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_M].summits[0] = (GEOMETRY_point_t){1190, 1450};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_M].summits[1] = (GEOMETRY_point_t){1800, 1450};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_M].summits[2] = (GEOMETRY_point_t){1800, 1550};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_M].summits[3] = (GEOMETRY_point_t){1190, 1550};

	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].nb_summits = 4;
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].summits[0] = (GEOMETRY_point_t){1375, 2016};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].summits[1] = (GEOMETRY_point_t){1462, 2105};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].summits[2] = (GEOMETRY_point_t){1894, 1677};
	OBJECTS_polygons.polygons[DEPOSE_MIDDLE_BS].summits[3] = (GEOMETRY_point_t){1823, 1606};

	for(i=0; i<NB_CIRCLES; i++){
		OBJECTS_circles.state[i] = OBJECT_NO_DATA;
	}

	for(i=0; i<NB_POLYGONS; i++){
		OBJECTS_polygons.state[i] = OBJECT_NO_DATA;
	}


}







