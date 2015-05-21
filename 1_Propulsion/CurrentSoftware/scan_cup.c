#include "scan_cup.h"
#include "QS/QS_adc.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_maths.h"
#include "QS/QS_measure.h"
#include "supervisor.h"
#include "secretary.h"
#include "corrector.h"
#include "roadmap.h"
#include "_Propulsion_config.h"
#include "odometry.h"
#include "config/config_use.h"

#ifdef SCAN_CUP

//------------------------------------------------------------------------------------ Macro

#define square(x) ((float)(x)*(float)(x))
#define conversion_capteur_LEFT(x) ((Sint32)(550)*(x)/1000 - 69)
#define conversion_capteur_RIGHT(x) ((Sint32)(551)*(x)/1000 - 63)


//------------------------------------------------------------------------------------ Define

#define NB_POINT_MAX		250					//Max : 255
#define Y1					0
#define Y2					400
#define Y3					2600
#define Y4					3000
#define X1					400
#define X2					800
#define X3					1200
#define X4					1600
#define QUANTUM_MESURE		2		//Distance entre deux mesures
#define RADIUS_CUP			78
#define NB_POINT_MIN		4
#define NB_POINT_ELIM       3		// Nombre de points que l'on élimine à chaque extrémitée
#define BORDER				30
#define MARGE				-10
#define ECART_MAX			8
#define ONE_CUP				30      // Initialement : 48
#define TWO_CUP				60		// Initialement : 79
#define ZERO_CUP			10
#define DEBUG				0
#define ECART_MAX_Y			2


//------------------------------------------------------------------------------------ Définition des structures et énumerations

typedef struct{
	GEOMETRY_point_t robot;
	Uint16 dist;
}scan_result_t;


typedef enum{
	NO_MSG_CAN,
	MSG_CAN_SCAN_LINEAR,
	MSG_CAN_SCAN_SIDE
}receve_msg_can_e;

typedef struct{
	bool_e scan_linear;			// 1 : scan linéaire	0 : scan sur le coté
	bool_e is_in_North;			// 1 : salle nord		0 : salle sud
	color_e color;				// 1 : couleur verte	0 : couleur jaune
	bool_e is_right_sensor;		// 1 : capteur droite   0 : capteur gauche
}proporties_scan;


//------------------------------------------------------------------------------------ Variables Globales

static GEOMETRY_point_t salleH[NB_POINT_MAX];
static GEOMETRY_point_t salleB[NB_POINT_MAX];
static GEOMETRY_point_t salleDebug[NB_POINT_MAX];
static Uint8 nbPointH = 0;
static Uint8 nbPointB = 0;
static Uint8 nbPointDebug = 0;
static Uint32 old_measure = 0;
static Uint8 nb_cup = 0;
static Uint8 nb_cupB;
static Uint8 nb_cupH;
static GEOMETRY_point_t coorCup[5];			//Dans un premier temps il sert à stocker les indices des tableaux puis les coordonnées des gobelets
static bool_e run_calcul,end_scan;
static bool_e error_scan;
static bool_e cup_detected;
static scan_result_t mesure_en_cours;
static receve_msg_can_e receve_msg_can;
static proporties_scan info_scan;


//------------------------------------------------------------------------------------ Prototype des fonctions local

static void inArea(scan_result_t * objet);
static GEOMETRY_point_t determine_center(GEOMETRY_point_t tab[], Uint8 nb_points, Uint8 first);
static void removeWrongPoints(GEOMETRY_point_t tab[],Uint8 *nb_points);
static Uint8 isValidCenter(GEOMETRY_point_t salle[], Uint8 first, Uint8 end, GEOMETRY_point_t center);
static void detectMaxMinY(GEOMETRY_point_t salle[], Uint8 nbPoint, Uint8 *iMax, Uint8 *iMin);
static void cupNumber();
static void detectCenter();

//------------------------------------------------------------------------------------ Fonctions





void SCAN_CUP_init(void)
{
	ADC_init();				//Initialisation des convertisseurs analogiques numeriques
	run_calcul = FALSE;
}

//---------------------------------------------------------------------------------------------------------------
//												MAE Scan Cup
//---------------------------------------------------------------------------------------------------------------

void SCAN_CUP_process_it(){
	typedef enum{
		INIT=0,
		WAIT,
		SCAN_LINEAR,
		SCAN_SIDE,
		SCAN_CALCUL,
		WAIT_CALCULATE,
		SEND_COOR_CUP,
		END
	}state_e;
	static state_e state = INIT;
	Uint8 i;
	//scan_result_t mesure_en_cours;
	bool_e last_point=0;
	error_scan = 0;
	cup_detected = 0;
	switch(state){

		case INIT :
			//info_scan.color = ODOMETRY_get_color();
			//debug_printf("Couleur : %s",(info_scan.color)?"vert":"jaune");
			receve_msg_can = NO_MSG_CAN;
			state = WAIT;
			break;

		case WAIT :
			switch(receve_msg_can){
				case MSG_CAN_SCAN_LINEAR:
					old_measure = global.position.x;
					state = SCAN_LINEAR;
					break;

				case MSG_CAN_SCAN_SIDE:
					old_measure = global.position.y;
					state = SCAN_SIDE;
					break;

				case NO_MSG_CAN :
				default :
					break;
			}
			break;

		case SCAN_LINEAR:
			if((old_measure-global.position.x)*(old_measure-global.position.x) >= QUANTUM_MESURE*QUANTUM_MESURE){
				Sint16 teta = global.position.teta;
				//debug_printf("Scan Linear -> Couleur : %s",(info_scan.color)?"vert":"jaune");
				if(info_scan.color==YELLOW){
					if(teta < PI4096/2 && teta > -PI4096/2){ //L'angle vaut 0
						mesure_en_cours.dist = conversion_capteur_RIGHT(ADC_getValue(SCAN_CUP_SENSOR_RIGHT));
						info_scan.is_right_sensor = TRUE;
						//debug_printf("\n\nCouleur jaune : capteur droite\n\n");
						//debug_printf("\tValeur mesuree = %d\n",mesure_en_cours.dist);
					}else{ // L'angle vaut Pi
						mesure_en_cours.dist = conversion_capteur_LEFT(ADC_getValue(SCAN_CUP_SENSOR_LEFT));
						info_scan.is_right_sensor = FALSE;
						//debug_printf("\n\nCouleur jaune : capteur gauche\n\n");
						//debug_printf("\tValeur mesuree = %d\n",mesure_en_cours.dist);
					}
				}else{
					if(teta < PI4096/2 && teta > -PI4096/2){ //L'angle vaut 0
						mesure_en_cours.dist = conversion_capteur_LEFT(ADC_getValue(SCAN_CUP_SENSOR_LEFT));
						info_scan.is_right_sensor = FALSE;
						//debug_printf("\n\nCouleur vert : capteur gauche\n\n");
						//debug_printf("\tValeur mesuree = %d\n",mesure_en_cours.dist);
					}else{ // L'angle vaut Pi
						mesure_en_cours.dist = conversion_capteur_RIGHT(ADC_getValue(SCAN_CUP_SENSOR_RIGHT));
						info_scan.is_right_sensor = TRUE;
						//debug_printf("\n\nCouleur vert : capteur droite\n\n");
						//debug_printf("\tValeur mesuree = %d\n",mesure_en_cours.dist);
					}
				}
				mesure_en_cours.robot.x = global.position.x;
				mesure_en_cours.robot.y = global.position.y;
				old_measure = global.position.x;
				inArea(&mesure_en_cours);
			}
			if(end_scan == TRUE){		//On passe dans la phase de calcul
				state = SCAN_CALCUL;
			}
			break;

		case SCAN_SIDE:{
			//debug_printf("%lu >= %d    ->   %s\n",(old_measure-global.position.y)*(old_measure-global.position.y),QUANTUM_MESURE*QUANTUM_MESURE,((old_measure-global.position.y)*(old_measure-global.position.y) >= QUANTUM_MESURE*QUANTUM_MESURE)?"Vrai":"Faux");
			if((old_measure-global.position.y)*(old_measure-global.position.y) >= QUANTUM_MESURE*QUANTUM_MESURE){
				Sint16 ADCvalue = ADC_getValue(SCAN_CUP_SENSOR_RIGHT);
				//debug_printf("SCAN_SIDE ->  ADC = %d\n",ADCvalue);
				mesure_en_cours.dist = conversion_capteur_RIGHT(ADCvalue);
				/*if(color==YELLOW){
					if(is_in_square(0,800,0,1000,(GEOMETRY_point_t){global.position.x,global.position.y})){ // Il est au nord
						mesure_en_cours.dist = conversion_capteur_LEFT(ADC_getValue(SCAN_CUP_SENSOR_LEFT));
						debug_printf("\n\nCouleur jaune : capteur gauche\n\n");
					}else{ // Il est au sud
						mesure_en_cours.dist = conversion_capteur_RIGHT(ADC_getValue(SCAN_CUP_SENSOR_RIGHT));
						debug_printf("\n\nCouleur jaune : capteur droite\n\n");
					}
				}else{
					if(is_in_square(0,800,2000,3000,(GEOMETRY_point_t){global.position.x,global.position.y})){ // Il est au nord
						mesure_en_cours.dist = conversion_capteur_RIGHT(ADC_getValue(SCAN_CUP_SENSOR_RIGHT));
						debug_printf("\n\nCouleur vert : capteur droite\n\n");
					}else{ // Il est au sud
						mesure_en_cours.dist = conversion_capteur_LEFT(ADC_getValue(SCAN_CUP_SENSOR_LEFT));
						debug_printf("\n\nCouleur vert : capteur gauche\n\n");
					}
				}*/
				mesure_en_cours.robot.x = global.position.x;
				mesure_en_cours.robot.y = global.position.y;
				old_measure = global.position.y;
				inArea(&mesure_en_cours);
			}
			if(end_scan == TRUE){		//On passe dans la phase de calcul
				state = SCAN_CALCUL;
			}
		}break;

		case SCAN_CALCUL:
			run_calcul = TRUE;
			state = WAIT_CALCULATE;
			break;

		case WAIT_CALCULATE:
			if(DEBUG){
				debug_printf("###############################################################################\n");
				debug_printf("\tnbCup = %d\n",nb_cup);
				debug_printf("\tnbPointDebug = %d\n",nbPointDebug);
				for(i=0;i<nbPointDebug;i++){
					debug_printf("{%d,%d} ",salleDebug[i].x,salleDebug[i].y);
				}
				debug_printf("\n###############################################################################\n");
				debug_printf("X = [ ");
				for(i=0;i<nbPointDebug;i++){
					debug_printf("%d ",salleDebug[i].x);
				}
				debug_printf("]\n");
				debug_printf("Y = [ ");
				for(i=0;i<nbPointDebug;i++){
					debug_printf("%d ",salleDebug[i].y);
				}
				debug_printf("]\n");
				debug_printf("\n###############################################################################\n");
			}
			SCAN_CUP_calculate();
			if(!run_calcul)
				state = SEND_COOR_CUP;
			break;

		case SEND_COOR_CUP:
			if(nb_cup==0){
				state = END;
				SECRETARY_send_cup_position(1,0,0,0,0);
			}
			for(i=0;i<nb_cup;i++){
				if(i<nb_cup-2){
					last_point=FALSE;
				}else{
					last_point=TRUE;
					state = END;
				}
				if(DEBUG){
					debug_printf("###############################################################################\n");
					debug_printf("\tCentre : {%d,%d}\n",coorCup[i].x,coorCup[i].y);
					debug_printf("###############################################################################\n");
				}
				SECRETARY_send_cup_position(last_point,0,1,coorCup[i].x, coorCup[i].y);
			}
			break;

		case END:
			state = INIT;
			return;
			break;
	}
}


//---------------------------------------------------------------------------------------------------------------
//											Détermination des centres
//---------------------------------------------------------------------------------------------------------------


/*
 * Fonction qui à pour but d'éliminer des points à l'extérieur d'une zone.
 * On récupère seulement les points à l'intérieur d'une salle de cinéma.
 * MARGE permet de d'agrandir ou non la zone de prise en compte du point.
 *		MARGE > 0 : on agrandit la zone de détection
 *		MARGE = 0 : taille réelle de la salle de cinéma
 *		MARGE < 0 : on diminue la zone de détection
*/

static void inArea(scan_result_t * objet){
	GEOMETRY_point_t cup;
	//debug_printf("in Area -> Couleur : %s",(info_scan.color)?"vert":"jaune");
	if(info_scan.color==YELLOW){
		if(info_scan.is_right_sensor){
			if(info_scan.is_in_North){
				cup.x = objet->robot.x - 60;
				cup.y = objet->robot.y-objet->dist -100;
			}else{
				cup.x = objet->robot.x - 60;
				cup.y = objet->robot.y-objet->dist -100;
			}

		}else{
				if(info_scan.is_in_North){
					debug_printf("Salle verte nord\n");
					cup.x = objet->robot.x + 54;
					cup.y = objet->robot.y-objet->dist -60;  // -100  position 159
				}else{
					cup.x = objet->robot.x - 60;
					cup.y = objet->robot.y-objet->dist -100;
				}
		}
	}else{
		if(info_scan.is_right_sensor){
			if(info_scan.is_in_North){
				debug_printf("Salle jaune nord\n");
				cup.x = objet->robot.x + 50;      //Pour diminuer x, augmenter l'offset
				cup.y = 3000 - (objet->robot.y+objet->dist + 70);		//Pour daugmenter y, diminuer l'offset
			}else{
				cup.x = objet->robot.x + 18;
				cup.y = 3000 - (objet->robot.y+objet->dist + 116);
			}
		}else{
			if(info_scan.is_in_North){
				cup.x = objet->robot.x - 76;
				cup.y = 3000 - (objet->robot.y+objet->dist + 116);
			}else{
				debug_printf("Salle jaune sud\n");
				cup.x = objet->robot.x - 60;  // 40  pour 1500
				cup.y = 3000 - (objet->robot.y+objet->dist + 68);  // 104 pour 174
			}

		}

	}
	if(!info_scan.scan_linear){
		Sint16 aux = cup.x;
		cup.x = cup.y;
		cup.y = aux;
	}


	if(DEBUG){
		salleDebug[nbPointDebug].x=cup.x;
		salleDebug[nbPointDebug].y=cup.y;
		nbPointDebug++;
	}
	if(0/*info_scan.color*/){
		//debug_printf("InArea Couleur est vert\n");
		if(cup.x>=X1-MARGE && cup.x<=X2+MARGE && cup.y>=Y3-MARGE && cup.y<=Y4+MARGE){ //Salle de cinema du haut
			if(nbPointH<=NB_POINT_MAX){
				//debug_printf("Vert et haut (%d)\n",nbPointH);
				salleH[nbPointH] = cup;
				nbPointH++;
			}
		}
		if(cup.x>=X3-MARGE && cup.x<=X4+MARGE && cup.y>=Y3-MARGE && cup.y<=Y4+MARGE){ //Salle de cinema du bas
			if(nbPointB<=NB_POINT_MAX){
				//debug_printf("Vert et bas (%d)\n",nbPointB);
				salleB[nbPointB] = cup;
				nbPointB++;
			}
		}
	}else{
		//debug_printf("InArea Couleur est jaune\n");
		if(cup.x>=X1-MARGE && cup.x<=X2+MARGE && cup.y>=Y1-MARGE && cup.y<=Y2+MARGE){ //Salle de cinema du haut
			if(nbPointH<=NB_POINT_MAX){
				//debug_printf("Jaune et haut (%d)\n",nbPointH);
				salleH[nbPointH] = cup;
				nbPointH++;
			}
		}
		if(cup.x>=X3-MARGE && cup.x<=X4+MARGE && cup.y>=Y1-MARGE && cup.y<=Y2+MARGE){ //Salle de cinema du bas
			if(nbPointB<=NB_POINT_MAX){
				//debug_printf("Jaune et bas (%d)\n", nbPointB);
				salleB[nbPointB] = cup;
				nbPointB++;
			}
		}
	}
}

GEOMETRY_point_t determine_center(GEOMETRY_point_t tab[], Uint8 end, Uint8 first){
	Sint64 sumX=0, sumY=0, sumX2=0, sumY2=0, sumX3=0, sumY3=0, sumXY=0, sumX2Y=0, sumXY2=0; //les sommes de 0 à nb_points-1 des coordonnées. ex: sumX=somme de toutes les abscisses
	Sint64 c11=0, c20=0, c30=0, c21=0, c02=0, c03=0, c12=0; //des coefficients intermédiaires de calculs
	Uint8 i, nb_points = end-first+1;
	GEOMETRY_point_t centre;

	//Calcul des sommes
	for(i=first;i<=end; i++){
		sumX=sumX+tab[i].x;
		sumY=sumY+tab[i].y;
		sumX2=sumX2+puissance(tab[i].x,2);
		sumY2=sumY2+puissance(tab[i].y,2);
		sumX3=sumX3+puissance(tab[i].x,3);
		sumY3=sumY3+puissance(tab[i].y,3);
		sumXY=sumXY+tab[i].x*tab[i].y;
		sumX2Y=sumX2Y+puissance(tab[i].x,2)*tab[i].y;
		sumXY2=sumXY2+tab[i].x*puissance(tab[i].y,2);
	}

	//Calcul des coefficients
	c11=nb_points*sumXY-sumX*sumY;
	c20=nb_points*sumX2-sumX*sumX;
	c30=nb_points*sumX3-sumX2*sumX;
	c21=nb_points*sumX2Y-sumX2*sumY;
	c02=nb_points*sumY2-sumY*sumY;
	c03=nb_points*sumY3-sumY2*sumY;
	c12=nb_points*sumXY2-sumX*sumY2;

	//calcul du centre de la balise
	centre.x = ((c30+c12)*c02-(c03+c21)*c11)/(2.*(c20*c02-c11*c11));
	centre.y = ((c03+c21)*c20-(c30+c12)*c11)/(2.*(c20*c02-c11*c11));

	return centre;
}

static void removeWrongPoints(GEOMETRY_point_t tab[],Uint8 *nb_points){
	Uint8 i, nbPointSalle = 0;
	Uint8 pointDepart = 0;
	/*Uint8 pointDepart = NB_POINT_ELIM;
	(*nb_points) = (*nb_points) - (2*NB_POINT_ELIM);*/
	GEOMETRY_point_t salle[NB_POINT_MAX];
	if((*nb_points) > 1){
		// Cas particulier du premier point
		if(GEOMETRY_distance(tab[0],tab[1]) < ECART_MAX){
			salle[nbPointSalle] = tab[0];
			nbPointSalle++;
		}
		for(i=pointDepart;i<(*nb_points)-1;i++){
			if(GEOMETRY_distance(tab[i-1],tab[i]) < ECART_MAX || GEOMETRY_distance(tab[i],tab[i+1]) < ECART_MAX){
				if(i<(*nb_points)-2){
					if((tab[i-1].y-tab[i+2].y)*(tab[i-1].y-tab[i+2].y) >= ECART_MAX_Y){
						salle[nbPointSalle] = tab[i];
						nbPointSalle++;
					}
				}
			}
		}
		// Cas particulier du dernier point
		if(GEOMETRY_distance(tab[(*nb_points)-2],tab[(*nb_points)-1]) < ECART_MAX){
			salle[nbPointSalle] = tab[(*nb_points)-1];
			nbPointSalle++;
		}
	}
	*nb_points = nbPointSalle;
	for(i=0;i<nbPointSalle;i++){
		tab[i] = salle[i];
	}
}

static void cupNumber(){
	if(nbPointH <= ZERO_CUP)
		nb_cupH = 0;
	if(nbPointH > ZERO_CUP && nbPointH <= ONE_CUP)
		nb_cupH = 1;
	if(nbPointH > ONE_CUP && nbPointH <= TWO_CUP)
		nb_cupH = 2;
	if(nbPointH > TWO_CUP)
		nb_cupH = 3;

	if(nbPointB <= ZERO_CUP)
		nb_cupB = 0;
	if(nbPointB > ZERO_CUP && nbPointH <= ONE_CUP)
		nb_cupB = 1;
	if(nbPointB > ONE_CUP && nbPointH <= TWO_CUP)
		nb_cupB = 2;
	if(nbPointB > TWO_CUP)
		nb_cupB = 3;

	//debug_printf("Nombre de gobelet haut : %d\n",nb_cupH);
	//debug_printf("Nombre de gobelet bas : %d\n",nb_cupB);
}

static void detectCenterONE_CUP(GEOMETRY_point_t salle[], Uint8 nbPoint){
	GEOMETRY_point_t centre = determine_center(salle,nbPoint-1,0);
	coorCup[nb_cup].x = centre.x;
	coorCup[nb_cup].y = centre.y;
	nb_cup++;
}

static void detectCenterTWO_CUP(GEOMETRY_point_t salle[], Uint8 nbPoint){
	//On regarde si un gobelet cache une partie d'un autre
	Uint8 i,cache=TRUE,indice = 0,indice2 = 0,iMax,iMin;
	Sint16 aux;
	GEOMETRY_point_t centre;
	for(i=0;i<nbPoint-1;i++){
		if((salle[i].x-salle[i+1].x)*(salle[i].x-salle[i+1].x) > 100){
			cache = FALSE;
			indice = i;
			break;
		}
	}
	if(cache){  // Cas ou il y a un gobelet qui est cache par un autre
		detectMaxMinY(salle,nbPoint,&iMax,&iMin);
		if(iMin <= 15){
			indice = 0;
			indice2 = 0;
			for(i=nbPoint-1;i>0;i--){
				aux = (salle[nbPoint-1].x-salle[i-1].x)*(salle[nbPoint-1].x-salle[i-1].x);
				if(aux > RADIUS_CUP*RADIUS_CUP){
					indice = i;
					break;
				}
			}
			centre = determine_center(salle,nbPoint-1-NB_POINT_ELIM,indice+NB_POINT_ELIM);
			if(isValidCenter(salle, nbPoint-1-NB_POINT_ELIM, indice+NB_POINT_ELIM, centre)){
				coorCup[nb_cup].x = centre.x;
				coorCup[nb_cup].y = centre.y;
				nb_cup++;
			}
			for(i=indice-1;i>0;i--){
				aux = (salle[indice-1].x-salle[i-1].x)*(salle[indice-1].x-salle[i-1].x);
				if(aux > RADIUS_CUP*RADIUS_CUP){
					indice2 = i;
					break;
				}
			}
			centre = determine_center(salle,indice-1-NB_POINT_ELIM,indice2+NB_POINT_ELIM);
			if(isValidCenter(salle, indice-1-NB_POINT_ELIM, indice2+NB_POINT_ELIM, centre)){
				coorCup[nb_cup].x = centre.x;
				coorCup[nb_cup].y = centre.y;
				nb_cup++;
			}
		}
		if(iMin >= nbPoint-16){
			indice = nbPoint-1;
			indice2 = nbPoint-1;
			for(i=0;i<nbPoint-1;i++){
				aux = (salle[0].x-salle[i+1].x)*(salle[0].x-salle[i+1].x);
				if(aux > RADIUS_CUP*RADIUS_CUP){
					indice = i;
					break;
				}
			}
			centre = determine_center(salle,indice-NB_POINT_ELIM,0+NB_POINT_ELIM);
			if(isValidCenter(salle, 0+NB_POINT_ELIM, indice-NB_POINT_ELIM, centre)){
				coorCup[nb_cup].x = centre.x;
				coorCup[nb_cup].y = centre.y;
				nb_cup++;
			}
			for(i=indice+1;i<nbPoint-1;i++){
				aux = (salle[indice+1].x-salle[i+1].x)*(salle[indice+1].x-salle[i+1].x);
				if(aux > RADIUS_CUP*RADIUS_CUP){
					indice2 = i;
					break;
				}
			}
			centre = determine_center(salle,indice2-NB_POINT_ELIM,indice+1+NB_POINT_ELIM);
			if(isValidCenter(salle, indice+1+NB_POINT_ELIM, indice2-NB_POINT_ELIM, centre)){
				coorCup[nb_cup].x = centre.x;
				coorCup[nb_cup].y = centre.y;
				nb_cup++;
			}
		}
	}else{		// Cas ou aucun gobelet n'est cache
		centre = determine_center(salle,indice,0);
		if(isValidCenter(salle, 0, indice, centre)){
			coorCup[nb_cup].x = centre.x;
			coorCup[nb_cup].y = centre.y;
			nb_cup++;
		}
		centre = determine_center(salle,nbPoint-1,indice+1);
		if(isValidCenter(salle, indice+1, nbPoint-1, centre)){
			coorCup[nb_cup].x = centre.x;
			coorCup[nb_cup].y = centre.y;
			nb_cup++;
		}
	}
}

static void detectCenter(){
	if(nb_cupH == 0){

	}
	if(nb_cupH == 1){
		detectCenterONE_CUP(salleH,nbPointH);
	}
	if(nb_cupH == 2){
		detectCenterTWO_CUP(salleH,nbPointH);
	}
	if(nb_cupB == 0){

	}
	if(nb_cupB == 1){
		detectCenterONE_CUP(salleB,nbPointB);
	}
	if(nb_cupB == 2){
		detectCenterTWO_CUP(salleB,nbPointB);
	}
}

static void detectMaxMinY(GEOMETRY_point_t salle[], Uint8 nbPoint, Uint8 *iMax, Uint8 *iMin){
	Uint8 i;
	Sint16 max,min;
	if(info_scan.color){
		max = 3200;
		min = 2500;
		for(i=0;i<nbPoint;i++){
			if(salle[i].y < max){  // Il faut que le Y soit le plus petit
				max = salle[i].y;
				(*iMax) = i;
			}
			if(salle[i].y > min){
				min = salle[i].y;
				(*iMin) = i;
			}
		}
	}else{
		max = -200;
		min = 500;
		for(i=0;i<nbPoint;i++){
			if(salle[i].y > max){  // Il faut que le Y soit le plus grand
				max = salle[i].y;
				(*iMax) = i;
			}
			if(salle[i].y < min){
				min = salle[i].y;
				(*iMin) = i;
			}
		}
	}
}

static Uint8 isValidCenter(GEOMETRY_point_t salle[], Uint8 first, Uint8 end, GEOMETRY_point_t center){
	printf("%d < %d && %d < %d\n",GEOMETRY_distance(salle[first],center),RADIUS_CUP + 10,GEOMETRY_distance(salle[end],center),RADIUS_CUP + 10);
	if( (GEOMETRY_distance(salle[first],center) < (RADIUS_CUP + 10))  && (GEOMETRY_distance(salle[end],center) < (RADIUS_CUP + 10)) )
		return TRUE;
	return FALSE;
}

void SCAN_CUP_calculate(void){
	if(run_calcul){
		removeWrongPoints(salleH,&nbPointH);
		removeWrongPoints(salleB,&nbPointB);
		removeWrongPoints(salleH,&nbPointH);
		removeWrongPoints(salleB,&nbPointB);
		removeWrongPoints(salleH,&nbPointH);
		removeWrongPoints(salleB,&nbPointB);
		cupNumber();
		detectCenter();
	}
	run_calcul = FALSE;
	if(DEBUG){
		int i;
		debug_printf("###############################################################################\n");
		debug_printf("\tnbPointB = %d\n",nbPointB);
		debug_printf("\tnbPointH = %d\n",nbPointH);
		debug_printf("\tnb_cup = %d\n",nb_cup);
		debug_printf("###############################################################################\n");
		debug_printf("###############################################################################\n");
		debug_printf("\tSalle du Haut\n");
		for(i=0;i<nbPointH;i++){
			debug_printf("{%d,%d} ",salleH[i].x,salleH[i].y);
		}
		debug_printf("###############################################################################\n");
		debug_printf("\tSalle du Bas\n");
		for(i=0;i<nbPointH;i++){
			debug_printf("{%d,%d} ",salleH[i].x,salleH[i].y);
		}
		debug_printf("###############################################################################\n");
	}
}


//---------------------------------------------------------------------------------------------------------------
//											Traitement messages CAN
//---------------------------------------------------------------------------------------------------------------

void SCAN_CUP_canMsg(CAN_msg_t *msg){
	switch(msg->data[0]){
		case 1:
			receve_msg_can=MSG_CAN_SCAN_LINEAR;
			info_scan.scan_linear = TRUE;
			end_scan = FALSE;
			break;
		case 2:
			end_scan = TRUE;
			break;
		case 3:
			receve_msg_can=MSG_CAN_SCAN_SIDE;
			info_scan.scan_linear = FALSE;
			end_scan = FALSE;
			break;
	}
	if(msg->data[1]){
		info_scan.is_in_North = TRUE;
	}else{
		info_scan.is_in_North = FALSE;
	}
	if(msg->data[2]){
		info_scan.color = 1;
	}else{
		info_scan.color = 0;
	}
}

#endif
