/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp, CheckNorris
 *
 *	Fichier : telemeter.h
 *	Package : Carte Principale
 *	Description : Caractéristiques des télémètres
 *	Auteur : A.GIRARD, Jacen (Modifié par Ronan)
 *	Version 20110428
 */

#include "QS/QS_all.h"
 
#ifndef TELEMETER_H
	#define TELEMETER_H

	/* Initialisation des télémètres */
	void TELEMETER_init(void);

	/* Mise à jour de la position du robot adverse grâce aux télémètres */
	void TELEMETER_update();
	
	#ifdef TELEMETER_C

		#include "QS/QS_timer.h"
		#include "QS/QS_adc.h"
		#include "maths_home.h"

		/*taille du buffer de mesures des telemeters*/
		#define TELEMETER_BUFFER_SIZE	8

		/*nombre de valeurs définies dans la matrice TELEMETER_conversion*/
		#define TELEMETER_VALUE_CONVERTED 22
	
		/*seuil de temps après lequel on considère que les télémètres prennent 
		le pouvoir pour mettre a jour la position de l'adversaire (non utilisable en 2011)*/
		#define DETECTION_RESPONSE_TIME 300
		
		#define BORDER_DELTA 100 /* 150 mm */
		
		/*seuil de filtrage des dt10 - en mm! */
		#define TELEMETER_THRESHOLD 450
	
		/*localisation des capteurs*/
		#define TELEMETER_NUMBER	0
		/*attention, code lié à detection_sensor_e dans detection.h */

		/*type de capteur : */
		typedef enum
		{
			TELE_GP2D12=0,
			TELE_GP2YO21,
			TELE_SICK_DT10,
			TELE_SICK_DT50,
			TELEMETER_TYPE_NB
		}telemeter_type_e;	

		typedef struct
		{
			Uint8	pinADC;							/*numéro de port*/
			telemeter_type_e	type;				/*type cf. telemeter_type*/
			Sint16 	x;								/*abscisse du capteur relative au centre du robot*/
		   	Sint16 	y;								/*ordonnée du capteur relative au centre du robot*/
			Sint16 	measures[TELEMETER_BUFFER_SIZE];/*nombre de mesures effectuées par le télémètre*/
			Uint8	measures_index;					/*prochaine position a ecrire dans le buffer circulaire */
			Sint16	angle;							/*angle relatif du capteur avec le robot*/
		}telemeter_device_t;
		
		/* indication des télémetres */
		static telemeter_device_t telemeter[TELEMETER_NUMBER];

		/* mise à jour des buffers circulaire */
		static void TELEMETER_update_value();

		/*	Complète les infos du telemetre passé en argument pour les inscrire dans global.env.sensor
		 *	Elimine les valeurs incohérentes
		 */
		static void TELEMETER_toFOE(Uint8 telemetre_id);

		/* fonction de conversion tension->distance*/
		static Uint16 TELEMETER_converter(Sint16 tension, telemeter_type_e type);

		static void TELEMETER_init_Measure(void);
		
		/* renvoie la valeur échantillonnée sur un télémetre (après traitement de signal */
		static Uint16 TELEMETER_get_value(Uint8 telemeter_id);
		
		#ifdef DEBUG_TELEMETER
			#define telemeter_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define telemeter_printf(...)	(void)0
		#endif

	#endif /* def TELEMETER_C */
	
#endif /* ndef TELEMETER_H */
