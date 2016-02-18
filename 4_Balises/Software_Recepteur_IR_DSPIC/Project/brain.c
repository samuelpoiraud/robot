/*
 *	Club Robot ESEO 2009 - 2010
 *	
 *
 *	Fichier : balise_localisation.c
 *	Package : Balises 2010
 *	Description : fonctions permettant la localisation du robot adverse � partir des donn�es de r�ceptions sur un cycle.
 *	Auteur : Nirgal
 *	Version 201002
 */

#define BRAIN_C
#include "brain.h"
#include "eyes.h"
#include "Secretary.h"
#include "balise_config.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"


#define OFFSET_ANGLE	(-PI4096/2)	//2014 : l'angle 0 est orient� vers la droite... c'est malin !
#define	NUMBER_OF_BEST_DETECTIONS (1)

//Une vue d'un adversaires est : une bonne d�tection de cet adversaire qui a eu lieu pendant les 100ms de mesure, 
//									assorti de caract�ristiques calcul�es, interm�diaires et permettant le calcul de sa position.
typedef struct
{
	detection_t *	detection;
	Uint8			main_tsop[3];
	Uint8			left_tsop;
	Uint8			right_tsop;
	Uint8			size;
	Uint16			distance;
	Sint16			angle;
}BRAIN_adversary_view_t;

static volatile BRAIN_adversary_view_t views[NUMBER_OF_BEST_DETECTIONS];	//Pointeurs vers les meilleurs d�tections des 100ms.
volatile EYES_adversary_detections_t * next_adversary_detection;
static volatile adversary_location_t adversary_location[ADVERSARY_NUMBER];

void BRAIN_process_treatment_detections(volatile EYES_adversary_detections_t * adversary_detection);
error_t BRAIN_find_three_main_tsop(Uint8 i);
error_t BRAIN_find_main_tsop(Uint8 i);
error_t BRAIN_find_left_right_tsop(Uint8 i);
error_t BRAIN_find_size(Uint8 i);
error_t BRAIN_find_angle(Uint8 i);
error_t BRAIN_find_distance(Uint8 i);
Uint16 	BRAIN_find_mediane_distance(void);
Sint16 	BRAIN_find_mediane_angle(void);
Sint16 calcul_recentrage_angle(Sint16 angle);


void BRAIN_init(void)
{
	Uint8 i;
	next_adversary_detection = 0;
	for(i = 0; i<ADVERSARY_NUMBER; i++)
	{
		adversary_location[i].distance = 0;
		adversary_location[i].past_distance = 0;
		adversary_location[i].angle = 0;
		adversary_location[i].past_angle = 0;
		adversary_location[i].error = AUCUN_SIGNAL;
	}
}

void BRAIN_task_add(volatile EYES_adversary_detections_t * adversary_detection)
{
	next_adversary_detection = adversary_detection;
}


static Sint16 BRAIN_modulo_angle(Sint16 angle)
{
	while(angle > PI4096)
		angle -= 2*PI4096;
	while(angle < -PI4096)
		angle += 2*PI4096;

	return angle;
}

void BRAIN_process_main(void)
{
	Uint8 i;
	if(next_adversary_detection != NULL)
	{
		//Il y a une p�riode de 100ms � traiter ?

		volatile EYES_adversary_detections_t * adversary_traitement = next_adversary_detection;
		Uint8 adv_number = next_adversary_detection->adversary;

		next_adversary_detection = NULL;

		//TRAITEMENT DES DONNEES RECUES (on trouve les plus grandes d�tections).
		BRAIN_process_treatment_detections(adversary_traitement);

		//for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
		//{
		//Chacune des meilleurs d�tections est analys�e ind�pendamment. On construit "n VUES" � partir de "n DETECTIONS".
		//adversary_traitement->error |= BRAIN_find_main_tsop(0);

		adversary_traitement->error |= BRAIN_find_three_main_tsop(0);
		adversary_traitement->error |= BRAIN_find_left_right_tsop(0);
		adversary_traitement->error |= BRAIN_find_distance(0);
		adversary_traitement->error |= BRAIN_find_size(0);
		adversary_traitement->error |= BRAIN_find_angle(0);
		//}
		/////////////////////////////////////////////////////////////////////
		//M�dianne des angles et distances obtenues.
		adversary_location[adv_number].distance 	= BRAIN_find_mediane_distance();
		adversary_location[adv_number].angle		= BRAIN_modulo_angle(BRAIN_find_mediane_angle() + OFFSET_ANGLE);
		adversary_location[adv_number].error		= adversary_traitement->error;

		/////////////////////////////////////////////////////////////////////
		//Moyennage de l'angle et de la distance
		//adversary_location[adv_number].past_angle		= adversary_location[adv_number].angle		= BRAIN_modulo_angle((3*adversary_location[adv_number].past_angle + adversary_location[adv_number].angle) >> 2);
		//adversary_location[adv_number].past_distance	= adversary_location[adv_number].distance	= (3*adversary_location[adv_number].past_distance + adversary_location[adv_number].distance) >> 2;

		SECRETARY_set_new_datas_available();

		#ifdef DEBUG_TSOP_MATLAB
		if(adv_number == 1){
			debug_printf("m%d\n", adversary_location[adv_number].distance);
			debug_printf("%d\n", adversary_location[adv_number].angle);
			for(i=0;i<16;i++)
				debug_printf("%d ", views[0].detection->counts[i]);
			debug_printf("\n");
		}
		#endif

		//DONNEES UTILISEES ET NETTOYABLES... ON REINITIALISE POUR LES PROCHAINES 100ms
		EYES_init_adversary_detection(adversary_traitement);
	}
}


volatile adversary_location_t * BRAIN_get_adversary_location(void)
{   
	return adversary_location;
}	
	



//Traitement des donn�es issues d'un adversaire. (100ms de donn�es sont trait�es ici...)
//Ce code sera ex�cut� avec une priorit� faible (t�che de fond ou IT secondaire...)	
void BRAIN_process_treatment_detections(volatile EYES_adversary_detections_t * adversary_detection)
{
	volatile detection_t	* detections;

	volatile time16_t duration, max_duration;
	volatile Uint8 i,max_index;

	max_duration = 0;
	max_index = 0;

	detections = (detection_t*)adversary_detection->detections;

	//Recherche de la plus longue d�tection...
	for(i = 0 ; i <= adversary_detection->index ; i++)
	{
		duration = detections[i].end - detections[i].begin;
		if(duration > max_duration)
		{
			max_duration = duration;
			max_index = i;
		}
	}

	//Sauvegarde des meilleures d�tections. (on r�cup�re leurs pointeurs...)
	views[0].detection = (detection_t *)&(detections[max_index]);
}
	

error_t BRAIN_find_three_main_tsop(Uint8 i){
	Uint8 tsop, main_tsop[3] = {0, 0, 0};
	Uint16 main_count[3] = {0, 0, 0}, val;

	for(tsop = 0; tsop < NOMBRE_TSOP; tsop++){
		val = views[i].detection->counts[tsop];

		if(val > main_count[0]){
			main_tsop[2] = main_tsop[1];
			main_count[2] = main_count[1];
			main_tsop[1] = main_tsop[0];
			main_count[1] = main_count[0];
			main_tsop[0] = tsop;
			main_count[0] = val;

		}else if(val > main_count[1]){
			main_tsop[2] = main_tsop[1];
			main_count[2] = main_count[1];
			main_tsop[1] = tsop;
			main_count[1] = val;

		}else if(val > main_count[2]){
			main_tsop[2] = tsop;
			main_count[2] = val;

		}
	}
	views[i].main_tsop[0] = main_tsop[0];
	views[i].main_tsop[1] = main_tsop[1];
	views[i].main_tsop[2] = main_tsop[2];

	if(main_count[0] < SEUIL_SIGNAL_INSUFFISANT)
		return SIGNAL_INSUFFISANT;	//PAS DE RECEPTION MAXI SUFFISANTE !!!!!!!)

	return AUCUNE_ERREUR;
}



//Traitement d'un cycle... d�duction de la position relative du robot adverse.
// (distance + angle)
error_t BRAIN_find_main_tsop(Uint8 i)
{
	Uint8 tsop, main_tsop = 0;
	Uint16 main_count;

	main_count = 0;
	//recherche du num�ro du r�cepteur ayant vu la plus grande valeur
	for(tsop = 0; tsop < NOMBRE_TSOP ; tsop++)
		{
		if(views[i].detection->counts[tsop] > main_count)
		{
			main_tsop = tsop;
			main_count = views[i].detection->counts[main_tsop];
		}
	}

	//On sauvegarde le num�ro du r�cepteur principal.
	views[i].main_tsop[0] = main_tsop;

	//Notre syst�me est tel que le maximum de r�ception sur un tour est born�.
	//Si un autre �metteur IR nous pollue, on peut le d�tecter ainsi...
	//R�sultat : indice de confiance faible sur notre r�sultat.
	if(main_count < SEUIL_SIGNAL_INSUFFISANT)
		return SIGNAL_INSUFFISANT;	//PAS DE RECEPTION MAXI SUFFISANTE !!!!!!!)
	//	if(views[i].detection->counts[main_count] > SEUIL_TROP_DE_SIGNAL)
	//		return TROP_DE_SIGNAL;

	return AUCUNE_ERREUR;
}	



//@pre recepteur_maximum MAJ
//@post extremite_gauche et droite MAJ
error_t BRAIN_find_left_right_tsop(Uint8 i)
{
	//m�thode : a partir du maximum, on analyse chaque cot�, jusqu'a des r�ceptions faibles < seuil
	//retourne un nombre de r�cepteurs
	Uint8 left_tsop, right_tsop, main_tsop;
	main_tsop 	= views[i].main_tsop[0];
	left_tsop 	= main_tsop;
	right_tsop 	= main_tsop;

	do
	{
		left_tsop = (Uint8)(left_tsop - 1) % NOMBRE_TSOP ;
	}while(views[i].detection->counts[left_tsop] > SEUIL_SIGNAL_TACHE && left_tsop != main_tsop);

	// A la sortie du while, on a l'index du r�cepteur suivant la tache... on passe donc au pr�c�dent
	views[i].left_tsop = (left_tsop + 1) % NOMBRE_TSOP;


	do
	{
		right_tsop = (right_tsop + 1) % NOMBRE_TSOP;
	}while(views[i].detection->counts[right_tsop] > SEUIL_SIGNAL_TACHE && right_tsop != left_tsop);

	// A la sortie du while, on a l'index du r�cepteur suivant la tache... on passe donc au pr�c�dent
	views[i].right_tsop = (Uint8)(right_tsop - 1) % NOMBRE_TSOP;


	//!!!!!!!!!!!!!!!!!!!!!!!!!ALGO EN PLUS SIMPLE :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// les '--' et '++' supposent une circularit� ( 0-- donne 15 et 15++ donne 0)
	//
	//	gauche = max
	//	droite = max
	//
	//	do{
	//		gauche--
	//	}while(compteurs[gauche] > SEUIL)
	//	gauche++		//pour revenir au dernier r�cepteur qui appartient � la tache
	//
	//	do{
	//		droite++
	//	}while(compteurs[droite] > SEUIL)
	//	droite--		//pour revenir au dernier r�cepteur qui appartient � la tache
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return AUCUNE_ERREUR;
}



//@pre recepteur_extremite_gauche et droite MAJ
//@post taille tache MAJ
error_t BRAIN_find_size(Uint8 i)
{
	Uint8 size;
	//calcule la taille de la tache
	if(views[i].detection->counts[views[i].main_tsop[0]] < SEUIL_SIGNAL_INSUFFISANT)
		size = 0;
	else
	{
		if(views[i].right_tsop >= views[i].left_tsop)
			size = 0;
		else
			size = NOMBRE_TSOP;

		size += views[i].right_tsop - views[i].left_tsop + 1;
	}
	//!!!!!!!!!!!!!!!!!!!!!!!!!ALGO EN PLUS SIMPLE :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//
	//	if(droite >= gauche)
	//  	taille = 0
	//	else
	//		taille = 15
	//	taille+= droite - gauche
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	views[i].size = size;

	if(size > TAILLE_TACHE_MAXIMALE)
		return TACHE_TROP_GRANDE;

	if(size == 0)
		return SIGNAL_INSUFFISANT;	//TACHE TROP PETITE

	return AUCUNE_ERREUR;
}	




//@pre extremite_gauche et droite MAJ
//@post barycentre MAJ...
//@post barycentre est exprim� en >>8, par exemple     1,25*256 si le robot est d�tect� entre le 1 et le 2... !
error_t BRAIN_find_angle(Uint8 i)
{
	Sint32 barycentre;
	Uint32 somme;
	Uint8 tsop;
	Uint8 n;

	//a partir des extr�mit�s de la tache et des poids des r�ception,
	//on peut d�terminer un barycentre en angle...
	//on ne s'occupe pas des r�cepteurs en dehors de la tache
	//cela suppose que tout les r�cepteurs fonctionnent bien

	//Formule :    0*compteur[0] + 1*compteur[1]......./somme r�cepteur
	//             	//on peut choisir d'�lever les entiers, pour obtenir des valeurs "d�cimales"...
	tsop = views[i].left_tsop;
	barycentre = 0;
	somme = 0;
	n = 1;
	do
	{
		//debug_printf("%d
		barycentre  += n*(views[i].detection->counts[tsop]);
		//on utilise le 'n' et non pas 'tsop' � cause du modulo...
		somme       += views[i].detection->counts[tsop];

		//Si on vient de traiter l'extr�mit� droite, on sort de la boucle.
		if(tsop ==  views[i].right_tsop)
			break;

		//recepteur suivant.
		tsop = (tsop + 1) % NOMBRE_TSOP;
		n++;
	}while(1);

	barycentre <<= 12;		// * 4096... pour pouvoir diviser
	if(somme !=0)
	{
		barycentre /= somme;	// moyenne obtenue = angle moyen au del� de l'extr�mit� gauche
		barycentre = (barycentre + ((Sint32)(views[i].left_tsop-1) << 12)) % ((Sint32)(NOMBRE_TSOP) << 12);	//angle adversaire absolu...
	}
	else
	{
		views[i].angle = 0;
		return AUCUN_SIGNAL;
	}

	barycentre = -(barycentre*2*PI4096)/NOMBRE_TSOP;
	barycentre  >>= 12;

	//recentrage angle.
	//OBJECTIF : angle exprim� en rad/4096, allant de -PI4096+1 � PI4096
	views[i].angle = calcul_recentrage_angle(barycentre);

	return AUCUNE_ERREUR;
}
	

	
//@pre MAJ de global.localisation_puissance_tache...
error_t BRAIN_find_distance(Uint8 i)
{
	Sint32 distance;
	Uint16 power;
	//La puissance correspond � la somme des 3 r�cepteurs encadrant le r�cepteur maximal...
	power =   views[i].detection->counts[views[i].main_tsop[0]]
			+ views[i].detection->counts[views[i].main_tsop[1]]
			+ views[i].detection->counts[views[i].main_tsop[2]];

	//+ views[i].detection->counts[(Uint8)(views[i].main_tsop - 1) % NOMBRE_TSOP]
	//+ views[i].detection->counts[(Uint8)(views[i].main_tsop + 1) % NOMBRE_TSOP];
	//Attention, le cast en Uint8 est tr�s utile !!! si on le met pas, le modulo est effectu� sur un Sint8 (� cause du -1) et on acc�de en -1 dans le tableau !!!


	if(power < 100){
		//y = 0,1167x2 - 22,872x + 1430,7 pour une distance en mm
		//y = 0,0058x2 - 1,1436x + 71,534 pour une distance en 2cm
		// On multiplie par 2^14 pour ne pas avoir de flottant
		distance = (((Sint32)437*(Sint32)power*(Sint32)power - (Sint32)79064*(Sint32)power + (Sint32)3981414) >> 10)/20;
				//distance = PRODUIT_DISTANCE_EVIT_PUISSANCE_EVIT/power;	//Regl� d'apr�s la mesure. ATTENTION : unit� = [2cm]...
				//PRODUIT_DISTANCE_EVIT_PUISSANCE_EVIT = "distance d'�vitement souhait�e" * "PuissanceRe�ue mesur�e � cette distance"
	}else
		distance = 12; // Soit 24 cm

	if(distance > 255)
	{
		//Attention, deux cas diff�rents ici :
		//Si aucun signal, la distance est quand m�me mise � 255.
		//Si le signal est trop faible mais qu'il est non nul, on envoi SIGNAL_INSUFFISANT, qui n'est pas tellement un cas d'erreur,
		//Mais plut�t un cas super cool o� le robot est tr�s loin !!!!!!

		views[i].distance = 255;
		return SIGNAL_INSUFFISANT;
	}
	views[i].distance = (Uint8)(distance);
	return AUCUNE_ERREUR;
}	


Uint16 BRAIN_find_mediane_distance(void)
{
	return views[0].distance;
	/*
	bool_e loop;
	Uint8 i;
	Uint16 array[NUMBER_OF_BEST_DETECTIONS], tmp;
	for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
	{	//On recopie les valeurs dont on veut trouver la m�diane
		array[i] = views[i].distance;
	}
	do
	{
		loop = FALSE;	//On suppose que c'est le dernier passage.
		for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
		{
			if(array[i] > array[i+1]) // Attention effet de bord !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			{
				tmp 		= array[i+1];	//inversion de deux �l�ments dans le d�sordre.
				array[i+1]	= array[i];
				array[i] 	= tmp;
				loop = TRUE;	//On vient d'effectuer une inversion, ce n'est donc pas le dernier passage.
			}
		}
	}while(loop);
	return array[NUMBER_OF_BEST_DETECTIONS/2];	//La m�diane est le milieu du tableau tri� !*/
}	


//ATTENTION, une m�diane d'angle pr�sente un GROS risque lorsque les donn�es d'entr�es sont autour du seuil -Pi/Pi ! 
//On fait le parie que dans ce cas la m�diane renverra l'une des trois valeurs, qui de toute fa�on n'ont pas � �tre trop fausses !
Sint16 BRAIN_find_mediane_angle(void)
{
	return views[0].angle;
	/*
	bool_e loop;
	Uint8 i;
	Sint16 tmp;
	Sint16 array[NUMBER_OF_BEST_DETECTIONS];
	for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
	{	//On recopie les valeurs dont on veut trouver la m�diane
		array[i] = views[i].angle;
	}
	do
	{
		loop = FALSE;	//On suppose que c'est le dernier passage.
		for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
		{
			if(array[i] > array[i+1])
			{
				tmp 		= array[i+1];	//inversion de deux �l�ments dans le d�sordre.
				array[i+1]	= array[i];
				array[i] 	= tmp;
				loop = TRUE;	//On vient d'effectuer une inversion, ce n'est donc pas le dernier passage.
			}
		}
	}while(loop);
	return array[NUMBER_OF_BEST_DETECTIONS/2];	//La m�diane est le milieu du tableau tri� ! */
}




Sint16 calcul_recentrage_angle(Sint16 angle)
{
	if(angle > PI4096)
		angle -= 2*PI4096;
	if(angle < -PI4096)
		angle += 2*PI4096;

	return angle;
}


//cette fonction PUBLIQUE (appel�e quand c'est n�cessaire, avant l'envoi d'une info) met � jour la position en x/y du robot 
//en utilisant la position relative en alpha/distance
//ainsi que la position connue de notre robot
//@precondition : on suppose connue la position actuelle de notre robot.
#if 0
#warning "Cette fonction n'est pas utilis�e en 2011... La fusion de donn�e est faite sur la strat�gie... La distance estim�e ici du robot adverse n'�tant pas tr�s fiable..."
	void MAJ_xy(void)
	{
		Sint16 cos, sin;
		Sint16 alpha;
		alpha = calcul_recentrage_angle(global.adverse_angle - global.nous_angle);	//[rad/4096]
		
		if(alpha<0)
			alpha += 2*PI4096;
		
		cos_sin(alpha, &cos, &sin);	
		#warning "n�cessite d'inclure math_maison.c"
	
	
		//cos et sin sont exprim�s en [/4096]
		global.adverse_x = (Sint16)(((Sint32)(global.nous_x)*4096 + (Sint32)(cos) * (Sint32)global.adverse_distance)/4096);
		global.adverse_y = (Sint16)(((Sint32)(global.nous_y)*4096 + (Sint32)(sin) * (Sint32)global.adverse_distance)/4096);
		
		//V�rification de plausibilit�...
		//TODO
		
		//debug_printf("\na=%d,d=%ld,cos=%d, x=%d, y=%d",alpha,global.adverse_distance,cos, global.adverse_x, global.adverse_y);	
	}
	
#endif

			
			
