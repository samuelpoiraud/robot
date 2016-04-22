/*
 *  Club Robot ESEO 2008 - 2013
 *
 *  Fichier : _Propulsion_config.h
 *  Package : Propulsion
 *  Description : Configuration de l'asser
 *  Auteur : Nirgal, FanFan
 *  Version 201203
 */

#ifndef _PROPULSION_CONFIG_H
	#define _PROPULSION_CONFIG_H

	#include "QS/QS_measure.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///ODOMETRIE////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//POUR REGLER L'ODOMETRIE:
//Consultez le Fichier: svn\propulsion\Documentation\TUTO_reglage_odometrie.txt

	#define SMALL_ODOMETRY_COEF_TRANSLATION_DEFAULT 0x0BED
	#define BIG_ODOMETRY_COEF_TRANSLATION_DEFAULT 0x0BCD  //Théorique pour 60mm : 0x0C10
	// COEF_ODOMETRIE_TRANSLATION : nombre de mm par impulsion de roue codeuse, par 5ms
	// d : diametre des roues codeuse en mm = 60
	// c : resolution du codeur = 4000
	// COEF_ODOMETRIE_TRANSLATION = 16*4096*(PI*d)/c;	 	[mm/16/4096/impulsion/5ms]
	//		on le calcule pour l'approcher, et on le détermine ensuite par l'expérience !
	//COEF_ODOMETRIE_TRANSLATION est le premier des coeffs a régler... il suffit d'envoyer le robot très loin et de regarder s'il arrive assez loin.
	//Si le robot va trop loin, il faut augmenter le coeff et vice versa


	#define SMALL_ODOMETRY_COEF_SYM_DEFAULT 0x320 //0x2EA
	#define BIG_ODOMETRY_COEF_SYM_DEFAULT 0x15

	#define SMALL_ODOMETRY_COEF_ROTATION_DEFAULT   0x00011400//0x0001139A	//Guy : 89mm entre roue codeuse et centre
	#define BIG_ODOMETRY_COEF_ROTATION_DEFAULT 0x0000ACB7  	//Holly : 278mm ENVIRON entre roues codeuses...
															// - 1% (estimation à l'arrache-mètre) -> B63D
															//Pierre : 138mm entre roue codeuse et centre - théorique : B308
	// COEF_ODOMETRIE_ROTATION : nombre de radians par impulsion de roue codeuse, par 5ms
	// COEF_ODOMETRIE_ROTATION = 1024*4096*64*(PI*d)/c /(2*r)	[rad/16/4096/1024/impulsions/5ms]
	// d : diametre des roues codeuse en mm = 60
	// c : resolution du codeur = 4000
	// r : rayon du cercle ayant pour diamètre le segment définit entre les roues codeuses en mm
			// r dépend du robot... sur Check Norris, r = 153 mm(145 sur chomp)
			// (on peut aussi dire que COEF_ODOMETRIE_ROTATION = 1024*COEF_ODOMETRIE_TRANSLATION/(2*r))
	// on le calcule pour l'approcher, et on le détermine ensuite par l'expérience !
	//COMMENT REGLER COEF_ODOMETRIE_ROTATION ? (cela est une image de la distance entre les deux roues codeuses...)
	// 1 - la démarche consiste à faire tourner le robot sur lui même et travailler par dichotomie.
	// 2 - on envoie la suite d'ordre PI > PI/2 > 0 > -PI/2... en boucle
	// > - Si le robot a trop tourné, le coeff est TROP PETIT
	// > - Si le robot a pas assez tourné, le coeff est TROP GRAND...

	//Amusez vous !

	//COEF_ODOMETRIE_CENTRIFUGE permet de régler la variation d'odométrie en courbe
	#define SMALL_ODOMETRY_COEF_CENTRIFUGAL_DEFAULT (0)
	#define BIG_ODOMETRY_COEF_CENTRIFUGAL_DEFAULT (0)


	#define FIELD_SIZE_Y 3000	//[mm]
	#define FIELD_SIZE_X 2000	//[mm]


// POSITION 2016
	//SMALL Position calibration
		//BOT_COLOR
		#define SMALL_BOT_COLOR_CALIBRATION_X  		1005
		#define SMALL_BOT_COLOR_CALIBRATION_Y  		SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE //10mm de marge avec le bord de la serviette
		#define SMALL_BOT_COLOR_CALIBRATION_TETA	(-PI4096/2)

		//TOP_COLOR
		#define SMALL_TOP_COLOR_CALIBRATION_X  		1005
		#define SMALL_TOP_COLOR_CALIBRATION_Y  		(3000 - SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE) //10mm de marge avec le bord de la serviette
		#define SMALL_TOP_COLOR_CALIBRATION_TETA	(PI4096/2)

	//BIG
		//BOT_COLOR
		#define BIG_BOT_COLOR_CALIBRATION_X  		750
		#define BIG_BOT_COLOR_CALIBRATION_Y  		BIG_CALIBRATION_FORWARD_BORDER_DISTANCE
		#define BIG_BOT_COLOR_CALIBRATION_TETA		(-PI4096/2)

		//TOP_COLOR
		#define BIG_TOP_COLOR_CALIBRATION_X  		750
		#define BIG_TOP_COLOR_CALIBRATION_Y  		(3000 - BIG_CALIBRATION_FORWARD_BORDER_DISTANCE)
		#define BIG_TOP_COLOR_CALIBRATION_TETA 		(PI4096/2)


//SMALL  Position initiale
		// BOT_COLOR
		#define SMALL_BOT_COLOR_START_X  		(1100 - SMALL_ROBOT_WIDTH/2)*65536 	// Calé au bord de la serviette, coté cabine de plage
		#define SMALL_BOT_COLOR_START_Y  		(SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE*65536)
		#define SMALL_BOT_COLOR_START_TETA 		(-6588416)			//-PI/2  (il part en marche arrière...)

		// TOP_COLOR
		#define SMALL_TOP_COLOR_START_X 		(1100 - SMALL_ROBOT_WIDTH/2)*65536   // Calé au bord de la serviette, coté cabine de plage
		#define SMALL_TOP_COLOR_START_Y 		(3000 - SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE) *65536
		#define SMALL_TOP_COLOR_START_TETA 		(6588416)				//PI/2  (il part en marche arrière...)

	//BIG
		// BOT_COLOR
		#define BIG_BOT_COLOR_START_X  	(610 + BIG_ROBOT_WIDTH/2)*65536	   // Calé au bord de la serviette, coté mer
		#define BIG_BOT_COLOR_START_Y  	(BIG_CALIBRATION_FORWARD_BORDER_DISTANCE)*65536  //calé contre la bordure
		#define BIG_BOT_COLOR_START_TETA 	(-6588416)				//PI/2  (il part en marche avant...)

		// TOP_COLOR
		#define BIG_TOP_COLOR_START_X  (610 + BIG_ROBOT_WIDTH/2)*65536	  // Calé au bord de la serviette, coté mer
		#define BIG_TOP_COLOR_START_Y  (3000 - BIG_CALIBRATION_FORWARD_BORDER_DISTANCE)*65536  //calé contre la bordure
		#define BIG_TOP_COLOR_START_TETA 	(6588416)			//-PI/2  (il part en marche avant...)



/////BUFFER////////////////////////////////////////////
	#define BUFFER_SIZE 64	//maximum : 255
		//ATTENTION, fortement lié à l'espace occupé en RAM !!!
		//Une case du buffer "coûte" 22 octets (ordre de grandeur)



////////REGLAGES DU CORRECTEUR PD////////////////////////
	#define SMALL_KD_TRANSLATION 	(0x20) //0x80
	#define BIG_KD_TRANSLATION 	(0x20) //0x80

	#define SMALL_KP_TRANSLATION 	(0x20)
	#define BIG_KP_TRANSLATION 	(0x30)
	// Sur archi'tech (2009) il s'est avéré meilleur de scinder les deux coeffs selon le sens de rotation...(POSITIF, NEGATIF)
	//Etaient alors définis deux coeffs pour le D et de pour le P : KD_ROTATION_POSITIF, KD_ROTATION_NEGATIF.....
	//en pratique, si le robot est équilibré, les coeffs sont les mêmes !
	#define SMALL_KD_ROTATION 	(0x150) //0x800
	#define BIG_KD_ROTATION 	(0x180) //0x800

	#define SMALL_KP_ROTATION 	(0x60)//40 //0x80
	#define BIG_KP_ROTATION 	(0x80) //0x80

	#define SMALL_KI_ROTATION   (0x0)
	#define BIG_KI_ROTATION     (0x05)

	//Ordre de grandeur :
	//A la vitesse lumière, KV_TRANSLATION * vitesse_translation doit valoir 100*4096
	//A la vitesse lumière, KV_ROTATION * vitesse_rotation doit valoir 100*1024
	//KVtrans  	= 16
	//KVrot = 1

	#define SMALL_KV_ROTATION 	1//CHOMP 1
	#define BIG_KV_ROTATION 	1//CHOMP 1

	#define SMALL_KV_TRANSLATION 	19
	#define BIG_KV_TRANSLATION 	12

	#ifdef CORRECTOR_ENABLE_ACCELERATION_ANTICIPATION
		#warning "mode non utilisé jusqu'à maintenant : coefs non reglés"
		#define SMALL_KA_ROTATION 	0
		#define BIG_KA_ROTATION 	0

		#define SMALL_KA_TRANSLATION 	(200)
		#define BIG_KA_TRANSLATION 	(200)
	#else
		#define SMALL_KA_ROTATION 	0
		#define BIG_KA_ROTATION 	0

		#define SMALL_KA_TRANSLATION 	0
		#define BIG_KA_TRANSLATION 	0
	#endif



/////SEUIL_TRAJECTOIRES_COURBES///////////////////////
	//Coefficients de réglages du gabarit pour les translations simples.
	#define ANGLE_MAXI_LANCEMENT_TRANSLATION  (PI4096/32)	//Si l'angle de vue est plus grand, on tournera avant d'avancer !


/////GESTION_TRAJECTOIRES/////////////////////////////

	//acc et vit a regler... IMPORTANT : fixer l'accélération quand le robot à environ son poids/comportement final, et ne plus y toucher ensuite...
	//le peu de gain qu'apporterait une modification de dernière minute serait infime devant le risque de faire foirer pas mal d'autres choses bien testées avant... comme les traj. courbes... !

	// de combien on accelere à chaque boucle d'asser
	#define SMALL_ACCELERATION_NORMAL	80	// ATTENTION : doit être un multiple de 16...	[mm/4096/5ms/5ms]
	#define SMALL_ACCELERATION_ROTATION_TRANSLATION 10			//Sur check Norris : 200mm entre les roues => 1024/50 = 20

	#define BIG_ACCELERATION_NORMAL	80	// ATTENTION : doit être un multiple de 16...	[mm/4096/5ms/5ms]
	#define BIG_ACCELERATION_ROTATION_TRANSLATION 7

	// 13 = (1024 / (distance entre les roues de propulsions divisée par 2 ) )
	//sur archi'tech : 2*80mm entre les roues
	//explication : une accélération de 1 en translation = une accélération de 13 en rotation
							//(du point de vue d'une roue)
	//ce rapport de 13 a été ramené à 8 car l'accélération en rotation ne peut pas être aussi forte que la théorie le prédit
	//il faut comprendre ce rapport comme un réglage de l'accélération en rotation par rapport à l'accélération en translation
	//ce réglage influe sur la gueule des courbes générées en trajectoires courbes


		//vitesse [mm/4096/5ms]
		/*
			24476 = 6[mm/5ms] = 1,2m/s
			16384 = 4[mm/5ms] = 0,8m/s
			8192  = 2[mm/5ms] = 0,4m/s
		*/
	// ATTENTION !!!!!!!!!!!!!!!!!!!!!!!! les valeurs ci-dessous doivent être multiples des accélérations...
	#define SMALL_TRANSLATION_SPEED_LIGHT 	(Sint32)(24576) 			//environ = 6[mm/5ms] = 1,2m/s
	#define SMALL_TRANSLATION_SPEED_MAX 		(Sint32)(10240)	//20480	//environ = 5[mm/5ms] = 1,0m/s
	#define SMALL_TRANSLATION_SPEED_LOW 		(Sint32)(10240)			//environ = 2,5[mm/5ms] = 0,5m/s
	#define SMALL_TRANSLATION_SPEED_VERY_LOW 	(Sint32)(5120)
	#define SMALL_TRANSLATION_SPEED_SNAIL		(Sint32)(516)

	//Une vitesse de 1024[rad/4096/1024/5ms] en rotation correspond à un déplacement des roues de 80[mm/4096/5ms]
	#define SMALL_ROTATION_SPEED_LIGHT 		(Sint32)(135160)			//[rad/4096/1024/5ms]
	#define SMALL_ROTATION_SPEED_MAX 			(Sint32)(135160)			//[rad/4096/1024/5ms]
	#define SMALL_ROTATION_SPEED_LOW 			(Sint32)(41140)			//environ 1,5 rad/s
	#define SMALL_ROTATION_SPEED_VERY_LOW 	(Sint32)(21120)			//environ 0,75 rad/s
	#define SMALL_ROTATION_SPEED_SNAIL		(Sint32)(1320)			//environ 0,075 rad/s

		// ATTENTION !!!!!!!!!!!!!!!!!!!!!!!! les valeurs ci-dessous doivent être multiples des accélérations...
	#define BIG_TRANSLATION_SPEED_LIGHT 	(Sint32)(24576) 			//environ = 6[mm/5ms] = 1,2m/s
	#define BIG_TRANSLATION_SPEED_MAX 		(Sint32)(22448)		 	//environ = 5[mm/5ms] = 1,0m/s
	#define BIG_TRANSLATION_SPEED_LOW 		(Sint32)(8256)			//environ = 2[mm/5ms] = 0,4m/s
	#define BIG_TRANSLATION_SPEED_VERY_LOW 	(Sint32)(4128)
	#define BIG_TRANSLATION_SPEED_SNAIL		(Sint32)(516)

	//Une vitesse de 1024[rad/4096/1024/5ms] en rotation correspond à un déplacement des roues de 80[mm/4096/5ms]
	#define BIG_ROTATION_SPEED_LIGHT 		(Sint32)(67580)			//[rad/4096/1024/5ms]
	#define BIG_ROTATION_SPEED_MAX 			(Sint32)(67580)			//[rad/4096/1024/5ms]
	#define BIG_ROTATION_SPEED_LOW 			(Sint32)(21120)			//environ 1,5 rad/s
	#define BIG_ROTATION_SPEED_VERY_LOW 	(Sint32)(10560)			//environ 0,75 rad/s
	#define BIG_ROTATION_SPEED_SNAIL		(Sint32)(1320)			//environ 0,075 rad/s

	//Le robot est 'arrivé' lorsque sa vitesse est faible et sa position proche :
	#define PRECISION_ARRIVE_POSITION_TRANSLATION 40960	//1cm		//81920 //2cm
	#define PRECISION_ARRIVE_SPEED_TRANSLATION (PILOT_get_coef(PILOT_ACCELERATION_NORMAL)*2)	//1mm/5ms = 2cm/s

	#define PRECISION_ARRIVE_POSITION_ROTATION (2*(PILOT_get_coef(PILOT_ROTATION_SPEED_MAX)/1024)+1) 		//TODO le 2* n'est pas justifié, mais nécessaire, coupe 2012...
	#define PRECISION_ARRIVE_SPEED_ROTATION (PILOT_get_coef(PILOT_ACCELERATION_NORMAL)*PILOT_get_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION)*2)	// rad/1024/5ms = 0,2rad/s


	//a partir de cette distance de l'arrivée, on cesse de recalculer l'angle de vue...
	//sinon, on va tourner comme des cons autour du point...
	//on risque de ne pas arriver pile poil au bon endroit...
	//et on risque d'avoir une distance qui va évoluer autour d'un minima supérieur au seuil d'arrivée...
	//ceci sera géré ailleurs !
	#define	THRESHOLD_STOPPING_CALCULATION_VIEWING_ANGLE ((Sint32)(51200)) //1,25cm // ((Sint32)(102400)) // 2,5cm  ((Sint32)(122880)) // 3cm

	#define	TRESHOLD_MAX_NUMBER_OF_ROUNDS_RETURNS 4
	//permet d'éviter les va et vient qui peuvent survenir en courbe... on s'arrete au bout de quelques va et vient...
	//réduire ce nombre empecherait à l'oeil du codeur propulsion de voir que CE BUG s'est produit ! et c'est important de le voir !

//////////////////////////////////////////////////////////////////
///////////SEUIL_ERREUR/////////////////////////////////
	#define THRESHOLD_ERROR_ROTATION 2048//1024 		//[rad/4096] = 1/4 rad

	#define TRESHOLD_ERROR_TRANSLATION 614400 //614400//204800 	//[mm/4096] = 15cm //409600//204800 	//[mm/4096]  = 10cm
		//ATTENTION, le seuil d'erreur est lié au coefficient Kp.......
		// L'erreur intervient lorsque le point fictif est trop éloigné du robot.
		//Dans une situation normale, le robot suit le point fictif de sorte que l'écart entre les deux soit proportionnel
		//à la commande moteur... (ordre de grandeur : 100% moteur pour environ 5 à 15cm...)
		// pour calculer la distance correspondant à 100% moteur (hors terme dérivé) :
		//		distance_erreur_mini = 100/Kp = 100/8 = 12.5 cm

	#define BIG_TRESHOLD_CALIBRATION_TRANSLATION 	150000 			//Attention, forcément inférieur au seuil d'erreur distance !
	#define SMALL_TRESHOLD_CALIBRATION_TRANSLATION 	100000 			//Attention, forcément inférieur au seuil d'erreur distance !
				// Attention, dépend aussi fortement de la vitesse en calage..
				//Si cette valeur est trop faible, le point fictif sera trop loin du robot à cause du besoin de vitesse...

	#define TRESHOLD_ERROR_IMMOBILITY_MAX_TIME (1000)	//Durée en ms au delà de laquelle on part en erreur sur robot "immobile non arrivé"



#endif /* ndef _PROPULSION_CONFIG_H */

