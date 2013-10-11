/*
 *	Club Robot ESEO 2010 - 2011
 *	CkeckNorris
 *
 *	Fichier : sick.c
 *	Package : Carte Principale
 *	Description : Traitement des données des capteurs SICK (télémètres LASER DT50)
 *	Auteur : Ronan
 *	Version 20110304
 */

#define SICK_C

#include "sick.h"

#include "config_use.h"

#ifdef USE_SICK

distance_sensor_config_t const distance_sensor_config[NB_DISTANCE_SENSOR] =
{
	/*A modifier*/
	{ADC_PIN_DT50_LEFT,MIN_ADC,MAX_ADC,DT50_RESPONSE_TIME,X_DT50_LEFT,Y_DT50_LEFT,ANGLE_DT50_LEFT,BLACK_CD},
	{ADC_PIN_DT50_TOP,MIN_ADC,MAX_ADC,DT50_RESPONSE_TIME,X_DT50_TOP,Y_DT50_TOP,ANGLE_DT50_TOP,GOLD_BAR}
};

/* Définition des télémetres */		
static distance_sensor_t distance_sensor[NB_DISTANCE_SENSOR];
		
void SICK_init(void)
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
		
	ADC_init();
	
	/* Intitialisation du DT50_LEFT */
	distance_sensor[DT50_LEFT].config = &distance_sensor_config[DT50_LEFT];
	distance_sensor[DT50_LEFT].point_updated = FALSE;
	distance_sensor[DT50_LEFT].distance = 0;
	distance_sensor[DT50_LEFT].nb_points = 0;
	distance_sensor[DT50_LEFT].locked = FALSE;

	/* Intitialisation du DT50_RIGHT */
	distance_sensor[DT50_RIGHT].config = &distance_sensor_config[DT50_RIGHT];
	distance_sensor[DT50_RIGHT].point_updated = FALSE;
	distance_sensor[DT50_RIGHT].distance = 0;
	distance_sensor[DT50_RIGHT].nb_points = 0;
	distance_sensor[DT50_RIGHT].locked = FALSE;

	/* Intitialisation du DT50_TOP */
	distance_sensor[DT50_TOP].config = &distance_sensor_config[DT50_TOP];
	distance_sensor[DT50_TOP].point_updated = FALSE;
	distance_sensor[DT50_TOP].distance = 0;
	distance_sensor[DT50_TOP].nb_points = 0;
	distance_sensor[DT50_TOP].locked = FALSE;
}


void SICK_update(void)
{
    sick_distance_sensor_e i;
	for(i=0;i<NB_DISTANCE_SENSOR;i++)
	{
		#ifndef USE_SICK_DT50_LEFT
			if(i == DT50_LEFT) i++;
   		#endif /* def USE_SICK_DT50_LEFT */
   		
   		#ifndef USE_SICK_DT50_RIGHT
   			if(i == DT50_RIGHT) i++;
   		#endif /* def USE_SICK_DT50_RIGHT */
   		
   		#ifndef USE_SICK_DT50_TOP
   			if(i == DT50_TOP) break;
   		#endif /* def USE_SICK_DT50_TOP */
		/* Algo de tri, de sélection de point et de calcul des centres de pions */
	 //	debug_printf("%d,%d,%d\n",distance_sensor[i].point_updated,distance_sensor[i].point.x, distance_sensor[i].point.y);
		if(distance_sensor[i].point_updated && (distance_sensor[i].point.x != -1 || distance_sensor[i].point.y != -1))
		{	
			distance_sensor[i].locked = TRUE;
			sick_printf("SICK n°%d : distance : %d mm \r\n", i, distance_sensor[i].distance);
			sick_printf("X : %d mm / Y : %d mm\r\n", distance_sensor[i].point.x, distance_sensor[i].point.y);
			distance_sensor[i].point_updated = FALSE; 
	  	//	SICK_points_selection(i);
	  		distance_sensor[i].locked = FALSE;
	 	}
 	}		 			
}

void SICK_update_points()
{
//	static position_t robot_position, last_robot_position = {0,0,0,0,0,0,0,0}; // Peut servir pour faire une correction par la suite
//  static time32_t robot_time, last_robot_time = 0;
	sick_distance_sensor_e i;
	
	/* Sauvegarde de la position et des données temporelles du robot */
//	robot_position = global.env.pos;
//	robot_time = global.env.match_time;

	/* Sécurité qui regarde si le robot a bien bougé */
//	if(robot_position.x != last_robot_position.x || robot_position.y != last_robot_position.y
//     || robot_position.angle != last_robot_position.angle) 
//	{
		for(i=0;i<NB_DISTANCE_SENSOR;i++)
		{
			#ifndef USE_SICK_DT50_LEFT
				if(i == DT50_LEFT) i++;
	   		#endif /* def USE_SICK_DT50_LEFT */
	   		
	   		#ifndef USE_SICK_DT50_RIGHT
	   			if(i == DT50_RIGHT) i++;
	   		#endif /* def USE_SICK_DT50_RIGHT */
	   		
	   		#ifndef USE_SICK_DT50_TOP
	   			if(i == DT50_TOP) break;
	   		#endif /* def USE_SICK_DT50_TOP */
			/* Vérification du non dépassement du temps de réponse du capteur pour correction de la valeur de distance si besoin */
//			if(robot_time-last_robot_time > distance_sensor[i].response_time)
//			{	
				/* On regarde si le télémetre n'est pas verrouillé */ 
				if(!distance_sensor[i].locked)
				{
			   		distance_sensor[i].point = SICK_get_point(i);
			   		distance_sensor[i].point_updated = TRUE;
			 	}  	
//			}
//		}	
	}
	
	/* Sauvegarde de la position et des données temporelles du robot */
//  	last_robot_position = robot_position;
//  last_robot_time = robot_time;
}

/* Accesseur sur la distance du capteur passé en parametre */
Uint16 SICK_get_distance(sick_distance_sensor_e sensor)
{
	return 	distance_sensor[sensor].distance;
}

//accesseur qui retourn le point scané par le capteur passé en paramètre
GEOMETRY_point_t SICK_get_point_position(sick_distance_sensor_e sensor)
{
	return distance_sensor[sensor].point;
}

	
static GEOMETRY_point_t SICK_get_point(sick_distance_sensor_e sick_sensor)
{
	distance_sensor_t* sensor = &(distance_sensor[sick_sensor]);
	position_t* robot_position = &global.env.pos; /* Pointeur de travail */
	GEOMETRY_point_t point = {-1,-1};
	Sint16 adc_value = ADC_getValue(sensor->config->pinADC);	
	
	if(adc_value > sensor->config->min_adc_value && adc_value < sensor->config->max_adc_value)
	{
		/* Mise à jour de la distance via le capteur sick (Télémètre LASER DT50) */
		sensor->distance = ((Uint32)(212612UL * adc_value) >> 16) - 399; // paramétrable (4mA -> 200mm / 20mA -> 2500mm)
	    
	    if(sick_sensor == DT50_LEFT || sick_sensor == DT50_TOP)
	    {
	    	/* Calcul de la position du point visé par les capteurs dans le repère du terrain */
			point.x = robot_position->x + sensor->config->x * robot_position->cosAngle - (sensor->distance + sensor->config->y) * robot_position->sinAngle;
			point.y = robot_position->y + sensor->config->x * robot_position->sinAngle + (sensor->distance + sensor->config->y) * robot_position->cosAngle;
		}
		else if(sick_sensor == DT50_RIGHT)
		{
			/* Calcul de la position du point visé par les capteurs dans le repère du terrain */
			point.x = robot_position->x + sensor->config->x * robot_position->cosAngle + (sensor->distance - sensor->config->y) * robot_position->sinAngle;
			point.y = robot_position->y + sensor->config->x * robot_position->sinAngle - (sensor->distance - sensor->config->y) * robot_position->cosAngle;	
		}
	}
	return point;
}
#endif

