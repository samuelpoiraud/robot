/*
 * scan_triangle.c
 *
 *  Created on: 11 oct. 2013
 *      Author: Arnaud
 */

#include "scan_triangle.h"
#include "QS/QS_adc.h"
#include "QS/QS_all.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"
#include "supervisor.h"
#include "secretary.h"
#include "corrector.h"
#include "roadmap.h"
#include "cos_sin.h"
#include "_Propulsion_config.h"
#include "pilot.h"
#include <math.h>

#ifdef SCAN_TRIANGLE

//------------------------------------------------------------------------------------ Macro

#define square(x) ((float)(x)*(float)(x))
#define conversion_DT10_mm(x) (((((Sint32)(597)*(x))/1024)- 78))
#define conversion_DT50_mm(x) (((((Sint32)(597)*(x))/1024)- 78))  // A �talonner
// �talonnage anti�rieur : ((((Sint32)(39970UL*(x)))>>16)- 56)
// Rapport sur la courbe DT10 couleur noir  y = 0,667*x - 119
// Rapport sur la courbe DT10 couleur blanc y = 0,583*x - 78,19

//------------------------------------------------------------------------------------ Define

#define X_SENSOR_BOT			-66
#define Y_SENSOR_BOT			90
#define X_SENSOR_MID			78
#define Y_SENSOR_MID			98
#define X_SENSOR_TOP			46
#define Y_SENSOR_TOP			98

#define COEF_DECTECTION_SEUIL	6

#define NB_POINTS_MAX			180

#define VITESSE_ANGULAIRE_SCAN_SLOW 20
#define VITESSE_ANGULAIRE_SCAN_FAST FAST

#define MARGE_SCAN_DECOR		30
#define MARGE_SCAN_ANYTHING		15

#define LONGUEUR_MIN_TRIANGLE	50
#define LONGUEUR_MIN_TORCHE		25
#define DIST_ARETE_MILIEU		37
#define RAYON_MIN_TORCHE		115
#define RAYON_MAX_TORCHE		145

//------------------------------------------------------------------------------------ Prototype des fonctions local

static void SCAN_TRIANGLE_in_process(Uint8 *n_mesure);
static void Acknowledge_Intern(void);

static bool_e est_dans_carre(Sint16 lx, Sint16 hx, Sint16 ly, Sint16 hy, Sint16 x, Sint16 y);
static bool_e est_dans_cercle(Sint16 x, Sint16 y, Sint16 x0, Sint16 y0, Sint16 rayon);
static Sint16 distance(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);
static float racine(float val);
static bool_e triangle_est_present();

//------------------------------------------------------------------------------------ D�finition des structures et �numerations

typedef struct{
	Uint8 indice_debut_point, indice_fin_point;
	bool_e active;
	bool_e torche;
	Sint16 longueur;
	struct{Sint16 x; Sint16 y;} point_milieu;
	struct{Sint16 x; Sint16 y;} milieu;
	struct{float x; float y;} vecteur;
	struct{float x; float y;} vecteur_Perpen;
}objet_s;

typedef struct{
	enum{ROTATE, LINEAR} type;
	Uint8 nb_points;
	position_t startPos;
	position_t endPos;
	ASSER_speed_e speed;
	way_e way;
}scan_param_s;

typedef enum{
	WARN_X_0, WARN_X_PI, WARN_Y_N, WARN_Y_P
}type_warner_e;

typedef struct{
	Uint8 number_triangle;
	type_warner_e type_warner;
	Sint16 warner;
	adc_id_e level;
}warner_param_s;

typedef enum{
	INIT=0,
	WAIT,

	PLACEMENT_TETA_SCAN_ROTATE,
	PLACEMENT_WAIT_TETA_SCAN_ROTATE,
	LAUNCH_WARNER_SCAN_ROTATE,
	IN_PROGRESS_SCAN_ROTATE,

	WARNER,

	WAIT_CALCULATE,
	SEND_MID_TRIANGLE,
	SEND_WARNER_REACH,
	TEST
}state_e;

typedef enum{
	NO_MSG_CAN,
	MSG_CAN_SCAN_ROTATE,
	MSG_CAN_SCAN_LINEAR,
	MSG_CAN_WARNER
}receve_msg_can_e;

typedef struct{
	type_warner_e type_warner;
	Uint8 level;
	Sint16 x;
	Sint16 y;
}triangle_localisation;

//------------------------------------------------------------------------------------ Variable Globale

static scan_param_s scan_param;
	// Variable contenant tout les param�tres du scan � effectuer

static warner_param_s warner_param;
	// Variable contenant tout les param�tres des warners � effectuer

volatile struct{Sint16 dist[3]; Sint16 x[3]; Sint16 y[3]; position_t pos;} scan[NB_POINTS_MAX];
	// Variable contenant la distance et les coordon�es x y de chaque points du scan

static objet_s objet[3][20];
	// Variable contenant les informations sur chaque objets diff�rent rep�r�s pendant le scan

static struct{Sint16 x; Sint16 y;} vect[3][NB_POINTS_MAX-1];
	// Variable contenant les vecteurs des entre-points des coordonn�es du scan

static Sint16 angle_vecteur[3][NB_POINTS_MAX-1];
	// Variable contenant les angles de chaque vecteurs

static Uint8 nb_objet[3];
	// Variable contenant le nombre d'objet par capteur

static struct{Sint8 lvl; Sint8 nb;} dernier_triangle_valide;
	// Variable contenant l'indice des derniers triangles valides ou -1 si non pr�sent

static receve_msg_can_e receve_msg_can;
	// Variable contenant le type de message CAN re�ut ou NO_MSG_CAN si aucun message CAN re�ut

static bool_e move_completed, run_calcul;
	// Flag indiquant si le mouvement est finit, si un calcul est en cours

static triangle_localisation position_triangle[]={
	{WARN_X_PI, ADC_SENSOR_SMALL_DT10_NV2, 800, 0},
	{WARN_Y_N, ADC_SENSOR_SMALL_DT10_NV2, 2000, 1300},
	{WARN_Y_N, ADC_SENSOR_SMALL_DT10_NV2, 2000, 1700},
	{WARN_X_0, ADC_SENSOR_SMALL_DT10_NV2, 800, 3000}
};
	// Tableau des triangles d�tectable avec la warner

#define NOMBER_MAX_TRIANGLE (sizeof(position_triangle)/sizeof(triangle_localisation))
	// Nombre de triangle pr�sent dans le tableau ci-dessus

static Sint16 distance_warner;
	// Distance mesur�e au coordonn�e du triangle mesur�

static bool_e anything_found = FALSE;
	// Flag qui est mis � TRUE lors d'un scan si celui-ci d�tecte quoi que ce soit lors du scan

//------------------------------------------------------------------------------------ Fonction

void SCAN_TRIANGLE_init(void){
	ADC_init();
	run_calcul = 0;
	// Initialisation des convertisseurs analogiques num�riques pour les DT10 de d�tection triangle
}

void SCAN_TRIANGLE_process_it(void){
	static state_e state = INIT;
	static Uint16 temp = 0;
	static Uint8 n_mesure = 0;
	static Uint16 time;
	static Uint8 i, j, k;

	switch(state){

		case INIT :
			move_completed = FALSE;
			run_calcul = FALSE;
			receve_msg_can = NO_MSG_CAN;
			nb_objet[0] = 0;
			nb_objet[1] = 0;
			nb_objet[2] = 0;
			dernier_triangle_valide.lvl = -1;
			dernier_triangle_valide.nb = -1;
			state = WAIT;
			break;

		case TEST :
			if(temp > 500){
					temp = 0;
					debug_printf("%d,%d,%d\n", (Sint16)ADC_getValue(ADC_SENSOR_SMALL_DT10_FLOOR), (Sint16)ADC_getValue(ADC_SENSOR_SMALL_DT10_NV1), (Sint16)ADC_getValue(ADC_SENSOR_SMALL_DT10_NV2));
					debug_printf("%d,%d,%d\n\n", (Sint16)conversion_DT10_mm(ADC_getValue(ADC_SENSOR_SMALL_DT10_FLOOR)), (Sint16)conversion_DT10_mm(ADC_getValue(ADC_SENSOR_SMALL_DT10_NV1)), (Sint16)conversion_DT10_mm(ADC_getValue(ADC_SENSOR_SMALL_DT10_NV2)));
				}
				else
					temp++;
			break;

		case WAIT :
			switch(receve_msg_can){
				case MSG_CAN_SCAN_ROTATE :
					state = PLACEMENT_TETA_SCAN_ROTATE;
					break;

				case MSG_CAN_SCAN_LINEAR :
					// A compl�ter
					break;

				case MSG_CAN_WARNER :
					state = WARNER;
					break;

				case NO_MSG_CAN :
				default :
					break;
			}
			break;

///-------------------------------------------------------------------------------------------------- Scan en rotation
		case PLACEMENT_TETA_SCAN_ROTATE :
			SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
			ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, scan_param.startPos.teta,
					NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
					SLOW, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			state = PLACEMENT_WAIT_TETA_SCAN_ROTATE;
			break;

		case PLACEMENT_WAIT_TETA_SCAN_ROTATE :
			if(move_completed){
				move_completed = FALSE;
				state = LAUNCH_WARNER_SCAN_ROTATE;
			}
			break;

		case LAUNCH_WARNER_SCAN_ROTATE :
			SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
			ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, scan_param.endPos.teta,
					NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
					scan_param.speed, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			state = IN_PROGRESS_SCAN_ROTATE;
			n_mesure = 0;
			break;

		case IN_PROGRESS_SCAN_ROTATE :
			if(absolute(global.position.teta - scan_param.startPos.teta)
					>= absolute((n_mesure+1)*(scan_param.endPos.teta - scan_param.startPos.teta)/scan_param.nb_points))
				SCAN_TRIANGLE_in_process(&n_mesure);

			if(move_completed){
				move_completed = FALSE;
				run_calcul = TRUE;
				time = 0;
				state = WAIT_CALCULATE;
			}
			break;

///-------------------------------------------------------------------------------------------------- Warner
		case WARNER :
			if(((warner_param.type_warner == WARN_X_0 || warner_param.type_warner == WARN_X_PI) && absolute(global.position.x - warner_param.warner) < (PILOT_get_coef(PILOT_TRANSLATION_SPEED_LIGHT)/4096)*2)
			   || ((warner_param.type_warner == WARN_Y_N || warner_param.type_warner == WARN_Y_P) && absolute(global.position.y - warner_param.warner) < (PILOT_get_coef(PILOT_TRANSLATION_SPEED_LIGHT)/4096)*2)){
				distance_warner = conversion_DT10_mm(ADC_getValue(warner_param.level));
				state = SEND_WARNER_REACH;
			}
			break;

///--------------------------------------------------------------------------------------------------
		case WAIT_CALCULATE :
			if(!run_calcul){
				debug_printf("Temps de calculs : %d\n", time);
				state = SEND_MID_TRIANGLE;
			}else
				time += 5;
			break;

		case SEND_MID_TRIANGLE :
			for(j=0;j<3;j++){
				k = 0;
				for(i=0;i<nb_objet[j];i++){
					if(objet[j][i].active){
						if(j == dernier_triangle_valide.lvl && i == dernier_triangle_valide.nb)
							SECRETARY_send_triangle_position(TRUE, j, k, objet[j][i].milieu.x, objet[j][i].milieu.y, (Sint16)(atan2(objet[j][i].vecteur.y, objet[j][i].vecteur.x)*PI4096/3.14));
						else
							SECRETARY_send_triangle_position(FALSE, j, k, objet[j][i].milieu.x, objet[j][i].milieu.y, (Sint16)(atan2(objet[j][i].vecteur.y, objet[j][i].vecteur.x)*PI4096/3.14));
						k++;
					}
				}
			}
			if(dernier_triangle_valide.lvl == -1 && dernier_triangle_valide.nb == -1)
				SECRETARY_send_triangle_position(TRUE, 0, 0, 0, 0, 0);
			SECRETARY_send_scan_anything(anything_found);
			state = INIT;
			break;

		case SEND_WARNER_REACH :
			SECRETARY_send_triangle_warner(triangle_est_present(), warner_param.number_triangle);
			state = INIT;
			break;
	}
}

static void SCAN_TRIANGLE_in_process(Uint8 *n_mesure){
	if((*n_mesure) < scan_param.nb_points){
		if(QS_WHO_AM_I_get() == BIG_ROBOT){
			scan[(*n_mesure)].dist[0] = conversion_DT10_mm(ADC_getValue(ADC_SENSOR_BIG_DT10_FLOOR));
			scan[(*n_mesure)].dist[1] = conversion_DT50_mm(ADC_getValue(ADC_SENSOR_BIG_DT50_NV1));
			scan[(*n_mesure)].dist[2] = conversion_DT50_mm(ADC_getValue(ADC_SENSOR_BIG_DT50_NV2));
		}else{
			scan[(*n_mesure)].dist[0] = conversion_DT10_mm(ADC_getValue(ADC_SENSOR_SMALL_DT10_FLOOR));
			scan[(*n_mesure)].dist[1] = conversion_DT10_mm(ADC_getValue(ADC_SENSOR_SMALL_DT10_NV1));
			scan[(*n_mesure)].dist[2] = conversion_DT10_mm(ADC_getValue(ADC_SENSOR_SMALL_DT10_NV2));
		}
		scan[*n_mesure].pos = global.position;
		(*n_mesure)++;
	}
}

void SCAN_TRIANGLE_calculate(void){
	if(run_calcul){
		debug_printf("Calcul\n");
		Uint8 k;
		Sint16 i, j;
		Sint16 cos, sin;
		bool_e in_objet = FALSE;
		anything_found = FALSE;

		// Initialisation
		for(i=0;i<20;i++)
			for(j=0;j<3;j++){
				objet[j][i].indice_debut_point = 0;
				objet[j][i].indice_fin_point = 0;
				objet[j][i].active = FALSE;
				objet[j][i].torche = FALSE;
				objet[j][i].longueur = 0;
				objet[j][i].milieu.x = 0;
				objet[j][i].milieu.y = 0;
				objet[j][i].vecteur.x = 0;
				objet[j][i].vecteur.y = 0;
			}

		// Calcul point x et y
		for(i=0;i<scan_param.nb_points;i++){									// Valid�
				COS_SIN_4096_get(scan[i].pos.teta, &cos, &sin);
				Sint32 cos32 = cos, sin32 = sin;
				scan[i].x[0] = scan[i].pos.x + X_SENSOR_BOT*cos32/4096 - (Y_SENSOR_BOT + scan[i].dist[0])*sin32/4096;
				scan[i].y[0] = scan[i].pos.y + X_SENSOR_BOT*sin32/4096 + (Y_SENSOR_BOT + scan[i].dist[0])*cos32/4096;
				scan[i].x[1] = scan[i].pos.x + X_SENSOR_MID*cos32/4096 - (Y_SENSOR_MID + scan[i].dist[1])*sin32/4096;
				scan[i].y[1] = scan[i].pos.y + X_SENSOR_MID*sin32/4096 + (Y_SENSOR_MID + scan[i].dist[1])*cos32/4096;
				scan[i].x[2] = scan[i].pos.x + X_SENSOR_TOP*cos32/4096 - (Y_SENSOR_TOP + scan[i].dist[2])*sin32/4096;
				scan[i].y[2] = scan[i].pos.y + X_SENSOR_TOP*sin32/4096 + (Y_SENSOR_TOP + scan[i].dist[2])*cos32/4096;
		}


		// Filtre des points x et y
		// Filtre � temps futur d'ordre 2 avec un poid de 2/3 pour le point courant et 1/3 pour le point suivant
		for(i=0;i<scan_param.nb_points-1;i++){						// TEST
			for(j=0;j<3;j++){
				scan[i].x[j] = (Sint16)(2./3.*(float)scan[i].x[j] + 1./3.*(float)scan[i+1].x[j]);
				scan[i].y[j] = (Sint16)(2./3.*(float)scan[i].y[j] + 1./3.*(float)scan[i+1].y[j]);
			}
		}

		// Filtre reverse des points x et y
		// Filtre � temps futur d'ordre 2 avec un poid de 2/3 pour le point courant et 1/3 pour le point suivant
		for(i=scan_param.nb_points-1;i>0;i--){						// TEST
			for(j=0;j<3;j++){
				scan[i].x[j] = (Sint16)(2./3.*(float)scan[i].x[j] + 1./3.*(float)scan[i-1].x[j]);
				scan[i].y[j] = (Sint16)(2./3.*(float)scan[i].y[j] + 1./3.*(float)scan[i-1].y[j]);
			}
		}

		// Calcul vecteur de chaque entre points
		for(i=0;i<scan_param.nb_points-1;i++){						// Valid�
			for(j=0;j<3;j++){
				vect[j][i].x = scan[i+1].x[j] - scan[i].x[j];
				vect[j][i].y = scan[i+1].y[j] - scan[i].y[j];
				if(vect[j][i].x == 0 && vect[j][i].y == 0){
					if(i == 0){
						vect[j][i].x = 1;
						vect[j][i].y = 1;
					}else{
						vect[j][i].x = vect[j][i-1].x;
						vect[j][i].y = vect[j][i-1].y;
					}
				}
			}
		}

		// Calcul de l'angle entre deux vecteurs
		for(i=0;i<scan_param.nb_points-1;i++){						// Valid�
			for(j=0;j<3;j++){
				angle_vecteur[j][i] = (Sint16)(atan2(vect[j][i].y, vect[j][i].x)*180/3.14);
			}
		}

		// filtrer temp futur
		for(i=0;i<scan_param.nb_points-1;i++){						// Valid�
			for(j=0;j<3;j++){
				if(i==scan_param.nb_points-3){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i+1])/2;
				}else if(i==scan_param.nb_points-4){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i+1] + angle_vecteur[j][i+2])/3;
				}else if (i!=scan_param.nb_points-2){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i+1] + angle_vecteur[j][i+2] +  angle_vecteur[j][i+3])/4;
				}
			}
		}

		// filtrer reverse temp futur
		for(i=scan_param.nb_points-2;i>=0;i--){					// Valid�
			for(j=0;j<3;j++){
				if(i==1){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1])/2;
				}else if(i==2){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1] + angle_vecteur[j][i-2])/3;
				}else if(i!=0){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1] + angle_vecteur[j][i-2] +  angle_vecteur[j][i-3])/4;
				}
			}
		}

		// d�riv�e
		for(i=0;i<scan_param.nb_points-2;i++){						// Valid�
			for(j=0;j<3;j++){
				angle_vecteur[j][i] -= angle_vecteur[j][i+1];
			}
		}

		// seuil changement brutal
		for(i=0;i<scan_param.nb_points-2;i++){						// Valid�
			for(j=0;j<3;j++){
				if((int)(absolute(angle_vecteur[j][i])) > COEF_DECTECTION_SEUIL){
					angle_vecteur[j][i] = 1;
				}else{
					angle_vecteur[j][i] = 0;
				}
			}
		}

		// Detection d'objet
		for(j=0;j<3;j++){						// Valid�
			in_objet = FALSE;
			for(i=0;i<scan_param.nb_points-1;i++){
				if(nb_objet[j] < 20){
					if(i==0){
						objet[j][nb_objet[j]].indice_debut_point = i;
						in_objet = TRUE;
					}else if(angle_vecteur[j][i-1] == 0 && angle_vecteur[j][i] == 1){
						objet[j][nb_objet[j]].indice_fin_point = i;
						nb_objet[j]++;
						in_objet = FALSE;
					}
					else if(angle_vecteur[j][i-1] == 1 && angle_vecteur[j][i] == 0){
						objet[j][nb_objet[j]].indice_debut_point = i;
						in_objet = TRUE;
					}
				}
			}
			if(in_objet){
				objet[j][nb_objet[j]].indice_fin_point = i;
				nb_objet[j]++;
			}
		}

		// S�paration des objets trop �loign�s
		for(j=0;j<3;j++){								// Valid�
			for(k=0;k<nb_objet[j];k++){
				if(nb_objet[j] < 20){
					for(i=objet[j][k].indice_debut_point;i<=objet[j][k].indice_fin_point;i++){
						if(distance(scan[i].x[j], scan[i].y[j], scan[i+1].x[j], scan[i+1].y[j]) > 25){
							objet[j][nb_objet[j]].indice_fin_point = objet[j][k].indice_fin_point;
							objet[j][nb_objet[j]].indice_debut_point = i+1;
							objet[j][k].indice_fin_point = i;
							nb_objet[j]++;
						}
					}
				}
			}
		}

		// Retrait des objets o� il ne faut pas faire de traitement
		for(j=0;j<3;j++){										// Valid�
			for(k=0;k<nb_objet[j];k++){
				objet[j][k].active = FALSE;
				for(i=objet[j][k].indice_debut_point;i<=objet[j][k].indice_fin_point;i++){
					if(    (j == 0 && !est_dans_cercle(scan[i].x[j], scan[i].y[j], 1050, 1500, 150 + MARGE_SCAN_DECOR)				// Foyer centre
								   && !est_dans_cercle(scan[i].x[j], scan[i].y[j], 2000, 0, 250 + MARGE_SCAN_DECOR)					// Foyer droite
								   && !est_dans_cercle(scan[i].x[j], scan[i].y[j], 2000, 3000, 250 + MARGE_SCAN_DECOR)				// Foyer gauche
								   && !est_dans_carre(0-MARGE_SCAN_DECOR, 300+MARGE_SCAN_DECOR,
											 400-MARGE_SCAN_DECOR, 1100+MARGE_SCAN_DECOR, scan[i].x[j], scan[i].y[j])				// Bac � fruit jaune
								   && !est_dans_carre(0-MARGE_SCAN_DECOR, 300+MARGE_SCAN_DECOR,
											 1900-MARGE_SCAN_DECOR, 2600+MARGE_SCAN_DECOR, scan[i].x[j], scan[i].y[j])				// Bac � fruit rouge
								   && est_dans_carre(0+MARGE_SCAN_DECOR, 2000-MARGE_SCAN_DECOR, 0+MARGE_SCAN_DECOR,
													 3000-MARGE_SCAN_DECOR, scan[i].x[j], scan[i].y[j]))							// Terrain

						|| (j == 1 && !est_dans_carre(0-MARGE_SCAN_DECOR, 300+MARGE_SCAN_DECOR,
											 400-MARGE_SCAN_DECOR, 1100+MARGE_SCAN_DECOR, scan[i].x[j], scan[i].y[j])				// Bac � fruit jaune
								   && !est_dans_carre(0-MARGE_SCAN_DECOR, 300+MARGE_SCAN_DECOR,
											 1900-MARGE_SCAN_DECOR, 2600+MARGE_SCAN_DECOR, scan[i].x[j], scan[i].y[j])				// Bac � fruit rouge
								   && est_dans_carre(0+MARGE_SCAN_DECOR, 2000-MARGE_SCAN_DECOR, 0+MARGE_SCAN_DECOR,
													 3000-MARGE_SCAN_DECOR, scan[i].x[j], scan[i].y[j]))							// Terrain

						|| (j == 2 && est_dans_carre(0+MARGE_SCAN_DECOR, 2000-MARGE_SCAN_DECOR, 0+MARGE_SCAN_DECOR,
													 3000-MARGE_SCAN_DECOR, scan[i].x[j], scan[i].y[j])))							// Terrain
						objet[j][k].active = TRUE;
				}
			}
		}

		// Calcul de la longueur de chaque objet
		// Et test des objets probablement des triangles
		for(j=0;j<3;j++){									// Valid�
			for(i=0;i<nb_objet[j];i++){
				if(objet[j][i].active){
					objet[j][i].longueur = distance(scan[objet[j][i].indice_debut_point].x[j], scan[objet[j][i].indice_debut_point].y[j],
							scan[objet[j][i].indice_fin_point].x[j], scan[objet[j][i].indice_fin_point].y[j]);
					if((objet[j][i].longueur < LONGUEUR_MIN_TRIANGLE && j!=2) || (objet[j][i].longueur < LONGUEUR_MIN_TORCHE && j==2))
						objet[j][i].active = FALSE;
				}
			}
		}

		// vecteurs unitaires d'un objet avec moyennage des vecteurs de l'objet
		for(j=0;j<2;j++){																// Valid�
			for(i=0;i<nb_objet[j];i++){
				if(objet[j][i].longueur){
					for(k=objet[j][i].indice_debut_point;k<objet[j][i].indice_fin_point;k++){
						objet[j][i].vecteur.x += vect[j][k].x;
						objet[j][i].vecteur.y += vect[j][k].y;
					}
					objet[j][i].vecteur.x = objet[j][i].vecteur.x/((float)(objet[j][i].longueur));
					objet[j][i].vecteur.y = objet[j][i].vecteur.y/((float)(objet[j][i].longueur));
				}
			}
		}

		// Calcul du point milieu de chaque objet
		for(j=0;j<2;j++){								// Valid�
			for(i=0;i<nb_objet[j];i++){
				objet[j][i].point_milieu.x = (scan[objet[j][i].indice_debut_point].x[j] + scan[objet[j][i].indice_fin_point].x[j])/2;
				objet[j][i].point_milieu.y = (scan[objet[j][i].indice_debut_point].y[j] + scan[objet[j][i].indice_fin_point].y[j])/2;
			}
		}

		// vecteur unitaire perpendiculaire
		for(j=0;j<2;j++){								// Valid�
			for(i=0;i<nb_objet[j];i++){
				objet[j][i].vecteur_Perpen.y = objet[j][i].vecteur.x;
				objet[j][i].vecteur_Perpen.x = -objet[j][i].vecteur.y;
			}
		}

		// Calcul milieu triangle
		for(j=0;j<2;j++){								// Valid�
			for(i=0;i<nb_objet[j];i++){
				objet[j][i].milieu.x = objet[j][i].point_milieu.x + DIST_ARETE_MILIEU * objet[j][i].vecteur_Perpen.x;
				objet[j][i].milieu.y = objet[j][i].point_milieu.y + DIST_ARETE_MILIEU * objet[j][i].vecteur_Perpen.y;
			}
		}

		// Concat�nation des centres des triangles
		for(j=0;j<2;j++){								// Valid�
			for(i=0;i<nb_objet[j];i++){
				for(k=0;k<nb_objet[j];k++){
					if(k != i && distance(objet[j][i].milieu.x, objet[j][i].milieu.y, objet[j][k].milieu.x, objet[j][k].milieu.y) < 50
							&& objet[j][i].active && objet[j][k].active){
						objet[j][k].active = FALSE;
						objet[j][i].milieu.x = (objet[j][i].milieu.x + objet[j][k].milieu.x)/2;
						objet[j][i].milieu.y = (objet[j][i].milieu.y + objet[j][k].milieu.y)/2;
					}
				}
				if(objet[j][i].active){
					dernier_triangle_valide.lvl = j;
					dernier_triangle_valide.nb = i;
				}
			}
		}

		// D�tection d'une chose quelconque dans la zone scann�e
		for(i=0;i<scan_param.nb_points;i++){
			if(!est_dans_cercle(scan[i].x[0], scan[i].y[0], 1050, 1500, 150 + MARGE_SCAN_ANYTHING)					// Foyer centre
				&& !est_dans_cercle(scan[i].x[0], scan[i].y[0], 2000, 0, 250 + MARGE_SCAN_ANYTHING)				// Foyer droite
				&& !est_dans_cercle(scan[i].x[0], scan[i].y[0], 2000, 3000, 250 + MARGE_SCAN_ANYTHING)				// Foyer gauche
				&& !est_dans_carre(0-MARGE_SCAN_ANYTHING, 300+MARGE_SCAN_ANYTHING,
						400-MARGE_SCAN_ANYTHING, 1100+MARGE_SCAN_ANYTHING, scan[i].x[0], scan[i].y[0])				// Bac � fruit jaune
				&& !est_dans_carre(0-MARGE_SCAN_ANYTHING, 300+MARGE_SCAN_ANYTHING,
						1900-MARGE_SCAN_ANYTHING, 2600+MARGE_SCAN_ANYTHING, scan[i].x[0], scan[i].y[0])				// Bac � fruit rouge
				&& est_dans_carre(0+MARGE_SCAN_ANYTHING, 2000-MARGE_SCAN_ANYTHING, 0+MARGE_SCAN_ANYTHING,
						3000-MARGE_SCAN_ANYTHING, scan[i].x[0], scan[i].y[0])){									// Terrain
				anything_found = TRUE;
				break;
			}
		}

		/*
		for(i=0;i<scan_param.nb_points;i++)
			for(j=0;j<scan_param.nb_points;j++)
				for(k=0;k<scan_param.nb_points;k++){
					if(est_dans_cercle(scan[i].x[0], scan[i].y[0], scan[k].x[2], scan[k].y[2], 20)
							&& est_dans_cercle(scan[i].x[1], scan[i].y[1], scan[k].x[2], scan[k].y[2], 20)){}

				}

		// D�tection de torche
		for(j=0;j<3;j++){
			for(i=0;i<nb_objet[j];j++){
				if(objet[j][i].active){
					objet[j][i].torche = TRUE;
					for(k=objet[j][i].indice_debut_point;k<=objet[j][i].indice_fin_point;k++){
						if(EstDansLeCercle(scan[k].x[j], scan[k].y[j], objet[j][i].milieu.x, objet[j][i].milieu.y, RAYON_MIN_TORCHE)
								|| !EstDansLeCercle(scan[k].x[j], scan[k].y[j], objet[j][i].milieu.x, objet[j][i].milieu.y, RAYON_MAX_TORCHE))
							objet[j][i].torche = FALSE;
					}
				}
			}
		}*/

	#ifdef SOFT_SCAN_TRIANGLE
		debug_printf("\n###-DEBUT_SCAN-###\n");
		for(k=0;k<3;k++){
			debug_printf("#Point\n");
			debug_printf("%d\n", k);
			for(i=0;i<90;i++){
				debug_printf("%d;%d\n", scan[i].x[k], scan[i].y[k]);
			}
			debug_printf("#End_Point\n");
		}
		for(k=0;k<3;k++){
			for(i=0;i<nb_objet[k];i++){
				if(objet[k][i].active){
					debug_printf("#Objet\n");
					debug_printf("%d;%d\n", k, i);
					for(j=objet[k][i].indice_debut_point;j<=objet[k][i].indice_fin_point;j++){
						debug_printf("%d;%d\n", scan[j].x[k], scan[j].y[k]);
					}
					debug_printf("#End_Objet\n");
				}
			}

			for(i=0;i<nb_objet[k];i++){
				if(objet[k][i].active){
					debug_printf("#PointMilieu\n");
					debug_printf("%d;%d\n", k, i);
					debug_printf("%d;%d\n", objet[k][i].point_milieu.x, objet[k][i].point_milieu.y);
					debug_printf("#End_PointMilieu\n");
				}
			}

			for(i=0;i<nb_objet[k];i++){
				if(objet[k][i].active){
					debug_printf("#Milieu\n");
					debug_printf("%d;%d\n", k, i);
					debug_printf("%d;%d\n", objet[k][i].milieu.x, objet[k][i].milieu.y);
					debug_printf("#End_Milieu\n");
				}
			}

			/*for(i=0;i<nb_objet[k];i++){
				if(objet[k][i].torche){
					debug_printf("#Torche");
					debug_printf("%d;%d\n", k, i);
				}
			}*/
		}
		debug_printf("#Robot\n");
		debug_printf("%d;%d\n", global.position.x, global.position.y);
		debug_printf("###-FIN_SCAN-###\n");
	#endif
		run_calcul = FALSE;
	}
}

void SCAN_TRIANGLE_canMsg(CAN_msg_t *msg){
	if(msg->data[0] & 0x80)
		scan_param.type = LINEAR;
	else
		scan_param.type = ROTATE;

	if(msg->data[0] & 0x40)
		scan_param.speed = VITESSE_ANGULAIRE_SCAN_FAST;
	else
		scan_param.speed = VITESSE_ANGULAIRE_SCAN_SLOW;

	if(msg->data[0] & 0x20)
		scan_param.way = BACKWARD;
	else
		scan_param.way = FORWARD;

	scan_param.nb_points = (msg->data[1] > NB_POINTS_MAX)? NB_POINTS_MAX : msg->data[1];

	if(scan_param.type == ROTATE){
		scan_param.startPos.teta = (((Sint16)(msg->data[2]) << 8) & 0xFF00) | ((Sint16)(msg->data[3]) & 0x00FF);
		scan_param.endPos.teta = (((Sint16)(msg->data[4]) << 8) & 0xFF00) | ((Sint16)(msg->data[5]) & 0x00FF);
	}else{
		scan_param.startPos.x = ((Sint16)(msg->data[2]) << 8) & 0xFF00;
		scan_param.startPos.y = ((Sint16)(msg->data[3]) << 8) & 0xFF00;
		scan_param.endPos.x = ((Sint16)(msg->data[4]) << 8) & 0xFF00;
		scan_param.endPos.y = ((Sint16)(msg->data[5]) << 8) & 0xFF00;
	}
	if(scan_param.type == ROTATE)
		receve_msg_can = MSG_CAN_SCAN_ROTATE;
	else
		receve_msg_can = MSG_CAN_SCAN_LINEAR;
}

void SCAN_TRIANGLE_WARNER_canMsg(CAN_msg_t *msg){
	warner_param.number_triangle = msg->data[0];
	if(warner_param.number_triangle < NOMBER_MAX_TRIANGLE){
		warner_param.level = position_triangle[warner_param.number_triangle].level;
		warner_param.type_warner = position_triangle[warner_param.number_triangle].type_warner;
		warner_param.warner = (warner_param.type_warner == WARN_X_0
							   || warner_param.type_warner == WARN_X_PI) ? position_triangle[warner_param.number_triangle].y
																		 : position_triangle[warner_param.number_triangle].x;
		receve_msg_can = MSG_CAN_WARNER;
	}
}

static bool_e est_dans_carre(Sint16 lx, Sint16 hx, Sint16 ly, Sint16 hy, Sint16 x, Sint16 y){
	return x > lx && x < hx && y > ly && y < hy;
}

float racine(float val){
	if(val >= 0)
		return sqrt(val);
	else{
		#warning "Debug printf dans une fonction de calcul"
		debug_printf("######Racine d'une valeur n�gative ! val = %d######\n", (int)val);
		return 0;
	}

}

static void Acknowledge_Intern(void){
	move_completed = 1;
}

static Sint16 distance(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2){
	return racine(square((Sint32)(y1 - y2)) + square((Sint32)(x1 - x2)));
}

static bool_e est_dans_cercle(Sint16 x, Sint16 y, Sint16 x0, Sint16 y0, Sint16 rayon){
	return square(x-x0) + square(y-y0) <= square(rayon);
}

static bool_e triangle_est_present(){
	if((warner_param.type_warner == WARN_X_0 && global.position.y + distance_warner > 3000)
			|| (warner_param.type_warner == WARN_X_PI && global.position.y - distance_warner < 0)
			|| (warner_param.type_warner == WARN_Y_N && global.position.x + distance_warner > 2000)
			|| (warner_param.type_warner == WARN_Y_P && global.position.x - distance_warner < 0))
		return TRUE;
	else
		return FALSE;
}

#endif
