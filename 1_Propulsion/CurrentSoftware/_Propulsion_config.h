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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///MODES////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Pour l'utilisation de l'écran LCD tactile et de la propulsion virtuelle hors du robot, activez ceci :
	//#define SIMULATION_VIRTUAL_PERFECT_ROBOT	//L'odométrie est faite sur un robot virtuel parfait.
	//#define MODE_SIMULATION						//Dans ce mode, le bus CAN est désactivé.
	//#define CAN_SEND_OVER_UART					//envoi des msg can sur l'uart, en utilisant le format normalisé des msg can over uart
	//#define LCD_TOUCH								//Active le LCD tactile

/*	MODE d'EMPLOI MODE SIMULATION AVEC ECRAN TACTILE
 * 	 1 - activez les 4 defines ci-dessus
 * 	 2 - activez MODE_SIMULATION sur la carte STRATEGIE
 *   3 - avec 4 fils : reliez entre les cartes PROP et STRAT (éventuellement le 5V...) :
 *   	GND<->GND
 *   	5V<->5V
 *   	PB6<->PB7
 *   	PB7<->PB6
 *   4 - désactivez le verbose stratégie en reliant PA7 à un potentiel GND. (par exemple jumper entre PA7 et PA5).
 *   Vous avez un robot virtuel parfait...
 */

//MODES INDISPENSABLES EN MATCHS
	#define PERIODE_IT_ASSER (5)	//[ms] ne pas y toucher sans savoir ce qu'on fait, (ou bien vous voulez vraiment tout casser !)

	#define ENABLE_CAN			//Activation du bus CAN...

	#define USE_CODEUR_SUR_IT_ET_QE		//Utiliser les IT externes et les QEx pour acquérir les infos codeurs au lieu du CPLD !

	#define USE_HOKUYO	//Active le module HOKUYO et la détection des ennemis... !

//MODES NON INDISPENSABLES OU INPENSABLES EN MATCHS


	#define VERBOSE_MSG_SEND_OVER_UART	//A la place d'un envoi CAN, envoi d'un texte explicite sur l'UART : "Position : " ou "TrajFinie : ....."


	//#define MODE_REGLAGE_KV
	#ifdef MODE_REGLAGE_KV
		#ifndef VERBOSE_MODE
			#warning "Le mode réglage KV a besoin du VERBOSE_MODE"
		#endif
	#endif

//	#define SCAN_TRIANGLE		// Mise en service des fonctionnalitées de scan de triangle

//	#define SOFT_SCAN_TRIANGLE	// Activation de l'envoie de chaine formatées par l'UART pour la rendue des triangles sur logiciel





//	#define MODE_PRINTF_TABLEAU		//Module permettant de visualiser après coup une grande série de valeur quelconque pour chaque IT...

 //	#define MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT	//Permet des affichage en débug d'un tas de variables
													//Mais comme le temps passé à l'affichage est supérieur au rythme d'évolution des variables
													//Il est pratique de figer une sauvegarde de toutes la structure global_t et d'afficher ensuite des données "cohérentes", prises au même "instant" !
													//Voir le code associé à cette macro !

//	#define SUPERVISOR_DISABLE_ERROR_DETECTION			//Dangereux, mais parfois utile !


//	#define CORRECTOR_ENABLE_ACCELERATION_ANTICIPATION //Inutile... Voir wiki...




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///ODOMETRIE////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//POUR REGLER L'ODOMETRIE:
//Consultez le Fichier: svn\propulsion\Documentation\TUTO_reglage_odometrie.txt

	#define TINY_ODOMETRY_COEF_TRANSLATION_DEFAULT 0x0C10
	#define KRUSTY_ODOMETRY_COEF_TRANSLATION_DEFAULT 0x0C43  //Original 0x0C47
	// COEF_ODOMETRIE_TRANSLATION : nombre de mm par impulsion de roue codeuse, par 5ms
	// d : diametre des roues codeuse en mm = 60
	// c : resolution du codeur = 4000
	// COEF_ODOMETRIE_TRANSLATION = 16*4096*(PI*d)/c;	 	[mm/16/4096/impulsion/5ms]
	//		on le calcule pour l'approcher, et on le détermine ensuite par l'expérience !
	//COEF_ODOMETRIE_TRANSLATION est le premier des coeffs a régler... il suffit d'envoyer le robot très loin et de regarder s'il arrive assez loin.
	//Si le robot va trop loin, il faut augmenter le coeff et vice versa


	#define TINY_ODOMETRY_COEF_SYM_DEFAULT (0)
	#define KRUSTY_ODOMETRY_COEF_SYM_DEFAULT (0)

	#define TINY_ODOMETRY_COEF_ROTATION_DEFAULT 0x00010AC0//0XA054	//(CHOMP : 43632)	Théorique : 41335
	#define KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT 0x0000C581  // Original 0x0000C5A2
	// COEF_ODOMETRIE_ROTATION : nombre de radians par impulsion de roue codeuse, par 5ms
	// COEF_ODOMETRIE_ROTATION = 1024*4096*16*(PI*d)/c /(2*r)	[rad/16/4096/1024/impulsions/5ms]
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
	#define TINY_ODOMETRY_COEF_CENTRIFUGAL_DEFAULT (0)
	#define KRUSTY_ODOMETRY_COEF_CENTRIFUGAL_DEFAULT (0)

	//à regler avec éléments mécaniques de blocages extrèmement parallèles à l'axe des codeurs !
	#define TINY_CALIBRATION_BACKWARD_BORDER_DISTANCE 	74 		//distance entre le 'centre' du robot et l'arrière en calage
	#define TINY_CALIBRATION_FORWARD_BORDER_DISTANCE 	74 		//distance entre le 'centre' du robot et l'avant en calage

	#define KRUSTY_CALIBRATION_BACKWARD_BORDER_DISTANCE 120 		//distance entre le 'centre' du robot et l'arrière en calage
	#define KRUSTY_CALIBRATION_FORWARD_BORDER_DISTANCE 	120 		//distance entre le 'centre' du robot et l'avant en calage

	#define FIELD_SIZE_Y 3000	//[mm]
	#define FIELD_SIZE_X 2000	//[mm]

	//TINY
		// RED
		#define TINY_RED_START_X  		16384000 			//250mm
		#define TINY_RED_START_Y  		4718592				//72mm
		#define TINY_RED_START_TETA 	(6588416)			//-PI/2 //(-3294199)

		// BLUE
		#define TINY_BLUE_START_X 		16384000 			//250mm
		#define TINY_BLUE_START_Y 		(196608000-4718592)	//(3000-72)mm
		#define TINY_BLUE_START_TETA 	(-6588416)				//-PI/2 //3294199

	//KRUSTY
		// RED
		#define KRUSTY_RED_START_X  	65536000 			//1000mm
		#define KRUSTY_RED_START_Y  	7864200 			//(120mm)//symétrique
		#define KRUSTY_RED_START_TETA 	6588416				//PI/2

		// BLUE
		#define KRUSTY_BLUE_START_X 	65536000 			//1000mm
		#define KRUSTY_BLUE_START_Y 	(196608000-7864200) //2000-120mm
		#define KRUSTY_BLUE_START_TETA 	(-6588416)			//-PI/2


/////BUFFER////////////////////////////////////////////
	#define BUFFER_SIZE 64	//maximum : 255
		//ATTENTION, fortement lié à l'espace occupé en RAM !!!
		//Une case du buffer "coûte" 22 octets (ordre de grandeur)



////////REGLAGES DU CORRECTEUR PD////////////////////////
	#define TINY_KD_TRANSLATION 	(0x05) //0x80
	#define KRUSTY_KD_TRANSLATION 	(0x36) //0x80

	#define TINY_KP_TRANSLATION 	(0x01) //0x03
	#define KRUSTY_KP_TRANSLATION 	(0x02) //0x03
	// Sur archi'tech (2009) il s'est avéré meilleur de scinder les deux coeffs selon le sens de rotation...(POSITIF, NEGATIF)
	//Etaient alors définis deux coeffs pour le D et de pour le P : KD_ROTATION_POSITIF, KD_ROTATION_NEGATIF.....
	//en pratique, si le robot est équilibré, les coeffs sont les mêmes !
	#define TINY_KD_ROTATION 	(0x100) //0x800
	#define KRUSTY_KD_ROTATION 	(0x800) //0x800

	#define TINY_KP_ROTATION 	(0x50)//40 //0x80
	#define KRUSTY_KP_ROTATION 	(0xA0) //0x80

	//Ordre de grandeur :
	//A la vitesse lumière, KV_TRANSLATION * vitesse_translation doit valoir 100*4096
	//A la vitesse lumière, KV_ROTATION * vitesse_rotation doit valoir 100*1024
	//KVtrans  	= 16
	//KVrot = 1

	#define TINY_KV_ROTATION 	1//CHOMP 1
	#define KRUSTY_KV_ROTATION 	1//CHOMP 1

	#define TINY_KV_TRANSLATION 	19
	#define KRUSTY_KV_TRANSLATION 	19

	#ifdef CORRECTOR_ENABLE_ACCELERATION_ANTICIPATION
		#warning "mode non utilisé jusqu'à maintenant : coefs non reglés"
		#define TINY_KA_ROTATION 	0
		#define KRUSTY_KA_ROTATION 	0

		#define TINY_KA_TRANSLATION 	(200)
		#define KRUSTY_KA_TRANSLATION 	(200)
	#else
		#define TINY_KA_ROTATION 	0
		#define KRUSTY_KA_ROTATION 	0

		#define TINY_KA_TRANSLATION 	0
		#define KRUSTY_KA_TRANSLATION 	0
	#endif



/////SEUIL_TRAJECTOIRES_COURBES///////////////////////
	//Coefficients de réglages du gabarit pour les translations simples.
	#define ANGLE_MAXI_LANCEMENT_TRANSLATION  (PI4096/32)	//Si l'angle de vue est plus grand, on tournera avant d'avancer !


/////GESTION_TRAJECTOIRES/////////////////////////////

	//acc et vit a regler... IMPORTANT : fixer l'accélération quand le robot à environ son poids/comportement final, et ne plus y toucher ensuite...
	//le peu de gain qu'apporterait une modification de dernière minute serait infime devant le risque de faire foirer pas mal d'autres choses bien testées avant... comme les traj. courbes... !

	// de combien on accelere à chaque boucle d'asser
	#define TINY_ACCELERATION_NORMAL	64	// ATTENTION : doit être un multiple de 16...	[mm/4096/5ms/5ms]
	#define TINY_ACCELERATION_ROTATION_TRANSLATION 10			//Sur check Norris : 200mm entre les roues => 1024/50 = 20

	#define KRUSTY_ACCELERATION_NORMAL	112	// ATTENTION : doit être un multiple de 16...	[mm/4096/5ms/5ms]
	#define KRUSTY_ACCELERATION_ROTATION_TRANSLATION 12

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
	#define TINY_TRANSLATION_SPEED_LIGHT 	(Sint32)(24576) 			//environ = 6[mm/5ms] = 1,2m/s
	#define TINY_TRANSLATION_SPEED_MAX 		(Sint32)(20448)		 	//environ = 5[mm/5ms] = 1,0m/s
	#define TINY_TRANSLATION_SPEED_LOW 		(Sint32)(8256)			//environ = 2[mm/5ms] = 0,4m/s
	#define TINY_TRANSLATION_SPEED_VERY_LOW 	(Sint32)(4128)
	#define TINY_TRANSLATION_SPEED_SNAIL		(Sint32)(516)

	//Une vitesse de 1024[rad/4096/1024/5ms] en rotation correspond à un déplacement des roues de 80[mm/4096/5ms]
	#define TINY_ROTATION_SPEED_LIGHT 		(Sint32)(135160)			//[rad/4096/1024/5ms]
	#define TINY_ROTATION_SPEED_MAX 			(Sint32)(135160)			//[rad/4096/1024/5ms]
	#define TINY_ROTATION_SPEED_LOW 			(Sint32)(41140)			//environ 1,5 rad/s
	#define TINY_ROTATION_SPEED_VERY_LOW 	(Sint32)(21120)			//environ 0,75 rad/s
	#define TINY_ROTATION_SPEED_SNAIL		(Sint32)(1320)			//environ 0,075 rad/s

		// ATTENTION !!!!!!!!!!!!!!!!!!!!!!!! les valeurs ci-dessous doivent être multiples des accélérations...
	#define KRUSTY_TRANSLATION_SPEED_LIGHT 	(Sint32)(24576) 			//environ = 6[mm/5ms] = 1,2m/s
	#define KRUSTY_TRANSLATION_SPEED_MAX 		(Sint32)(20448)		 	//environ = 5[mm/5ms] = 1,0m/s
	#define KRUSTY_TRANSLATION_SPEED_LOW 		(Sint32)(8256)			//environ = 2[mm/5ms] = 0,4m/s
	#define KRUSTY_TRANSLATION_SPEED_VERY_LOW 	(Sint32)(4128)
	#define KRUSTY_TRANSLATION_SPEED_SNAIL		(Sint32)(516)

	//Une vitesse de 1024[rad/4096/1024/5ms] en rotation correspond à un déplacement des roues de 80[mm/4096/5ms]
	#define KRUSTY_ROTATION_SPEED_LIGHT 		(Sint32)(135160)			//[rad/4096/1024/5ms]
	#define KRUSTY_ROTATION_SPEED_MAX 			(Sint32)(135160)			//[rad/4096/1024/5ms]
	#define KRUSTY_ROTATION_SPEED_LOW 			(Sint32)(21120)			//environ 1,5 rad/s
	#define KRUSTY_ROTATION_SPEED_VERY_LOW 	(Sint32)(10560)			//environ 0,75 rad/s
	#define KRUSTY_ROTATION_SPEED_SNAIL		(Sint32)(1320)			//environ 0,075 rad/s



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
	#define	THRESHOLD_STOPPING_CALCULATION_VIEWING_ANGLE ((Sint32)(204800)) //5cm // ((Sint32)(102400)) // 2,5cm  ((Sint32)(122880)) // 3cm

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

	#define TRESHOLD_CALIBRATION_TRANSLATION 200000 			//Attention, forcément inférieur au seuil d'erreur distance !
				// Attention, dépend aussi fortement de la vitesse en calage..
				//Si cette valeur est trop faible, le point fictif sera trop loin du robot à cause du besoin de vitesse...

	#define TRESHOLD_ERROR_IMMOBILITY_MAX_TIME (1000)	//Durée en ms au delà de laquelle on part en erreur sur robot "immobile non arrivé"



#endif /* ndef _PROPULSION_CONFIG_H */

