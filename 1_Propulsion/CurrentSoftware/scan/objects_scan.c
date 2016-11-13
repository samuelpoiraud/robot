
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
extern OBJECTS_circles_t OBJECTS_circles;	// Variable externe définit dans QS_objects.c
OBJECTS_SCAN_state_e circles_state[NB_MAX_CIRCLES][MIN_POINTS_PER_OBJECT];

extern OBJECTS_polygons_t OBJECTS_polygons;	// Variable externe définit dans QS_objects.c
OBJECTS_SCAN_state_e polygons_state[NB_MAX_POLYGONS][MIN_POINTS_PER_OBJECT];

static Uint8 index = 0;

void OBJECTS_SCAN_init(){
	OBJECTS_init();
}


void OBJECTS_SCAN_treatment(scan_data_t tab[]){
	assert(OBJECTS_polygons.nb_polygons <= NB_MAX_POLYGONS);
	assert(OBJECTS_circles.nb_circles <= NB_MAX_CIRCLES);
	Uint16 i, j, k;
	Uint8 nb_intersections = 0;
	bool_e flagIn = FALSE;
	bool_e object_present, object_absent;


	for(i=0; i<NB_SCAN_DATA; i++){
		// Polygones
		for(j=0; j<OBJECTS_polygons.nb_polygons; j++){
			flagIn = is_in_polygon(OBJECTS_polygons.polygons[j].summits, OBJECTS_polygons.polygons[j].nb_summits, tab[i].pos_mesure, tab[i].pos_laser, &nb_intersections);
			if(flagIn){
				polygons_state[j][index] = OBJECT_PRESENT;
			}else if(nb_intersections){ // Si il y a eu des intersections
				polygons_state[j][index] = OBJECT_ABSENT;
			}else{
				polygons_state[j][index] = OBJECT_NO_DATA;
			}
		}

		for(j=0; j<OBJECTS_circles.nb_circles; j++){
			if(OBJECTS_SCAN_check_presence_circle(OBJECTS_circles.circles[j], tab[i].pos_mesure)){
				circles_state[j][index] = OBJECT_PRESENT;
			}else if(OBJECTS_SCAN_check_absence_circle(OBJECTS_circles.circles[j], (GEOMETRY_segment_t){tab[i].pos_mesure, tab[i].pos_laser})){
				circles_state[j][index] = OBJECT_ABSENT;
			}else{
				circles_state[j][index] = OBJECT_NO_DATA;
			}
		}

		index = (index + 1)%MIN_POINTS_PER_OBJECT; // On incrémente l'index

		object_present = TRUE;
		object_absent = TRUE;
		for(j=0; j<OBJECTS_polygons.nb_polygons; j++){
			for(k=0; k<MIN_POINTS_PER_OBJECT; k++){
				if(polygons_state[j][k] != OBJECT_PRESENT){
						object_present = FALSE;
				}
				if(polygons_state[j][k] != OBJECT_ABSENT){
						object_absent = FALSE;
				}
			}
			if(object_present)
				debug_printf("L'objet polygon %d a été détecté présent\n", j);
			if(object_absent)
				debug_printf("L'objet polygon %d a été détecté absent\n", j);
		}

		object_present = TRUE;
		object_absent = TRUE;
		for(j=0; j<OBJECTS_circles.nb_circles; j++){
			for(k=0; k<MIN_POINTS_PER_OBJECT; k++){
				if(circles_state[j][k] != OBJECT_PRESENT){
						object_present = FALSE;
				}
				if(circles_state[j][k] != OBJECT_ABSENT){
						object_absent = FALSE;
				}
			}
			if(object_present)
				debug_printf("L'objet circle %d a été détecté présent\n", j);
			if(object_absent)
				debug_printf("L'objet circle %d a été détecté absent\n", j);
		}

	}
}



static bool_e OBJECTS_SCAN_check_absence_circle(GEOMETRY_circle_t circle, GEOMETRY_segment_t seg){
	double a1, b1, a2, b2;
	GEOMETRY_point_t p;
	bool_e onSegment = FALSE, returnValue = FALSE;
	Uint16 dist;

	// Calcul du projeté du centre du cercle sur le segment
	if(seg.a.x != seg.b.x && seg.a.y != seg.b.y){
		// Calcul de l'équation de la droite du segment
		a1 = (seg.a.y - seg.b.y)/(seg.a.x - seg.b.x);
		b1 = seg.a.y - a1 * seg.a.x;
		// Calcul de l'équation de la droite orthogonale passant par le centre du cercle
		a2 = -1/a1;
		b2 = circle.c.y - a2 * circle.c.x;

		p.x = (b2 - b1)/(a1 - a2);
		p.y = a1 * p.x + b1;
	}else if(seg.a.x != seg.b.x){ // seg.a.y == seg.b.y Gestion du cas particulier où l'équation de la droite du segment est du type y = c
		p.x = circle.c.x;
		p.y = seg.a.y;
	}else{  // seg.a.x == seg.b.x Gestion du cas particulier où l'équation de la droite du segment est du type y = c
		p.x = seg.a.x;
		p.y = circle.c.y;
	}

	// On vérifie que le projeté est bien sur le segment
	if(p.x > MIN(seg.a.x, seg.b.x) && p.x < MAX(seg.a.x, seg.b.x) && p.y > MIN(seg.a.y, seg.b.y) && p.y < MAX(seg.a.y, seg.b.y)){
		onSegment = TRUE;
	}else{
		onSegment = FALSE;
	}

	if(onSegment){
		dist = GEOMETRY_distance(p, circle.c); // Calcul de la distance

		// On regarde si la distance du projeté avec le centre du cercle est inférieure au rayon
		if(dist < circle.r){
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
