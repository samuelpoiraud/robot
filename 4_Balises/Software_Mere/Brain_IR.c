/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Brain_IR.c
 *	Package : Projet Balise Mère
 *	Description : Traitement des infos reçues par les balises IR et calculs des positions adverses.
 *	Auteur : Nirgal
 *	Version 201208
 */
	#include "Brain_IR.h"
	#include "Array_IR.h"
	#include "Secretary.h"
	#include "Eyes.h"
	
/*
O : balises fixes
R : 1 robot adverse

  ----y->				3m
  |		________________________________O  Corner
  x		|							  /	|
  |		|							 /	|
  v		|				    	    / 	|
		O--------------------------R   	|  2m
		Midle					  	\	|
		|							 \	|
		|							  \	|
		________________________________O  Mother

*/

volatile adversary_eyes_buffers_t * p_adversary_eyes_buffers;

void BRAIN_IR_init(void)
{
	p_adversary_eyes_buffers = NULL;
}

	
//Return TRUE si le traitement précédent est terminé... FALSE sinon (dans ce cas, il ne faut pas écraser les données encore en cours de traitement)
bool_e BRAIN_IR_task_add(adversary_eyes_buffers_t * adversary_eyes_buffers)
{
	if(p_adversary_eyes_buffers == NULL)	//Si le pointeur qui retient la prochaine struture à traiter est disponible...
	{
		p_adversary_eyes_buffers = adversary_eyes_buffers;
		return TRUE;
	}	
	else
		return FALSE;
}	

typedef struct
{
	Uint16 center;
	Uint16 duration;
}	
detections_t;

detections_t bigger_detections[BEACON_ID_MIDLE + 1];


void BRAIN_IR_end_of_cycle(adversary_eyes_buffers_t * adversary_eyes_buffers)
{
	//calcul des durations et des midle !
	Uint16 index_max, index;
	Uint16 * p_buffer;
	Uint16 bigger_detection_duration;
	Uint16 bigger_detection_center;
	Uint16 current_detection_begin;
	Uint16 current_detection_end;
	Uint16 current_detection_duration;
	Uint16 time;
	beacon_id_e beacon_id;
	
	for(beacon_id = 0; beacon_id <= BEACON_ID_MIDLE; beacon_id++)
	{
		
		p_buffer = &adversary_eyes_buffers->buffers[beacon_id].buffer[0];
		
		bigger_detection_duration = 0;
		bigger_detection_center = 0;
		
		current_detection_begin = 0;
		current_detection_end = 0;
		current_detection_duration = 0;
		
		index_max = adversary_eyes_buffers->buffers[beacon_id].index;
	//	debug_printf("A%d:%d\n",adversary_eyes_buffers->adversary, adversary_eyes_buffers->buffers[0].index);
		for(index = 0; index < index_max; index++)
		{	//Parcourt de chaque détection pour une balise.
			//	debug_printf("%d ",adversary_eyes_buffers->buffers[BEACON_ID_MOTHER].buffer[index]);
		
			time = p_buffer[index];	//Instant du front à traiter.
			if(time - current_detection_end < 30) //=1000 us	//on consulte la détection concernée pour savoir où elle en est.
			{ 	//La détection se poursuit.
				//=> on met à jour la fin de la détection
				current_detection_end = time;
				current_detection_duration = time - current_detection_begin;
			}	
			else
			{	//si le dernier front montant date de + de 1000 us :	
				
				//On traite la détection précédente (si elle existe)
				if(current_detection_duration > bigger_detection_duration)
				{
				//	debug_printf("D%d\n",current_detection_duration);
					bigger_detection_duration = current_detection_duration;
					bigger_detection_center = current_detection_begin + (bigger_detection_duration/2);
				}
				
				//On démarre une nouvelle détection	
				current_detection_begin = time;
				current_detection_end = time;
				current_detection_duration = 0;
				
			}	
		}	
		//Dans le cas où la dernière détection est la plus grande... (et ou du coup on est pas passé dans le else pour l'enregistrer !)
		if(current_detection_duration > bigger_detection_duration)
		{
			bigger_detection_duration = current_detection_duration;
			bigger_detection_center = current_detection_begin + (bigger_detection_duration/2);
		}
				
	//	debug_printf("B%d:%d\n",bigger_detection_duration,bigger_detection_center);
		bigger_detections[beacon_id].center = bigger_detection_center;
		bigger_detections[beacon_id].duration = bigger_detection_duration;
		
	}	
	
	//Les plus grandes détections pour chaque balises sont dans bigger_detections.
		
}




#define MOTOR_ONE_TURN_DURATION 2353 //[25us] //1506	//58,8ms = 1506 [39.06us]
Uint8 BRAIN_IR_find_ab(Sint16 * a, Sint16 * b)
{
	Sint16 a16,b16;
	//On a 3 instants correspondants aux milieu des plus grandes détections pour chaque balise.
	Uint16 time_mother = bigger_detections[BEACON_ID_MOTHER].center;
	Uint16 time_corner = bigger_detections[BEACON_ID_CORNER].center;
	Uint16 time_midle = bigger_detections[BEACON_ID_MIDLE].center;
	
/*	Sint16 tmo, tmi, tco;
	tmo = (Sint16)((((Sint32)(time_mother))*360)/MOTOR_ONE_TURN_DURATION);
	tmi = (Sint16)((((Sint32)(time_midle))*360)/MOTOR_ONE_TURN_DURATION);
	tco = (Sint16)((((Sint32)(time_corner))*360)/MOTOR_ONE_TURN_DURATION);
	debug_printf("%d %d %d\n", tmo, tmi, tco);
*/	 
	if(time_mother < time_corner && time_mother < time_midle)
	{
		time_corner -= time_mother;
		time_midle -= time_mother;
		time_mother = 0;		
		if(time_midle > MOTOR_ONE_TURN_DURATION)
			time_midle -= MOTOR_ONE_TURN_DURATION;
		if(time_corner > MOTOR_ONE_TURN_DURATION)
			time_corner -= MOTOR_ONE_TURN_DURATION;
		if(time_corner < time_midle)
		{
			debug_printf("way error CoMi\n");
			return -2;
		}	
		a16 = MOTOR_ONE_TURN_DURATION-time_corner;
		b16 = time_midle;
		
	}
	else if(time_corner < time_mother && time_corner < time_midle)
	{
		time_mother -= time_corner;
		time_midle -= time_corner;
		time_corner = 0;
		if(time_midle > MOTOR_ONE_TURN_DURATION)
			time_midle -= MOTOR_ONE_TURN_DURATION;
		if(time_mother > MOTOR_ONE_TURN_DURATION)
			time_mother -= MOTOR_ONE_TURN_DURATION;
		if(time_midle < time_mother)
		{
			debug_printf("way error MiMo\n");
			return -2;
		}	
		a16 = time_mother;
		b16 = time_midle - time_mother;
	}
	else
	{
		time_mother -= time_midle;
		time_corner -= time_midle;
		time_midle = 0;
		if(time_corner > MOTOR_ONE_TURN_DURATION)
			time_corner -= MOTOR_ONE_TURN_DURATION;
		if(time_mother > MOTOR_ONE_TURN_DURATION)
			time_mother -= MOTOR_ONE_TURN_DURATION;
		if(time_mother < time_corner)
		{
			debug_printf("way error MoCo\n");
			return -2;
		}
		a16 = time_mother - time_corner;
		b16 = MOTOR_ONE_TURN_DURATION - time_mother;
	}	
		
	*a = (Sint16)((((Sint32)(a16))*223)>>12);
	*b = (Sint16)((((Sint32)(b16))*223)>>12);
	//2353 * 223 >> 12   =  128,1 ! (proche de 128)	
	return 0;
}	


Uint8 BRAIN_IR_find_xy(Sint16 a, Sint16 b, Sint16 * x, Sint16 * y)
{
	*x = find_x(a,b);
	*y = find_y(a,b);
	if(*x == BAD || *y == BAD)
		return -4;
	else
		return 0;
}	


void BRAIN_IR_process_main(void)
{
	Sint16 a, b;
	Sint16 x, y;
	Sint8 error = 0;
	if(p_adversary_eyes_buffers)
	{
		BRAIN_IR_end_of_cycle((adversary_eyes_buffers_t*)(p_adversary_eyes_buffers));
		
		error |= BRAIN_IR_find_ab(&a, &b);
		if(!error)
			error |= BRAIN_IR_find_xy(a, b, &x, &y);
		//printf("a%d b%d\n",a,b);
		//printf("%d", TMR2);
		if(error)
			printf("err -%d\n",-error);
		else
			printf("x%d y%d\n",x,y);
		
		#warning "TODO remontée d'info."
		//SECRETARY_send_....._(p_adversary_eyes_buffers->adversary, x, y, error);
		
		EYES_init_adversary_eyes_buffers((adversary_eyes_buffers_t*)(p_adversary_eyes_buffers));
		//EYES_init_adversary_beacons_detections((adversary_beacons_detections_t *)(p_adversary_eyes_buffers));
		
		
		p_adversary_eyes_buffers = NULL;
	}	
}	


//____________________________FONCTIONS DE TEST_________________

	
void test_BRAIN_IR_find_ab(Uint16 time_corner, Uint16 time_mother, Uint16 time_midle)
{
	Sint16 a, b, x, y;
	Uint8 r;
	bigger_detections[BEACON_ID_CORNER].center 	= time_corner;
	bigger_detections[BEACON_ID_MOTHER].center 	= time_mother;
	bigger_detections[BEACON_ID_MIDLE].center 	= time_midle;
	r = BRAIN_IR_find_ab(&a,&b);
	if(r)
		printf("bad ab %d - ",r);
	else
		printf("a%d b%d - ",a,b);
	r = BRAIN_IR_find_xy(a,b,&x,&y);
	if(r)
		printf("bad xy %d\n",r);
	else
		printf("x%d y%d\n",x,y);
}	



void jeu_test_BRAIN_IR_find_ab(void)
{
/*
test_BRAIN_IR_find_ab(2876,1605,217);
test_BRAIN_IR_find_ab(2759,1605,218);
test_BRAIN_IR_find_ab(2641,1605,218);
test_BRAIN_IR_find_ab(2523,1605,218);
test_BRAIN_IR_find_ab(2406,1605,218);
test_BRAIN_IR_find_ab(2288,1605,218);
test_BRAIN_IR_find_ab(2170,1605,218);
test_BRAIN_IR_find_ab(2053,1605,218);
test_BRAIN_IR_find_ab(1935,1605,218);
test_BRAIN_IR_find_ab(1817,1605,218);
test_BRAIN_IR_find_ab(2993,1605,334);
test_BRAIN_IR_find_ab(2876,1605,334);
test_BRAIN_IR_find_ab(2758,1605,334);
test_BRAIN_IR_find_ab(2640,1605,334);
test_BRAIN_IR_find_ab(2523,1605,335);
test_BRAIN_IR_find_ab(2405,1605,334);
test_BRAIN_IR_find_ab(2287,1605,334);
test_BRAIN_IR_find_ab(2170,1605,335);
test_BRAIN_IR_find_ab(2052,1605,334);
test_BRAIN_IR_find_ab(1934,1605,334);
test_BRAIN_IR_find_ab(2994,1605,453);
test_BRAIN_IR_find_ab(2876,1605,452);
test_BRAIN_IR_find_ab(2758,1605,452);
test_BRAIN_IR_find_ab(2641,1605,453);
test_BRAIN_IR_find_ab(2523,1605,453);
test_BRAIN_IR_find_ab(2405,1605,452);
test_BRAIN_IR_find_ab(2288,1605,453);
test_BRAIN_IR_find_ab(2170,1605,453);
test_BRAIN_IR_find_ab(2052,1605,452);
test_BRAIN_IR_find_ab(2994,1605,571);
test_BRAIN_IR_find_ab(2876,1605,570);
test_BRAIN_IR_find_ab(2759,1605,571);
test_BRAIN_IR_find_ab(2641,1605,571);
test_BRAIN_IR_find_ab(2523,1605,571);
test_BRAIN_IR_find_ab(2406,1605,571);
test_BRAIN_IR_find_ab(2288,1605,571);
test_BRAIN_IR_find_ab(2170,1605,571);
test_BRAIN_IR_find_ab(2994,1605,689);
test_BRAIN_IR_find_ab(2877,1605,689);
test_BRAIN_IR_find_ab(2759,1605,689);
test_BRAIN_IR_find_ab(2641,1605,689);
test_BRAIN_IR_find_ab(2524,1605,690);
test_BRAIN_IR_find_ab(2406,1605,689);
test_BRAIN_IR_find_ab(2288,1605,689);
test_BRAIN_IR_find_ab(2994,1605,806);
test_BRAIN_IR_find_ab(2876,1605,805);
test_BRAIN_IR_find_ab(2758,1605,805);
test_BRAIN_IR_find_ab(2641,1605,806);
test_BRAIN_IR_find_ab(2523,1605,806);
test_BRAIN_IR_find_ab(2405,1605,805);
test_BRAIN_IR_find_ab(2994,1605,924);
test_BRAIN_IR_find_ab(2876,1605,923);
test_BRAIN_IR_find_ab(2759,1605,924);
test_BRAIN_IR_find_ab(2641,1605,924);
test_BRAIN_IR_find_ab(2523,1605,924);
test_BRAIN_IR_find_ab(2994,1605,1042);
test_BRAIN_IR_find_ab(2877,1605,1042);
test_BRAIN_IR_find_ab(2759,1605,1042);
test_BRAIN_IR_find_ab(2641,1605,1042);
test_BRAIN_IR_find_ab(2994,1605,1159);
test_BRAIN_IR_find_ab(2876,1605,1158);
test_BRAIN_IR_find_ab(2758,1605,1158);
test_BRAIN_IR_find_ab(2994,1605,1277);
test_BRAIN_IR_find_ab(2876,1605,1276);
test_BRAIN_IR_find_ab(2994,1605,1395);
*/
/*
test_BRAIN_IR_find_ab(1270,1505,1623);
test_BRAIN_IR_find_ab(1153,1505,1623);
test_BRAIN_IR_find_ab(1035,1505,1623);
test_BRAIN_IR_find_ab(917,1505,1623);
test_BRAIN_IR_find_ab(800,1505,1623);
test_BRAIN_IR_find_ab(682,1505,1623);
test_BRAIN_IR_find_ab(564,1505,1623);
test_BRAIN_IR_find_ab(447,1505,1623);
test_BRAIN_IR_find_ab(329,1505,1623);
test_BRAIN_IR_find_ab(211,1505,1623);
test_BRAIN_IR_find_ab(1387,1505,1740);
test_BRAIN_IR_find_ab(1270,1505,1740);
test_BRAIN_IR_find_ab(1152,1505,1740);
test_BRAIN_IR_find_ab(1034,1505,1740);
test_BRAIN_IR_find_ab(917,1505,1740);
test_BRAIN_IR_find_ab(799,1505,1740);
test_BRAIN_IR_find_ab(681,1505,1740);
test_BRAIN_IR_find_ab(564,1505,1740);
test_BRAIN_IR_find_ab(446,1505,1740);
test_BRAIN_IR_find_ab(328,1505,1740);
test_BRAIN_IR_find_ab(1388,1505,1858);
test_BRAIN_IR_find_ab(1270,1505,1858);
test_BRAIN_IR_find_ab(1152,1505,1858);
test_BRAIN_IR_find_ab(1035,1505,1858);
test_BRAIN_IR_find_ab(917,1505,1858);
test_BRAIN_IR_find_ab(799,1505,1858);
test_BRAIN_IR_find_ab(682,1505,1858);
test_BRAIN_IR_find_ab(564,1505,1858);
test_BRAIN_IR_find_ab(446,1505,1858);
test_BRAIN_IR_find_ab(1388,1505,1976);
test_BRAIN_IR_find_ab(1270,1505,1976);
test_BRAIN_IR_find_ab(1153,1505,1976);
test_BRAIN_IR_find_ab(1035,1505,1976);
test_BRAIN_IR_find_ab(917,1505,1976);
test_BRAIN_IR_find_ab(800,1505,1976);
test_BRAIN_IR_find_ab(682,1505,1976);
test_BRAIN_IR_find_ab(564,1505,1976);
test_BRAIN_IR_find_ab(1388,1505,2094);
test_BRAIN_IR_find_ab(1271,1505,2094);
test_BRAIN_IR_find_ab(1153,1505,2094);
test_BRAIN_IR_find_ab(1035,1505,2094);
test_BRAIN_IR_find_ab(918,1505,2094);
test_BRAIN_IR_find_ab(800,1505,2094);
test_BRAIN_IR_find_ab(682,1505,2094);
test_BRAIN_IR_find_ab(1388,1505,2211);
test_BRAIN_IR_find_ab(1270,1505,2211);
test_BRAIN_IR_find_ab(1152,1505,2211);
test_BRAIN_IR_find_ab(1035,1505,2211);
test_BRAIN_IR_find_ab(917,1505,2211);
test_BRAIN_IR_find_ab(799,1505,2211);
test_BRAIN_IR_find_ab(1388,1505,2329);
test_BRAIN_IR_find_ab(1270,1505,2329);
test_BRAIN_IR_find_ab(1153,1505,2329);
test_BRAIN_IR_find_ab(1035,1505,2329);
test_BRAIN_IR_find_ab(917,1505,2329);
test_BRAIN_IR_find_ab(1388,1505,2447);
test_BRAIN_IR_find_ab(1271,1505,2447);
test_BRAIN_IR_find_ab(1153,1505,2447);
test_BRAIN_IR_find_ab(1035,1505,2447);
test_BRAIN_IR_find_ab(1388,1505,2564);
test_BRAIN_IR_find_ab(1270,1505,2564);
test_BRAIN_IR_find_ab(1152,1505,2564);
test_BRAIN_IR_find_ab(1388,1505,2682);
test_BRAIN_IR_find_ab(1270,1505,2682);
test_BRAIN_IR_find_ab(1388,1505,2800);
*/
	
/*	test_BRAIN_IR_find_ab(2877,100,1724);
test_BRAIN_IR_find_ab(2760,100,1724);
test_BRAIN_IR_find_ab(2642,100,1724);
test_BRAIN_IR_find_ab(2524,100,1724);
test_BRAIN_IR_find_ab(2407,100,1724);
test_BRAIN_IR_find_ab(2289,100,1724);
test_BRAIN_IR_find_ab(2171,100,1724);
test_BRAIN_IR_find_ab(2054,100,1724);
test_BRAIN_IR_find_ab(1936,100,1724);
test_BRAIN_IR_find_ab(1818,100,1724);
test_BRAIN_IR_find_ab(2994,100,1841);
test_BRAIN_IR_find_ab(2877,100,1841);
test_BRAIN_IR_find_ab(2759,100,1841);
test_BRAIN_IR_find_ab(2641,100,1841);
test_BRAIN_IR_find_ab(2524,100,1841);
test_BRAIN_IR_find_ab(2406,100,1841);
test_BRAIN_IR_find_ab(2288,100,1841);
test_BRAIN_IR_find_ab(2171,100,1841);
test_BRAIN_IR_find_ab(2053,100,1841);
test_BRAIN_IR_find_ab(1935,100,1841);
test_BRAIN_IR_find_ab(2995,100,1959);
test_BRAIN_IR_find_ab(2877,100,1959);
test_BRAIN_IR_find_ab(2759,100,1959);
test_BRAIN_IR_find_ab(2642,100,1959);
test_BRAIN_IR_find_ab(2524,100,1959);
test_BRAIN_IR_find_ab(2406,100,1959);
test_BRAIN_IR_find_ab(2289,100,1959);
test_BRAIN_IR_find_ab(2171,100,1959);
test_BRAIN_IR_find_ab(2053,100,1959);
test_BRAIN_IR_find_ab(2995,100,2077);
test_BRAIN_IR_find_ab(2877,100,2077);
test_BRAIN_IR_find_ab(2760,100,2077);
test_BRAIN_IR_find_ab(2642,100,2077);
test_BRAIN_IR_find_ab(2524,100,2077);
test_BRAIN_IR_find_ab(2407,100,2077);
test_BRAIN_IR_find_ab(2289,100,2077);
test_BRAIN_IR_find_ab(2171,100,2077);
test_BRAIN_IR_find_ab(2995,100,2195);
test_BRAIN_IR_find_ab(2878,100,2195);
test_BRAIN_IR_find_ab(2760,100,2195);
test_BRAIN_IR_find_ab(2642,100,2195);
test_BRAIN_IR_find_ab(2525,100,2195);
test_BRAIN_IR_find_ab(2407,100,2195);
test_BRAIN_IR_find_ab(2289,100,2195);
test_BRAIN_IR_find_ab(2995,100,2312);
test_BRAIN_IR_find_ab(2877,100,2312);
test_BRAIN_IR_find_ab(2759,100,2312);
test_BRAIN_IR_find_ab(2642,100,2312);
test_BRAIN_IR_find_ab(2524,100,2312);
test_BRAIN_IR_find_ab(2406,100,2312);
test_BRAIN_IR_find_ab(2995,100,2430);
test_BRAIN_IR_find_ab(2877,100,2430);
test_BRAIN_IR_find_ab(2760,100,2430);
test_BRAIN_IR_find_ab(2642,100,2430);
test_BRAIN_IR_find_ab(2524,100,2430);
test_BRAIN_IR_find_ab(2995,100,2548);
test_BRAIN_IR_find_ab(2878,100,2548);
test_BRAIN_IR_find_ab(2760,100,2548);
test_BRAIN_IR_find_ab(2642,100,2548);
test_BRAIN_IR_find_ab(2995,100,2665);
test_BRAIN_IR_find_ab(2877,100,2665);
test_BRAIN_IR_find_ab(2759,100,2665);
test_BRAIN_IR_find_ab(2995,100,2783);
test_BRAIN_IR_find_ab(2877,100,2783);
test_BRAIN_IR_find_ab(2995,100,2901);
*/
	/*
test_BRAIN_IR_find_ab(1589,200,1825);
test_BRAIN_IR_find_ab(1471,200,1824);
test_BRAIN_IR_find_ab(1354,200,1825);
test_BRAIN_IR_find_ab(1236,200,1825);
test_BRAIN_IR_find_ab(1118,200,1825);
test_BRAIN_IR_find_ab(1001,200,1825);
test_BRAIN_IR_find_ab(883,200,1825);
test_BRAIN_IR_find_ab(765,200,1825);
test_BRAIN_IR_find_ab(648,200,1825);
test_BRAIN_IR_find_ab(530,200,1825);
test_BRAIN_IR_find_ab(412,200,1825);
test_BRAIN_IR_find_ab(1588,200,1941);
test_BRAIN_IR_find_ab(1471,200,1941);
test_BRAIN_IR_find_ab(1353,200,1941);
test_BRAIN_IR_find_ab(1235,200,1941);
test_BRAIN_IR_find_ab(1118,200,1942);
test_BRAIN_IR_find_ab(1000,200,1941);
test_BRAIN_IR_find_ab(882,200,1941);
test_BRAIN_IR_find_ab(765,200,1942);
test_BRAIN_IR_find_ab(647,200,1941);
test_BRAIN_IR_find_ab(529,200,1941);
test_BRAIN_IR_find_ab(1589,200,2060);
test_BRAIN_IR_find_ab(1471,200,2059);
test_BRAIN_IR_find_ab(1353,200,2059);
test_BRAIN_IR_find_ab(1236,200,2060);
test_BRAIN_IR_find_ab(1118,200,2060);
test_BRAIN_IR_find_ab(1000,200,2059);
test_BRAIN_IR_find_ab(883,200,2060);
test_BRAIN_IR_find_ab(765,200,2060);
test_BRAIN_IR_find_ab(647,200,2059);
test_BRAIN_IR_find_ab(1589,200,2178);
test_BRAIN_IR_find_ab(1471,200,2177);
test_BRAIN_IR_find_ab(1354,200,2178);
test_BRAIN_IR_find_ab(1236,200,2178);
test_BRAIN_IR_find_ab(1118,200,2178);
test_BRAIN_IR_find_ab(1001,200,2178);
test_BRAIN_IR_find_ab(883,200,2178);
test_BRAIN_IR_find_ab(765,200,2178);
test_BRAIN_IR_find_ab(1589,200,2296);
test_BRAIN_IR_find_ab(1472,200,2296);
test_BRAIN_IR_find_ab(1354,200,2296);
test_BRAIN_IR_find_ab(1236,200,2296);
test_BRAIN_IR_find_ab(1119,200,2297);
test_BRAIN_IR_find_ab(1001,200,2296);
test_BRAIN_IR_find_ab(883,200,2296);
test_BRAIN_IR_find_ab(1589,200,2413);
test_BRAIN_IR_find_ab(1471,200,2412);
test_BRAIN_IR_find_ab(1353,200,2412);
test_BRAIN_IR_find_ab(1236,200,2413);
test_BRAIN_IR_find_ab(1118,200,2413);
test_BRAIN_IR_find_ab(1000,200,2412);
test_BRAIN_IR_find_ab(1589,200,2531);
test_BRAIN_IR_find_ab(1471,200,2530);
test_BRAIN_IR_find_ab(1354,200,2531);
test_BRAIN_IR_find_ab(1236,200,2531);
test_BRAIN_IR_find_ab(1118,200,2531);
test_BRAIN_IR_find_ab(1589,200,2649);
test_BRAIN_IR_find_ab(1472,200,2649);
test_BRAIN_IR_find_ab(1354,200,2649);
test_BRAIN_IR_find_ab(1236,200,2649);
test_BRAIN_IR_find_ab(1589,200,2766);
test_BRAIN_IR_find_ab(1471,200,2765);
test_BRAIN_IR_find_ab(1353,200,2765);
test_BRAIN_IR_find_ab(1589,200,2884);
test_BRAIN_IR_find_ab(1471,200,2883);
test_BRAIN_IR_find_ab(1589,200,3002);
*/
	
/*
test_BRAIN_IR_find_ab(100,218,336);
test_BRAIN_IR_find_ab(100,335,453);
test_BRAIN_IR_find_ab(100,453,571);
test_BRAIN_IR_find_ab(100,571,689);
test_BRAIN_IR_find_ab(100,689,807);
test_BRAIN_IR_find_ab(100,806,924);
test_BRAIN_IR_find_ab(100,924,1042);
test_BRAIN_IR_find_ab(100,1042,1160);
test_BRAIN_IR_find_ab(100,1159,1277);
test_BRAIN_IR_find_ab(100,1277,1395);
test_BRAIN_IR_find_ab(100,1395,1513);
test_BRAIN_IR_find_ab(100,218,453);
test_BRAIN_IR_find_ab(100,335,570);
test_BRAIN_IR_find_ab(100,453,688);
test_BRAIN_IR_find_ab(100,571,806);
test_BRAIN_IR_find_ab(100,689,924);
test_BRAIN_IR_find_ab(100,806,1041);
test_BRAIN_IR_find_ab(100,924,1159);
test_BRAIN_IR_find_ab(100,1042,1277);
test_BRAIN_IR_find_ab(100,1159,1394);
test_BRAIN_IR_find_ab(100,1277,1512);
test_BRAIN_IR_find_ab(100,218,571);
test_BRAIN_IR_find_ab(100,335,688);
test_BRAIN_IR_find_ab(100,453,806);
test_BRAIN_IR_find_ab(100,571,924);
test_BRAIN_IR_find_ab(100,689,1042);
test_BRAIN_IR_find_ab(100,806,1159);
test_BRAIN_IR_find_ab(100,924,1277);
test_BRAIN_IR_find_ab(100,1042,1395);
test_BRAIN_IR_find_ab(100,1159,1512);
test_BRAIN_IR_find_ab(100,218,689);
test_BRAIN_IR_find_ab(100,335,806);
test_BRAIN_IR_find_ab(100,453,924);
test_BRAIN_IR_find_ab(100,571,1042);
test_BRAIN_IR_find_ab(100,689,1160);
test_BRAIN_IR_find_ab(100,806,1277);
test_BRAIN_IR_find_ab(100,924,1395);
test_BRAIN_IR_find_ab(100,1042,1513);
test_BRAIN_IR_find_ab(100,218,807);
test_BRAIN_IR_find_ab(100,335,924);
test_BRAIN_IR_find_ab(100,453,1042);
test_BRAIN_IR_find_ab(100,571,1160);
test_BRAIN_IR_find_ab(100,689,1278);
test_BRAIN_IR_find_ab(100,806,1395);
test_BRAIN_IR_find_ab(100,924,1513);
test_BRAIN_IR_find_ab(100,218,924);
test_BRAIN_IR_find_ab(100,335,1041);
test_BRAIN_IR_find_ab(100,453,1159);
test_BRAIN_IR_find_ab(100,571,1277);
test_BRAIN_IR_find_ab(100,689,1395);
test_BRAIN_IR_find_ab(100,806,1512);
test_BRAIN_IR_find_ab(100,218,1042);
test_BRAIN_IR_find_ab(100,335,1159);
test_BRAIN_IR_find_ab(100,453,1277);
test_BRAIN_IR_find_ab(100,571,1395);
test_BRAIN_IR_find_ab(100,689,1513);
test_BRAIN_IR_find_ab(100,218,1160);
test_BRAIN_IR_find_ab(100,335,1277);
test_BRAIN_IR_find_ab(100,453,1395);
test_BRAIN_IR_find_ab(100,571,1513);
test_BRAIN_IR_find_ab(100,218,1277);
test_BRAIN_IR_find_ab(100,335,1394);
test_BRAIN_IR_find_ab(100,453,1512);
test_BRAIN_IR_find_ab(100,218,1395);
test_BRAIN_IR_find_ab(100,335,1512);
test_BRAIN_IR_find_ab(100,218,1513);
*/
/*
test_BRAIN_IR_find_ab(1489,100,218);
test_BRAIN_IR_find_ab(1371,100,218);
test_BRAIN_IR_find_ab(1254,100,218);
test_BRAIN_IR_find_ab(1136,100,218);
test_BRAIN_IR_find_ab(1018,100,218);
test_BRAIN_IR_find_ab(901,100,218);
test_BRAIN_IR_find_ab(783,100,218);
test_BRAIN_IR_find_ab(665,100,218);
test_BRAIN_IR_find_ab(548,100,218);
test_BRAIN_IR_find_ab(430,100,218);
test_BRAIN_IR_find_ab(312,100,218);
test_BRAIN_IR_find_ab(1488,100,335);
test_BRAIN_IR_find_ab(1371,100,335);
test_BRAIN_IR_find_ab(1253,100,335);
test_BRAIN_IR_find_ab(1135,100,335);
test_BRAIN_IR_find_ab(1018,100,335);
test_BRAIN_IR_find_ab(900,100,335);
test_BRAIN_IR_find_ab(782,100,335);
test_BRAIN_IR_find_ab(665,100,335);
test_BRAIN_IR_find_ab(547,100,335);
test_BRAIN_IR_find_ab(429,100,335);
test_BRAIN_IR_find_ab(1489,100,453);
test_BRAIN_IR_find_ab(1371,100,453);
test_BRAIN_IR_find_ab(1253,100,453);
test_BRAIN_IR_find_ab(1136,100,453);
test_BRAIN_IR_find_ab(1018,100,453);
test_BRAIN_IR_find_ab(900,100,453);
test_BRAIN_IR_find_ab(783,100,453);
test_BRAIN_IR_find_ab(665,100,453);
test_BRAIN_IR_find_ab(547,100,453);
test_BRAIN_IR_find_ab(1489,100,571);
test_BRAIN_IR_find_ab(1371,100,571);
test_BRAIN_IR_find_ab(1254,100,571);
test_BRAIN_IR_find_ab(1136,100,571);
test_BRAIN_IR_find_ab(1018,100,571);
test_BRAIN_IR_find_ab(901,100,571);
test_BRAIN_IR_find_ab(783,100,571);
test_BRAIN_IR_find_ab(665,100,571);
test_BRAIN_IR_find_ab(1489,100,689);
test_BRAIN_IR_find_ab(1372,100,689);
test_BRAIN_IR_find_ab(1254,100,689);
test_BRAIN_IR_find_ab(1136,100,689);
test_BRAIN_IR_find_ab(1019,100,689);
test_BRAIN_IR_find_ab(901,100,689);
test_BRAIN_IR_find_ab(783,100,689);
test_BRAIN_IR_find_ab(1489,100,806);
test_BRAIN_IR_find_ab(1371,100,806);
test_BRAIN_IR_find_ab(1253,100,806);
test_BRAIN_IR_find_ab(1136,100,806);
test_BRAIN_IR_find_ab(1018,100,806);
test_BRAIN_IR_find_ab(900,100,806);
test_BRAIN_IR_find_ab(1489,100,924);
test_BRAIN_IR_find_ab(1371,100,924);
test_BRAIN_IR_find_ab(1254,100,924);
test_BRAIN_IR_find_ab(1136,100,924);
test_BRAIN_IR_find_ab(1018,100,924);
test_BRAIN_IR_find_ab(1489,100,1042);
test_BRAIN_IR_find_ab(1372,100,1042);
test_BRAIN_IR_find_ab(1254,100,1042);
test_BRAIN_IR_find_ab(1136,100,1042);
test_BRAIN_IR_find_ab(1489,100,1159);
test_BRAIN_IR_find_ab(1371,100,1159);
test_BRAIN_IR_find_ab(1253,100,1159);
test_BRAIN_IR_find_ab(1489,100,1277);
test_BRAIN_IR_find_ab(1371,100,1277);
test_BRAIN_IR_find_ab(1489,100,1395);
*/

//  while(1);
}


