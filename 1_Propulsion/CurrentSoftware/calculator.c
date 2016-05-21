 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : calculator.c
 *  Package : Propulsion
 *  Description : prototypes Fonctions de calculs servant à la gestion de trajectoire
 *  Auteur : Val' && Thomas MOESSE alias Ketchom && Arnaud AUMONT alias Koubi && Nirgal 2009
 *  Version 201203
 */

#include <math.h>

#include "calculator.h"
#include "QS/QS_maths.h"

/* calcul de la distance que l'on a à parcourir entre la position actuelle et la position finale ( en mm ) OK
et calcul de l'angle */
Sint32 CALCULATOR_viewing_algebric_distance(Sint32 start_x, Sint32 start_y, Sint32 destination_x, Sint32 destination_y, Sint32 angle_de_vue)
{
	Sint32 deltaX,deltaY;
	deltaX= absolute(destination_x - start_x);
	deltaY= absolute(destination_y - start_y);
	//debug_printf("deltaX %ld, deltaY %ld sqrt %ld", deltaX, deltaY, (Sint32)sqrt( deltaX*deltaX + deltaY*deltaY ));

	if(absolute(angle_de_vue) > PI4096/2)
		return -(Sint32)sqrt( deltaX*deltaX + deltaY*deltaY );
	else
		return (Sint32)sqrt( deltaX*deltaX + deltaY*deltaY );
	// en mm
}

/* calcul de la distance que l'on a à parcourir entre la position actuelle et la position finale ( en mm.4096 ) OK
et calcul de l'angle, valeur d'entrée en [mm.16] et [rad.4096]*/
Sint32 CALCULATOR_viewing_algebric_distance_mm16(Sint32 start_x, Sint32 start_y, Sint32 destination_x, Sint32 destination_y, Sint32 angle_de_vue)
{
	Sint32 deltaX,deltaY;
	deltaX= absolute(destination_x - start_x);
	deltaY= absolute(destination_y - start_y);

	if(absolute(angle_de_vue) > PI4096/2)
		return -(float)sqrt( deltaX*deltaX + deltaY*deltaY )*256;
	else
		return (float)sqrt( deltaX*deltaX + deltaY*deltaY )*256;
	// en mm
}

//retourne l'angle de la destination...[rad.4096]
Sint16 CALCULATOR_viewing_angle(Sint32 start_x, Sint32 start_y, Sint32 destination_x, Sint32 destination_y)
{
	Sint32 deltaX,deltaY;
	deltaX= destination_x - start_x;
	deltaY= destination_y - start_y;
	if(deltaX || deltaY)
		return (float)atan2(deltaY,deltaX)*4096;
	else
		return 0;
}

//retourne l'angle de la destination...[rad.4096.1024]
Sint32 CALCULATOR_viewing_angle_22(Sint32 start_x, Sint32 start_y, Sint32 destination_x, Sint32 destination_y)
{
	Sint32 deltaX,deltaY;
	deltaX= destination_x - start_x;
	deltaY= destination_y - start_y;
	if(deltaX || deltaY)
		return (float)atan2(deltaY,deltaX)*4096*1024;
	else
		return 0;
}

Sint16	CALCULATOR_modulo_angle(Sint16 angle)
{
	while(angle > PI4096)
			angle -= 2*PI4096;
	while(angle < -PI4096)
			angle += 2*PI4096;

	return angle;
}

Sint32 CALCULATOR_modulo_angle_22(Sint32 angle)	//rad/4096/1024
{
	if(angle > TWO_PI22/2)
		angle -= TWO_PI22;
	if(angle < -TWO_PI22/2)
		angle += TWO_PI22;

	return angle;
}

Uint16 CALCULATOR_distance(Sint16 xfrom, Sint16 yfrom, Sint16 xto, Sint16 yto){
	Sint32 deltaX,deltaY;
	deltaX= xto - xfrom;
	deltaY= yto - yfrom;
	return sqrt(deltaX*deltaX + deltaY*deltaY);
}

Uint16 CALCULATOR_manhattan_distance(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
	return (x1>x2 ? x1-x2 : x2-x1) + (y1>y2 ? y1-y2 : y2-y1);
}


