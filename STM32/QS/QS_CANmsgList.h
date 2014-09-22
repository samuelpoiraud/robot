/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_CANmsgList.h
 *  Package : Qualit� Soft
 *  Description : alias des sid des messages CAN
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20101216
 */

#ifndef QS_CANMSGLIST_H
	#define QS_CANMSGLIST_H

	/* Masque des cartes (des destinataires) */
	#define MASK_BITS					0x700
	#define BROADCAST_FILTER			0x000
	#define PROP_FILTER					0x100
	#define STRAT_FILTER				0x200
	#define ACT_FILTER					0x300
	#define BALISE_FILTER				0x400
	#define DEBUG_FILTER				0x700
	#define IHM_FILTER					0x600
	//Ces messages ne sont pas destin�s � voyager sur les bus CAN des robot.
	#define XBEE_FILTER					0x500

	/***********************************************************
						Message pour tous
	***********************************************************/
	#define BROADCAST_START	 			0x001
	#define BROADCAST_STOP_ALL 			0x002
	#define BROADCAST_COULEUR			0x003			// couleur de notre robot : (Uint8):(RED=0, YELLOW =1)
	#define BROADCAST_POSITION_ROBOT	0x004
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
		Raison : Uint8 (Raison de l'envoi du Broadcast... s'agit-il d'un avertisseur ? d'un envoi p�riodique ? sur simple demande ?...)
			Remarque : Il est possible de CUMULER plusieurs raisons !!!
	*/
		#define WARNING_NO					(0b00000000)
		#define WARNING_TIMER				(0b00000001)
		#define WARNING_TRANSLATION			(0b00000010)
		#define WARNING_ROTATION			(0b00000100)
		#define WARNING_REACH_X				(0b00001000)		//Nous venons d'atteindre une position en X pour laquelle on nous a demand� une surveillance.
		#define WARNING_REACH_Y				(0b00010000)		//Nous venons d'atteindre une position en Y pour laquelle on nous a demand� une surveillance.
		#define WARNING_REACH_TETA			(0b00100000)		//Nous venons d'atteindre une position en Teta pour laquelle on nous a demand� une surveillance.
		#define WARNING_NEW_TRAJECTORY		(0b01000000)		//Changement de trajectoire (attention, cela inclue les trajectoires pr�alables ajout�es en propulsion...)
	/*
		Status : Uint8 (�tat actuel de la carte propulsion...)
			error_byte = ((Uint8)(COPILOT_get_trajectory()) << 5 | (Uint8)(COPILOT_get_way())) << 3 | (Uint8)(error_source);
		 8 bits  : T R x W W E E E
			 T : TRUE si robot en translation
			 R : TRUE si robot en rotation
			 x : non utilis�
			 WW : Way, sens actuel
				ANY_WAY						= 0,
				BACKWARD					= 1,
				FORWARD						= 2,
			 EEE : Erreur
				NO_ERROR = 0,
				UNABLE_TO_GO_ERROR,
				IMMOBILITY_ERROR,
				ROUNDS_RETURNS_ERROR,
				UNKNOW_ERROR
	*/

	#define BROADCAST_ALIM				0x005		// Message envoy� lors d'un changement d'�tat de l'alimentation 24V
		#define ALIM_OFF					(0b00000000)
		#define ALIM_ON						(0b00000001)
	/*	argument (size = 3) :
	 *	octet 0	: Etat de l'alimentation | ALIM_OFF / ALIM_ON
	 *	octets 1 et 2	: Mesure de l'alimentation sur 16 bit en mV
	 */

	#define BROADCAST_BEACON_ADVERSARY_POSITION_IR	0x006	//Balise InfraRouge


	/***********************************************************
						Message pour personne
	***********************************************************/
	#define DEBUG_CARTE_P				0x742
	#define DEBUG_FOE_POS				0x748
	/* arguments (size = 8) :
	 *	Uint8	octet d'erreur (toujours � 0)
	 *	Sint16	angle de vue de l'adversaire [rad/4096] (de -PI4096 � PI4096)
	 *	Uint8	distance de vue de l'adversaire [cm] (ATTENTION, unit� = cm !!! donc maximum = 2,55m)
	 *	Sint16	position adverse x	[mm]	(ATTENTION, peut �tre n�gatif si on le voit pr�t de la bordure avec une petite erreur...)
	 *	Sint16  position adverse y	[mm]	(idem)
	 */
	#define DEBUG_ELEMENT_UPDATED		0x749
	/* arguments (size = 6) :
	 *	Uint8	raison (ADD, UPDATE, DELETE)
	 *	Uint8 	type de pion (cf. module element)
	 *	Sint16	position element x	[mm]
	 *	Sint16	position element y	[mm]
	*/
	#define DEBUG_BEACON_US1_ERROR      0x750  //Envoy� par la strat
	#define DEBUG_BEACON_US2_ERROR      0x751  //Envoy� par la strat
	#define DEBUG_BEACON_US_POWER       0x752  //Enovy� par la balise receptrice US (mais non activ� actuellement)
	#define IR_ERROR_RESULT				0x753	//Message de la super pour l'EEPROM -> compteur des erreurs de balise
	#define US_ERROR_RESULT				0x754	//Message de la super pour l'EEPROM -> compteur des erreurs de balise
	#define DEBUG_FOE_REASON			0x755	//Message de debug qui explique la raison d 'un evitement
	#define DEBUG_US_NOT_RELIABLE		0x756	//Message qui indique que on ne fait plus confiance aux ultrasons

	#define DEBUG_STRAT_STATE_CHANGED	0x760  //Envoy� par la strat quand un �tat change d'une machine � �tat
		//Param�tres: data[0]:data[1] = ID d'une machine � �tat (data[0] le poids fort), data[2] = old_state, data[3] = new_state, data suivant: param�tres divers


	#define DEBUG_ENABLE_MODE_BEST_EFFORT							0x70F
	#define DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT					0x710

	#define DEBUG_DO_TRAJECTORY_FOR_TEST_COEFS						0x711
	#define DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE					0x712

	#define DEBUG_PROPULSION_SET_ACCELERATION						0x713		//Data sur 16 bits. (unit� : mm/4096/5ms/5ms) Grandeur typique : 100
		//Uint16 : dur�e de la trajectoire en [ms]

	#define DEBUG_SET_ERROR_TRESHOLD_TRANSLATION					0x714
		//Sint32 sur data 0 � 3 : en mm.
	typedef enum
	{
		ODOMETRY_COEF_TRANSLATION = 0,
		ODOMETRY_COEF_SYM,
		ODOMETRY_COEF_ROTATION,
		ODOMETRY_COEF_CENTRIFUGAL,		//attention, la valeur de ODOMETRY_COEF_CENTRIFUGAL est utilis� comme borne dans le code de propulsion, il faut le laisser en dernier dans les coefs d'odom�trie !
		CORRECTOR_COEF_KP_TRANSLATION,
		CORRECTOR_COEF_KD_TRANSLATION,
		CORRECTOR_COEF_KV_TRANSLATION,
		CORRECTOR_COEF_KA_TRANSLATION,
		CORRECTOR_COEF_KP_ROTATION,
		CORRECTOR_COEF_KD_ROTATION,
		CORRECTOR_COEF_KV_ROTATION,
		CORRECTOR_COEF_KA_ROTATION,
		PROPULSION_NUMBER_COEFS
	}PROPULSION_coef_e;

	#define DEBUG_PROPULSION_GET_COEFS								0x720
		//Demande � connaitre l'ensemble des coefs de la propulsion
	#define DEBUG_PROPULSION_COEF_IS								0x721
		//data[0] : id du coef
		//datas[1 � 4] : coef  !!! sur 32 bits !!!
	#define DEBUG_PROPULSION_SET_COEF								0x722
		//data[0] : id du coef
		//datas[1 � 4] : coef !!! sur 32 bits !!!

	#define DEBUG_RTC_GET				(0x780)
	#define DEBUG_RTC_SET				(0x781)
					/*	GET et SET :
						Uint8 secondes
						Uint8 minutes
						Uint8 hours
						Uint8 day
						Uint8 date
						Uint8 months
						Uint8 year	(11 pour 2011)
					*/
	#define	DEBUG_RTC_TIME				(0x782)

	//message pour debug strategie

	#define DEBUG_DETECT_FOE			0x799	//D�clenchement manuel d'un �vitement.

/*****************************************************************
 *
 *		Messages echang�s entre les cartes strat�gies des
 *		deux robots via le Xbee
 *
 *****************************************************************/
/*	Envoi d'update des elements de l'environnement	*/
	//Envois
	#define XBEE_START_MATCH				0x5A4	//Ce message est envoy� pour lancer le match de l'autre robot
	#define XBEE_PING						0x516	//PING = pInG = p1n6, Ce message est envoy� pour pinger l'autre carte strat�gie
	#define XBEE_PONG						0x506	//PONG = pOnG = p0n6, Ce message est envoy� en r�ponse d'un ping vers l'autre carte strat�gie
	#define XBEE_REACH_POINT_GET_OUT_INIT	0x507	// Envoie un message quand il sort de la zone de d�part
	#define XBEE_REACH_POINT_C1				0x508	// Envoie un message quand il arrive au point C1

	#define XBEE_TORCH_NEW_POS				0x509	// Envoie la nouvelle position de la torche a l autre robot si l'un des deux a d�plac�e une torche
	#define XBEE_GUY_HAVE_DONE_FIRE			0x50B	// Informe Pierre que Guy a d�cid� de prendre un feu ou une torche
		//Data[0] : ID du fire
		typedef enum
		{									//ATTENTION, SI VOUS CHANGEZ CETTE ENUM, VOUS DEVEZ CHANGER AUSSI fire_id_string dans Verbose_can_msg
			FIRE_ID_TORCH_OUR = 0x00,
			FIRE_ID_ADV_TORCH,
			FIRE_ID_START,		//Notre feu fixe pr�s de la zone de d�part
			FIRE_ID_MOBILE_NORTH,
			FIRE_ID_MOBILE_CENTRAL,
			FIRE_ID_MOBILE_SOUTH,
			FIRE_ID_SOUTH_OUR,	//Au sud, de notre cot�
			FIRE_ID_ADV_START,
			FIRE_ID_ADV_MOBILE_NORTH,
			FIRE_ID_ADV_MOBILE_CENTRAL,
			FIRE_ID_ADV_MOBILE_SOUTH,
			FIRE_ID_ADV_SOUTH,	//Au sud, du cot� adverse
			FIRE_ID_NB
		}fire_id_e;

	#define XBEE_GUY_IS_BLOQUED_IN_NORTH	0x50C	// Informe Pierre que Guy est bloqu� au Nord et que Pierre ferait mieux d'activer son �vitement.
	#define XBEE_ZONE_COMMAND				0x5AA	//Effectue une demande li� au zones (un SID pour toute la gestion des zones comme �a)
	//Commande dans data[0]:
		#define XBEE_ZONE_TRY_LOCK       0	//Dans data[1]: la zone, type: map_zone_e. La r�ponse de l'autre robot sera envoy� avec XBEE_ZONE_LOCK_RESULT
		#define XBEE_ZONE_LOCK_RESULT    1	//Dans data[1]: la zone concern�e, data[2]: TRUE/FALSE suivant si le verouillage � �t� accept� ou non
		#define XBEE_ZONE_UNLOCK         2  //Dans data[1]: la zone lib�r�e. Lib�re une zone qui a �t� verouill�e
	#define XBEE_MY_POSITION_IS				0x5FF
			/*
				X : Sint16 (mm)
				Y : Sint16 (mm)
				robot_id : robot_id_e (Uint8)
				+ 3 octets RFU
			*/

/******************************************************************
 *
 * 		Messages �chang�s entre les cartes strat�gies et la balise fixe
 *
 ******************************************************************/
											//B comme Beacon (ou Balise !)
	#define ENABLE_WATCHING_ZONE			0x5BE	//E comme Enable
	#define DISABLE_WATCHING_ZONES			0x5BD	//D comme Disable
	#define GET_ZONE_INFOS					0x5B1	//1 comme 1nfos...

	#define STRAT_ZONE_INFOS				0x551	//5trat�gie 1nfos

	typedef enum
	{
		ZONE_FRESQUO = 0,
		ZONE_CENTRAL_QUATER_HEART_RED_MAMMOTH,
		ZONE_CENTRAL_QUATER_HEART_RED_TREE,
		ZONE_RED_TORCH,
		ZONE_RED_START_FIRE,
		ZONE_RED_HEART,
		ZONE_RED_TREE,
		ZONE_TWICE_FIXED_FIRES,
		ZONE_YELLOW_TREE,
		ZONE_YELLOW_HEART,
		ZONE_YELLOW_START_FIRE,
		ZONE_YELLOW_TORCH,
		ZONE_CENTRAL_QUATER_HEART_YELLOW_MAMMOTH,
		ZONE_CENTRAL_QUATER_HEART_YELLOW_TREE,
		ZONE_YELLOW_MAMMOTH,
		ZONE_RED_MAMMOTH,
		ZONE_START_YELLOW,
		ZONE_START_RED,
		ZONE_NUMBER		//Nombre de zones...
	}zone_e;

	typedef Uint8 zone_event_t;
	#define EVENT_NO_EVENT	0b00000000
	#define EVENT_GET_IN	0b00000001
	#define EVENT_GET_OUT	0b00000010
	#define EVENT_TIME_IN	0b00000100
	#define EVENT_SPECIAL	0b00001000


 /*****************************************************************
 *
 *		Messages echang�s entre la carte Supervision
 *		et les cartes strat�gie, actionneur et propulsion
 *
 *****************************************************************/


	/* Carte super vers carte actionneur */
	#define ACT_DO_SELFTEST		0x300
	#define ACT_PING			0x316	//16 = IG = PING

	/* Carte super vers carte propulsion */

	#define PROP_DO_SELFTEST	0x100
	#define PROP_PING			0x116	//16 = IG = PING

	/* Carte super vers carte balise */
	#define BEACON_DO_SELFTEST	0x400
	#define BEACON_PING			0x416	//16 = IG = PING

	/* Carte actionneur vers Strat */
	#define STRAT_ACT_SELFTEST_DONE 				0x2E3
	#define STRAT_ACT_PONG							0x226
		//Data[0] contient l'ID du robot (WHO_AM_I)

	/* Carte propulsion vers Strat */
	#define STRAT_PROP_SELFTEST_DONE 				0x2E1
	#define STRAT_PROP_PONG							0x216
		//Data[0] contient l'ID du robot (WHO_AM_I)

	/* Carte balise vers Strat */
	#define STRAT_BEACON_IR_SELFTEST_DONE			0x2E4
	#define STRAT_BEACON_US_SELFTEST_DONE			0x2E5
	#define STRAT_BEACON_PONG						0x246

	#define DEBUG_SELFTEST_LAUNCH					0x700
	//Jusqu'� 8 codes d'erreurs peuvent �tre transmis dans la r�ponse de chaque carte. (la size contient le nombre de code d'erreurs envoy�s.)
	//En cas de test r�ussi, size vaut 0...
	typedef enum
	{
		SELFTEST_NOT_DONE = 0,
		SELFTEST_BEACON_ADV1_NOT_SEEN,				//Ne rien mettre avant ceci sans synchroniser le QS_CANmsgList dsPIC pour la balise !!!
		SELFTEST_BEACON_ADV2_NOT_SEEN,
		SELFTEST_BEACON_SYNCHRO_NOT_RECEIVED,
		SELFTEST_TIMEOUT,
		SELFTEST_PROP_FAILED,
		SELFTEST_PROP_HOKUYO_FAILED,
		SELFTEST_PROP_DT10_1_FAILED,
		SELFTEST_PROP_DT10_2_FAILED,
		SELFTEST_PROP_DT10_3_FAILED,
		SELFTEST_PROP_DT50_2_FAILED,
		SELFTEST_PROP_DT50_3_FAILED,
		SELFTEST_PROP_IN_SIMULATION_MODE,
		SELFTEST_PROP_IN_LCD_TOUCH_MODE,
		SELFTEST_STRAT_AVOIDANCE_SWITCH_DISABLE,
		SELFTEST_STRAT_XBEE_SWITCH_DISABLE,
		SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE,
		SELFTEST_STRAT_RTC,
		SELFTEST_STRAT_BATTERY_NO_24V,
		SELFTEST_STRAT_BATTERY_LOW,
		SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME,
		SELFTEST_STRAT_BIROUTE_FORGOTTEN,
		SELFTEST_STRAT_FRESQUE_1_MISSING,
		SELFTEST_STRAT_FRESQUE_2_MISSING,
		SELFTEST_STRAT_FRESQUE_3_MISSING,
		SELFTEST_STRAT_LASER_TORCH,
		SELFTEST_STRAT_SD_WRITE_FAIL,
		SELFTEST_ACT_UNREACHABLE,
		SELFTEST_PROP_UNREACHABLE,
		SELFTEST_BEACON_UNREACHABLE,

		// Self test de la carte actionneur (si actionneur indiqu�, alors il n'a pas fonctionn� comme pr�vu, pour plus d'info voir la sortie uart de la carte actionneur) :
		SELFTEST_ACT_MISSING_TEST,	//Test manquant apr�s un timeout du selftest actionneur, certains actionneur n'ont pas le selftest d'impl�ment� ou n'ont pas termin� leur action (ou plus rarement, la pile �tait pleine et le selftest n'a pas pu se faire)
		SELFTEST_ACT_UNKNOWN_ACT,	//Un actionneur inconnu a fail son selftest. Pour avoir le nom, ajoutez un SELFTEST_ACT_xxx ici et g�rez l'actionneur dans selftest.c de la carte actionneur
		SELFTEST_ACT_FRUIT_MOUTH,
		SELFTEST_ACT_LANCELAUNCHER,
		SELFTEST_ACT_ARM,
		SELFTEST_ACT_SMALL_ARM,
		SELFTEST_ACT_FILET,
		SELFTEST_ACT_GACHE,
		SELFTEST_ACT_TORCH_LOCKER,
		SELFTEST_POMPE,
		SELFTEST_FAIL_UNKNOW_REASON,
		SELFTEST_ERROR_NB,
		SELFTEST_NO_ERROR = 0xFF
		//... ajouter ici d'�ventuels nouveaux code d'erreur.
	}SELFTEST_error_code_e;


/*****************************************************************
 *		Petits mots doux echang�s entre la carte propulsion
 *		et la carte strat�gie
 *
 *****************************************************************/
/*
 * Proc�dure et messages �chang�s pour la localisation de l'adversaire.
 * 1- la carte propulsion localise les adversaires avec l'hokuyo
 * 2- la carte balise IR envoie ses infos � la carte propulsion
 * 3- la carte propulsion analyse la correspondance pour comprendre qui est qui (2 adv + friend) + fusion de donn�es et calcul de la fiabilit�.
 * 4- envoi des positions adverses � la strat�gie (x, y, dist, teta) + fiabilit� pour chaque donn�e
 *
 * Possibilit� de forcer en d�bog les positions adverses en envoyant le m�me message de position STRAT_ADVERSARIES_POSITION
 */


	/* carte propulsion vers carte strat�gie */
//TODO renommer ces messages pour respecter le nom es cartes (STRAT et PROP)
	#define STRAT_TRAJ_FINIE			0x210
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define STRAT_PROP_ERREUR			0x211
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
		0x00  : Uint8 RFU (Reserved for Future Use)
		Error : Octet caract�risant l'erreur rencontr�e :
				0bTRxWWEEE
					 T = bool_e
					 R = bool_e
					 x = non utilis�
					 WW  = way_e
					 EEE = SUPERVISOR_error_source_e
				avec :
					T = 1 si robot en translation
					R = 1 si robot en rotation
					La combinaison des 2 bit T et R est possible lors de courbe ou correction d'angle lors d'un mouvement

					typedef enum
					{
						ANY_WAY=0,
						BACKWARD,
						FORWARD
					} way_e;

					typedef enum
					{
						NO_ERROR = 0,					//Ne doit pas arriver, s'il y a eu erreur, c'est qu'il y a une raison
						UNABLE_TO_GO_ERROR,				//La raison la plus fr�quente : un obstacle emp�che la propulsion d'atteindre son objectif. (se produit lors d'une absence d'alim de puissance)
						IMMOBILITY_ERROR,				//Le robot est proche de son objectif, mais est immobilis� (par un �l�ment o� un obstacle...)
						ROUNDS_RETURNS_ERROR,			//Une erreur d'algo de propulsion ou de m�canique produit une oscillation autour de notre objectif et nous le rend difficile � atteindre. On peut consid�rer qu'on est arriv� � notre objectif (il est proche !)
						UNKNOW_ERROR					//N'existe pas � l'heure o� j'�cris ces lignes... RFU (Reserved for Futur Use)
					}SUPERVISOR_error_source_e;


	*/

	#define STRAT_ROBOT_FREINE		0x213
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define PROP_ROBOT_CALIBRE		0x214

	#define STRAT_PROP_FOE_DETECTED		0x215
	/* Message CAN envoy� par la propulsion vers la strat�gie lors de la d�tection du adversaire dans le d�placement demand� */


	#define STRAT_ADVERSARIES_POSITION	0x299	//Position des adversaires
		#define IT_IS_THE_LAST_ADVERSARY	0x80	//Bit lev� si l'adversaire envoy� est le dernier...
		/*		0 : ADVERSARY_NUMBER | IT_IS_THE_LAST_ADVERSARY	//de 0 � n, il peut y avoir plus de deux adversaires si l'on inclut notre ami...
		 * 		1 :  x [2cm]
		 * 		2 :  y [2cm]
		 * 		3-4 : teta
		 * 		5 : distance [2cm]
		 * 		6 : fiability	:    "0 0 0 0 d t y x" (distance, teta, y, x) : 1 si fiable, 0 sinon.
		 */
		#define ADVERSARY_DETECTION_FIABILITY_X			0b00000001
		#define ADVERSARY_DETECTION_FIABILITY_Y			0b00000010
		#define ADVERSARY_DETECTION_FIABILITY_TETA		0b00000100
		#define ADVERSARY_DETECTION_FIABILITY_DISTANCE	0b00001000
		#define ADVERSARY_DETECTION_FIABILITY_ALL		0b00001111


	/* carte strat�gie vers carte propulsion */
	#define PROP_GO_POSITION					0x155
	/* arguments :
		CONFIG : Uint8	=>	|..0. .... - ordre non multipoint
							|..1. .... - ou multipoint
							|
							|...0 .... - ordre a executer maintenant
							|...1 .... - ou a la fin du buffer
							|
							|.... ...0 - ordre dans referentiel pas relatif (donc absolu)
							|.... ...1 - ou relatif
		XHIGH : Uint8	bits les plus significatifs de X, (mm)
		XLOW : Uint8	bits les moins significatifs de X
		YHIGH : Uint8	bits les plus significatifs de Y, (mm)
		YLOW  : Uint8	bits les moins significatifs de Y
		VITESSE : Uint8	=>	0x00 : rapide
							0x01 : lent
							0x02 : tr�s lent
							....
							0x08 � 0xFF : vitesse "analogique"

		MARCHE : Uint8	=>	|...0 ...0 |_ marche avt ou arri�re
							|...1 ...1 |
							|
							|...0 ...1 	- marche avant oblig�
							|...1 ...0 	- marche arri�re oblig�e

		RAYONCRB : Uint8 =>	|.... ...0 | > courbe automatique d�sactiv� (choix en propulsion)
							|.... ...1 | > courbe automatique d�sactiv� (choix en propulsion)
							|xxxx .... | > type d'�vitement sur la trajectoire
	*/
	#define PROP_GO_ANGLE						0x177
	/* argument :
			CONFIG : Uint8	=>	|..0. .... - ordre non multipoint
								|..1. .... - ou multipoint
								|
								|...0 .... - ordre a executer maintenant
								|...1 .... - ou a la fin du buffer
								|
								|.... ...0 - ordre dans referentiel pas relatif (donc absolu)
								|.... ...1 - ou relatif
			TETAHIGH : Uint8	bits les plus significatifs de TETA,(rad/4096)
			TETALOW : Uint8		bits les moins significatifs de TETA
			0
			0
			VITESSE : Uint8	=>	0x00 : rapide
								0x01 : lent
								0x02 : tr�s lent
								....
								0x08 � 0xFF : vitesse "analogique"

			MARCHE : Uint8	=>	|...0 ...0 |_ marche avt ou arri�re
								|...1 ...1 |
								|
								|...0 ...1 	- marche avant oblig�
								|...1 ...0 	- marche arri�re oblig�e

			RAYONCRB : Uint8
		*/
	#define PROP_SEND_PERIODICALLY_POSITION		0x188
		/*
			PERIODE : 		Uint16 		unit� : [ms] 	P�riode � laquelle on veut recevoir des messages de BROADCAST_POSITION
			TRANSLATION : 	Sint16 		unit� : mm		D�placement du robot au del� duquel on veut recevoir un BROADCAST_POSITION
			ROTATION : 		Sint16		unit� : rad4096	D�placement du robot au del� duquel on veut recevoir un BROADCAST_POSITION

			Si l'un des param�tres vaut 0, l'avertisseur correspondant est d�sactiv�.

			Remarque :
			l'it tourne � 5ms => plus petite p�riode d'envoi
			donc la PERIODE effective sera la p�riode demand�e arrondie au 5ms sup�rieur !

		*/
	#define PROP_STOP							0x101
	#define PROP_TELL_POSITION					0x105
	#define	PROP_SET_POSITION					0x107
	/*
		XHIGH : Uint8		bits les plus significatifs de X, (mm)
		XLOW : Uint8		bits les moins significatifs de X
		YHIGH : Uint8		bits les plus significatifs de Y, (mm)
		YLOW  : Uint8		bits les moins significatifs de Y
		TETAHIGH : Uint8	bits les plus significatifs de TETA,(rad/4096)
		TETALOW : Uint8		bits les moins significatifs de TETA
	*/
	#define PROP_RUSH_IN_THE_WALL				0x109
	/*
		SENS : way_e Uint8
		asservissement en rotation on (1)/off(0) : Uint8
	*/
	#define PROP_CALIBRATION					0x10B
	/* argument :
		SENS : way_e (Uint8)
		0 pour demander un d�sarmement !!!
	*/

	#define PROP_WARN_ANGLE						0x10C
	/* argument :
		Angle : Sint16 (RAD4096)
			0 pour demander un d�sarmement !!!
			ATTENTION, pas d'armement possible en 0, demandez 1[rad/4096], c'est pas si loin.
	*/
	#define PROP_WARN_X							0x10D
	/* argument :
		x : Sint16 (mm)
			0 pour demander un d�sarmement !!!
	*/
	#define PROP_WARN_Y							0x10E
	/* argument :
		y : Sint16 (mm)
			0 pour demander un d�sarmement !!!
	*/
	#define PROP_SET_CORRECTORS					0x10F
		//data 0 : bool_e  correcteur en rotation
		//data 1 : bool_e  correcteur en translation
	#define PROP_JOYSTICK 						0x111


	#define PROP_DEBUG_FORCED_FOE				0x118
	// Message CAN pour forcer la d�tection d'un adversaire

/*****************************************************************
 *
 *		Messages echang�s entre les cartes Actionneurs
 *		et la carte Strat�gie
 *
 *****************************************************************/

	//////////////// Results ////////////////
	//D�finir un SID du type ACT_nom_actionneur_RESULT avec un nombre de type (STRAT_FILTER | (ACT_FILTER >> 4) | valeur)
	//avec valeur diff�rente pour chaque message de la carte actionneur � la carte strat
	#define ACT_RESULT (STRAT_FILTER | (ACT_FILTER >> 4) | 0)
	//Dans data[0]: le sid actionneur & 0xFF (on ne garde que l'octet de poid faible, l'octet de poids fort contient le filtre et on en a pas besoin)
	//Dans data[1]: la commande dont en envoi le r�sultat. Par ex: ACT_BALLLAUNCHER_ACTIVATE
	//Dans data[2]: �tat de la commande, une de ces valeurs:
		#define ACT_RESULT_DONE        0	//Tout s'est bien pass�
		#define ACT_RESULT_FAILED      1	//La commande s'est mal pass� et on ne sait pas dans quel �tat est l'actionneur (par: les capteurs ne fonctionnent plus)
		#define ACT_RESULT_NOT_HANDLED 2	//La commande ne s'est pas effectu� correctement et on sait que l'actionneur n'a pas boug� (ou quand la commande a �t� ignor�e)
	//Dans data[3]: la raison du probl�me (si pas DONE), une de ces valeurs:
		#define ACT_RESULT_ERROR_OK           0	//Quand pas d'erreur
		#define ACT_RESULT_ERROR_TIMEOUT      1	//Il y a eu timeout, par ex l'asservissement prend trop de temps
		#define ACT_RESULT_ERROR_OTHER        2	//La commande �tait li� � une autre qui ne s'est pas effectu� correctement, utilis� avec ACT_RESULT_NOT_HANDLED
		#define ACT_RESULT_ERROR_NOT_HERE     3	//L'actionneur ou le capteur ne r�pond plus (on le sait par un autre moyen que le timeout, par exemple l'AX12 ne renvoie plus d'info apr�s l'envoi d'une commande.)
		#define ACT_RESULT_ERROR_LOGIC        4	//Erreur de logique interne � la carte actionneur, probablement une erreur de codage (par exemple un �tat qui n'aurait jamais d� arriv�)
		#define ACT_RESULT_ERROR_NO_RESOURCES 5 //La carte n'a pas assez de resource pour g�rer la commande. Commande � renvoyer plus tard.
		#define ACT_RESULT_ERROR_INVALID_ARG  6 //La commande ne peut pas �tre effectu�e, l'argument n'est pas valide ou est en dehors des valeurs accept�s
		#define ACT_RESULT_ERROR_CANCELED     7 //L'action a �t� annul�

		//Ajoutez-en si n�cessaire
		#define ACT_RESULT_ERROR_UNKNOWN      255	//Erreur inconnue ou qui ne correspond pas aux pr�c�dentes.
	/////////////////////////////////////////

	///////////////COMMON ARG////////////////////
	// Dans data[1]
	#define ACT_CONFIG						0xFF	// Argument ajout� apr�s un SID d'actionneur pour le configure
		// Dans data[2]
		#define AX12_SPEED_CONFIG				0x00
		   // wheel mode (0 � 100)
		   // position mode (0 � 500)
		#define AX12_TORQUE_CONFIG				0x01
		   // (0 � 100)
	/////////////////////////////////////////////

	/////////////////// FILET ///////////////////
	// Message de l'actionneur vers la strat�gie pour informer de l'�tat du filet
	#define STRAT_INFORM_FILET (STRAT_FILTER | (ACT_FILTER >> 4) | 1)
		// Dans data[0]
		#define STRAT_INFORM_FILET_ABSENT	(0b000000000)
		#define STRAT_INFORM_FILET_PRESENT	(0b000000001)
	//////////////////////////////////////////////

	/////////////////// FRUIT_MOUTH ///////////////////
	// Message de l'actionneur vers la strat�gie pour informer de l'�tat du fruit mouth
	#define STRAT_INFORM_FRUIT_MOUTH (STRAT_FILTER | (ACT_FILTER >> 4) | 2)
		// Dans data[0]
		#define STRAT_INFORM_FRUIT_MOUTH_OPEN	(0b000000000)
		#define STRAT_INFORM_FRUIT_MOUTH_CLOSE	(0b000000001)
	//////////////////////////////////////////////

	/////////////////// POMPE ///////////////////
	#define ACT_ASK_POMPE_IN_PRESSURE	(STRAT_FILTER | (ACT_FILTER >> 4) | 3)
	#define STRAT_ANSWER_POMPE			(STRAT_FILTER | (ACT_FILTER >> 4) | 4)
	// Dans data[0]
		#define STRAT_ANSWER_POMPE_NO		(0b00000000)
		#define STRAT_ANSWER_POMPE_YES		(0b00000001)
	/////////////////////////////////////////////

// Code des SID des messages: 0x30x = message pour Tiny, 0x31x = message pour Krusty.
// Le SID 0x300 est reserv� pour le self_test
// Ceci est un enum de SID d'actionneur avec les param�tres de chaque actions d�finie par des defines. L'enum est utilis� pour v�rifier que tous les messages de retour d'actionneurs sont g�r� en strat

typedef enum { //SEUL les SID des actionneurs doivent �tre mis comme enum, le reste en #DEFINE

	////////////////// FRUIT_MOUTH ///////////
	ACT_FRUIT_MOUTH = (ACT_FILTER | 0x01),   //0x16: collision avec ACT_PING
		//Param�tres de la pompe
		#define ACT_FRUIT_MOUTH_CLOSE           0x10
		#define ACT_FRUIT_MOUTH_OPEN            0x11
		#define ACT_FRUIT_MOUTH_VIBRATION		0x12
		#define ACT_FRUIT_MOUTH_CANCELED        0x13
		#define ACT_FRUIT_MOUTH_STOP	        0x1F

		//Fruit Labium (trappe)
		#define ACT_FRUIT_LABIUM_CLOSE          0x14
		#define ACT_FRUIT_LABIUM_OPEN           0x15
		#define ACT_FRUIT_LABIUM_STOP           0x1E

	/////////////////////////////////////////

	/////////////////LANCELAUNCHER////////////////////
	ACT_LANCELAUNCHER = (ACT_FILTER | 0x02),
		//Param�tres de LANCELAUNCHER (dans data[0])
		#define ACT_LANCELAUNCHER_RUN_1_BALL	0x11
		#define ACT_LANCELAUNCHER_RUN_5_BALL	0x12
		#define ACT_LANCELAUNCHER_RUN_ALL		0x13
		#define ACT_LANCELAUNCHER_STOP			0x1F
	////////////////////////////////////////////////

	////////////////// ARM  /////////////////
	ACT_ARM = (ACT_FILTER | 0x03),
		//Param�tres de ARM (dans data[0])
		#define ACT_ARM_GOTO 0   // Va � la position demand�e dans data[1] (une des valeurs ci-dessous)
			// Voir position du bras ci-dessous (ARM_STATE_ENUM)

		#define ACT_ARM_STOP 1  // Stoppe l'asservissement des moteurs

		//Pas utilis� par la strat mais ici pour �tre testable
		#define ACT_ARM_INIT 3

		// Positions prise par le bras
		// Pour ajouter une position, ajoutez une ligne avec:
		// XX(<nom �tat>) \                                                                            text ici pour garder des espaces apr�s le slash, sinon multiline comment
		// N'oubliez pas le \ !!!!! (sauf pour le dernier)
		#define ARM_STATE_ENUMVALS(XX) \
			XX(ACT_ARM_POS_PRE_PARKED_1) \
			XX(ACT_ARM_POS_PRE_PARKED_2) \
			XX(ACT_ARM_POS_PARKED) \
			XX(ACT_ARM_POS_MID) \
			XX(ACT_ARM_POS_OPEN) \
			XX(ACT_ARM_POS_OPEN_2) \
			XX(ACT_ARM_POS_ON_TORCHE) \
			XX(ACT_ARM_POS_ON_TORCHE_SMALL_ARM) \
			XX(ACT_ARM_POS_ON_TORCHE_SMALL_ARM_RESCUE) \
			XX(ACT_ARM_POS_PREPARE_1_TORCHE_AUTO) \
			XX(ACT_ARM_POS_PREPARE_2_TORCHE_AUTO) \
			XX(ACT_ARM_POS_ON_TORCHE_AUTO) \
			XX(ACT_ARM_POS_ON_TORCHE_AUTO_ESCAPE_1) \
			XX(ACT_ARM_POS_ON_TORCHE_AUTO_ESCAPE_2) \
			XX(ACT_ARM_POS_ON_PREPARE_DROP_1_AUTO) \
			XX(ACT_ARM_POS_ON_PREPARE_DROP_2_AUTO) \
			XX(ACT_ARM_POS_ON_DROP_1_AUTO) \
			XX(ACT_ARM_POS_ON_DROP_2_AUTO) \
			XX(ACT_ARM_POS_ON_PREPARE_1_DROP_3_AUTO) \
			XX(ACT_ARM_POS_ON_PREPARE_2_DROP_3_AUTO) \
			XX(ACT_ARM_POS_ON_DROP_3_AUTO) \
			XX(ACT_ARM_POS_TO_STORAGE) \
			XX(ACT_ARM_POS_TO_CARRY) \
			XX(ACT_ARM_POS_TO_PREPARE_RETURN) \
			XX(ACT_ARM_POS_TO_DOWN_RETURN) \
			XX(ACT_ARM_POS_TO_RETURN) \
			XX(ACT_ARM_POS_WAIT_RETURN) \
			XX(ACT_ARM_POS_TO_PREPARE_TAKE_RETURN) \
			XX(ACT_ARM_POS_TO_TAKE_RETURN) \
			XX(ACT_ARM_POS_TO_UNBLOCK_RETURN) \
			XX(ACT_ARM_POS_TO_UNBLOCK_RETURN_UP) \
			XX(ACT_ARM_POS_ON_TRIANGLE) \
			XX(ACT_ARM_POS_PREPARE_BACKWARD) \
			XX(ACT_ARM_POS_DOWN_BACKWARD) \
			XX(ACT_ARM_POS_LOCK_BACKWARD) \
			XX(ACT_ARM_POS_PREPARE_TAKE_ON_EDGE) \
			XX(ACT_ARM_POS_PREPARE_TAKE_ON_EDGE_2) \
			XX(ACT_ARM_POS_TAKE_ON_EDGE) \
			XX(ACT_ARM_POS_RETURN_ON_EDGE) \
			XX(ACT_ARM_POS_DISPOSED_SIMPLE) \
			XX(ACT_ARM_POS_PREPARE_TAKE_ON_ROAD) \
			XX(ACT_ARM_POS_TAKE_ON_ROAD) \
			XX(ACT_ARM_POS_TAKE_ON_ROAD_MAMOUTH) \
			XX(ACT_ARM_POS_DISPOSED_TORCH) \
			XX(ACT_ARM_POS_ESCAPE_TORCH_1) \
			XX(ACT_ARM_POS_ESCAPE_TORCH_2) \
			XX(ACT_ARM_POS_TORCHE_CENTRAL) \
			XX(ACT_ARM_POS_TORCHE_ADV)	\
			XX(ACT_ARM_POS_ON_THE_LEFT)

		#define ACT_ARM_PRINT_POS 2  //Affiche les positions des actionneurs sur l'uart

		#define ACT_ARM_PRINT_STATE_TRANSITIONS 4

		// Param�tres permettant de g�rer la hauteur du bras
		#define ACT_ARM_UPDOWN_GOTO 5

		// Param�tres permettant de g�rer la hauteur du bras
		#define ACT_ARM_UPDOWN_RUSH_IN_FLOOR 6

	/////////////////////////////////////////

	/////////////////////FILET///////////////////
	ACT_FILET = (ACT_FILTER | 0x04),
		//Param�tres de FILET (dans data[0])
		#define ACT_FILET_IDLE				0x11
		#define ACT_FILET_LAUNCHED			0x12
		#define ACT_FILET_STOP				0x13
	/////////////////////////////////////////////

	//////////////////PETIT BRAS/////////////////
	ACT_SMALL_ARM = (ACT_FILTER | 0x05),
		//Param�tres de SMALL_ARM (dans data[0])
		#define ACT_SMALL_ARM_IDLE			0x11
		#define ACT_SMALL_ARM_MID			0x12
		#define ACT_SMALL_ARM_DEPLOYED		0x13
		#define ACT_SMALL_ARM_STOP			0x14
	/////////////////////////////////////////////

	/////////////////////POMPE///////////////////
	ACT_POMPE = (ACT_FILTER | 0x06),
		//Param�tres de SMALL_ARM (dans data[0])
		#define ACT_POMPE_NORMAL			0x11
		#define ACT_POMPE_REVERSE			0x12
		// Pour les deux actions ci-dessus dans data[1]
		   // Le rapport cyclique voulue de la pompe entre 0 et 100

		#define ACT_POMPE_STOP				0x13

	/////////////////////////////////////////////

	//////////////////TORCH LOCKER/////////////////
	ACT_TORCH_LOCKER = (ACT_FILTER | 0x07),
		//Param�tres de TORCH_LOCKER (dans data[0])
		#define ACT_TORCH_LOCKER_LOCK		0x11
		#define ACT_TORCH_LOCKER_UNLOCK		0x12
		#define ACT_TORCH_LOCKER_INSIDE		0x13
		#define ACT_TORCH_LOCKER_STOP		0x14
	/////////////////////////////////////////////

	/////////////////////GACHE///////////////////
	ACT_GACHE = (ACT_FILTER | 0x08),
		//Param�tres de GACHE (dans data[0])
		#define ACT_GACHE_IDLE				0x11
		#define ACT_GACHE_LAUNCHED			0x12
		#define ACT_GACHE_STOP				0x13
	/////////////////////////////////////////////

	/////////////////TEST SERVO//////////////////
	ACT_TEST_SERVO = (ACT_FILTER | 0x09)
		//Param�tres de TEST_SERVO (dans data[0])
		#define ACT_TEST_SERVO_IDLE				0x11
		#define ACT_TEST_SERVO_STATE_1			0x12
		#define ACT_TEST_SERVO_STATE_2			0x13
		#define ACT_TEST_SERVO_STOP				0x14

	/////////////////////////////////////////////

} ACT_sid_e; //FIN de l'enum des SID d'actionneurs

#define ENUM_ITEMIZE(val) val,
typedef enum {
	ARM_STATE_ENUMVALS(ENUM_ITEMIZE)
	ARM_ST_NUMBER
} ARM_state_e;
#undef ENUM_ITEMIZE

//Info sur l'astuce du XX:
//https://github.com/joyent/libuv/blob/422d2810b37d1ec8a12f967089d04039800c2b44/include/uv.h#L65
//http://www.drdobbs.com/the-new-c-x-macros/184401387



/*****************************************************************
 *
 *		Messages echang�s entre la carte balise
 *		et la carte strat�gie
 *
 *****************************************************************/

	/* carte strat�gie vers carte balises */
	#define BEACON_ENABLE_PERIODIC_SENDING	0x410
	#define BEACON_DISABLE_PERIODIC_SENDING	0x411
	#define STRAT_FRIEND_FORCE_POSITION						0x258	//Forcer la position du robot ami




		/* DEFINITION DES ERREURS RENVOYEES PAR LA BALISE :
		###ATTENTION : ce texte est une copie extraite du fichier "balise_config.h" du projet balise.
		--> Plusieurs erreurs peuvent se cumuler... donc 1 bit chacune...
		*/
		#define AUCUNE_ERREUR						0b00000000
						//COMPORTEMENT : le r�sultat d�livr� semble bon, il peut �tre utilis�.

		#define AUCUN_SIGNAL						0b00000001
						//survenue de l'interruption timer 3 car strictement aucun signal re�u depuis au moins deux tours moteurs
						//cette erreur peut se produire si l'on est tr�s loin
						//COMPORTEMENT : pas d'�vittement par balise, prise en compte des t�l�m�tres !

		#define SIGNAL_INSUFFISANT					0b00000010
						//il peut y avoir un peu de signal, mais pas assez pour estimer une position fiable (se produit typiquement si l'on est trop loin)
						//cette erreur n'est pas grave, on peut consid�rer que le robot est LOIN !
						//COMPORTEMENT : pas d'�vittement, pas de prise en compte des t�l�m�tres !

		#define TACHE_TROP_GRANDE					0b00000100
						//Ce cas se produit si trop de r�cepteurs ont vu du signal.
						// Ce seuil est STRICTEMENT sup�rieur au cas normal d'un robot tr�s pret. Il y a donc probablement un autre �metteur quelque part, ou on est entour� de miroir.
						//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se r�f�rer aux t�l�m�tres...

		#define TROP_DE_SIGNAL						0b00001000
						//Le r�cepteur ayant re�u le plus de signal en � trop recu
						//	cas 1, peu probable, le moteur est bloqu� (cas de test facile pour v�rifier cette fonctionnalit� !)
						//	cas 2, probable, il y a un autre �metteur quelque part !!!
						// 	cas 3, on est dans une enceinte ferm�e et on capte trop
						//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se r�f�rer aux t�l�m�tres...

		#define ERREUR_POSITION_INCOHERENTE 		0b00010000
						//La position obtenue en x/y est incoh�rente, le robot semble �tre franchement hors du terrain
						//COMPORTEMENT : si la position obtenue indique qu'il est loin, on ne fait pas d'�vitement !
						//sinon, on fait confiance � nos t�l�m�tres (?)

		#define OBSOLESCENCE						0b10000000
						//La position adverse connue est obsol�te compte tenu d'une absence de r�sultat valide depuis un certain temps.
						//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se r�f�rer aux t�l�m�tres...


/*****************************************************************
 *
 *		Messages echang�s entre la carte IHM
 *		et les autres cartes
 *
 *****************************************************************/

#define IHM_SWITCH  0x601
// Envois un message CAN quand un switch est d�placer

#endif	/* ndef QS_CANMSGLIST_H */
