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
#include "cos_sin.h"
/* calcul de la distance que l'on a à parcourir entre la position actuelle et la position finale ( en mm ) OK 
et calcul de l'angle */ 
Sint32 CALCULATOR_viewing_algebric_distance(Sint16 start_x, Sint16 start_y, Sint16 destination_x, Sint16 destination_y, Sint16 angle_de_vue)
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

//retourne l'angle de la destination...[rad/4096]
Sint16 CALCULATOR_viewing_angle(Sint16 start_x, Sint16 start_y, Sint16 destination_x, Sint16 destination_y)
{
	Sint16 deltaX,deltaY;
	deltaX= destination_x - start_x;
	deltaY= destination_y - start_y;
	return (float)atan2(deltaY,deltaX)*4096;
	return (Sint16)((float)atan2(destination_y - start_y , destination_x - start_x)*4096);
}	

Sint16 CALCULATOR_modulo_angle(Sint16 angle)
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
