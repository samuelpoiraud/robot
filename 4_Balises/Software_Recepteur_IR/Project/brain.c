/*
 *	Club Robot ESEO 2009 - 2010
 *	
 *
 *	Fichier : balise_localisation.c
 *	Package : Balises 2010
 *	Description : fonctions permettant la localisation du robot adverse à partir des données de réceptions sur un cycle.
 *	Auteur : Nirgal
 *	Version 201002
 */

#define BRAIN_C
#include "brain.h"
#include "eyes.h"
//#include "QS/QS_timer.h"



#define	NUMBER_OF_BEST_DETECTIONS (1)

//Une vue d'un adversaires est : une bonne détection de cet adversaire qui a eu lieu pendant les 100ms de mesure, 
//									assorti de caractéristiques calculées, intermédiaires et permettant le calcul de sa position.
typedef struct
{
	detection_t * 	detection;
	Uint8			main_tsop;
	Uint8			left_tsop;
	Uint8			right_tsop;
	Uint8			size;
	Uint8 			distance;	//[2cm]
	Sint16			angle;
}BRAIN_adversary_view_t;
static volatile BRAIN_adversary_view_t views[NUMBER_OF_BEST_DETECTIONS];	//Pointeurs vers les meilleurs détections des 100ms.
volatile EYES_adversary_detections_t * next_adversary_detection;
static volatile adversary_location_t adversary_location[ADVERSARY_NUMBER];	

void BRAIN_process_treatment_detections(volatile EYES_adversary_detections_t * adversary_detection);
error_t BRAIN_find_main_tsop(Uint8 i);
error_t BRAIN_find_left_right_tsop(Uint8 i);
error_t BRAIN_find_size(Uint8 i);
error_t BRAIN_find_angle(Uint8 i);
error_t BRAIN_find_distance(Uint8 i);
Uint8 	BRAIN_find_mediane_distance(void);
Sint16 	BRAIN_find_mediane_angle(void);
Sint16 calcul_recentrage_angle(Sint16 angle);


void BRAIN_init(void)
{
	Uint8 i;
	next_adversary_detection = 0;
	for(i = 0; i<ADVERSARY_NUMBER; i++)
	{
		adversary_location[i].distance = 0;
		adversary_location[i].angle = 0;
		adversary_location[i].error = AUCUN_SIGNAL;
	}	
}

void BRAIN_task_add(volatile EYES_adversary_detections_t * adversary_detection)
{
	next_adversary_detection = adversary_detection;	
}	

void BRAIN_process_main(void)
{
	Uint8 i;
	if(next_adversary_detection != 0)
	{	//Il y a une période de 100ms à traiter ?

		//TRAITEMENT DES DONNEES RECUES (on trouve les plus grandes détections).
		BRAIN_process_treatment_detections(next_adversary_detection);

		for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
		{
			//Chacune des meilleurs détections est analysée indépendamment. On construit "n VUES" à partir de "n DETECTIONS".
			next_adversary_detection->error |= BRAIN_find_main_tsop(i);
			next_adversary_detection->error |= BRAIN_find_left_right_tsop(i);
			next_adversary_detection->error |= BRAIN_find_size(i);		
			next_adversary_detection->error |= BRAIN_find_angle(i);
			next_adversary_detection->error |= BRAIN_find_distance(i);
		}		
		/////////////////////////////////////////////////////////////////////
		//Médianne des angles et distances obtenues.
		adversary_location[next_adversary_detection->adversary].distance 	= BRAIN_find_mediane_distance();
		adversary_location[next_adversary_detection->adversary].angle 		= BRAIN_find_mediane_angle();
		adversary_location[next_adversary_detection->adversary].error		= next_adversary_detection->error;
		
		
		//DONNEES UTILISEES ET NETTOYABLES... ON REINITIALISE POUR LES PROCHAINES 100ms
		EYES_init_adversary_detection(next_adversary_detection);
		
		//LOCALISATION
		
		next_adversary_detection = 0;	//Traité.
	}	
}

	
volatile adversary_location_t * BRAIN_get_adversary_location(void)
{
	return adversary_location;
}	
	

//	static Uint8 k;
	//PROCESS DE TACHE DE FOND.......
//	if(global.flag_nouveau_cycle)
//	{		
		//LOCALISATION !		
		
		
		//if(!k)²
/*		debug_printf("\n%ld\t%d %d",
									global.localisation_puissance_tache,
									global.adverse_distance,
									global.localisation_erreur[global.current_emetteur][global.current_emetteur]
				);
*/				
	
/*	if(!k)
	debug_printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
			global.cycles[global.index_cycles].compteurs[0],
			global.cycles[global.index_cycles].compteurs[1],
			global.cycles[global.index_cycles].compteurs[2],
			global.cycles[global.index_cycles].compteurs[3],
			global.cycles[global.index_cycles].compteurs[4],
			global.cycles[global.index_cycles].compteurs[5],
			global.cycles[global.index_cycles].compteurs[6],
			global.cycles[global.index_cycles].compteurs[7],
			global.cycles[global.index_cycles].compteurs[8],
			global.cycles[global.index_cycles].compteurs[9],
			global.cycles[global.index_cycles].compteurs[10],
			global.cycles[global.index_cycles].compteurs[11],
			global.cycles[global.index_cycles].compteurs[12],
			global.cycles[global.index_cycles].compteurs[13],
			global.cycles[global.index_cycles].compteurs[14],
			global.cycles[global.index_cycles].compteurs[15]);
*/			
/*	if(!k)
		debug_printf("\n G:%d\t M:%d\t D:%d\t Taille:%d\t Pssce:%ld\t | angle:%ld\t a:%ld\t | dist:%ld\t | erreur:%x",		
				global.localisation_recepteur_extremite_gauche,
				global.cycles[global.index_cycles].compteurs[global.localisation_recepteur_maximum],
				global.localisation_recepteur_extremite_droite,
				global.localisation_taille_tache,
				global.localisation_puissance_tache,
				global.adverse_angle,
				global.adverse_angle/256,
				global.adverse_distance,
				global.localisation_erreur[global.current_emetteur][global.current_emetteur]
			);
*/
	



	//Traitement des données issues d'un adversaire. (100ms de données sont traitées ici...)
	//Ce code sera exécuté avec une priorité faible (tâche de fond ou IT secondaire...)	
	void BRAIN_process_treatment_detections(volatile EYES_adversary_detections_t * adversary_detection)
	{
		volatile detection_t	* detections;
		
		volatile time16_t duration, max_duration;	
		volatile Uint8 i,max_index;
		max_duration = 0;
		max_index = 0;
		
		detections = (detection_t*)next_adversary_detection->detections;

		//Recherche de la plus longue détection...
		for(i = 0 ; i <= next_adversary_detection->index ; i++)
		{
			duration = detections[i].end - detections[i].begin;
			if(duration > max_duration)
			{
				max_duration = duration;
				max_index = i;
			}	
		}	
		
		//Sauvegarde des meilleures détections. (on récupère leurs pointeurs...)
		views[0].detection = &(detections[max_index]);
						
	}
	
	



//Traitement d'un cycle... déduction de la position relative du robot adverse.
// (distance + angle)
error_t BRAIN_find_main_tsop(Uint8 i)
{
	Uint8 tsop, main_count;
	Uint8 main_tsop = 0;
	
	main_count = 0;
	//recherche du numéro du récepteur ayant vu la plus grande valeur
	for(tsop = 0; tsop < NOMBRE_TSOP ; tsop++)
	{
		if(views[i].detection->counts[tsop] > main_count)
		{
			main_tsop = tsop;
			main_count = views[i].detection->counts[main_tsop];
		}	
	}
	
	//On sauvegarde le numéro du récepteur principal.
	views[i].main_tsop = main_tsop;
	
	//Notre système est tel que le maximum de réception sur un tour est borné.
	//Si un autre émetteur IR nous pollue, on peut le détecter ainsi...
	//Résultat : indice de confiance faible sur notre résultat.
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
	//méthode : a partir du maximum, on analyse chaque coté, jusqu'a des réceptions faibles < seuil
	//retourne un nombre de récepteurs
	Uint8 left_tsop, right_tsop, main_tsop;
	main_tsop 	= views[i].main_tsop;
	left_tsop 	= main_tsop;
	right_tsop 	= main_tsop;
	
	do
	{
		left_tsop = (Uint8)(left_tsop - 1) % NOMBRE_TSOP ;	
	}while(views[i].detection->counts[left_tsop] > SEUIL_SIGNAL_TACHE && left_tsop != main_tsop);
		
	// A la sortie du while, on a l'index du récepteur suivant la tache... on passe donc au précédent
	views[i].left_tsop = (left_tsop + 1) % NOMBRE_TSOP;



	
	do
	{
		right_tsop = (right_tsop + 1) % NOMBRE_TSOP;
	}while(views[i].detection->counts[right_tsop] > SEUIL_SIGNAL_TACHE && right_tsop != left_tsop);
		
	// A la sortie du while, on a l'index du récepteur suivant la tache... on passe donc au précédent
	views[i].right_tsop = (Uint8)(right_tsop - 1) % NOMBRE_TSOP;


//!!!!!!!!!!!!!!!!!!!!!!!!!ALGO EN PLUS SIMPLE :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// les '--' et '++' supposent une circularité ( 0-- donne 15 et 15++ donne 0)
//
//	gauche = max
//	droite = max
//	
//	do{
//		gauche--
//	}while(compteurs[gauche] > SEUIL)
//	gauche++		//pour revenir au dernier récepteur qui appartient à la tache
//	
//	do{
//		droite++
//	}while(compteurs[droite] > SEUIL)
//	droite--		//pour revenir au dernier récepteur qui appartient à la tache
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	

	return AUCUNE_ERREUR;


}



//@pre recepteur_extremite_gauche et droite MAJ
//@post taille tache MAJ
error_t BRAIN_find_size(Uint8 i)
{
	Uint8 size;
		//calcule la taille de la tache 
	if(views[i].detection->counts[views[i].main_tsop] < SEUIL_SIGNAL_INSUFFISANT)
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
//@post barycentre est exprimé en >>8, par exemple     1,25*256 si le robot est détecté entre le 1 et le 2... !
error_t BRAIN_find_angle(Uint8 i)
{
	Sint32 barycentre;
	Uint32 somme;
	Uint8 tsop;
	Uint8 n;
	
	//a partir des extrémités de la tache et des poids des réception, 
	//on peut déterminer un barycentre en angle...
	//on ne s'occupe pas des récepteurs en dehors de la tache
	//cela suppose que tout les récepteurs fonctionnent bien
	
	//Formule :    0*compteur[0] + 1*compteur[1]......./somme récepteur
	//             	//on peut choisir d'élever les entiers, pour obtenir des valeurs "décimales"...
	tsop = views[i].left_tsop;
	barycentre = 0;
	somme = 0;
	n = 1;
	do
	{
		//debug_printf("%d 
		barycentre  += n*(views[i].detection->counts[tsop]);
		//on utilise le 'n' et non pas 'tsop' à cause du modulo...
		somme 		+= views[i].detection->counts[tsop];
		
		//Si on vient de traiter l'extrémité droite, on sort de la boucle.
		if(tsop ==  views[i].right_tsop)
			break;
			
		//recepteur suivant.
		tsop = (tsop + 1) % NOMBRE_TSOP;
		n++;
		
	}while(1);	

	barycentre <<= 12;		// * 4096... pour pouvoir diviser
	if(somme !=0)
	{
		barycentre /= somme;	// moyenne obtenue = angle moyen au delà de l'extrémité gauche
		barycentre = (barycentre + ((Sint32)(views[i].left_tsop-1) << 12)) % ((Sint32)(NOMBRE_TSOP) << 12);	//angle adversaire absolu...
	}	
	else
	{
		views[i].angle = 0;
		return SIGNAL_INSUFFISANT;
	}	

	barycentre = -(barycentre*2*PI4096)/NOMBRE_TSOP;
	barycentre  >>= 12;
	
	//recentrage angle.
	//OBJECTIF : angle exprimé en rad/4096, allant de -PI4096+1 à PI4096
	views[i].angle = calcul_recentrage_angle(barycentre);
	
	return AUCUNE_ERREUR;	
}
	

	
//@pre MAJ de global.localisation_puissance_tache...
error_t BRAIN_find_distance(Uint8 i)
{
	Sint32 distance;
	Uint16 power;
	//La puissance correspond à la somme des 3 récepteurs encadrant le récepteur maximal...
	power =   views[i].detection->counts[views[i].main_tsop]
			+ views[i].detection->counts[(Uint8)(views[i].main_tsop - 1) % NOMBRE_TSOP]
			+ views[i].detection->counts[(Uint8)(views[i].main_tsop + 1) % NOMBRE_TSOP];
				//Attention, le cast en Uint8 est très utile !!! si on le met pas, le modulo est effectué sur un Sint8 (à cause du -1) et on accède en -1 dans le tableau !!!
	if(power == 0)
		distance = 255;
	else
		distance = PRODUIT_DISTANCE_EVIT_PUISSANCE_EVIT/power;	//Reglé d'après la mesure. ATTENTION : unité = [2cm]...
		// PRODUIT_DISTANCE_EVIT_PUISSANCE_EVIT = "distance d'évitement souhaitée" * "PuissanceReçue mesurée à cette distance"
	
	if(distance > 255)
	{
		//Attention, deux cas différents ici :
		//Si aucun signal, la distance est quand même mise à 255.
		//Si le signal est trop faible mais qu'il est non nul, on envoi SIGNAL_INSUFFISANT, qui n'est pas tellement un cas d'erreur,
		//Mais plutôt un cas super cool où le robot est très loin !!!!!!
		
		views[i].distance = 255;
		return SIGNAL_INSUFFISANT;
	}
	views[i].distance = (Uint8)(distance);
	return AUCUNE_ERREUR;
}	


Uint8 BRAIN_find_mediane_distance(void)
{
	bool_e loop;
	Uint8 i, tmp;
	Uint8 array[NUMBER_OF_BEST_DETECTIONS];
	for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
	{	//On recopie les valeurs dont on veut trouver la médiane
		array[i] = views[i].distance;
	}
	do
	{
		loop = FALSE;	//On suppose que c'est le dernier passage.
		for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
		{
			if(array[i] > array[i+1])
			{
				tmp 		= array[i+1];	//inversion de deux éléments dans le désordre.
				array[i+1]	= array[i];
				array[i] 	= tmp;
				loop = TRUE;	//On vient d'effectuer une inversion, ce n'est donc pas le dernier passage.
			}
		}	
	}while(loop);
	return array[NUMBER_OF_BEST_DETECTIONS/2];	//La médiane est le milieu du tableau trié !
}	


//ATTENTION, une médiane d'angle présente un GROS risque lorsque les données d'entrées sont autour du seuil -Pi/Pi ! 
//On fait le parie que dans ce cas la médiane renverra l'une des trois valeurs, qui de toute façon n'ont pas à être trop fausses !
Sint16 BRAIN_find_mediane_angle(void)
{
	bool_e loop;
	Uint8 i;
	Sint16 tmp;
	Sint16 array[NUMBER_OF_BEST_DETECTIONS];
	for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
	{	//On recopie les valeurs dont on veut trouver la médiane
		array[i] = views[i].angle;
	}
	do
	{
		loop = FALSE;	//On suppose que c'est le dernier passage.
		for(i=0;i<NUMBER_OF_BEST_DETECTIONS;i++)
		{
			if(array[i] > array[i+1])
			{
				tmp 		= array[i+1];	//inversion de deux éléments dans le désordre.
				array[i+1]	= array[i];
				array[i] 	= tmp;
				loop = TRUE;	//On vient d'effectuer une inversion, ce n'est donc pas le dernier passage.
			}
		}	
	}while(loop);
	return array[NUMBER_OF_BEST_DETECTIONS/2];	//La médiane est le milieu du tableau trié !
}




Sint16 calcul_recentrage_angle(Sint16 angle)
{
	if(angle > PI4096)
		angle -= 2*PI4096;
	if(angle < -PI4096)
		angle += 2*PI4096;
		
	return angle;		
}


//cette fonction PUBLIQUE (appelée quand c'est nécessaire, avant l'envoi d'une info) met à jour la position en x/y du robot 
//en utilisant la position relative en alpha/distance
//ainsi que la position connue de notre robot
//@precondition : on suppose connue la position actuelle de notre robot.
#if 0
#warning "Cette fonction n'est pas utilisée en 2011... La fusion de donnée est faite sur la stratégie... La distance estimée ici du robot adverse n'étant pas très fiable..."
	void MAJ_xy(void)
	{
		Sint16 cos, sin;
		Sint16 alpha;
		alpha = calcul_recentrage_angle(global.adverse_angle - global.nous_angle);	//[rad/4096]
		
		if(alpha<0)
			alpha += 2*PI4096;
		
		cos_sin(alpha, &cos, &sin);	
		#warning "nécessite d'inclure math_maison.c"
	
	
		//cos et sin sont exprimés en [/4096]
		global.adverse_x = (Sint16)(((Sint32)(global.nous_x)*4096 + (Sint32)(cos) * (Sint32)global.adverse_distance)/4096);
		global.adverse_y = (Sint16)(((Sint32)(global.nous_y)*4096 + (Sint32)(sin) * (Sint32)global.adverse_distance)/4096);
		
		//Vérification de plausibilité...
		//TODO
		
		//debug_printf("\na=%d,d=%ld,cos=%d, x=%d, y=%d",alpha,global.adverse_distance,cos, global.adverse_x, global.adverse_y);	
	}
	
#endif

			
			
