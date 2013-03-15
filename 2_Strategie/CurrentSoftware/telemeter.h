/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp, CheckNorris
 *
 *	Fichier : telemeter.h
 *	Package : Carte Principale
 *	Description : Caract�ristiques des t�l�m�tres
 *	Auteur : A.GIRARD, Jacen (Modifi� par Ronan)
 *	Version 20110428
 */

#include "QS/QS_all.h"
 
#ifndef TELEMETER_H
	#define TELEMETER_H

	/* Initialisation des t�l�m�tres */
	void TELEMETER_init(void);

	/* Mise � jour de la position du robot adverse gr�ce aux t�l�m�tres */
	void TELEMETER_update();
	
	#ifdef TELEMETER_C

		#include "QS/QS_timer.h"
		#include "QS/QS_adc.h"
		#include "maths_home.h"

		/*taille du buffer de mesures des telemeters*/
		#define TELEMETER_BUFFER_SIZE	8

		/*nombre de valeurs d�finies dans la matrice TELEMETER_conversion*/
		#define TELEMETER_VALUE_CONVERTED 22
	
		/*seuil de temps apr�s lequel on consid�re que les t�l�m�tres prennent 
		le pouvoir pour mettre a jour la position de l'adversaire (non utilisable en 2011)*/
		#define DETECTION_RESPONSE_TIME 300
		
		#define BORDER_DELTA 100 /* 150 mm */
		
		/*seuil de filtrage des dt10 - en mm! */
		#define TELEMETER_THRESHOLD 450
	
		/*localisation des capteurs*/
		#define TELEMETER_NUMBER	0
		/*attention, code li� � detection_sensor_e dans detection.h */

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
			Uint8	pinADC;							/*num�ro de port*/
			telemeter_type_e	type;				/*type cf. telemeter_type*/
			Sint16 	x;								/*abscisse du capteur relative au centre du robot*/
		   	Sint16 	y;								/*ordonn�e du capteur relative au centre du robot*/
			Sint16 	measures[TELEMETER_BUFFER_SIZE];/*nombre de mesures effectu�es par le t�l�m�tre*/
			Uint8	measures_index;					/*prochaine position a ecrire dans le buffer circulaire */
			Sint16	angle;							/*angle relatif du capteur avec le robot*/
		}telemeter_device_t;
		
		/* indication des t�l�metres */
		static telemeter_device_t telemeter[TELEMETER_NUMBER];

		/* mise � jour des buffers circulaire */
		static void TELEMETER_update_value();

		/*	Compl�te les infos du telemetre pass� en argument pour les inscrire dans global.env.sensor
		 *	Elimine les valeurs incoh�rentes
		 */
		static void TELEMETER_toFOE(Uint8 telemetre_id);

		/* fonction de conversion tension->distance*/
		static Uint16 TELEMETER_converter(Sint16 tension, telemeter_type_e type);

		static void TELEMETER_init_Measure(void);
		
		/* renvoie la valeur �chantillonn�e sur un t�l�metre (apr�s traitement de signal */
		static Uint16 TELEMETER_get_value(Uint8 telemeter_id);
		
		#ifdef DEBUG_TELEMETER
			#define telemeter_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define telemeter_printf(...)	(void)0
		#endif

	#endif /* def TELEMETER_C */
	
#endif /* ndef TELEMETER_H */
