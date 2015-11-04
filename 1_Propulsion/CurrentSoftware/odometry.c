/*
 *  Club Robot ESEO 2006 - 2011
 *  Game Hoover, CheckNorris
 *
 *  Fichier : main.c
 *  Package : Asser
 *  Description : Fonctions permettant de calculer la position du robot
 *  Année 2005-2006 - modifiées réutilisées en 2006-2007
 *  Auteur : Asser 2006 - modif by Val'
 *  MAJ 2009 : Nirgal
 *  MAJ 200912 : Nirgal - modification des grandeurs/unités et de l'algo de MAJ_ODOMETRIE
 *  Version 201101
 */


#include "odometry.h"
#include "copilot.h"
#include "encoders.h"
#include "QS/QS_maths.h"
#include "debug.h"
#include "supervisor.h"
#include "gyroscope.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_timer.h"

volatile static Sint32 coefs[ODOMETRY_COEF_CENTRIFUGAL+1];
static Sint32 total_teta = 0;       //[rad/1024/4096]
static Sint32 max_total_teta = 0;   //[rad/1024/4096]
static Uint32 total_dist_x = 0;       //[mm/65536]
static Uint32 total_dist_y = 0;       //[mm/65536]



volatile Sint32 x32;		//Position précise en x [mm/65536]		<<16
volatile Sint32 y32;		//Position précise en y [mm/65536]		<<16
volatile Sint32 teta32;		//Position précise en teta [rad/4096/1024]	<<22
volatile color_e color;
volatile Sint16 calibration_backward_border_distance;
volatile Sint16 calibration_forward_border_distance;

#ifdef USE_GYROSCOPE
	static Sint32 ODOMETRY_get_speed_rotation_gyroway_corrected(void);
#endif

void ODOMETRY_init()
{
	ENCODERS_init();
	ODOMETRY_set_color(BOT_COLOR);
	if(QS_WHO_AM_I_get()==SMALL_ROBOT)
	{
		//SMALL
		coefs[ODOMETRY_COEF_TRANSLATION] 	 = 	SMALL_ODOMETRY_COEF_TRANSLATION_DEFAULT;
		coefs[ODOMETRY_COEF_SYM] 		 	 = 	SMALL_ODOMETRY_COEF_SYM_DEFAULT;
		coefs[ODOMETRY_COEF_ROTATION] 		 = 	SMALL_ODOMETRY_COEF_ROTATION_DEFAULT;
		coefs[ODOMETRY_COEF_CENTRIFUGAL] 	 = 	SMALL_ODOMETRY_COEF_CENTRIFUGAL_DEFAULT;
		calibration_backward_border_distance = 	SMALL_CALIBRATION_BACKWARD_BORDER_DISTANCE;
		calibration_forward_border_distance  = 	SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE;
	}
	else
	{
		//BIG
		coefs[ODOMETRY_COEF_TRANSLATION] 	 = 	BIG_ODOMETRY_COEF_TRANSLATION_DEFAULT;
		coefs[ODOMETRY_COEF_SYM] 			 = 	BIG_ODOMETRY_COEF_SYM_DEFAULT;
		coefs[ODOMETRY_COEF_ROTATION] 		 = 	BIG_ODOMETRY_COEF_ROTATION_DEFAULT;
		coefs[ODOMETRY_COEF_CENTRIFUGAL] 	 = 	BIG_ODOMETRY_COEF_CENTRIFUGAL_DEFAULT;
		calibration_backward_border_distance = 	BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE;
		calibration_forward_border_distance  = 	BIG_CALIBRATION_FORWARD_BORDER_DISTANCE;
	}
}

Sint16 get_calibration_backward_distance(void)
{
	return calibration_backward_border_distance;
}

void ODOMETRY_set_coef(PROPULSION_coef_e coef, Sint32 value)
{
	if(coef <= ODOMETRY_COEF_CENTRIFUGAL)
		coefs[coef] = value;
}

Sint32 ODOMETRY_get_coef(PROPULSION_coef_e coef)
{
	if(coef <= ODOMETRY_COEF_CENTRIFUGAL)
		return coefs[coef];
	else
		return 0;
}

void ODOMETRY_set_color(color_e new_color)
{
	color = new_color;

	if(QS_WHO_AM_I_get()==SMALL_ROBOT)
	{
		//SMALL
		if (new_color == TOP_COLOR)
		{
			x32	= SMALL_TOP_COLOR_START_X;		//[mm/65536]		(<<16)
			y32	= SMALL_TOP_COLOR_START_Y;		//[mm/65536]		(<<16)
			teta32 = SMALL_TOP_COLOR_START_TETA;		//[rad/4096/1024]	(<<22)
		}
		else
		{
			x32	= SMALL_BOT_COLOR_START_X ;		//[mm/65536]		(<<16)
			y32	= SMALL_BOT_COLOR_START_Y ;		//[mm/65536]		(<<16)
			teta32 = SMALL_BOT_COLOR_START_TETA ;		//[rad/4096/1024]	(<<22)
		}
	}
	else
	{
		//BIG
		if (new_color == TOP_COLOR)
		{
			x32	= BIG_TOP_COLOR_START_X;		//[mm/65536]		(<<16)
			y32	= BIG_TOP_COLOR_START_Y;		//[mm/65536]		(<<16)
			teta32 = BIG_TOP_COLOR_START_TETA;		//[rad/4096/1024]	(<<22)
		}
		else
		{
			x32	= BIG_BOT_COLOR_START_X ;		//[mm/65536]		(<<16)
			y32	= BIG_BOT_COLOR_START_Y ;		//[mm/65536]		(<<16)
			teta32 = BIG_BOT_COLOR_START_TETA ;		//[rad/4096/1024]	(<<22)
		}
	}
	global.position.x	= x32  >> 16;	//[mm]
	global.position.y	= y32  >> 16;	//[mm]
	global.position.teta = teta32 >> 10;	//[rad/4096]	(<<12)
}

color_e ODOMETRY_get_color(void)
{
	return color;
}
void ODOMETRY_set(Sint16 x, Sint16 y, Sint16 teta)
{
	//Permet d'imposer une positon au robot... utile pour les tests !!!
	//Par exemple pour les tests de trajectoires rectangles sans se prendre la tete !
	//Egalement utilisé lors des calage, et pour l'autocalibration
	global.position.x = x;								//[mm]
	global.position.y = y;								//[mm]
	global.position.teta = teta;						//[rad/4096]	(<<12)

	x32	= (Sint32)x	<< 16;		//[mm/65536]		(<<16)
	y32	= (Sint32)y	<< 16;		//[mm/65536]		(<<16)
	teta32 = (Sint32)teta << 10;		//[rad/4096/1024]	(<<22)
}



void ODOMETRY_correct_with_border(way_e way)
{
	Sint32 angle32;
	angle32 = (Sint32)(global.position.teta);

	if(way == BACKWARD)
	{
		if(angle32 >= -(Sint32)PI4096/4 && angle32 <= (Sint32)PI4096/4)
			ODOMETRY_set(calibration_backward_border_distance, global.position.y, 0);									//pas de changement en y.
		else if(angle32 >= (Sint32)PI4096/4 && angle32 <= (Sint32)PI4096*3/4)
			ODOMETRY_set(global.position.x, calibration_backward_border_distance, PI4096/2);							//pas de changement en x.
		else if(angle32 >= (Sint32)PI4096*3/4 || angle32 <= -(Sint32)PI4096*3/4)
			ODOMETRY_set(FIELD_SIZE_X - calibration_backward_border_distance, global.position.y, PI4096);			//pas de changement en y.
		else //(angle32 >= -PI4096/4*3 && angle32 <= -PI4096/4)
			ODOMETRY_set(global.position.x, FIELD_SIZE_Y - calibration_backward_border_distance, -(PI4096/2));	//pas de changement en x.
	}
	else	//global.sens_marche == AVANCER
	{
		if(angle32 >= -PI4096/4 && angle32 <= PI4096/4)
			ODOMETRY_set(FIELD_SIZE_X - calibration_forward_border_distance, global.position.y, 0);				//pas de changement en y.
		else if(angle32 >= PI4096/4 && angle32 <= PI4096/4*3)
			ODOMETRY_set(global.position.x, FIELD_SIZE_Y - calibration_forward_border_distance, PI4096/2);			//pas de changement en x.
		else if(angle32 >= PI4096/4*3 || angle32 <= -PI4096/4*3)
			ODOMETRY_set(calibration_forward_border_distance, global.position.y, PI4096);								//pas de changement en y.
		else //(angle32 >= -PI4096/4*3 && angle32 <= -PI4096/4)
			ODOMETRY_set(global.position.x, calibration_forward_border_distance, -(PI4096/2));							//pas de changement en x.
	}
}

#ifdef USE_GYROSCOPE
#define GYRO_BUFFER_SIZE	4096

static Sint32 ODOMETRY_get_speed_rotation_gyroway_corrected(void)
{
	static Uint8 loop = 0;
	static Sint32 buffer_gyro[GYRO_BUFFER_SIZE];		//Buffer contenant les derniers écarts mesurés entre gyro et roues codeuses.
	static Uint16 gyro_buffer_index = 0;
	static Uint16 gyro_buffer_nb = 0;
	static Sint32 sum_corrector_gyro = 0;
	static Sint32 corrector_gyro = 0;
	int temperature = ADXRS453_GetTemperature();
	float decalage = 17;
	float rapport = 4;

	static Sint32 gyro_teta = 0;
	bool_e gyro_valid = FALSE;
	Sint16 degre;
	Sint32 gyro_speed;


	gyro_speed = -GYRO_get_speed_rotation(&gyro_valid, TRUE);	//Le '-' permet d'avoir le même signe entre le gyro et l'odométrie.

	if(gyro_valid)
	{
		//Remplissage du buffer...
		if(global.real_speed_rotation <= 500 && global.real_speed_rotation >= -500) // Faible vitesse de rotation -> confiance accordée au roues codeuses
		{
			if(gyro_buffer_nb == GYRO_BUFFER_SIZE)
				sum_corrector_gyro -= buffer_gyro[gyro_buffer_index];					//On retire la correction la plus âgée...avant de l'écraser
			buffer_gyro[gyro_buffer_index] = gyro_speed - global.real_speed_rotation;	//On mémorise la correction actuelle
			sum_corrector_gyro += buffer_gyro[gyro_buffer_index];						//On ajoute la correction actuelle
			gyro_buffer_index = (gyro_buffer_index + 1) % GYRO_BUFFER_SIZE;				//On déplace l'index
			gyro_buffer_nb = MIN(gyro_buffer_nb + 1, GYRO_BUFFER_SIZE);					//On augmente le nombre, si pas déjà au max
			if(gyro_buffer_nb)															//Inutile... mais sage.
				corrector_gyro = sum_corrector_gyro/gyro_buffer_nb;						//Mise à jour du correcteur
		}

		gyro_teta += gyro_speed - corrector_gyro - (gyro_speed / (2*PI4096))*(temperature-decalage)*rapport;
		degre = ((gyro_teta / PI4096)*180) >> 10;
		if(!loop)
		{
			temperature = ADXRS453_GetTemperature();
			//debug_printf("gspeed:%6ld\tgcorrect:%6ld,gspeed_corrected%ld,gteta:%ld\tgdegre%3d\n",gyro_speed, corrector_gyro, gyro_speed - corrector_gyro, gyro_teta, degre);
			debug_printf("Gyro temperature is %d\n", temperature);
			loop = 50;
		}
		loop--;

		//Correction...
		return gyro_speed - corrector_gyro - (gyro_speed / (2*PI4096))*(temperature-decalage)*rapport;		//TEMPORAIRE... on fait confiance au gyro uniquement (corrigé d'une déviation estimée)
	}
	else
	{

		gyro_teta += global.real_speed_rotation;
		return global.real_speed_rotation;	//Gyro inaccessible :  aucune modif, confiance accordée aux roues codeuses.

	}
}
#endif

void ODOMETRY_update(void)
{
	Sint16 cos,sin; 	//[pas d'unité/4096]
	Sint32 cos32, sin32;
	Sint32 left, right;
	Sint32 left2, right2;
	//deviation occasionné par la force centrifuge
	Sint32 deviation_x;
	Sint32 deviation_y;
	Sint32 real_speed_x;	//[mm/65536/5ms]
	Sint32 real_speed_y;	//[mm/65536/5ms]

	//CALCUL PREALABLE...DES COS ET SIN...
	//ATTENTION... le calcul des x et y se fait avec l'angle de la précédente IT, on considère qu'on s'est déplacé avec l'angle précédent...
	//Calcul des cos et sin

	COS_SIN_16384_get(teta32 >> 8,&cos,&sin);

	cos32 = (Sint32)(cos);
	sin32 = (Sint32)(sin);

	ENCODERS_get(&left, &right);
	ENCODERS_get2(&left2,&right2);
	// CALCUL DES VITESSES REELLES	 (on multiplie toujours AVANT de diviser...)
	global.real_speed_rotation	= (Sint32)((-left*(coefs[ODOMETRY_COEF_ROTATION]+coefs[ODOMETRY_COEF_SYM]) + right*(coefs[ODOMETRY_COEF_ROTATION]-coefs[ODOMETRY_COEF_SYM])) >> 6);		//[rad/1024/4096/5ms] = [impulsions] * [rad/16/4096/1024/impulsions/5ms] * [16]

	global.real_speed_translation = (Sint32)(((left + right)*coefs[ODOMETRY_COEF_TRANSLATION]) >> 4 >> 1);	//[mm/4096/5ms] =  [impulsions + impulsions]*[mm/65536/impulsion/5ms]*[16]*[2]
	//le 4 pour remettre à la bonne unité (/16), le 1 pour la moyenne : (a+b)/2=(a+b)>>1
	//calcul de la vitesse de l'accéléromètre pour le gros robot. L'accéléromètre est au dessus de la roue codeuse gauche
	global.real_speed_translation_for_accelero = (Sint32)(((left)*coefs[ODOMETRY_COEF_TRANSLATION]) >> 4);

#ifdef USE_GYROSCOPE
	TIMER1_disableInt();
	global.real_speed_rotation = ODOMETRY_get_speed_rotation_gyroway_corrected();
	TIMER1_enableInt();
#endif

	//TODO : comparer speed avec global.real_speed_rotation produit ci-dessous

#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
#warning "Simulation avec robot virtuel parfait activée... ne pas utiliser ce code dans le robot sans volonté volontaire de nuire à la bonne pratique > robot en 'boucle ouverte' !"
	DEBUG_envoi_point_fictif_alteration_coordonnees_reelles();
#endif

	//vitesses X et Y
	//cos et sin sont exprimés en [pas d'unité/4096]
	real_speed_x =(global.real_speed_translation*cos32)>>8;		//[mm/65536/5ms] = [mm/4096/5ms]*[/4096]*[256]
	real_speed_y =(global.real_speed_translation*sin32)>>8;		//[mm/65536/5ms] = [mm/4096/5ms]*[/4096]*[256]

	real_speed_x /= 4;
	real_speed_y /= 4;

	//Explication, pourquoi xFin, yFin, vitesse_x_reelle et vitesse_y_reelle sont exprimés en [mm/65536]
	// limite haute
	// 	les coordonnées en x et en y doivent pouvoir être bien supérieures aux limites du terrain, c'est à dire, plus grandes que 3m
	//	par exemple : de -32768mm à +32768mm
	//	ainsi, l'unité minimale de xFin est le mm/65536, puisque pour un nombre de 32768mm, cela donne "xFin = le nombre maxi sous 32 bits."
	// limite basse
	// 	plus l'unité est faible et mieux c'est....
	//	explication : COEF_ODOMETRIE_TRANSLATION<<4<<1 vaut environ 700... le cosinus/sinus minimal vaut 1... on obtient ainsi un produit mini de 700... si on le divise trop, on perd en précision
	//	si on avait voulu utiliser l'unité [mm/4096] très présente ailleurs dans le code,
	//	ce 700 aurait été simplifié divisé par 8.... c'est à dire 2... c'est génant... surtout que l'on intègre les erreurs !
	// 	plus d'infos : samuelp5@gmail.com

	//calcul de la deviation du à l'action de la force centrifuge
	deviation_x = -(coefs[ODOMETRY_COEF_CENTRIFUGAL]*(global.real_speed_rotation/128)*(real_speed_y/128))/1024; //[rad/65536/5ms]*[mm/512/5ms]
	deviation_y = (coefs[ODOMETRY_COEF_CENTRIFUGAL]*(global.real_speed_rotation/128)*(real_speed_x/128))/1024; //[rad/65536/5ms]*[mm/512/5ms]
	//Ordre de grandeur : pour 1 unité du coef_odometrie_centrifuge, et à la vitesse max (rot et trans), on corrige de 1mm/secondes environ !
	//Donc si on a l'impression que le robot perd un centimètre quand il est à fond, et pendant 1 seconde, le coef aura probablement une réglage en dizaine...

	//Mise a jour de la position en x et y
	x32 += real_speed_x + deviation_x; 				//[mm/65536]
	y32 += real_speed_y + deviation_y;				//[mm/65536]
	global.position.x = x32 >> 16;		//[mm]
	global.position.y = y32 >> 16;		//[mm]
	//debug_printf("Position : x=%d  y=%d \n", global.position.x, global.position.y);
	//Mise à jour de l'angle
	teta32 += global.real_speed_rotation;	//[rad/1024/4096]

	//Mise à jour de la distance total parcourue
	total_dist_x += absolute(real_speed_x + deviation_x);		//[mm/65536]
	total_dist_y += absolute(real_speed_y + deviation_y);		//[mm/65536]

	//Mise à jour de l'angle total parcourue
	total_teta += global.real_speed_rotation;	//[rad/1024/4096]
	if(absolute(total_teta) > absolute(max_total_teta))
		max_total_teta = total_teta;			//[rad/1024/4096]

	//Gestion de l'angle modulo 2PI !!!
	if(teta32 < (-PI_22) )
		teta32 += TWO_PI22;
	if(teta32 > PI_22)
		teta32 -= TWO_PI22;

	global.position.teta = teta32 >> 10;	//[rad/4096]


	if((!COPILOT_is_arrived()) || (SUPERVISOR_get_state() == SUPERVISOR_ERROR))
	{	//Si je ne suis pas arrivé à destination, le référentiel me suit... et repart à zéro !)
		global.real_position_translation = 0;
		global.real_position_rotation = 0;
	}

	// translation réelle parcourue depuis la dernière RAZ du référentiel IT
	global.real_position_translation +=  global.real_speed_translation;  //[mm/4096]

	// rotation réelle parcourue depuis la dernière RAZ du référentiel IT
	global.real_position_rotation += global.real_speed_rotation;	//[rad/4096/1024]

}


Sint32 ODOMETRY_get_max_total_teta(){
	return max_total_teta >> 10;
}

Sint32 ODOMETRY_get_total_teta(){
	return total_teta >> 10;
}


Uint32 ODOMETRY_get_total_dist(){
	return (Uint32)sqrt(SQUARE(total_dist_x >> 16) + SQUARE(total_dist_y >> 16));
}

