/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen (inspiré du package QS d'Axel Voitier)
 *  Version 20081010
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_vars.h, lire le CQS."
	#endif


	

	//Le buffer d'instant contient pour chaque cycle les 3 instants de détections des 3 balises, ainsi que la durée du cycle.
	volatile Uint16 buffer_instants[NOMBRE_BALISES + 1][NOMBRE_CYCLES_SAUVES];
	
	//index du cycle en cours dans le buffer circulaire
	volatile Uint8 index_cycle;
	
	//un nouveau cycle est pret et peut être analysé en tâche de fond...
	volatile bool_e flag_nouveau_cycle;

	volatile bool_e flag_envoi;
	
	//Les coordonnées courantes x et y (sans correction due à la couleur...)

	volatile module_id_e XBEE_i_am_module;
	volatile bool_e match_started;

	/*
		Une détection est un couple levée + descente de signal recu. 
		Plusieurs détections peuvent être faites par tour de moteur s'il y a des perturbations où si plusieurs capteurs ne se chevauchent pas...
		La plus longue détection sera ensuite considérée comme la bonne sur le cycle.
		
		L'index de détection est, pour chaque balise, pendant un cycle moteur, le numéro de la détection en cours...
		par exemple, s'il y a eu une perturbation, puis le vrai signal, l'index sera rendu à 2 pour cette balise
	*/

	volatile detection detections[NOMBRE_BALISES][NOMBRE_DETECTIONS_PAR_BALISE];
	volatile Uint8 index_detection[NOMBRE_BALISES];






//	volatile Sint16 angle_a, angle_b, angle_c;	//Intervales de temps entre les détections
	
	
	//Les temps partiel sont les durées entre les détections, avant l'ajout de la demi durée détection qui sera connue plus tard.
	//Merci de contacter les anciens pour toute questions métaphysiques à ce sujet.
//	volatile Sint16 temps31_partiel, temps12_partiel, temps23_partiel;
	
	//Durée de détection signifie la durée entre le début d'une détection par une balise et la fin de cette détection (connue au moment précis ou la balise suivante commence a détecter...)
	//Merci de contacter les anciens pour toute questions métaphysiques à ce sujet.
	//Cette fonctionnalité est interessant notamment en cas de rebond et de multiples détections sur la même balise.
	//L'OBJECTIF EST DE DETERMINER LE CENTRE DE LA OU DES DETECTIONS...
//	volatile Sint16 duree_detection1, duree_detection2, duree_detection3;
		
	
//	volatile Uint8 compteur_interruption;
	
//	volatile Uint8 compteur_precision;
/*	volatile Sint16 tabx[20];
	volatile Sint16 taby[20];
	volatile Sint16 Medx[20];
	volatile Sint16 Medy[20];
*/	volatile Uint16 color;
	


//	volatile Sint32 moy_x;
//	volatile Sint32 moy_y;
	volatile CAN_msg_t CAN_msg;

		
//	#define TAILLE_TABLEAU_BUBBLE_SORT 20
		
	
#endif /* ndef GLOBAL_VARS_H */
