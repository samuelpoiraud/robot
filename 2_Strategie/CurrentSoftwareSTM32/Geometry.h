/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Geometry.h
 *	Package : Carte Principale
 *	Description : 	Fonctions génériques de géométrie
 *	Auteur : Cyril
 *	Version 20090426
 */

#include "QS/QS_all.h"

#ifndef GEOMETRY_H
	#define GEOMETRY_H

	typedef struct {
		Sint16 x;
		Sint16 y;
	} GEOMETRY_point_t;

	typedef struct {
		Sint16 x;
		Sint16 y;
	} GEOMETRY_vector_t;

	typedef struct {
		GEOMETRY_point_t a;
		GEOMETRY_point_t b;
	} GEOMETRY_segment_t;

	typedef struct {
		GEOMETRY_point_t c;
		Uint16 r;
	} GEOMETRY_circle_t;

	Uint16 GEOMETRY_distance(GEOMETRY_point_t a, GEOMETRY_point_t b);

	Uint32 GEOMETRY_squared_distance(GEOMETRY_point_t a, GEOMETRY_point_t b);

	bool_e GEOMETRY_segments_intersects(GEOMETRY_segment_t s1,GEOMETRY_segment_t s2);

	GEOMETRY_point_t GEOMETRY_segment_middle(
		GEOMETRY_segment_t s);

	GEOMETRY_circle_t GEOMETRY_circle_from_diameter(
		GEOMETRY_segment_t diameter);

	GEOMETRY_segment_t GEOMETRY_circle_intersections(
		GEOMETRY_circle_t c0,
		GEOMETRY_circle_t c1);

#endif /* ndef GEOMETRY_H */
