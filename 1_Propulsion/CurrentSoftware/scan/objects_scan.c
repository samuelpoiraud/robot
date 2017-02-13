
#include "objects_scan.h"
#include "scan.h"
#include "../QS/QS_objects.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_types.h"


#define MIN_POINTS_PER_OBJECT (3)

// Fonctions privées
static bool_e OBJECTS_SCAN_check_absence_circle(GEOMETRY_circle_t circle, GEOMETRY_segment_t seg);
static bool_e OBJECTS_SCAN_check_presence_circle(GEOMETRY_circle_t circle, GEOMETRY_point_t p);

// Variables globales
extern OBJECTS_circles_t OBJECTS_circles[OBJ_NB_CIRCLES];	// Variable externe définit dans QS_objects.c
OBJECTS_state_e circles_state[NB_MAX_CIRCLES][MIN_POINTS_PER_OBJECT];

extern OBJECTS_polygons_t OBJECTS_polygons[OBJ_NB_POLYGONS];	// Variable externe définit dans QS_objects.c
OBJECTS_state_e polygons_state[NB_MAX_POLYGONS][MIN_POINTS_PER_OBJECT];

static Uint8 index = 0;

void OBJECTS_SCAN_init(){
	Uint8 i;

	OBJECTS_init();
	for(i=0; i<NB_MAX_CIRCLES; i++){
		OBJECTS_circles[i].state = OBJECT_NO_DATA;
	}
	for(i=0; i<NB_MAX_POLYGONS; i++){
		OBJECTS_polygons[i].state = OBJECT_NO_DATA;
	}
}


void OBJECTS_SCAN_treatment(scan_data_t tab[]){
	Uint16 i, j, k;
	Uint8 nb_intersections = 0;
	bool_e flagIn = FALSE;
	bool_e object_present, object_absent;


	for(i=0; i<NB_SCAN_DATA; i++){
		// Polygones
		for(j=0; j<OBJ_NB_POLYGONS; j++){
			flagIn = is_in_polygon(OBJECTS_polygons[j].presence.summits, OBJECTS_polygons[j].presence.nb_summits, tab[i].pos_mesure, tab[i].pos_laser, NULL);
			is_in_polygon(OBJECTS_polygons[j].absence.summits, OBJECTS_polygons[j].absence.nb_summits, tab[i].pos_mesure, tab[i].pos_laser, &nb_intersections);
			if(flagIn){
				polygons_state[j][index] = OBJECT_PRESENT;
			}else if(nb_intersections){ // Si il y a eu des intersections
				polygons_state[j][index] = OBJECT_ABSENT;
			}else{
				polygons_state[j][index] = OBJECT_NO_DATA;
			}
		}

		for(j=0; j<OBJ_NB_CIRCLES; j++){
			if(OBJECTS_SCAN_check_presence_circle(OBJECTS_circles[j].presence, tab[i].pos_mesure)){
				circles_state[j][index] = OBJECT_PRESENT;
			}else if(OBJECTS_SCAN_check_absence_circle(OBJECTS_circles[j].absence, (GEOMETRY_segment_t){tab[i].pos_mesure, tab[i].pos_laser})){
				circles_state[j][index] = OBJECT_ABSENT;
			}else{
				circles_state[j][index] = OBJECT_NO_DATA;
			}
		}

		index = (index + 1)%MIN_POINTS_PER_OBJECT; // On incrémente l'index

		object_present = TRUE;
		object_absent = TRUE;
		for(j=0; j<OBJ_NB_POLYGONS; j++){
			for(k=0; k<MIN_POINTS_PER_OBJECT; k++){
				if(polygons_state[j][k] != OBJECT_PRESENT){
						object_present = FALSE;
				}
				if(polygons_state[j][k] != OBJECT_ABSENT){
						object_absent = FALSE;
				}
			}
			if(object_present){
				OBJECTS_polygons[j].state = OBJECT_PRESENT;
            //	debug_printf("L'objet polygon %d a été détecté présent\n", j);
			}
			if(object_absent){
				OBJECTS_polygons[j].state = OBJECT_ABSENT;
            //	debug_printf("L'objet polygon %d a été détecté absent\n", j);
			}
		}

		object_present = TRUE;
		object_absent = TRUE;
		for(j=0; j<OBJ_NB_CIRCLES; j++){
			for(k=0; k<MIN_POINTS_PER_OBJECT; k++){
				if(circles_state[j][k] != OBJECT_PRESENT){
						object_present = FALSE;
				}
				if(circles_state[j][k] != OBJECT_ABSENT){
						object_absent = FALSE;
				}
			}
			if(object_present){
				OBJECTS_circles[j].state = OBJECT_PRESENT;
                //debug_printf("L'objet circle %d a été détecté présent\n", j);
			}
			if(object_absent){
				OBJECTS_circles[j].state = OBJECT_ABSENT;
                //debug_printf("L'objet circle %d a été détecté absent\n", j);
			}
		}

	}
}



static bool_e OBJECTS_SCAN_check_absence_circle(GEOMETRY_circle_t circle, GEOMETRY_segment_t seg){
	GEOMETRY_point_t p; // Le projeté du centre du cercle sur le segment
	Uint16 dist = 0;
	bool_e onSegment = FALSE, returnValue = FALSE;
	Sint16 angleSeg;

	// Calcul des angles
	if(seg.b.y - seg.a.y == 0 && seg.b.x - seg.a.x < 0) {
		angleSeg = - PI4096; // On gère le cas de la valeur interdite de atan2_4096
	}else{
		angleSeg = atan2_4096(seg.b.y - seg.a.y, seg.b.x - seg.a.x);
	}
	Sint16 angleCircle = atan2_4096(circle.c.y - seg.a.y, circle.c.x - seg.a.x);
	Sint16 angleDiff = GEOMETRY_modulo_angle(angleCircle - angleSeg);

	// Calcul de la distance
	dist = GEOMETRY_distance(seg.a, circle.c);

	p.x = seg.a.x + dist*cos4096(angleDiff)*cos4096(angleSeg);
	p.y = seg.a.y + dist*cos4096(angleDiff)*sin4096(angleSeg);

	// On vérifie que le projeté est bien sur le segment
	if(p.x >= MIN(seg.a.x, seg.b.x) && p.x <= MAX(seg.a.x, seg.b.x) && p.y >= MIN(seg.a.y, seg.b.y) && p.y <= MAX(seg.a.y, seg.b.y)){
		onSegment = TRUE;
	}else{
		onSegment = FALSE;
	}

	if(onSegment){
		dist = GEOMETRY_distance(p, circle.c); // Calcul de la distance

		// On regarde si la distance du projeté avec le centre du cercle est inférieure au rayon
		if(dist <= circle.r){
			returnValue = TRUE; // Si oui, l'objet est absent
		}else{
			returnValue = FALSE; // Si non, on ne peut pas savoir
		}
	}else{
		returnValue = FALSE;
	}
	return returnValue;
}


static bool_e OBJECTS_SCAN_check_presence_circle(GEOMETRY_circle_t circle, GEOMETRY_point_t p){
	bool_e returnValue = FALSE;
	Uint16 dist = GEOMETRY_distance(p, circle.c);

	if(dist < circle.r){
		returnValue = TRUE;
	}else{
		returnValue = FALSE;
	}

	return returnValue;
}
