/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp, CheckNorris
 *
 *	Fichier : telemeter.c
 *	Package : Carte Principale
 *	Description : Caractéristiques des télémètres
 *	Auteur : A.GIRARD, Jacen (Modifié par Ronan)
 *	Version 20110428
 */

#define TELEMETER_C

#include "telemeter.h"

/* Initialisation des télémètres */
void TELEMETER_init(void)
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
		
	TIMER_init();
	ADC_init();
	#if TELEMETER_NUMBER != 0
		/*intitialisation du télémètre TELE_FRONT_LEFT*/
		telemeter[TELE_FRONT_LEFT].pinADC = ADC_PIN_DT10_0;
		telemeter[TELE_FRONT_LEFT].type = TELE_SICK_DT10;
		telemeter[TELE_FRONT_LEFT].x = 136;
		telemeter[TELE_FRONT_LEFT].y = 137;
		telemeter[TELE_FRONT_LEFT].angle = 5L*PI4096/36;	
		
		/*intitialisation du télémètre TELE_FRONT_RIGHT*/
		telemeter[TELE_FRONT_RIGHT].pinADC = ADC_PIN_DT10_1;
		telemeter[TELE_FRONT_RIGHT].type = TELE_SICK_DT10;
		telemeter[TELE_FRONT_RIGHT].x = 136;
		telemeter[TELE_FRONT_RIGHT].y = -137;
		telemeter[TELE_FRONT_RIGHT].angle = -5L*PI4096/36;	
	
		/*intitialisation du télémètre TELE_BACK_LEFT*/
		telemeter[TELE_BACK_LEFT].pinADC = ADC_PIN_DT10_2;
		telemeter[TELE_BACK_LEFT].type = TELE_SICK_DT10;
		telemeter[TELE_BACK_LEFT].x = -136;
		telemeter[TELE_BACK_LEFT].y = 137;
		telemeter[TELE_BACK_LEFT].angle = 31L*PI4096/36;
	
		/*intitialisation du télémètre TELE_BACK_RIGHT*/
		telemeter[TELE_BACK_RIGHT].pinADC = ADC_PIN_DT10_3;
		telemeter[TELE_BACK_RIGHT].type = TELE_SICK_DT10;
		telemeter[TELE_BACK_RIGHT].x = -136;
		telemeter[TELE_BACK_RIGHT].y = -137;
		telemeter[TELE_BACK_RIGHT].angle = -31L*PI4096/36;
	#endif
	TELEMETER_init_Measure();
	TIMER3_run(100);
}

/* Mise à jour de la position du robot adverse grâce aux télémètres */
void TELEMETER_update()
{
	Uint8 telemeter_id; 	/*index de parcour du tableau de télémètre*/
	static time32_t last_update=0;

	//on synchronise les MàJ témémetre sur les MàJ Balise
	if((global.env.sensor[BEACON_IR_FOE_1].updated) || (global.env.sensor[BEACON_IR_FOE_2].updated) || //si on a une mise a jour balise
		((global.env.match_time - last_update) > (DETECTION_RESPONSE_TIME))) //ou que le dernier update est trop vieux.
	{
		last_update = global.env.match_time;
		for (telemeter_id=0; telemeter_id < TELEMETER_NUMBER; telemeter_id++)
		{
			switch(telemeter[telemeter_id].type)
			{
				case TELE_GP2D12:
					global.env.sensor[telemeter_id].distance = TELEMETER_converter(TELEMETER_get_value(telemeter_id)>>2, telemeter[telemeter_id].type);
					break;				
				case TELE_GP2YO21: 
					global.env.sensor[telemeter_id].distance = TELEMETER_converter(TELEMETER_get_value(telemeter_id)>>2, telemeter[telemeter_id].type);
					break;
				case TELE_SICK_DT10:
					global.env.sensor[telemeter_id].distance = TELEMETER_converter(TELEMETER_get_value(telemeter_id), telemeter[telemeter_id].type);
					break;
				default:
					break;
			}
			global.env.sensor[telemeter_id].angle = telemeter[telemeter_id].angle;	
			TELEMETER_toFOE(telemeter_id);
		}
	}
}

/*	Complète les infos du telemetre passé en argument pour les inscrire dans global.env.sensor
 *	Elimine les valeurs incohérentes
 */
static void TELEMETER_toFOE(Uint8 telemetre_id)
{
	Sint16 x; /*abscisse de calcul intermédiaire*/
	Sint16 y; /*ordonnée de calcul intermédiaire*/
	Sint16 foeX, foeY;

	if(global.env.sensor[telemetre_id].distance < TELEMETER_THRESHOLD)
	{
		/*considération de la distance si elle est inférieure au seuil de détection*/		
		/*calcul de la position de l'obstacle*/
		/*position dans le repère du robot*/
		//on incrémente distance de 15 pour être plus proches du centre du robot adverse
		x = (global.env.sensor[telemetre_id].distance + 150) * cos4096(telemeter[telemetre_id].angle) + telemeter[telemetre_id].x;
		y = (global.env.sensor[telemetre_id].distance + 150) * sin4096(telemeter[telemetre_id].angle) + telemeter[telemetre_id].y;
		
		/*transposition dans le repère absolu du terraine*/				
		foeX = x * global.env.pos.cosAngle - y * global.env.pos.sinAngle + global.env.pos.x;
		foeY = x * global.env.pos.sinAngle + y * global.env.pos.cosAngle + global.env.pos.y;
		
		if(ENV_game_zone_filter(foeX, foeY, BORDER_DELTA))
		{
			/*considération des valeurs si elles sont valides (cf. ENV_game_zone_filter)*/
			global.env.sensor[telemetre_id].foeX = foeX;
			global.env.sensor[telemetre_id].foeY = foeY;
			global.env.sensor[telemetre_id].update_time = global.env.match_time;
			global.env.sensor[telemetre_id].updated = TRUE;
		}
		else
		{
		}
	}
}

static Uint16 TELEMETER_converter(Sint16 tension, telemeter_type_e type)
{
	Uint8 index;
	Uint16 distance = 0;

	/*matrice de conversion tension->distance en fonction du capteur*/
	static const Uint8 TELEMETER_conversion[TELEMETER_TYPE_NB][TELEMETER_VALUE_CONVERTED][2] = 
	{
		{
			{9, 135},
			{9, 134},
			{9, 133},
			{9, 132},
			{10, 123},
			{12, 107},
			{14, 93},
			{16, 84},
			{18, 77},
			{20, 70},
			{25, 58},
			{30, 49},
			{35, 43},
			{40, 38},
			{45, 34},
			{50, 31},
			{55, 29},
			{60, 27},
			{65, 25},
			{70, 23},
			{75, 22},
			{80, 20}
		},
		{
			{6, 163},
			{7, 147},
			{8, 137},
			{9, 127},
			{10, 117},
			{12, 107},
			{14, 91},
			{16, 81},
			{18, 77},
			{20, 66},
			{25, 56},
			{30, 45},
			{35, 40},
			{40, 38},
			{45, 33},
			{50, 30},
			{55, 26},
			{60, 25},
			{65, 23},
			{70, 22},
			{75, 21},
			{80, 20}
		}
	};	
	
	if(type == TELE_GP2D12 || type == TELE_GP2YO21)
	{
		index = TELEMETER_VALUE_CONVERTED-1;
		while (index && (TELEMETER_conversion[type][index][1] < tension))
		{
			index --;		
		}
		/* on renvoie la premiere distance inférieure à la mesure*/
		distance = TELEMETER_conversion[type][index][0]*10;
	}
	else if(type == TELE_SICK_DT10)
	{
		distance = ((Uint32)(39970UL * tension) >> 16) - 56;
	}	
	
	return distance; //en mm
}

static void TELEMETER_init_Measure(void)
{
	int i,j;
	for( i = 0 ; i < TELEMETER_NUMBER ; i++)
	{
		for( j = 0 ; j < TELEMETER_BUFFER_SIZE; j++)
		{
			telemeter[i].measures[j] = 0;	
		}
	}
}

/* Renvoie la valeur échantillonnée sur un télémetre (après traitement de signal) */
static Uint16 TELEMETER_get_value(Uint8 telemeter_id)
{
	Sint8 index; //Pour faire un Warning à 128 echantillons. Gerer le dépassement de capacité ci dessous si modification
	Uint16 sum = 0;
	Sint16* measures = telemeter[telemeter_id].measures;
	for (index=0; index < TELEMETER_BUFFER_SIZE; index++)
		sum += ((Uint16) *(measures+index));
	return (sum / TELEMETER_BUFFER_SIZE);
}	

void _ISR _T3Interrupt (void)
{
	TELEMETER_update_value();
	IFS0bits.T3IF = 0;
}

static void TELEMETER_update_value()
{
#if TELEMETER_NUMBER != 0
	Uint8 i;
	telemeter_device_t* device; /*pointeur de travail pour l'utilisation des télémètres*/
	for(i = 0; i < TELEMETER_NUMBER ; i++)
	{
		/*initialisation*/
		device = &(telemeter[i]);	
		
		device->measures[device->measures_index] = (ADC_getValue(device->pinADC));
		device->measures_index++;
		device->measures_index %= TELEMETER_BUFFER_SIZE;
	}
#endif
}
