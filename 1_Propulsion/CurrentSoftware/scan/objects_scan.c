
#include "objects_scan.h"
#include "scan.h"
#include "../QS/QS_objects.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_types.h"


#define MIN_POINTS_PER_OBJECT (3)

typedef enum{
	OBJECTS_NO_DATA,
	OBJECTS_PRESENT,
	OBJECTS_ABSENT
}OBJECTS_SCAN_state_e;

extern OBJECTS_circles_t OBJECTS_circles;	// Variable externe définit dans QS_objects.c
OBJECTS_SCAN_state_e circles_state[NB_MAX_CIRCLES][MIN_POINTS_PER_OBJECT];

extern OBJECTS_polygons_t OBJECTS_polygons;	// Variable externe définit dans QS_objects.c
OBJECTS_SCAN_state_e polygons_state[NB_MAX_POLYGONS][MIN_POINTS_PER_OBJECT];

void OBJECTS_SCAN_init(){
	OBJECTS_init();
}


void OBJECTS_SCAN_treatment(scan_data_t tab[]){

}



bool_e OBJECTS_SCAN_circle_intersect(GEOMETRY_circle_t circle, GEOMETRY_segment_t seg){
	GEOMETRY_point_t proj;
	Uint16 distance_to_laser = 0;
	bool_e return_value = FALSE;

	// Calcul de l'angle entre le segment seg et le segment liant une des extremité de seg et le centre du cercle
	Sint16 angleWithCircleCenter = atan2_4096(circle.c.y - seg.a.y, circle.c.x - seg.a.x);
	Sint16 angleSeg = atan2_4096(seg.b.y- seg.a.y, seg.b.x - seg.a.x);
	Sint16 angle = GEOMETRY_modulo_angle(angleWithCircleCenter - angleSeg);

	// Calcul de la distance
	Uint16 dist = GEOMETRY_distance(seg.a, circle.c);

	// Calcul du projeté
	proj.x = seg.a.x + dist*cos4096(angle);
	proj.y = seg.a.y + dist*cos4096(angle);

	// Si le projeté est bien sur le segment et qu'il se situe à une distance inférieur au rayon du centre, alors il y a intersection
	if(proj.x > MIN(seg.a.x, seg.b.x) && proj.x < MAX(seg.a.x, seg.b.x)
			&& proj.y > MIN(seg.a.y, seg.b.y) && proj.y < MAX(seg.a.y, seg.b.y)){
		distance_to_laser = GEOMETRY_distance(proj, circle.c);
		if(distance_to_laser < circle.r){
			return_value = TRUE;
		}
	}
	return return_value;
}













/*
void ELEMENTS_SCAN_treatment(scan_data_treatment_t tab[]){
	Uint16 i = 0;
	GEOMETRY_point_t point_ref;
	GEOMETRY_circle_t circle_ref;

	// Variables pour le calcul du barycentre
	Uint32 sumX, sumY;
	Uint16 nb_points;
	GEOMETRY_point_t barycentre;

	while( i < NB_SCAN_DATA ){

		sumX = 0;
		sumY = 0;
		nb_points = 0;

		//### Début d'un nouvel objet ###
		// On cherche un point valide
		while(!tab[i].enable && i < NB_SCAN_DATA ){
			i++;
		}
		// On a trouvé un point valide
		if( i < NB_SCAN_DATA){
			point_ref = tab[i].pos;
			circle_ref = (GEOMETRY_circle_t){point_ref, MAX_SIZE_ELEMENT};
			// On stocke les coordonnées pour le calcul du barycentre
			sumX = point_ref.x;
			sumY = point_ref.y;
			nb_points++;
			i++;
		}

		//### C'est toujours le même objet ###
		// Si le point est valide et proche du point de départ
		while(i < NB_SCAN_DATA && tab[i].enable && is_in_circle(tab[i].pos, circle_ref)){
			// On stocke les coordonnées pour le calcul du barycentre
			sumX = tab[i].pos.x;
			sumY = tab[i].pos.y;
			nb_points++;
			i++;
		}

		//### C'est la fin de l'objet ###
		if(nb_points < MIN_POINTS_PER_ELEMENT){
			// Calcul du barycentre
			barycentre.x = sumX/nb_points;
			barycentre.y = sumY/nb_points;
		}
	}
}

void ELEMENTS_SCAN_check_presence(GEOMETRY_point_t barycentre){
	Uint8 i;
	GEOMETRY_point_t out_point = (GEOMETRY_point_t){3000, 4000}; // Ce point est en dehors du terrain c'est normal

	for(i=0; i<OBJECTS_circles.nb_circles; i++){
		if(is_in_circle(barycentre, OBJECTS_circles.circles[i])){
			OBJECTS_circles.enable[i] = TRUE;
		}
	}

	for(i=0; i<OBJECTS_polygons.nb_polygons; i++){
		if(is_in_polygon(OBJECTS_polygons.polygons[i].summits, OBJECTS_polygons.polygons[i].nb_summits, barycentre, out_point, NULL)){
			OBJECTS_polygons.enable[i] = TRUE;
		}
	}
}*/
