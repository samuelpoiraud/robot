/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_CANmsgDoc.h
 *  Package : Qualit� Soft
 *  Description : descriptif des messages CAN
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20101216
 */
 
#if 0 // c'est que de la doc
#ifndef QS_CANMSGDOC_H
	#define QS_CANMSGDOC_H

	/* Masque des cartes (des destinataires) */
	
  	/* Message pour tous */
  	#define BROADCAST_START
  	#define BROADCAST_STOP_ALL
	#define BROADCAST_COULEUR
	/* argument
		couleur de notre robot : (Uint8):(RED=0, PURPLE =1)
	*/
	#define BROADCAST_POSITION_ROBOT // cf. CARTE_P_POSITION_ROBOT

	/* Message pour personne */
	#define DEBUG_CARTE_P				0x742
	/*
	 *	Non document� : pour �quipe Carte P uniquement
	 */

	#define DEBUG_FOE_POS				0x748
	/*
		arguments (size = 8) :
		Uint8	octet d'erreur (toujours � 0)
		Sint16	angle de vue de l'adversaire [rad/4096] (de -PI4096 � PI4096)
		Uint8	distance de vue de l'adversaire [cm] (ATTENTION, unit� = cm !!! donc maximum = 2,55m)
		Sint16	position adverse x	[mm]	(ATTENTION, peut �tre n�gatif si on le voit pr�t de la bordure avec une petite erreur...)
		Sint16  position adverse y	[mm]	(idem)
	*/
	
	#define DEBUG_ELEMENT_UPDATED		0x749
	/*
		arguments (size = 6) :
		Uint8	raison (ADD, UPDATE, DELETE)
		Uint8 	type de pion (cf. module element)
		Sint16	position element x	[mm]
		Sint16	position element y	[mm]
	*/

	#define DEBUG_ASSER_POINT_FICTIF
	/*
	Message utilis� par le code Asser pour le debug...
		argument
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	
	
	#define SUPER_EEPROM_RESET			(0x770)
	/*  
		Message de l'utilisateur vers Super 
		Permet de demander (exceptionnellement et en toute conscience) le reset de la m�moire EEPROM externe
		Aucun argument.
	*/
	
	#define SUPER_EEPROM_PRINT_MATCH	(0x777)
	/*
		demande l'affichage d'un match dont on connait l'adresse de l'ent�te.
		(l'adresse de l'ent�te d'un match est affich�e lorsque l'on affiche un match...)
		exemple d'utilisation : si vous avez retenu l'adresse du match d'hier que vous voulez voir, vous pouvez le revoir (s'il n'a pas �t� �crabouill� par les nombreux tests de la nuit...)
		argument
		match_address_x8 : Uint16 (x8 signifie ici que l'unit� de ce nombre est une adresse physique d'eeprom/8, voir le code pour plus d'infos)
	*/
	
	#define SUPER_RTC_GET				(0x780)
	
	#define SUPER_RTC_SET				(0x781)
	/*
		Uint8 seconds
		Uint8 minutes
		Uint8 hours
		Uint8 day
		Uint8 months
		Uint8 year	(11 pour 2011)
	*/
	
	#define	SUPER_RTC_TIME				(0x782)
	/*
		Si communication impossible avec la RTC, les champs sont mis � z�ro, sauf la date et le mois qui sont � 1.
		Uint8 seconds
		Uint8 minutes
		Uint8 hours
		Uint8 day
		Uint8 months
		Uint8 year	(11 pour 2011)
	*/
	
/*****************************************************************
 *
 *		Messages echang�s entre la carte Supervision
 *		et les cartes strat�gie, actionneur et propulsion
 *
 *****************************************************************/

	/* Carte super vers carte strat�gie */
	#define SUPER_ASK_CONFIG
	/* arguments :
		strategie   : Uint8
		color : Uint8 (color_e)
		evitement  : Uint8 (bool_e)
		balise   : Uint8 (bool_e)
	*/
	#define SUPER_ASK_STRAT_SELFTEST	
	/*
		Vide
	*/
	
	/* Carte super vers carte actionneur */
	#define SUPER_ASK_ACT_SELFTEST		
	/*
		Vide
	*/
	
	/* Carte super vers carte propulsion */
	#define SUPER_ASK_ASSER_SELFTEST	
	/*
		sens : Uint8 (FORWARD|REAR)
	*/
	
	/* Carte super vers carte balise */
	#define SUPER_ASK_BEACON_SELFTEST
	/*
		Vide
	*/
	
	/* Carte carte strat�gie vers super */
	#define SUPER_CONFIG_IS
	/* arguments :
		strategie  : Uint8
		color : Uint8 (color_e)
		evitement : Uint8 (bool_e)
		balise  : Uint8 (bool_e)
	*/
	#define STRAT_SELFTEST				
	/* arguments :
		conection_capteurs : Uint8 (cf. code strat)
		biroute : Uint8
	*/
	
	/* Carte actionneur vers Super */
	#define ACT_SELFTEST 				
	/*
		etat_pince_av : Uint8
		etat_pince_arr : Uint8
		etat_acenseur_av : Uint8
		etat_acenseur_arr : Uint8
	*/
	
	/* Carte propulsion vers Super */
	#define ASSER_SELFTEST 				
	/* arguments :
		etat_moteur_g : Uint8
		etat_moteur_d : Uint8
		etat_roues_codeuse_g : Uint8
		etat_roues_codeuse_d : Uint8
	*/
	
	/* Carte balise vers Super */
	#define BEACON_IR_SELFTEST
	/* 3 arguments :
		erreur	: 0 si pas d'erreur, 1 si une erreur.
		erreur_reception_ir_adversaire_1 : Uint8 (tout va bien quand on recoit 0, sinon voir le code d'erreur non nul en bas de ce fichier)
		erreur_reception_ir_adversaire_2 : Uint8 (tout va bien quand on recoit 0, sinon voir le code d'erreur non nul en bas de ce fichier)
	*/
	#define BEACON_US_SELFTEST
	/* arguments :
		erreur_reception_ir : Uint8 (tout va bien quand on recoit 0, 
			sinon voir le code d'erreur non nul en bas de ce fichier)
		erreur_reception_us : Uint8 (tout va bien quand on recoit 0, sinon voir le code d'erreur non nul)
		distance_cm : Uint16 (information gratuite que l'on n'exploite pas)
		etat_synchro : Uint8 (nb de secondes de synchro si synchro OK, 0 sinon !) Consid�rer comme ok si > 10 secondes 
			car la balise a besoin d'au moins 10 secondes de fonctionnement pour r�pondre un selftest validable ...
	*/
	
/******************************************************************
 *
 *		Petits mots doux echang�s entre la carte propulsion 
 *		et la carte strat�gie
 *
 *			typedef enum
 *			{
 *				ANY_WAY,						// Peu importe le sens de d�placement
 *				REAR,							// En arri�re uniquement
 *				FORWARD							// En avant uniquement
 *			}ASSER_way_e;
 *
 *			typedef enum
 *			{
 *				FAST, 
 * 				SLOW
 *	 		}ASSER_speed_e;
 *****************************************************************/

  	/* carte propulsion vers carte strat�gie */
  	#define CARTE_P_TRAJ_FINIE
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define CARTE_P_ASSER_ERREUR
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
		0x00  : Uint8 RFU (Reserved for Future Use)
		Error : Octet caract�risant l'erreur rencontr�e :
				0bTTTWWEEE	
					 TTT = trajectory_e  
					 WW  = way_e
					 EEE = SUPERVISOR_error_source_e
				avec : 
					typedef enum
					{
						TRAJECTORY_TRANSLATION = 0,		//Translation -> on peut envisager rattraper une erreur en effectuant une translation dans l'autre sens
						TRAJECTORY_ROTATION,			//Rotation    -> on peut envisager rattraper une erreur en effectuant une rotation dans l'autre sens
						TRAJECTORY_STOP,				//Trajectoire avec pour objectif d'avoir un robot � l'arr�t. -> Aucune trajectoire de rattrapage, le robot est forc�ment � l'arr�t maintenant.
						TRAJECTORY_AUTOMATIC_CURVE,		//Courbe	  -> on peut envisager rattraper une erreur en effectuant une TRANSLATION dans l'autre sens
						TRAJECTORY_NONE					//Aucune trajectoire en cours -> Aucune trajectoire de rattrapage
					} trajectory_e;		
						
					typedef enum 
					{
						FORWARD_OR_BACKWARD=0,
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
	#define CARTE_P_POSITION_ROBOT
		/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
		Raison : Uint8 (Raison de l'envoi du Broadcast... s'agit-il d'un avertisseur ? d'un envoi p�riodique ? sur simple demande ?...)
			Remarque : Il est possible de CUMULER plusieurs raisons !!!
						#define WARNING_NO					(0b00000000)
						#define WARNING_TIMER				(0b00000001)
						#define WARNING_TRANSLATION			(0b00000010)
						#define WARNING_ROTATION			(0b00000100)
						#define WARNING_REACH_X				(0b00001000)		//Nous venons d'atteindre une position en X pour laquelle on nous a demand� une surveillance.
						#define WARNING_REACH_Y				(0b00010000)		//Nous venons d'atteindre une position en Y pour laquelle on nous a demand� une surveillance.
						#define WARNING_REACH_TETA			(0b00100000)		//Nous venons d'atteindre une position en Teta pour laquelle on nous a demand� une surveillance.
						#define WARNING_MOVE				(0b01000000)		//Nous venons de nous d�placer sur une grille virtuelle pour laquelle on nous a demand� une surveillance.
						#define WARNING_BRAKE				(0b10000000)		
	*/
 	#define CARTE_P_ROBOT_FREINE
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define CARTE_P_ROBOT_CALIBRE
	/*
		Vide
	*/

/************************************************************************************/

  	/* carte strat�gie vers carte propulsion */
  	#define ASSER_GO_ANGLE
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
		#define ASSER_GO_POSITION
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
		RAYONCRB : Uint8
	*/
	#define ASSER_SET_POSITION
	/*
		XHIGH : Uint8		bits les plus significatifs de X, (mm)
		XLOW : Uint8		bits les moins significatifs de X
		YHIGH : Uint8		bits les plus significatifs de Y, (mm)
		YLOW  : Uint8		bits les moins significatifs de Y
		TETAHIGH : Uint8	bits les plus significatifs de TETA,(rad/4096)
		TETALOW : Uint8		bits les moins significatifs de TETA
	*/
	#define ASSER_SEND_PERIODICALLY_POSITION
	/*
		PERIODE : 		Uint16 		unit� : [ms] 	P�riode � laquelle on veut recevoir des messages de BROADCAST_POSITION
		TRANSLATION : 	Sint16 		unit� : mm		D�placement du robot au del� duquel on veut recevoir un BROADCAST_POSITION
		ROTATION : 		Sint16		unit� : rad4096	D�placement du robot au del� duquel on veut recevoir un BROADCAST_POSITION
				
		Si l'un des param�tres vaut 0, l'avertisseur correspondant est d�sactiv�.
		
		Remarque :
		l'it tourne � 5ms => plus petite p�riode d'envoi
		donc la PERIODE effective sera la p�riode demand�e arrondie au 5ms sup�rieur !	
	
	*/

	#define ASSER_STOP
	/* pas d'argument */
	
	#define ASSER_TELL_POSITION
	/* pas d'argument */	
	
	#define ASSER_TYPE_ASSERVISSEMENT
	/* argument :
		Rotation : Uint8		asservissement en rotation on (1)/ off(0)
		Translation : Uint8		asservissement en translation on (1)/ off(0)
		au demarrage du robot les deux asservissements sont actifs
	*/
	#define ASSER_RUSH_IN_THE_WALL
	/*
		SENS : ASSER_way_e Uint8
		asservissement en rotation on (1)/off(0) : Uint8
	*/
	#define ASSER_CALIBRATION
	/* argument :
		SENS : ASSER_way_e (Uint8)
		0 pour demander un d�sarmement !!!
	*/
	#define ASSER_WARN_ANGLE
	/* argument :
		Angle : Sint16 (RAD4096)
			0 pour demander un d�sarmement !!!
			ATTENTION, pas d'armement possible en 0, demandez 1[rad/4096], c'est pas si loin.
	*/		
	#define ASSER_WARN_X
	/* argument :
		x : Sint16 (mm)
			0 pour demander un d�sarmement !!!
	*/
	#define ASSER_WARN_Y
	/* argument :
		y : Sint16 (mm)
			0 pour demander un d�sarmement !!!
	*/
	#define CARTE_ASSER_FIN_ERREUR
	/* pas d'argument */

/******************************************************************
 *
 *		Messages echang�s entre la carte Actionneur
 *		et la carte Strat�gie
 *
 *****************************************************************/

	/* Liste de valeurs int�ressantes */
	#define ACT_OFF						0
	#define ACT_ON						1
	
	#define ACT_FRONT					2	
	#define ACT_BACK					3

	#define ACT_CLAMP_FRONT				4
	#define ACT_CLAMP_BACK				5
	#define ACT_LIFT_FRONT				6
	#define ACT_LIFT_BACK				7
	
	#define ACT_CLAMP_CLOSED_ON_PAWN	8
	#define ACT_CLAMP_OPENED			9
	#define ACT_CLAMP_CLOSED			10
	#define ACT_LIFT_BOTTOM				11
	#define ACT_LIFT_MIDDLE				12
	#define ACT_LIFT_TOP				13
	#define ACT_LIFT_MOVE_TOWER			14
	
	#define ACK_LIFT_ACTION				15
	#define ACK_CLAMP_ACTION			16
	
	/*Carte Actionneur vers carte Strat�gie */
	#define ACT_DCM_POS
	/* Arguments:
		act_id: Uint8 (ACT_CLAMP_FRONT|ACT_CLAMP_BACK|ACT_LIFT_FRONT|ACT_LIFT_BACK)
	*/
	#define ACT_CLAMP_PREPARED
	/* Arguments :
		act_id : Uint8 (ACT_FRONT|ACT_BACK)
	*/
	#define ACT_PAWN_GOT
	/* Arguments:
		act_id: Uint8 (ACT_FRONT|ACT_BACK)
	*/
	#define ACT_PAWN_FILED
	/* Arguments:
		act_id: Uint8 (ACT_FRONT|ACT_BACK)
	*/
	#define ACT_PAWN_DETECTED
	/* Arguments:
		act_id: Uint8 (ACT_FRONT|ACT_BACK)
	*/
	#define ACT_PAWN_NO_LONGER_DETECTED
	/* Arguments:
		act_id: Uint8 (ACT_FRONT|ACT_BACK)
	*/							
	#define ACT_EMPTY
	/* Arguments:
		act_id: Uint8 (ACT_CLAMP_FRONT|ACT_CLAMP_BACK)
	*/						
	#define ACT_FULL
	/* Arguments:
		act_id: Uint8 (ACT_CLAMP_FRONT|ACT_CLAMP_BACK)
	*/							
	#define ACT_FAILURE
	/* Arguments:
		act_id: Uint8 (ACT_CLAMP_FRONT|ACT_CLAMP_BACK|ACT_LIFT_FRONT|ACT_LIFT_BACK)
		error: Uint8
	*/								
	#define ACT_READY										
	/* Arguments:
		act_id: Uint8 (ACT_CLAMP_FRONT|ACT_CLAMP_BACK|ACT_LIFT_FRONT|ACT_LIFT_BACK)
	*/	
	
	/* Carte Strat�gie vers Carte Actionneur*/
	#define ACT_DCM_SETPOS
	/* Ce message permet de donner la position d'un actionneur
		Arguments:
		act_id: Uint8 (ACT_CLAMP_FRONT|ACT_CLAMP_BACK|ACT_LIFT_FRONT|ACT_LIFT_BACK)
		position: Uint8	(ACT_CLAMP_CLOSED_ON_PAWN|ACT_CLAMP_OPENED|ACT_CLAMP_CLOSED|ACT_LIFT_BOTTOM|ACT_LIFT_MIDDLE|ACT_LIFT_TOP|ACT_LIFT_MOVE_TOWER)
	*/
	#define ACT_PREPARE_CLAMP
	/* Ce message permet de pr�parer l'actionneur � pousser ou ramasser en mouvement un �l�ment de jeu 
		Arguments :
		act_id : Uint8 (ACT_FRONT|ACT_BACK)
		out_position : Uint8(ACT_LIFT_BOTTOM|ACT_LIFT_MIDDLE|ACT_LIFT_TOP|ACT_LIFT_MOVE_TOWER)
	*/
	#define ACT_TAKE_PAWN
	/* Ce message demande � l'actionneur pass� en param�tre de ramasser un pion et de le stocker � la position donn�e 
		Arguments:
		act_id : Uint8 (ACT_FRONT|ACT_BACK)
		out_position : Uint8(ACT_LIFT_BOTTOM|ACT_LIFT_MIDDLE|ACT_LIFT_TOP|ACT_LIFT_MOVE_TOWER)
		acquittement : Uint8(ACK_LIFT_ACTION|ACK_CLAMP_ACTION)
	*/
	#define ACT_FILE_PAWN
	/* Ce message permet de l�cher le pion stock� dans la pince � un niveau d'ascenceur pass� en param�tres
	 	Arguments:
		act_id : Uint8 (ACT_FRONT|ACT_BACK)
		position : Uint8(ACT_LIFT_BOTTOM|ACT_LIFT_MIDDLE|ACT_LIFT_TOP|ACT_LIFT_MOVE_TOWER)
		acquittement : Uint8(ACK_LIFT_ACTION|ACK_CLAMP_ACTION)
	*/
	
/*****************************************************************
 *
 *		Messages echang�s entre la carte balise
 *		et la carte strat�gie
 *
 *****************************************************************/
 
	/* carte strat�gie vers carte balises */
	#define BEACON_ENABLE_PERIODIC_SENDING	
	/* 
		Aucun arguments.
		Demande d'activation d'envois p�riodiques de la position adverse.
	*/
	
	#define BEACON_DISABLE_PERIODIC_SENDING
	/* 
		Aucun arguments.
		Demande de d�sactivation d'envois p�riodiques de la position adverse.
	*/
	
	//Balises autour du terrain avec r�ception InfraRouge
	#define BEACON_ADVERSARY_POSITION_IR_ARROUND_AREA		0x252	
	//Balises autour du terrain avec r�ception UltraSon
	#define BEACON_ADVERSARY_POSITION_US_ARROUND_AREA		0x253
		/* Pour ces deux messages, les arguments sont :
			(size = 6)
		0:	bool_e	adversary		0 pour l'adversaire 1, 1 pour l'adversaire 2.
		1:	Uint8	fiability		0 si la valeur renvoy�e est consid�r�e comme fiable
		2-3:Sint16	x
		4-5:Sint16 	y
		*/
		
	
	/* Carte balises vers carte strat�gie */
	#define BEACON_ADVERSARY_POSITION_IR	0x250
	/*
		arguments (size = 8) :
		Uint8	ADV1 	octet d'erreur		(VOIR DESCRIPTION DES ERREURS CI DESSOUS !)			
		Sint16	ADV1 	angle de vue de l'adversaire [rad/4096] (de -PI4096 � PI4096)
		Uint8	ADV1 	distance de vue de l'adversaire [cm] (ATTENTION, unit� = cm !!! donc maximum = 2,55m, un �cretage est r�alis�)
		
		Uint8	ADV2 	octet d'erreur		(VOIR DESCRIPTION DES ERREURS CI DESSOUS !)			
		Sint16	ADV2 	angle de vue de l'adversaire [rad/4096] (de -PI4096 � PI4096)
		Uint8	ADV2 	distance de vue de l'adversaire [cm] (ATTENTION, unit� = cm !!! donc maximum = 2,55m, un �cretage est r�alis�)
		
	*/

	#define BEACON_ADVERSARY_POSITION_US	0x251
	/*
		arguments (size = 2) :
		Uint8	octet d'erreur		(VOIR dESCRIPTION DES ERREURS CI DESSOUS !)			
		Uint8	distance de vue de l'adversaire [cm] MSB (ATTENTION, unit� = cm !!!)
		Uint8	distance de vue de l'adversaire [cm] LSB (ATTENTION, unit� = cm !!!)
	*/

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
		
#endif	/* ndef QS_CANMSGLIST_H */
#endif	/* 0 */
