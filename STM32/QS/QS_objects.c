
#include "QS_objects.h"


OBJECTS_circles_t OBJECTS_circles[OBJ_NB_CIRCLES];
OBJECTS_polygons_t OBJECTS_polygons[OBJ_NB_POLYGONS];


void OBJECTS_init(){

	Uint8 i;

	// Définition des cercles

	OBJECTS_circles[OBJ_ROCKET_BLUE].presence = (GEOMETRY_circle_t){{40 + OBJECT_MARGIN, 1150}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_ROCKET_BLUE].absence = (GEOMETRY_circle_t){{40 + OBJECT_MARGIN, 1150}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_ROCKET_YELLOW].presence = (GEOMETRY_circle_t){{40 + OBJECT_MARGIN, 1850}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_ROCKET_YELLOW].absence = (GEOMETRY_circle_t){{40 + OBJECT_MARGIN, 1850}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_ROCKET_POLYC_BS].presence = (GEOMETRY_circle_t){{1350, 40 + OBJECT_MARGIN}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_ROCKET_POLYC_BS].absence = (GEOMETRY_circle_t){{1350, 40 + OBJECT_MARGIN}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_ROCKET_POLYC_YS].presence = (GEOMETRY_circle_t){{1350, 2960 - OBJECT_MARGIN}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_ROCKET_POLYC_YS].absence = (GEOMETRY_circle_t){{1350, 2960 - OBJECT_MARGIN}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_POLYC1_BS].presence = (GEOMETRY_circle_t){{600, 1000}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_POLYC1_BS].absence = (GEOMETRY_circle_t){{600, 1000}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_POLYC2_BS].presence = (GEOMETRY_circle_t){{1100, 500}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_POLYC2_BS].absence = (GEOMETRY_circle_t){{1100, 500}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_POLYC3_BS].presence = (GEOMETRY_circle_t){{1400, 900}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_POLYC3_BS].absence = (GEOMETRY_circle_t){{1400, 900}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_POLYC1_YS].presence = (GEOMETRY_circle_t){{600, 2000}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_POLYC1_YS].absence = (GEOMETRY_circle_t){{600, 2000}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_POLYC2_YS].presence = (GEOMETRY_circle_t){{1100, 2500}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_POLYC2_YS].absence = (GEOMETRY_circle_t){{1100, 2500}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_POLYC3_YS].presence = (GEOMETRY_circle_t){{1400, 2100}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_POLYC3_YS].absence = (GEOMETRY_circle_t){{1400, 2100}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_BLUE_N].presence = (GEOMETRY_circle_t){{600, 200}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_BLUE_N].absence = (GEOMETRY_circle_t){{600, 200}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_BLUE_S].presence = (GEOMETRY_circle_t){{1850, 800}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_BLUE_S].absence = (GEOMETRY_circle_t){{1850, 800}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_BLUE_S].presence = (GEOMETRY_circle_t){{1850, 800}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_BLUE_S].absence = (GEOMETRY_circle_t){{1850, 800}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_MODULE_YELLOW_N].presence = (GEOMETRY_circle_t){{600, 2800}, 40 + OBJECT_MARGIN};
	OBJECTS_circles[OBJ_MODULE_YELLOW_S].presence = (GEOMETRY_circle_t){{1850, 2200}, 40 - OBJECT_MARGIN};

	OBJECTS_circles[OBJ_BIG_CRATER_BS].presence = (GEOMETRY_circle_t){{2000, 0}, 510};
	OBJECTS_circles[OBJ_BIG_CRATER_BS].absence = (GEOMETRY_circle_t){{2000, 0}, 510};

	OBJECTS_circles[OBJ_BIG_CRATER_YS].presence = (GEOMETRY_circle_t){{2000, 3000}, 510};
	OBJECTS_circles[OBJ_BIG_CRATER_YS].absence = (GEOMETRY_circle_t){{2000, 3000}, 510};

	OBJECTS_circles[OBJ_SMALL_CRATER_NORTH_BS].presence = (GEOMETRY_circle_t){{540, 650}, 90};
	OBJECTS_circles[OBJ_SMALL_CRATER_NORTH_BS].absence = (GEOMETRY_circle_t){{540, 650}, 90};

	OBJECTS_circles[OBJ_SMALL_CRATER_SOUTH_BS].presence = (GEOMETRY_circle_t){{1870, 1065}, 90};
	OBJECTS_circles[OBJ_SMALL_CRATER_SOUTH_BS].absence = (GEOMETRY_circle_t){{1870, 1065}, 90};

	OBJECTS_circles[OBJ_SMALL_CRATER_NORTH_YS].presence = (GEOMETRY_circle_t){{540, 2350}, 90};
	OBJECTS_circles[OBJ_SMALL_CRATER_NORTH_YS].absence = (GEOMETRY_circle_t){{540, 2350}, 90};

	OBJECTS_circles[OBJ_SMALL_CRATER_SOUTH_YS].presence = (GEOMETRY_circle_t){{1870, 1935}, 90};
	OBJECTS_circles[OBJ_SMALL_CRATER_SOUTH_YS].absence = (GEOMETRY_circle_t){{1870, 1935}, 90};

	// Définition des polygones

	OBJECTS_polygons[OBJ_DEPOSE_BLUE].presence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].presence.summits[0] = (GEOMETRY_point_t){700, 0};
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].presence.summits[1] = (GEOMETRY_point_t){1150, 0};
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].presence.summits[2] = (GEOMETRY_point_t){1150, 80 + OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].presence.summits[3] = (GEOMETRY_point_t){700, 80 + OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].absence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].absence.summits[0] = (GEOMETRY_point_t){700 + OBJECT_MARGIN, OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].absence.summits[1] = (GEOMETRY_point_t){1150 - OBJECT_MARGIN, OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].absence.summits[2] = (GEOMETRY_point_t){1150 - OBJECT_MARGIN, 80 - OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_BLUE].absence.summits[3] = (GEOMETRY_point_t){700 + OBJECT_MARGIN, 80 - OBJECT_MARGIN};

	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].presence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].presence.summits[0] = (GEOMETRY_point_t){700, 3000};
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].presence.summits[1] = (GEOMETRY_point_t){1150, 3000};
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].presence.summits[2] = (GEOMETRY_point_t){1150, 2920 - OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].presence.summits[3] = (GEOMETRY_point_t){700, 2920 - OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].absence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].absence.summits[0] = (GEOMETRY_point_t){700 + OBJECT_MARGIN, 3000 - OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].absence.summits[1] = (GEOMETRY_point_t){1150 - OBJECT_MARGIN, 3000 - OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].absence.summits[2] = (GEOMETRY_point_t){1150 - OBJECT_MARGIN, 2920 + OBJECT_MARGIN};
	OBJECTS_polygons[OBJ_DEPOSE_YELLOW].absence.summits[3] = (GEOMETRY_point_t){700 + OBJECT_MARGIN, 2920 + OBJECT_MARGIN};

	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.summits[0] = (GEOMETRY_point_t){1375, 984};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.summits[1] = (GEOMETRY_point_t){1462, 895};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.summits[2] = (GEOMETRY_point_t){1894, 1323};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.summits[3] = (GEOMETRY_point_t){1823, 1394};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.summits[0] = (GEOMETRY_point_t){1375, 984};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.summits[1] = (GEOMETRY_point_t){1462, 895};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.summits[2] = (GEOMETRY_point_t){1894, 1323};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.summits[3] = (GEOMETRY_point_t){1823, 1394};

	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].presence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].presence.summits[0] = (GEOMETRY_point_t){1190, 1450};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].presence.summits[1] = (GEOMETRY_point_t){1800, 1450};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].presence.summits[2] = (GEOMETRY_point_t){1800, 1550};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].presence.summits[3] = (GEOMETRY_point_t){1190, 1550};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].absence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].absence.summits[0] = (GEOMETRY_point_t){1190, 1450};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].absence.summits[1] = (GEOMETRY_point_t){1800, 1450};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].absence.summits[2] = (GEOMETRY_point_t){1800, 1550};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_M].absence.summits[3] = (GEOMETRY_point_t){1190, 1550};

	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.summits[0] = (GEOMETRY_point_t){1375, 2016};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.summits[1] = (GEOMETRY_point_t){1462, 2105};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.summits[2] = (GEOMETRY_point_t){1894, 1677};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].presence.summits[3] = (GEOMETRY_point_t){1823, 1606};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.nb_summits = 4;
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.summits[0] = (GEOMETRY_point_t){1375, 2016};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.summits[1] = (GEOMETRY_point_t){1462, 2105};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.summits[2] = (GEOMETRY_point_t){1894, 1677};
	OBJECTS_polygons[OBJ_DEPOSE_MIDDLE_BS].absence.summits[3] = (GEOMETRY_point_t){1823, 1606};

	for(i=0; i<OBJ_NB_CIRCLES; i++){
		OBJECTS_circles[i].state = OBJECT_NO_DATA;
	}

	for(i=0; i<OBJ_NB_POLYGONS; i++){
		OBJECTS_polygons[i].state = OBJECT_NO_DATA;
	}


}







