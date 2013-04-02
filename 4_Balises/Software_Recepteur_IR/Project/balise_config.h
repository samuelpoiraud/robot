/*
 *	Club Robot ESEO 2009 - 2010
 *	
 *
 *	Fichier : balise_config.h
 *	Package : Balises 2010
 *	Description : configuration du code de la balise
 *	Auteur : Nirgal
 *	Version 200907
 */

#ifndef BALISE_CONFIG_H
	#define BALISE_CONFIG_H
	

/////////////////////////////////COMMUNICATION//////////////////////////////////////
	
	//#define VERBOSE_CAN_OVER_UART1	//Envoyer le contenu des messages can sur l'uart, avec une belle présentation..
	
	//#define CAN_SEND_OVER_UART1	//Envoyer les messages can sur l'uart 1.

	//#define VERBOSE_MODE			//Activation des debug_printf()
	
	#define USE_CAN
	#define CAN_BUF_SIZE		4	//	Nombre de messages CAN conservés pour traitement hors interuption
	#define QS_CAN_RX_IT_PRI CAN_INT_PRI_5	//Priorité du CAN baissée volontairement.

	#define USE_UART1
	#define USE_UART1RXINTERRUPT
//	#define USE_UART2
//	#define USE_UART2RXINTERRUPT
	#define UART_RX_BUF_SIZE	12	//	Taille de la chaine de caracteres memorisant les caracteres recus sur UART
	

//////////////////////////////////MODES_DE_FONCTIONNEMENT////////////////////////////
	//CHOISIR L'UN de ces modes...
	
//	#define MODE_TEST_DIODES
	//les diodes s'allument chacune leur tour selon le timer 2
	//ce mode peut être super fun en fin de match !!!
	
	//#define MODE_RECEPTION_SIMPLE	
	//dans ce mode, aucun buffer n'est pris en compte, 
	//les diodes s'allument simplement lorsque les récepteurs recoivent
	
	typedef Uint8 error_t;
/////////ERREURS POSSIBLES !!!////////////////////////////////////////////////////////////////////////////	
	
	//Ces erreurs possibles sont appliquées à l'Uint8 global.localisation_erreur...
	//plusieurs erreurs peuvent se cumuler... donc 1 bit chacune...
	#define AUCUNE_ERREUR						0b00000000
					//COMPORTEMENT : le résultat délivré semble bon, il peut être utilisé.
					
	#define AUCUN_SIGNAL						0b00000001	
					//survenue de l'interruption timer 3 car strictement aucun signal reçu depuis au moins deux tours moteurs
					//cette erreur peut se produire si l'on est très loin
					//COMPORTEMENT : pas d'évittement par balise, prise en compte des télémètres !
											
	#define SIGNAL_INSUFFISANT					0b00000010	
					//il peut y avoir un peu de signal, mais pas assez pour estimer une position fiable (se produit typiquement si l'on est trop loin)
					//cette erreur n'est pas grave, on peut considérer que le robot est LOIN !
					//COMPORTEMENT : pas d'évittement, pas de prise en compte des télémètres !
					
	#define TACHE_TROP_GRANDE					0b00000100
					//Ce cas se produit si trop de récepteurs ont vu du signal.
					// Ce seuil est STRICTEMENT supérieur au cas normal d'un robot très pret. Il y a donc probablement un autre émetteur quelque part, ou on est entouré de miroir.
					//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...
					
	#define TROP_DE_SIGNAL						0b00001000
					//Le récepteur ayant reçu le plus de signal en à trop recu
					//	cas 1, peu probable, le moteur est bloqué (cas de test facile pour vérifier cette fonctionnalité !)
					//	cas 2, probable, il y a un autre émetteur quelque part !!!
					// 	cas 3, on est dans une enceinte fermée et on capte trop
					//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...
					
	#define ERREUR_POSITION_INCOHERENTE 		0b00010000
					//La position obtenue en x/y est incohérente, le robot semble être franchement hors du terrain
					//COMPORTEMENT : si la position obtenue indique qu'il est loin, on ne fait pas d'évitement !
					//					sinon, on fait confiance à nos télémètres (?)
					
	#define OBSOLESCENCE						0b10000000
					//La position adverse connue est obsolète compte tenu d'une absence de résultat valide depuis un certain temps.
					//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...
	
	
				


//////////////////////////////////PARAMETRAGES///////////////////////////////////////	
/////////////////////////////////////////////////////////////////////////////////////		
	#define MINIMUM_DURATION_BETWEEN_TWO_DETECTIONS 11// =1,43ms //[130us] si l'on a rien reçu pendant cette durée, on inaugure la détection suivante.  Unité : [TMR3]
	

	//Période d'échantillonage des réceptions.
	//Attention, si trop petit le PIC est pas assez rapide pour éxécuter l'IT...
	//la frequence minimum vitale est 6,66kHz = 150µs...(shannon).... (=2*période de l'émetteur)
	//je choisis environ 7,7kHz	 > 6,66... il fauat être sur qu'un éventuel rapport cyclique !=50% ne nous gène pas...
	//Ensuite, on compte chaque changement d'état...
	//plus d'infos ? >> samuelp5@gmail.com
	#define	CONFIG_TIMER_ECHANTILLONAGE 130//130		//[µs]	
	
	//Durée minimale entre deux envois vers la carte stratégie...
	#define DUREE_MINI_ENTRE_DEUX_ENVOIS 250	//ms (maximum = 255 !)
	
	//pour info, utilisé dans le code, figé et imposé par la carte.
	//Le code n'est pas cependant suffisamment générique pour permette que cette seule modifications soit répercutés aux endroits concernés...
	#define NOMBRE_TSOP 				16	


	#define SEUIL_SIGNAL_TACHE			10	
	//TODO regler !!! > si un récepteur à reçu une "puissance" d'au moins ceci, il peut appartenir à la tache
	
	
	#define SEUIL_SIGNAL_INSUFFISANT	4	
	//TODO regler !!! > si aucun récepteur n'a recu une "puissance" d'au moins ceci, on déclare l'état d'erreur de signal insuffisant !
	
	
	#define SEUIL_TROP_DE_SIGNAL		300	
	// > quel maximum a t'on au plus près pour un récepteur
	//Si un récepteur à recu plus que ce signal en un cycle, on lève l'état d'erreur de TROP_DE_SIGNAL !
	
	
	#define TAILLE_TACHE_MAXIMALE		12
	// > combien de récepteurs captent au plus près
	//Si ce nombre de récepteur ont recu du signal (ils appartiennent tous à la tache) >> état d'erreur levé !
	
	#define PRODUIT_DISTANCE_EVIT_PUISSANCE_EVIT	10800
	//  = "distance d'évitement souhaitée" * "PuissanceReçue mesurée à cette distance"
	//Ce coefficient permet la mesure de distance... 
	//La distance étant déterminée par une courbe en 1/P, il faut connaitre le produit Puissance*Distance
	//Pour plus de précision, on détermine ce produit sur la distance que l'on souhaite la plus précise : la distance d'évitement !
	

	
	
	
#endif /* ndef BALISE_CONFIG_H */
