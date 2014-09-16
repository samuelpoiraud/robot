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


	//Pour l'utilisation de l'�cran LCD tactile et de la propulsion virtuelle hors du robot, activez ceci :
	//#define SIMULATION_VIRTUAL_PERFECT_ROBOT	//L'odom�trie est faite sur un robot virtuel parfait.
	//#define MODE_SIMULATION						//Dans ce mode, le bus CAN est d�sactiv�.
	//#define CAN_SEND_OVER_UART					//envoi des msg can sur l'uart, en utilisant le format normalis� des msg can over uart
	//#define LCD_TOUCH								//Active le LCD tactile

/*	MODE d'EMPLOI MODE SIMULATION ET ECRAN TACTILE (en dehors d'un fond de panier !)
 * 	 1 - activez les 4 defines ci-dessus
 * 	 2 - activez MODE_SIMULATION sur la carte STRATEGIE
 *   3 - avec 4 fils : reliez entre les cartes PROP et STRAT (�ventuellement le 5V...) :
 *   	GND<->GND
 *   	5V<->5V
 *   	PB6<->PB7
 *   	PB7<->PB6
 *   4 - d�sactivez le verbose strat�gie en reliant PA7 � un potentiel GND. (par exemple jumper entre PA7 et PA5).
 *   Vous avez un robot virtuel parfait...
 */


/* MODE d'EMPLOI : carte propulsion sur un fond de panier sans robot r�el
 *
 * Activer les define SIMULATION_VIRTUAL_PERFECT_ROBOT et CAN_SEND_OVER_UART (et c'est tout !)
 *
 */

/* ECRAN TACTILE - sans simulation (= � cot� d'un fond de panier, avec un robot virtuel ou r�el)
 *
 * Vous pouvez brancher une carte avec un LCD, avec les param�tres suivants :
 * - Activer les 4 defines ci-dessus. (oui, oui !)
 * - Relier l'UART Tx de la propulsion vers l'UART Rx de la carte supportant l'�cran tactile.
 */

//MODES INDISPENSABLES EN MATCHS
	#define PERIODE_IT_ASSER (5)	//[ms] ne pas y toucher sans savoir ce qu'on fait, (ou bien vous voulez vraiment tout casser !)

	#define ENABLE_CAN			//Activation du bus CAN...

	#define USE_CODEUR_SUR_IT_ET_QE		//Utiliser les IT externes et les QEx pour acqu�rir les infos codeurs au lieu du CPLD !

	#define USE_HOKUYO	//Active le module HOKUYO et la d�tection des ennemis... !

	#define USE_GYROSCOPE

	//#define USE_PROP_AVOIDANCE

	#define FAST_COS_SIN

	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 4
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

//MODES NON INDISPENSABLES OU INPENSABLES EN MATCHS

	#define USE_GYROSCOPE
	#define VERBOSE_MSG_SEND_OVER_UART	//A la place d'un envoi CAN, envoi d'un texte explicite sur l'UART : "Position : " ou "TrajFinie : ....."


	//#define MODE_REGLAGE_KV
	#ifdef MODE_REGLAGE_KV
		#ifndef VERBOSE_MODE
			#warning "Le mode r�glage KV a besoin du VERBOSE_MODE"
		#endif
	#endif

	#define SCAN_TRIANGLE		// Mise en service des fonctionnalit�es de scan de triangle

//	#define SOFT_SCAN_TRIANGLE	// Activation de l'envoie de chaine format�es par l'UART pour la rendue des triangles sur logiciel





//	#define MODE_PRINTF_TABLEAU		//Module permettant de visualiser apr�s coup une grande s�rie de valeur quelconque pour chaque IT...

 //	#define MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT	//Permet des affichage en d�bug d'un tas de variables
													//Mais comme le temps pass� � l'affichage est sup�rieur au rythme d'�volution des variables
													//Il est pratique de figer une sauvegarde de toutes la structure global_t et d'afficher ensuite des donn�es "coh�rentes", prises au m�me "instant" !
													//Voir le code associ� � cette macro !

//	#define SUPERVISOR_DISABLE_ERROR_DETECTION			//Dangereux, mais parfois utile !


//	#define CORRECTOR_ENABLE_ACCELERATION_ANTICIPATION //Inutile... Voir wiki...




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///ODOMETRIE////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//POUR REGLER L'ODOMETRIE:
//Consultez le Fichier: svn\propulsion\Documentation\TUTO_reglage_odometrie.txt

	#define SMALL_ODOMETRY_COEF_TRANSLATION_DEFAULT 0x0C10
	#define BIG_ODOMETRY_COEF_TRANSLATION_DEFAULT 0x0BE6  //Th�orique pour 60mm : 0x0C10
	// COEF_ODOMETRIE_TRANSLATION : nombre de mm par impulsion de roue codeuse, par 5ms
	// d : diametre des roues codeuse en mm = 60
	// c : resolution du codeur = 4000
	// COEF_ODOMETRIE_TRANSLATION = 16*4096*(PI*d)/c;	 	[mm/16/4096/impulsion/5ms]
	//		on le calcule pour l'approcher, et on le d�termine ensuite par l'exp�rience !
	//COEF_ODOMETRIE_TRANSLATION est le premier des coeffs a r�gler... il suffit d'envoyer le robot tr�s loin et de regarder s'il arrive assez loin.
	//Si le robot va trop loin, il faut augmenter le coeff et vice versa


	#define SMALL_ODOMETRY_COEF_SYM_DEFAULT (0)//(0x0010)
	#define BIG_ODOMETRY_COEF_SYM_DEFAULT (-160)

	#define SMALL_ODOMETRY_COEF_ROTATION_DEFAULT 0x00010F05	//Guy : 92mm entre roue codeuse et centre
	#define BIG_ODOMETRY_COEF_ROTATION_DEFAULT 0x0000AFC0  	//Pierre : 138mm entre roue codeuse et centre - th�orique : B308
	// COEF_ODOMETRIE_ROTATION : nombre de radians par impulsion de roue codeuse, par 5ms
	// COEF_ODOMETRIE_ROTATION = 1024*4096*64*(PI*d)/c /(2*r)	[rad/16/4096/1024/impulsions/5ms]
	// d : diametre des roues codeuse en mm = 60
	// c : resolution du codeur = 4000
	// r : rayon du cercle ayant pour diam�tre le segment d�finit entre les roues codeuses en mm
			// r d�pend du robot... sur Check Norris, r = 153 mm(145 sur chomp)
			// (on peut aussi dire que COEF_ODOMETRIE_ROTATION = 1024*COEF_ODOMETRIE_TRANSLATION/(2*r))
	// on le calcule pour l'approcher, et on le d�termine ensuite par l'exp�rience !
	//COMMENT REGLER COEF_ODOMETRIE_ROTATION ? (cela est une image de la distance entre les deux roues codeuses...)
	// 1 - la d�marche consiste � faire tourner le robot sur lui m�me et travailler par dichotomie.
	// 2 - on envoie la suite d'ordre PI > PI/2 > 0 > -PI/2... en boucle
	// > - Si le robot a trop tourn�, le coeff est TROP PETIT
	// > - Si le robot a pas assez tourn�, le coeff est TROP GRAND...

	//Amusez vous !

	//COEF_ODOMETRIE_CENTRIFUGE permet de r�gler la variation d'odom�trie en courbe
	#define SMALL_ODOMETRY_COEF_CENTRIFUGAL_DEFAULT (0)
	#define BIG_ODOMETRY_COEF_CENTRIFUGAL_DEFAULT (0)

	//� regler avec �l�ments m�caniques de blocages extr�mement parall�les � l'axe des codeurs !
	#define SMALL_CALIBRATION_BACKWARD_BORDER_DISTANCE 	81		//distance entre le 'centre' du robot et l'arri�re en calage
	#define SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE 	81 		//distance entre le 'centre' du robot et l'avant en calage

	#define BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE	155 		//distance entre le 'centre' du robot et l'arri�re en calage
	#define BIG_CALIBRATION_FORWARD_BORDER_DISTANCE 	155 		//distance entre le 'centre' du robot et l'avant en calage

	#define BIG_CALIBRATION_WIDTH					 	285 		//largeur du robot

	#define FIELD_SIZE_Y 3000	//[mm]
	#define FIELD_SIZE_X 2000	//[mm]


	//SMALL  Positin initiale
		// RED
		#define SMALL_RED_START_X  		41615360 			//635mm
		#define SMALL_RED_START_Y  		5308416				//81mm
		#define SMALL_RED_START_TETA 	(-6588416)			//-PI/2 //(-3294199)

		// YELLOW
		#define SMALL_YELLOW_START_X 		41615360 			//635mm
		#define SMALL_YELLOW_START_Y 		(196608000-5308416)	//(3000-81)mm
		#define SMALL_YELLOW_START_TETA 	(6588416)				//PI/2 //3294199

	//BIG
		// RED
		#define BIG_RED_START_X  	(730 - BIG_CALIBRATION_WIDTH/2)*65536			// 587mm
		#define BIG_RED_START_Y  	65536 * BIG_CALIBRATION_FORWARD_BORDER_DISTANCE			//155mm
		#define BIG_RED_START_TETA 	6588416				//PI/2

		// YELLOW
		#define BIG_YELLOW_START_X  (730 - BIG_CALIBRATION_WIDTH/2)*65536			// 587mm
		#define BIG_YELLOW_START_Y 	(196608000-(65536 * BIG_CALIBRATION_FORWARD_BORDER_DISTANCE)) //3000-155mm
		#define BIG_YELLOW_START_TETA 	(-6588416)			//-PI/2



	//SMALL Position calibration
		//RED
		#define SMALL_RED_CALIBRATION_X  		535
		#define SMALL_RED_CALIBRATION_Y  		134
		#define SMALL_RED_CALIBRATION_TETA		500    //environ 7�

		//YELLOW
		#define SMALL_YELLOW_CALIBRATION_X  		535
		#define SMALL_YELLOW_CALIBRATION_Y  		(3000 - 134)
		#define SMALL_YELLOW_CALIBRATION_TETA		 -500	//environ -7�

	//BIG
		//RED
		#define BIG_RED_CALIBRATION_X  		243
		#define BIG_RED_CALIBRATION_Y  		191
		#define BIG_RED_CALIBRATION_TETA	1430		//environ 20� (20*71.5)

		//YELLOW
		#define BIG_YELLOW_CALIBRATION_X  		243
		#define BIG_YELLOW_CALIBRATION_Y  		(3000 - 200)
		#define BIG_YELLOW_CALIBRATION_TETA 	11440	 //-20� + 180� fait une rotation � avoir les balles du bon c�t�




/////BUFFER////////////////////////////////////////////
	#define BUFFER_SIZE 64	//maximum : 255
		//ATTENTION, fortement li� � l'espace occup� en RAM !!!
		//Une case du buffer "co�te" 22 octets (ordre de grandeur)



////////REGLAGES DU CORRECTEUR PD////////////////////////
	#define SMALL_KD_TRANSLATION 	(12) //0x80
	#define BIG_KD_TRANSLATION 	(0x36) //0x80

	#define SMALL_KP_TRANSLATION 	(34)
	#define BIG_KP_TRANSLATION 	(17)
	// Sur archi'tech (2009) il s'est av�r� meilleur de scinder les deux coeffs selon le sens de rotation...(POSITIF, NEGATIF)
	//Etaient alors d�finis deux coeffs pour le D et de pour le P : KD_ROTATION_POSITIF, KD_ROTATION_NEGATIF.....
	//en pratique, si le robot est �quilibr�, les coeffs sont les m�mes !
	#define SMALL_KD_ROTATION 	(288) //0x800
	#define BIG_KD_ROTATION 	(0x200) //0x800

	#define SMALL_KP_ROTATION 	(130)//40 //0x80
	#define BIG_KP_ROTATION 	(0xF0) //0x80

	//Ordre de grandeur :
	//A la vitesse lumi�re, KV_TRANSLATION * vitesse_translation doit valoir 100*4096
	//A la vitesse lumi�re, KV_ROTATION * vitesse_rotation doit valoir 100*1024
	//KVtrans  	= 16
	//KVrot = 1

	#define SMALL_KV_ROTATION 	1//CHOMP 1
	#define BIG_KV_ROTATION 	1//CHOMP 1

	#define SMALL_KV_TRANSLATION 	19
	#define BIG_KV_TRANSLATION 	12

	#ifdef CORRECTOR_ENABLE_ACCELERATION_ANTICIPATION
		#warning "mode non utilis� jusqu'� maintenant : coefs non regl�s"
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
	//Coefficients de r�glages du gabarit pour les translations simples.
	#define ANGLE_MAXI_LANCEMENT_TRANSLATION  (PI4096/32)	//Si l'angle de vue est plus grand, on tournera avant d'avancer !


/////GESTION_TRAJECTOIRES/////////////////////////////

	//acc et vit a regler... IMPORTANT : fixer l'acc�l�ration quand le robot � environ son poids/comportement final, et ne plus y toucher ensuite...
	//le peu de gain qu'apporterait une modification de derni�re minute serait infime devant le risque de faire foirer pas mal d'autres choses bien test�es avant... comme les traj. courbes... !

	// de combien on accelere � chaque boucle d'asser
	#define SMALL_ACCELERATION_NORMAL	48	// ATTENTION : doit �tre un multiple de 16...	[mm/4096/5ms/5ms]
	#define SMALL_ACCELERATION_ROTATION_TRANSLATION 10			//Sur check Norris : 200mm entre les roues => 1024/50 = 20

	#define BIG_ACCELERATION_NORMAL	80	// ATTENTION : doit �tre un multiple de 16...	[mm/4096/5ms/5ms]
	#define BIG_ACCELERATION_ROTATION_TRANSLATION 7

	// 13 = (1024 / (distance entre les roues de propulsions divis�e par 2 ) )
	//sur archi'tech : 2*80mm entre les roues
	//explication : une acc�l�ration de 1 en translation = une acc�l�ration de 13 en rotation
							//(du point de vue d'une roue)
	//ce rapport de 13 a �t� ramen� � 8 car l'acc�l�ration en rotation ne peut pas �tre aussi forte que la th�orie le pr�dit
	//il faut comprendre ce rapport comme un r�glage de l'acc�l�ration en rotation par rapport � l'acc�l�ration en translation
	//ce r�glage influe sur la gueule des courbes g�n�r�es en trajectoires courbes


		//vitesse [mm/4096/5ms]
		/*
			24476 = 6[mm/5ms] = 1,2m/s
			16384 = 4[mm/5ms] = 0,8m/s
			8192  = 2[mm/5ms] = 0,4m/s
		*/
	// ATTENTION !!!!!!!!!!!!!!!!!!!!!!!! les valeurs ci-dessous doivent �tre multiples des acc�l�rations...
	#define SMALL_TRANSLATION_SPEED_LIGHT 	(Sint32)(24576) 			//environ = 6[mm/5ms] = 1,2m/s
	#define SMALL_TRANSLATION_SPEED_MAX 		(Sint32)(20448)		 	//environ = 5[mm/5ms] = 1,0m/s
	#define SMALL_TRANSLATION_SPEED_LOW 		(Sint32)(8256)			//environ = 2[mm/5ms] = 0,4m/s
	#define SMALL_TRANSLATION_SPEED_VERY_LOW 	(Sint32)(4128)
	#define SMALL_TRANSLATION_SPEED_SNAIL		(Sint32)(516)

	//Une vitesse de 1024[rad/4096/1024/5ms] en rotation correspond � un d�placement des roues de 80[mm/4096/5ms]
	#define SMALL_ROTATION_SPEED_LIGHT 		(Sint32)(135160)			//[rad/4096/1024/5ms]
	#define SMALL_ROTATION_SPEED_MAX 			(Sint32)(135160)			//[rad/4096/1024/5ms]
	#define SMALL_ROTATION_SPEED_LOW 			(Sint32)(41140)			//environ 1,5 rad/s
	#define SMALL_ROTATION_SPEED_VERY_LOW 	(Sint32)(21120)			//environ 0,75 rad/s
	#define SMALL_ROTATION_SPEED_SNAIL		(Sint32)(1320)			//environ 0,075 rad/s

		// ATTENTION !!!!!!!!!!!!!!!!!!!!!!!! les valeurs ci-dessous doivent �tre multiples des acc�l�rations...
	#define BIG_TRANSLATION_SPEED_LIGHT 	(Sint32)(24576) 			//environ = 6[mm/5ms] = 1,2m/s
	#define BIG_TRANSLATION_SPEED_MAX 		(Sint32)(22448)		 	//environ = 5[mm/5ms] = 1,0m/s
	#define BIG_TRANSLATION_SPEED_LOW 		(Sint32)(8256)			//environ = 2[mm/5ms] = 0,4m/s
	#define BIG_TRANSLATION_SPEED_VERY_LOW 	(Sint32)(4128)
	#define BIG_TRANSLATION_SPEED_SNAIL		(Sint32)(516)

	//Une vitesse de 1024[rad/4096/1024/5ms] en rotation correspond � un d�placement des roues de 80[mm/4096/5ms]
	#define BIG_ROTATION_SPEED_LIGHT 		(Sint32)(67580)			//[rad/4096/1024/5ms]
	#define BIG_ROTATION_SPEED_MAX 			(Sint32)(67580)			//[rad/4096/1024/5ms]
	#define BIG_ROTATION_SPEED_LOW 			(Sint32)(21120)			//environ 1,5 rad/s
	#define BIG_ROTATION_SPEED_VERY_LOW 	(Sint32)(10560)			//environ 0,75 rad/s
	#define BIG_ROTATION_SPEED_SNAIL		(Sint32)(1320)			//environ 0,075 rad/s



	//Le robot est 'arriv�' lorsque sa vitesse est faible et sa position proche :
	#define PRECISION_ARRIVE_POSITION_TRANSLATION 40960	//1cm		//81920 //2cm
	#define PRECISION_ARRIVE_SPEED_TRANSLATION (PILOT_get_coef(PILOT_ACCELERATION_NORMAL)*2)	//1mm/5ms = 2cm/s

	#define PRECISION_ARRIVE_POSITION_ROTATION (2*(PILOT_get_coef(PILOT_ROTATION_SPEED_MAX)/1024)+1) 		//TODO le 2* n'est pas justifi�, mais n�cessaire, coupe 2012...
	#define PRECISION_ARRIVE_SPEED_ROTATION (PILOT_get_coef(PILOT_ACCELERATION_NORMAL)*PILOT_get_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION)*2)	// rad/1024/5ms = 0,2rad/s


	//a partir de cette distance de l'arriv�e, on cesse de recalculer l'angle de vue...
	//sinon, on va tourner comme des cons autour du point...
	//on risque de ne pas arriver pile poil au bon endroit...
	//et on risque d'avoir une distance qui va �voluer autour d'un minima sup�rieur au seuil d'arriv�e...
	//ceci sera g�r� ailleurs !
	#define	THRESHOLD_STOPPING_CALCULATION_VIEWING_ANGLE ((Sint32)(51200)) //1,25cm // ((Sint32)(102400)) // 2,5cm  ((Sint32)(122880)) // 3cm

	#define	TRESHOLD_MAX_NUMBER_OF_ROUNDS_RETURNS 4
	//permet d'�viter les va et vient qui peuvent survenir en courbe... on s'arrete au bout de quelques va et vient...
	//r�duire ce nombre empecherait � l'oeil du codeur propulsion de voir que CE BUG s'est produit ! et c'est important de le voir !

//////////////////////////////////////////////////////////////////
///////////SEUIL_ERREUR/////////////////////////////////
	#define THRESHOLD_ERROR_ROTATION 2048//1024 		//[rad/4096] = 1/4 rad

	#define TRESHOLD_ERROR_TRANSLATION 614400 //614400//204800 	//[mm/4096] = 15cm //409600//204800 	//[mm/4096]  = 10cm
		//ATTENTION, le seuil d'erreur est li� au coefficient Kp.......
		// L'erreur intervient lorsque le point fictif est trop �loign� du robot.
		//Dans une situation normale, le robot suit le point fictif de sorte que l'�cart entre les deux soit proportionnel
		//� la commande moteur... (ordre de grandeur : 100% moteur pour environ 5 � 15cm...)
		// pour calculer la distance correspondant � 100% moteur (hors terme d�riv�) :
		//		distance_erreur_mini = 100/Kp = 100/8 = 12.5 cm

	#define BIG_TRESHOLD_CALIBRATION_TRANSLATION 	150000 			//Attention, forc�ment inf�rieur au seuil d'erreur distance !
	#define SMALL_TRESHOLD_CALIBRATION_TRANSLATION 	100000 			//Attention, forc�ment inf�rieur au seuil d'erreur distance !
				// Attention, d�pend aussi fortement de la vitesse en calage..
				//Si cette valeur est trop faible, le point fictif sera trop loin du robot � cause du besoin de vitesse...

	#define TRESHOLD_ERROR_IMMOBILITY_MAX_TIME (1000)	//Dur�e en ms au del� de laquelle on part en erreur sur robot "immobile non arriv�"



#endif /* ndef _PROPULSION_CONFIG_H */

