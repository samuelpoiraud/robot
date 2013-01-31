/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Geometry.c
 *	Package : Carte Principale
 *	Description : 	Fonctions génériques de géométrie
 *	Auteur : Cyril
 *	Version 20090426
 */

#include "Geometry.h"

#include <math.h>

Uint16 GEOMETRY_distance(GEOMETRY_point_t a, GEOMETRY_point_t b)
{
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	return sqrt(dx*dx+dy*dy);
}

Uint32 GEOMETRY_squared_distance(GEOMETRY_point_t a, GEOMETRY_point_t b)
{
	Sint32 dx = a.x - b.x;
	Sint32 dy = a.y - b.y;
	return dx*dx+dy*dy;
}

GEOMETRY_point_t GEOMETRY_segment_middle(GEOMETRY_segment_t s)
{
	return (GEOMETRY_point_t){
		x : (s.a.x + s.b.x) / 2,
		y : (s.a.y + s.b.y) / 2
	};
}

bool_e GEOMETRY_segments_intersects(
		GEOMETRY_segment_t s1,
		GEOMETRY_segment_t s2)
{
	/* On calcule les vecteur orthogonaux à s1 et s2 */

	GEOMETRY_vector_t u1 = {
		-(s1.b.y - s1.a.y),
		 (s1.b.x - s1.a.x)
	};

	GEOMETRY_vector_t u2 = {
		-(s2.b.y - s2.a.y),
		 (s2.b.x - s2.a.x)
	};

	/* on projète s1 et s2 sur u1 */

	Sint32 proj_s1_a = ((Sint32)s1.a.x) * ((Sint32)u1.x) + ((Sint32)s1.a.y) * ((Sint32)u1.y);
	Sint32 proj_s1_b = ((Sint32)s1.b.x) * ((Sint32)u1.x) + ((Sint32)s1.b.y) * ((Sint32)u1.y);
	Sint32 proj_s2_a = ((Sint32)s2.a.x) * ((Sint32)u1.x) + ((Sint32)s2.a.y) * ((Sint32)u1.y);
	Sint32 proj_s2_b = ((Sint32)s2.b.x) * ((Sint32)u1.x) + ((Sint32)s2.b.y) * ((Sint32)u1.y);

	/*
	 * Si les projetés ne se chevauchent pas, les segments ne
	 * se croisent pas
	 */

	if ( MIN(proj_s1_a,proj_s1_b) > MAX(proj_s2_a,proj_s2_b) ||
		 MAX(proj_s1_a,proj_s1_b) < MIN(proj_s2_a,proj_s2_b)
	) {
		return FALSE;
	}

	/* sinon on projète s1 et s2 sur u2 et on recommence*/

	proj_s1_a = ((Sint32)s1.a.x) * ((Sint32)u2.x) + ((Sint32)s1.a.y) * ((Sint32)u2.y);
	proj_s1_b = ((Sint32)s1.b.x) * ((Sint32)u2.x) + ((Sint32)s1.b.y) * ((Sint32)u2.y);
	proj_s2_a = ((Sint32)s2.a.x) * ((Sint32)u2.x) + ((Sint32)s2.a.y) * ((Sint32)u2.y);
	proj_s2_b = ((Sint32)s2.b.x) * ((Sint32)u2.x) + ((Sint32)s2.b.y) * ((Sint32)u2.y);

	/*
	 * Si les projetés ne se chevauchent pas, les segments ne
	 * se croisent pas
	 */

	if ( MIN(proj_s1_a,proj_s1_b) > MAX(proj_s2_a,proj_s2_b) ||
		 MAX(proj_s1_a,proj_s1_b) < MIN(proj_s2_a,proj_s2_b)
	) {
		return FALSE;
	}

	return TRUE;
}

GEOMETRY_circle_t GEOMETRY_circle_from_diameter(GEOMETRY_segment_t diameter)
{
	GEOMETRY_circle_t c;
	c.c = GEOMETRY_segment_middle(diameter);
	c.r = GEOMETRY_distance(c.c, diameter.a);
	return c;
}

/*
 * Equations trouvées sur
 * Explications sur : http://2000clicks.com/MathHelp/GeometryConicSectionCircleIntersection.aspx
 * Renvoie une segment avec les deux coordonées valant 0,0 si les cercles ne se croisent pas 
 * ou sont coradiaux.
 */
GEOMETRY_segment_t GEOMETRY_circle_intersections(GEOMETRY_circle_t c1, GEOMETRY_circle_t c2)
{
	GEOMETRY_segment_t points;
	double x1 = c1.c.x;
	double y1 = c1.c.y;
	double r1 = c1.r;
	double x2 = c2.c.x;
	double y2 = c2.c.y;
	double r2 = c2.r;
	
	double x_diff = x2-x1;
	double y_diff = y2-y1;
	double sq_r1 = r1*r1;
	double sq_r2 = r2*r2;
	
    double radius_sum_square = (r1+r2)*(r1+r2); /* somme des rayons au carré */
    double radius_diff_square = (r1-r2)*(r1-r2); /* différence des rayons au carré */
	double d2 = x_diff*x_diff+y_diff*y_diff; /* d est la distance entre les centres des cercles */
	double k; /* k vaut deux fois l'aire du triangle formé par les centres des deux cercles et un point d'intersection */
	double x_midpoint; /* Coordonnée en x du milieu des deux points d'intersection des cercles */
    double y_midpoint; /* Coordonnée en y du milieu des deux points d'intersection des cercles */
    double x_ratio_of_change;
    double y_ratio_of_change;

    /*On s'assure que la distance entre les centres des cercles se situe entre la différence de leurs rayons et la somme de leurs rayons (inclus) */
    if(d2>radius_diff_square && d2<=radius_sum_square && d2!=0)
    {
        k = sqrt((radius_sum_square-d2)*(d2-radius_diff_square))/2.;
        x_midpoint = (x2+x1)/2. + (x_diff*(sq_r1-sq_r2))/(2.*d2);
        y_midpoint = (y2+y1)/2. + (y_diff*(sq_r1-sq_r2))/(2.*d2);
        x_ratio_of_change = y_diff*k/d2;
        y_ratio_of_change = x_diff*k/d2;
        points.a.x = x_midpoint + x_ratio_of_change;
        points.a.y = y_midpoint - y_ratio_of_change;
        points.b.x = x_midpoint - x_ratio_of_change;
        points.b.y = y_midpoint + y_ratio_of_change;
    }
    else
    {
        points.a.x = 0;
        points.a.y = 0;
        points.b.x = 0;
        points.b.y = 0;
    }

    return points;
}
